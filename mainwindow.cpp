#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QCoreApplication>
#include <QtGui>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QFileDialog>
#include <iostream>
#include <QHashIterator>
#include <string>
#include <QHash>
#include <QStringList>
#include <string.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include "metrics.h"
#include <QCloseEvent>
#include <stdlib.h>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->hide();
    this->setFixedSize(400,269);

}
MainWindow::~MainWindow()
{
    delete ui;
}

int cflag=1;

void delall()
{
    QFile::remove("clist.txt");
    QDir dis("output");
       dis.removeRecursively();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton ans;
    ans=QMessageBox::question(this,"Exit","Are you sure you want to close CRG ? ",QMessageBox::Yes|QMessageBox::No);
    if(ans == QMessageBox::Yes)
    {
        if(!cflag)
        {
            delall();
            system("taskkill /F /IM CRG.exe");
        }
        event->accept();        
    }
    else
    {
        event->ignore();
    }
}
void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox::about(this,"About tool","CRG calculates the total number of changes class wise for each common class between any two versions of software developed using Java.\n\nTool developed by Sourabh Jajoria under the guidance and constant support of Ms Ankita Bansal (Assistant Professor, NSIT).\n\nGot any bugs or hugs? - Write at sourabhjajoria@gmail.com");
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox::information(this,"Help","Browse source code directory paths and press calculate change");
}

void MainWindow::on_toolButton_clicked()
{
    QString f=QFileDialog::getExistingDirectory(this,tr("Choose directory of version 1"),QDir::homePath());
    ui->lineEdit->setText(f);
}

void MainWindow::on_toolButton_2_clicked()
{
    QString f=QFileDialog::getExistingDirectory(this,tr("Choose directory of version 2"),QDir::homePath());
    ui->lineEdit_2->setText(f);
}

string str,dir1,dir2;          //type of project,command string,output directories,package name
QString package,qdir1,qdir2;
int choose,packf;              //Choose directory version
QHash<QString,QStringList> has;

void rcomments(string name,string path)
{
 bool multi,sg,st;
 int i,n;
 string a,temp;
 fstream f1,f2;
 name=path+name;
 temp=path+"temp.java";
 f1.open(name.c_str(),ios::in);
 f2.open(temp.c_str(),ios::out);

 multi=false;
 sg=false;
 st=false;
 while(!f1.eof())
 {
  getline(f1,a);
  n=a.length();
  sg=false;
  st=false;

  for(i=0;i<n;i++)
  {
  QCoreApplication::processEvents();
  if(st&&a[i]!='"')
  {
   f2<<a[i];
  }
  else
  {
   if(st&&a[i]=='"')
   {
    f2<<'"';
    st=false;
   }
   else if(multi&&a[i]=='*'&&a[i+1]=='/')
   {
    multi=false;
    i++;
   }
   else if(multi||sg)
    continue;
   else if(a[i]=='/'&&a[i+1]=='/')
   {
    sg=true;
    i++;
   }
   else if(a[i]=='/'&&a[i+1]=='*')
   {
    multi=true;
    i++;
   }
   else if((multi==false&&sg==false)&&a[i]=='"')
   {
    st=true;
    f2<<a[i];
   }
   else
   {
    f2<<a[i];
   }
  }
 }
 f2<<endl;
}
f1.close();
f2.close();
remove(name.c_str());
rename(temp.c_str(),name.c_str());
}
void remake(string path1,string path2,vector<string> a,vector<string> b,vector<string> &name)
{

 int i,j,m,n;
 path1+="/";
 path2+="/";
 m=a.size();
 n=b.size();
 sort(a.begin(),a.end());
 sort(b.begin(),b.end());
 i=j=0;
 while(i<m&&j<n)
 {
  if(a[i]==b[j])
  {
    QCoreApplication::processEvents();
    rcomments(a[i],path1);
    rcomments(a[i],path2);
    name.push_back(a[i]);
    i++;
    j++;
  }
  else if(a[i]<b[j])
  {
    i++;
  }
  else
  {
    j++;
  }
 }
}
//Class generating code

