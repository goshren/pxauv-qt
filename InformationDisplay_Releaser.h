#ifndef INFORMATIONDISPLAY_RELEASE_H
#define INFORMATIONDISPLAY_RELEASE_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

class InformationDisplay_Releaser : public QWidget
{
    Q_OBJECT
public:
    explicit InformationDisplay_Releaser(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DisplayReleaser;
    QLabel *Label_Releaser1Status;              //释放器1标签
    QLabel *LabelVal_Releaser1Status;        //释放器1状态数值

    QLabel *Label_Releaser2Status;              //释放器2标签
    QLabel *LabelVal_Releaser2Status;        //释放器2状态数值

    bool Releaser1Status = false;
    bool Releaser2Status = false;

public slots:
    void Slot_ShowReleaserStatus(const QString &text);

signals:
    void Signal_UpdataRelease1Status(const bool Releaser1Status);
    void Signal_UpdataRelease2Status(const bool Releaser2Status);



};

#endif // INFORMATIONDISPLAY_RELEASE_H
