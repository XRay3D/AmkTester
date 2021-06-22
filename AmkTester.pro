QT += core gui serialport widgets

TARGET = AmkTester

TEMPLATE = app

DESTDIR = $$_PRO_FILE_PWD_/bin

QMAKE_CXXFLAGS += \
    /std:c++latest \
    /await

CONFIG(debug, debug|release) {
    TARGET = $$TARGET"_D"
} else {
    TARGET = $$TARGET"_R"
}

DEFINES += \
    QT_DEPRECATED_WARNINGS \
    QT_DISABLE_DEPRECATED_BEFORE=0x060000 \    # disables all the APIs deprecated before Qt 6.0.0
    EL_ALWAYS_OPEN=1 \
    EL_LOG \
    FORCE_READ \

win32:RC_FILE = main_icon/myapp.rc

SOURCES += \
    autotest/autotest.cpp \
    autotest/autotestmodel.cpp \
    autotest/header.cpp \
    autotest/resultdialog.cpp \
    autotest/tableview.cpp \
    devices/devices.cpp \
    devices/resistancematrix.cpp \
    devices/tester.cpp \
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
    devices/devices.h \
    devices/resistancematrix.h \
    devices/tester.h \
    mainwindow.h \
    pointedit.h \
    relayset.h \
    relaysetholder.h \
    resistanceview.h \


FORMS += \
    autotest/resultdialog.ui \
    mainwindow.ui

RESOURCES += \
    res/res.qrc

include(XrProtokol/XrProtokol.pri)
include(ElemerDevice/ElemerDevice.pri)
include(../kds/kds.pri)

INCLUDEPATH += range-v3/include


