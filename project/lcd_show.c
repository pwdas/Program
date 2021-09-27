#include <stdio.h>   	//printf scanf
#include <fcntl.h>		//open write read lseek close  	 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "jpeglib.h"

#define LCD_WIDTH  			800
#define LCD_HEIGHT 			480
#define FB_SIZE				(LCD_WIDTH * LCD_HEIGHT * 4)
#define EN_LCD_SHOW_JPG		1

static char g_color_buf[FB_SIZE]={0};
static int  g_fb_fd;
static int *g_pfb_memory;


/*画中画显示的坐标 */
volatile int g_jpg_in_jpg_x;
volatile int g_jpg_in_jpg_y;


/****************************************************
 *函数名称:file_size_get
 *输入参数:pfile_path	-文件路径
 *返 回 值:-1		-失败
		   其他值	-文件大小
 *说	明:获取文件大小
 ****************************************************/
unsigned long file_size_get(const char *pfile_path)
{
	unsigned long filesize = -1;	
	struct stat statbuff;
	
	if(stat(pfile_path, &statbuff) < 0)
	{
		return filesize;
	}
	else
	{
		filesize = statbuff.st_size;
	}
	
	return filesize;
}
//初始化LCD
int lcd_open(const char *str)
{
	g_fb_fd = open(str, O_RDWR);
	if(g_fb_fd<0)
	{
			printf("open lcd error\n");
			return -1;
	}
	g_pfb_memory  = (int *)mmap(NULL,FB_SIZE,PROT_READ|PROT_WRITE, MAP_SHARED,g_fb_fd,0);
	if (MAP_FAILED == g_pfb_memory )
	{
		printf ("mmap failed %d\n",__LINE__);
	}
	return g_fb_fd;
}

//LCD关闭
void close_lcd(void)
{
	/* 取消内存映射 */
	munmap(g_pfb_memory, FB_SIZE);
	/* 关闭LCD设备 */
	close(g_fb_fd);
}

//LCD画点
void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color)
{
	*(g_pfb_memory+y*800+x)=color;
}
/**
	参数说明：
	x,y @表示显示图片的起始坐标 
	pjpg_path @jpg图片路径名
	pjpg_buf  @像素点
	jpg_buf_size @像素点的多少
*/

int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size)  
{
	/*定义解码对象，错误处理对象*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	char 	*pcolor_buf = g_color_buf;
	char 	*pjpg;
	unsigned int 	i=0;
	unsigned int	color =0;
	unsigned int	count =0;
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
			 int	jpg_fd;
	unsigned int 	jpg_size;
	unsigned int 	jpg_width;
	unsigned int 	jpg_height;
	
	lcd_open("/dev/fb0");

	if(pjpg_path!=NULL)
	{
		/* 申请jpg资源，权限可读可写 */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* 获取jpg文件的大小 */
		jpg_size=file_size_get(pjpg_path);	

		/* 为jpg文件申请内存空间 */	
		pjpg = malloc(jpg_size);

		/* 读取jpg文件所有内容到内存 */		
		read(jpg_fd,pjpg,jpg_size);
	}
	else
	{
		jpg_size = jpg_buf_size;
		
		pjpg = pjpg_buf;
	}

	/*注册出错处理*/
	cinfo.err = jpeg_std_error(&jerr);

	/*创建解码*/
	jpeg_create_decompress(&cinfo);

	/*直接解码内存数据*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*读文件头*/
	jpeg_read_header(&cinfo, TRUE);

	/*开始解码*/
	jpeg_start_decompress(&cinfo);	
	x_e	= x_s+cinfo.output_width;
	y_e	= y  +cinfo.output_height;	
	/*读解码数据*/
	while(cinfo.output_scanline < cinfo.output_height )
	{		
		pcolor_buf = g_color_buf;
		/* 读取jpg一行的rgb值 */
		jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
		for(i=0; i<cinfo.output_width; i++)
		{
			/* 获取rgb值 */
			color = 		*(pcolor_buf+2);
			color = color | *(pcolor_buf+1)<<8;
			color = color | *(pcolor_buf)<<16;
			/* 显示像素点 */
			lcd_draw_point(x,y,color);	
			pcolor_buf +=3;
			x++;
		}	
		/* 换行 */
		y++;			
		x = x_s;		
	}		
	/*解码完成*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(pjpg_path!=NULL)
	{
		/* 关闭jpg文件 */
		close(jpg_fd);	
		/* 释放jpg文件内存空间 */
		free(pjpg);		
	}
	close_lcd();
	return 0;
}
