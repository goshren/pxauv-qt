#include "DebugWindow.h"
#include <QDebug>

DebugWindow::DebugWindow(QWidget *parent) : QWidget(parent)
{
    //定义控件
    GroupBox_DebugWindow = new QGroupBox("调试信息",this);
    TextEdit_ShowText = new QTextEdit(this);
    Btn_SendMsg = new QPushButton("发送", this);
    Btn_SaveMsg = new QPushButton("保存信息", this);
    LineEdit_DebugMsg = new QLineEdit(this);

    //设置参数和初始化
    Btn_SendMsg->setEnabled(false);
    TextEdit_ShowText->setReadOnly(true);

    //设定 地理位置 以及 部件大小
    GroupBox_DebugWindow->setGeometry(0,0,578,311);
    TextEdit_ShowText->setGeometry(12,30,554,225);
    Btn_SaveMsg->setGeometry(12,265,80,37);
    Btn_SendMsg->setGeometry(486,265,80,37);
    LineEdit_DebugMsg->setGeometry(104,265,370,37);

    //连接信号和槽函数
    connect(Btn_SendMsg, &QPushButton::clicked, this, &DebugWindow::Slot_Btn_SendMsg_Clicked);
    connect(Btn_SaveMsg, &QPushButton::clicked, this, &DebugWindow::Slot_Btn_SaveMsg_Clicked);

}

void DebugWindow::Slot_PrintToDebug(const QString &text)
{
    TextEdit_ShowText->append(text);
}

void DebugWindow::Slot_UpdateBtnStatus(const bool status)
{
    if(status == true)
    {
       Btn_SendMsg->setEnabled(true);
    }
    else
    {
       Btn_SendMsg->setEnabled(false);
    }
}

void DebugWindow::Slot_RabtnTcpStatus(const bool status)
{
    RabtnTcpStatus = status;
    if(RabtnTcpStatus == true)
    {
        Btn_SendMsg->setText("TCP发送");
    }
}

void DebugWindow::Slot_RabtnUartStatus(const bool status)
{
    RabtnUartStatus = status;
    if(RabtnUartStatus == true)
    {
        Btn_SendMsg->setText("Uart发送");
    }
}

void DebugWindow::Slot_Btn_SendMsg_Clicked()
{
    LineEditText = LineEdit_DebugMsg->text();
    if(RabtnTcpStatus == true)
    {
        emit Signal_TcpSendMsg(LineEditText);
    }

    if(RabtnUartStatus == true)
    {
        emit Signal_UartSendMsg(LineEditText);
    }
}

void DebugWindow::Slot_Btn_SaveMsg_Clicked()
{
    CurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    FileName = QFileDialog::getSaveFileName(this, tr("保存文件"), CurrentTime + ".txt", tr("文本文件 (*.txt)"));
    QSaveFile File(FileName);

    if(!File.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        TextEdit_ShowText->append(CurrentTime + '\n'+ "系统提示：文件保存失败！" + '\n');
        return;
    }

    QByteArray Data = TextEdit_ShowText->toPlainText().toUtf8();

    //写入数据
    File.write(Data);

    // 提交写入
    if (!File.commit())
    {
       TextEdit_ShowText->append(CurrentTime + '\n'+ "系统提示：文件保存失败！" + '\n');
    }
    else
    {
        TextEdit_ShowText->append(CurrentTime + '\n'+ "系统提示：文件保存成功！" + '\n');
    }
}
