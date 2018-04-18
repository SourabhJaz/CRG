#-------------------------------------------------
#
# Project created by QtCreator 2016-03-02T19:08:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CRG
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    metrics.cpp

HEADERS  += mainwindow.h \
    metrics.h

FORMS    += mainwindow.ui \
    metrics.ui

RESOURCES += \
    myresource.qrc

DISTFILES += \
    app.rc

win32:RC_FILE += app.rc
