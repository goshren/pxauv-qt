#include "TcpSocketLink.h"


TcpSocketLink::TcpSocketLink(QWidget *parent) : QWidget(parent)
{
    //定义Tcp控件
    Label_IP = new QLabel("IP地址:", this);
    Label_Port = new QLabel("端口号:", this);
    LineEdit_IP = new QLineEdit("192.168.2.1", this);
    LineEdit_Port = new QLineEdit("6666", this);
    Btn_Connect = new QPushButton("连接", this);
    Btn_Disconnect = new QPushButton("断开", this);

    //设置参数和初始化
    Btn_Disconnect->setEnabled(false);
    Rabtn_TcpStatus = false;

    //定义大小和位置
    Label_IP->setGeometry(1,1,64,32);
    Label_Port->setGeometry(1,39,64,32);
    LineEdit_IP->setGeometry(71,4,153,25);
    LineEdit_Port->setGeometry(71,42,153,25);
    Btn_Connect->setGeometry(230,1,80,32);
    Btn_Disconnect->setGeometry(230,39,80,32);

    //定义TCP连接
    TcpSocket = new QTcpSocket(this);

    //连接 "连接" 和 "断开" 按键的信号和槽函数
    connect(Btn_Connect, &QPushButton::clicked,this,&TcpSocketLink::Slot_Btn_Connect_Clicked);
    connect(Btn_Disconnect, &QPushButton::clicked,this,&TcpSocketLink::Slot_Btn_Disconnect_Clicked);
    connect(TcpSocket, &QTcpSocket::connected,this,&TcpSocketLink::Slot_TcpConnectHandle);
    connect(TcpSocket, &QTcpSocket::disconnected,this,&TcpSocketLink::Slot_TcpDisconnectHandle);
    connect(TcpSocket,&QTcpSocket::readyRead,this,&TcpSocketLink::Slot_TcpRecvMsgHandle);

}

void TcpSocketLink::Slot_Btn_Connect_Clicked()
{
    TcpSocket->connectToHost(LineEdit_IP->text(),LineEdit_Port->text().toUShort());
}

void TcpSocketLink::Slot_Btn_Disconnect_Clicked()
{
    TcpSocket->disconnectFromHost();
}

void TcpSocketLink::Slot_TcpConnectHandle()
{
    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    PrintMessage = CurrentTime + '\n'+ "系统提示：" + "与服务器成功连接！" + '\n';
    emit Signal_TcpConnected(PrintMessage);

    //更新按钮
    Btn_Connect->setEnabled(false);
    Btn_Disconnect->setEnabled(true);

    TcpStatus = true;
    emit Signal_TcpStatus(TcpStatus);
}

void TcpSocketLink::Slot_TcpDisconnectHandle()
{
    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    PrintMessage = CurrentTime + '\n'+ "系统提示：" + "与服务器断开连接！" + '\n';
    emit Signal_TcpDisconnected(PrintMessage);

    //更新按钮
    Btn_Connect->setEnabled(true);
    Btn_Disconnect->setEnabled(false);

    TcpStatus = false;
    Rabtn_TcpStatus = false;
    emit Signal_TcpStatus(TcpStatus);
}

void TcpSocketLink::Slot_RabtnTcpStatus(const bool status)
{
    Rabtn_TcpStatus = status;
}

void TcpSocketLink::Slot_TcpSendMsgHandle(const QString &TcpSendMsg)
{
    if(Rabtn_TcpStatus == true)
    {
        CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        TcpSocket->write(TcpSendMsg.toUtf8());
        PrintMessage = CurrentTime + '\n'+ "客户端(TCP)发送：" + TcpSendMsg + '\n';
        emit Signal_TcpSendMsg(PrintMessage);
    }
}

void TcpSocketLink::Slot_TcpRecvMsgHandle()
{
    int n = TcpSocket->bytesAvailable(); //接收到的字节数
    QByteArray TcpRecvData = TcpSocket->read(n);
    QByteArray TcpRecvData16 = TcpRecvData.toHex().toUpper();
    QString TcpRecvMsg = (QString)TcpRecvData;

    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString TcpSignalMassage = CurrentTime + '\n'+ "客户端(TCP)接收：" + TcpRecvMsg + '\n';
    emit Signal_TcpRecvMsg(TcpSignalMassage);

    int datalen = TcpRecvMsg.size();

/*以下代码解析采集舱状态    new  */
    if(TcpRecvMsg[0] == '=' && TcpRecvMsg[datalen-1] == '=')
    {
        Temp_CollectionData = TcpRecvMsg;
        emit Signal_CollectionData(Temp_CollectionData);
    }

/*以下代码解析电压的数据    new  */
    else if(TcpRecvMsg[0] == '-' && TcpRecvMsg[datalen-1] == '-')
    {
        Temp_BatteryData = TcpRecvMsg;
        emit Signal_BatteryData(Temp_BatteryData);
    }

/*以下代码解析温盐深的数据  new */
    else if(TcpRecvMsg[0] == '#' && TcpRecvMsg[datalen-1] == '#')
    {
        Temp_CtdData = TcpRecvMsg;
        emit Signal_CtdData(Temp_CtdData);
    }

/*以下代码解析DVL的数据    new   */
    else if(TcpRecvMsg[0] == '*' && TcpRecvMsg[datalen-1] == '*')
    {
        Temp_DvlData = TcpRecvMsg;
        emit Signal_DvlData(Temp_DvlData);
    }


/*以下代码解析位置信息     new    */
    else if(TcpRecvMsg[0] == '/' && TcpRecvMsg[datalen-1] == '/')
    {
        Temp_LocationData = TcpRecvMsg;
        emit Signal_LocationData(Temp_LocationData);
    }

/*以下代码解析主控舱状态   new   */
    else if(TcpRecvMsg[0] == '@' && TcpRecvMsg[datalen-1] == '@')
    {
        Temp_MainControlData = TcpRecvMsg;
        emit Signal_MainControlData(Temp_MainControlData);
    }

    TcpRecvData.clear();
    TcpRecvData16.clear();
}
