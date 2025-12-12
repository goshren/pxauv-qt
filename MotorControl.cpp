#include "MotorControl.h"

MotorControl::MotorControl(QWidget *parent) : QWidget(parent)
{
    //定义控件
    GroupBox_MotorControl = new QGroupBox("运动控制", this);
    Btn_MCUp = new QPushButton("上浮", this);
    Btn_MCDown = new QPushButton("下沉", this);
    Btn_MCLeft = new QPushButton("左转", this);
    Btn_MCRight = new QPushButton("右转", this);
    Btn_MCForward = new QPushButton("前进", this);
    Btn_MCBackward = new QPushButton("后退", this);
    Btn_MCStop = new QPushButton("停止", this);
    Btn_MCCheck = new QPushButton("检测", this);
    Spinbox_Arg = new QSpinBox(this);

    //设置参数以及初始化
    Spinbox_Arg->setMinimum(0);
    Spinbox_Arg->setMaximum(5);   
    Btn_MCUp->setEnabled(false);
    Btn_MCDown->setEnabled(false);
    Btn_MCLeft->setEnabled(false);
    Btn_MCRight->setEnabled(false);
    Btn_MCForward->setEnabled(false);
    Btn_MCBackward->setEnabled(false);
    Btn_MCStop->setEnabled(false);
    Btn_MCCheck->setEnabled(false);

    //设定 地理位置 以及 部件大小
    GroupBox_MotorControl->setGeometry(0,0,345,311);
    Btn_MCStop->setGeometry(15,47,95,32);
    Btn_MCLeft->setGeometry(15,149,95,32);
    Btn_MCBackward->setGeometry(15,251,95,32);

    Btn_MCUp->setGeometry(125,98,95,32);
    Spinbox_Arg->setGeometry(125,151,95,32);
    Btn_MCDown->setGeometry(125,200,95,32);

    Btn_MCForward->setGeometry(240,47,95,32);
    Btn_MCRight->setGeometry(240,149,95,32);
    Btn_MCCheck->setGeometry(240,251,95,32);
    
    connect(Btn_MCUp, &QPushButton::clicked, this, &MotorControl::Slot_Btn_MCUp_Clicked);
    connect(Btn_MCDown, &QPushButton::clicked, this, &MotorControl::Slot_Btn_MCDown_Clicked);
    connect(Btn_MCLeft, &QPushButton::clicked, this, &MotorControl::Slot_Btn_MCLeft_Clicked);
    connect(Btn_MCRight, &QPushButton::clicked, this, &MotorControl::Slot_Btn_MCRight_Clicked);
    connect(Btn_MCForward, &QPushButton::clicked, this, &MotorControl::Slot_Btn_MCForward_Clicked);
    connect(Btn_MCBackward, &QPushButton::clicked, this, &MotorControl::Slot_Btn_MCBackward_Clicked);
    connect(Btn_MCStop, &QPushButton::clicked, this, &MotorControl::Slot_Btn_MCStop_Clicked);
    connect(Btn_MCCheck, &QPushButton::clicked, this, &MotorControl::Slot_Btn_MCCheck_Clicked);
    
}

void MotorControl::Slot_Btn_MCUp_Clicked()
{
    int param = Spinbox_Arg->value();
    QString ctlcommand = QString("%1").arg(param, 2, 10, QChar('0'));
    QString sendMsg = "#UP$$" + ctlcommand + "#";
    
    emit Signal_Up(sendMsg);
}

void MotorControl::Slot_Btn_MCDown_Clicked()
{
    int param = Spinbox_Arg->value();
    QString ctlcommand = QString("%1").arg(param, 2, 10, QChar('0'));
    QString sendMsg = "#DN$$" + ctlcommand + "#";

    emit Signal_Down(sendMsg);
}

void MotorControl::Slot_Btn_MCLeft_Clicked()
{
    int param = Spinbox_Arg->value();
    QString ctlcommand = QString("%1").arg(param, 2, 10, QChar('0'));
    QString sendMsg = "#LT$$" + ctlcommand + "#";

    emit Signal_Left(sendMsg);
}

void MotorControl::Slot_Btn_MCRight_Clicked()
{
    int param = Spinbox_Arg->value();
    QString ctlcommand = QString("%1").arg(param, 2, 10, QChar('0'));
    QString sendMsg = "#RT$$" + ctlcommand + "#";

    emit Signal_Right(sendMsg);
}

void MotorControl::Slot_Btn_MCForward_Clicked()
{
    int param = Spinbox_Arg->value();
    QString ctlcommand = QString("%1").arg(param, 2, 10, QChar('0'));
    QString sendMsg = "#FD$$" + ctlcommand + "#";

    emit Signal_Forward(sendMsg);
}

void MotorControl::Slot_Btn_MCBackward_Clicked()
{
    int param = Spinbox_Arg->value();
    QString ctlcommand = QString("%1").arg(param, 2, 10, QChar('0'));
    QString sendMsg = "#BD$$" + ctlcommand + "#";

    emit Signal_Backward(sendMsg);
}

void MotorControl::Slot_Btn_MCStop_Clicked()
{
    int param = Spinbox_Arg->value();
    QString ctlcommand = QString("%1").arg(param, 2, 10, QChar('0'));
    QString sendMsg = "#ZE$$" + ctlcommand + "#";

    emit Signal_Stop(sendMsg);
}

void MotorControl::Slot_Btn_MCCheck_Clicked()
{
    int param = Spinbox_Arg->value();
    QString ctlcommand = QString("%1").arg(param, 2, 10, QChar('0'));
    QString sendMsg = "#CK$$" + ctlcommand + "#";

    emit Signal_Check(sendMsg);
}

void MotorControl::Slot_UpdateBtnMotorStatus(const bool status)
{
    if(status == true)
    {
        Btn_MCUp->setEnabled(true);
        Btn_MCDown->setEnabled(true);
        Btn_MCLeft->setEnabled(true);
        Btn_MCRight->setEnabled(true);
        Btn_MCForward->setEnabled(true);
        Btn_MCBackward->setEnabled(true);
        Btn_MCStop->setEnabled(true);
        Btn_MCCheck->setEnabled(true);
    }
    else
    {
        Btn_MCUp->setEnabled(false);
        Btn_MCDown->setEnabled(false);
        Btn_MCLeft->setEnabled(false);
        Btn_MCRight->setEnabled(false);
        Btn_MCForward->setEnabled(false);
        Btn_MCBackward->setEnabled(false);
        Btn_MCStop->setEnabled(false);
        Btn_MCCheck->setEnabled(false);
    }
}
