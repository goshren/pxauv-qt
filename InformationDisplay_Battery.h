#ifndef INFORMATIONDISPLAY_BATTERY_H
#define INFORMATIONDISPLAY_BATTERY_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QDateTime>

class InformationDisplay_Battery : public QWidget
{
    Q_OBJECT
public:
    explicit InformationDisplay_Battery(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DisplayBattery;
    QLabel *Label_Voltage;
    QLabel *LabelVal_Voltage;

    QString CurrentTime;                //用于存放当前时间
    QString PrintMessage;               //用于存放打印信息

private:
    int Data_Voltage;                   //用于存储电压数据(16进制)
    QString Str_Voltage;                //用于存储电压数据(字符串)

public slots:
    void Slot_ShowBatteryVoltage(const QString &text);

signals:
    void Signal_PrintBatteryVoltage(const QString &text);
};

#endif // INFORMATIONDISPLAY_BATTERY_H
