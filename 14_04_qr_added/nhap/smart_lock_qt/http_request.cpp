#include <stdio.h>

#include <QObject>

#include <QtNetwork>

#include <QUrl>

#include <QFile>

#include <iostream>

#include <QTime>

#include <wiringPi.h>

#include <QCoreApplication>

#include <cstdio>

#include <pthread.h>

#include <QThread>

#include <QDebug>

#include <unistd.h>				//Needed for I2C port

#include <fcntl.h>				//Needed for I2C port

#include <sys/ioctl.h>			//Needed for I2C port

#include <linux/i2c-dev.h>





#include <termios.h> //gethch

#include <linux/input.h>



#include <unistd.h>

#include <pthread.h>



#include "http_request.h"

#include "rgb_lcd.h"

#include "my_lcd.h"

#include "my_lock.h"

#include "KHOI_TLC5971.h"

#include "processthread.h"

#include "config.h"

#include "distance_sensor.h"

#include <my_qr_code.h>

#include <QElapsedTimer>

#include "keyboard.h"



extern char key_pressed_value;

extern bool key_pressed;

extern bool waittingQr;

extern QString maQr;



HttpRequest::HttpRequest() : QObject()

{

    pthd.count=0;

    pthd.counting=0;

    pthd.stop_count=0;

    reply = nullptr;

    httpRequestAborted = false;

    isInputting = false;

    mode= -1;

    resetParam();



    timer = new QTimer(this);

    //get param(location, locker_name) from file setting

    QFile file(PATH_FILE_SETTING);

    if(!file.open(QIODevice::ReadOnly))

    {

        qDebug() << "error opening file: " << file.error();

        return;

    }

    QTextStream instream(&file);    /* Tao stream vao ra file */

    QString line = instream.readLine(); /* tra ve 1 dong van ban */

    file.close();

    QByteArray fileContent("");

    fileContent.append(line);   /* Them vao sau file dong vua moi doc duoc */



    QJsonDocument response = QJsonDocument::fromJson(fileContent);  /*  */

    location = response.object().value("location").toString();

    locker_name = response.object().value("locker_name").toString();

    qDebug()<<"ReadLocation: " << location;

    qDebug()<<"ReadLockerName: " << locker_name;

    writeLog("Start smartlock!!!");

    connect(this, SIGNAL(callLock()), this, SLOT(lock()));

}



void HttpRequest::start()

{

    startNewSession();

}



void HttpRequest::resetParam()

{

    isRequestPrevSuccess = true;

    res_isValid = 0;

    res_link = "";

    res_receiver_code = "";

    res_receiver_phone = "";

    res_drawer_position = "";

}



void HttpRequest::writeLog(QString mess)

{

    QString fileName = QDate::currentDate().toString("yyyy_MM_dd");

    QString fileUrl = File_Log + fileName + ".txt";



    QFile file(fileUrl);

    if (!file.open(QIODevice::Append | QIODevice::Text))

        /*mode Append: Data is writen at the end of file

         *mode Text: read: eol is '\n', but write: eol is '\r\n' */

        return;



    QTextStream in(&file);

    if (in.atEnd())

        in << QTime::currentTime().toString("hh:mm:ss")<<" "<< mess << "\n";

}

void HttpRequest::startRequest()

{

    my_lcd_clear_line(0);

    my_lcd_clear_line(1);

    setCursor(0,0);

    lcd_write_string("waiting....");

    writeLog("waiting....");

    httpRequestAborted = false;

    timer->setSingleShot(true);

    timer->setInterval(10000);

    connect(timer,&QTimer::timeout,this,&HttpRequest::TimeOut);

    timer->start();

    qDebug() << url;



    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));



    reply = qnam.get(request);

    connect(reply, &QNetworkReply::finished, this, &HttpRequest::httpFinished);

    connect(reply, &QIODevice::readyRead, this, &HttpRequest::httpReadyRead);

}



void HttpRequest::senderUnlock()

{

    statusRequest = SENDER_UNLOCK;

    writeLog("Sender unlock");

    url = QString("http://api.adela.com.vn/api/smart_locker/getValidCode")

            + QString("?location=") + location

            + QString("&code=") + code

            + QString("&locker_name=") + locker_name;



    startRequest();

}



void HttpRequest::receiverUnlock()

