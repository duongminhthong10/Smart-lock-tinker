TEMPLATE = app
CONFIG += console c++11
QT += network
QT += core
QT -= gui
CONFIG -= app_bundle

#OPENCV
LIBS += -lzbar
INCLUDEPATH += /usr/local/include/opencv4
LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_imgcodecs -lopencv_videoio


SOURCES += \
    distance_sensor.cpp \
        http_request.cpp \
        main.cpp \
        my_lcd.cpp \
        my_lock.cpp \
    Khoi_TLC5971.cpp \
    my_qr_code.cpp \
    processthread.cpp \
    rgb_lcd.cpp \
    keyboard.cpp

DISTFILES += \
    smart_lock_qt.pro.user

INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib -lwiringPi

HEADERS += \
    distance_sensor.h \
    http_request.h \
    my_lcd.h \
    my_lock.h \
    KHOI_TLC5971.h \
    my_qr_code.h \
    processthread.h \
    rgb_lcd.h \
    config.h \
    keyboard.h

FORMS +=
