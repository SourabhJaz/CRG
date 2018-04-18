#ifndef METRICS_H
#define METRICS_H

#include <QDialog>

namespace Ui {
class Metrics;
}

class Metrics : public QDialog
{
    Q_OBJECT

public:
    explicit Metrics(QWidget *parent = 0);
    ~Metrics();

private:
    Ui::Metrics *ui;

private slots:
     void on_toolButton_clicked();
     void on_pushButton_clicked();
     void on_pushButton_2_clicked();
     void on_pushButton_3_clicked();
     void on_toolButton_2_clicked();
};

#endif // METRICS_H
