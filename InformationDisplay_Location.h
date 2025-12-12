#ifndef INFORMATIONDISPLAY_LOCATION_H
#define INFORMATIONDISPLAY_LOCATION_H

#include <QGroupBox>
#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QDateTime>
#include <QDebug>

class InformationDisplay_Location : public QWidget
{
    Q_OBJECT
public:
    explicit InformationDisplay_Location(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DisplayLocation;
    QLabel *Label_SystemFlag;
    QLabel *Label_SatelliteNum;
    QLabel *Label_Longitude;
    QLabel *Label_Latitude;

    QLabel *LabelVal_SystemFlag;
    QLabel *LabelVal_SatelliteNum;
    QLabel *LabelVal_Longitude;
    QLabel *LabelVal_Latitude;

    QString CurrentTime;                //用于存放当前时间
    QString PrintMessage;               //用于存放打印信息

public slots:
    void Slot_ShowLocationInformation(const QString &text);

signals:
    void Signal_PrintLocationInformation(const QString &text);
    void Signal_LocationString(const QString &Longitude, const QString &Latitude);

private:
    QString Data_Location;                   //用于接收未经处理的临时位置信息数据

    QString Temp_SystemFlag;
    QString Temp_IsValid;
    QString Temp_SatelliteNum;
    QString Temp_LongitudeDirection;
    QString Temp_Longitude;
    QString Temp_LatitudeDirection;
    QString Temp_Latitude;

    QString Data_Longitude;                  //经度
    QString Data_Latitude;                   //纬度


};

#endif // INFORMATIONDISPLAY_LOCATION_H
