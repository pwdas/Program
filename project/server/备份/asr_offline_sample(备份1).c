#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "../../include/qisr.h"
#include "../../include/msp_cmn.h"
#include "../../include/msp_errors.h"

#define SAMPLE_RATE_16K (16000)
#define SAMPLE_RATE_8K (8000)
#define MAX_GRAMMARID_LEN (32)
#define MAX_PARAMS_LEN (1024)

const char *ASR_RES_PATH = "fo|res/asr/common.jet"; //离线语法识别资源路径
const char *GRM_BUILD_PATH = "res/asr/GrmBuilld";	//构建离线语法识别网络生成数据保存路径
const char *GRM_FILE = "call.bnf";					//构建离线识别语法网络所用的语法文件
const char *LEX_NAME = "contact";					//更新离线识别语法的contact槽（语法文件为此示例中使用的call.bnf）

typedef struct _UserData
{
	int build_fini;						//标识语法构建是否完成
	int update_fini;					//标识更新词典是否完成
	int errcode;						//记录语法构建或更新词典回调错误码
	char grammar_id[MAX_GRAMMARID_LEN]; //保存语法构建返回的语法ID
} UserData;

const char *get_audio_file(void);	 //选择进行离线语法识别的语音文件
int build_grammar(UserData *udata);	 //构建离线识别语法网络
int update_lexicon(UserData *udata); //更新离线识别语法词典
int run_asr(UserData *udata);		 //进行离线语法识别
void ServerInit(void);

/*************************************** 服务器初始化 *********************************************/
int sockfd = 0;
int connetfd = 0;

void ServerInit(void)
{
	//创建套接字
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("create socket failed!\n");
		exit(-1);
	}

	// 跳过等待时间
	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	//绑定本机IP
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;						  /* 地址簇：IPv4 */
	server_addr.sin_port = htons(9522);						  /* 端口号 */
	server_addr.sin_addr.s_addr = inet_addr("192.168.16.42"); /* 本机IP */
	memset(&(server_addr.sin_zero), 0, 8);					  /*补充0*/

	int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret == -1)
	{
		perror("bind failed!\n");
		exit(-1);
	}

	//监听
	listen(sockfd, 5);
	printf("等待客服端连接.....\n");
	//接受链接请求并建立链接
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	connetfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
	if (connetfd == -1)
	{
		perror("connet failed");
		exit(-1);
	}
	printf("客户端 IP:%s 端口:%d 上线！\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
}

/*************************************** 获取音频文件 *********************************************/
const char *get_audio_file(void)
{
	// return "wav/ddhg.pcm";
	// 录音
	// char buf[2];
	// printf("输入任意数开启识别：");
	// scanf("%s",buf);
	// system("arecord  -d3 -c1 -r16000 -fS16_LE -traw cmd.pcm");
	// //获取客户端音频
	// return "cmd.pcm";

	// 服务器接受数据
	int msg_len = 0;
	printf("Waiting for client's data....\n");
	recv(connetfd, &msg_len, sizeof(msg_len), 0); // 接受文件大小

	printf("msg_len:%d\n", msg_len);

	char msg_buf[msg_len];
	int total_rec = msg_len, ret = 0, nByte = 0;
	char *ptr_buf = msg_buf;

	memset(msg_buf, 0, msg_len);

	while (total_rec > 0)
	{
		ret = recv(connetfd, ptr_buf, msg_len, 0); // 接受识别结果
		if(ret == -1)
		{
			perror("Recieve data error!\n");
			exit(-1);
		}
		nByte += ret;
		if(ret == 0)
		{
			printf("Recieve data finished.\n");
			break;
		}
		ptr_buf += ret;
		total_rec -= ret;
	}

	printf("Recieve %d Bytes.\n", nByte);
	int pcm_fd = open("cmd.pcm", O_RDWR | O_CREAT | O_TRUNC, 0644);
	if(pcm_fd == -1)
	{
		perror("Open pcm file error!\n");
		exit(-1);
	}
	write(pcm_fd, msg_buf, msg_len);
	close(pcm_fd);

	return "cmd.pcm";
}

/*************************************** 构建语法库 *********************************************/
int build_grm_cb(int ecode, const char *info, void *udata)
{
	UserData *grm_data = (UserData *)udata;

	if (NULL != grm_data)
	{
		grm_data->build_fini = 1;
		grm_data->errcode = ecode;
	}

	if (MSP_SUCCESS == ecode && NULL != info)
	{
		printf("构建语法成功！ 语法ID:%s\n", info);
		if (NULL != grm_data)
			snprintf(grm_data->grammar_id, MAX_GRAMMARID_LEN - 1, info);
	}
	else
		printf("构建语法失败！%d\n", ecode);

	return 0;
}