{

    statusRequest = RECEIVER_UNLOCK;



    writeLog("API receiver Unlock");

    url = QString("http://api.adela.com.vn/api/smart_locker/unlockLocker")

            + QString("?location=") + location

            + QString("&code=") + code

            + QString("&locker_name=") + locker_name;



    startRequest();

}



void HttpRequest::lock()

{

    if (mode == 0) {

        // sender

        statusRequest = SENDER_LOCK;

    } else {

        // receiver

        statusRequest = RECEIVER_LOCK;



    }

    writeLog("API lock");

    QString recent_shiper_code = "0";

    if (statusRequest == SENDER_LOCK) {

        recent_shiper_code = code;

    }



    url = QString("http://api.adela.com.vn/api/smart_locker/lockLocker")

            + QString("?location=") + location

            + QString("&locker_name=") + locker_name

            + QString("&drawer_position=") + drawer_position

            + QString("&code_shiper=") + recent_shiper_code;



    recent_shiper_code = "0";



    startRequest();

}



void HttpRequest::httpFinished()

{

    std::cout<< "finished\n";

    if (httpRequestAborted) {

        reply->deleteLater();

        reply = nullptr;

        writeLog("Http Aborted");

        startNewSession();

    }



    if (reply->error()) {

        qDebug()<<reply->errorString();

        reply->deleteLater();

        reply = nullptr;

        writeLog("No Internet");

        my_lcd_set_cursor(0,1);

        lcd_write_string("No internet");

        QThread::msleep(2000);

        startNewSession();

    }



    reply->deleteLater();

    reply = nullptr;



    if ((statusRequest == SENDER_UNLOCK) && isRequestPrevSuccess) {

        //sender unlock --> statusRequest = SENDER_UNLOCK

        resetParam();   //đưa isRequestPre = true

        //sleep and call lock()

        QTimer::singleShot(1000, this, SLOT(onWaitLock()));

        //       lock();

    } else if ((statusRequest == SENDER_LOCK) && isRequestPrevSuccess) {

        // sendSMS

        resetParam();

        startNewSession();

    } else if (statusRequest == SEND_SMS) {

        resetParam();

        startNewSession();

    } else if ((statusRequest == RECEIVER_UNLOCK) && isRequestPrevSuccess) {

        resetParam();

        //sleep and call lock()

        QTimer::singleShot(1000, this, SLOT(onWaitLock()));

        //        lock();

    } else if (statusRequest == RECEIVER_LOCK) {

        resetParam();

        startNewSession();

    } else {

        resetParam();

        startNewSession();

    }

}



void HttpRequest::onWaitLock()

{

    qDebug()<< "Waiting lock position: "<< drawer_position;

    writeLog("Waiting Lock position: " + drawer_position);



    QElapsedTimer myTimer;

    myTimer.start();



    // write to LCD

    my_lcd_clear_line(0);

    my_lcd_clear_line(1);

    setCursor(0, 0);

    //    lcd_write_string("Enter to lock");



    //    // start two thread

    //    //    static pthread_t t_cd_id;

    //    static pthread_t t_pe_id;

    //    khoi_message_queue_in_angry_mode = 0;

    //    pthread_create(&t_cd_id, NULL, count_down_to_lock, (void *)&t_pe_id);

    //    pthread_create(&t_pe_id, NULL, press_enter_to_lock, (void *)&khoi_message_queue_in_angry_mode);



    //    int cd_tl = 2;

    //    while(cd_tl > 0){

    //        cd_tl--;

    //        if(khoi_message_queue_in_angry_mode == 1){

    //            break;

    //        }

    //        QThread::msleep(200);

    //    }

    QThread::msleep(200);

    emit callLock();

}



void HttpRequest::httpReadyRead()

