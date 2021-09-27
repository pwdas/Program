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


/*���л���ʾ������ */
volatile int g_jpg_in_jpg_x;
volatile int g_jpg_in_jpg_y;


/****************************************************
 *��������:file_size_get
 *�������:pfile_path	-�ļ�·��
 *�� �� ֵ:-1		-ʧ��
		   ����ֵ	-�ļ���С
 *˵	��:��ȡ�ļ���С
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
//��ʼ��LCD
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

//LCD�ر�
void close_lcd(void)
{
	/* ȡ���ڴ�ӳ�� */
	munmap(g_pfb_memory, FB_SIZE);
	/* �ر�LCD�豸 */
	close(g_fb_fd);
}

//LCD����
void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color)
{
	*(g_pfb_memory+y*800+x)=color;
}
/**
	����˵����
	x,y @��ʾ��ʾͼƬ����ʼ���� 
	pjpg_path @jpgͼƬ·����
	pjpg_buf  @���ص�
	jpg_buf_size @���ص�Ķ���
*/

int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size)  
{
	/*���������󣬴��������*/
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
		/* ����jpg��Դ��Ȩ�޿ɶ���д */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* ��ȡjpg�ļ��Ĵ�С */
		jpg_size=file_size_get(pjpg_path);	

		/* Ϊjpg�ļ������ڴ�ռ� */	
		pjpg = malloc(jpg_size);

		/* ��ȡjpg�ļ��������ݵ��ڴ� */		
		read(jpg_fd,pjpg,jpg_size);
	}
	else
	{
		jpg_size = jpg_buf_size;
		
		pjpg = pjpg_buf;
	}

	/*ע�������*/
	cinfo.err = jpeg_std_error(&jerr);

	/*��������*/
	jpeg_create_decompress(&cinfo);

	/*ֱ�ӽ����ڴ�����*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*���ļ�ͷ*/
	jpeg_read_header(&cinfo, TRUE);

	/*��ʼ����*/
	jpeg_start_decompress(&cinfo);	
	x_e	= x_s+cinfo.output_width;
	y_e	= y  +cinfo.output_height;	
	/*����������*/
	while(cinfo.output_scanline < cinfo.output_height )
	{		
		pcolor_buf = g_color_buf;
		/* ��ȡjpgһ�е�rgbֵ */
		jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
		for(i=0; i<cinfo.output_width; i++)
		{
			/* ��ȡrgbֵ */
			color = 		*(pcolor_buf+2);
			color = color | *(pcolor_buf+1)<<8;
			color = color | *(pcolor_buf)<<16;
			/* ��ʾ���ص� */
			lcd_draw_point(x,y,color);	
			pcolor_buf +=3;
			x++;
		}	
		/* ���� */
		y++;			
		x = x_s;		
	}		
	/*�������*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(pjpg_path!=NULL)
	{
		/* �ر�jpg�ļ� */
		close(jpg_fd);	
		/* �ͷ�jpg�ļ��ڴ�ռ� */
		free(pjpg);		
	}
	close_lcd();
	return 0;
}
