#ifndef QCALWIDGET_H
#define QCALWIDGET_H

#include <QMainWindow>
#include <QDebug>
#include "visa/visa.h"
#include <QMessageBox>
#include "ui_qcalwidget.h"

#define READ_BUFFER_SIZE 4096

namespace Ui {
class QCALWidget;
}

class QCALWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit QCALWidget(QWidget *parent = 0);
    ~QCALWidget();
    void sleep(int msec);
    Ui::QCALWidget *ui;
private slots:
    void on_pushButtonQCALScan_released();
    void on_pushButtonQCALConnect_released();
    void on_TEXT_CLEAR_released();
private:
    //连接设备所需
    char instrDescriptor[VI_FIND_BUFLEN];
    ViUInt32 numInstrs;
    ViFindList findList;
};

#endif // QCALWIDGET_H
