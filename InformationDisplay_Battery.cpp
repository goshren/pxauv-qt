#include "InformationDisplay_Battery.h"

InformationDisplay_Battery::InformationDisplay_Battery(QWidget *parent) : QWidget(parent)
{
    //定义显示信息的控件
    GroupBox_DisplayBattery= new QGroupBox("电池信息",this);
    Label_Voltage = new QLabel("电压( V )",this);
    LabelVal_Voltage = new QLabel("0",this);

    //定义大小和位置
    GroupBox_DisplayBattery->setGeometry(0,0,143,139);
    Label_Voltage->setGeometry(14,35,115,38);
    LabelVal_Voltage->setGeometry(14,88,115,38);

    //设置样式表
    QString StyleSheet = "QLabel { "
                         "font-size: 22px; "   // 设置字体大小为22像素
                         "border: 1px solid black; "  // 设置边框为1像素宽的实线
                         "border-radius: 5px; "  // 设置边框圆角为5像素
                         "padding: 5px; "  // 设置内边距为5像素
                         "qproperty-alignment: AlignCenter; "  // 设置文本居中
                         "}";
    Label_Voltage->setStyleSheet(StyleSheet);
    LabelVal_Voltage->setStyleSheet(StyleSheet);
}

void InformationDisplay_Battery::Slot_ShowBatteryVoltage(const QString &text)
{
    Str_Voltage = text.section('-',1,1);
    LabelVal_Voltage->setText(Str_Voltage);

    PrintMessage = CurrentTime + '\n'+ "客户端：电池电压(V)："+ Str_Voltage + '\n';
    emit Signal_PrintBatteryVoltage(PrintMessage);
}

