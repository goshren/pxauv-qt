#ifndef UARTLINK_H
#define UARTLINK_H

#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QDateTime>
#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

class UartLink : public QWidget
{
    Q_OBJECT
public:
    explicit UartLink(QWidget *parent = nullptr);

    QPushButton *Btn_OpenUart;          //打开串口按键
    QPushButton *Btn_CloseUart;         //关闭串口按键
    QRadioButton *Rabtn_SelTcp;         //多选一按钮 选择Tcp
    QRadioButton *Rabtn_SelUart;        //多选一按钮 选择Uart
    QComboBox *ComboBox_SelUartPort;    //下拉框 选择Uart端口
    QComboBox *ComboBox_SelBaudrate;    //下拉框 选择波特率
    bool UartStatus;                    //Uart连接状态
    bool Rabtn_UartStatus;               //多选一“UART”被选状态

    QString CurrentTime;                //用于存放当前时间
    QString PrintMessage;               //用于存放打印信息

    int recvDataLength;                 //接收的数据长度

    QString Temp_LocationData;          //用于存放Uart接收到的位置信息


public slots:
    void Slot_UpdateRabtnTcpStatus(const bool TcpStatus);       //更新TCP选择状态
    void Slot_UpdateRabtnUartStatus(const bool UartStatus);     //更新Uart选择状态
    void Slot_UartSendMsgHandle(const QString &UartSendMsg);    //处理Uart发送数据的槽函数

private:
    QSerialPort *Uart_DataRadio;        //定义数传电台的串口通信

private slots:
    void Slot_Btn_OpenUart_Clicked();   //"打开串口" 按键点击槽函数
    void Slot_Btn_CloseUart_Clicked();  //"关闭串口" 按键点击槽函数
    void Slot_UartRecvMsgHandle();      //串口接收数据处理槽函数

    void Slot_RabtnTcpToggled();        //多选一"TCP"状态翻转
    void Slot_RabtnUartToggled();       //多选一"Uart"状态翻转

signals:
    void Signal_UartConnected(const QString &text);     //自定义信号，用于发出Uart连接成功的显示
    void Signal_UartErrorConnect(const QString &text);  //自定义信号，用于发出Uart连接错误的显示
    void Signal_UartDisconnected(const QString &text);  //自定义信号，用于发出Uart断开连接的显示
    void Signal_UartRecvMsg(const QString &text);       //自定义信号，用于发出接收到的信息显示
    void Signal_UartSendMsg(const QString &text);       //自定义信号，用于发出发送的信息显示
    void Signal_UartStatus(const bool UartStatus);      //自定义信号，用于发送Uart状态

    void Signal_RabtnTcpStatus(const bool RabtnTcpStatus);      //发送Rabtn Tcp的选中状态
    void Signal_RabtnUartStatus(const bool RabtnUartStatus);    //发送Rabtn Uart的选中状态

    void Signal_LocationData(const QString &text);      //用于发出位置信息的数据信息

};

#endif // UARTLINK_H
