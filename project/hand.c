#include "myheadfile.h"

// printf("%x\n",tmp);
// switch(tmp){			
// case 1:printf("test-Up\n");break;
// case 2:printf("test-Down\n");break;
// case 3:printf("test-Left\n");break;
// case 4:printf("test-Right\n");break;
// case 5:printf("test-Forward\n");break;
// case 6:printf("test-Backward\n");break;
// case 7:printf("test-Clockwise\n");break;
// case 8:printf("test-AntiClockwise\n");break;
// case 9:printf("test-Wave\n");break;
// default:break;
// }

extern char handControlFlag;

void HandRecInit(void)
{
	int fd;
	fd = open("/dev/IIC_drv", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		exit(-1);	
	}
	
	while(1)
	{
		read(fd,&handControlFlag,1);
		if(handControlFlag>=1 && handControlFlag<=9)
			printf("Hand flag: %d\n", handControlFlag);			
	}

	close(fd);
}



