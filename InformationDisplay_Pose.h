#ifndef INFORMATIONDISPLAY_POSE_H
#define INFORMATIONDISPLAY_POSE_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>

class InformationDisplay_Pose : public QWidget
{
    Q_OBJECT
public:
    explicit InformationDisplay_Pose(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DisplayPose;
    QLabel *Label_Roll;
    QLabel *Label_Pitch;
    QLabel *Label_Heading;

    QLabel *LabelVal_Roll;
    QLabel *LabelVal_Pitch;
    QLabel *LabelVal_Heading;

public slots:

    void Slot_ShowRollInformation(const QString &text);
    void Slot_ShowPitchInformation(const QString &text);
    void Slot_ShowHeadingInformation(const QString &text);

};




#endif // INFORMATIONDISPLAY_POSE_H