{

    /* get attribute and header of reply */

    std::cout<< "readedyRead\n";

    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    QVariant contentLengthV = reply->header(QNetworkRequest::ContentLengthHeader);



    /* reason = attribute of reply */

    QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();

    qDebug() <<"reason "<< reason;



    qDebug() <<"statuscode: "<< statusCodeV.toString();

    qDebug() <<"contentLength: "<< contentLengthV.toString();



    /* Luu het vao 1 mang */

    QByteArray rep = reply->readAll();

    qDebug()<<"rep   "<<rep;



    /* Ma hoa mang rep va tao 1 QJsonDocument tu do*/

    QJsonDocument response = QJsonDocument::fromJson(rep);

    /*toJson: chuyen QJsonDocumet thanh ma utf8  o dinh dang duoc cung cap: Compact*/

    /* Compact: {"Array":[true,999,"string"],"Key":"Value","null":null}*/

    QString strFromObj = QJsonDocument(response).toJson(QJsonDocument::Compact).toStdString().c_str();

    qDebug()<<"jsonn: " << strFromObj;

    timer->stop();

    if (statusCodeV.toString() == "200") {  /* 200: OK */

        if (statusRequest == SENDER_UNLOCK) {

            /* lay value tra ve tu server */

            res_isValid = response.object().value("isValid").toInt();   /* tra ve value cua isValid thoi */

            qDebug() <<"res_isvalid"<< res_isValid;

            bool a = res_isValid != 0;

            qDebug() <<"is_invalid: "<< a;  /* a: check invalid */

            if (res_isValid != 0) { /* Co gia tri tra ve */

                // unlock for sender

                res_link = response.object().value("linkt").toString(); /* lay value cua key */

                res_receiver_code = response.object().value("receiver_code").toString();

                res_receiver_phone = response.object().value("receiver_phone").toString();

                res_drawer_position = response.object().value("drawer_position").toString();



                drawer_position = res_drawer_position;

                link = res_link;

                receiver_code = "1" + res_receiver_code;

                receiver_phone = res_receiver_phone;

                int position = res_drawer_position.midRef(locker_name.length(), res_drawer_position.length() - 1).toInt();

                code_drawer_position[position - 1] = res_receiver_code;

                shiper_code[position - 1] = code;



                qDebug() <<"receiver_code: "<< receiver_code;

                qDebug() <<"drawer_position: "<<res_drawer_position;

                qDebug() <<"drawer_position_toint: "<<res_drawer_position.midRef(locker_name.length(), res_drawer_position.length() - 1).toInt() - 1;



                writeLog("receiver_code: " + receiver_code);

                writeLog("drawer_position: " + res_drawer_position);

                writeLog("Unlock position " + res_drawer_position +" success!");

                // *****************

                // write to driver

                //                sellected_id = res_drawer_position.midRef(1, res_drawer_position.length() - 1).toInt() - 1;

                //                my_lock_unlock(res_drawer_position.midRef(1, res_drawer_position.length() - 1).toInt() - 1);

                if(res_drawer_position != "-1"){

                    sellected_id = res_drawer_position.midRef(locker_name.length(), res_drawer_position.length() - 1).toInt();

                    write_position(QString::number(sellected_id));

                    bat_video_mo_tu();

                    delay(1500);

                    my_lock_unLock_and_lock();

                }

                else {

                    sellected_id = -1;

                    write_position("het_tu");

                    bat_video_mo_tu();

                }

            } else {    /* code ko hop le: ko có gia trị trả về */

                isRequestPrevSuccess = false;

                bat_video_nhap_sai();

                writeLog("Code invalid");

                my_lcd_clear_line(0);

                my_lcd_clear_line(1);

                setCursor(0,0);

                lcd_write_string("Code invalid!");

                QThread::sleep(3);

            }

        } else if (statusRequest == SENDER_LOCK) {

            // lock by sender

            writeLog("Lock position " + drawer_position +" success!");

            //******************

            // write to driver

            my_lock_lock(drawer_position.midRef(locker_name.length(), drawer_position.length() - 1).toInt() - 1);

        } else if (statusRequest == SEND_SMS) {

            // check success when send sms

            QString codeResult = response.object().value("CodeResult").toString();

            qDebug()<< "codeResult: "<<codeResult;

            if (codeResult == "100") {

                qDebug() << "Sent SMS successfully.";

                writeLog("Sent SMS successfully.");

            } else {

                writeLog("Sent SMS failed");

            }

        } else if (statusRequest == RECEIVER_UNLOCK) {

            res_isValid = response.object().value("isValid").toInt();

            if (res_isValid != 0) {

                // unlock for receiver

                res_drawer_position = response.object().value("drawer_position").toString();



                drawer_position = res_drawer_position;



                int position = res_drawer_position.midRef(locker_name.length(), res_drawer_position.length() - 1).toInt();

                code_drawer_position[position - 1] = "";

                shiper_code[position - 1] = "";



                writeLog("Unlock position " + drawer_position +" success!");

                //******************

                // write to driver

                //                sellected_id = drawer_position.midRef(1, drawer_position.length() - 1).toInt() - 1;

                sellected_id = drawer_position.midRef(locker_name.length(), drawer_position.length() - 1).toInt();

                write_position(QString::number(sellected_id));

                bat_video_mo_tu();

                delay(1500);

                my_lock_unLock_and_lock();

              } else {

                isRequestPrevSuccess = false;

                writeLog("Code invalid");

                bat_video_nhap_sai();

                my_lcd_clear_line(0);

                my_lcd_clear_line(1);

                setCursor(0,0);

                lcd_write_string("Code invalid!");

                QThread::sleep(3);

            }

        } else if (statusRequest == RECEIVER_LOCK) {

            // lock by receiver

            writeLog("Lock position " + drawer_position +" success!");

            //******************

            // write to driver

            my_lock_lock(drawer_position.midRef(locker_name.length(), drawer_position.length() - 1).toInt() - 1);

        }

    } else {

        isRequestPrevSuccess = false;

        if (statusRequest == SEND_SMS) {

            qDebug() << "Sent SMS failed.";

            writeLog("Sent SMS failed.");

        } else {

            writeLog("Something wrong!");

            my_lcd_clear_line(0);

            my_lcd_clear_line(1);

            setCursor(0,0);

            lcd_write_string("Something wrong!");

            QThread::sleep(3);

        }

        startNewSession();

    }







    reply->deleteLater();

}





