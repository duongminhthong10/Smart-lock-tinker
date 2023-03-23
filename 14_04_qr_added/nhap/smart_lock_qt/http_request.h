#ifndef HTTPREQUEST_H

#define HTTPREQUEST_H



#include <QObject>

#include <QNetworkAccessManager>

#include <QTimer>



extern int sellected_id;



struct pthread_data{    /* luong du lieu */

   uint8_t  count;  /*dem*/

   uint8_t counting;    /*dang dem*/

   uint8_t stop_count;  /*dung dem*/

};



class HttpRequest :public QObject

{

    Q_OBJECT



public:

    HttpRequest();



    void getDrawPosition();

    void resetParam();      /* Reset tất cả về mo */

    void start();           /* ko có gì đặc biệt */

    void senderUnlock();    /* nguoi gui mo khoa */

    void receiverUnlock();  /* Nguoi nhan mo khoa */

    void sendSMS();         /* Gui SMS */

    void writeLog(QString mess);    /* ghi ra file */



    static void *overwrite_last_char(void *vargp);

    static void *count_down_to_lock(void *vargp);

    static void *press_enter_to_lock(void *vargp);

    void startNewSession(); /* bắt đầu mùa giải mới fabet */

    void startRequest();    /* gửi requet đi, nhận về re pờ lai */

    void getCode();         /* nhận mã, các trường hợp như pageup, down... ,., enter... */

//    static char getch(unsigned char check_terminal);    /* get từ phím */



    void write_position(QString id);

public slots:

    void TimeOut();

private slots:

    void httpFinished();

    void httpReadyRead();

    void lock();    /* đưa code lên url, sau đó gửi đi */

    void onWaitLock();



signals:

    void callLock();



private:

    QTimer *timer;

    const int SENDER_UNLOCK = 1;

    const int SENDER_LOCK = 2;

    const int SEND_SMS = 3;

    const int RECEIVER_UNLOCK = 4;

    const int RECEIVER_LOCK = 5;



    struct pthread_data pthd;

    int count;

    int counting;

    int stop_count;

    char key[19];    //1-9: code, 0: set mode

    bool isInputting;   //đang nhập thôi, enter without admin_tool cái --> đi luôn

    bool isRequestPrevSuccess;

    int khoi_message_queue_in_angry_mode;



    QUrl url;

    int mode;   /* 0: sender, 1: receiver*/

    int statusRequest;  /* mấy state ở trêm */

    QString code;

    QString location;

    QString locker_name;

    QString drawer_position;

    QString link;

    QString receiver_code;

    QString receiver_phone;

    QString code_drawer_position[49];

    QString shiper_code[49];



    int res_isValid;    /* dữ liệu trả về --> toInt */

    QString res_link;

    QString res_receiver_code;

    QString res_receiver_phone;

    QString res_drawer_position;





    QNetworkAccessManager qnam;

    QNetworkReply *reply;

    bool httpRequestAborted;

};



#endif

