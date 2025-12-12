#include "InformationDisplay_Location.h"

#include "Tool.h"

InformationDisplay_Location::InformationDisplay_Location(QWidget *parent) : QWidget(parent)
{
    //定义显示信息的控件
    GroupBox_DisplayLocation= new QGroupBox("位置信息",this);
    Label_SystemFlag = new QLabel("系统标识",this);
    Label_SatelliteNum = new QLabel("卫星数量",this);
    Label_Longitude = new QLabel("经度(E)",this);
    Label_Latitude = new QLabel("纬度(N)",this);

    LabelVal_SystemFlag = new QLabel("0",this);
    LabelVal_SatelliteNum = new QLabel("0",this);
    LabelVal_Longitude = new QLabel("0",this);
    LabelVal_Latitude = new QLabel("0",this);

    //定义大小和位置
    GroupBox_DisplayLocation->setGeometry(0,0,474,139);
    Label_SystemFlag->setGeometry(14,35,101,38);
    Label_SatelliteNum->setGeometry(129,35,101,38);
    Label_Longitude->setGeometry(244,35,101,38);
    Label_Latitude->setGeometry(359,35,101,38);

    LabelVal_SystemFlag->setGeometry(14,88,101,38);
    LabelVal_SatelliteNum->setGeometry(129,88,101,38);
    LabelVal_Longitude->setGeometry(244,88,101,38);
    LabelVal_Latitude->setGeometry(359,88,101,38);

    //设置样式表
    QString StyleSheet = "QLabel { "
                         "font-size: 18px; "   // 设置字体大小为20像素
                         "border: 1px solid black; "  // 设置边框为1像素宽的实线
                         "border-radius: 5px; "  // 设置边框圆角为5像素
                         "padding: 5px; "  // 设置内边距为5像素
                         "qproperty-alignment: AlignCenter; "  // 设置文本居中
                         "}";
    Label_SystemFlag->setStyleSheet(StyleSheet);
    Label_SatelliteNum->setStyleSheet(StyleSheet);
    Label_Longitude->setStyleSheet(StyleSheet);
    Label_Latitude->setStyleSheet(StyleSheet);

    LabelVal_SystemFlag->setStyleSheet(StyleSheet);
    LabelVal_SatelliteNum->setStyleSheet(StyleSheet);
    LabelVal_Longitude->setStyleSheet(StyleSheet);
    LabelVal_Latitude->setStyleSheet(StyleSheet);
}

void InformationDisplay_Location::Slot_ShowLocationInformation(const QString &text)
{
    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    Data_Location = text;

    Temp_SystemFlag = Data_Location.section('/',1,1);
    Temp_IsValid = Data_Location.section('/',2,2);
    Temp_SatelliteNum = Data_Location.section('/',3,3);
    Temp_LongitudeDirection = Data_Location.section('/',4,4);
    Temp_Longitude = Data_Location.section('/',5,5);
    Temp_LatitudeDirection = Data_Location.section('/',6,6);
    Temp_Latitude = Data_Location.section('/',7,7);

    Data_Longitude = Tool::trimLeftSpace(Temp_Longitude);
    Data_Latitude = Tool::trimLeftSpace(Temp_Latitude);

    LabelVal_SystemFlag->setText(Temp_SystemFlag);
    LabelVal_SatelliteNum->setText(Temp_SatelliteNum);
    LabelVal_Longitude->setText(Data_Longitude);
    LabelVal_Latitude->setText(Data_Latitude);

    PrintMessage = CurrentTime + '\n' + "客户端：经纬度信息:经度:" + Data_Longitude + "纬度:" + Data_Latitude;
    emit Signal_PrintLocationInformation(PrintMessage);
    emit Signal_LocationString(Data_Longitude, Data_Latitude);
}
