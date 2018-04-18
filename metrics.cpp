#include "metrics.h"
#include "ui_metrics.h"
#include <QMessageBox>
#include <QtGui>
#include <QWidget>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QString>
#include <stdlib.h>
#include <QDirIterator>
#include <QFileDialog>
#include <qfile.h>
#include <string>
#include<fstream>
#include <string.h>
#include <iostream>
#include <QWindow>
using namespace std;

Metrics::Metrics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Metrics)
{
    ui->setupUi(this);
    ui->label_3->hide();
    this->setFixedSize(400,269);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Metrics::~Metrics()
{
    delete ui;
}

void Metrics::on_toolButton_clicked()
{
    QString f=QFileDialog::getExistingDirectory(this,tr("Locate Understand installation"),QDir::homePath());
     ui->lineEdit_2->setText(f);
}

void Metrics::on_pushButton_clicked()
{
    QString fa=ui->lineEdit_4->text();
    QString fb=ui->lineEdit_2->text();
    QString fn=ui->lineEdit_3->text();
    QDir di;
    QString curr=QDir::currentPath();

    QFile::remove("anb.csv");

    if(fb=="")
    {
     fb="C:/Program Files/SciTools";
    }
    if(fn=="")
    {
        fn="cdcum";
    }

    di.setPath(fb);
    if (!di.exists()) {
        QMessageBox::critical(this,"Error","Directory path for Understand is invalid!");
        return;
    }

    if(fa=="")
    {
        fa=curr;
    }
    else
    {
        di.setPath(fa);
        if (!di.exists())
        {
            QMessageBox::critical(this,"Error","Directory path to output folder is invalid!");
            return;
        }
    }

    fn=fa+"/"+fn;
    fn+=".csv";
    string s,com,cur,fname;

    ui->label_3->setText("Initialising..");
    QCoreApplication::processEvents();
    QDirIterator it(fb, QStringList() << "und.exe", QDir::Files, QDirIterator::Subdirectories);
    if(it.hasNext())
    {
      it.next();
      s=it.fileInfo().absolutePath().toStdString();
    }
    else
    {
        QMessageBox::critical(this,"Error","und.exe not found in Understand directory!");
        return;
    }
    ui->label_3->show();

    cur=curr.toStdString();
    fname=fn.toStdString();

    ui->label_3->setText("Calculating..");
    QCoreApplication::processEvents();

    fstream fp;
    fp.open("comlist.txt",ios::out);
    fp<<"create -db "+cur+"/myproj.udb -languages java add @"+cur+"/myfiles.lis"<<endl;
    fp<<"settings -JavaClassPathsAdd "+cur+"/Java/src-jdk.zip "+cur+"/myproj.udb"<<endl;
    fp<<"settings -JavaClassPathsAdd "+cur+"/Java/rt.jar "+cur+"/myproj.udb"<<endl;
    fp<<"-quiet analyze "+cur+"/myproj.udb"<<endl;
    fp<<"uperl "+cur+"/finalperl.pl -db "+cur+"/myproj.udb -r "+cur+"/results.csv -f "+cur+"/anb.csv";
    fp.close();

    com="cd /d "+s+" && und.exe process "+cur+"/comlist.txt";

    if(system(com.c_str())<0)
    {
        QMessageBox::critical(this,"Internal Error","Command could not be executed");
        QFile::remove(curr+"/myproj.udb");
        return;
    }

    QTextStream in2;
    QFile inputFile;
    QString lal;
    QStringList lis;

    ui->label_3->setText("Renaming Metrics");

    inputFile.setFileName("anb.csv");
    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       in2.setDevice(&inputFile);
       if(!in2.atEnd())
       {
           lal=in2.readLine();
           lal.replace(",CountClassCoupled,",",CBO,");
           lal.replace(",CountClassDerived,",",NOC,");
           lal.replace(",CountDeclMethod,",",WMC,");
           lal.replace(",CountLineCode,",",LOC,");
           lal.replace(",CountDeclMethodAll,",",RFC,");
           lal.replace(",MaxInheritanceTree,",",DIT,");
           lal.replace(",PercentLackOfCohesion,",",LCOM,");
           lal.replace(",CountDeclMethodPublic,",",NPM,");
           lal.replace(",CountDeclMethodProtected,",",NPROM,");
           lal.replace(",CountDeclMethodPrivate,",",NPRM,");

           lis.append(lal);
       }
       while(!in2.atEnd())
       {
           lal=in2.readLine();
           lis.append(lal);
       }
    }
    else
    {
        QMessageBox::critical(this,"Internal Error 1","Metrics name could not be changed");
        return;
    }
    inputFile.close();

    QStringListIterator lisit(lis);
    inputFile.setFileName(fname.c_str());
    if (inputFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
       in2.setDevice(&inputFile);
       while(lisit.hasNext())
       {
           in2<<lisit.next()<<endl;
       }
    }
    else
    {
        QMessageBox::critical(this,"Internal Error 2","Metrics name could not be changed");
        return;
    }

    QFile::remove("anb.csv");
    inputFile.close();

    QFile::remove(curr+"/myproj.udb");

    ui->label_3->setText("Completed!");
    QMessageBox::information(this,"Done","Results saved to "+fn);
}

void Metrics::on_pushButton_2_clicked()
{
    QMessageBox::about(this,"About tool","Automates SciTools Understand tool for Object-Oriented metrics calculation for each class present in the change report.\n\nTool developed by Sourabh Jajoria under the guidance and constant support of Ms Ankita Bansal (Assistant Professor, NSIT).\n\nGot any bugs or hugs? - Write at sourabhjajoria@gmail.com");
}

void Metrics::on_pushButton_3_clicked()
{
    QMessageBox::information(this,"Help","Enter the Understand installation directory, choose a directory for storing output (Optional), enter a name for the output file and press calculate metrics");
}

void Metrics::on_toolButton_2_clicked()
{
    QString f=QFileDialog::getExistingDirectory(this,tr("Locate output folder"),QDir::homePath());
    ui->lineEdit_4->setText(f);
}
