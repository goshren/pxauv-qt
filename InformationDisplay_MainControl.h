#ifndef INFORMATIONDISPLAY_MAINCONTROL_H
#define INFORMATIONDISPLAY_MAINCONTROL_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QDateTime>

class InformationDisplay_MainControl : public QWidget
{
    Q_OBJECT
public:
    explicit InformationDisplay_MainControl(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DisplayMainControl;
    QLabel *Label_MainTemperature;
    QLabel *Label_MainHumidness;
    QLabel *Label_MainPressure;
    QLabel *Label_MainLeak;
    QLabel *Label_MainDeviceState;
    QLabel *Label_MainReleaser1State;
    QLabel *Label_MainReleaser2State;

    QLabel *LabelVal_MainTemperature;
    QLabel *LabelVal_MainHumidness;
    QLabel *LabelVal_MainPressure;
    QLabel *LabelVal_MainLeak;
    QLabel *LabelVal_MainDeviceState;
    QLabel *LabelVal_MainReleaser1State;
    QLabel *LabelVal_MainReleaser2State;

    QString CurrentTime;                //用于存放当前时间
    QString PrintMessage;               //用于存放打印信息

public slots:
    void Slot_ShowMainControlInformation(const QString &text);

signals:
    void Signal_PrintMainControlInformation(const QString &text);
    void Signal_UpdateRelease1Status(const bool Releaser1Status);
    void Signal_UpdateRelease2Status(const bool Releaser2Status);
    void Signal_UpdateUnderWaterSensorStatus(const bool UnderWaterSensorStatus);

private:
    QString Data_MainControl;                   //用于接收未经处理的临时位置信息数据
    QString Temp_Temperature;            //临时温度数据
    QString Temp_Humidity;               //临时湿度数据
    QString Temp_Pressure;               //临时压力数据
    QString Temp_Leak01;
    QString Temp_Leak02;
    QString Temp_DeviceState;
    QString Temp_Releaser1State;
    QString Temp_Releaser2State;

    QString Data_Pressure;               //压力数据
    QString Data_Humidity;               //湿度数据
    QString Data_Temperature;            //温度数据
    QString Data_Leak;                  //泄露数据

};

#endif // INFORMATIONDISPLAY_MAINCONTROL_H
