#include "keyboard.h"

#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>
#include <QUrl>
#include <QFile>
#include <iostream>
#include <termios.h> //gethch
#include <linux/input.h>
#include <pthread.h>

char key_pressed_value = 0;
bool key_pressed = false;

void *getchHandler(void * arg) {
    struct input_event ev[64];
    int size = sizeof (struct input_event);
    ssize_t n;
    int file_keyboard;
    char* filename = (char*)"/dev/input/event0";

    while (1) {
        //    qDebug()<<"\n ct."<< check_terminal <<" ct.\n";
        for (;;)
        {
            if ((file_keyboard = open(filename, O_RDONLY)) < 0)
            {
                printf("Failed to open the keyboard ");
                return nullptr;
            }
            n = read(file_keyboard, &ev, size * 64);

            close(file_keyboard);

            if ((n!=0) &&
                    (ev[0].type == 4) &&
                    (ev[1].value == 1))
            {

                key_pressed_value = ev[1].code -1 + '0';
                key_pressed = true;
                printf("\n...key pressed %c...", key_pressed_value);

                if( key_pressed_value == 48){
                    key_pressed_value = 58;
                    break;
                }
                if(key_pressed_value == 58){
                    key_pressed_value = 48;
                    break;
                }
                break;
            }
        }
    }
}

void getch(unsigned char checkterminal){
    pthread_t getchID;
    pthread_create(&getchID, nullptr, &getchHandler, nullptr);
}

















