#ifndef INFORMATIONDISPLAY_DVL_H
#define INFORMATIONDISPLAY_DVL_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QDateTime>

class InformationDisplay_DVL : public QWidget
{
    Q_OBJECT
public:
    explicit InformationDisplay_DVL(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DisplayDVL;
    QLabel *Label_Height;
    QLabel *Label_Depth;
    QLabel *Label_SpeedX;
    QLabel *Label_SpeedY;
    QLabel *Label_SpeedZ;

    QLabel *LabelVal_Height;
    QLabel *LabelVal_Depth;
    QLabel *LabelVal_SpeedX;
    QLabel *LabelVal_SpeedY;
    QLabel *LabelVal_SpeedZ;

    QString CurrentTime;                //用于存放当前时间
    QString PrintMessage;               //用于存放打印信息

public slots:
    void Slot_ShowDvlInformation(const QString &text);

signals:
    void Signal_PrintDvlInformation(const QString &text);

    //将解析后的数值传递到InformationDisplay_Pose类中对应的成员
    void Signal_LabelVal_Roll(const QString &text);
    void Signal_LabelVal_Pitch(const QString &text);
    void Signal_LabelVal_Heading(const QString &text);

private:

    QString Temp_Dvl;                   //用于接收未经处理的临时DVL数据
    QString Temp_Pitch;                 //俯仰数据
    QString Temp_Roll;                  //横滚数据
    QString Temp_Heading;               //航向数据
    QString Temp_SpeedX;                //X轴速度数据
    QString Temp_SpeedY;                //Y轴速度数据
    QString Temp_SpeedZ;                //Z轴速度数据
    QString Temp_TransducerEntryDepth;  //深度数据
    QString Temp_ButtomDistance;        //高度数据

    QString Data_Dvl;                   //用于接收未经处理的临时DVL数据
    QString Data_Pitch;                 //俯仰数据
    QString Data_Roll;                  //横滚数据
    QString Data_Heading;               //航向数据
    QString Data_SpeedX;                //X轴速度数据
    QString Data_SpeedY;                //Y轴速度数据
    QString Data_SpeedZ;                //Z轴速度数据
    QString Data_TransducerEntryDepth;  //深度数据
    QString Data_ButtomDistance;        //高度数据
};



#endif // INFORMATIONDISPLAY_DVL_H
