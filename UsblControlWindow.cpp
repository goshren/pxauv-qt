#include "UsblControlWindow.h"
#include "ui_UsblControlWindow.h" 
#include <QDebug>
#include <QRegularExpression>
#include <QtMath> 

const double EARTH_RADIUS = 6378137.0;

UsblControlWindow::UsblControlWindow(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("USBL水声遥控 & 岸基定位系统");
    this->setFixedSize(1050, 860); 

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
    GroupBox_LeftPanel->setGeometry(5, 5, 440, 850); 
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

    GroupBox_Map = new QGroupBox("岸基位置实时显示", this);
    GroupBox_Map->setGeometry(450, 10, 590, 840); 
    
    MapView = new QWebEngineView(GroupBox_Map);
    MapView->setGeometry(10, 25, 570, 805);

    QWebChannel *channel = new QWebChannel(this);
    JSBridge = new bridge(this); 
    channel->registerObject("webBridge", JSBridge);
    MapView->page()->setWebChannel(channel);
    MapView->page()->load(QUrl("qrc:/map/BDMap.html")); 
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

    // 计算完成后，立即带校验下发
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
    QString strLon = QString::asprintf("%10.6f", lon);
    QString payloadStr = "+" + strLat + "+" + strLon + "+";

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

// 槽函数绑定
void UsblControlWindow::Slot_Btn_Up_Clicked()       { SendUsblCommand("UP", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Down_Clicked()     { SendUsblCommand("DN", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Left_Clicked()     { SendUsblCommand("LT", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Right_Clicked()    { SendUsblCommand("RT", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Forward_Clicked()  { SendUsblCommand("FD", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Backward_Clicked() { SendUsblCommand("BD", SpinBox_Speed->value()); }
void UsblControlWindow::Slot_Btn_Stop_Clicked()     { SendUsblCommand("ZE", 0); }