#include "myheadfile.h"

#define TOTAL_SONG 3
#define MAX_MUSIC TOTAL_SONG-1

void MusicPlayer(void)
{
    extern int x;
    extern int y;
    extern char handControlFlag;
    extern char udp_msg[1024];
    // 音乐播放器相关变量
    int flag = 0, on_off = 0, nMusic = 0;
    char *ptrMusicList[100] = {"music0.mp3","music1.mp3","music2.mp3"};
    char musicName[30] = "madplay music_list/", bg[3] = " &";
    char *temp = NULL; // 存放拼接完成的命令字符串

    // 音乐背景
    ShowBMP("img/music_bg.bmp", 0, 0); 
    ShowBMP("img/quit_music.bmp", 50, 50);
    ShowBMP("img/last_music.bmp", 300, 100);
    ShowBMP("img/next_music.bmp", 300, 600);
    ShowBMP("img/start_music.bmp", 300, 370);

    // 启动 madplay 进程
    system("madplay music_list/music0.mp3 &");

    while (1)
    {
        /******************************** 上一首歌曲 *******************************/ 
        if((x>300 && x<400 && y>100 && y<200) || (handControlFlag==3) || (strcmp(udp_msg, "MUSIC_PREV")==0))
        {
            if(nMusic > 0)
            {
                nMusic -= 1; 
                // 申请内存
                temp = malloc(30);
                // 音乐命令拼接
                strcpy(temp, musicName);
                strcat(temp, *(ptrMusicList+nMusic));
                strcat(temp, bg);
                printf("name:\t%s\n", temp);
                // 切换下一首
                system("killall madplay");
                system(temp);
                // 释放内存
                free(temp);
                temp = NULL;
                printf("Play last music.\n");
            }
            else
                printf("This is first song.\n");
            
            x = 0;
            y = 0;
            handControlFlag = 0;
        }
        /******************************** 下一首歌曲 *******************************/
        else if((x>300 && x<400 && y>600 && y<700) || (handControlFlag==4) || (strcmp(udp_msg, "MUSIC_NEXT")==0))
        {
            if(nMusic < MAX_MUSIC)
            {
                nMusic += 1; 
                // 申请内存
                temp = malloc(30);
                // 音乐命令拼接
                strcpy(temp, musicName);
                strcat(temp, *(ptrMusicList+nMusic));
                strcat(temp, bg);
                printf("name:\t%s\n", temp);
                // 切换下一首
                system("killall madplay");
                system(temp);
                // 释放内存
                free(temp);
                temp = NULL;
                printf("Play next music.\n");
            }
            else
                printf("This is finally song.\n");
            
            x = 0;
            y = 0;
            handControlFlag = 0;
        }
        /******************************** 开始/停止音乐 *******************************/        
        else if((x>300 && x<400 && y>370 && y<470) || (handControlFlag==1) || (handControlFlag==2) || (strcmp(udp_msg, "MUSIC_STOP_CONT")==0))
        {
            printf("bool:%d\n", on_off); 
            if(on_off == 0)
            {
                system("killall -STOP madplay");
                ShowBMP("img/stop_music.bmp", 300, 370);
                printf("Stop play music.\n");
                on_off += 1;
            }        
            else
            {
                system("killall -CONT madplay");
                ShowBMP("img/start_music.bmp", 300, 370);
                printf("Continue play music.\n");
                on_off -= 1;
            } 
            x = 0;
            y = 0;  
            handControlFlag = 0;  
        }
        /******************************** 退出音乐 *******************************/ 
         else if((x>50 && x<150 && y>50 && y<150) || (handControlFlag==9) || (strcmp(udp_msg, "BACK")==0))
        {
            system("killall madplay");
            DisplayGUI();
            x = 0;
            y = 0;
            handControlFlag = 0;
            break;
        }
    }
}




