#ifndef TCPSOCKETLINK_H
#define TCPSOCKETLINK_H

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QTcpSocket>
#include <QDateTime>
#include <QWidget>

class TcpSocketLink : public QWidget
{
    Q_OBJECT
public:
    explicit TcpSocketLink(QWidget *parent = nullptr);

    QPushButton *Btn_Connect;           //连接按键
    QPushButton *Btn_Disconnect;        //断开按键
    QLabel *Label_IP;                   //显示字符串"IP地址:"
    QLabel *Label_Port;                 //显示字符串"端口号:"
    QLineEdit *LineEdit_IP;             //编辑IP地址
    QLineEdit *LineEdit_Port;           //编辑端口号
    QTcpSocket *TcpSocket;              //TCP连接
    bool TcpStatus;                     //TCP连接状态
    bool Rabtn_TcpStatus;               //多选一“TCP”的被选状态

    QString CurrentTime;                //用于存放当前时间
    QString PrintMessage;               //用于存放打印信息


public slots:
    void Slot_TcpSendMsgHandle(const QString &TcpSendMsg);     //处理TCP发送数据的槽函数
    void Slot_RabtnTcpStatus(const bool status);               //多选一“TCP”被选状态

private slots:
    void Slot_Btn_Connect_Clicked();                        //"连接" 按键点击槽函数
    void Slot_Btn_Disconnect_Clicked();                     //"断开" 按键点击槽函数
    void Slot_TcpConnectHandle();                           //处理TCP连接之后的槽函数
    void Slot_TcpDisconnectHandle();                        //处理TCP断开之后的槽函数
    void Slot_TcpRecvMsgHandle();                           //处理TCP接收数据的槽函数

signals:
    void Signal_TcpConnected(const QString &text);      //自定义信号，用于通知连接成功
    void Signal_TcpDisconnected(const QString &text);   //自定义信号，用于通知断开连接
    void Signal_TcpRecvMsg(const QString &text);        //自定义信号，用于发出接收到的信息显示
    void Signal_TcpSendMsg(const QString &text);        //自定义信号，用于发出发送的信息显示
    void Signal_TcpStatus(const bool TcpStatus);        //自定义信号，用于发送TCP状态

    void Signal_CollectionData(const QString &text);    //用于发出显示采集舱的状态信息
    void Signal_BatteryData(const QString &text);       //用于发出显示电池的状态信息
    void Signal_CtdData(const QString &text);           //用于发出温盐深的数据信息
    void Signal_DvlData(const QString &text);           //用于发出Dvl的数据信息
    void Signal_LocationData(const QString &text);      //用于发出位置信息的数据信息
    void Signal_MainControlData(const QString &text);   //用于发出主控舱的数据信息
    void Signal_ReleaserData(const QString &text);      //用于发出释放器的数据信息

private:
    QString Temp_CollectionData;        //用于存放未经处理的采集舱的临时数据
    QString Temp_BatteryData;           //用于存放未经处理的电池电压的临时数据
    QString Temp_CtdData;               //用于存放未经处理的温盐深的临时数据
    QString Temp_DvlData;               //用于存放未经处理的DVL的临时数据
    QString Temp_LocationData;          //用于存放未经处理的位置信息的临时数据
    QString Temp_MainControlData;       //用于存放未经处理的主控舱的临时数据
    QString Temp_ReleaserData;          //用于存放未经处理的释放器的临时数据
    QString Temp_UnderWaterSensorData;  //用于存放未经处理的传感器开关的临时数据

};

#endif // TCPSOCKETLINK_H