void *HttpRequest::overwrite_last_char(void *vargp)

{

    struct pthread_data *pd;

    pd = (struct pthread_data *)vargp;

    pd->counting = 1;

    QThread::msleep(1000);

    if(pd->stop_count){

        pd->stop_count = 0;

    }

    else {

        setCursor(pd->count - 1, 1);

        lcd_write('*');

    }



    pd->counting = 0;

    return NULL;

}





void *HttpRequest::count_down_to_lock(void *vargp)

{

    //    char c = getch(1);

    //    *((int *)vargp) = 1;



    //    pthread_cancel(*((pthread_t *)vargp));

    //    return NULL;

}





void *HttpRequest::press_enter_to_lock(void *vargp)

{



    //    char c = getch(1);

    //    *((int *)vargp) = 1;

    //    return NULL;

}



void HttpRequest::TimeOut() {

    writeLog("Timeout");

    reply->deleteLater();

    reply=nullptr;

    timer->stop();

    delete timer;

    my_lcd_clear_line(1);

    setCursor(0,1);

    lcd_write_string("Bad internet");

    QThread::sleep(2);

    timer=new QTimer(this);

    startNewSession();

}

void HttpRequest::startNewSession(){

    // write to LCD

    my_lcd_clear_line(0);

    my_lcd_clear_line(1);

    setCursor(0, 0);

    lcd_write_string("Enter key: ");



    resetParam();

    mode = -1;

    code = "";

    drawer_position = "";

    link = "";

    receiver_code = "";

    receiver_phone = "";

    isInputting = true;

    waittingQr = true;

    qDebug()<< "Enter code: ";

    while (isInputting && waittingQr){

        getCode();

    }

    //check ma qr

    if(maQr.mid(0,1) == "0" && (waittingQr == false)){

        mode = 0;

        code = maQr.mid(1,8);

        writeLog("===========================");

        writeLog("SENDER SCAN QR");

        writeLog("<code> " + code);

        senderUnlock();

    }

    else if (maQr.mid(0,1) == "1" && (waittingQr == false)) {

        mode = 1;

        code = maQr.mid(1,8);

        writeLog("===========================");

        writeLog("RECEIVER SCAN QR");

        writeLog("<code> " + code);

        receiverUnlock();

    }

    else if ((key[0] == '0') && (isInputting == false) ) {

        mode = 0;

        writeLog("SENDER USE KEYBOARD");

        writeLog("<code> " + code);

        senderUnlock();

    } else if ((key[0] == '1') && (isInputting == false)) {

        mode = 1;

        writeLog("RECEIVER USE KEYBOARD");

        writeLog("<code> " + code);

        receiverUnlock();

    }

    else if ((key[0] == '2') && (isInputting == false)){

        QString keyadmin = key;

        if(keyadmin == "2999999999"){

        }

        QThread::msleep(500);

        startNewSession();

    }

    else {

        bat_video_nhap_sai();

        writeLog("Code invalid!");

        my_lcd_clear_line(0);

        my_lcd_clear_line(1);

        setCursor(0,0);

        lcd_write_string("Code invalid!");

        QThread::sleep(1);

        startNewSession();

    }

}