int build_grammar(UserData *udata)
{
	FILE *grm_file = NULL;
	char *grm_content = NULL;
	unsigned int grm_cnt_len = 0;
	char grm_build_params[MAX_PARAMS_LEN] = {NULL};
	int ret = 0;

	grm_file = fopen(GRM_FILE, "rb");
	if (NULL == grm_file)
	{
		printf("打开\"%s\"文件失败！[%s]\n", GRM_FILE, strerror(errno));
		return -1;
	}

	fseek(grm_file, 0, SEEK_END);
	grm_cnt_len = ftell(grm_file);
	fseek(grm_file, 0, SEEK_SET);

	grm_content = (char *)malloc(grm_cnt_len + 1);
	if (NULL == grm_content)
	{
		printf("内存分配失败!\n");
		fclose(grm_file);
		grm_file = NULL;
		return -1;
	}
	fread((void *)grm_content, 1, grm_cnt_len, grm_file);
	grm_content[grm_cnt_len] = '\0';
	fclose(grm_file);
	grm_file = NULL;

	snprintf(grm_build_params, MAX_PARAMS_LEN - 1,
			 "engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, ",
			 ASR_RES_PATH,
			 SAMPLE_RATE_16K,
			 GRM_BUILD_PATH);
	ret = QISRBuildGrammar("bnf", grm_content, grm_cnt_len, grm_build_params, build_grm_cb, udata);

	free(grm_content);
	grm_content = NULL;

	return ret;
}

