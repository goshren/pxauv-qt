#include "UartLink.h"

UartLink::UartLink(QWidget *parent) : QWidget(parent)
{
    //定义Uart控件
    Btn_OpenUart = new QPushButton("打开串口", this);
    Btn_CloseUart = new QPushButton("关闭串口", this);
    Rabtn_SelTcp = new QRadioButton("TCP", this);
    Rabtn_SelUart= new QRadioButton("UART", this);
    ComboBox_SelUartPort = new QComboBox(this);
    ComboBox_SelBaudrate = new QComboBox(this);

    //设置参数以及初始化
    Btn_OpenUart->setEnabled(true);
    Btn_CloseUart->setEnabled(false);
    Rabtn_SelTcp->setCheckable(false);
    Rabtn_SelUart->setCheckable(false);

    Rabtn_UartStatus = false;

    //定义大小和位置
    Btn_OpenUart->setGeometry(106,3,99,32);
    Btn_CloseUart->setGeometry(211,3,99,32);
    Rabtn_SelTcp->setGeometry(1,6,99,25);
    Rabtn_SelUart->setGeometry(1,43,99,25);
    ComboBox_SelUartPort->setGeometry(106,43,99,25);
    ComboBox_SelBaudrate->setGeometry(211,43,99,25);

    //定义数传电台的Uart连接
    Uart_DataRadio = new QSerialPort(this);

    //availablePorts 是返回当前系统所有能用的串口,返回值是一个容器,QList 是容器类,在此的容器里边装的是 QSerialPortInfo
    QList<QSerialPortInfo> SerialPortsInfo = QSerialPortInfo::availablePorts();
    for(int i=0; i<SerialPortsInfo.size(); i++)
        ComboBox_SelUartPort->addItem(SerialPortsInfo[i].portName()); //addItem 加入数据到下拉菜单里
    QStringList baudrate = {"4800","9600","57600","115200"};
    ComboBox_SelBaudrate->addItems(baudrate);

    //连接信号与槽函数
    connect(Btn_OpenUart, &QPushButton::clicked, this, &UartLink::Slot_Btn_OpenUart_Clicked);
    connect(Btn_CloseUart, &QPushButton::clicked, this, &UartLink::Slot_Btn_CloseUart_Clicked);
    connect(Uart_DataRadio, &QSerialPort::readyRead, this, &UartLink::Slot_UartRecvMsgHandle);
    connect(Rabtn_SelTcp, &QPushButton::toggled, this, &UartLink::Slot_RabtnTcpToggled);
    connect(Rabtn_SelUart, &QPushButton::toggled, this, &UartLink::Slot_RabtnUartToggled);
}

void UartLink::Slot_Btn_OpenUart_Clicked()
{
    Uart_DataRadio->setPortName(ComboBox_SelUartPort->currentText());
    Uart_DataRadio->setBaudRate(ComboBox_SelBaudrate->currentText().toInt());
    Uart_DataRadio->setDataBits(QSerialPort::Data8);            //8位数据
    Uart_DataRadio->setParity(QSerialPort::NoParity);           //无校验
    Uart_DataRadio->setStopBits(QSerialPort::OneStop);          //一个停止位

    if(Uart_DataRadio->open(QIODevice::ReadWrite))
    {
        CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        emit Signal_UartConnected(CurrentTime + '\n' + "系统提示：" + "串口已打开！\n");

        UartStatus = true;
        emit Signal_UartStatus(UartStatus);

        Btn_OpenUart->setEnabled(false);
        Btn_CloseUart->setEnabled(true);
        Rabtn_SelUart->setCheckable(true);
    }
    else
    {
        CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        emit Signal_UartErrorConnect(CurrentTime + '\n' + "系统提示：" + "串口打开失败！错误信息：" + Uart_DataRadio->errorString() + '\n');

        UartStatus = false;
        Rabtn_UartStatus = false;
        emit Signal_UartStatus(UartStatus);

        Btn_OpenUart->setEnabled(true);
        Btn_CloseUart->setEnabled(false);
        Rabtn_SelUart->setCheckable(false);
    }
}

void UartLink::Slot_Btn_CloseUart_Clicked()
{
    if (Uart_DataRadio->isOpen())
    {
        Uart_DataRadio->close();
        CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        emit Signal_UartDisconnected(CurrentTime + '\n' + "系统提示：" + "串口已关闭！\n");

        UartStatus = false;
        Rabtn_UartStatus = false;
        emit Signal_UartStatus(UartStatus);

        Btn_OpenUart->setEnabled(true);
        Btn_CloseUart->setEnabled(false);
        Rabtn_SelUart->setCheckable(false);
    }
}

void UartLink::Slot_UartRecvMsgHandle()
{
    QByteArray UartRecvData = Uart_DataRadio->readAll();
    QByteArray UartRecvData16 = UartRecvData.toHex().toUpper();
    QString UartRecvMsg = (QString)UartRecvData;
    recvDataLength = UartRecvData.size();

    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    emit Signal_UartRecvMsg(CurrentTime + '\n' + ComboBox_SelUartPort->currentText() + "：" + UartRecvMsg + '\n');

    /*以下代码解析位置信息*/
        if(UartRecvMsg[0] == '?' && recvDataLength == 23)
        {
            Temp_LocationData = UartRecvMsg;
            emit Signal_LocationData(Temp_LocationData);
        }
}

void UartLink::Slot_UartSendMsgHandle(const QString &UartSendMsg)
{
   if(Rabtn_SelUart->isChecked())
   {
        CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        Uart_DataRadio->write(UartSendMsg.toUtf8());

        PrintMessage = CurrentTime + '\n'+ "客户端(UART)发送：" + UartSendMsg + '\n';
        emit Signal_UartSendMsg(PrintMessage);
    }
}

void UartLink::Slot_UpdateRabtnTcpStatus(const bool TcpStatus)
{
    if(TcpStatus == true)
    {
        Rabtn_SelTcp->setCheckable(true);
    }
    else
    {
        Rabtn_SelTcp->setCheckable(false);
    }
}

void UartLink::Slot_RabtnTcpToggled()
{
    emit Signal_RabtnTcpStatus(Rabtn_SelTcp->isChecked());
}

void UartLink::Slot_UpdateRabtnUartStatus(const bool UartStatus)
{
    if(UartStatus == true)
    {
        Rabtn_SelUart->setCheckable(true);
    }
    else
    {
        Rabtn_SelUart->setCheckable(false);
    }
}

void UartLink::Slot_RabtnUartToggled()
{
    emit Signal_RabtnUartStatus(Rabtn_SelUart->isChecked());
}
