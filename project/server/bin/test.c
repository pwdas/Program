 #include <stdlib.h>
 #include <stdio.h>


 int main(void)
 {
    system("./tts_offline_sample  打电话给戚哥");
    printf("语音合成完成.\n");
    system("aplay tts_sample.wav");

    return 0;
 }



