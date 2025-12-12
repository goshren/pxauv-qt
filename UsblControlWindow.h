#ifndef USBLCONTROLWINDOW_H
#define USBLCONTROLWINDOW_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QSpinBox>
#include <QDateTime>
#include <QLineEdit>

// Web与地图相关头文件
#include <QWebEngineView>
#include <QWebChannel>
#include "bridge.h"

class UsblControlWindow : public QWidget
{
    Q_OBJECT
public:
    explicit UsblControlWindow(QWidget *parent = nullptr);
    ~UsblControlWindow();

private:
    // --- 界面布局 ---
    QGroupBox *GroupBox_LeftPanel;
    
    // 1. USBL 串口配置
    QGroupBox *GroupBox_Serial;
    QComboBox *ComboBox_Port;
    QComboBox *ComboBox_Baud;
    QPushButton *Btn_OpenSerial;
    QPushButton *Btn_CloseSerial;

    // 2. GPS 串口配置
    QGroupBox *GroupBox_GpsSerial;
    QComboBox *ComboBox_GpsPort;
    QComboBox *ComboBox_GpsBaud;
    QPushButton *Btn_OpenGps;
    QPushButton *Btn_CloseGps;

    // 3. GPS 文本显示
    QGroupBox *GroupBox_GpsInfo;
    QLineEdit *LineEdit_GpsLat;
    QLineEdit *LineEdit_GpsLon;
    QLabel *Label_GpsTime;

    // 4. USBL 相对定位数据显示
    QGroupBox *GroupBox_UsblPos;
    QLineEdit *LineEdit_UsblX; // 东向 X
    QLineEdit *LineEdit_UsblY; // 北向 Y
    QLineEdit *LineEdit_UsblZ; // 深度 Z
    QLabel *Label_UsblStatus;  // 解析状态提示

    // 5. 运动控制
    QGroupBox *GroupBox_Control;
    QPushButton *Btn_Up;
    QPushButton *Btn_Down;
    QPushButton *Btn_Left;
    QPushButton *Btn_Right;
    QPushButton *Btn_Forward;
    QPushButton *Btn_Backward;
    QPushButton *Btn_Stop;
    QSpinBox *SpinBox_Speed;
    QLabel *Label_Speed;

    // --- 【新增】目标绝对坐标显示控件 ---
    QLineEdit *LineEdit_TargetLat; // 目标纬度
    QLineEdit *LineEdit_TargetLon; // 目标经度
    
    QPushButton *Btn_QueryLoc; // 查询定位按钮

    // --- 右侧地图区域 ---
    QGroupBox *GroupBox_Map;
    QWebEngineView *MapView;     
    bridge *JSBridge;            
    QString last_longitude;      
    QString last_latitude;

    // --- 逻辑变量 ---
    QSerialPort *UsblSerial;
    QSerialPort *GpsSerial;

    // --- 【新增】逻辑变量 ---
    double m_baseLatitude = 0.0;  // 岸基纬度 (十进制)
    double m_baseLongitude = 0.0; // 岸基经度 (十进制)
    bool m_hasGpsFix = false;     // 是否已获取有效GPS
    
    // 【新增】USBL数据缓冲区
    QByteArray m_usblBuffer; 

    // --- 辅助函数 ---
    void InitUi();           
    void SendUsblCommand(const QString &type, int gear); 
        // 在 private: 区域的辅助函数部分，增加一行：
    void UpdateTargetMapPosition(const QString &lon, const QString &lat);
    QString GetProtocolString(const QString &type, int gear);
    
    // GPS相关
    void ParseNmeaData(const QByteArray &data);
    double NmeaToDecimal(const QString &nmeaStr);
    void UpdateMapPosition(const QString &lon, const QString &lat);

    // 【新增】计算目标经纬度函数
    void CalculateTargetGeoPos(float x_offset, float y_offset);

    // USBL解析相关
    void ParseSeatracData(const QString &hexString);


private slots:
    // 串口槽
    void Slot_Btn_OpenSerial_Clicked();
    void Slot_Btn_CloseSerial_Clicked();
    
    // 【修改】读取 USBL 数据槽函数
    void Slot_UsblReadData(); 
    
    // GPS 槽
    void Slot_Btn_OpenGps_Clicked();
    void Slot_Btn_CloseGps_Clicked();
    void Slot_GpsReadData(); 

    // 运动控制槽
    void Slot_Btn_Up_Clicked();
    void Slot_Btn_Down_Clicked();
    void Slot_Btn_Left_Clicked();
    void Slot_Btn_Right_Clicked();
    void Slot_Btn_Forward_Clicked();
    void Slot_Btn_Backward_Clicked();
    void Slot_Btn_Stop_Clicked();
    
    void Slot_Btn_QueryLoc_Clicked();
};

#endif // USBLCONTROLWINDOW_H