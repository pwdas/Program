#include "myheadfile.h"


/************ 全局变量定义 ***************/
// 打开 lcd 文件返回的描述符 
int lcd_fd = 0; 
int *ptrLcd = NULL;
// 打开 lcd 触摸屏文件返回的描述符
int touchLcd_fd = 0;
int x = 0, y = 0;
// 手势识别标志
char handControlFlag = 0;
// 摄像头全局变量
int udp_sockfd = 0, cameraFlag = 0;
struct sockaddr_in  client_addr; // 存放客户发送信息携带的 IP 地址
int addrlen = sizeof(client_addr);
// 视频播放器变量
int fifo_fd = 0;
// tcp 相关变量
int sockfd = 0;
// 语音控制相关变量
int id = 0;
// 灯光控制
bool lightFlag = false;
// udp 变量
char udp_msg[1024] = {0};

/************************************ 函数定义 ************************************/
// 主页面显示函数
void DisplayGUI(void)
{
    // 界面显示
    ShowBMP("img/backgrounds.bmp", 0, 0); 
}

/************************************ 线程定义 ***************************************/
// 获取 x、y 坐标线程
void *getXY(void *arg)
{
    while (1)
        GetTouchXY(&x, &y);
}
// 手势识别线程
void *hand(void *arg)
{
    HandRecInit();
}

/*********************************** 主控程序 ***************************************/
int main(int argc, char *argv[])
{
    // 主函数变量定义
    pthread_t touLcdPid, musicPid, handPid, udpPid, moviePid;

    // 初始化设备函数
    LcdInit(); // lcd 显示屏初始化
    TouchLcdInit(); // lcd 触摸屏初始化
    UdpServerInit(); // UDP 服务器初始化
    
    // 密码输入
    OpenMachine();

    // 功能函数
    DisplayGUI(); // 主界面显示

    pthread_create(&touLcdPid, NULL, getXY, NULL); // 
    pthread_create(&handPid, NULL, hand, NULL);
    pthread_create(&udpPid, NULL, UdpRS, NULL);

    int led_fd = open("/dev/Led", O_RDWR);

    while (1)
    {
        // 进行语音识别
        if(x>240 && x<370 && y>170 && y<310)
        {
            ShowBMP("img/speak.bmp", 0, 0); 
            VoiceControl();
            ShowBMP("img/backgrounds.bmp", 0, 0); 
            x = 0;
            y = 0;
        }
        // 打开音乐播放器 //|| (handControlFlag==9)
        if((x>30 && x<200 && y>160 && y<325) || (id==3) || (strcmp(udp_msg, "ENTER_KUGOU")==0)) // 多媒体
        {
            printf("Open music.\n");
            MusicPlayer();     
            x = 0;
            y = 0;
            handControlFlag = 0;
            id = 0;
        }
        // 打开游戏
        else if((x>0 && x<125 && y>0 && y<140) || (id==2))
        {     
            printf("Open game.\n");
            game();                   
            x = 0;
            y = 0;
            id = 0;                                 
        }
        // 视频监控
        else if((x>220 && x<380 && y>555 && y<710) || (id==5))
        {
            printf("Open camera.\n");
            ShowBMP("img/background.bmp", 0, 0); // 白色背景
            ShowBMP("img/quit_camera.bmp", 380, 650);
            // 创建 udp 线程
            
            get_picture();
            DisplayGUI();
            x = 0;
            y = 0;
            id = 0;
        }
        // 视频播放器
        else if((x>15 && x<190 && y>360 && y<500) || (id==4) || (strcmp(udp_msg, "MUSIC_PLAY")==0))
        {
            printf("Open movie.\n");
            movie();
            x = 0;
            y = 0;  
            id = 0;
        }
        // 灯控制
        else if((x>230 && x<380 && y>380 && y<480))
        {
            lightFlag ^= true;
            if(lightFlag)
            {
                ioctl(led_fd, LED1, LED_ON);
                ShowBMP("img/light_on.bmp", 200, 720); // 80 92
            }    
            else
            {
                ioctl(led_fd, LED1, LED_OFF);
                ShowBMP("img/light_off.bmp", 200, 720); 
            }            
            x = 0;
            y = 0;  
            id = 0;
        }
        else if((id==6) || (strcmp(udp_msg, "LED_ON")==0))
        {
            ioctl(led_fd, LED1, LED_ON);
            ShowBMP("img/light_on.bmp", 200, 720); // 80 92
            id = 0;
        }
        else if((id==7) || (strcmp(udp_msg, "LED_OFF")==0))
        {
            ioctl(led_fd, LED1, LED_OFF);
            ShowBMP("img/light_off.bmp", 200, 720); 
            id = 0;
        }
        // 天气预报
        else if((x>50 && x<180 && y>560 && y<690) )
        {
            printf("Open movie.\n");
            ShowBMP("img/weather.bmp", 0, 0);

            VoiceControl();
            ShowBMP("img/backgrounds.bmp", 0, 0);  
            x = 0;
            y = 0;  
            id = 0;
        }
    }
    
    // 关闭设备函数
    CloseTouchLcd(); // 关闭 lcd 触摸屏
    CloseLcd(); // 关闭 lcd 显示屏
    close(udp_sockfd); // 关闭 udp 套接字
    close(led_fd);

    return 0;
}

















