#include <processthread.h>
#include <QProcess>
#include <pthread.h>
#include "my_lcd.h"
#include <QFile>
#include "config.h"
#include <QDebug>
#include <QTime>
QProcess process_processThread;

bool videoNhapSaiIsOpen = false;

extern int64_t counter_qrcode;
extern int batCam;

static bool video_mo_tu_is_open = false;
static bool videoFullIsOpen = false;

void *luong_dem_handler(void *arg);
static void *bat_video_qc_handler(void *arg);
static void *tat_video_qcHandler(void *arg);



/**************************************
    CODE
***************************************/

void *bat_video_mo_tu_handler(void *arg){
    //    video_mo_tu_is_open = true;
    process_processThread.execute(PATH_VIDEO_MOTU);
    return nullptr;
}

void bat_video_mo_tu(){
    //    tat_video_so_tu();
    //    tat_video_qc();
    //    tat_video_nhap_sai();
    //    QThread::msleep(200);
    //    pthread_t threadID_batlen;
    //    pthread_create(&threadID_batlen, nullptr, bat_video_mo_tu_handler,nullptr);
    //    luong_dem_12s();
    tat_video_qc();
    tat_video_so_tu();
    tat_video_nhap_sai();
    QThread::msleep(200);
    pthread_t threadID_batlen;
    pthread_create(&threadID_batlen, nullptr, bat_video_mo_tu_handler,nullptr);
}

void *tat_video_so_tu_handler(void *arg){
    video_mo_tu_is_open = false;
    process_processThread.execute("pkill video_mo_tu");  //video  tu so bao nhieu mo len
    return nullptr;
}

void tat_video_so_tu(){
    pthread_t tat_video_so_tu_id;
    pthread_create(&tat_video_so_tu_id, nullptr, &tat_video_so_tu_handler,nullptr);
}
void *bat_video_qc_handler(void *arg)
{
    videoFullIsOpen = true;
    QString fileUrl = "/home/pi/Desktop/ADELA/serverToAdvertisement.txt";
    QFile file(fileUrl);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "Khong the mo file ";
        //        return;
    }
    QTextStream in(&file);
    QString content_file = in.readAll();
    QString name_of_video = content_file.split("\n").last();
//    QString name_of_video = in.readLine();
    qDebug() << "Name off ivdoe : " << name_of_video;
    QString path_to_video = "sudo omxplayer --loop -o local --aspect-mode stretch /home/pi/Desktop/ADELA/VideoQuangCao_server_thaydoi/" + name_of_video;
    //process_processThread.startDetached("sudo omxplayer --display 0 -o local --aspect-mode stretch /home/pi/Downloads/adela_huongdan.mp4");
    //process_processThread.execute(path_to_video);
    //process_processThread.execute("sudo omxplayer --display 0 -o local --aspect-mode stretch adela_huongdan.mp4");

    //process_processThread.execute("/home/pi/Desktop/ADELA/video_gui_deThang/build-cay-Desktop-Debug/cay");
    QTime cd = QTime::currentTime();
    int gio = cd.hour();
    qDebug() << gio;
    if (gio < 22 && gio > 7)
    {
        process_processThread.execute(path_to_video);
    }
    else {
        process_processThread.execute("sudo omxplayer --loop -n -1 --aspect-mode stretch /home/pi/Desktop/ADELA/VideoQuangCao_server_thaydoi/" + name_of_video);
    }
    return nullptr;
}

void bat_video_qc()
{
    pthread_t threadID_bat_video_qc;
    pthread_create(&threadID_bat_video_qc, nullptr, bat_video_qc_handler, nullptr);

}

void *codeSaiHandler(void * arg){
}

void videoCodeSai(){
    tat_video_qc();
    pthread_t threadID_codeSai;
    pthread_create(&threadID_codeSai, nullptr, codeSaiHandler,nullptr);
    luong_dem_12s();

}

/**
 * @brief Kill video full
 * @param arg
 * @return
 */
void *tat_video_qcHandler(void *arg)
{
    videoFullIsOpen = false;
    process_processThread.execute("sudo killall omxplayer.bin");
    return nullptr;
}
void tat_video_qc()
{
    pthread_t threadID_tat_video_qc;
    pthread_create(&threadID_tat_video_qc, nullptr, tat_video_qcHandler, nullptr);
}


/**
 * @brief Su dung de dem cho Cam bat 10s
 * @param arg
 * @return
 */
void *luong_dem_handler(void *arg)
{
    for (int i = 0; i < COUNTER_QR_CODE; i ++) {
        QThread::msleep(500);
        if(counter_qrcode != 0){
            counter_qrcode --;
            batCam = 0;
        }
    }
    return  nullptr;
}
void luongDem()
{
    pthread_t threadID_dem;
    pthread_create(&threadID_dem, nullptr, luong_dem_handler, nullptr);
}

void *luong_dem_12s_handler(void *arg)
{
    QThread::msleep(12000);
    //    if(video_mo_tu_is_open){
    //        tat_video_so_tu();
    //        bat_video_qc();
    //    }
    tat_video_so_tu();
    //    tat_video_nhap_sai();
    bat_video_qc();

    return  nullptr;
}
void luong_dem_12s()
{
    pthread_t threadID_dem12s;
    pthread_create(&threadID_dem12s, nullptr, luong_dem_12s_handler, nullptr);
}

void *luong_dem_12s_handlerNhapSai(void *arg)
{
    QThread::msleep(12000);
    //    if(videoNhapSaiIsOpen){
    //        tat_video_nhap_sai();
    //        bat_video_qc();
    //    }


    tat_video_nhap_sai();
    tat_video_qc();
    bat_video_qc();
    return  nullptr;
}
void luong_dem_12sNhapSai()
{
    pthread_t threadID_dem12s;
    pthread_create(&threadID_dem12s, nullptr, luong_dem_12s_handlerNhapSai, nullptr);
}

void *bat_video_nhap_sai_handler(void *arg){
    videoNhapSaiIsOpen = true;
    process_processThread.execute("/home/pi/Desktop/ADELA/video_nhap_sai/build/video_nhap_sai");
    return nullptr;
}

void bat_video_nhap_sai(){
    //    tat_video_nhap_sai();
    //    tat_video_qc();
    //    tat_video_so_tu();
    //    QThread::msleep(200);
    //    pthread_t threadID_batlennhapsai;
    //    pthread_create(&threadID_batlennhapsai, nullptr, bat_video_nhap_sai_handler,nullptr);
    //    luong_dem_12sNhapSai();

    tat_video_qc();
    tat_video_so_tu();
    tat_video_nhap_sai();
    QThread::msleep(200);
    pthread_t threadID_batlennhapsai;
    pthread_create(&threadID_batlennhapsai, nullptr, bat_video_nhap_sai_handler,nullptr);

}

void *tat_video_nhap_saiHandler(void *arg){
    //    videoNhapSaiIsOpen = false;
    process_processThread.execute("pkill video_nhap_sai");
    return nullptr;
}

void tat_video_nhap_sai(){
    pthread_t threadID_tat_video_nhap_sai;
    pthread_create(&threadID_tat_video_nhap_sai, nullptr, &tat_video_nhap_saiHandler,nullptr);
}
