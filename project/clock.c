#include "myheadfile.h"

extern int x;
extern int y;

//触屏输入密码函数
char GetInputCode(void)
{
    GetTouchXY(&x, &y);

    if((x>75 && x<165 && y>555 && y<620))
        return 1;
    else if((x>75 && x<165 && y>624 && y<700))
        return 2;
    else if((x>75 && x<165 && y>704 && y<784))
        return 3;
    else if((x>175 && x<250 && y>555 && y<615))
        return 4;
    else if((x>175 && x<250 && y>625 && y<700))
        return 5;
    else if((x>175 && x<250 && y>705 && y<785))
        return 6;
    else if((x>254 && x<345 && y>550 && y<615))
        return 7;
    else if((x>256 && x<345 && y>618 && y<695))
        return 8;
    else if((x>263 && x<340 && y>700 && y<786))
        return 9;
    else if((x>345 && x<425 && y>610 && y<700))
        return 0;
    else if((x>345 && x<425 && y>700 && y<786))
        return 10;
    else if((x>350 && x<410 && y>480 && y<550))
        return 11;
}


void OpenMachine(void)
{   
    ShowBMP("img/lockGUI.bmp", 0, 0);
    char OpenCode[6] = {1,2,3,4,5,6}; 
    char inputCode[6] = {0};
    char temp = -1;
    int count = 0, i = 1, j = 0; 
    
    while(1)
    {  
        count = 0, i = 1, j = 0;    
        while(1)//获得用户输入的密码
        {
            printf("请按键盘输入密码");
            temp = GetInputCode();
            printf("%d\n", temp);
            if(temp>=0 && temp<= 9 && i<7) // 10 为删除键
            {
                inputCode[i-1] = temp; 
                ShowBMP("img/code.bmp", 370, 280+30*(i-1)); // (370,280+i*30) 星星
                i++; 
            }
            else if(temp==10 && i>1)
            {
                i--;
                inputCode[i-1] = 0;
                ShowBMP("img/code_bg.bmp", 370, 280+30*(i-1)); // 白块
            }
            else if(temp==11) // 确认进入
                break;
            
            if (i>6) // 不越界
                i = 7;              
        }

        for(j = 0; j < 6; j++)//获得用户输入的密码
        {    
            if(inputCode[j] == OpenCode[j])
                count++;    
        } 

        if(count == 6)
        {
            printf("code right.\n");
            break;
        }       
        else
        {
            printf("code error!\n");
            for (i = 0; i < 6; i++)
            {
                ShowBMP("img/code_bg.bmp", 370, 280+30*i);
            }  
        }

    }
}