void HttpRequest::getCode(){

    static pthread_t tid;

    static unsigned char admin_tool = 0;

    char c;

    if(admin_tool){

        my_lcd_set_cursor(13, 0);

        lcd_write_string("AD");

    }

    else {

        my_lcd_set_cursor(13, 0);

        lcd_write_string("  ");

    }

    //  char c = getch(0);

    while((!key_pressed) && (waittingQr == true));

    QThread::msleep(10);

    c = key_pressed_value;

    key_pressed_value  = 0;

    key_pressed = false;

    if((c>='0')&&(c<='9')){ /* neu nhap các số vào từ 0 - 9*/

        //if(pthd.count>0){

        //setCursor(pthd.count-1,1);

        //lcd_write('*');

        //}

        key[pthd.count]=c;  //pthread data. count

        setCursor(pthd.count,1);    /*set con tro den vi tri pthd.count, 1 */

        lcd_write(c);               /* viet len LCD */



        pthd.count++;

        if(pthd.count >=16) pthd.count = 0;     /* Neu viet qua 16 ki tu --> quay xe*/



        if(pthd.counting) pthd.stop_count=1;

        //pthread_create(&tid, NULL, overwrite_last_char, (void *)&pthd);

    }



    else{ /* Không phải số */

        switch(c){

        //pu

        case 0:

            break;

        case 151:

            if(pthd.count == 1){

                sellected_id = key[0] - '0';

            }

            else if(pthd.count == 2){

                sellected_id = ((key[0] - '0') * 10 + (key[1] - '0'));

            }

            else{

                //Nhap 3 so --> ko co gi

                sellected_id = 255;

            }

            my_lcd_clear_line(1);

            pthd.count = 0;

            writeLog("admin tool press Page up");   //ghi ra file

            writeLog("Key: " + QString::fromStdString(key));  // ghi ra file key nhap vao

            writeLog("Sellected ID: " + QString::number(sellected_id));

            /* Khi enter cung voi key 299999... --> toggle admin_tool */

            if(admin_tool){

                qDebug() << "admin tool press PAGE UP";

                qDebug() << "Selected id = " << sellected_id;

                //batLen();

                my_lock_unLock_and_lock();  //unlock selected id

                qDebug () << "admin tool open";

                writeLog("admin tool open");

                my_lcd_set_cursor(0,1);

                (sellected_id == 255)? lcd_write_string("Tu khong hop le ") : lcd_write_string("OPEN: ");

                my_lcd_write_number(sellected_id);

                my_lcd_clear_line(1);

            }



            break;

            //pd

        case 156:

            if(pthd.count == 1){

                sellected_id = key[0] - '0';

            }

            else if(pthd.count == 2){

                sellected_id = ((key[0] - '0') * 10 + (key[1] - '0'));

            }

            else{

                sellected_id = 255;

            }

            my_lcd_clear_line(1);

            pthd.count = 0;

            writeLog("admin tool press Page down");   //ghi ra file

            writeLog(key);

            if(admin_tool){

                qDebug() << "admin tool press PAGE DOWN";

                qDebug() << "Selected id = " << sellected_id;

                //                batLen();

                my_lock_unLock_and_lock();  //unlock selected id

                qDebug () << "admin tool open";

                writeLog("admin tool open");

                my_lcd_set_cursor(0,1);

                (sellected_id == 255)? lcd_write_string("Tu khong hop le ") : lcd_write_string("OPEN: ");

                my_lcd_write_number(sellected_id);

                my_lcd_clear_line(1);

            }

            break;

            //esc

        case 58:

            pthread_cancel(tid);

            pthd.count = 0;

            if(pthd.counting) pthd.stop_count = 1;

            my_lcd_clear_line(1);

            qDebug() << "---ESC---stop_count---"<<(pthd.stop_count);

            break;



            //clear

        case 61:

            pthread_cancel(tid);

            pthd.count = 0;

            if(pthd.counting) pthd.stop_count = 1;

            my_lcd_clear_line(1);

            break;



            // dot

        case 99:



            break;

        case 75:

            // ===============ENTER================

            pthd.count = 0;

            isInputting = false;

            QString keyStringFull;

            keyStringFull = key;

            QString keyString = keyStringFull.mid(0,9);

            qDebug() << "input: " << keyString;

            writeLog("===========================");

            writeLog("<input> " + keyString);

            code = keyString.mid(1, 8);



            for(int x = 1; x < 9;x++) key[x] = 0;



            if(pthd.counting) pthd.stop_count = 1;

            my_lcd_clear_line(1);



            if((admin_tool == 0) && (keyString == "299999999")){

                my_lcd_clear_line(0);

                my_lcd_clear_line(1);

                setCursor(0, 0);

                lcd_write_string("Admin tool:");

                admin_tool = 1;

                qDebug() << "Admin tool: " << admin_tool;

                QThread::msleep(500);

                writeLog("admin tool active");

            }

            else if((admin_tool == 1) && (keyString == "299999999")){

                my_lcd_clear_line(0);

                my_lcd_clear_line(1);

                setCursor(0, 0);

                lcd_write_string("Enter key:");

                admin_tool = 0;

                qDebug() << "Admin tool: " << admin_tool;

                QThread::msleep(500);

                writeLog("admin tool deactive");

            }

            else{

                if(admin_tool == 0) {

                    writeLog("receive normal key");

                    isInputting = false;

                }

            }

            break;

        }

    }

}



