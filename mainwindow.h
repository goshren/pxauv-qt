#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QDateTime>
#include <QTimer>
#include "UsblControlWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStatusBar *StatusBar_ShowCurrentTime;  //创建状态栏，用于显示当前时间
    QString CurrentTime;                    //用于存放当前时间
    QTimer* Timer_UpdateCurrentTime;        //定义定时器，更新当前时间
    UsblControlWindow *usblWindow; // 声明新窗口指针

    bool TcpStatus;                         //Tcp连接状态
    bool UartStatus;                        //Uart连接状态
    bool IsUpdateBtnStatus;


public slots:
    void Slot_ShowCurrentTime();            //功能显示系统时间
    void Slot_TcpStatus(const bool status);
    void Slot_UartStatus(const bool status);
    void Slot_OpenUsblWindow();    // 打开新窗口的槽函数

signals:
    void Signal_UpdateBtnStatus(const bool IsUpdateBtnStatus);
    void Signal_UpdateRabtnTcpStatus(const bool TcpStatus);
    void Signal_UpdateRabtnUartStatus(const bool UartStatus);
    void Signal_UpdateBtnControlArray(const bool TcpStatus);
};
#endif // MAINWINDOW_H