void wr(QString line,QString s)
{
 QCoreApplication::processEvents();
 int len,j;
 QString name;
 len=s.length();
 j=len;

 while(j>=0)
 {
  s=s.mid(0,j);
  if(choose==1)
   name=qdir1+"/"+package+'.'+s;
  else
   name=qdir2+"/"+package+'.'+s;

   has[name].push_back(line);
   j-=1;
   while(j>=0&&s[j]!='.')
   {
    j--;
   }
 }
}
//
QString fnd(QString a,QString b,int &p1,int &p2)
{
    int i,j,n,m;
    n=a.length();
    m=b.length();
    QString tmp="";
    j=0;

    for(i=0;i<n;i++)
    {
     if(a[i]=='"')
     {
      i=i+1;
      while(i<n&&a[i]!='"')
        i++;
     }
     if(i>=n)
      break;
     if(j<m)
     {
      if(a[i]==b[j])
      {
            j++;
         if(j==m)
         {
             i++;
             while(i<n)
             {
              if(a[i]==' '||a[i]=='\n'||a[i]=='\t'||a[i]==';'||a[i]=='\r'||a[i]=='<'||a[i]=='{')
                break;
              tmp+=a[i];
              i++;
             }
         }
      }
      else if(j>0)
      {
       j=0;
       if(a[i]==b[j])
       {
            j++;
       }
      }
     }
     if(a[i]=='{')
     {
            p1++;
     }
     else if(a[i]=='}')
     {
            p2++;
     }
    }
    return tmp;
}
//
QString fnp(QString a,QString b)
{
    int i,j,n,m;
    QString tmp="";
    n=a.length();
    m=b.length();
    j=0;
    for(i=0;i<n;i++)
    {
     if(a[i]=='"')
     {
      i=i+1;
      while(i<n&&a[i]!='"')
        i++;
     }
     if(i>=n)
      break;
     if(j<m)
     {
      if(a[i]==b[j])
      {
         j++;
         if(j==m)
         {
             i++;
             while(i<n)
             {
              if(a[i]==' '||a[i]=='\n'||a[i]=='\t'||a[i]==';'||a[i]=='\r')
                return tmp;
              tmp+=a[i];
              i++;
             }
         }
      }
      else if(j>0)
      {
       j=0;
       if(a[i]==b[j])
       {
            j++;
       }
      }
     }
    }
    return tmp;
}
//
QString ispack(QString line)
{
 int idx,n;
 QString nam;
 QString c=" package ";
 n=line.length();
 nam=fnp(line,c);
 if(nam!="")
  return nam;

  nam=line.mid(0,8);
  c="";
  if(nam=="package ")
  {
   idx=8;
   while(idx<n)
   {
    if(line[idx]==' '||line[idx]=='\n'||line[idx]=='\t'||line[idx]==';'||line[idx]=='\r')
      break;
    c+=line[idx];
    idx++;
   }
  }
 return c;
}
//
QString isclass(QString line,int &p1,int &p2)
{
 int idx,n;
 QString c=" class ";
 QString nam;
 n=line.length();
 nam=fnd(line,c,p1,p2);
 if(nam!="")
   return nam;

  c="";
  nam=line.mid(0,6);
  if(nam=="class ")
  {
   idx=6;
   while(idx<n)
   {
    if(line[idx]==' '||line[idx]=='\n'||line[idx]=='\t'||line[idx]==';'||line[idx]=='\r'||line[idx]=='<'||line[idx]=='{')
     break;
    c+=line[idx];
    idx++;
   }
  }
 return c;
}
//
void rd(QString s,QString prev,QTextStream &f,int opn,int clo)
{
 QCoreApplication::processEvents();
 QString temp,n,pname;
 int flag,k,p1,p2;
 flag=0;
 k=0;
 if(prev!="")
 {
  wr(prev,s);
  if(opn)
  {
   k+=opn;
    if(!flag)
     flag=1;
  }
  if(clo)
  {
   k-=clo;
    if(!flag)
     flag=1;
  }
  if(k==0&&flag)
  {
   return;
  }
 }
 while(!f.atEnd())
 {
  temp=f.readLine();
  if(!packf)
  {
   pname=ispack(temp);
   if(pname!="")
   {
    packf=10;
    package=pname;
   }
  }
   p1=p2=0;
   n=isclass(temp,p1,p2);
  if(n!="")
  {
   //qDebug()<<"s: "<<s<<endl;
   if(s=="")
    s=n;
   else
   {
    rd(s+'.'+n,temp,f,p1,p2);
    continue;
   }
  }
  if(s!="")
  {
   //qDebug()<<"Working "<<s<<endl;
   wr(temp,s);
  }
  if(p1)
  {
   k+=p1;
    if(!flag)
     flag=1;
  }
  if(p2)
  {
   k-=p2;
    if(!flag)
     flag=1;
  }
  if(k==0&&flag)
  {
   return;
  }
 }
}
void common(vector<string> a,vector<string> b,vector<string> &name)
{
 string c;
 int i,j,m,n;
 m=a.size();
 n=b.size();
 sort(a.begin(),a.end());
 sort(b.begin(),b.end());
 i=j=0;
 while(i<m&&j<n)
 {
  if(a[i]==b[j])
  {
    name.push_back(a[i]);
    i++;
    j++;
  }
  else if(a[i]<b[j])
  {
    i++;
  }
  else
  {
    j++;
  }
 }
}

