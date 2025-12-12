#include "InformationDisplay_Releaser.h"

InformationDisplay_Releaser::InformationDisplay_Releaser(QWidget *parent) : QWidget(parent)
{
    //定义显示信息的控件
    GroupBox_DisplayReleaser= new QGroupBox("释放器",this);
    Label_Releaser1Status = new QLabel("释放器1",this);
    LabelVal_Releaser1Status = new QLabel("关",this);
    Label_Releaser2Status = new QLabel("释放器2",this);
    LabelVal_Releaser2Status = new QLabel("关",this);

    //定义大小和位置
    GroupBox_DisplayReleaser->setGeometry(0,0,143+14+115,139);
    Label_Releaser1Status->setGeometry(14,35,115,38);
    LabelVal_Releaser1Status->setGeometry(14,88,115,38);
    Label_Releaser2Status->setGeometry(143,35,115,38);
    LabelVal_Releaser2Status->setGeometry(143,88,115,38);

    //设置样式表
    QString StyleSheet = "QLabel { "
                         "font-size: 22px; "   // 设置字体大小为22像素
                         "border: 1px solid black; "  // 设置边框为1像素宽的实线
                         "border-radius: 5px; "  // 设置边框圆角为5像素
                         "padding: 5px; "  // 设置内边距为5像素
                         "qproperty-alignment: AlignCenter; "  // 设置文本居中
                         "}";
    Label_Releaser1Status->setStyleSheet(StyleSheet);
    LabelVal_Releaser1Status->setStyleSheet(StyleSheet);
    Label_Releaser2Status->setStyleSheet(StyleSheet);
    LabelVal_Releaser2Status->setStyleSheet(StyleSheet);
}

void InformationDisplay_Releaser::Slot_ShowReleaserStatus(const QString &text)
{
    QString StatusData = text;
    if(StatusData == "!OK_releaser1!")
    {
        LabelVal_Releaser1Status->setText("开");
        emit Signal_UpdataRelease1Status(true);
    }
    else if(StatusData == "!ON_releaser1!")
    {
        LabelVal_Releaser1Status->setText("关");
        emit Signal_UpdataRelease1Status(false);
    }
    else if(StatusData == "!OK_releaser2!")
    {
        LabelVal_Releaser2Status->setText("开");
        emit Signal_UpdataRelease2Status(true);
    }
    else if(StatusData == "!ON_releaser2!")
    {
        LabelVal_Releaser2Status->setText("关");
        emit Signal_UpdataRelease2Status(false);
    }

}
