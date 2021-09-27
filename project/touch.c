#include "myheadfile.h"


// 定义外部文件变量
extern int touchLcd_fd; // 打开 lcd 触摸屏文件返回的描述符

/*
* 函数名：TouchLcdInit
* 功能：初始化 lcd 触摸屏
* 输入参数：无
* 说明：调用函数前需要定义全局的 整型变量 touchLcd_fd
* 返回值：无
*/
void TouchLcdInit(void)
{
    touchLcd_fd = open("/dev/input/event0", O_RDONLY);
    if (touchLcd_fd == -1)
    {
        perror("Open event0 error!\n");
        exit(-1);
    }
}


/*
* 函数名：CloseTouchLcd
* 功能：关闭 lcd 触摸屏读写操作
* 输入参数：无
* 说明：调用函数前需要定义全局的 整型变量 touchLcd_fd
* 返回值：无
*/
void CloseTouchLcd(void)
{    
    // 关闭文件
    close(touchLcd_fd);
}


/*
* 函数名：GetTouchXY
* 功能：实现 LCD 触摸，获取相应触摸点的 x、y 坐标
* 输入参数：触摸点位置坐标 x, y 的指针
* 说明：使用前需要定义全局的 lcd 触摸屏打开文件描述符, 存储位置的整形的 x、y 变量
* 返回值：无
*/
void GetTouchXY(int *x, int *y)
{
    struct input_event tcLcdEvent;
    int count = 0;

    while (1)
    {
        read(touchLcd_fd, &tcLcdEvent, sizeof(tcLcdEvent)); //read自动阻塞等待数据

        if (tcLcdEvent.type == EV_ABS)
        {
            if (tcLcdEvent.code == ABS_X)
            {
                (*y) = tcLcdEvent.value;
                count++;
            }
            if (tcLcdEvent.code == ABS_Y)
            {
                (*x) = tcLcdEvent.value;
                count++;
            }
        }
        if (count == 2)
        {
            printf("(%d,%d)\n", (*x), (*y));
            break;
        }
    }
}