void MainWindow::on_pushButton_3_clicked()
{
    QString fa=ui->lineEdit->text();
    QString fb=ui->lineEdit_2->text();
    QString fc="java";            //ui->lineEdit_3->text();

    if(fa==""||fb=="")
    {
        QMessageBox::warning(this,"Warning","Please enter the directory paths!");
        return;
    }
    else if(fa==fb)
    {
        QMessageBox::warning(this,"Warning","Please enter two different directories!");
        return;
    }

    QDir di;
    di.setPath(fa);
    if (!di.exists()) {
        QMessageBox::critical(this,"Error","Directory path 1 is invalid!");
        return;
    }
    di.setPath(fb);
    if (!di.exists()) {
        QMessageBox::critical(this,"Error","Directory path 2 is invalid!");
        return;
    }

    string a,b,op,s1,s2,current,dir,res,com,cdir1,cdir2;
    int n,i,j;
    vector<string> names,v1,v2;
    fstream f1,f2;

    ui->progressBar->show();
    ui->statusBar->showMessage("Initialising..");

    delall();
    ui->statusBar->showMessage("Deleting old data..");
    ui->progressBar->setValue(5);

    QString curr=QDir::currentPath();
    current=curr.toStdString();
    dir="java";                 //fc.toStdString();

    dir1="output/"+dir+"1";
    dir2="output/"+dir+"2";
    cdir1="output/cl"+dir+"1";
    cdir2="output/cl"+dir+"2";
    qdir1=curr+"/output/cl"+fc+"1";
    qdir2=curr+"/output/cl"+fc+"2";

    QCoreApplication::processEvents();

    QDirIterator it(fa, QStringList() << "*.java", QDir::Files, QDirIterator::Subdirectories);
    QString fold,fname;
    QDir md;

    fold=curr+"/output/cl"+fc+"1";
    md.mkpath(fold);
    fold=curr+"/output/cl"+fc+"2";
    md.mkpath(fold);


    fold=curr+"/output/"+fc+"1";
    md.mkpath(fold);

    v1.clear();
    res="myfiles.lis";
    f1.open(res.c_str(),ios::out);

    cflag=0;

    ui->statusBar->showMessage("Extracting Java files from version 1..");
    if(it.hasNext())
    {
     while (it.hasNext())
     {
        QCoreApplication::processEvents();
        //qDebug()<<
        it.next();
        fname=it.fileInfo().fileName();
        if(QFile::copy(it.filePath(),fold+"/"+fname))
        {
                   v1.push_back(fname.toStdString());
                   a=it.filePath().toStdString();
                   f1<<a<<endl;
        }

     }
    }
    else
    {
        QMessageBox::critical(this,"Error","No Java files found in directory 1!");
        ui->statusBar->showMessage("Completed!");
        ui->progressBar->hide();
        cflag=1;
        return;
    }
    f1.close();
    ui->progressBar->setValue(28);

    QDirIterator it2(fb, QStringList() << "*.java", QDir::Files, QDirIterator::Subdirectories);
    fold=curr+"/output/"+fc+"2";
    md.mkpath(fold);

    v2.clear();
    ui->statusBar->showMessage("Extracting Java files from version 2..");
    if(it2.hasNext())
    {
     while (it2.hasNext())
     {
        QCoreApplication::processEvents();
        //qDebug()<<
        it2.next();
        fname=it2.fileInfo().fileName();
        if(QFile::copy(it2.filePath(),fold+"/"+fname))
        {
                   v2.push_back(fname.toStdString());
        }
     }
    }
    else
    {
        QMessageBox::critical(this,"Error","No Java files found in directory 2!");
        ui->statusBar->showMessage("Completed!");
        ui->progressBar->hide();
        cflag=1;
        return;
    }
    ui->progressBar->setValue(40);

    names.clear();
    ui->statusBar->showMessage("Preprocessing common Java files..");
    ui->pushButton->setEnabled(false);
    ui->pushButton_4->setEnabled(false);

    remake(dir1,dir2,v1,v2,names);
    n=names.size();
    ui->progressBar->setValue(60);

    a=current+"/"+dir1+"/";
    b=current+"/"+dir2+"/";

    QTextStream in2;
    QFile inputFile;
    QStringList::iterator ll;
    QString kk;
    QTextStream stream;
    QFile inp;

    has.clear();
    choose=1;
    ui->statusBar->showMessage("Extracting Classes for version 1..");
    for(i=0;i<n;i++)
    {
     op=a+names[i];
     inputFile.setFileName(op.c_str());
     if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
     {
        in2.setDevice(&inputFile);
        packf=0;
        while (!in2.atEnd())
        {
         rd("","",in2,0,0);
        }
        inputFile.close();
     }
    }

    QHashIterator<QString,QStringList> iter(has);
    while(iter.hasNext())
    {
        QCoreApplication::processEvents();
        iter.next();
        kk=iter.key();
        inp.setFileName(kk);
        if (inp.open(QIODevice::Append | QIODevice::Text))
        {
         stream.setDevice(&inp);

         for(ll=has[kk].begin();ll!=has[kk].end();ll++)
         {
              stream <<*ll<<"\n";
         }
         inp.close();
        }
    }
    ui->progressBar->setValue(70);

    ui->statusBar->showMessage("Extracting Classes for version 2..");
    has.clear();
    choose=2;
    for(i=0;i<n;i++)
    {
     op=b+names[i];
     inputFile.setFileName(op.c_str());
     if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
     {
        in2.setDevice(&inputFile);
        packf=0;
        while (!in2.atEnd())
        {
         rd("","",in2,0,0);
        }
        inputFile.close();
     }
    }

    QHashIterator<QString,QStringList> iter2(has);
    while(iter2.hasNext())
    {
        QCoreApplication::processEvents();
        iter2.next();
        kk=iter2.key();
        inp.setFileName(kk);
        if (inp.open(QIODevice::Append | QIODevice::Text))
        {
         stream.setDevice(&inp);

         for(ll=has[kk].begin();ll!=has[kk].end();ll++)
         {
              stream <<*ll<<"\n";
         }
         inp.close();
        }
    }
    ui->progressBar->setValue(75);
    has.clear();

    v1.clear();
    fb=curr+"/output/cl"+fc+"1";
    QDirIterator it3(fb, QStringList() << "*", QDir::Files);
    while (it3.hasNext())
    {
        it3.next();
        fname=it3.fileInfo().fileName();
        op=fname.toStdString();
        v1.push_back(op);
    }

    v2.clear();
    fb=curr+"/output/cl"+fc+"2";
    QDirIterator it4(fb, QStringList() << "*", QDir::Files);
    while (it4.hasNext())
    {
        it4.next();
        fname=it4.fileInfo().fileName();
        op=fname.toStdString();
        v2.push_back(op);
    }
    ui->statusBar->showMessage("Finding common Classes..");
    ui->progressBar->setValue(80);
    names.clear();
    common(v1,v2,names);

    res="results.csv";
    n=names.size();

    f1.open("gen.bat",ios::out);
    ui->statusBar->showMessage("Calculating change for common classes..");
    ui->progressBar->setValue(82);

    f1<<"cd GnuWin32/bin"<<endl;
    for(i=0;i<n;i++)
    {
     s1=current+"/"+cdir1+"/"+names[i];
     s2=current+"/"+cdir2+"/"+names[i];

     com="diff.exe -u -b -B "+s1+" "+s2+" | findstr \"+ -\" |find /c /v \"\" >>"+current+"/clist.txt";
     if(com.length()>7000)
     {
         QMessageBox::critical(this,"Internal Error","Directory paths too long for command!");
         cflag=1;
         return;
     }
     f1<<com<<endl;
     QCoreApplication::processEvents();
    }
    f1.close();
    if(system("gen.bat")==-1)
    {
        QMessageBox::critical(this,"Internal Error","Command could not be executed");
        return;
    }
    ui->progressBar->setValue(85);

    f1.open("results.csv",ios::out);
    f2.open("clist.txt",ios::in);
    i=0;
    while(!f1.eof()&&i<n)
    {
        f2>>j;
        QCoreApplication::processEvents();
        f1<<names[i]<<","<<j<<endl;
        i++;
    }
    f1.close();
    f2.close();

    ui->progressBar->setValue(98);
    ui->statusBar->showMessage("Deleting temporary files..");

    QFile::remove(curr+"/gen.bat");
    delall();
    ui->progressBar->setValue(100);

    cflag=1;

    QMessageBox::information(this,"CRG Done","Change report saved to "+curr+"/results.csv");
    ui->statusBar->showMessage("Completed!");
    ui->progressBar->hide();
    ui->pushButton->setEnabled(true);
    ui->pushButton_4->setEnabled(true);

    QMessageBox::StandardButton rep;
    rep=QMessageBox::question(this,"Use Understand?","Do you want to merge Object-Oriented metrics data with change report? \n( *Understand tool should be installed for this step )",QMessageBox::Yes|QMessageBox::No);
    if(rep == QMessageBox::Yes)
    {
        Metrics mwid(this);

        mwid.exec();
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    QString fa=ui->lineEdit->text();
    Metrics mwid(this);

    mwid.exec();
}
