#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>
int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    QString path= "/home/pi/Desktop/ADELA/VideoQuangCao_server_thaydoi";
    QDir dir(path);
    dir.setFilter( QDir::NoDotAndDotDot | QDir::Files );
    foreach( QString dirItem, dir.entryList() )
        dir.remove( dirItem );

    dir.setFilter( QDir::NoDotAndDotDot | QDir::Dirs );
    foreach( QString dirItem, dir.entryList() )
    {
        QDir subDir( dir.absoluteFilePath( dirItem ) );
        subDir.removeRecursively();
    }
    qDebug() << "xoa OK";
    return 0;
}
