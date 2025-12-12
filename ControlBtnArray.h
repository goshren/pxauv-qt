#ifndef CONTROLBTNARRAY_H
#define CONTROLBTNARRAY_H

#include <QObject>
#include <QWidget>
#include <QObject>
#include <QGroupBox>
#include <QPushButton>

class ControlBtnArray : public QWidget
{
    Q_OBJECT
public:
    explicit ControlBtnArray(QWidget *parent = nullptr);
    QGroupBox *GroupBox_ControlBtnArray;
    QPushButton *Btn_Releaser1Switch;
    QPushButton *Btn_Releaser2Switch;
    QPushButton *Btn_CollectionSwitch;
    QPushButton *Btn_UnderWaterSensorSwitch;

    bool Releaser1Status = false;           //默认为关闭状态
    bool Releaser2Status = false;
    bool CollectionStatus = false;
    bool UnderWaterSensorStatus = false;

public slots:
    void Slot_Btn_Releaser1Switch_Clicked();
    void Slot_Btn_Releaser2Switch_Clicked();
    void Slot_Btn_CollectionSwitch_Clicked();
    void Slot_Btn_UnderWaterSensorSwitch_Clicked();

    void Slot_UpdateBtnControlArrayState(const bool TcpStatus);
    void Slot_UpdateReleaser1Status(const bool Status);             //更新释放器1状态
    void Slot_UpdateReleaser2Status(const bool Status);             //更新释放器2状态
    void Slot_UpdateCollectionStatus(const bool Status);            //更新采集舱状态
    void Slot_UpdateUnderWaterSensorStatus(const bool Status);      //更新传感器状态


signals:
    void Signal_Releaser1TurnOff(const QString &text);      //释放器1关闭信号
    void Signal_Releaser1TurnOn(const QString &text);       //释放器1开启信号
    void Signal_Releaser2TurnOff(const QString &text);      //释放器2关闭信号
    void Signal_Releaser2TurnOn(const QString &text);       //释放器2开启信号
    void Signal_CollectionTurnOff(const QString &text);     //采集舱关闭信号
    void Signal_CollectionTurnOn(const QString &text);      //采集舱开启信号
    void Signal_UnderWaterSensorTurnOff(const QString &text); //传感器关闭信号
    void Signal_UnderWaterSensorTurnOn(const QString &text);  //传感器开启信号

};

#endif // CONTROLBTNARRAY_H
