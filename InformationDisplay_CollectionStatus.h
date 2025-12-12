#ifndef INFORMATIONDISPLAY_COLLECTIONSTATUS_H
#define INFORMATIONDISPLAY_COLLECTIONSTATUS_H

#include <QGroupBox>
#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QDateTime>

class InformationDisplay_CollectionStatus : public QWidget
{
    Q_OBJECT
public:
    explicit InformationDisplay_CollectionStatus(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DisplayCollectionStatus;
    QLabel *Label_CollectionStatus;
    QLabel *Label_EmergencyPower;
    QLabel *LabelVal_CollectionStatus;
    QLabel *LabelVal_EmergencyPower;

    QString CurrentTime;                //用于存放当前时间
    QString PrintMessage;               //用于存放打印信息

    bool CollectionStatus = false;

public slots:
    void Slot_ShowCollectionStatus(const QString &text);

signals:
    void Signal_UpdateCollectionStatus(const bool CollectionStatus);
    void Signal_PrintCollectionStatus(const QString &text);

private:
    QString Temp_CollectionStatus;      //用于存放采集舱状态
    QString Temp_EmergencyPower;        //用于紧急电源状态
};

#endif // INFORMATIONDISPLAY_COLLECTIONSTATUS_H
