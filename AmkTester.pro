QT += core gui serialport widgets

TARGET = AmkTester
TEMPLATE = app
DESTDIR = $$_PRO_FILE_PWD_/bin

QMAKE_CXXFLAGS += /std:c++latest
QMAKE_CXXFLAGS += /await

DEFINES += \
    QT_DEPRECATED_WARNINGS \
    QT_DISABLE_DEPRECATED_BEFORE=0x060000 \    # disables all the APIs deprecated before Qt 6.0.0
    EL_ALWAYS_OPEN=1 \
    EL_LOG \

win32:RC_FILE = main_icon/myapp.rc

SOURCES += \
    autotest/autotest.cpp \
    autotest/autotestmodel.cpp \
    autotest/header.cpp \
    autotest/resultdialog.cpp \
    autotest/tableview.cpp \
    connection.cpp \
    devices/devices.cpp \
    devices/kds.cpp \
    devices/resistancematrix.cpp \
    devices/tester.cpp \
    kdsdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    pointedit.cpp \
    relayset.cpp \
    relaysetholder.cpp \
    resistanceview.cpp \


HEADERS += \
    autotest/autotest.h \
    autotest/autotestmodel.h \
    autotest/header.h \
    autotest/resultdialog.h \
    autotest/tableview.h \
    connection.h \
    devices/devices.h \
    devices/kds.h \
    devices/resistancematrix.h \
    devices/tester.h \
    kdsdialog.h \
    mainwindow.h \
    pointedit.h \
    relayset.h \
    relaysetholder.h \
    resistanceview.h \


FORMS += \
    autotest/resultdialog.ui \
    kdsdialog.ui \
    mainwindow.ui

include(MyProtokol/XrProtokol.pri)
include(ElemerDevice/ElemerDevice.pri)

INCLUDEPATH += range-v3/include

RESOURCES += \
    res/res.qrc
