#include "InformationDisplay_DVL.h"

#include "Tool.h"

InformationDisplay_DVL::InformationDisplay_DVL(QWidget *parent) : QWidget(parent)
{
    //定义显示信息的控件
    GroupBox_DisplayDVL = new QGroupBox("多普勒测速仪(DVL)",this);
    Label_Height = new QLabel("高度(M)",this);
    Label_Depth = new QLabel("深度(M)",this);
    Label_SpeedX = new QLabel("X轴速度(mm/s)",this);
    Label_SpeedY = new QLabel("Y轴速度(mm/s)",this);
    Label_SpeedZ = new QLabel("Z轴速度(mm/s)",this);
    LabelVal_Height = new QLabel("0",this);
    LabelVal_Depth = new QLabel("0",this);
    LabelVal_SpeedX = new QLabel("0",this);
    LabelVal_SpeedY = new QLabel("0",this);
    LabelVal_SpeedZ = new QLabel("0",this);

    //定义大小和位置
    GroupBox_DisplayDVL->setGeometry(0,0,933,139);
    Label_Height->setGeometry(10,35,176,38);
    Label_Depth->setGeometry(195,35,176,38);
    Label_SpeedX->setGeometry(380,35,176,38);
    Label_SpeedY->setGeometry(565,35,176,38);
    Label_SpeedZ->setGeometry(750,35,176,38);
    LabelVal_Height->setGeometry(10,88,176,38);
    LabelVal_Depth->setGeometry(195,88,176,38);
    LabelVal_SpeedX->setGeometry(380,88,176,38);
    LabelVal_SpeedY->setGeometry(565,88,176,38);
    LabelVal_SpeedZ->setGeometry(750,88,176,38);

    //设置样式表
    QString StyleSheet = "QLabel { "
                         "font-size: 22px; "   // 设置字体大小为22像素
                         "border: 1px solid black; "  // 设置边框为1像素宽的实线
                         "border-radius: 5px; "  // 设置边框圆角为5像素
                         "padding: 5px; "  // 设置内边距为5像素
                         "qproperty-alignment: AlignCenter; "  // 设置文本居中
                         "}";
    Label_Height->setStyleSheet(StyleSheet);
    Label_Depth->setStyleSheet(StyleSheet);
    Label_SpeedX->setStyleSheet(StyleSheet);
    Label_SpeedY->setStyleSheet(StyleSheet);
    Label_SpeedZ->setStyleSheet(StyleSheet);
    LabelVal_Height->setStyleSheet(StyleSheet);
    LabelVal_Depth->setStyleSheet(StyleSheet);
    LabelVal_SpeedX->setStyleSheet(StyleSheet);
    LabelVal_SpeedY->setStyleSheet(StyleSheet);
    LabelVal_SpeedZ->setStyleSheet(StyleSheet);
}


void InformationDisplay_DVL::Slot_ShowDvlInformation(const QString &text)
{
    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    Data_Dvl = text;

    Temp_Pitch = Data_Dvl.section('*', 1, 1);
    Temp_Roll = Data_Dvl.section('*', 2, 2);
    Temp_Heading = Data_Dvl.section('*', 3, 3);
    Temp_TransducerEntryDepth = Data_Dvl.section('*', 4, 4);
    Temp_SpeedX = Data_Dvl.section('*', 5, 5);
    Temp_SpeedY = Data_Dvl.section('*', 6, 6);
    Temp_SpeedZ = Data_Dvl.section('*', 7, 7);
    Temp_ButtomDistance = Data_Dvl.section('*', 8, 8);


    Data_Pitch = Tool::trimLeftSpace(Temp_Pitch);
    Data_Roll= Tool::trimLeftSpace(Temp_Roll);
    Data_Heading = Tool::trimLeftSpace(Temp_Heading);
    Data_TransducerEntryDepth = Tool::trimLeftSpace(Temp_TransducerEntryDepth);
    Data_SpeedX = Tool::trimLeftSpace(Temp_SpeedX);
    Data_SpeedY = Tool::trimLeftSpace(Temp_SpeedY);
    Data_SpeedZ = Tool::trimLeftSpace(Temp_SpeedZ);
    Data_ButtomDistance = Tool::trimLeftSpace(Temp_ButtomDistance);

    /*
    QStringList datalist = Data_Dvl.split('*', QString::SkipEmptyParts); //提取数据
    Data_Pitch = Tool::trimLeftSpace(datalist[0]);
    Data_Roll= Tool::trimLeftSpace(datalist[1]);
    Data_Heading = Tool::trimLeftSpace(datalist[2]);
    Data_TransducerEntryDepth = Tool::trimLeftSpace(datalist[3]);
    Data_SpeedX = Tool::trimLeftSpace(datalist[4]);
    Data_SpeedY = Tool::trimLeftSpace(datalist[5]);
    Data_SpeedZ = Tool::trimLeftSpace(datalist[6]);
    Data_ButtomDistance = Tool::trimLeftSpace(datalist[7]);
    */
    emit Signal_LabelVal_Pitch(Data_Pitch);
    emit Signal_LabelVal_Roll(Data_Roll);
    emit Signal_LabelVal_Heading(Data_Heading);

    LabelVal_SpeedX->setText(Data_SpeedX);
    LabelVal_SpeedY->setText(Data_SpeedY);
    LabelVal_SpeedZ->setText(Data_SpeedZ);

    LabelVal_Depth->setText(Data_TransducerEntryDepth);
    LabelVal_Height->setText(Data_ButtomDistance);

    PrintMessage = CurrentTime + '\n' \
                   + "客户端：DVL数据:X轴速度(mm/s):" + Data_SpeedX + "Y轴速度(mm/s):" + Data_SpeedY + "Z轴速度(mm/s):" + Data_SpeedZ + '\n' \
                   + "DVL数据:高度(M):" + Data_ButtomDistance + "深度(M):" + Data_TransducerEntryDepth + '\n' \
                   + "DVL数据:俯仰(°):" + Data_Pitch + "横滚(°):" + Data_Roll + "航向(°):" + Data_Heading ;

    emit Signal_PrintDvlInformation(PrintMessage);
}

