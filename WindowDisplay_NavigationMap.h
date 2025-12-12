#ifndef WINDOWDISPLAY_NAVIGATIONMAP_H
#define WINDOWDISPLAY_NAVIGATIONMAP_H

#include <QObject>
#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include <QGroupBox>

#include "bridge.h"

class WindowDisplay_NavigationMap : public QWidget
{
    Q_OBJECT
public:
    explicit WindowDisplay_NavigationMap(QWidget *parent = nullptr);

    QGroupBox *GroupBox_DisplayNavigationMap;
    QWebEngineView *Window_NavigationMap;
    bridge *JSBridge;

public slots:
    void Slot_LocationString(const QString &Longitude, const QString &Latitude);

signals:

private:
    QString last_longitude = "109.541";
    QString last_latitude = "18.319";

};

#endif // WINDOWDISPLAY_NAVIGATIONMAP_H
