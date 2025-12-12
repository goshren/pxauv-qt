#include "InformationDisplay_Pose.h"

InformationDisplay_Pose::InformationDisplay_Pose(QWidget *parent) : QWidget(parent)
{
    //定义显示信息的控件
    GroupBox_DisplayPose = new QGroupBox("姿态信息",this);
    Label_Roll = new QLabel("横滚( ° )",this);
    Label_Pitch = new QLabel("俯仰( ° )",this);
    Label_Heading = new QLabel("航向( ° )",this);
    LabelVal_Roll = new QLabel("0",this);
    LabelVal_Pitch = new QLabel("0",this);
    LabelVal_Heading = new QLabel("0",this);

    //定义大小和位置
    GroupBox_DisplayPose->setGeometry(0,0,372,139);
    Label_Roll->setGeometry(14,35,105,38);
    Label_Pitch->setGeometry(133,35,105,38);
    Label_Heading->setGeometry(252,35,105,38);
    LabelVal_Roll->setGeometry(14,88,105,38);
    LabelVal_Pitch->setGeometry(133,88,105,38);
    LabelVal_Heading->setGeometry(252,88,105,38);

    //设置样式表
    QString StyleSheet = "QLabel { "
                         "font-size: 22px; "   // 设置字体大小为22像素
                         "border: 1px solid black; "  // 设置边框为1像素宽的实线
                         "border-radius: 5px; "  // 设置边框圆角为5像素
                         "padding: 5px; "  // 设置内边距为5像素
                         "qproperty-alignment: AlignCenter; "  // 设置文本居中
                         "}";
    Label_Roll->setStyleSheet(StyleSheet);
    Label_Pitch->setStyleSheet(StyleSheet);
    Label_Heading->setStyleSheet(StyleSheet);
    LabelVal_Roll->setStyleSheet(StyleSheet);
    LabelVal_Pitch->setStyleSheet(StyleSheet);
    LabelVal_Heading->setStyleSheet(StyleSheet);
}


void InformationDisplay_Pose::Slot_ShowRollInformation(const QString &text)
{
    LabelVal_Roll->setText(text);
}

void InformationDisplay_Pose::Slot_ShowPitchInformation(const QString &text)
{
    LabelVal_Pitch->setText(text);
}

void InformationDisplay_Pose::Slot_ShowHeadingInformation(const QString &text)
{
    LabelVal_Heading->setText(text);
}


