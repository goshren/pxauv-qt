#include "InformationDisplay_CollectionStatus.h"

InformationDisplay_CollectionStatus::InformationDisplay_CollectionStatus(QWidget *parent) : QWidget(parent)
{
    //定义显示信息的控件
    GroupBox_DisplayCollectionStatus= new QGroupBox("采集舱",this);
    Label_CollectionStatus = new QLabel("状态",this);
    Label_EmergencyPower = new QLabel("紧急电源",this);
    LabelVal_CollectionStatus = new QLabel("关",this);
    LabelVal_EmergencyPower = new QLabel("关",this);

    //定义大小和位置
    GroupBox_DisplayCollectionStatus->setGeometry(0,0,227,139);
    Label_CollectionStatus->setGeometry(14,35,92,38);
    Label_EmergencyPower->setGeometry(120,35,92,38);
    LabelVal_CollectionStatus->setGeometry(14,88,92,38);
    LabelVal_EmergencyPower->setGeometry(120,88,92,38);

    //设置样式表
    QString StyleSheet = "QLabel { "
                         "font-size: 18px; "   // 设置字体大小为22像素
                         "border: 1px solid black; "  // 设置边框为1像素宽的实线
                         "border-radius: 5px; "  // 设置边框圆角为5像素
                         "padding: 5px; "  // 设置内边距为5像素
                         "qproperty-alignment: AlignCenter; "  // 设置文本居中
                         "}";
    Label_CollectionStatus->setStyleSheet(StyleSheet);
    Label_EmergencyPower->setStyleSheet(StyleSheet);
    LabelVal_CollectionStatus->setStyleSheet(StyleSheet);
    LabelVal_EmergencyPower->setStyleSheet(StyleSheet);
}

void InformationDisplay_CollectionStatus::Slot_ShowCollectionStatus(const QString &text)
{
    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    Temp_CollectionStatus = text.section('=', 1, 1);
    Temp_EmergencyPower = text.section('=', 2, 2);

    if("CCON " == Temp_CollectionStatus)
    {
        CollectionStatus = true;
        LabelVal_CollectionStatus->setText("开");
        PrintMessage = CurrentTime + '\n'+ "客户端：采集舱已打开！"+ '\n';
        emit Signal_PrintCollectionStatus(PrintMessage);
        emit Signal_UpdateCollectionStatus(CollectionStatus);
    }
    else if("CCOFF" == Temp_CollectionStatus)
    {
        CollectionStatus = false;
        LabelVal_CollectionStatus->setText("关");
        PrintMessage = CurrentTime + '\n'+ "客户端：采集舱已关闭！"+ '\n';
        emit Signal_PrintCollectionStatus(PrintMessage);
        emit Signal_UpdateCollectionStatus(CollectionStatus);
    }

    if(Temp_EmergencyPower == "EPON ")
    {
        LabelVal_EmergencyPower->setText("开");
    }
    else if(Temp_EmergencyPower == "EPOFF")
    {
        LabelVal_EmergencyPower->setText("关");
    }
}
