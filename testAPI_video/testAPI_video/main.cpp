#include <QCoreApplication>
#include "http_request.h"
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QObject>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    http_request t;
    t.getID();
    QObject::connect(&t, SIGNAL(callQuit()), &a, SLOT(quit()), Qt::QueuedConnection);
    return a.exec();
}
