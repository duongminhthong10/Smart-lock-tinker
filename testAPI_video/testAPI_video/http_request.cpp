#include "http_request.h"
#include <QtNetwork>
#include <QFile>
#include <QIODevice>
#include <QDebug>
#include <QObject>
#include <QCoreApplication>
#include <QProcess>
#include <QDate>
#include <QTime>
http_request::http_request(QObject *parent) : QObject(parent)
{
    QFile file(PATH_FILE_SETTING);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error opening file: " << file.error();
        return;
    }
    QTextStream instream(&file);
    QString line = instream.readLine();
    file.close();
    QByteArray fileContent("");
    fileContent.append(line);

    QJsonDocument response = QJsonDocument::fromJson(fileContent);
    location = response.object().value("location").toString();
    reply = nullptr;

}

void http_request::getID() {
    check_getID = 1;
    url = QString("http://crm.adela.com.vn/current_video/" + location);
    qDebug() << url;
    QNetworkRequest request(url);
    reply = qnam.get(request);
    connect(reply, &QIODevice::readyRead, this, &http_request::httpReadyRead_ID);
    connect(reply,&QNetworkReply::finished,this,&http_request::httpFinished_ID);

}

void http_request::dowloadVideoForID()
{
    url = QString("http://crm.adela.com.vn/get_current_video/" + id);
    qDebug() << url;
    QNetworkRequest request(url);
    reply = qnam.get(request);
    connect(reply,&QNetworkReply::finished,this,&http_request::httpFinished_dowload);
}

QString http_request::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "dowloads";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}

bool http_request::saveToDisk(const QString &filename, QIODevice *data)
{
//    QFile file("/home/pi/Desktop/testAPI_video/build-testAPI_video-Desktop-Debug/" + filename + ".wmv");
    QString url_video = PATH_TO_FOLDER_VIDEO + filename;
    QFile file(url_video);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }


    file.write(data->readAll());
    file.close();

    return true;
}

QString http_request::layID()
{
    return id;
}

void http_request::writeLog(QString mess)
{
    QFile file(PATH_VIDEO_TO_QML);
    if (!file.open(QIODevice::ReadWrite))
        {
            qDebug() << "Eror while open file ";
            return;
        }

        QTextStream in(&file);
        QString line;

        QStringList all_data = in.readAll().split("\n");
        line = all_data.last();
        qDebug() << "All data: " << all_data;
        qDebug() << "LINE: " <<line;
        qDebug() << "MESS: " << mess;
        if(line != mess){
            qDebug() << "Da ghi " << mess << " vao file";
            in << "\n" + mess;
            //bat_video_qc();
        }
    file.close();

}

void http_request::httpReadyRead_ID()
{
    qDebug()<< "readedyRead";
    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QVariant contentLengthV = reply->header(QNetworkRequest::ContentLengthHeader);

    QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
    qDebug() <<"reason "<< reason;

    qDebug() <<"statuscode: "<< statusCodeV.toString();
    //    qDebug() <<"contentLength: "<< contentLengthV.toString();

    QByteArray rep = reply->readAll();
    //qDebug()<<"rep   "<<rep;

    QJsonDocument response = QJsonDocument::fromJson(rep);
    if (statusCodeV.toString() == "200") {
        QJsonValue data = response.object().value("content").toArray();
        QString name =  data[0].toObject().value("name").toString();
        QStringList a = name.split("-");
        id = a.first();
        qDebug() << id;

    }
}

void http_request::httpFinished_ID()
{
    reply->deleteLater();
    dowloadVideoForID();
}

void http_request::httpFinished_dowload()
{
    QUrl url = reply->url();
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
    } else {
        int t = 0;
        QString filename = saveFileName(url);
        QDir directory(PATH_TO_FOLDER_VIDEO);
        QStringList video = directory.entryList(QStringList() << "*",QDir::Files);
        for (int i = 0; i < video.length();i++)
        {
            if (video[i] == id)
            {
                t++;
            }
        }
        if (t == 0)
        {
            if (saveToDisk(filename, reply))
                printf("Download of %s succeded (saved to %s)\n",
                       url.toEncoded().constData(), qPrintable(filename));            
        }
        writeLog(id);
        bat_video_qc();
    }
    reply->deleteLater();
    QThread::sleep(1);
    emit callQuit();
    //return ;
}

void http_request::tat_video_qc()
{
    QProcess process_tat_video_qc;
    process_tat_video_qc.execute("sudo pkill omxplayer*");
}

void *bat_video_qc_handler(void * arg){
    QProcess process_bat;
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
    QString path_no_video = "sudo omxplayer --loop -n -1 --aspect-mode stretch /home/pi/Desktop/ADELA/VideoQuangCao_server_thaydoi/" + name_of_video;
    QString pathNo = path_no_video;
    //process_processThread.startDetached("sudo omxplayer --display 0 -o local --aspect-mode stretch /home/pi/Downloads/adela_huongdan.mp4");
    QTime cd = QTime::currentTime();
    int gio = cd.hour();
    qDebug() << gio;
    if (gio < 22 && gio > 7)
    {
        process_bat.execute(path_to_video);
    }
    else {
        qDebug() << "tat am thanh";
        process_bat.execute(pathNo);
    }

    //return nullptr;
    // thu


}

void http_request::bat_video_qc()
{
    tat_video_qc();
    QThread::msleep(200);
    pthread_t tid;
    pthread_create(&tid, nullptr, &bat_video_qc_handler, nullptr);
}



