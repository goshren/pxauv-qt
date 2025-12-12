#include "ControlBtnArray.h"

ControlBtnArray::ControlBtnArray(QWidget *parent) : QWidget(parent)
{
    //定义控件
    GroupBox_ControlBtnArray = new QGroupBox("控制按钮",this);
    Btn_Releaser1Switch = new QPushButton("关闭释放1",this);
    Btn_Releaser2Switch = new QPushButton("关闭释放2",this);
    Btn_CollectionSwitch = new QPushButton("打开采集舱",this);
    Btn_UnderWaterSensorSwitch = new QPushButton("打开传感器",this);

    //设置控件的参数和初始化
    Btn_Releaser1Switch->setEnabled(false);
    Btn_Releaser2Switch->setEnabled(false);
    Btn_CollectionSwitch->setEnabled(false);
    Btn_UnderWaterSensorSwitch->setEnabled(false);

    //设定 地理位置 以及 部件大小
    GroupBox_ControlBtnArray->setGeometry(0,0,212,139);
    Btn_Releaser1Switch->setGeometry(14,35,85,38);
    Btn_Releaser2Switch->setGeometry(113,35,85,38);
    Btn_CollectionSwitch->setGeometry(14,88,85,38);
    Btn_UnderWaterSensorSwitch->setGeometry(113,88,85,38);

    //连接信号和槽函数
    connect(Btn_Releaser1Switch,&QPushButton::clicked,this,&ControlBtnArray::Slot_Btn_Releaser1Switch_Clicked);
    connect(Btn_Releaser2Switch,&QPushButton::clicked,this,&ControlBtnArray::Slot_Btn_Releaser2Switch_Clicked);
    connect(Btn_CollectionSwitch,&QPushButton::clicked,this,&ControlBtnArray::Slot_Btn_CollectionSwitch_Clicked);
    connect(Btn_UnderWaterSensorSwitch,&QPushButton::clicked,this,&ControlBtnArray::Slot_Btn_UnderWaterSensorSwitch_Clicked);
}

void ControlBtnArray::Slot_UpdateBtnControlArrayState(const bool TcpStatus)
{
    Btn_Releaser1Switch->setEnabled(TcpStatus);
    Btn_Releaser2Switch->setEnabled(TcpStatus);
    Btn_CollectionSwitch->setEnabled(TcpStatus);
    Btn_UnderWaterSensorSwitch->setEnabled(TcpStatus);
}

void ControlBtnArray::Slot_Btn_Releaser1Switch_Clicked()
{
    if(Releaser1Status)
    {
        emit Signal_Releaser1TurnOff("@close1@");
    }
    else
    {
        emit Signal_Releaser1TurnOn("@open1@");
    }
}

void ControlBtnArray::Slot_UpdateReleaser1Status(const bool Status)
{
    Releaser1Status = Status;
    if(true == Releaser1Status)
    {
        Btn_Releaser1Switch->setText("关闭释放1");
    }
    else
    {
        Btn_Releaser1Switch->setText("打开释放1");
    }
}

void ControlBtnArray::Slot_Btn_Releaser2Switch_Clicked()
{
    if(Releaser2Status)
    {
        emit Signal_Releaser1TurnOff("@close2@");
    }
    else
    {
        emit Signal_Releaser1TurnOn("@open2@");
    }
}

void ControlBtnArray::Slot_UpdateReleaser2Status(const bool Status)
{
    Releaser2Status = Status;
    if(true == Releaser2Status)
    {
        Btn_Releaser2Switch->setText("关闭释放2");
    }
    else
    {
        Btn_Releaser2Switch->setText("打开释放2");
    }
}

void ControlBtnArray::Slot_Btn_CollectionSwitch_Clicked()
{
    if(CollectionStatus)
    {
        emit Signal_CollectionTurnOff("+closecollect+");
    }
    else
    {
        emit Signal_CollectionTurnOn("+opencollect+");
    }
}

void ControlBtnArray::Slot_UpdateCollectionStatus(const bool Status)
{
    CollectionStatus = Status;
    if(true == CollectionStatus)
    {
        Btn_CollectionSwitch->setText("关闭采集舱");
    }
    else
    {
        Btn_CollectionSwitch->setText("打开采集舱");
    }
}

void ControlBtnArray::Slot_Btn_UnderWaterSensorSwitch_Clicked()
{
    if(true == UnderWaterSensorStatus)
    {
        emit Signal_UnderWaterSensorTurnOff("=closesensor=");
    }
    else
    {
        emit Signal_UnderWaterSensorTurnOn("=opensensor=");
    }
}


void ControlBtnArray::Slot_UpdateUnderWaterSensorStatus(const bool Status)
{
    UnderWaterSensorStatus = Status;
    if(true == UnderWaterSensorStatus)
    {
        Btn_UnderWaterSensorSwitch->setText("关闭传感器");
    }
    else
    {
        Btn_UnderWaterSensorSwitch->setText("打开传感器");
    }
}


