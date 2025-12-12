#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QWidget>
#include <QObject>
#include <QGroupBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QDateTime>
#include <QSaveFile>
#include <QFileDialog>

class DebugWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DebugWindow(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DebugWindow;        //用于存放信息调试相关的控件
    QTextEdit *TextEdit_ShowText;           //用于显示信息
    QPushButton *Btn_SendMsg;               //"发送"按钮
    QPushButton *Btn_SaveMsg;               //"保存信息"按钮
    QLineEdit *LineEdit_DebugMsg;           //用于编辑发送的信息

    bool RabtnTcpStatus;                    //UartLink 中 Tcp被选状态
    bool RabtnUartStatus;                   //UartLink 中 Uart被选状态

private:
    QString CurrentTime;                    //用于存放当前时间
    QString PrintMessage;                   //用于存放打印信息
    QString LineEditText;                   //行编辑器的内容
    QString FileName;                       //用于存放要保存的文件名字

public slots:
    void Slot_PrintToDebug(const QString &text);
    void Slot_Btn_SendMsg_Clicked();                //发送按钮槽函数
    void Slot_Btn_SaveMsg_Clicked();                //保存按钮槽函数

    void Slot_UpdateBtnStatus(const bool status);   //更新按钮状态槽函数
    void Slot_RabtnTcpStatus(const bool status);    //UartLink 中 Tcp被选状态槽函数
    void Slot_RabtnUartStatus(const bool status);   //UartLink 中 Uart被选状态槽函数

signals:
    void Signal_TcpSendMsg(const QString &text);
    void Signal_UartSendMsg(const QString &text);
};

#endif // DEBUGWINDOW_H
