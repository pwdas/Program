#include "myheadfile.h"


void game(void)
{
    // 初始化触摸屏
    extern int x, y;
    extern int touchLcd_fd;
    struct input_event tcLcdEvent;   

    int gameFlag = 0;

    while(1)
    {
        // 初始化游戏地图
        int game1[10][10] = {
            {0,0,0,3,3,3,3,0,0,0},
            {0,3,3,3,0,0,3,3,0,0},
            {0,3,0,2,0,4,4,3,0,0},
            {0,3,0,3,0,4,4,3,0,0},
            {0,3,0,3,2,3,3,3,0,0},
            {0,3,0,3,0,0,0,3,0,0},
            {0,3,0,0,0,2,0,3,0,0},
            {0,3,3,0,2,0,0,3,0,0},
            {0,0,3,0,0,3,3,3,0,0},
            {0,0,3,3,3,3,0,0,0,0}
        };

        int manX = 2, manY = 4;
        int curFlag = 0, nextFlag = 0, next2Flag = 0;

        // GUI 初始化
        ShowBMP("img/game_bg.bmp", 0, 480); // 游戏背景
        ShowBMP("img/up.bmp", 240, 600); // 上下左右键
        ShowBMP("img/left.bmp", 320, 500);
        ShowBMP("img/right.bmp", 320, 700);
        ShowBMP("img/down.bmp", 400, 600);      
        ShowBMP("img/quit.bmp", 20, 650); // 重置推出键
        ShowBMP("img/reset.bmp", 100, 650);
        
        for(int i = 0; i < 10; i++)
        {
            for(int j = 0; j < 10; j++)
            {
                if(game1[i][j] == 0)
                    ShowBMP("img/cross.bmp", 48*i, 48*j);
                else if(game1[i][j] == 1)
                    ShowBMP("img/man.bmp", 48*i, 48*j);
                else if(game1[i][j] == 2)
                    ShowBMP("img/box.bmp", 48*i, 48*j);
                else if(game1[i][j] == 3)
                    ShowBMP("img/wall.bmp", 48*i, 48*j);
                else if(game1[i][j] == 4)
                    ShowBMP("img/aim.bmp", 48*i, 48*j);
            }
        }

        

        /************************************* 游戏控制部分 *********************************/
        
        // 初始化火柴人位置
        ShowBMP("img/man.bmp", 48*manX, 48*manY);

        while (1)
        {
            read(touchLcd_fd, &tcLcdEvent, sizeof(tcLcdEvent)); //read自动阻塞等待数据
            GetTouchXY(&x, &y);

        /******************************** 向上移动 *******************************/
            if(x>240 && x<320 && y>600 && y<680)
            {
                if(manX > 0)
                {
                    curFlag = game1[manX][manY]; // 记录当前位置的标志
                    nextFlag = game1[manX-1][manY];

                    if(nextFlag == 0) // 向上是通道，可以向前
                    {                
                        if(curFlag == 5) // 人当前位置和目标重合
                        {
                            game1[manX][manY] = 4;
                            ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                        }
                        else // 人当前位置是过道                  
                            ShowBMP("img/cross.bmp", 48*manX, 48*manY);
                        ShowBMP("img/man.bmp", 48*(manX-1), 48*manY);                              
                        manX = manX - 1;
                        printf("up\n");
                    }               
                    else if(nextFlag==4) // 向上是目标，可以向前
                    {
                        game1[manX-1][manY] = 5;
                        if(curFlag == 5) // 人当前位置和目标重合
                        {
                            game1[manX][manY] = 4;
                            ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                        }
                        else // 人当前位置是过道
                            ShowBMP("img/cross.bmp", 48*manX, 48*manY); 

                        ShowBMP("img/man.bmp", 48*(manX-1), 48*manY); 
                        manX = manX - 1;
                        printf("up\n");
                    }                  
                    else if(nextFlag==2) // 向前是箱子
                    {
                        next2Flag = game1[manX-2][manY];

                        if(next2Flag == 0) // 向前第二个是过道
                        {
                            game1[manX-1][manY] = 0;
                            game1[manX-2][manY] = 2;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*(manX-1), 48*manY); 
                            ShowBMP("img/box.bmp", 48*(manX-2), 48*manY); 
                            manX = manX - 1;
                            printf("up\n");
                        }
                        else if(next2Flag == 4) // 向前第二个是目标
                        {
                            game1[manX-1][manY] = 0;
                            game1[manX-2][manY] = 6;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*(manX-1), 48*manY); 
                            ShowBMP("img/vtor.bmp", 48*(manX-2), 48*manY); // 显示箱子和目标重合图像 ！
                            manX = manX - 1;
                            printf("up\n");
                        }
                    }
                    else if(nextFlag==6) // 向前是箱子和目标重合
                    {
                        next2Flag = game1[manX-2][manY];
                        if (next2Flag == 0) // 向前第二个是过道
                        {
                            game1[manX-1][manY] = 4;
                            game1[manX-2][manY] = 2;

                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*(manX-1), 48*manY); 
                            ShowBMP("img/box.bmp", 48*(manX-2), 48*manY); // 显示箱子和目标重合图像 ！
                            manX = manX - 1;
                            printf("up\n");
                        }
                        else if(next2Flag == 4) 
                        {
                            game1[manX-1][manY] = 5;
                            game1[manX-2][manY] = 6;

                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);
                                                
                            ShowBMP("img/man.bmp", 48*(manX-1), 48*manY); 
                            ShowBMP("img/vtor.bmp", 48*(manX-2), 48*manY); // 显示箱子和目标重合图像 ！
                            manX = manX - 1;
                            printf("up\n");
                        }                
                    }                     
                }
            }  

        /******************************** 向左移动 *******************************/ 
            else if(x>320 && x<400 && y>500 && y<580)
            {
                if(manY > 0)
                {
                    curFlag = game1[manX][manY]; // 记录当前位置的标志
                    nextFlag = game1[manX][manY-1];

                    if(nextFlag == 0) // 向上是通道，可以向前
                    {
                        if(curFlag == 5) // 人当前位置和目标重合
                        {
                            game1[manX][manY] = 4;
                            ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                        }
                        else // 人当前位置是过道
                            ShowBMP("img/cross.bmp", 48*manX, 48*manY); 
                        ShowBMP("img/man.bmp", 48*manX, 48*(manY-1));         
                        manY = manY - 1;
                        printf("up\n");
                    }                    
                    else if(nextFlag==4) // 向上是目标，可以向前
                    {
                        game1[manX][manY-1] = 5;
                        if(curFlag == 5) // 人当前位置和目标重合
                        {
                            game1[manX][manY] = 4;
                            ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                        }
                        else // 人当前位置是过道
                            ShowBMP("img/cross.bmp", 48*manX, 48*manY); 
                        ShowBMP("img/man.bmp", 48*manX, 48*(manY-1)); 
                        manY = manY - 1;
                        printf("up\n");
                    }                  
                    else if(nextFlag==2) // 向前是箱子
                    {
                        next2Flag = game1[manX][manY-2];

                        if(next2Flag == 0) // 向前第二个是过道
                        {
                            game1[manX][manY-1] = 0;
                            game1[manX][manY-2] = 2;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*manX, 48*(manY-1)); 
                            ShowBMP("img/box.bmp", 48*manX, 48*(manY-2)); 
                            manY = manY - 1;
                            printf("up\n");
                        }
                        else if(next2Flag == 4) // 向前第二个是目标
                        {
                            game1[manX][manY-1] = 0;
                            game1[manX][manY-2] = 6;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*manX, 48*(manY-1)); 
                            ShowBMP("img/vtor.bmp", 48*manX, 48*(manY-2)); // 显示箱子和目标重合图像 ！
                            manY = manY - 1;
                            printf("up\n");
                        }
                    }
                    else if(nextFlag==6) // 向前是箱子和目标重合
                    {
                        next2Flag = game1[manX][manY-2];
                        if (next2Flag == 0) // 向前第二个是过道
                        {
                            game1[manX][manY-1] = 4;
                            game1[manX][manY-2] = 2;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*manX, 48*(manY-1)); 
                            ShowBMP("img/box.bmp", 48*manX, 48*(manY-2)); // 显示箱子和目标重合图像 ！
                            manY = manY - 1;
                            printf("up\n");
                        }
                        else if(next2Flag == 4)
                        {
                            game1[manX][manY-1] = 5;
                            game1[manX][manY-2] = 6;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*manX, 48*(manY-1)); 
                            ShowBMP("img/vtor.bmp", 48*manX, 48*(manY-2)); // 显示箱子和目标重合图像 ！
                            manY = manY - 1;
                            printf("up\n");
                        }                
                    }
                    printf("left.\n");     
                }        
            }

        /******************************** 向右移动 *******************************/
            else if(x>320 && x<400 && y>700 && y<780)
            {
                if(manY < 10)
                {
                    curFlag = game1[manX][manY]; // 记录当前位置的标志
                    nextFlag = game1[manX][manY+1];

                    if(nextFlag == 0) // 向上是通道，可以向前
                    {
                        if(curFlag == 5) // 人当前位置和目标重合
                        {
                            game1[manX][manY] = 4;
                            ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                        }
                        else // 人当前位置是过道
                            ShowBMP("img/cross.bmp", 48*manX, 48*manY); 
                        ShowBMP("img/man.bmp", 48*manX, 48*(manY+1));         
                        manY = manY + 1;
                        printf("up\n");
                    }                    
                    else if(nextFlag==4) // 向上是目标，可以向前
                    {
                        game1[manX][manY+1] = 5;
                        if(curFlag == 5) // 人当前位置和目标重合
                        {
                            game1[manX][manY] = 4;
                            ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                        }
                        else // 人当前位置是过道
                            ShowBMP("img/cross.bmp", 48*manX, 48*manY); 
                        ShowBMP("img/man.bmp", 48*manX, 48*(manY+1)); 
                        manY = manY + 1;
                        printf("up\n");
                    }                  
                    else if(nextFlag==2) // 向前是箱子
                    {
                        next2Flag = game1[manX][manY+2];

                        if(next2Flag == 0) // 向前第二个是过道
                        {
                            game1[manX][manY+1] = 0;
                            game1[manX][manY+2] = 2;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*manX, 48*(manY+1)); 
                            ShowBMP("img/box.bmp", 48*manX, 48*(manY+2)); 
                            manY = manY + 1;
                            printf("up\n");
                        }
                        else if(next2Flag == 4) // 向前第二个是目标
                        {
                            game1[manX][manY+1] = 0;
                            game1[manX][manY+2] = 6;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*manX, 48*(manY+1)); 
                            ShowBMP("img/vtor.bmp", 48*manX, 48*(manY+2)); // 显示箱子和目标重合图像 ！
                            manY = manY + 1;
                            printf("up\n");
                        }
                    }
                    else if(nextFlag==6) // 向前是箱子和目标重合
                    {
                        next2Flag = game1[manX][manY+2];
                        if (next2Flag == 0) // 向前第二个是过道
                        {
                            game1[manX][manY+1] = 4;
                            game1[manX][manY+2] = 2;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*manX, 48*(manY+1)); 
                            ShowBMP("img/box.bmp", 48*manX, 48*(manY+2)); // 显示箱子和目标重合图像 ！
                            manY = manY + 1;
                            printf("up\n");
                        }
                        else if(next2Flag == 4)
                        {
                            game1[manX][manY+1] = 5;
                            game1[manX][manY+2] = 6;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*manX, 48*(manY+1)); 
                            ShowBMP("img/vtor.bmp", 48*manX, 48*(manY+2)); // 显示箱子和目标重合图像 ！
                            manY = manY + 1;
                            printf("up\n");
                        }                
                    }
                    printf("right.\n"); 
                }
            }

        /******************************** 向下移动 *******************************/        
            else if(x>400 && x<480 && y>600 && y<680)
            {
                if(manX < 10)
                {

                    curFlag = game1[manX][manY]; // 记录当前位置的标志
                    nextFlag = game1[manX+1][manY];

                    if(nextFlag == 0) // 向上是通道，可以向前
                    {
                        if(curFlag == 5) // 人当前位置和目标重合
                        {
                            game1[manX][manY] = 4;
                            ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                        }
                        else // 人当前位置是过道
                            ShowBMP("img/cross.bmp", 48*manX, 48*manY); 
                        ShowBMP("img/man.bmp", 48*(manX+1), 48*manY);         
                        manX = manX +1;
                        printf("up\n");
                    }               
                    else if(nextFlag==4) // 向上是目标，可以向前
                    {
                        game1[manX+1][manY] = 5;
                        if(curFlag == 5) // 人当前位置和目标重合
                        {
                            game1[manX][manY] = 4;
                            ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                        }
                        else // 人当前位置是过道
                            ShowBMP("img/cross.bmp", 48*manX, 48*manY); 
                        ShowBMP("img/man.bmp", 48*(manX+1), 48*manY); 
                        manX = manX +1;
                        printf("up\n");
                    }                  
                    else if(nextFlag==2) // 向前是箱子
                    {
                        next2Flag = game1[manX+2][manY];

                        if(next2Flag == 0) // 向前第二个是过道
                        {
                            game1[manX+1][manY] = 0;
                            game1[manX+2][manY] = 2;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*(manX+1), 48*manY); 
                            ShowBMP("img/box.bmp", 48*(manX+2), 48*manY); 
                            manX = manX +1;
                            printf("up\n");
                        }
                        else if(next2Flag == 4) // 向前第二个是目标
                        {
                            game1[manX+1][manY] = 0;
                            game1[manX+2][manY] = 6;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*(manX+1), 48*manY); 
                            ShowBMP("img/vtor.bmp", 48*(manX+2), 48*manY); // 显示箱子和目标重合图像 ！
                            manX = manX +1;
                            printf("up\n");
                        }
                    }
                    else if(nextFlag==6) // 向前是箱子和目标重合
                    {
                        next2Flag = game1[manX+2][manY];
                        if (next2Flag == 0) // 向前第二个是过道
                        {
                            game1[manX+1][manY] = 4;
                            game1[manX+2][manY] = 2;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*(manX+1), 48*manY); 
                            ShowBMP("img/box.bmp", 48*(manX+2), 48*manY); // 显示箱子和目标重合图像 ！
                            manX = manX +1;
                            printf("up\n");
                        }
                        else if(next2Flag == 4)
                        {
                            game1[manX+1][manY] = 5;
                            game1[manX+2][manY] = 6;
                            if(curFlag == 5) // 人当前位置和目标重合
                            {
                                game1[manX][manY] = 4;
                                ShowBMP("img/aim.bmp", 48*manX, 48*manY);
                            }
                            else // 人当前位置是过道
                                ShowBMP("img/cross.bmp", 48*manX, 48*manY);                   
                            ShowBMP("img/man.bmp", 48*(manX+1), 48*manY); 
                            ShowBMP("img/vtor.bmp", 48*(manX+2), 48*manY); // 显示箱子和目标重合图像 ！
                            manX = manX +1;
                            printf("up\n");
                        }                
                    }  
                    printf("down.\n"); 
                }
            }

        /******************************** 退出游戏 *******************************/
            else if(x>20 && x<80 && y>650 && y<750)
            {
                gameFlag = 1;
                x = 0;
                y = 0;
                break;
            }

        /******************************** 重置游戏 *******************************/
            else if(x>100 && x<160 && y>650 && y<750)
            {
                printf("Reset game.\n");
                gameFlag = 2;
                x = 0;
                y = 0;
                break;
            }
        
        /******************************** 游戏胜利 *******************************/
            if((game1[2][5]==6) && (game1[2][6]==6) && (game1[3][5]==6) && (game1[3][6]==6))
                ShowBMP("img/win.bmp", 20, 400);
        }
        
        if(gameFlag == 1) // 退出游戏
        {
            printf("Quit game.\n");
            DisplayGUI(); // 主界面显示
            break;
        }       
    }
}




