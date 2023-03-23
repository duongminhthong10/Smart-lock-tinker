#include "distance_sensor.h"
#include <QCoreApplication>
#include <wiringPi.h>
#include <QDebug>
#include "processthread.h"
#include "rgb_lcd.h"
#include "config.h"
#include "my_lcd.h"

extern int batCam;
extern int counter_qrcode;

static volatile uint64_t time_elapse_pulse;
static volatile uint64_t start_time = 0;
static volatile uint64_t elapsed_time;
static int64_t distance;

uint64_t doXungECHO(uint8_t pin){
    while (digitalRead(pin) == LOW);
    //    start_time = millis();
    start_time = micros();
    while (digitalRead(pin) == HIGH);
    //    elapsed_time = millis() - start_time;
    elapsed_time = micros() - start_time;

    return elapsed_time;
}

uint64_t doXungECHOTimeout(uint8_t pin, uint64_t time_out){
    start_time = micros();
    unsigned long end_time;
    while (digitalRead(pin) == LOW){
        end_time = micros();
        if((end_time - start_time) >= time_out)
        {
            qDebug() <<" --- OUT---";
            goto RETURN_ELAPSE_TIME;
        }
    }
    start_time = micros();
    while (digitalRead(pin) == HIGH);
    //    elapsed_time = millis() - start_time;
    elapsed_time = micros() - start_time;

RETURN_ELAPSE_TIME:
    return elapsed_time;
}

void distance_sensor_init(){
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LED_PIN_TEST, OUTPUT);
}

int64_t doKhoangCach(){
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    const unsigned long duration = doXungECHOTimeout(ECHO_PIN,2000000);
    distance = (duration/29/2);
    return  distance;
}


void *distanceHandle(void *arg){
    int64_t khoangcach;
    qDebug() << "====Do khoang cach =====";
    while (1) {
        khoangcach = doKhoangCach();
        if((khoangcach < DISTANCE_MAX) && ( khoangcach > DISTANCE_MIN)){
            delay(500);
            khoangcach = doKhoangCach();
            if((khoangcach < DISTANCE_MAX) && (khoangcach > DISTANCE_MIN)){
                batCam = 1;
                counter_qrcode = COUNTER_QR_CODE;
                qDebug() << "bat cam";
                luongDem();
                my_lcd_clear_line(0);
                setCursor(0,0);
                lcd_write_string("DANG QUET QR");
                QThread::msleep(5000);
                my_lcd_clear_line(0);
                setCursor(0,0);
                lcd_write_string("Enter key:");
            }
            qDebug() << "khoang cach: " << khoangcach;
        }
    }
}

void distanceThread()
{
    pthread_t distanceID;
    pthread_create(&distanceID, nullptr, &distanceHandle, nullptr);

}
