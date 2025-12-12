#include "UsblControlWindow.h"
#include <QDebug>
#include <QRegularExpression>
#include <QtMath> // 用于 qCos, M_PI

// 地球半径 (米)
const double EARTH_RADIUS = 6378137.0;

UsblControlWindow::UsblControlWindow(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("USBL水声遥控 & 岸基定位系统");
    // 调整窗口高度以容纳所有信息
    this->setFixedSize(1050, 780); 

    UsblSerial = new QSerialPort(this);
    GpsSerial = new QSerialPort(this);
    
    // 初始化逻辑变量
    m_baseLatitude = 0.0;
    m_baseLongitude = 0.0;
    m_hasGpsFix = false;
    
    InitUi();

    // 自动扫描并填充串口列表
    QList<QSerialPortInfo> serials = QSerialPortInfo::availablePorts();
    QStringList portList;
    for(const QSerialPortInfo &info : serials) {
        portList << info.portName();
    }
    ComboBox_Port->addItems(portList);
    ComboBox_GpsPort->addItems(portList);
    
    // 填充波特率
    QStringList baudRates = {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
    ComboBox_Baud->addItems(baudRates);
    ComboBox_Baud->setCurrentText("9600");
    ComboBox_GpsBaud->addItems(baudRates);
    ComboBox_GpsBaud->setCurrentText("9600");

    // --- 连接信号槽 (USBL 串口) ---
    connect(Btn_OpenSerial, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_OpenSerial_Clicked);
    connect(Btn_CloseSerial, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_CloseSerial_Clicked);
    // 连接 USBL 数据读取信号 (带缓冲区处理)
    connect(UsblSerial, &QSerialPort::readyRead, this, &UsblControlWindow::Slot_UsblReadData);

    // --- 连接信号槽 (GPS 串口) ---
    connect(Btn_OpenGps, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_OpenGps_Clicked);
    connect(Btn_CloseGps, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_CloseGps_Clicked);
    connect(GpsSerial, &QSerialPort::readyRead, this, &UsblControlWindow::Slot_GpsReadData);

    // --- 连接信号槽 (运动控制) ---
    connect(Btn_Up, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Up_Clicked);
    connect(Btn_Down, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Down_Clicked);
    connect(Btn_Left, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Left_Clicked);
    connect(Btn_Right, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Right_Clicked);
    connect(Btn_Forward, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Forward_Clicked);
    connect(Btn_Backward, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Backward_Clicked);
    connect(Btn_Stop, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_Stop_Clicked);
    
    // 连接查询定位按钮
    connect(Btn_QueryLoc, &QPushButton::clicked, this, &UsblControlWindow::Slot_Btn_QueryLoc_Clicked);
}

UsblControlWindow::~UsblControlWindow()
{
    if(UsblSerial->isOpen()) UsblSerial->close();
    if(GpsSerial->isOpen()) GpsSerial->close();
}

void UsblControlWindow::InitUi()
{
    // === 左侧面板：控制与配置 ===
    GroupBox_LeftPanel = new QGroupBox(this);
    GroupBox_LeftPanel->setGeometry(5, 5, 440, 770); 
    GroupBox_LeftPanel->setTitle(""); 
    GroupBox_LeftPanel->setStyleSheet("QGroupBox{border:none;}"); 

    // 1. USBL 串口配置
    GroupBox_Serial = new QGroupBox("1. USBL通信设置", GroupBox_LeftPanel);
    GroupBox_Serial->setGeometry(5, 0, 430, 80);
    
    QLabel *lblPort1 = new QLabel("端口:", GroupBox_Serial); lblPort1->setGeometry(10, 30, 40, 25);
    ComboBox_Port = new QComboBox(GroupBox_Serial); ComboBox_Port->setGeometry(50, 30, 100, 25);
    QLabel *lblBaud1 = new QLabel("波特率:", GroupBox_Serial); lblBaud1->setGeometry(160, 30, 50, 25);
    ComboBox_Baud = new QComboBox(GroupBox_Serial); ComboBox_Baud->setGeometry(215, 30, 80, 25);
    Btn_OpenSerial = new QPushButton("打开", GroupBox_Serial); Btn_OpenSerial->setGeometry(310, 25, 50, 30); Btn_OpenSerial->setStyleSheet("color: green; font-weight: bold;");
    Btn_CloseSerial = new QPushButton("关闭", GroupBox_Serial); Btn_CloseSerial->setGeometry(370, 25, 50, 30); Btn_CloseSerial->setEnabled(false);

    // 2. GPS 串口配置
    GroupBox_GpsSerial = new QGroupBox("2. GPS通信设置", GroupBox_LeftPanel);
    GroupBox_GpsSerial->setGeometry(5, 85, 430, 80);
    
    QLabel *lblPort2 = new QLabel("端口:", GroupBox_GpsSerial); lblPort2->setGeometry(10, 30, 40, 25);
    ComboBox_GpsPort = new QComboBox(GroupBox_GpsSerial); ComboBox_GpsPort->setGeometry(50, 30, 100, 25);
    QLabel *lblBaud2 = new QLabel("波特率:", GroupBox_GpsSerial); lblBaud2->setGeometry(160, 30, 50, 25);
    ComboBox_GpsBaud = new QComboBox(GroupBox_GpsSerial); ComboBox_GpsBaud->setGeometry(215, 30, 80, 25);
    Btn_OpenGps = new QPushButton("打开", GroupBox_GpsSerial); Btn_OpenGps->setGeometry(310, 25, 50, 30); Btn_OpenGps->setStyleSheet("color: blue; font-weight: bold;");
    Btn_CloseGps = new QPushButton("关闭", GroupBox_GpsSerial); Btn_CloseGps->setGeometry(370, 25, 50, 30); Btn_CloseGps->setEnabled(false);

    // 3. GPS 数据显示
    GroupBox_GpsInfo = new QGroupBox("3. GPS定位数据 (基站)", GroupBox_LeftPanel);
    GroupBox_GpsInfo->setGeometry(5, 170, 430, 90);
    
    QLabel *lblLat = new QLabel("纬度:", GroupBox_GpsInfo); lblLat->setGeometry(20, 25, 40, 25);
    LineEdit_GpsLat = new QLineEdit(GroupBox_GpsInfo); LineEdit_GpsLat->setGeometry(60, 25, 140, 25); LineEdit_GpsLat->setReadOnly(true);
    QLabel *lblLon = new QLabel("经度:", GroupBox_GpsInfo); lblLon->setGeometry(220, 25, 40, 25);
    LineEdit_GpsLon = new QLineEdit(GroupBox_GpsInfo); LineEdit_GpsLon->setGeometry(260, 25, 140, 25); LineEdit_GpsLon->setReadOnly(true);
    Label_GpsTime = new QLabel("UTC时间: --:--:--", GroupBox_GpsInfo); Label_GpsTime->setGeometry(20, 55, 300, 25); Label_GpsTime->setStyleSheet("color: gray;");

    // 4. USBL 定位信息 (相对 + 绝对)
    GroupBox_UsblPos = new QGroupBox("4. 水下信标定位信息", GroupBox_LeftPanel);
    GroupBox_UsblPos->setGeometry(5, 265, 430, 140);

    // 第一行：相对坐标
    QLabel *lblRel = new QLabel("相对坐标(米):", GroupBox_UsblPos);
    lblRel->setGeometry(10, 30, 90, 25);
    lblRel->setStyleSheet("font-weight: bold; color: #333;");

    QLabel *lblX = new QLabel("X:", GroupBox_UsblPos); lblX->setGeometry(100, 30, 15, 25);
    LineEdit_UsblX = new QLineEdit("0.0", GroupBox_UsblPos); LineEdit_UsblX->setGeometry(120, 30, 60, 25);
    LineEdit_UsblX->setReadOnly(true); LineEdit_UsblX->setAlignment(Qt::AlignCenter);

    QLabel *lblY = new QLabel("Y:", GroupBox_UsblPos); lblY->setGeometry(190, 30, 15, 25);
    LineEdit_UsblY = new QLineEdit("0.0", GroupBox_UsblPos); LineEdit_UsblY->setGeometry(210, 30, 60, 25);
    LineEdit_UsblY->setReadOnly(true); LineEdit_UsblY->setAlignment(Qt::AlignCenter);

    QLabel *lblZ = new QLabel("Z:", GroupBox_UsblPos); lblZ->setGeometry(280, 30, 15, 25);
    LineEdit_UsblZ = new QLineEdit("0.0", GroupBox_UsblPos); LineEdit_UsblZ->setGeometry(300, 30, 60, 25);
    LineEdit_UsblZ->setReadOnly(true); LineEdit_UsblZ->setAlignment(Qt::AlignCenter);

    // 第二行：绝对经纬度 (计算得出)
    QLabel *lblAbs = new QLabel("信标经纬度:", GroupBox_UsblPos);
    lblAbs->setGeometry(10, 70, 90, 25);
    lblAbs->setStyleSheet("font-weight: bold; color: darkblue;");

    QLabel *lblTLat = new QLabel("纬度:", GroupBox_UsblPos); lblTLat->setGeometry(100, 70, 40, 25);
    LineEdit_TargetLat = new QLineEdit("-", GroupBox_UsblPos); 
    LineEdit_TargetLat->setGeometry(140, 70, 90, 25);
    LineEdit_TargetLat->setReadOnly(true);
    LineEdit_TargetLat->setStyleSheet("color: blue; font-weight: bold;");

    QLabel *lblTLon = new QLabel("经度:", GroupBox_UsblPos); lblTLon->setGeometry(240, 70, 40, 25);
    LineEdit_TargetLon = new QLineEdit("-", GroupBox_UsblPos); 
    LineEdit_TargetLon->setGeometry(280, 70, 90, 25);
    LineEdit_TargetLon->setReadOnly(true);
    LineEdit_TargetLon->setStyleSheet("color: blue; font-weight: bold;");
    
    Label_UsblStatus = new QLabel("等待数据...", GroupBox_UsblPos);
    Label_UsblStatus->setGeometry(20, 110, 350, 20);
    Label_UsblStatus->setStyleSheet("color: gray; font-size: 10px;");

    // 5. 运动控制
    GroupBox_Control = new QGroupBox("5. 水声遥控指令", GroupBox_LeftPanel);
    GroupBox_Control->setGeometry(5, 415, 430, 290); 
    GroupBox_Control->setEnabled(false); 

    int cx = 215; // Center X
    int startY = 30; 
    int btnW = 90; int btnH = 35; int gap = 40;

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
    
    // 查询水下定位按钮
    Btn_QueryLoc = new QPushButton("查询水下定位 (Ping)", GroupBox_Control);
    Btn_QueryLoc->setGeometry(cx - 75, startY + gap*5 + 5, 150, 40); 
    Btn_QueryLoc->setStyleSheet("color: blue; font-weight: bold; border: 1px solid blue; border-radius: 5px;");

    // === 右侧面板：地图显示 ===
    GroupBox_Map = new QGroupBox("岸基位置实时显示", this);
    GroupBox_Map->setGeometry(450, 10, 590, 760);
    
    MapView = new QWebEngineView(GroupBox_Map);
    MapView->setGeometry(10, 25, 570, 725);

    QWebChannel *channel = new QWebChannel(this);
    JSBridge = new bridge(this); 
    channel->registerObject("webBridge", JSBridge);
    MapView->page()->setWebChannel(channel);
    MapView->page()->load(QUrl("qrc:/map/BDMap.html")); 
}

// ================== USBL 读取与解析核心逻辑 ==================

void UsblControlWindow::Slot_UsblReadData()
{
    // 1. 将新数据追加到缓冲区
    m_usblBuffer.append(UsblSerial->readAll());

    // 2. 循环处理缓冲区中的每一行数据 (以 \n 分割)
    while (m_usblBuffer.contains('\n')) {
        int lineEndIndex = m_usblBuffer.indexOf('\n');
        
        // 提取一行 (移除 \r \n)
        QByteArray lineData = m_usblBuffer.left(lineEndIndex).trimmed();
        
        // 从缓冲区移除已处理的行 (包括 \n)
        m_usblBuffer.remove(0, lineEndIndex + 1);

        QString lineString = QString(lineData);

        // 3. 过滤：只处理以 $39 或 #39 开头的数据
        if (lineString.startsWith("$39") || lineString.startsWith("#39")) {
            qDebug() << "Captured USBL Fix Data:" << lineString;
            ParseSeatracData(lineString);
        }
    }
}

void UsblControlWindow::ParseSeatracData(const QString &hexString)
{
    // 1. 去除头部标识符
    QString cleanHex = hexString;
    if (cleanHex.startsWith('$') || cleanHex.startsWith('#')) {
        cleanHex = cleanHex.mid(1);
    }

    // 2. 转换为字节数组
    QByteArray buf = QByteArray::fromHex(cleanHex.toLatin1());
    
    int len = buf.size();
    if (len < 5) return;

    // 3. 检查 CID 是否为 0x39 (CID_XCVR_FIX)
    if ((uint8_t)buf[0] != 0x39) return;

    // --- 开始解析 Payload ---
    int ptr = 1; 
    ptr += 2; // 跳过 DestID, SrcID

    // FLAGS (1 byte)
    uint8_t flags = (uint8_t)buf[ptr++];
    // MSG_TYPE (1 byte) 跳过
    ptr++; 

    // --- 跳过基础字段 (固定 12 字节: Attitude, Depth, VOS, RSSI) ---
    ptr += 12;

    // --- 处理 Range 字段 (Bit 0) ---
    if (flags & 0x01) {
        ptr += 10; 
    }

    // --- 处理 USBL 字段 (Bit 1) ---
    if (flags & 0x02) {
        if (ptr >= len) return;
        uint8_t channels = (uint8_t)buf[ptr++];
        // 计算块大小: 2*ch + 6
        int usbl_block_size = (2 * channels) + 6;
        ptr += usbl_block_size;
    }

    // --- 处理 Position 字段 (Bit 2) ---
    if (flags & 0x04) {
        // 确保剩余数据足够 (6字节: East, North, Depth)
        if (ptr + 6 <= len) {
            // 解析 Easting (X) - INT16 Little Endian
            int16_t easting_raw = (int16_t)((uint8_t)buf[ptr] | ((uint8_t)buf[ptr+1] << 8));
            float easting_x = easting_raw / 10.0f; // 分米转米
            ptr += 2;

            // 解析 Northing (Y)
            int16_t northing_raw = (int16_t)((uint8_t)buf[ptr] | ((uint8_t)buf[ptr+1] << 8));
            float northing_y = northing_raw / 10.0f;
            ptr += 2;

            // 解析 Depth (Z)
            int16_t depth_raw = (int16_t)((uint8_t)buf[ptr] | ((uint8_t)buf[ptr+1] << 8));
            float depth_z = depth_raw / 10.0f;
            ptr += 2;

            // 更新 UI - 相对坐标
            LineEdit_UsblX->setText(QString::number(easting_x, 'f', 1));
            LineEdit_UsblY->setText(QString::number(northing_y, 'f', 1));
            LineEdit_UsblZ->setText(QString::number(depth_z, 'f', 1));
            
            // 计算并更新绝对经纬度
            if (m_hasGpsFix) {
                CalculateTargetGeoPos(easting_x, northing_y);
                Label_UsblStatus->setText("解析成功(含绝对坐标) " + QDateTime::currentDateTime().toString("HH:mm:ss"));
            } else {
                Label_UsblStatus->setText("解析成功(等待GPS...) " + QDateTime::currentDateTime().toString("HH:mm:ss"));
                LineEdit_TargetLat->setText("GPS无效");
                LineEdit_TargetLon->setText("GPS无效");
            }
            
            qDebug() << "USBL Pos -> X:" << easting_x << " Y:" << northing_y << " Z:" << depth_z;
        } else {
            Label_UsblStatus->setText("解析错误: 数据不完整");
        }
    } else {
        Label_UsblStatus->setText("数据包不包含位置信息");
    }
}

// 【修正】根据相对位移计算目标经纬度
void UsblControlWindow::CalculateTargetGeoPos(float x_offset, float y_offset)
{
    // 简单平面近似计算 (短距离适用)
    
    // 1. 计算纬度变化 (dLat)
    // 纬度每度长度 ≈ 111319.9米 (地球周长/360)
    // 或者用公式: dLat(弧度) = y / R
    double dLat = (y_offset / EARTH_RADIUS) * (180.0 / M_PI);
    double targetLat = m_baseLatitude + dLat;

    // 2. 计算经度变化 (dLon)
    // 经度每度长度随纬度变化 = 111319.9 * cos(lat)
    double radLat = m_baseLatitude * (M_PI / 180.0);
    double dLon = (x_offset / (EARTH_RADIUS * qCos(radLat))) * (180.0 / M_PI);
    double targetLon = m_baseLongitude + dLon;

    // 3. 更新界面
    LineEdit_TargetLat->setText(QString::number(targetLat, 'f', 6));
    LineEdit_TargetLon->setText(QString::number(targetLon, 'f', 6));

    // 4. 更新地图上的信标位置 (传入经度, 纬度)
    UpdateTargetMapPosition(QString::number(targetLon, 'f', 6), QString::number(targetLat, 'f', 6));
}

// ================== GPS 数据解析 ==================

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
    if (nmea.contains("GGA")) { // BDGGA, GPGGA, GNGGA
        QStringList parts = nmea.split(',');
        if (parts.size() < 6) return;

        QString time = parts[1];
        QString latNmea = parts[2];
        QString latDir = parts[3];
        QString lonNmea = parts[4];
        QString lonDir = parts[5];

        if(latNmea.isEmpty() || lonNmea.isEmpty()) {
            m_hasGpsFix = false; // 丢失定位
            return;
        }

        // 显示时间
        if(time.size() >= 6) {
             Label_GpsTime->setText("UTC时间: " + time.mid(0,2) + ":" + time.mid(2,2) + ":" + time.mid(4,2));
        }
        // 显示 NMEA 原始格式
        LineEdit_GpsLat->setText(latNmea + " " + latDir);
        LineEdit_GpsLon->setText(lonNmea + " " + lonDir);

        // 转换为十进制并保存
        double dLat = NmeaToDecimal(latNmea);
        double dLon = NmeaToDecimal(lonNmea);
        if(latDir == "S") dLat = -dLat;
        if(lonDir == "W") dLon = -dLon;

        m_baseLatitude = dLat;
        m_baseLongitude = dLon;
        m_hasGpsFix = true; // 标记定位有效

        // 更新地图上的"岸基"位置
        UpdateMapPosition(QString::number(dLon, 'f', 6), QString::number(dLat, 'f', 6));
    }
}

