#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QTime>
#include <QTimer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QProcess>

#define PATH_FOLDER_VIDEO "/home/pi/Desktop/ADELA/video_mo_tu/video_mo_tu/video/"
#define PATH_NAME_VIDEO_TO_OPEN "/home/pi/Desktop/ADELA/Name_video_tu/"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

public:
    QProcess process_bat_video_qc;
    QProcess process_tat_video_qc;

private:
    QTimer *timer;
    QTimer *timer_play_video;
    Ui::Widget *ui;
    QMediaPlayer *player;
    QMediaPlaylist *playlist;

signals:
    void callquit();
public slots:
    void tat_video_nhap_sai();
    void tat_video_qc();
    void bat_video_qc();
};

#endif // WIDGET_H
