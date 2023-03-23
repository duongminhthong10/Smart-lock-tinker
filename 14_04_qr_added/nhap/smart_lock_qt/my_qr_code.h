#ifndef MY_QR_CODE_H
#define MY_QR_CODE_H

#include <QObject>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <zbar.h>
#include <iostream>
#include <QString>
#include <QProcess>

using namespace cv;
using namespace std;
using namespace zbar;
extern QString code;

class MyQrCode : public QObject
{
    Q_OBJECT
public:
    explicit MyQrCode(QObject *parent = nullptr);
    bool checkStaff(QString codeToCheck);
    void checkLockQr();
    void qrCodeInit();
    void qr_start();
};

#endif // MY_QR_CODE_H
