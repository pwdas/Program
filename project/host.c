#include "myheadfile.h"

extern int sockfd;
extern int id;

void ClientInit(void)
{
    //创建套接字
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        perror("create socket failed");
        exit(-1);
    }

    // 连接服务器
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;						  /* 地址簇：IPv4 */
    server_addr.sin_port = htons(9522);						  /* 端口号 */
    server_addr.sin_addr.s_addr = inet_addr("192.168.16.42"); /* 本机IP */
    memset(&(server_addr.sin_zero), 0, 8);	

    int status = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (status == -1)
    {
        perror("connect server failed");
        exit(-1);
    }
    printf("Connect server successfully.\n");
}


void VoiceControl(void)
{
    // 标志位
    long  ret = 0;
    char buf[2];
    // 客户机相关变量
    int pcm_fd = 0;
    long pcm_len = 0;
    char *pcm_buf = NULL;
    // 接受服务器结果变量
    int id_size = 0;
    char *id_buf = NULL;
    char *msg_buf = NULL;
    long total_rec = 0, nByte = 0, msg_len = 0;
    int wav_fd = 0; 
    
    // while (1)
    // {
        // 初始化客户端
        ClientInit();
        /****************************************** 采集并发送主人音频 ***************************************/
        // printf("输入任意字符开始：");
        // scanf("%s",buf);
        system("arecord -d3 -c1 -r16000 -fS16_LE -traw cmd.pcm"); // 录音

        //打开音频文件
        pcm_fd = open("cmd.pcm", O_RDONLY);
        if(pcm_fd == -1)
        {
            perror("Open PCM failed!\n");
            exit(-1);
        }

        //读取数据
        pcm_len = lseek(pcm_fd, 0, SEEK_END); // 获取文件大小     
        ret = send(sockfd, &pcm_len, sizeof(pcm_len), 0); // 发送 pcm 长度
        printf("ret: %ld, PCM data is %ld bytes.\n", ret, pcm_len);
        pcm_buf = malloc(pcm_len); // 申请内存并清空缓冲区
        memset(pcm_buf, 0, pcm_len);
        lseek(pcm_fd, 0, SEEK_SET); 
        read(pcm_fd, pcm_buf, pcm_len);
        close(pcm_fd);

        //发送给服务器
        ret = send(sockfd, pcm_buf, pcm_len, 0); // 发送数据
        printf("Send pcm data %ld bytes finished.\n\n", ret);

        free(pcm_buf); // 释放内存
        pcm_buf = NULL;

        /****************************************** 接收服务器结果 ***************************************/
        //接收服务器识别结果 id
        printf("Recieve recognize result's id...\n");
        ret = recv(sockfd, &id_size, sizeof(id_size), 0); // 接收文件大小
        printf("ret: %ld, id_size: %d\n", ret, id_size);
 
        id_buf = malloc(id_size);
        if(id_buf == NULL)
        {
            perror("malloc id_buf failed.\n");
            exit(-1);
        }
        memset(id_buf, 0, id_size); // 清空缓冲区
        ret = recv(sockfd, id_buf, id_size, 0); // 接受文件大小
        printf("ret: %ld, id is: %s\n", ret, id_buf);
        printf("Recieve id finished.\n");
        id = atoi(id_buf); // 字符串转整形
        free(id_buf);
        id_buf = NULL;

        //接收服务器识别结果合成的语音并播放
        if(id != -1)
        {
            printf("Recieve compose wav data...\n");
            ret = recv(sockfd, &msg_len, 8, 0); // 接受文件大小
            printf("Recieve wav_size %ld bytes, wav_size: %ld.\n", ret, msg_len);
            printf("Recieve wav_size finished.\n");

            msg_buf = malloc(msg_len);
            if(msg_buf == NULL)
            {
                perror("malloc msg_buf failed.\n");
                exit(-1);
            }
            memset(msg_buf, 0, msg_len);
            total_rec = msg_len, nByte = 0;
        
            while (total_rec > 0)
            {
                ret = recv(sockfd, (msg_buf+nByte), msg_len, 0); // 接受识别结果
                if(ret == -1)
                {
                    perror("Recieve wav data error!\n");
                    exit(-1);
                }
                printf("ret: %ld\n", ret);
                nByte += ret;
                total_rec -= ret;
            }
            printf("Recieve wav data %ld bytes.\n", nByte);
            printf("Recieve wav data finished.\n");                 

            // 存放合成语音到开发板本地
            wav_fd = open("aim.wav", O_RDWR | O_CREAT | O_TRUNC, 0644);
            if(wav_fd == -1)
            {
                perror("Open pcm file error!\n");
                exit(-1);
            }
            lseek(wav_fd, 0, SEEK_SET);
            write(wav_fd, msg_buf, msg_len);            
            close(wav_fd);

            free(msg_buf);
            msg_buf = NULL;
            
            // 播放合成语音
            system("aplay aim.wav");
        }
        else
        { 
            system("aplay puzzled.wav");
            // 播放我不明白
            printf("我不是很明白\n");
        }
        // 关闭套接字
        close(sockfd);
    // }
}