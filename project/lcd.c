#include "myheadfile.h"

// 定义外部文件变量
extern int lcd_fd; // 打开 lcd 文件返回的描述符 
extern int *ptrLcd;

/*
* 函数名：LcdInit
* 功能：初始化 lcd 显示屏
* 输入参数：无
* 说明：调用函数前需要定义全局的 整型变量 lcd_fd，和整形指针 ptrLcd
* 返回值：无
*/
void LcdInit(void)
{
    // 读写权限，打开 lcd 设备文件
    lcd_fd = open("/dev/fb0", O_RDWR);
    if(lcd_fd == -1)
    {
        perror("Open lcd failed!\n"); // 打印错误信息
        exit(-1);
    }

    // 映射设备文件到内存，相当于申请缓存
    ptrLcd = (int*)mmap(NULL, LCD_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if(ptrLcd == MAP_FAILED)
    {
        perror("Map memory error!\n");
        exit(-1);
    }
}

/*
* 函数名：CloseLcd
* 功能：关闭 lcd 显示屏读写操作
* 输入参数：无
* 说明：调用函数前需要定义全局的 整型变量 lcd_fd，和整形指针 ptrLcd
* 返回值：无
*/
void CloseLcd(void)
{
    // 解除映射
    munmap(ptrLcd, LCD_SIZE);
    
    // 关闭文件
    close(lcd_fd);
}

/*
* 函数名：ShowBMP
* 功能：实现 LCD 屏幕任意位置显示任意大小图片
* 输入参数：图片路径 bmpPath，显示位置坐标 x, y
* 说明：使用前需要定义全局的 lcd 显存映射指针 ptrLcd
* 返回值：无
*/
int ShowBMP(char *bmpPath, int x, int y)
{
    // 只读模式打开 bmp 图片文件
    int bmp_fd = open(bmpPath, O_RDONLY);
    if(bmp_fd == -1)
    {
        perror("Open bmp img error!");
        return -1;
    }

    // 读取图片数据
    int bmpSize = lseek(bmp_fd, 0, SEEK_END); // 指针位置偏移获取 bmp 图片大小
    char buffer[bmpSize];
    memset(buffer, 0, bmpSize); // 清楚缓冲区
    lseek(bmp_fd, 54, SEEK_SET); // 指针偏移，越过 bmp 图片头信息字节
    int ret = read(bmp_fd, buffer, bmpSize); // 读取图片数据
    if(ret == -1) // 读取图片数据失败返回错误提示
    {
        perror("Read bmp image error!");
        return -1;
    }

    // 获取图片宽信息
    char bmpWidth[4];
    int nWidth = 0;
    memset(bmpWidth, 0, 4); // 清楚缓冲区
    lseek(bmp_fd, 18, SEEK_SET); // 指针偏移到，获取图片宽信息
    read(bmp_fd, bmpWidth, 4); // 读取 4 个字节
    nWidth = bmpWidth[0] | bmpWidth[1]<<8 | bmpWidth[2]<<16 | bmpWidth[3]<<24; // 转换成整形类型，获得图片的宽

    // 获取图片高信息
    char bmpHeight[4];
    int nHeight = 0;
    memset(bmpHeight, 0, 4); // 清楚缓冲区
    lseek(bmp_fd, 22, SEEK_SET); // 指针偏移到，获取图片高信息
    read(bmp_fd, bmpHeight, 4); // 读取 4 个字节
    nHeight = bmpHeight[0] | bmpHeight[1]<<8 | bmpHeight[2]<<16 | bmpHeight[3]<<24; // 转换成整形类型，获得图片的宽

    // 输出图像宽高
    printf("this picture w and h:%d\t%d\n", nWidth, nHeight); 

    // 显示图像
    for (int i = 0; i < nHeight; i++)
    {
       for (int j = 0; j < nWidth; j++)
       {
          *(ptrLcd+800*(x + i)+ y + j) = buffer[((nHeight-1-i)*nWidth+j)*3] | buffer[((nHeight-1-i)*nWidth+j)*3+1]<<8 | buffer[((nHeight-1-i)*nWidth+j)*3+2]<<16 | 0x00<<24;
       }   
    }

    // 关闭
    close(bmp_fd);

    return 0;
}