/**************************************** 识别函数 **************************************/
int run_asr(UserData *udata)
{
	char asr_params[MAX_PARAMS_LEN] = {NULL};
	const char *rec_rslt = NULL;
	const char *session_id = NULL;
	const char *asr_audiof = NULL;
	FILE *f_pcm = NULL;
	char *pcm_data = NULL;
	long pcm_count = 0;
	long pcm_size = 0;
	int last_audio = 0;
	int aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	int ep_status = MSP_EP_LOOKING_FOR_SPEECH;
	int rec_status = MSP_REC_STATUS_INCOMPLETE;
	int rss_status = MSP_REC_STATUS_INCOMPLETE;
	int errcode = -1;

	/******************************* 读取语音文件 ****************************/
	asr_audiof = get_audio_file();
	f_pcm = fopen(asr_audiof, "rb");
	if (NULL == f_pcm)
	{
		printf("打开\"%s\"失败！[%s]\n", f_pcm, strerror(errno));
		goto run_error;
	}
	fseek(f_pcm, 0, SEEK_END);
	pcm_size = ftell(f_pcm);
	fseek(f_pcm, 0, SEEK_SET);
	pcm_data = (char *)malloc(pcm_size);
	if (NULL == pcm_data)
		goto run_error;
	fread((void *)pcm_data, pcm_size, 1, f_pcm);
	fclose(f_pcm);
	f_pcm = NULL;





	//离线语法识别参数设置
	snprintf(asr_params, MAX_PARAMS_LEN - 1,
			 "engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, local_grammar = %s, \
		result_type = xml, result_encoding = UTF-8, ",
			 ASR_RES_PATH,
			 SAMPLE_RATE_16K,
			 GRM_BUILD_PATH,
			 udata->grammar_id);
	session_id = QISRSessionBegin(NULL, asr_params, &errcode);
	if (NULL == session_id)
		goto run_error;
	printf("开始识别...\n\n");

	while (1)
	{
		unsigned int len = 6400;

		if (pcm_size < 12800)
		{
			len = pcm_size;
			last_audio = 1;
		}

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;

		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		if (len <= 0)
			break;

		printf(">");
		fflush(stdout);
		errcode = QISRAudioWrite(session_id, (const void *)&pcm_data[pcm_count], len, aud_stat, &ep_status, &rec_status);
		if (MSP_SUCCESS != errcode)
			goto run_error;

		pcm_count += (long)len;
		pcm_size -= (long)len;

		//检测到音频结束
		if (MSP_EP_AFTER_SPEECH == ep_status)
			break;

		// usleep(150 * 1000); //模拟人说话时间间隙
	}
	//主动点击音频结束
	QISRAudioWrite(session_id, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_status, &rec_status);

	free(pcm_data);
	pcm_data = NULL;

	//获取识别结果
	while (MSP_REC_STATUS_COMPLETE != rss_status && MSP_SUCCESS == errcode)
	{
		rec_rslt = QISRGetResult(session_id, &rss_status, 0, &errcode);
		// usleep(150 * 1000);
	}
	printf("\n\n识别结束：\n");

	printf("=============================================================\n");

	/************************************ 语音识别完成，获取语义 id 进行语音合成 ******************************************/


	int rec_len = 2, ret = 0, wav_len = 0, wav_fd = 0;

	if (NULL != rec_rslt)
	{
		printf("%s\n", rec_rslt);
		// 查找字符
		char *p = strstr(rec_rslt, "id=");
		// 字符切割
		char *q = strtok(p, "\"");
		char *id = strtok(NULL, "\"");

		// rec_len = strlen(id);
		// send(connetfd, &rec_len, sizeof(rec_len), 0);
		// send(connetfd, id, rec_len, 0);

		int flag = atoi(id); // 字符串转整形
		printf("id:%s\n", id);
		printf("flag:%d\n", flag);

		char *func[200] = {"你好", "我叫啦啦，有什么可以帮到您呢", "游戏打开好了"};
		char buf[2048] = "0";
		sprintf(buf,"./tts_offline_sample  %s", func[atoi(id)]);
		system(buf);


		/*********************** 发送合成语音给客户端 *************************/
		//打开音频文件
		wav_fd = open("tts_sample.wav", O_RDONLY);
		if(wav_fd == -1)
		{
			perror("Open PCM failed!\n");
			exit(-1);
		}

		//读取数据
		wav_len = lseek(wav_fd, 0, SEEK_END); // 获取文件大小
		printf("PCM data is %d Bytes.\n", wav_len);

		char wav_buf[wav_len];
		send(connetfd, &wav_len, sizeof(wav_len), 0);

		memset(wav_buf, 0, wav_len);
		lseek(wav_fd, 0, SEEK_SET); 
		read(wav_fd, wav_buf, wav_len);
		close(wav_fd);

		//发送给服务器
		ret = send(connetfd, wav_buf, wav_len, 0); // 发送数据
		printf("发送 %d Byte完成\n",ret);

	}
	else
	{
		// send(connetfd, &rec_len, sizeof(rec_len), 0);
		// send(connetfd, "-1", rec_len, 0);
		// printf("没有识别结果！\n");

		system("./tts_offline_sample  我的功能还没完善，没有这个功能");

		//打开音频文件
		wav_fd = open("tts_sample.wav", O_RDONLY);
		if(wav_fd == -1)
		{
			perror("Open PCM failed!\n");
			exit(-1);
		}

		//读取数据
		wav_len = lseek(wav_fd, 0, SEEK_END); // 获取文件大小

		printf("PCM data is %d Bytes.\n", wav_len);
		char wav_buf[wav_len];
		send(connetfd, &wav_len, sizeof(wav_len), 0);

		memset(wav_buf, 0, wav_len);
		lseek(wav_fd, 0, SEEK_SET); 
		read(wav_fd, wav_buf, wav_len);
		close(wav_fd);

		//发送给服务器
		ret = send(connetfd, wav_buf, wav_len, 0); // 发送数据
		printf("发送 %d Byte完成\n",ret);
		printf("没有识别结果！\n");
	}




	printf("=============================================================\n");

	goto run_exit;

run_error:
	if (NULL != pcm_data)
	{
		free(pcm_data);
		pcm_data = NULL;
	}
	if (NULL != f_pcm)
	{
		fclose(f_pcm);
		f_pcm = NULL;
	}
run_exit:
	QISRSessionEnd(session_id, NULL);
	return errcode;
}

/************************************************ 主控函数 ************************************************/
int main(int argc, char *argv[])
{
	const char *login_config = "appid = 1563184f"; //登录参数
	UserData asr_data;
	int ret = 0;
	char c;

	ret = MSPLogin(NULL, NULL, login_config); //第一个参数为用户名，第二个参数为密码，传NULL即可，第三个参数是登录参数
	if (MSP_SUCCESS != ret)
	{
		printf("登录失败：%d\n", ret);
		goto exit;
	}

	memset(&asr_data, 0, sizeof(UserData));
	printf("构建离线识别语法网络...\n");
	ret = build_grammar(&asr_data); //第一次使用某语法进行识别，需要先构建语法网络，获取语法ID，之后使用此语法进行识别，无需再次构建
	if (MSP_SUCCESS != ret)
	{
		printf("构建语法调用失败！\n");
		goto exit;
	}
	while (1 != asr_data.build_fini)
		usleep(300 * 1000);
	if (MSP_SUCCESS != asr_data.errcode)
		goto exit;
	printf("离线识别语法网络构建完成，开始识别...\n");

	/************************************ 开始离线识别 ***********************************/
	ServerInit();

	while (1)
	{
		ret = run_asr(&asr_data);
		if (MSP_SUCCESS != ret)
		{
			printf("离线语法识别出错: %d \n", ret);
			goto exit;
		}
	}

exit:
	MSPLogout();
	return 0;
}
