QT       += core gui network serialport sql webenginewidgets  webchannel

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
ICONS=favicon.icns

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ControlBtnArray.cpp \
    DebugWindow.cpp \
    InformationDisplay_Battery.cpp \
    InformationDisplay_CTD.cpp \
    InformationDisplay_CollectionStatus.cpp \
    InformationDisplay_DVL.cpp \
    InformationDisplay_Location.cpp \
    InformationDisplay_MainControl.cpp \
    InformationDisplay_Pose.cpp \
    MotorControl.cpp \
    TcpSocketLink.cpp \
    Tool.cpp \
    UartLink.cpp \
    WindowDisplay_NavigationMap.cpp \
    bridge.cpp \
    main.cpp \
    mainwindow.cpp\
    UsblControlWindow.cpp


HEADERS += \
    ControlBtnArray.h \
    DebugWindow.h \
    InformationDisplay_Battery.h \
    InformationDisplay_CTD.h \
    InformationDisplay_CollectionStatus.h \
    InformationDisplay_DVL.h \
    InformationDisplay_Location.h \
    InformationDisplay_MainControl.h \
    InformationDisplay_Pose.h \
    MotorControl.h \
    TcpSocketLink.h \
    Tool.h \
    UartLink.h \
    WindowDisplay_NavigationMap.h \
    bridge.h \
    mainwindow.h\
    UsblControlWindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Baidu_JS/map.qrc \
    res.qrc
