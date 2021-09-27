#ifndef _MYHEADFILE_H
#define _MYHEADFILE_H

/******************** 包含头文件 *********************/
// C语言常用操作包含头文件
#include <stdio.h>
#include <math.h>
// exit 函数包含头文件
#include <stdlib.h>
// perror 函数包含头文件
#include <string.h>
// 读文件包含头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// 写文件和关闭文件包含头文件
#include <unistd.h>
// 内存映射包含头文件
#include <sys/mman.h>
// 外设输入输出包含头文件
#include <linux/input.h>
// 多线程编程包含头文件
#include <pthread.h>
// 摄像头
#include <dirent.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h> // yuyc 包含头文件
#include <getopt.h>            
#include <errno.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/ioctl.h>     
#include <linux/videodev2.h>
#include "jpeglib.h"
#include <netinet/in.h>
// bool 常量头文件
#include <stdbool.h>
// led 相关头文件
#include <sys/stat.h>





/******************** 宏定义变量 *********************/
#define LCD_SIZE 800*480*4 // lcd 显示屏内存映射宏定义
// led 相关宏定义
#define TEST_MAGIC 'x'                    //定义幻数
#define LED1 _IO(TEST_MAGIC, 0)              
#define LED2 _IO(TEST_MAGIC, 1)
#define LED3 _IO(TEST_MAGIC, 2)
#define LED4 _IO(TEST_MAGIC, 3) 
#define LED_ON  	0	//灯亮
#define LED_OFF		1   //灯灭


/******************** 结构体定义 *********************/


/******************** 函数声明 **********************/
// lcd 显示屏相关函数
void LcdInit(void);
void CloseLcd(void);
int ShowBMP(char *bmpPath, int x, int y);

// lcd 触摸屏相关函数
void TouchLcdInit(void);
void CloseTouchLcd(void);
void GetTouchXY(int *x, int *y);

// 手势识别初始化函数
void HandRecInit(void);

// 主界面显示函数
void DisplayGUI(void);

// 音乐播放器
void MusicPlayer(void);

// 游戏
void game(void);

// 视频监控
int get_xy(void);
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size);
int get_picture();

// udp 初始化函数
void UdpServerInit(void);
void *UdpRS(void *arg);

// 视频播放器函数
void fifo_init(void);
void movie(void);

// tcp 客户端相关函数
void ClientInit(void);
void VoiceControl(void);

// 开机相关函数
char GetInputCode(void);
void OpenMachine(void);

#endif