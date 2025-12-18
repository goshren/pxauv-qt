#include "UsblControlWindow.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtMath> 

const double EARTH_RADIUS = 6378137.0;

UsblControlWindow::UsblControlWindow(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("无缆水下机器人控制软件");
    this->setFixedSize(1350, 900);

    UsblSerial = new QSerialPort(this);
    GpsSerial = new QSerialPort(this);
    
    m_baseLatitude = 0.0;
    m_baseLongitude = 0.0;
    m_hasGpsFix = false;
    
    InitUi();

    m_queryTimer = new QTimer(this);
    connect(m_queryTimer, &QTimer::timeout, this, &UsblControlWindow::Slot_OnQueryTimerTimeout);

    QList<QSerialPortInfo> serials = QSerialPortInfo::availablePorts();
    QStringList portList;
    for(const QSerialPortInfo &info : serials) {
        portList << info.portName();
    }
    ComboBox_Port->addItems(portList);
    ComboBox_GpsPort->addItems(portList);
    
    QStringList baudRates = {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
    ComboBox_Baud->addItems(baudRates);
    ComboBox_Baud->setCurrentText("9600");
    ComboBox_GpsBaud->addItems(baudRates);
    ComboBox_GpsBaud->setCurrentText("9600");

    connect(Btn_OpenSerial, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_OpenSerial_Clicked);
    connect(Btn_CloseSerial, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_CloseSerial_Clicked);
    connect(UsblSerial, &QSerialPort::readyRead, this, &UsblControlWindow::Slot_UsblReadData);

    connect(Btn_OpenGps, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_OpenGps_Clicked);
    connect(Btn_CloseGps, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_CloseGps_Clicked);
    connect(GpsSerial, &QSerialPort::readyRead, this, &UsblControlWindow::Slot_GpsReadData);

    connect(Btn_Up, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Up_Clicked);
    connect(Btn_Down, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Down_Clicked);
    connect(Btn_Left, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Left_Clicked);
    connect(Btn_Right, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Right_Clicked);
    connect(Btn_Forward, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Forward_Clicked);
    connect(Btn_Backward, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Backward_Clicked);
    connect(Btn_Stop, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Stop_Clicked);
    
    connect(Btn_QueryLoc, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_QueryLoc_Clicked);

    connect(Btn_Rel1_Open, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Rel1_Open_Clicked);
    connect(Btn_Rel1_Close, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Rel1_Close_Clicked);
    connect(Btn_Rel2_Open, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Rel2_Open_Clicked);
    connect(Btn_Rel2_Close, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Rel2_Close_Clicked);
}

UsblControlWindow::~UsblControlWindow()
{
    if(UsblSerial->isOpen()) UsblSerial->close();
    if(GpsSerial->isOpen()) GpsSerial->close();
}

void UsblControlWindow::InitUi()
{
    GroupBox_LeftPanel = new QGroupBox(this);
// [修改] 增加左侧面板高度 (850 -> 890)
    GroupBox_LeftPanel->setGeometry(5, 5, 440, 890); 
    GroupBox_LeftPanel->setTitle(""); 
    GroupBox_LeftPanel->setStyleSheet("QGroupBox{border:none;}");

    GroupBox_Serial = new QGroupBox("1. USBL通信设置", GroupBox_LeftPanel);
    GroupBox_Serial->setGeometry(5, 0, 430, 80);
    
    QLabel *lblPort1 = new QLabel("端口:", GroupBox_Serial); lblPort1->setGeometry(10, 30, 40, 25);
    ComboBox_Port = new QComboBox(GroupBox_Serial); ComboBox_Port->setGeometry(50, 30, 100, 25);
    QLabel *lblBaud1 = new QLabel("波特率:", GroupBox_Serial); lblBaud1->setGeometry(160, 30, 50, 25);
    ComboBox_Baud = new QComboBox(GroupBox_Serial); ComboBox_Baud->setGeometry(215, 30, 80, 25);
    Btn_OpenSerial = new QPushButton("打开", GroupBox_Serial); Btn_OpenSerial->setGeometry(310, 25, 50, 30); Btn_OpenSerial->setStyleSheet("color: green; font-weight: bold;");
    Btn_CloseSerial = new QPushButton("关闭", GroupBox_Serial); Btn_CloseSerial->setGeometry(370, 25, 50, 30); Btn_CloseSerial->setEnabled(false);

    GroupBox_GpsSerial = new QGroupBox("2. GPS通信设置", GroupBox_LeftPanel);
    GroupBox_GpsSerial->setGeometry(5, 85, 430, 80);
    
    QLabel *lblPort2 = new QLabel("端口:", GroupBox_GpsSerial); lblPort2->setGeometry(10, 30, 40, 25);
    ComboBox_GpsPort = new QComboBox(GroupBox_GpsSerial); ComboBox_GpsPort->setGeometry(50, 30, 100, 25);
    QLabel *lblBaud2 = new QLabel("波特率:", GroupBox_GpsSerial); lblBaud2->setGeometry(160, 30, 50, 25);
    ComboBox_GpsBaud = new QComboBox(GroupBox_GpsSerial); ComboBox_GpsBaud->setGeometry(215, 30, 80, 25);
    Btn_OpenGps = new QPushButton("打开", GroupBox_GpsSerial); Btn_OpenGps->setGeometry(310, 25, 50, 30); Btn_OpenGps->setStyleSheet("color: blue; font-weight: bold;");
    Btn_CloseGps = new QPushButton("关闭", GroupBox_GpsSerial); Btn_CloseGps->setGeometry(370, 25, 50, 30); Btn_CloseGps->setEnabled(false);

    GroupBox_GpsInfo = new QGroupBox("3. GPS定位数据 (基站)", GroupBox_LeftPanel);
    GroupBox_GpsInfo->setGeometry(5, 170, 430, 90);
    
    QLabel *lblLat = new QLabel("纬度:", GroupBox_GpsInfo); lblLat->setGeometry(20, 25, 40, 25);
    LineEdit_GpsLat = new QLineEdit(GroupBox_GpsInfo); LineEdit_GpsLat->setGeometry(60, 25, 140, 25); LineEdit_GpsLat->setReadOnly(true);
    QLabel *lblLon = new QLabel("经度:", GroupBox_GpsInfo); lblLon->setGeometry(220, 25, 40, 25);
    LineEdit_GpsLon = new QLineEdit(GroupBox_GpsInfo); LineEdit_GpsLon->setGeometry(260, 25, 140, 25); LineEdit_GpsLon->setReadOnly(true);
    Label_GpsTime = new QLabel("UTC时间: --:--:--", GroupBox_GpsInfo); Label_GpsTime->setGeometry(20, 55, 300, 25); Label_GpsTime->setStyleSheet("color: gray;");

    GroupBox_UsblPos = new QGroupBox("4. 水下信标定位信息", GroupBox_LeftPanel);
    GroupBox_UsblPos->setGeometry(5, 265, 430, 140);

    QLabel *lblRel = new QLabel("相对坐标(米):", GroupBox_UsblPos); lblRel->setGeometry(10, 30, 90, 25); lblRel->setStyleSheet("font-weight: bold; color: #333;");
    QLabel *lblX = new QLabel("X:", GroupBox_UsblPos); lblX->setGeometry(100, 30, 15, 25);
    LineEdit_UsblX = new QLineEdit("0.0", GroupBox_UsblPos); LineEdit_UsblX->setGeometry(120, 30, 60, 25); LineEdit_UsblX->setReadOnly(true); LineEdit_UsblX->setAlignment(Qt::AlignCenter);
    QLabel *lblY = new QLabel("Y:", GroupBox_UsblPos); lblY->setGeometry(190, 30, 15, 25);
    LineEdit_UsblY = new QLineEdit("0.0", GroupBox_UsblPos); LineEdit_UsblY->setGeometry(210, 30, 60, 25); LineEdit_UsblY->setReadOnly(true); LineEdit_UsblY->setAlignment(Qt::AlignCenter);
    QLabel *lblZ = new QLabel("Z:", GroupBox_UsblPos); lblZ->setGeometry(280, 30, 15, 25);
    LineEdit_UsblZ = new QLineEdit("0.0", GroupBox_UsblPos); LineEdit_UsblZ->setGeometry(300, 30, 60, 25); LineEdit_UsblZ->setReadOnly(true); LineEdit_UsblZ->setAlignment(Qt::AlignCenter);

    QLabel *lblAbs = new QLabel("信标经纬度:", GroupBox_UsblPos); lblAbs->setGeometry(10, 70, 90, 25); lblAbs->setStyleSheet("font-weight: bold; color: darkblue;");
    QLabel *lblTLat = new QLabel("纬度:", GroupBox_UsblPos); lblTLat->setGeometry(100, 70, 40, 25);
    LineEdit_TargetLat = new QLineEdit("-", GroupBox_UsblPos); LineEdit_TargetLat->setGeometry(140, 70, 90, 25); LineEdit_TargetLat->setReadOnly(true); LineEdit_TargetLat->setStyleSheet("color: blue; font-weight: bold;");
    QLabel *lblTLon = new QLabel("经度:", GroupBox_UsblPos); lblTLon->setGeometry(240, 70, 40, 25);
    LineEdit_TargetLon = new QLineEdit("-", GroupBox_UsblPos); LineEdit_TargetLon->setGeometry(280, 70, 90, 25); LineEdit_TargetLon->setReadOnly(true); LineEdit_TargetLon->setStyleSheet("color: blue; font-weight: bold;");
    
    Label_UsblStatus = new QLabel("等待数据...", GroupBox_UsblPos); Label_UsblStatus->setGeometry(20, 110, 350, 20); Label_UsblStatus->setStyleSheet("color: gray; font-size: 10px;");

    GroupBox_Control = new QGroupBox("5. 水声遥控指令", GroupBox_LeftPanel);
    GroupBox_Control->setGeometry(5, 415, 430, 290); 
    GroupBox_Control->setEnabled(false); 

    int cx = 215; // Center X
    int startY = 30; int btnW = 90; int btnH = 35; int gap = 40;

    Btn_Forward = new QPushButton("前进 (FD)", GroupBox_Control); Btn_Forward->setGeometry(cx - btnW/2, startY, btnW, btnH);
    Btn_Left = new QPushButton("左转 (LT)", GroupBox_Control); Btn_Left->setGeometry(cx - btnW/2 - 110, startY + gap*2, btnW, btnH);
    Btn_Right = new QPushButton("右转 (RT)", GroupBox_Control); Btn_Right->setGeometry(cx - btnW/2 + 110, startY + gap*2, btnW, btnH);
    Btn_Backward = new QPushButton("后退 (BD)", GroupBox_Control); Btn_Backward->setGeometry(cx - btnW/2, startY + gap*4, btnW, btnH);
    Btn_Up = new QPushButton("上浮 (UP)", GroupBox_Control); Btn_Up->setGeometry(cx - btnW/2 - 110, startY + gap, btnW, btnH);
    Btn_Down = new QPushButton("下潜 (DN)", GroupBox_Control); Btn_Down->setGeometry(cx - btnW/2 + 110, startY + gap, btnW, btnH);
    Btn_Stop = new QPushButton("停止 (ZE)", GroupBox_Control); Btn_Stop->setGeometry(cx - btnW/2, startY + gap*2, btnW, btnH);
    Btn_Stop->setStyleSheet("background-color: #ffcccc; border: 1px solid red; font-weight: bold;");

    Label_Speed = new QLabel("动力档位:", GroupBox_Control); Label_Speed->setGeometry(cx - 50, startY + gap*3, 70, 25);
    SpinBox_Speed = new QSpinBox(GroupBox_Control); SpinBox_Speed->setGeometry(cx + 30, startY + gap*3, 50, 25);
    SpinBox_Speed->setRange(1, 5); SpinBox_Speed->setValue(1); SpinBox_Speed->setAlignment(Qt::AlignCenter);
    
    Btn_QueryLoc = new QPushButton("查询水下定位 (Ping)", GroupBox_Control);
    Btn_QueryLoc->setGeometry(cx - 75, startY + gap*5 + 5, 150, 40); 
    Btn_QueryLoc->setStyleSheet("color: blue; font-weight: bold; border: 1px solid blue; border-radius: 5px;");

    GroupBox_Releaser = new QGroupBox("6. 释放器控制 (USBL)", GroupBox_LeftPanel);
    GroupBox_Releaser->setGeometry(5, 715, 430, 80);

    Btn_Rel1_Open = new QPushButton("开启释放1", GroupBox_Releaser);
    Btn_Rel1_Open->setGeometry(20, 30, 85, 35);
    Btn_Rel1_Close = new QPushButton("关闭释放1", GroupBox_Releaser);
    Btn_Rel1_Close->setGeometry(115, 30, 85, 35);
    Btn_Rel2_Open = new QPushButton("开启释放2", GroupBox_Releaser);
    Btn_Rel2_Open->setGeometry(230, 30, 85, 35);
    Btn_Rel2_Close = new QPushButton("关闭释放2", GroupBox_Releaser);
    Btn_Rel2_Close->setGeometry(325, 30, 85, 35);

    // --- 右侧面板：地图显示 ---
    GroupBox_Map = new QGroupBox("岸基位置实时显示", this);
    GroupBox_Map->setGeometry(450, 10, 590, 880); 
    
    // [新增] 清除轨迹按钮 (悬浮在地图区域右上角)
    Btn_ClearTrack = new QPushButton("清除轨迹", GroupBox_Map);
    Btn_ClearTrack->setGeometry(480, 25, 100, 30); // 放在右上角
    Btn_ClearTrack->setStyleSheet("color: white; background-color: #d9534f; border-radius: 4px; font-weight: bold;");
    connect(Btn_ClearTrack, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_ClearTrack_Clicked);

    // [修改] 地图视图位置 (给上方按钮留一点空隙，或者直接覆盖也行，这里选择让出一点顶部空间)
    // 之前 y=25, 现在改为 y=60，或者保持不变让按钮浮在上方
    // 为了美观，建议让地图稍微下降一点点，或者按钮放在标题栏旁边
    MapView = new QWebEngineView(GroupBox_Map);
    MapView->setGeometry(10, 60, 570, 810); // y从25改为60，高度相应减小

    QWebChannel *channel = new QWebChannel(this);
    JSBridge = new bridge(this); 
    channel->registerObject("webBridge", JSBridge);
    MapView->page()->setWebChannel(channel);
    MapView->page()->load(QUrl("qrc:/map/BDMap.html"));

// --- [新增] 第三列：右侧面板 (x = 1050) ---
    GroupBox_RightPanel = new QGroupBox("智能控制面板", this);
    GroupBox_RightPanel->setGeometry(1050, 10, 290, 880); 
    // 稍微美化一下右侧面板背景，区分功能区
    GroupBox_RightPanel->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #aaa; margin-top: 10px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px; }");

    // [新增] 8. 定深/定高控制组件
    GroupBox_AutoControl = new QGroupBox("8. 定深/定高控制", GroupBox_RightPanel);
    GroupBox_AutoControl->setGeometry(10, 30, 270, 200); // 在右侧面板顶部
    
    // === 定深部分 ===
    Label_DepthTitle = new QLabel("定深控制 (Auto Depth)", GroupBox_AutoControl);
    Label_DepthTitle->setGeometry(10, 30, 200, 20);
    Label_DepthTitle->setStyleSheet("color: darkblue; font-weight: bold;");

    QLabel *lblDInput = new QLabel("目标深度(m):", GroupBox_AutoControl);
    lblDInput->setGeometry(10, 60, 80, 25);

    LineEdit_DepthVal = new QLineEdit("01.5", GroupBox_AutoControl);
    LineEdit_DepthVal->setGeometry(95, 60, 60, 25);
    LineEdit_DepthVal->setAlignment(Qt::AlignCenter);
    LineEdit_DepthVal->setPlaceholderText("xx.x");

    Btn_Depth_Open = new QPushButton("开启定深", GroupBox_AutoControl);
    Btn_Depth_Open->setGeometry(165, 58, 70, 30);
    Btn_Depth_Open->setStyleSheet("color: white; background-color: #28a745; border-radius: 4px;");

    Btn_Depth_Close = new QPushButton("关闭定深", GroupBox_AutoControl);
    Btn_Depth_Close->setGeometry(165, 95, 70, 30);
    Btn_Depth_Close->setStyleSheet("color: white; background-color: #dc3545; border-radius: 4px;");

    // 分隔线
    QFrame *line = new QFrame(GroupBox_AutoControl);
    line->setGeometry(10, 135, 250, 2);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    // === 定高部分 ===
    Label_AltTitle = new QLabel("定高控制 (Auto Altitude)", GroupBox_AutoControl);
    Label_AltTitle->setGeometry(10, 145, 200, 20);
    Label_AltTitle->setStyleSheet("color: darkcyan; font-weight: bold;");

    QLabel *lblAInput = new QLabel("目标高度(m):", GroupBox_AutoControl);
    lblAInput->setGeometry(10, 175, 80, 25);

    LineEdit_AltVal = new QLineEdit("01.0", GroupBox_AutoControl);
    LineEdit_AltVal->setGeometry(95, 175, 60, 25);
    LineEdit_AltVal->setAlignment(Qt::AlignCenter);

    Btn_Alt_Open = new QPushButton("开启定高", GroupBox_AutoControl);
    Btn_Alt_Open->setGeometry(165, 173, 70, 30);
    Btn_Alt_Open->setStyleSheet("color: white; background-color: #17a2b8; border-radius: 4px;");

    Btn_Alt_Close = new QPushButton("关闭定高", GroupBox_AutoControl);
    Btn_Alt_Close->setGeometry(165, 210, 70, 30); // 注意：这里高度可能超出了GroupBox_AutoControl (200)，需调整GroupBox高度
    
    // 调整 GroupBox 高度以适应内容
    GroupBox_AutoControl->setGeometry(10, 30, 270, 260); 

    // 连接信号槽
    connect(Btn_Depth_Open, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Depth_Open_Clicked);
    connect(Btn_Depth_Close, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Depth_Close_Clicked);
    connect(Btn_Alt_Open, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Alt_Open_Clicked);
    connect(Btn_Alt_Close, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Alt_Close_Clicked);

// [新增] 9. 预编程运动序列设置
    GroupBox_Program = new QGroupBox("9. 预编程任务下发", GroupBox_RightPanel);
    GroupBox_Program->setGeometry(10, 300, 270, 220); 

    int lineGap = 45;
    int progStartY = 30;
// --- 步骤 1 ---
    Label_Step1 = new QLabel("动作1:", GroupBox_Program);
    Label_Step1->setGeometry(10, progStartY, 40, 25);
    
    Combo_Act1 = new QComboBox(GroupBox_Program);
    Combo_Act1->setGeometry(55, progStartY, 100, 25);
    Combo_Act1->addItem("前进 (F)", 'F');
    Combo_Act1->addItem("后退 (B)", 'B');
    Combo_Act1->addItem("左转 (L)", 'L');
    Combo_Act1->addItem("右转 (R)", 'R');
    
    // [修改] 连接信号槽：当动作改变时，刷新后面的参数框
    connect(Combo_Act1, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UsblControlWindow::Slot_Act1_Changed);

    Combo_Dist1 = new QComboBox(GroupBox_Program);
    Combo_Dist1->setGeometry(165, progStartY, 90, 25);
    
    // [修改] 不再使用 for 循环写死，而是调用函数初始化
    RefreshDistCombo(Combo_Act1, Combo_Dist1); 


    // --- 步骤 2 ---
    Label_Step2 = new QLabel("动作2:", GroupBox_Program);
    Label_Step2->setGeometry(10, progStartY + lineGap, 40, 25);
    
    Combo_Act2 = new QComboBox(GroupBox_Program);
    Combo_Act2->setGeometry(55, progStartY + lineGap, 100, 25);
    Combo_Act2->addItem("前进 (F)", 'F');
    Combo_Act2->addItem("后退 (B)", 'B');
    Combo_Act2->addItem("左转 (L)", 'L');
    Combo_Act2->addItem("右转 (R)", 'R');
    
    connect(Combo_Act2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UsblControlWindow::Slot_Act2_Changed);

    Combo_Dist2 = new QComboBox(GroupBox_Program);
    Combo_Dist2->setGeometry(165, progStartY + lineGap, 90, 25);
    
    RefreshDistCombo(Combo_Act2, Combo_Dist2);


    // --- 步骤 3 ---
    Label_Step3 = new QLabel("动作3:", GroupBox_Program);
    Label_Step3->setGeometry(10, progStartY + lineGap*2, 40, 25);
    
    Combo_Act3 = new QComboBox(GroupBox_Program);
    Combo_Act3->setGeometry(55, progStartY + lineGap*2, 100, 25);
    Combo_Act3->addItem("前进 (F)", 'F');
    Combo_Act3->addItem("后退 (B)", 'B');
    Combo_Act3->addItem("左转 (L)", 'L');
    Combo_Act3->addItem("右转 (R)", 'R');
    
    connect(Combo_Act3, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UsblControlWindow::Slot_Act3_Changed);

    Combo_Dist3 = new QComboBox(GroupBox_Program);
    Combo_Dist3->setGeometry(165, progStartY + lineGap*2, 90, 25);
    
    RefreshDistCombo(Combo_Act3, Combo_Dist3);

    // --- 发送按钮 ---
    Btn_SendProg = new QPushButton("发送任务序列", GroupBox_Program);
    Btn_SendProg->setGeometry(55, progStartY + lineGap*3 + 5, 160, 35); // 【修改点】使用 progStartY
    Btn_SendProg->setStyleSheet("color: white; background-color: #6f42c1; font-weight: bold; border-radius: 5px;");
    
    connect(Btn_SendProg, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_SendProg_Clicked);

// ================== [移动并修改] 10. 目标航路点设置 ==================
    // 位置计算: Section 9 (预编程) y=300, h=220 -> 底部 y=520
    // 我们留 20px 间距，设置新 Y = 540
    
    GroupBox_Waypoint = new QGroupBox("7. 目标航路点下发 (导航)", GroupBox_RightPanel);
    GroupBox_Waypoint->setGeometry(10, 540, 270, 100); // 宽度适配右侧，高度略增适应两行布局

    // --- 紧凑布局设计 (270px 宽) ---
    // 左边放输入框，右边放发送按钮
    
    // Row 1: 纬度
    QLabel *lblGLat = new QLabel("纬度:", GroupBox_Waypoint);
    lblGLat->setGeometry(10, 30, 35, 25);
    
    LineEdit_GoalLat = new QLineEdit(GroupBox_Waypoint);
    LineEdit_GoalLat->setGeometry(50, 30, 90, 25);
    LineEdit_GoalLat->setPlaceholderText("18.xxxxxx");
    LineEdit_GoalLat->setAlignment(Qt::AlignCenter);

    // Row 2: 经度
    QLabel *lblGLon = new QLabel("经度:", GroupBox_Waypoint);
    lblGLon->setGeometry(10, 60, 35, 25);
    
    LineEdit_GoalLon = new QLineEdit(GroupBox_Waypoint);
    LineEdit_GoalLon->setGeometry(50, 60, 90, 25);
    LineEdit_GoalLon->setPlaceholderText("109.xxxxxx");
    LineEdit_GoalLon->setAlignment(Qt::AlignCenter);

    // Right Side: 大大的发送按钮
    Btn_SendGoal = new QPushButton("下发\n目标点", GroupBox_Waypoint);
    Btn_SendGoal->setGeometry(150, 30, 110, 55); // 占据右侧空间
    Btn_SendGoal->setStyleSheet("color: white; background-color: #d9534f; font-weight: bold; border-radius: 5px;");

    // 连接信号槽 (确保没有重复连接，如果之前删除了旧代码，这里需要重新连接)
    connect(Btn_SendGoal, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_SendGoal_Clicked);


}

void UsblControlWindow::Slot_Btn_Rel1_Open_Clicked()
{
    if(!UsblSerial->isOpen()) {
        QMessageBox::warning(this, "提示", "请先打开USBL通信串口！");
        return;
    }
    QString cmd = "#60010407406f70656e3140BEC3\r\n";
    UsblSerial->write(cmd.toUtf8());
}

void UsblControlWindow::Slot_Btn_Rel1_Close_Clicked()
{
    if(!UsblSerial->isOpen()) {
        QMessageBox::warning(this, "提示", "请先打开USBL通信串口！");
        return;
    }
    QString cmd = "#6001040740636c6f736531407731\r\n";
    UsblSerial->write(cmd.toUtf8());
}

void UsblControlWindow::Slot_Btn_Rel2_Open_Clicked()
{
    if(!UsblSerial->isOpen()) {
        QMessageBox::warning(this, "提示", "请先打开USBL通信串口！");
        return;
    }
    QString cmd = "#60010407406f70656e3240BE33\r\n";
    UsblSerial->write(cmd.toUtf8());
}

void UsblControlWindow::Slot_Btn_Rel2_Close_Clicked()
{
    if(!UsblSerial->isOpen()) {
        QMessageBox::warning(this, "提示", "请先打开USBL通信串口！");
        return;
    }
    QString cmd = "#6001040740636c6f7365324077C1\r\n";
    UsblSerial->write(cmd.toUtf8());
}

void UsblControlWindow::Slot_UsblReadData()
{
    m_usblBuffer.append(UsblSerial->readAll());
    while (m_usblBuffer.contains('\n')) {
        int lineEndIndex = m_usblBuffer.indexOf('\n');
        QByteArray lineData = m_usblBuffer.left(lineEndIndex).trimmed();
        m_usblBuffer.remove(0, lineEndIndex + 1);
        QString lineString = QString(lineData);

        if (lineString.startsWith("$39") || lineString.startsWith("#39")) {
            ParseSeatracData(lineString);
        }
    }
}

void UsblControlWindow::ParseSeatracData(const QString &hexString)
{
    QString cleanHex = hexString;
    if (cleanHex.startsWith('$') || cleanHex.startsWith('#')) {
        cleanHex = cleanHex.mid(1);
    }
    QByteArray buf = QByteArray::fromHex(cleanHex.toLatin1());
    
    int len = buf.size();
    if (len < 5) return;
    if ((uint8_t)buf[0] != 0x39) return;

    int ptr = 3; 
    uint8_t flags = (uint8_t)buf[ptr++];
    ptr++; 
    ptr += 12;

    if (flags & 0x01) { ptr += 10; }

    if (flags & 0x02) {
        if (ptr >= len) return;
        uint8_t channels = (uint8_t)buf[ptr++];
        int usbl_block_size = (2 * channels) + 6;
        ptr += usbl_block_size;
    }

    if (flags & 0x04) {
        if (ptr + 6 <= len) {
            int16_t easting_raw = (int16_t)((uint8_t)buf[ptr] | ((uint8_t)buf[ptr+1] << 8));
            float easting_x = easting_raw / 10.0f; 
            ptr += 2;

            int16_t northing_raw = (int16_t)((uint8_t)buf[ptr] | ((uint8_t)buf[ptr+1] << 8));
            float northing_y = northing_raw / 10.0f;
            ptr += 2;

            int16_t depth_raw = (int16_t)((uint8_t)buf[ptr] | ((uint8_t)buf[ptr+1] << 8));
            float depth_z = depth_raw / 10.0f;
            ptr += 2;

            LineEdit_UsblX->setText(QString::number(easting_x, 'f', 1));
            LineEdit_UsblY->setText(QString::number(northing_y, 'f', 1));
            LineEdit_UsblZ->setText(QString::number(depth_z, 'f', 1));
            
            if (m_hasGpsFix) {
                CalculateTargetGeoPos(easting_x, northing_y);
                Label_UsblStatus->setText("解析成功(含绝对坐标) " + QDateTime::currentDateTime().toString("HH:mm:ss"));
            } else {
                Label_UsblStatus->setText("解析成功(等待GPS...) " + QDateTime::currentDateTime().toString("HH:mm:ss"));
                LineEdit_TargetLat->setText("GPS无效");
                LineEdit_TargetLon->setText("GPS无效");
            }
        } else {
            Label_UsblStatus->setText("解析错误: 数据不完整");
        }
    } else {
        Label_UsblStatus->setText("数据包不包含位置信息");
    }
}

void UsblControlWindow::CalculateTargetGeoPos(float x_offset, float y_offset)
{
    double dLat = (y_offset / EARTH_RADIUS) * (180.0 / M_PI);
    double targetLat = m_baseLatitude + dLat;

    double radLat = m_baseLatitude * (M_PI / 180.0);
    double dLon = (x_offset / (EARTH_RADIUS * qCos(radLat))) * (180.0 / M_PI);
    double targetLon = m_baseLongitude + dLon;

    LineEdit_TargetLat->setText(QString::number(targetLat, 'f', 6));
    LineEdit_TargetLon->setText(QString::number(targetLon, 'f', 6));

    UpdateTargetMapPosition(QString::number(targetLon, 'f', 6), QString::number(targetLat, 'f', 6));

    // 计算完成后，自动下发坐标 (使用 '/' 协议)
    SendTargetPosToAUV(targetLat, targetLon);
}

void UsblControlWindow::Slot_GpsReadData()
{
    while(GpsSerial->canReadLine()) {
        QByteArray line = GpsSerial->readLine().trimmed();
        if(line.isEmpty()) continue;
        ParseNmeaData(line);
    }
}

void UsblControlWindow::ParseNmeaData(const QByteArray &data)
{
    QString nmea = QString::fromLatin1(data);
    if (nmea.contains("GGA")) { 
        QStringList parts = nmea.split(',');
        if (parts.size() < 6) return;

        QString time = parts[1];
        QString latNmea = parts[2];
        QString latDir = parts[3];
        QString lonNmea = parts[4];
        QString lonDir = parts[5];

        if(latNmea.isEmpty() || lonNmea.isEmpty()) {
            m_hasGpsFix = false;
            return;
        }

        if(time.size() >= 6) {
             Label_GpsTime->setText("UTC时间: " + time.mid(0,2) + ":" + time.mid(2,2) + ":" + time.mid(4,2));
        }
        LineEdit_GpsLat->setText(latNmea + " " + latDir);
        LineEdit_GpsLon->setText(lonNmea + " " + lonDir);

        double dLat = NmeaToDecimal(latNmea);
        double dLon = NmeaToDecimal(lonNmea);
        if(latDir == "S") dLat = -dLat;
        if(lonDir == "W") dLon = -dLon;

        m_baseLatitude = dLat;
        m_baseLongitude = dLon;
        m_hasGpsFix = true; 

        UpdateMapPosition(QString::number(dLon, 'f', 6), QString::number(dLat, 'f', 6));
    }
}

double UsblControlWindow::NmeaToDecimal(const QString &nmeaStr)
{
    if(nmeaStr.isEmpty()) return 0.0;
    int dotPos = nmeaStr.indexOf('.');
    if(dotPos < 2) return 0.0;
    double minutes = nmeaStr.mid(dotPos - 2).toDouble();
    double degrees = nmeaStr.left(dotPos - 2).toDouble();
    return degrees + (minutes / 60.0);
}

void UsblControlWindow::UpdateMapPosition(const QString &lon, const QString &lat)
{
    if(lon == last_longitude && lat == last_latitude) return;
    last_longitude = lon;
    last_latitude = lat;
    QString cmd = QString("setBasePoint(%1, %2)").arg(lon).arg(lat);
    MapView->page()->runJavaScript(cmd);
}

void UsblControlWindow::Slot_Btn_OpenSerial_Clicked() {
    UsblSerial->setPortName(ComboBox_Port->currentText());
    UsblSerial->setBaudRate(ComboBox_Baud->currentText().toInt());
    UsblSerial->setDataBits(QSerialPort::Data8);
    UsblSerial->setParity(QSerialPort::NoParity);
    UsblSerial->setStopBits(QSerialPort::OneStop);
    UsblSerial->setFlowControl(QSerialPort::NoFlowControl);
    
    m_usblBuffer.clear(); 

    if(UsblSerial->open(QIODevice::ReadWrite)) {
        Btn_OpenSerial->setEnabled(false);
        Btn_CloseSerial->setEnabled(true);
        GroupBox_Control->setEnabled(true);
        GroupBox_Releaser->setEnabled(true); 
        ComboBox_Port->setEnabled(false);
        ComboBox_Baud->setEnabled(false);
    } else {
        QMessageBox::critical(this, "错误", "无法打开USBL串口！");
    }
}

void UsblControlWindow::Slot_Btn_CloseSerial_Clicked() {
    if (m_queryTimer->isActive()) {
        m_queryTimer->stop();
        Btn_QueryLoc->setText("查询水下定位 (Ping)");
        Btn_QueryLoc->setStyleSheet("color: blue; font-weight: bold; border: 1px solid blue; border-radius: 5px;");
    }
    
    if(UsblSerial->isOpen()) UsblSerial->close();
    Btn_OpenSerial->setEnabled(true);
    Btn_CloseSerial->setEnabled(false);
    GroupBox_Control->setEnabled(false);
    GroupBox_Releaser->setEnabled(false);
    ComboBox_Port->setEnabled(true);
    ComboBox_Baud->setEnabled(true);
}

void UsblControlWindow::Slot_Btn_OpenGps_Clicked() {
    GpsSerial->setPortName(ComboBox_GpsPort->currentText());
    GpsSerial->setBaudRate(ComboBox_GpsBaud->currentText().toInt());
    GpsSerial->setDataBits(QSerialPort::Data8);
    GpsSerial->setParity(QSerialPort::NoParity);
    GpsSerial->setStopBits(QSerialPort::OneStop);
    if(GpsSerial->open(QIODevice::ReadOnly)) {
        Btn_OpenGps->setEnabled(false);
        Btn_CloseGps->setEnabled(true);
        ComboBox_GpsPort->setEnabled(false);
        ComboBox_GpsBaud->setEnabled(false);
    } else {
        QMessageBox::critical(this, "错误", "无法打开GPS串口！");
    }
}

void UsblControlWindow::Slot_Btn_CloseGps_Clicked() {
    if(GpsSerial->isOpen()) GpsSerial->close();
    Btn_OpenGps->setEnabled(true);
    Btn_CloseGps->setEnabled(false);
    ComboBox_GpsPort->setEnabled(true);
    ComboBox_GpsBaud->setEnabled(true);
}

void UsblControlWindow::Slot_Btn_QueryLoc_Clicked() {
    if(!UsblSerial->isOpen()) {
        QMessageBox::warning(this, "提示", "请先打开USBL通信串口！");
        if(m_queryTimer->isActive()) {
            m_queryTimer->stop();
            Btn_QueryLoc->setText("查询水下定位 (Ping)");
            Btn_QueryLoc->setStyleSheet("color: blue; font-weight: bold; border: 1px solid blue; border-radius: 5px;");
        }
        return;
    }

    if (m_queryTimer->isActive()) {
        m_queryTimer->stop();
        Btn_QueryLoc->setText("查询水下定位 (Ping)");
        Btn_QueryLoc->setStyleSheet("color: blue; font-weight: bold; border: 1px solid blue; border-radius: 5px;");
    } else {
        Slot_OnQueryTimerTimeout();
        m_queryTimer->start(2000);
        Btn_QueryLoc->setText("停止自动查询 (Stop)");
        Btn_QueryLoc->setStyleSheet("color: red; font-weight: bold; border: 1px solid red; border-radius: 5px; background-color: #ffeeee;");
    }
}

void UsblControlWindow::Slot_OnQueryTimerTimeout() {
    if(!UsblSerial->isOpen()) {
        m_queryTimer->stop();
        Btn_QueryLoc->setText("查询水下定位 (Ping)");
        Btn_QueryLoc->setStyleSheet("color: blue; font-weight: bold; border: 1px solid blue; border-radius: 5px;");
        return;
    }

    QString cmd = "#4001040187\r\n"; 
    UsblSerial->write(cmd.toUtf8());
    qDebug() << "USBL Auto Query TX:" << cmd.trimmed() << " Time:" << QDateTime::currentDateTime().toString("HH:mm:ss");
}

void UsblControlWindow::SendUsblCommand(const QString &type, int gear) {
    if(!UsblSerial->isOpen()) {
        QMessageBox::warning(this, "提示", "请先打开USBL通信串口！");
        return;
    }
    QString cmdData = GetProtocolString(type, gear);
    if(cmdData.isEmpty()) return;
    UsblSerial->write((cmdData + "\r\n").toUtf8());
}

void UsblControlWindow::UpdateTargetMapPosition(const QString &lon, const QString &lat)
{
    QString cmd = QString("setTargetPoint(%1, %2)").arg(lon).arg(lat);
    MapView->page()->runJavaScript(cmd);
}

// [新增] CRC16-IBM 计算 (Poly: 0xA001)
uint16_t UsblControlWindow::calculateCRC16(const QByteArray &data)
{
    uint16_t crc = 0;
    uint16_t poly = 0xA001; // Reversed polynomial

    for (int j = 0; j < data.size(); ++j) {
        uint8_t byte = (uint8_t)data[j];
        uint8_t v = byte;
        for (int i = 0; i < 8; ++i) {
            if ((v & 0x01) ^ (crc & 0x01)) {
                crc >>= 1;
                crc ^= poly;
            } else {
                crc >>= 1;
            }
            v >>= 1;
        }
    }
    return crc;
}

// [重写] 发送坐标 (十六进制封装 + CRC16 + 固定开头)
void UsblControlWindow::SendTargetPosToAUV(double lat, double lon)
{
    // 1. 构建明文载荷: "+18.123456+109.262626+" (22字节)
    QString strLat = QString::asprintf("%09.6f", lat);
    QString strLon = QString::asprintf("%09.6f", lon);
    
    // 【关键修改点】这里将 "+" 替换为 "/"
    QString payloadStr = "/" + strLat + "/" + strLon + "/";

    // 2. 构建二进制数据包 (CID, DID, Type, Len, Payload)
    QByteArray binaryPacket;
    binaryPacket.append((char)0x60); // CID
    binaryPacket.append((char)0x01); // DID
    binaryPacket.append((char)0x04); // MSG_TYPE
    binaryPacket.append((char)0x16); // LEN (22字节)
    binaryPacket.append(payloadStr.toLatin1()); // Payload Bytes

    // 3. 计算 CRC16
    uint16_t crc = calculateCRC16(binaryPacket);

    // 4. 转换为十六进制字符串 (大写)
    // binaryPacket -> HexString
    QString hexData = binaryPacket.toHex().toUpper();

    // 5. 拼接 CRC (小端模式: 低字节在前)
    // crc & 0xFF 是低字节, (crc >> 8) 是高字节
    QString hexCrc = QString::asprintf("%02X%02X", (crc & 0xFF), ((crc >> 8) & 0xFF));

    // 6. 组合最终发送指令: "$" + HexData + HexCRC + "\r\n"
    QString fullCommand = "$" + hexData + hexCrc + "\r\n";

    // 7. 发送
    if (UsblSerial->isOpen()) {
        UsblSerial->write(fullCommand.toUtf8());
        
        // 调试打印
        qDebug() << "[AutoSend] HexCmd:" << fullCommand.trimmed();
    }
}

// [新增] 目标点下发槽函数 (使用 '+' 分隔符，ID: 0x04)
void UsblControlWindow::Slot_Btn_SendGoal_Clicked()
{
    // 1. 检查串口是否打开
    if(!UsblSerial->isOpen()) {
        QMessageBox::warning(this, "提示", "请先打开USBL通信串口！");
        return;
    }

    // 2. 获取并校验输入数据
    bool okLat, okLon;
    double goalLat = LineEdit_GoalLat->text().toDouble(&okLat);
    double goalLon = LineEdit_GoalLon->text().toDouble(&okLon);

    if (!okLat || !okLon) {
        QMessageBox::warning(this, "错误", "请输入有效的经纬度数值！");
        return;
    }

    // 3. 构建载荷 (Payload)
    // 协议要求: "+XX.XXXXXX+XXX.XXXXXX+" (分隔符为 +, 总长22字节)
    QString strLat = QString::asprintf("%09.6f", goalLat);
    QString strLon = QString::asprintf("%10.6f", goalLon);
    QString payloadStr = "+" + strLat + "+" + strLon + "+";

    // 4. 构建二进制数据包
    QByteArray binaryPacket;
    binaryPacket.append((char)0x60); // CID (固定)
    binaryPacket.append((char)0x01); // DID (固定)
    
    // 标识位 (Message Type)
    // 定位下发是 0x04，目标点下发设为 0x04
    binaryPacket.append((char)0x04);
    
    binaryPacket.append((char)0x16); // LEN (22字节)
    binaryPacket.append(payloadStr.toLatin1()); // Payload Bytes

    // 5. 计算 CRC16
    uint16_t crc = calculateCRC16(binaryPacket);

    // 6. 转换为十六进制字符串 (大写)
    QString hexData = binaryPacket.toHex().toUpper();

    // 7. 拼接 CRC (小端模式)
    QString hexCrc = QString::asprintf("%02X%02X", (crc & 0xFF), ((crc >> 8) & 0xFF));

    // 8. 组合最终指令: "#" + HexData + HexCRC + "\r\n"
    QString fullCommand = "#" + hexData + hexCrc + "\r\n";

    // 9. 发送
    UsblSerial->write(fullCommand.toUtf8());
    
    // 10. 调试信息与反馈
    qDebug() << "[GoalSend] HexCmd:" << fullCommand.trimmed();
    qDebug() << "[GoalSend] Payload:" << payloadStr;
    QMessageBox::information(this, "成功", "目标航路点指令已发送！\n载荷：" + payloadStr);
}

// [新增] 通用定深定高发送函数实现
void UsblControlWindow::SendAutoControlCmd(const QString &type, const QString &value, bool isEnable)
{
    if(!UsblSerial->isOpen()) {
        QMessageBox::warning(this, "提示", "请先打开USBL通信串口！");
        return;
    }

    QString payloadStr;

    if (isEnable) {
        // 1. 格式化数值: 确保是 xx.x 格式 (例如 1.5 -> 01.5)
        double val = value.toDouble();
        // 使用 %04.1f 格式化为总宽4位(含小数点)，一位小数，前补0
        // 例如: 1.5 -> "01.5", 10.0 -> "10.0"
        QString formattedVal = QString::asprintf("%04.1f", val);
        
        // 构建开启指令: !AD:xx.x!
        payloadStr = QString("!%1:%2!").arg(type).arg(formattedVal);
    } else {
        // 构建关闭指令: !AD:OFFF!
        payloadStr = QString("!%1:OFFF!").arg(type);
    }

    // 2. 检查载荷长度是否为9字节
    if (payloadStr.length() != 9) {
        qDebug() << "Warning: Payload length is not 9 bytes:" << payloadStr;
    }

    // 3. 构建二进制数据包
    QByteArray binaryPacket;
    binaryPacket.append((char)0x60); // CID
    binaryPacket.append((char)0x01); // DID
    binaryPacket.append((char)0x04); // MSG_TYPE (参考旧代码定为0x04)
    
    // 【关键】长度自动设为载荷长度（9），如果必须是08，请手动改为 (char)0x08
    binaryPacket.append((char)payloadStr.length()); 
    
    binaryPacket.append(payloadStr.toLatin1()); // 添加载荷

    // 4. 计算 CRC16 (使用您现有的辅助函数)
    uint16_t crc = calculateCRC16(binaryPacket);

    // 5. 转十六进制字符串并拼接
    QString hexData = binaryPacket.toHex().toUpper();
    QString hexCrc = QString::asprintf("%02X%02X", (crc & 0xFF), ((crc >> 8) & 0xFF)); // 小端

    // 6. 最终指令结构: # + HexData + HexCRC + \r\n
    QString fullCommand = "#" + hexData + hexCrc + "\r\n";

    // 7. 发送
    UsblSerial->write(fullCommand.toUtf8());
    
    // 调试打印
    qDebug() << "[AutoControl] Type:" << type << " Cmd:" << payloadStr << " Hex:" << fullCommand.trimmed();
}

void UsblControlWindow::Slot_Btn_Depth_Open_Clicked()
{
    QString val = LineEdit_DepthVal->text();
    SendAutoControlCmd("AD", val, true);
}

void UsblControlWindow::Slot_Btn_Depth_Close_Clicked()
{
    SendAutoControlCmd("AD", "", false);
}

void UsblControlWindow::Slot_Btn_Alt_Open_Clicked()
{
    QString val = LineEdit_AltVal->text();
    SendAutoControlCmd("AH", val, true);
}

void UsblControlWindow::Slot_Btn_Alt_Close_Clicked()
{
    SendAutoControlCmd("AH", "", false);
}

void UsblControlWindow::Slot_Btn_SendProg_Clicked()
{
    // 1. 检查串口
    if(!UsblSerial->isOpen()) {
        QMessageBox::warning(this, "提示", "请先打开USBL通信串口！");
        return;
    }

    // 2. 提取用户选择的动作和距离 (从 itemData 获取对应的字符)
    QChar act1 = Combo_Act1->currentData().toChar();
    QChar dist1 = Combo_Dist1->currentData().toChar();
    
    QChar act2 = Combo_Act2->currentData().toChar();
    QChar dist2 = Combo_Dist2->currentData().toChar();
    
    QChar act3 = Combo_Act3->currentData().toChar();
    QChar dist3 = Combo_Dist3->currentData().toChar();

    // 3. 构建载荷字符串 "&XNXNXN&" (8字节)
    QString payloadStr = QString("&%1%2%3%4%5%6&")
            .arg(act1).arg(dist1)
            .arg(act2).arg(dist2)
            .arg(act3).arg(dist3);
    
    // 校验长度 (理论上必然是8)
    if (payloadStr.length() != 8) {
        qDebug() << "Error: Program payload length incorrect!";
        return;
    }

    // 4. 构建二进制数据包 (参考之前的协议结构)
    QByteArray binaryPacket;
    binaryPacket.append((char)0x60); // CID
    binaryPacket.append((char)0x01); // DID
    binaryPacket.append((char)0x04); // MSG_TYPE (控制指令通用类型)
    
    binaryPacket.append((char)0x08); // LEN (固定8字节载荷)
    binaryPacket.append(payloadStr.toLatin1()); // Payload

    // 5. 计算 CRC16
    uint16_t crc = calculateCRC16(binaryPacket);
    
    // 6. 转十六进制并拼接
    QString hexData = binaryPacket.toHex().toUpper();
    QString hexCrc = QString::asprintf("%02X%02X", (crc & 0xFF), ((crc >> 8) & 0xFF));
    
    QString fullCommand = "#" + hexData + hexCrc + "\r\n";

    // 7. 发送
    UsblSerial->write(fullCommand.toUtf8());

    // 8. 用户反馈与日志
    qDebug() << "[ProgSend] Payload:" << payloadStr << " Hex:" << fullCommand.trimmed();
    QMessageBox::information(this, "发送成功", 
        QString("预编程任务已发送:\n步骤1: %1 %2\n步骤2: %3 %4\n步骤3: %5 %6")
        .arg(Combo_Act1->currentText()).arg(Combo_Dist1->currentText())
        .arg(Combo_Act2->currentText()).arg(Combo_Dist2->currentText())
        .arg(Combo_Act3->currentText()).arg(Combo_Dist3->currentText()));
}

// [新增] 辅助函数：根据动作类型动态填充参数列表
void UsblControlWindow::RefreshDistCombo(QComboBox* actCombo, QComboBox* distCombo)
{
    // 1. 获取当前选中的动作字符 (F, B, L, R)
    QChar actChar = actCombo->currentData().toChar();
    
    // 2. 清空现有选项
    distCombo->clear();

    // 3. 根据动作类型填充不同内容
    if (actChar == 'F' || actChar == 'B') {
        // --- 直线运动 (米) ---
        // 1~9 代表 5m~45m
        for(int i=1; i<=9; i++) {
            distCombo->addItem(QString("%1 米").arg(i*5), QChar('0' + i));
        }
    } 
    else if (actChar == 'L' || actChar == 'R') {
        // --- 转向运动 (度) ---
        // 1 代表 90度
        // 2 代表 180度
        // 3 代表 270度
        // 4 代表 360度
        for(int i=1; i<=4; i++) {
            distCombo->addItem(QString("%1 度").arg(i*90), QChar('0' + i));
        }
    }
    
    // 默认选中第一个
    if(distCombo->count() > 0) distCombo->setCurrentIndex(0);
}

// [新增] 槽函数实现
void UsblControlWindow::Slot_Act1_Changed(int index) { RefreshDistCombo(Combo_Act1, Combo_Dist1); }
void UsblControlWindow::Slot_Act2_Changed(int index) { RefreshDistCombo(Combo_Act2, Combo_Dist2); }
void UsblControlWindow::Slot_Act3_Changed(int index) { RefreshDistCombo(Combo_Act3, Combo_Dist3); }


QString UsblControlWindow::GetProtocolString(const QString &type, int gear) {
    if (type == "ZE") return "#60010408235A4524243030233BD5";
    QString result = "";
    // ... 其他指令保持不变 ...
    if (type == "FD") {
        switch (gear) {
            case 1: result = "#600104082346442424303123E655"; break;
            case 2: result = "#600104082346442424303223E6A5"; break;
            case 3: result = "#600104082346442424303323E7A5"; break;
            case 4: result = "#60010408234644242430342305E5"; break;
            case 5: result = "#60010408234644242430352395E4"; break;
        }
    } else if (type == "BD") {
        switch (gear) {
            case 1: result = "#600104082342442424303123A395"; break;
            case 2: result = "#600104082342442424303223A365"; break;
            case 3: result = "#600104082342442424303323A2F5"; break;
            case 4: result = "#600104082342442424303423A0C5"; break;
            case 5: result = "#600104082342442424303523A155"; break;
        }
    } else if (type == "LT") {
        switch (gear) {
            case 1: result = "#60010408234C5424243031234EC5"; break;
            case 2: result = "#60010408234C5424243032234E35"; break;
            case 3: result = "#60010408234C5424243033234FA5"; break;
            case 4: result = "#60010408234C5424243034234D95"; break;
            case 5: result = "#60010408234C5424243035234C05"; break;
        }
    } else if (type == "RT") {
        switch (gear) {
            case 1: result = "#600104082352542424303123B0C4"; break;
            case 2: result = "#600104082352542424303223B034"; break;
            case 3: result = "#600104082352542424303323B1A4"; break;
            case 4: result = "#600104082352542424303423B394"; break;
            case 5: result = "#600104082352542424303523B204"; break;
        }
    } else if (type == "UP") {
        switch (gear) {
            case 1: result = "#600104082355502424303123C780"; break;
            case 2: result = "#600104082355502424303223C770"; break;
            case 3: result = "#600104082355502424303323C6E0"; break;
            case 4: result = "#600104082355502424303423C4D0"; break;
            case 5: result = "#600104082355502424303523C540"; break;
        }
    } else if (type == "DN") {
        switch (gear) {
            case 1: result = "#6001040823444E2424303123C53F"; break;
            case 2: result = "#6001040823444E2424303223C5CF"; break;
            case 3: result = "#6001040823444E2424303323C45F"; break;
            case 4: result = "#6001040823444E2424303423C66F"; break;
            case 5: result = "#6001040823444E2424303523C7FF"; break;
        }
    }
    return result;
}
// 清除轨迹槽函数
void UsblControlWindow::Slot_Btn_ClearTrack_Clicked()
{
    // 调用 JS 中的 clearTrack() 函数
    MapView->page()->runJavaScript("clearTrack();");
    
    // 可以在这里打印日志
    qDebug() << "User cleared the map trajectory.";
}

// 槽函数绑定
void UsblControlWindow::Slot_Btn_Up_Clicked()       { SendUsblCommand("UP", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Down_Clicked()     { SendUsblCommand("DN", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Left_Clicked()     { SendUsblCommand("LT", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Right_Clicked()    { SendUsblCommand("RT", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Forward_Clicked()  { SendUsblCommand("FD", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Backward_Clicked() { SendUsblCommand("BD", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Stop_Clicked()     { SendUsblCommand("ZE", 0); }
