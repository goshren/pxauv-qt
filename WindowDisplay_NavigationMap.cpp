#include "WindowDisplay_NavigationMap.h"

#include <QDebug>

WindowDisplay_NavigationMap::WindowDisplay_NavigationMap(QWidget *parent) : QWidget(parent)
{
    //定义显示信息的控件
    GroupBox_DisplayNavigationMap = new QGroupBox("导航地图",this);
    Window_NavigationMap = new QWebEngineView(this);

    //定义大小和位置
    GroupBox_DisplayNavigationMap->setGeometry(0,0,880,469);
    Window_NavigationMap->setGeometry(14,35,852,399+25);

    //加入百度地图
    QWebChannel *channel = new QWebChannel(this);
    bridge *bridgeObject = new bridge(this);

    channel->registerObject("webBridge", bridgeObject);
    /* 设置通信桥梁 */
    Window_NavigationMap->page()->setWebChannel(channel);
    /* 加载百度地图网页 */
    Window_NavigationMap->page()->load(QUrl("qrc:/map/BDMap.html"));   
    /* 发送位置信息到百度地图 */

}

void WindowDisplay_NavigationMap::Slot_LocationString(const QString &Longitude, const QString &Latitude)
{
    QString cmd = QString("setPoint(%1,%2)").arg(Longitude).arg(Latitude);

    if(last_latitude != Latitude || last_longitude != Longitude)
    {
        qDebug() << "位置发生变化";
        qDebug() << "经度:" << Longitude << "纬度:" << Latitude << endl;
        Window_NavigationMap->page()->runJavaScript(cmd);

    }

    last_latitude = Latitude;
    last_longitude = Longitude;
}
