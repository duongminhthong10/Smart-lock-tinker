#include <QDebug>
#include <QFile>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>
#include <wiringPi.h>
#include <stdio.h>
#include <cstdio>
#include <QIODevice>
#include <termios.h>
#include <linux/input.h>
#include <QProcess>
#include <pthread.h>
#include "config.h"
#include "processthread.h"
#include "distance_sensor.h"
#include "my_qr_code.h"
#include "my_qr_code.h"
#include "my_lcd.h"
#include "rgb_lcd.h"
#include "my_lock.h"


int8_t batCam = 0;
int64_t counter_qrcode = COUNTER_QR_CODE;

QString maQr;
string str_code;
bool waittingQr = true;

static void *checkQrThreadHandle(void *arg);

MyQrCode::MyQrCode(QObject *parent) : QObject(parent)
{

}

void MyQrCode::qr_start(){
    checkLockQr();
    distanceThread();
}

void MyQrCode::qrCodeInit(){
    //Init
    batCam = 1;
    counter_qrcode = COUNTER_QR_CODE;
    luongDem();
}

void *checkQrThreadHandle(void *arg)
{
  while(1){
        while (batCam){
            VideoCapture cap(0); // open the video camera no. 0
            if (!cap.isOpened()) // if not success, exit program
            {
                qDebug() << "Cannot open the video cam" << endl;
            }
            ImageScanner scanner;
            scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

            //get the width of frames of the video
            double dWidth = cap.get(CAP_PROP_FRAME_WIDTH);
            //get the height of frames of the video
            double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
            qDebug() << "Frame size : " << dWidth << " x " << dHeight << endl;

            //create a window called "MyVideo"
//            namedWindow("MyVideo",WINDOW_AUTOSIZE);

            while (counter_qrcode)
            {
                Mat frame;
                bool bSuccess = cap.read(frame); // read a new frame from video
                if (!bSuccess) //if not success, break loop
                {
                    qDebug() << "Cannot read a frame from video stream" << endl;
                    break;
                }

                Mat grey;
                cvtColor(frame,grey,cv :: COLOR_BGRA2GRAY);
                int width = frame.cols;
                int height = frame.rows;
                uchar *raw = (uchar *)grey.data;

                // wrap image data
                Image image(width, height, "Y800", raw, width * height);
                // scan the image for barcodes
                int n = scanner.scan(image);
                // extract results
                for(Image::SymbolIterator symbol = image.symbol_begin();
                    symbol != image.symbol_end(); ++symbol)
                {
                    vector<Point> vp;
                    // do something useful with results
                    //cout << "decoded " << symbol->get_type_name() << " symbol \"" <<             symbol->get_data() << '"' <<" "<< endl;
                    str_code = symbol->get_data();
                    QString keyString = QString::fromStdString(str_code);
                    maQr = keyString.mid(0,9);

                    if(maQr != ""){
                        qDebug() << "Da quet duoc: " << maQr;
                        waittingQr = false;                        
                        my_lcd_clear_line(0);
                        setCursor(0,0);
                        lcd_write_string("Enter key:");
                        QThread::sleep(5);
                    }
                    else{
                        //do no thing
                    }

                    cout << str_code << endl;
                    int n = symbol->get_location_size();
                    for(int i=0;i<n;i++){
                        vp.push_back(Point(symbol->get_location_x(i),
                                           symbol->get_location_y(i)));
                    }

                    RotatedRect r = minAreaRect(vp);
                    Point2f pts[4];
                    r.points(pts);
                    for(int i=0;i<4;i++){
                        line(frame,pts[i],pts[(i+1)%4],Scalar(255,0,0),3);
                    }
                    //cout<<"Angle: "<<r.angle<<endl;

                }
//                imshow("MyVideo", frame); //show the frame in "MyVideo" window
            }
        }
    }
}

void MyQrCode::checkLockQr()
{
    qrCodeInit();
    pthread_t checkCheck;
    pthread_create(&checkCheck, nullptr, &checkQrThreadHandle, nullptr);
}


