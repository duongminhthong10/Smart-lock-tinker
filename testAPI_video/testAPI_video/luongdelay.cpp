#include "luongdelay.h"
#include <QThread>
#include "http_request.h"
int dem;
void *demGiayHandler (void *arg)
{
    QThread::sleep(10);
    dem = 1;
    return nullptr;
}
void demGiay()
{
    pthread_t thread_demGiay;
    pthread_create(&thread_demGiay,nullptr,demGiayHandler,nullptr);
}