void HttpRequest::write_position(QString id)

{

    QString fileUrl = FILE_ID_VIDEO + QString::fromStdString("So_tu_duoc_mo");



    QFile file(fileUrl);

    if (!file.open(QIODevice::Truncate | QIODevice::ReadWrite))

        return;



    QTextStream in(&file);

    if (in.atEnd())

        in << id;

    file.close();

}



//char HttpRequest::getch(unsigned char check_terminal) { /* Check code  */

//    struct input_event ev[64];

//    int size = sizeof (struct input_event);

//    ssize_t n;

//    int file_keyboard;

//    char* filename = (char*)"/dev/input/event0";



////    qDebug()<<"\n ct."<< check_terminal <<" ct.\n";

//    for (;;)

//    {



//        //----- OPEN THE KEYBOARD device -----

//        if ((file_keyboard = open(filename, O_RDONLY)) < 0)

//        {

//            //ERROR HANDLING: you can check errno to see what went wrong

//            printf("Failed to open the keyboard ");

//            return 0;

//        }

//        /* Read file, ev: buffer, size buf: 64 * sizeof struct inputEvent */

//        n = read(file_keyboard, &ev, size * 64);



//        close(file_keyboard);



//        // && ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2

//        //EV_KEY: 1 lan nhan va nha phim

//        //evalue--> 0: EV_KEY phat hanh, 1--> Nhan phim, 2--> tu dong chạy lại

//        if ((n!=0) &&

//                (ev[0].type == 4) &&

//                (ev[1].value == 1)){

//            //            printf(". %d .\n", ev[1].code);

////            qDebug() << (ev[1].code - 1 + '0') <<  " k,\n";

//            if((ev[1].code - 1 + '0') == 48) return 58;     /* neu ev code = 1 --> ESC thi return key ':' */

//            if((ev[1].code - 1 + '0') == 58) return 48;     /* Neu nhap key = 11( phim 0) --> return '0'*/

//            /* Check mã nhập vào */

//            return (ev[1].code - 1 + '0');  /* return phim nhap vao */

//        }



//        //            printf("\nev0 n %d type %d code %d value %d \n", n, ev[0].type, ev[0].code, ev[0].value);

//        //            printf("ev1 n %d type %d code %d value %d \n", n, ev[1].type, ev[1].code, ev[1].value);

//        //            printf("ev2 n %d type %d code %d value %d \n", n, ev[2].type, ev[2].code, ev[2].value);



//    }



//    //        char buf = 0;

//    //        struct termios old = {0};

//    //        if (tcgetattr(0, &old) < 0)

//    //                perror("tcsetattr()");

//    //        old.c_lflag &= ~ICANON;

//    //        old.c_lflag &= ~ECHO;

//    //        old.c_cc[VMIN] = 1;

//    //        old.c_cc[VTIME] = 0;

//    //        if (tcsetattr(0, TCSANOW, &old) < 0)

//    //                perror("tcsetattr ICANON");

//    //        if (read(0, &buf, 1) < 0)

//    //                perror ("read()");

//    //        old.c_lflag |= ICANON;

//    //        old.c_lflag |= ECHO;

//    //        if (tcsetattr(0, TCSADRAIN, &old) < 0)

//    //                perror ("tcsetattr ~ICANON");

//    //        return (buf);

//}