double UsblControlWindow::NmeaToDecimal(const QString &nmeaStr)
{
    if(nmeaStr.isEmpty()) return 0.0;
    int dotPos = nmeaStr.indexOf('.');
    if(dotPos < 2) return 0.0;
    // ddmm.mmmmm -> dd + mm.mmmmm/60
    double minutes = nmeaStr.mid(dotPos - 2).toDouble();
    double degrees = nmeaStr.left(dotPos - 2).toDouble();
    return degrees + (minutes / 60.0);
}

void UsblControlWindow::UpdateMapPosition(const QString &lon, const QString &lat)
{
    // 只有坐标变化时才更新，减少刷新频率
    if(lon == last_longitude && lat == last_latitude) return;
    
    last_longitude = lon;
    last_latitude = lat;

    // 调用 JS 的 setBasePoint
    QString cmd = QString("setBasePoint(%1, %2)").arg(lon).arg(lat);
    MapView->page()->runJavaScript(cmd);
}


// ================== 串口与按钮槽函数 ==================

void UsblControlWindow::Slot_Btn_OpenSerial_Clicked() {
    UsblSerial->setPortName(ComboBox_Port->currentText());
    UsblSerial->setBaudRate(ComboBox_Baud->currentText().toInt());
    UsblSerial->setDataBits(QSerialPort::Data8);
    UsblSerial->setParity(QSerialPort::NoParity);
    UsblSerial->setStopBits(QSerialPort::OneStop);
    UsblSerial->setFlowControl(QSerialPort::NoFlowControl);
    
    m_usblBuffer.clear(); // 打开前清空缓冲区

    if(UsblSerial->open(QIODevice::ReadWrite)) {
        Btn_OpenSerial->setEnabled(false);
        Btn_CloseSerial->setEnabled(true);
        GroupBox_Control->setEnabled(true);
        ComboBox_Port->setEnabled(false);
        ComboBox_Baud->setEnabled(false);
    } else {
        QMessageBox::critical(this, "错误", "无法打开USBL串口！");
    }
}

