#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "TcpSocketLink.h"
#include "UartLink.h"
#include "MotorControl.h"
#include "DebugWindow.h"
#include "ControlBtnArray.h"
#include "InformationDisplay_CTD.h"
#include "InformationDisplay_Pose.h"
#include "InformationDisplay_MainControl.h"
#include "InformationDisplay_Battery.h"
#include "InformationDisplay_DVL.h"
#include "InformationDisplay_Releaser.h"
#include "InformationDisplay_CollectionStatus.h"
#include "InformationDisplay_Location.h"
#include "WindowDisplay_NavigationMap.h"
#include "UsblControlWindow.h" // 【新增】引入USBL窗口头文件

#include <QMenu> // 【新增】引入菜单类

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //软件标题
    setWindowTitle("AUV测试程序客户端");
    setWindowIcon(QIcon("qrc:/ico/res/favicon.ico"));

    //设定软件窗口大小
    setFixedSize(1845,810);

    //显示系统时间
    StatusBar_ShowCurrentTime = new QStatusBar(this);
    this->setStatusBar(StatusBar_ShowCurrentTime);
    QFont statusBarFont = StatusBar_ShowCurrentTime->font();
    statusBarFont.setPointSize(14);                             // 设置为14号字体
    StatusBar_ShowCurrentTime->setFont(statusBarFont);

    Timer_UpdateCurrentTime = new QTimer(this);
    Timer_UpdateCurrentTime->start(1000);

    connect(Timer_UpdateCurrentTime, &QTimer::timeout, this, &MainWindow::Slot_ShowCurrentTime);

    // 【新增】USBL 独立窗口初始化
    usblWindow = new UsblControlWindow(); // 实例化但不显示，等待菜单触发

    // 【新增】在菜单栏添加工具入口
    QMenu *toolsMenu = ui->menubar->addMenu("工具箱");
    QAction *actionUsbl = toolsMenu->addAction("USBL水声遥控");
    connect(actionUsbl, &QAction::triggered, this, &MainWindow::Slot_OpenUsblWindow);


    //创建TcpSocketLink对象
    TcpSocketLink *tcpSocketLink = new TcpSocketLink(this);
    tcpSocketLink->setGeometry(10,10,310,71);
    tcpSocketLink->show();

    //创建UartLink对象
    UartLink *uartLink = new UartLink(this);
    uartLink->setGeometry(10,81,310,68);
    uartLink->show();

    //创建InformationDisplay_CTD对象
    InformationDisplay_CTD *informationDisplay_CTD = new InformationDisplay_CTD(this);
    informationDisplay_CTD->setGeometry(10,159,880,139);
    informationDisplay_CTD->show();

    //创建InformationDisplay_Pose对象
    InformationDisplay_Pose *informationDisplay_Pose = new InformationDisplay_Pose(this);
    informationDisplay_Pose->setGeometry(1461,10,372,139);
    informationDisplay_Pose->show();

    //创建InformationDisplay_MainControl对象
    InformationDisplay_MainControl *informationDisplay_MainControl = new InformationDisplay_MainControl(this);
    informationDisplay_MainControl->setGeometry(330,10,968,139);
    informationDisplay_MainControl->show();

    //创建InformationDisplay_Battery对象
    InformationDisplay_Battery *informationDisplay_Battery = new InformationDisplay_Battery(this);
    informationDisplay_Battery->setGeometry(1308,10,143,139);
    informationDisplay_Battery->show();

    //创建InformationDisplay_DVL对象
    InformationDisplay_DVL *informationDisplay_DVL = new InformationDisplay_DVL(this);
    informationDisplay_DVL->setGeometry(900,159,933,139);
    informationDisplay_DVL->show();

    //创建InformationDisplay_Releaser对象
    //InformationDisplay_Releaser *informationDisplay_Releaser = new InformationDisplay_Releaser(this);
    //informationDisplay_Releaser->setGeometry(900,10,272,139);
    //informationDisplay_Releaser->show();

    //创建InformationDisplay_CollectionStatus对象
    InformationDisplay_CollectionStatus *informationDisplay_CollectionStatus = new InformationDisplay_CollectionStatus(this);
    informationDisplay_CollectionStatus->setGeometry(1384,308,227,139);
    informationDisplay_CollectionStatus->show();

    //创建InformationDisplay_Location对象
    InformationDisplay_Location *informationDisplay_Location = new InformationDisplay_Location(this);
    informationDisplay_Location->setGeometry(900,308,474,139);
    informationDisplay_Location->show();

    //创建InformationDisplay_controlBtnArray对象
    ControlBtnArray *controlBtnArray = new ControlBtnArray(this);
    controlBtnArray->setGeometry(1621,308,272,139);
    controlBtnArray->show();

    //创建MotorControl对象
    MotorControl *motorControl = new MotorControl(this);
    motorControl->setGeometry(1488,457,345,311);
    motorControl->show();

    //创建DebugWindow对象
    DebugWindow *debugWindow = new DebugWindow(this);
    debugWindow->setGeometry(900,457,578,311);
    debugWindow->show();

    //创建WindowDisplay_NavigationMap对象
    WindowDisplay_NavigationMap *windowDisplay_NavigationMap = new WindowDisplay_NavigationMap(this);
    windowDisplay_NavigationMap->setGeometry(10,299,880,469);
    windowDisplay_NavigationMap->show();

    //连接信号和槽函数----采集舱状态
    connect(controlBtnArray, &ControlBtnArray::Signal_CollectionTurnOn, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_CollectionTurnOff, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    
    // 【修改】增加采集舱到串口的连接，解决只能TCP发送的问题
    connect(controlBtnArray, &ControlBtnArray::Signal_CollectionTurnOn, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_CollectionTurnOff, uartLink, &UartLink::Slot_UartSendMsgHandle);

    connect(tcpSocketLink, &TcpSocketLink::Signal_CollectionData, \
            informationDisplay_CollectionStatus, &InformationDisplay_CollectionStatus::Slot_ShowCollectionStatus);

    connect(informationDisplay_CollectionStatus, &InformationDisplay_CollectionStatus::Signal_PrintCollectionStatus, \
            debugWindow, &DebugWindow::Slot_PrintToDebug);

    connect(informationDisplay_CollectionStatus, &InformationDisplay_CollectionStatus::Signal_UpdateCollectionStatus, \
            controlBtnArray, &ControlBtnArray::Slot_UpdateCollectionStatus);

    //连接信号和槽函数----电池信息
    connect(tcpSocketLink, &TcpSocketLink::Signal_BatteryData, \
            informationDisplay_Battery, &InformationDisplay_Battery::Slot_ShowBatteryVoltage);

    connect(informationDisplay_Battery, &InformationDisplay_Battery::Signal_PrintBatteryVoltage, \
            debugWindow, &DebugWindow::Slot_PrintToDebug);

    //连接信号和槽函数----温盐深信息
    connect(tcpSocketLink, &TcpSocketLink::Signal_CtdData, \
            informationDisplay_CTD, &InformationDisplay_CTD::Slot_ShowCtdInformation);

    connect(informationDisplay_CTD, &InformationDisplay_CTD::Signal_PrintCtdInformation, \
            debugWindow, &DebugWindow::Slot_PrintToDebug);

    //连接信号和槽函数----DVL信息
    connect(tcpSocketLink, &TcpSocketLink::Signal_DvlData, \
            informationDisplay_DVL, &InformationDisplay_DVL::Slot_ShowDvlInformation);

    connect(informationDisplay_DVL, &InformationDisplay_DVL::Signal_PrintDvlInformation, \
            debugWindow, &DebugWindow::Slot_PrintToDebug);

    //连接信号和槽函数----DVL的Roll、Pitch、Heading数据传送到Pose

    connect(informationDisplay_DVL, &InformationDisplay_DVL::Signal_LabelVal_Roll, \
            informationDisplay_Pose, &InformationDisplay_Pose::Slot_ShowRollInformation);

    connect(informationDisplay_DVL, &InformationDisplay_DVL::Signal_LabelVal_Pitch, \
            informationDisplay_Pose, &InformationDisplay_Pose::Slot_ShowPitchInformation);

    connect(informationDisplay_DVL, &InformationDisplay_DVL::Signal_LabelVal_Heading, \
            informationDisplay_Pose, &InformationDisplay_Pose::Slot_ShowHeadingInformation);

    //连接信号和槽函数----位置信息
    connect(tcpSocketLink, &TcpSocketLink::Signal_LocationData, \
            informationDisplay_Location, &InformationDisplay_Location::Slot_ShowLocationInformation);

    connect(uartLink, &UartLink::Signal_LocationData, \
            informationDisplay_Location, &InformationDisplay_Location::Slot_ShowLocationInformation);

    connect(informationDisplay_Location, &InformationDisplay_Location::Signal_PrintLocationInformation, \
            debugWindow, &DebugWindow::Slot_PrintToDebug);

    //连接信号和槽函数----主控舱状态信息
    connect(tcpSocketLink, &TcpSocketLink::Signal_MainControlData, \
            informationDisplay_MainControl, &InformationDisplay_MainControl::Slot_ShowMainControlInformation);

    connect(informationDisplay_MainControl, &InformationDisplay_MainControl::Signal_PrintMainControlInformation, \
            debugWindow, &DebugWindow::Slot_PrintToDebug);

    //连接信号和槽函数---释放器
    connect(controlBtnArray, &ControlBtnArray::Signal_Releaser1TurnOn, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_Releaser1TurnOff, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_Releaser2TurnOn, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_Releaser2TurnOff, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    
    // 【修改】增加释放器到串口的连接
    connect(controlBtnArray, &ControlBtnArray::Signal_Releaser1TurnOn, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_Releaser1TurnOff, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_Releaser2TurnOn, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_Releaser2TurnOff, uartLink, &UartLink::Slot_UartSendMsgHandle);
    
    connect(informationDisplay_MainControl, &InformationDisplay_MainControl::Signal_UpdateRelease1Status, controlBtnArray, &ControlBtnArray::Slot_UpdateReleaser1Status);
    connect(informationDisplay_MainControl, &InformationDisplay_MainControl::Signal_UpdateRelease2Status, controlBtnArray, &ControlBtnArray::Slot_UpdateReleaser2Status);

    //连接信号和槽函数---传感器开关
    connect(controlBtnArray, &ControlBtnArray::Signal_UnderWaterSensorTurnOn, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_UnderWaterSensorTurnOff, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    
    // 【修改】增加传感器到串口的连接
    connect(controlBtnArray, &ControlBtnArray::Signal_UnderWaterSensorTurnOn, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(controlBtnArray, &ControlBtnArray::Signal_UnderWaterSensorTurnOff, uartLink, &UartLink::Slot_UartSendMsgHandle);
    
    connect(informationDisplay_MainControl, &InformationDisplay_MainControl::Signal_UpdateUnderWaterSensorStatus, controlBtnArray, &ControlBtnArray::Slot_UpdateUnderWaterSensorStatus);

    //连接信号和槽函数---地图显示
    connect(informationDisplay_Location, &InformationDisplay_Location::Signal_LocationString, \
            windowDisplay_NavigationMap, &WindowDisplay_NavigationMap::Slot_LocationString);

    //连接信号和槽函数----运动控制
    connect(motorControl, &MotorControl::Signal_Up, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Down, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Left, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Right, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Forward, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Backward, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Stop, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Check, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle);

    connect(motorControl, &MotorControl::Signal_Up, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Down, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Left, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Right, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Forward, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Backward, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Stop, uartLink, &UartLink::Slot_UartSendMsgHandle);
    connect(motorControl, &MotorControl::Signal_Check, uartLink, &UartLink::Slot_UartSendMsgHandle);

    //连接信号和槽函数----TCP的连接、断开、数据接收显示
    connect(tcpSocketLink, &TcpSocketLink::Signal_TcpConnected, debugWindow, &DebugWindow::Slot_PrintToDebug);
    connect(tcpSocketLink, &TcpSocketLink::Signal_TcpDisconnected, debugWindow, &DebugWindow::Slot_PrintToDebug);
    connect(tcpSocketLink, &TcpSocketLink::Signal_TcpRecvMsg, debugWindow, &DebugWindow::Slot_PrintToDebug);
    connect(tcpSocketLink, &TcpSocketLink::Signal_TcpSendMsg, debugWindow, &DebugWindow::Slot_PrintToDebug);

    //连接信号和槽函数----调试窗发送信息按钮
    connect(debugWindow, &DebugWindow::Signal_TcpSendMsg, tcpSocketLink, &TcpSocketLink::Slot_TcpSendMsgHandle); 
    connect(debugWindow, &DebugWindow::Signal_UartSendMsg, uartLink, &UartLink::Slot_UartSendMsgHandle);

    //连接信号和槽函数----串口
    connect(uartLink, &UartLink::Signal_UartConnected, debugWindow, &DebugWindow::Slot_PrintToDebug);
    connect(uartLink, &UartLink::Signal_UartDisconnected, debugWindow, &DebugWindow::Slot_PrintToDebug);
    connect(uartLink, &UartLink::Signal_UartErrorConnect, debugWindow, &DebugWindow::Slot_PrintToDebug);
    connect(uartLink, &UartLink::Signal_UartRecvMsg, debugWindow, &DebugWindow::Slot_PrintToDebug);
    connect(uartLink, &UartLink::Signal_UartSendMsg, debugWindow, &DebugWindow::Slot_PrintToDebug);

    //连接信号和槽函数----按钮状态更新
    connect(tcpSocketLink, &TcpSocketLink::Signal_TcpStatus, this, &MainWindow::Slot_TcpStatus);
    connect(uartLink, &UartLink::Signal_UartStatus, this, &MainWindow::Slot_UartStatus);
    connect(this, &MainWindow::Signal_UpdateBtnStatus, motorControl, &MotorControl::Slot_UpdateBtnMotorStatus);
    connect(this, &MainWindow::Signal_UpdateBtnStatus, debugWindow, &DebugWindow::Slot_UpdateBtnStatus);
    connect(this, &MainWindow::Signal_UpdateRabtnTcpStatus, uartLink, &UartLink::Slot_UpdateRabtnTcpStatus);
    connect(this, &MainWindow::Signal_UpdateRabtnUartStatus, uartLink, &UartLink::Slot_UpdateRabtnUartStatus);
    connect(uartLink, &UartLink::Signal_RabtnTcpStatus, debugWindow, &DebugWindow::Slot_RabtnTcpStatus);
    connect(uartLink, &UartLink::Signal_RabtnTcpStatus, tcpSocketLink, &TcpSocketLink::Slot_RabtnTcpStatus);
    connect(uartLink, &UartLink::Signal_RabtnUartStatus, debugWindow, &DebugWindow::Slot_RabtnUartStatus);
    
    // 【修改】将控制按钮的启用逻辑改为监听 MainWindow 的汇总状态
    // 原代码: connect(tcpSocketLink, &TcpSocketLink::Signal_TcpStatus, controlBtnArray, &ControlBtnArray::Slot_UpdateBtnControlArrayState);
    connect(this, &MainWindow::Signal_UpdateBtnStatus, controlBtnArray, &ControlBtnArray::Slot_UpdateBtnControlArrayState);
}

MainWindow::~MainWindow()
{
    // 【新增】安全释放 USBL 窗口
    if(usblWindow) {
        usblWindow->close();
        delete usblWindow;
    }
    delete ui;
}

// 【新增】打开 USBL 窗口的槽函数
void MainWindow::Slot_OpenUsblWindow()
{
    if(usblWindow) {
        usblWindow->show();
        usblWindow->raise();
        usblWindow->activateWindow();
    }
}

void MainWindow::Slot_ShowCurrentTime()
{
    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    StatusBar_ShowCurrentTime->showMessage("当前时间: " + CurrentTime);  // 设置标签内容
}

void MainWindow::Slot_TcpStatus(const bool status)
{
    TcpStatus = status;
    emit Signal_UpdateRabtnTcpStatus(TcpStatus);
    emit Signal_UpdateBtnControlArray(TcpStatus);

    if(true == TcpStatus || UartStatus)
    {
        IsUpdateBtnStatus = true;
        emit Signal_UpdateBtnStatus(IsUpdateBtnStatus);
    }
    else
    {
        IsUpdateBtnStatus = false;
        emit Signal_UpdateBtnStatus(IsUpdateBtnStatus);
    }
}

void MainWindow::Slot_UartStatus(const bool status)
{
    UartStatus = status;
    emit Signal_UpdateRabtnUartStatus(UartStatus);

    if(true == TcpStatus || UartStatus)
    {
        IsUpdateBtnStatus = true;
        emit Signal_UpdateBtnStatus(IsUpdateBtnStatus);
    }
    else
    {
        IsUpdateBtnStatus = false;
        emit Signal_UpdateBtnStatus(IsUpdateBtnStatus);
    }
}