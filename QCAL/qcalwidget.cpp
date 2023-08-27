#include "qcalwidget.h"
#include "ui_qcalwidget.h"
#include <QTimer>


//以下为仪器句柄
static ViSession defaultRM;//默认
static ViSession instr;


static ViStatus status;

static ViUInt32 retCount;
static ViUInt32 writeCount;
static unsigned char buffer[100];
static char stringinput[512];

uint8_t cal_cnt,cal_flag=0;//校准查询指令读取变量，判断校准查询指令变量
//界面初始化
QCALWidget::QCALWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QCALWidget)
{
    ui->setupUi(this);
}
//关闭窗口
QCALWidget::~QCALWidget()
{

    viClose (instr);
    viClose (defaultRM);

    delete ui;
}
//扫描按键按下时，自动扫描电脑的通信口，目前串口测试通过
void QCALWidget::on_pushButtonQCALScan_released()
{
    static ViString DevCmd; //指令字符串
    ui->device_select->clear();//通信口选择框清除

   status=viOpenDefaultRM (&defaultRM);//使用VISA获取资源管理器 把这个句柄存储在defaulultrm
   if (status < VI_SUCCESS)//没有成功打开，弹出消息框提示
   {
       QMessageBox::information(this,tr("提示"),tr("不能打开VISA资源!"),QMessageBox::Ok);
       return;//Could no open a session to the VISA Resource!
   }

   //所有端口查询，添加至选项框 'G','P','I','B',
   char GPIBCmd[]={'?','*','I','N','S','T','R','\0'};
   DevCmd=GPIBCmd;
   status = viFindRsrc (defaultRM, DevCmd, &findList, &numInstrs, instrDescriptor);    //查询VISA系统进行资源定位
   if (status < VI_SUCCESS)//没有成功打开，弹出消息框提示
   {
       QMessageBox::information(this,tr("提示"),tr("查找资源时出错!"),QMessageBox::Ok);
       viClose (defaultRM);//An error occurred while finding resources
       return;
   }
      //添加第一个通信口
   ui->device_select->addItem(tr("%1").arg(instrDescriptor));//总选择框

   while (--numInstrs)//将查询到的通信口循环添加至选择框
   {
      status = viFindNext (findList, instrDescriptor);  /* 查找下一个 */
      if (status < VI_SUCCESS)         //查找下一个通信口出错，提示
      {
         QMessageBox::information(this,tr("提示"),tr("查找下一个资源时出错!\n点击以继续。."),QMessageBox::Ok);
         viClose (defaultRM);//An error occurred finding the next resource.\nHit enter to continue.
      }
      ui->device_select->addItem(tr("%1").arg(instrDescriptor));
   }    /* 结束循环 */

}

//延时函数
void QCALWidget::sleep(int msec)
{
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}

//查询按钮被点击
void QCALWidget::on_pushButtonQCALConnect_released()
{
    QString midname=ui->device_select->currentText();//得到当前选择的通讯口
    char *name=midname.toLatin1().data();//格式转换

   status = viOpen (defaultRM, (ViRsrc) name, VI_NULL, VI_NULL, &instr);//打开选择的资源
   if (status < VI_SUCCESS)//打开通信口失败，进行提示
   {
       QMessageBox::information(this,tr("提示"),tr("无法打开该设备!\n"),QMessageBox::Ok);
        return;//Cannot open a session to the device.
   }

   sleep(300);//延时300ms，读返回消息
   strcpy(stringinput,"*IDN?\r\n");//发送查询指令
   status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);//写入查询指令
   if (status < VI_SUCCESS)//写入失败提示
   {
      QMessageBox::information(this,tr("提示"),tr("写入错误！"),QMessageBox::Ok);
      return;//Error writing to the device\n
   }

   sleep(100);//延时100ms

   status = viRead (instr, buffer, 100, &retCount);//尝试读返回消息
   if (status < VI_SUCCESS)
   {
     QMessageBox::information(this,tr("提示"),tr("读取错误!"),QMessageBox::Ok);
   }
   else{
      QString str;
      uint16_t i;
      char mid_buffer[100];
      for(i=0;i<100;i++)
      {
         mid_buffer[i]= buffer[i];
      }
      printf("\nData read: %s\n", buffer);

      str=QString::fromLocal8Bit(mid_buffer,100);
      ui->textBrowser->append(mid_buffer);
      ui->textBrowser->moveCursor(QTextCursor::End);
   }
   viClose (instr);
}

void QCALWidget::on_TEXT_CLEAR_released()
{
   ui->textBrowser->clear();
}