void UsblControlWindow::Slot_Btn_CloseSerial_Clicked() {
    if(UsblSerial->isOpen()) UsblSerial->close();
    Btn_OpenSerial->setEnabled(true);
    Btn_CloseSerial->setEnabled(false);
    GroupBox_Control->setEnabled(false);
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
        return;
    }
    // 发送查询指令 #4001040187 + \r\n
    QString cmd = "#4001040187\r\n"; 
    UsblSerial->write(cmd.toUtf8());
    qDebug() << "USBL TX (Query Loc):" << cmd.trimmed();
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
    // 调用 JS 的 setTargetPoint
    QString cmd = QString("setTargetPoint(%1, %2)").arg(lon).arg(lat);
    MapView->page()->runJavaScript(cmd);
}

QString UsblControlWindow::GetProtocolString(const QString &type, int gear) {
    if (type == "ZE") return "#60010408235A4524243030233BD5";
    QString result = "";
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

// 槽函数绑定
void UsblControlWindow::Slot_Btn_Up_Clicked()       { SendUsblCommand("UP", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Down_Clicked()     { SendUsblCommand("DN", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Left_Clicked()     { SendUsblCommand("LT", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Right_Clicked()    { SendUsblCommand("RT", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Forward_Clicked()  { SendUsblCommand("FD", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Backward_Clicked() { SendUsblCommand("BD", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Stop_Clicked()     { SendUsblCommand("ZE", 0); }