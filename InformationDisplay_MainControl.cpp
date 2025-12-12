#include "InformationDisplay_MainControl.h"

#include "Tool.h"

InformationDisplay_MainControl::InformationDisplay_MainControl(QWidget *parent) : QWidget(parent)
{
    //定义显示信息的控件
    GroupBox_DisplayMainControl = new QGroupBox("主控舱状态",this);
    Label_MainTemperature = new QLabel("温度( ℃ )",this);
    Label_MainHumidness = new QLabel("湿度(%)",this);
    Label_MainPressure = new QLabel("气压(hPa)",this);
    Label_MainLeak = new QLabel("舱体泄露",this);
    Label_MainDeviceState = new QLabel("传感器", this);
    Label_MainReleaser1State = new QLabel("释放器1", this);
    Label_MainReleaser2State = new QLabel("释放器2", this);

    LabelVal_MainTemperature = new QLabel("0",this);
    LabelVal_MainHumidness = new QLabel("0",this);
    LabelVal_MainPressure = new QLabel("0",this);
    LabelVal_MainLeak = new QLabel("未泄露",this);
    LabelVal_MainDeviceState = new QLabel("关", this);
    LabelVal_MainReleaser1State = new QLabel("关", this);
    LabelVal_MainReleaser2State = new QLabel("关", this);

    //定义大小和位置
    GroupBox_DisplayMainControl->setGeometry(0,0,968,139);
    Label_MainTemperature->setGeometry(14,35,122,38);
    Label_MainHumidness->setGeometry(150,35,122,38);
    Label_MainPressure->setGeometry(286,35,122,38);
    Label_MainLeak->setGeometry(422,35,122,38);
    Label_MainDeviceState->setGeometry(558,35,122,38);
    Label_MainReleaser1State->setGeometry(694,35,122,38);
    Label_MainReleaser2State->setGeometry(830,35,122,38);

    LabelVal_MainTemperature->setGeometry(14,88,122,38);
    LabelVal_MainHumidness->setGeometry(150,88,122,38);
    LabelVal_MainPressure->setGeometry(286,88,122,38);
    LabelVal_MainLeak->setGeometry(422,88,122,38);
    LabelVal_MainDeviceState->setGeometry(558,88,122,38);
    LabelVal_MainReleaser1State->setGeometry(694,88,122,38);
    LabelVal_MainReleaser2State->setGeometry(830,88,122,38);

    //设置样式表
    QString StyleSheet = "QLabel { "
                         "font-size: 18px; "   // 设置字体大小为22像素
                         "border: 1px solid black; "  // 设置边框为1像素宽的实线
                         "border-radius: 5px; "  // 设置边框圆角为5像素
                         "padding: 5px; "  // 设置内边距为5像素
                         "qproperty-alignment: AlignCenter; "  // 设置文本居中
                         "}";
    Label_MainTemperature->setStyleSheet(StyleSheet);
    Label_MainHumidness->setStyleSheet(StyleSheet);
    Label_MainPressure->setStyleSheet(StyleSheet);
    Label_MainLeak->setStyleSheet(StyleSheet);
    Label_MainDeviceState->setStyleSheet(StyleSheet);
    Label_MainReleaser1State->setStyleSheet(StyleSheet);
    Label_MainReleaser2State->setStyleSheet(StyleSheet);

    LabelVal_MainTemperature->setStyleSheet(StyleSheet);
    LabelVal_MainHumidness->setStyleSheet(StyleSheet);
    LabelVal_MainPressure->setStyleSheet(StyleSheet);
    LabelVal_MainLeak->setStyleSheet(StyleSheet);
    LabelVal_MainDeviceState->setStyleSheet(StyleSheet);
    LabelVal_MainReleaser1State->setStyleSheet(StyleSheet);
    LabelVal_MainReleaser2State->setStyleSheet(StyleSheet);
}

void InformationDisplay_MainControl::Slot_ShowMainControlInformation(const QString &text)
{
    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    Data_MainControl = text;

    Temp_Temperature = Data_MainControl.section('@', 1, 1);
    Temp_Humidity = Data_MainControl.section('@', 2, 2);
    Temp_Pressure = Data_MainControl.section('@', 3, 3);
    Temp_Leak01 = Data_MainControl.section('@', 4, 4);
    Temp_Leak02 = Data_MainControl.section('@', 5, 5);
    Temp_DeviceState = Data_MainControl.section('@', 6, 6);
    Temp_Releaser1State = Data_MainControl.section('@', 7, 7);
    Temp_Releaser2State = Data_MainControl.section('@', 8, 8);

    //温度数据处理
    LabelVal_MainTemperature->setText(Temp_Temperature);

    //湿度数据处理
    LabelVal_MainHumidness->setText(Temp_Humidity);

    //气压数据处理
    LabelVal_MainPressure->setText(Temp_Pressure);

    //泄露判断处理
    if(Temp_Leak01 == "01LEAK" || Temp_Leak02 == "02LEAK")
    {
        LabelVal_MainLeak->setText("泄露");
    }
    else
    {
        LabelVal_MainLeak->setText("未泄露");
    }

    //传感器
    if(Temp_DeviceState == "DEVOPEN")
    {
        LabelVal_MainDeviceState->setText("开");
        emit Signal_UpdateUnderWaterSensorStatus(true);
    }
    else if(Temp_DeviceState == "DEVCLOS")
    {
        LabelVal_MainDeviceState->setText("关");
        emit Signal_UpdateUnderWaterSensorStatus(false);
    }

    //释放器
    if(Temp_Releaser1State == "R1OPEN")
    {
        LabelVal_MainReleaser1State->setText("开");
        emit Signal_UpdateRelease1Status(true);
    }
    else if(Temp_Releaser1State == "R1CLOSE")
    {
        LabelVal_MainReleaser1State->setText("关");
        emit Signal_UpdateRelease1Status(false);
    }

    if(Temp_Releaser2State == "R2OPEN")
    {
        LabelVal_MainReleaser2State->setText("开");
        emit Signal_UpdateRelease2Status(true);
    }
    else if(Temp_Releaser2State == "R2CLOSE")
    {
        LabelVal_MainReleaser2State->setText("关");
        emit Signal_UpdateRelease2Status(false);
    }

    PrintMessage = CurrentTime + '\n' \
                   + "客户端：气压(hPa)：" + Data_Pressure + '\n' \
                   + "湿度(%)：" + Data_Humidity + '\n' \
                   + "温度(°c)：" + Data_Temperature + '\n' \
                   + "泄露：" + LabelVal_MainLeak->text();
    emit Signal_PrintMainControlInformation(PrintMessage);
}
