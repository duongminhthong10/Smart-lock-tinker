#include <QDebug>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QtNetwork>
#include <iostream>
#include <QThread>
#include "http_request.h"

#include "my_lock.h"
#include "KHOI_TLC5971.h"
#include "config.h"

#include "rgb_lcd.h"
#include "my_lcd.h"
#include <wiringPi.h>
#include "keyboard.h"
#include "my_qr_code.h"
#include "distance_sensor.h"
#include "processthread.h"


#include <time.h>
extern int sellected_id;

int main(int argc, char *argv[]) // int argc, char *argv[]
{
    QCoreApplication app(argc, argv);
    qDebug() << "\nWelcome to smart lock!\n";

    //setup lock, keyboard, wiringPi and LCD
        my_lock_init();
        wiringPiSetup();
        my_lcd_init();
        QThread::msleep(500);

        //setup QrCode
        distance_sensor_init();
    //    // test
    //    distanceThread();
    //    //
        MyQrCode qrCode;
        qrCode.qr_start();
        QThread::msleep(500);

        getch(1);
        bat_video_qc();
        //setup http
        HttpRequest locker;
        locker.start();


//    my_lock_init();
//    wiringPiSetup();
//    while(1){
//        sellected_id = rand() % 100 + 1;
//        my_lock_unLock_and_lock();
//        delay(500);
//    }
    return app.exec();
}
