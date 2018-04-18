#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QIODevice>
#include <QIcon>
#include <QMenuBar>
#include <QDir>
#include <QTimer>
#include <QtDebug>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSplashScreen *sp=new QSplashScreen;
    sp->setPixmap(QPixmap(":/imgs/cdcn.png"));
    sp->show();
    QIcon icon(":/imgs/cdcn.png");
    MainWindow w;
    w.setWindowFlags( (w.windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);
    QTimer::singleShot(2000,sp,SLOT(close()));
    QTimer::singleShot(2000,&w,SLOT(show()));
    w.setWindowIcon(icon);
    return a.exec();
}
