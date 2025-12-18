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
#include <QTimer>

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
    QLineEdit *LineEdit_UsblX; 
    QLineEdit *LineEdit_UsblY; 
    QLineEdit *LineEdit_UsblZ; 
    QLabel *Label_UsblStatus;  

    // 目标绝对坐标显示控件
    QLineEdit *LineEdit_TargetLat; 
    QLineEdit *LineEdit_TargetLon; 

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
    QPushButton *Btn_QueryLoc; // 查询定位按钮

    // 6. 释放器控制区域
    QGroupBox *GroupBox_Releaser;
    QPushButton *Btn_Rel1_Open;
    QPushButton *Btn_Rel1_Close;
    QPushButton *Btn_Rel2_Open;
    QPushButton *Btn_Rel2_Close;
    // [新增] 7. 目标航路点设置 (导航)
    QGroupBox *GroupBox_Waypoint;
    QLineEdit *LineEdit_GoalLat; // 目标纬度
    QLineEdit *LineEdit_GoalLon; // 目标经度
    QPushButton *Btn_SendGoal;   // 发送按钮


// --- 右侧地图区域 ---
    QGroupBox *GroupBox_Map;
    QPushButton *Btn_ClearTrack; // [新增] 清除轨迹按钮
    QWebEngineView *MapView;     
    bridge *JSBridge;            
    QString last_longitude;      
    QString last_latitude;

// [新增] --- 右侧面板区域 ---
    QGroupBox *GroupBox_RightPanel; // 第3列容器
    
    // [新增] 8. 定深/定高控制 (放置在右侧面板)
    QGroupBox *GroupBox_AutoControl;
    
    // === 定深控件声明 ===
    QLabel *Label_DepthTitle;        
    QLineEdit *LineEdit_DepthVal;
    QPushButton *Btn_Depth_Open;
    QPushButton *Btn_Depth_Close;
    
    // === 定高控件声明 ===
    QLabel *Label_AltTitle;          
    QLineEdit *LineEdit_AltVal;
    QPushButton *Btn_Alt_Open;
    QPushButton *Btn_Alt_Close;

    // --- 逻辑变量 ---
    QSerialPort *UsblSerial;
    QSerialPort *GpsSerial;
    QTimer *m_queryTimer; // 定时器

    // --- 逻辑变量 ---
    double m_baseLatitude = 0.0;  
    double m_baseLongitude = 0.0; 
    bool m_hasGpsFix = false;     
    
    QByteArray m_usblBuffer; 

    // --- 辅助函数 ---
    void InitUi();           
    void SendUsblCommand(const QString &type, int gear); 
    void UpdateTargetMapPosition(const QString &lon, const QString &lat);
    QString GetProtocolString(const QString &type, int gear);
    
    // GPS相关
    void ParseNmeaData(const QByteArray &data);
    double NmeaToDecimal(const QString &nmeaStr);
    void UpdateMapPosition(const QString &lon, const QString &lat);

    // 计算目标经纬度函数
    void CalculateTargetGeoPos(float x_offset, float y_offset);

    // USBL解析相关
    void ParseSeatracData(const QString &hexString);

    // 发送解算后的坐标给AUV (带CRC校验，自动发送)
    void SendTargetPosToAUV(double lat, double lon);

    // [新增] 通用定深定高发送函数
    // type: "AD" or "AH", value: 数值字符串(如"1.5"), isEnable: true开启/false关闭
    void SendAutoControlCmd(const QString &type, const QString &value, bool isEnable);

    // [新增] 9. 预编程运动控制 (位于右侧面板下方)
    QGroupBox *GroupBox_Program;
    
    // 步骤1
    QLabel *Label_Step1;
    QComboBox *Combo_Act1;
    QComboBox *Combo_Dist1;
    
    // 步骤2
    QLabel *Label_Step2;
    QComboBox *Combo_Act2;
    QComboBox *Combo_Dist2;
    
    // 步骤3
    QLabel *Label_Step3;
    QComboBox *Combo_Act3;
    QComboBox *Combo_Dist3;
    
    QPushButton *Btn_SendProg; // 发送任务按钮

    void RefreshDistCombo(QComboBox* actCombo, QComboBox* distCombo);
    
    // CRC16计算函数
    uint16_t calculateCRC16(const QByteArray &data);

private slots:
    // 串口槽
    void Slot_Btn_OpenSerial_Clicked();
    void Slot_Btn_CloseSerial_Clicked();
    
    // 读取 USBL 数据槽函数
    void Slot_UsblReadData(); 
    
    // GPS 槽
    void Slot_Btn_OpenGps_Clicked();
    void Slot_Btn_CloseGps_Clicked();
    void Slot_GpsReadData(); 

    // 定时器超时槽函数
    void Slot_OnQueryTimerTimeout();

    // 运动控制槽
    void Slot_Btn_Up_Clicked();
    void Slot_Btn_Down_Clicked();
    void Slot_Btn_Left_Clicked();
    void Slot_Btn_Right_Clicked();
    void Slot_Btn_Forward_Clicked();
    void Slot_Btn_Backward_Clicked();
    void Slot_Btn_Stop_Clicked();
    void Slot_Btn_QueryLoc_Clicked();
    
    // 释放器控制槽
    void Slot_Btn_Rel1_Open_Clicked();
    void Slot_Btn_Rel1_Close_Clicked();
    void Slot_Btn_Rel2_Open_Clicked();
    void Slot_Btn_Rel2_Close_Clicked();

    void Slot_Btn_ClearTrack_Clicked();

    // [新增] 发送目标点按钮槽函数
    void Slot_Btn_SendGoal_Clicked();

    // [新增] 定深定高槽函数
    void Slot_Btn_Depth_Open_Clicked();
    void Slot_Btn_Depth_Close_Clicked();
    void Slot_Btn_Alt_Open_Clicked();
    void Slot_Btn_Alt_Close_Clicked();
    
    void Slot_Btn_SendProg_Clicked();
    // [新增] 动作下拉框改变时的槽函数
    void Slot_Act1_Changed(int index);
    void Slot_Act2_Changed(int index);
    void Slot_Act3_Changed(int index);
};

#endif // USBLCONTROLWINDOW_H