#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>
#include <QDebug>
class bridge : public QObject
{
    Q_OBJECT
public:
    explicit bridge(QObject *parent = 0);

signals:   
    //void msgToJs(const QString& msg);       // 向js发送信号

public slots:
    //void RcvPoint(const QString &lng,const QString &lat);
};

#endif // BRIDGE_H


