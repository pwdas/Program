#include "myheadfile.h"

extern int fifo_fd;

void fifo_init(void)
{
    //创建管道
    if(access("/video_fifo", F_OK))
    {
        mkfifo("/video_fifo", 0666);
        printf("你好\n");
    }
        
    //后台播放视频
    system("mplayer -slave -quiet -input file=/video_fifo -geometry 170:60 -zoom -x 450 -y 265 kjwcbz.avi &");
    //打开管道发送指令
    fifo_fd = open("/video_fifo",O_WRONLY);
    if(fifo_fd == -1)
    {
        perror("open FIFO failed");
        exit(-1);
    }
    sleep(1);
    system("clear");
    //printf("\n请输入指令：0暂停/继续，1静音，2关闭静音，3前进，4后退，5加大音量， 6减小音量，7退出:");
    //scanf("%d",&i);
}


void movie(void)
{
    extern int x;
    extern int y;
    extern char udp_msg[1024];
    int on_off = 0, quite_flag = 0;
    char *buf[8]={"pause\n", "mute 1\n", "mute 0\n","seek +10\n", "seek -10\n", "volume [30]\n","volume [-100]\n" ,"quit\n"};

    // 视频播放器背景
    ShowBMP("img/video.bmp", 0, 0);

    // 启动 mplay 进程
    fifo_init();

    while (1)
    {      
        /******************************** 退出视频播放器 *******************************/
        if((x>55&& x<155 && y>0&& y<130) || (strcmp(udp_msg, "BACK")==0))
        {
            write(fifo_fd, buf[7],strlen(buf[7]));
            system("killall mplayer");
            close(fifo_fd);
            usleep(1000);
            printf("退出\n");
            DisplayGUI();
            x = 0;
            y = 0;
            break;
        }
        /****************************** 减小音量 *****************************/        
        else if(x>410 && x<455 && y>205&& y<265)
        {
            write(fifo_fd, buf[6],strlen(buf[6]));
            printf("减小音量\n");
            x = 0;
            y = 0;
        }
        /******************************** 快退 *******************************/ 
        else if(x>410&& x<455 && y>280 && y<335)
        {
            write(fifo_fd, buf[4],strlen(buf[4]));
            printf("快退\n");
            x = 0;
            y = 0;
        }
        /******************************** 停止/开始 *******************************/ 
        else if(x>405 && x<460 && y>360 && y<420)
        {
            printf("bool:%d\n", on_off); 
            if(on_off == 0)
            {
                write(fifo_fd, buf[0],strlen(buf[0]));
                
                ShowBMP("img/stop.bmp", 400, 360);
                printf("Stop play movie.\n");
                on_off += 1;
            }        
            else
            {
                write(fifo_fd, buf[0],strlen(buf[0]));

                ShowBMP("img/start.bmp", 400, 360);
                printf("Continue play movie.\n");
                on_off -= 1;
            } 
            x = 0;
            y = 0;     
        }
        /******************************** 快进 *******************************/
        else if(x>410 && x<455 && y>450 && y<500)
        {
            write(fifo_fd, buf[3],strlen(buf[3]));
            printf("快进\n");
            x = 0;
            y = 0;
        }
        /****************************** 增大音量 *****************************/        
        else if(x>410 && x<455 && y>530 && y<595)
        {
            write(fifo_fd, buf[5],strlen(buf[5]));
            printf("增大音量\n");
            x = 0;
            y = 0;
        }
    }
}


