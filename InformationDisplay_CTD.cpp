#include "InformationDisplay_CTD.h"

InformationDisplay_CTD::InformationDisplay_CTD(QWidget *parent) : QWidget(parent)
{
    //定义显示信息的控件
    GroupBox_DisplayCTD = new QGroupBox("温盐深(CTD)",this);
    Label_Temperature = new QLabel("温度( ℃ )",this);
    Label_Salinity = new QLabel("盐度(PSU)",this);
    Label_Depth = new QLabel("深度( M )",this);
    LabelVal_Temperature = new QLabel("0",this);
    LabelVal_Salinity = new QLabel("0",this);
    LabelVal_Depth = new QLabel("0",this);

    Label_Conductivity = new QLabel("电导率(mS/cm)",this);
    Label_SoundVelocity = new QLabel("声速(m/s)",this);
    Label_Density = new QLabel("密度(kg/m³)",this);
    LabelVal_Conductivity = new QLabel("0",this);
    LabelVal_SoundVelocity = new QLabel("0",this);
    LabelVal_Density = new QLabel("0",this);


    //定义大小和位置
    GroupBox_DisplayCTD->setGeometry(0,0,880,139);
    Label_Temperature->setGeometry(14,35,130,38);
    Label_Salinity->setGeometry(158,35,130,38);
    Label_Depth->setGeometry(302,35,130,38);
    Label_Conductivity->setGeometry(446,35,130,38);
    Label_SoundVelocity->setGeometry(590,35,130,38);
    Label_Density->setGeometry(734,35,130,38);

    LabelVal_Temperature->setGeometry(14,88,130,38);
    LabelVal_Salinity->setGeometry(158,88,130,38);
    LabelVal_Depth->setGeometry(302,88,130,38);
    LabelVal_Conductivity->setGeometry(446,88,130,38);
    LabelVal_SoundVelocity->setGeometry(590,88,130,38);
    LabelVal_Density->setGeometry(734,88,130,38);



    //设置样式表
    QString StyleSheet = "QLabel { "
                         "font-size: 20px; "   // 设置字体大小为22像素
                         "border: 1px solid black; "  // 设置边框为1像素宽的实线
                         "border-radius: 5px; "  // 设置边框圆角为5像素
                         "padding: 5px; "  // 设置内边距为5像素
                         "qproperty-alignment: AlignCenter; "  // 设置文本居中
                         "}";
    Label_Temperature->setStyleSheet(StyleSheet);
    Label_Salinity->setStyleSheet(StyleSheet);
    Label_Depth->setStyleSheet(StyleSheet);
    Label_Conductivity->setStyleSheet(StyleSheet);
    Label_SoundVelocity->setStyleSheet(StyleSheet);
    Label_Density->setStyleSheet(StyleSheet);

    LabelVal_Temperature->setStyleSheet(StyleSheet);
    LabelVal_Salinity->setStyleSheet(StyleSheet);
    LabelVal_Depth->setStyleSheet(StyleSheet);
    LabelVal_Conductivity->setStyleSheet(StyleSheet);
    LabelVal_SoundVelocity->setStyleSheet(StyleSheet);
    LabelVal_Density->setStyleSheet(StyleSheet);

}

void InformationDisplay_CTD::Slot_ShowCtdInformation(const QString &text)
{
    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    Data_Ctd = text;

    Data_Temperature = Data_Ctd.section('#', 1, 1);
    Data_Conductivity = Data_Ctd.section('#', 2, 2);
    Data_Depth = Data_Ctd.section('#', 3, 3);
    Data_Salinity = Data_Ctd.section('#', 4, 4);
    Data_SoundVelocity = Data_Ctd.section('#', 5, 5);
    Data_Density = Data_Ctd.section('#', 6, 6);

    LabelVal_Temperature->setText(Data_Temperature);
    LabelVal_Salinity->setText(Data_Salinity);
    LabelVal_Depth->setText(Data_Depth);
    LabelVal_Conductivity->setText(Data_Conductivity);
    LabelVal_SoundVelocity->setText(Data_SoundVelocity);
    LabelVal_Density->setText(Data_Density);


    PrintMessage = CurrentTime + '\n' + "客户端：温盐深：温度(°c)："+ Data_Temperature \
            + "盐度(PPm):" + Data_Salinity + "深度(M):" + Data_Depth + '\n';
    emit Signal_PrintCtdInformation(PrintMessage);
}

