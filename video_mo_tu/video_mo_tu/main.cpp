#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.showFullScreen();
//    QObject::connect(&w, SIGNAL(callquit()), &a, SLOT(quit()), Qt::QueuedConnection);
    return a.exec();
}
