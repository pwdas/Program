#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "curl/curl.h"
#include"cJSON.h"

#define MAX_BUF      65536

char wr_buf[MAX_BUF + 1];
int  wr_index;
//char *temp = NULL;
char *ai[3072];
//这个函数是为了符合CURLOPT_WRITEFUNCTION而构造的
//完成数据保存功能
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    int segsize = size * nmemb;
    /* Check to see if this data exceeds the size of our buffer. If so,
    * set the user-defined context value and return 0 to indicate a
    * problem to curl.
    */
    if (wr_index + segsize > MAX_BUF) {
        *(int *)userp = 1;
        return 0;
    }
 
    /* Copy the data from the curl buffer into our buffer */
    memcpy((void *)&wr_buf[wr_index], buffer, (size_t)segsize);
 
    /* Update the write index */
    wr_index += segsize;
 
    /* Null terminate the buffer */
    wr_buf[wr_index] = 0;
 
    /* Return the number of bytes received, indicating to curl that all is okay */
    return segsize;
}

void show_it(char *p ,int i)//传参：内容整段 , 第几天
{
	//创建一个矩阵装 今天明天后天
	char *day[3] = {"今天","明天","后天"};
	//将普通的字符串转换为JSON对象
	cJSON *json = cJSON_Parse(p);
	if(!json)
	{
		printf("Error:%s\n",cJSON_GetErrorPtr());
		return;
	}

	//进入到result获取天气情况
	cJSON *Item = cJSON_GetArrayItem(json, 1);
	cJSON *future = cJSON_GetArrayItem(Item, 2);
	cJSON *days = cJSON_GetArrayItem(future, i);


	//获取城市，得到result的第0个元素
	cJSON *city = cJSON_GetArrayItem(Item, 0);
	//printf("%s\n", cJSON_Print(city));

	//得到空气质量
	cJSON *realtime = cJSON_GetArrayItem(Item, 1);
	cJSON *aqi = cJSON_GetArrayItem(realtime, 6);
	//printf("%s\n", cJSON_Print(aqi));	

	cJSON *temperarure = cJSON_GetArrayItem(days,1);
	cJSON *info = cJSON_GetArrayItem(days, 2);
	cJSON *direct = cJSON_GetArrayItem(days, 4);	
	//printf("该日天气：%s\n",cJSON_Print(weather));

	cJSON *w_day = cJSON_GetArrayItem(future, i);
	printf("第%d日天气：%s\n",i,cJSON_Print(w_day));


    printf("ok\n");

	memset(ai,0,sizeof(ai));
	sprintf(ai,"%s%s温度为%s天气%s风向为%s",cJSON_Print(city), day[i], cJSON_Print(temperarure),cJSON_Print(info),cJSON_Print(direct));
	printf("语句：%s", ai);
}

void weather(char *city, char *day)
{
	printf("city: %s, day: %s\n", city, day);

	CURL *curl;
	CURLcode ret;
	
	int wr_error=0;
	wr_index = 0;
	int i;
	//char placename[100];
	//初始化curl
	curl = curl_easy_init();
	if(!curl)
	{
		printf("couldn't init curl\n");
		exit(-1);
	}

	//while(1)
	//{
	// temp = malloc(100);
	printf("请输入查询地点：");
	// scanf("%s",placename);
	char *url = NULL;
	url = malloc(200);
	memset(url,0,200);
	sprintf(url, "http://apis.juhe.cn/simpleWeather/query?city=%s&key=5b670e1fbe713dc5316bdee3017d807f",city); 

	printf("url: %s\n", url);

	//判断是第几天 今天0 明天1 后天
	if((strcmp(day,"今天"))==0)
		i = 0;
	else if((strcmp(day,"明天"))==0)
		i = 1;
	else if((strcmp(day,"后天"))==0)
		i = 2;
	else
		i = -1;

	

	//访问URL--网址 ,输入网址
	curl_easy_setopt(curl,CURLOPT_URL,url);
	
	free(url);
	url = NULL;

	// 使用回调函数并传递一个错误指针
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&wr_error);
	//获取数据
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
 
    //获取返回结果，正确为0
    ret = curl_easy_perform(curl);
	//成功则输出结果
	if(ret == CURLE_OK)
	{
		// printf("查询结果：\n");
		//printf("%s\n",wr_buf);
		show_it(wr_buf, i);
	}
	
	
	//}
	//释放curl
	curl_easy_cleanup(curl);
	// free(temp);
	// temp = NULL;
	
}
