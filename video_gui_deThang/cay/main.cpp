#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QObject>
#include <QQuickView>
#include <QQuickItem>
#include <QMediaPlayer>
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
//    qmlRegisterType <API_Request>("API_Lib",1,0,"API_Request");
//    qmlRegisterType <Thread_1minute>("Thread_Lib",1,0,"Thread_1minute");
//    qmlRegisterType <http_request> ("http_lib",1,0,"Http_request");
    QQuickView view(QUrl("/home/pi/Desktop/ADELA/video_gui_deThang/cay/main.qml"));
//    QObject *item = view.rootObject();
//    //API_Request API_Request;
//    http_request http_request;

    //QObject::connect(item,SIGNAL(doSomething()),&http_request,SLOT(getID()));
    //QObject::connect(&engine,&QQmlApplicationEngine::doSomething ,&API_Request,&API_Request::test);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
//    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
//                     &app, [url](QObject *obj, const QUrl &objUrl) {
//        if (!obj && url == objUrl)
//            QCoreApplication::exit(-1);
//    }, Qt::QueuedConnection);
//    engine.load(url);
    view.showFullScreen();
    return app.exec();
}
