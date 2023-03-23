#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QString>
#include <QThread>
#include <pthread.h>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    timer_play_video = new QTimer(this);

    QString fileUrl = PATH_NAME_VIDEO_TO_OPEN + QString::fromStdString("So_tu_duoc_mo");

    tat_video_qc();
    QThread::msleep(100);
    tat_video_nhap_sai();

    QFile file(fileUrl);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "Khong the mo file ";
        return;
    }
    QTextStream in(&file);
//    QString file_content = in.readAll();
//    QString file_name_video = file_content.split("\n").last();

    QString file_name_video = in.readLine();
    qDebug() << "File name  da doc duoc tu file : " << file_name_video;
    QString file_url = PATH_FOLDER_VIDEO +  file_name_video  + ".wmv";
    qDebug() << "File URL: " << file_url;
    file.close();

    QThread::msleep(50);
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist();
    player->setVolume(70);

    QVideoWidget *videoWidget = new QVideoWidget(this);
    player->setVideoOutput(videoWidget);
    ui->movieLayout->addWidget(videoWidget);
    playlist->addMedia(QUrl::fromLocalFile(file_url));
    player->setMedia(playlist);
    player->play();
    connect(timer, &QTimer::timeout, qApp, QApplication::quit);
    timer->start(13000);
    connect(timer_play_video, &QTimer::timeout, this, &Widget::bat_video_qc);
    timer_play_video->start(11000);
//    emit callquit();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::tat_video_qc()
{
    process_tat_video_qc.execute("sudo killall omxplayer.bin");
}

void Widget::tat_video_nhap_sai()
{
    process_tat_video_qc.execute("sudo pkill video_nhap_sai");
}

static void *bat_video_qc_handler(void * arg)
{
    QThread::msleep(300);
    Widget wg;
    QString fileUrl = "/home/pi/Desktop/ADELA/serverToAdvertisement.txt";
    QFile file(fileUrl);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "Khong the mo file ";
        bat_video_qc_handler(nullptr);
    }

    QTextStream in(&file);
    QString content_file = in.readAll();
    QString name_of_video = content_file.split("\n").last();
//    QString name_of_video = in.readLine();
    qDebug() << "Name off ivdoe : " << name_of_video;
    QString path_to_video = "sudo omxplayer --loop -o local --aspect-mode stretch /home/pi/Desktop/ADELA/VideoQuangCao_server_thaydoi/" + name_of_video;
    //process_processThread.startDetached("sudo omxplayer --display 0 -o local --aspect-mode stretch /home/pi/Downloads/adela_huongdan.mp4");
    //wg.process_bat_video_qc.execute(path_to_video);
    QTime cd = QTime::currentTime();
    int gio = cd.hour();
    qDebug() << gio;
    if (gio < 22 && gio > 7)
    {
        wg.process_bat_video_qc.execute(path_to_video);
    }
    else {
        wg.process_bat_video_qc.execute("sudo omxplayer --loop -n -1 --aspect-mode stretch /home/pi/Desktop/ADELA/VideoQuangCao_server_thaydoi/" + name_of_video);
    }
    return nullptr;
}

void Widget::bat_video_qc(){
    pthread_t tid;
    pthread_create(&tid, nullptr, &bat_video_qc_handler, nullptr);
}
