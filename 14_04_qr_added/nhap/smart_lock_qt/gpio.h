#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>
#include <pthread.h>
#include "config.h"
#include <wiringPi.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

    extern int8_t openLock1;
    extern int8_t openLock2;
    extern bool lock1IsOpenning;
    extern bool lock2IsOpenning;

    void gpio_init();
    void gpio_lock(uint8_t ledPin);
    void gpio_unlock(uint8_t ledPin);
    void unlockLock1(int *selected_id);
    void check1QR();
}


#endif // GPIO_H
