#ifndef INFORMATIONDISPLAY_CTD_H
#define INFORMATIONDISPLAY_CTD_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QDateTime>

class InformationDisplay_CTD : public QWidget
{
    Q_OBJECT
public:
    explicit InformationDisplay_CTD(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DisplayCTD;
    QLabel *Label_Temperature;
    QLabel *Label_Salinity;
    QLabel *Label_Depth;
    QLabel *Label_Conductivity;
    QLabel *Label_SoundVelocity;
    QLabel *Label_Density;

    QLabel *LabelVal_Temperature;
    QLabel *LabelVal_Salinity;
    QLabel *LabelVal_Depth;
    QLabel *LabelVal_Conductivity;
    QLabel *LabelVal_SoundVelocity;
    QLabel *LabelVal_Density;

    QString CurrentTime;                //用于存放当前时间
    QString PrintMessage;               //用于存放打印信息

public slots:
    void Slot_ShowCtdInformation(const QString &text);

signals:
    void Signal_PrintCtdInformation(const QString &text);

private:
    QString Data_Ctd;                   //用于接收未经处理的临时温盐深数据
    QString Data_Temperature;
    QString Data_Salinity;
    QString Data_Depth;
    QString Data_Conductivity;
    QString Data_SoundVelocity;
    QString Data_Density;
};

#endif // INFORMATIONDISPLAY_H
