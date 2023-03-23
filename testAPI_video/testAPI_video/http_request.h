#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#define PATH_VIDEO_TO_QML "/home/pi/Desktop/ADELA/serverToAdvertisement.txt"
#define PATH_TO_FOLDER_VIDEO "/home/pi/Desktop/ADELA/VideoQuangCao_server_thaydoi/"
#define PATH_VIDEO_QC ""
#define PATH_FILE_SETTING "/home/pi/Desktop/ADELA/14_04_qr_added/nhap/build_Smart_LOCK_qt/Setting.txt"
extern int dem;
class http_request : public QObject
{
    Q_OBJECT
public:
    explicit http_request(QObject *parent = nullptr);
    //ham
    void getID();
    void dowloadVideoForID();
    void startRequest();
    void resetParam();
    QString saveFileName(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);
    QString layID();
    void writeLog(QString mess);

    // tham so

    QUrl url;
    QString id;
    QString name;
    int check_getID ;
    // tham so http
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QString location;
private slots:
    void httpReadyRead_ID();
    void httpFinished_ID();
    void httpFinished_dowload();
    //void http_quit();
signals:
    void callQuit();
public:
    void tat_video_qc();
    void bat_video_qc();
};

#endif // HTTP_REQUEST_H
