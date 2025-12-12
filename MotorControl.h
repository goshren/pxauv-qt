#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>

class MotorControl : public QWidget
{
    Q_OBJECT
public:
    explicit MotorControl(QWidget *parent = nullptr);
    QGroupBox *GroupBox_MotorControl;        //用于存放运动控制相关的控件
    QPushButton *Btn_MCUp;              //上浮按键
    QPushButton *Btn_MCDown;            //下沉按键
    QPushButton *Btn_MCLeft;            //左转按键
    QPushButton *Btn_MCRight;           //右转按键
    QPushButton *Btn_MCForward;         //前进按键
    QPushButton *Btn_MCBackward;        //后退按键
    QPushButton *Btn_MCStop;            //停止按键
    QPushButton *Btn_MCCheck;           //检查按键
    QSpinBox *Spinbox_Arg;              //档位参数

public slots:
    void Slot_Btn_MCUp_Clicked();
    void Slot_Btn_MCDown_Clicked();
    void Slot_Btn_MCLeft_Clicked();
    void Slot_Btn_MCRight_Clicked();
    void Slot_Btn_MCForward_Clicked();
    void Slot_Btn_MCBackward_Clicked();
    void Slot_Btn_MCStop_Clicked();
    void Slot_Btn_MCCheck_Clicked();

    void Slot_UpdateBtnMotorStatus(const bool status);         //自定义信号，用于更新按钮状态
    
signals:
    void Signal_Up(const QString &SendMsg);
    void Signal_Down(const QString &SendMsg);
    void Signal_Left(const QString &SendMsg);
    void Signal_Right(const QString &SendMsg);
    void Signal_Forward(const QString &SendMsg);
    void Signal_Backward(const QString &SendMsg);
    void Signal_Stop(const QString &SendMsg);
    void Signal_Check(const QString &SendMsg);
    
};

#endif // MOTORCONTROL_H
