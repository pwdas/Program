#include "myheadfile.h"

extern int udp_sockfd;
extern int cameraFlag;
extern struct sockaddr_in  client_addr; // 存放客户发送信息携带的 IP 地址
extern int addrlen;

void UdpServerInit(void)
{
    //创建套接字
    udp_sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if (udp_sockfd == -1)
    {
        perror("create socket failed");
        exit(-1);
    }
    //绑定本机IP
	struct sockaddr_in  server_addr;
    server_addr.sin_family = AF_INET;         /* 地址簇：IPv4 */  
    server_addr.sin_port  = htons (2234);     /* 手机APP端口号 2234 */ 
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.2");      /* 自己IP */
    memset(&(server_addr.sin_zero), 0, 8); /*补充0*/

    int ret = bind(udp_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));//服务器绑定自己IP
    if(ret == -1)
    {
        perror("bind failed");
        exit(-1);
    }

    printf("Waiting for client connect...\n");	
}

void *UdpRS(void *arg)
{
    extern char udp_msg[1024];
    
    while (1)
    {
        // 等待客户机发送信息
        memset(udp_msg, 0, sizeof(udp_msg));
        recvfrom(udp_sockfd, udp_msg, sizeof(udp_msg), 0, (struct sockaddr *)&client_addr, &addrlen);
        printf("ip is: %s udp_msg is: %s\n", inet_ntoa(client_addr.sin_addr), udp_msg);

        if(strcmp(udp_msg, "GET_VIDEO") == 0)
			cameraFlag = 1;
		else if(strcmp(udp_msg, "BACK") == 0)
			cameraFlag = 0;
    }
}

