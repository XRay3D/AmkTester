QT += core gui serialport widgets

TARGET = AmkTester
TEMPLATE = app
DESTDIR = $$_PRO_FILE_PWD_/bin

QMAKE_CXXFLAGS += /std:c++latest
QMAKE_CXXFLAGS += /await

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += \
    QT_DISABLE_DEPRECATED_BEFORE=0x060000 \    # disables all the APIs deprecated before Qt 6.0.0
    EL_ALWAYS_OPEN=1 \
#    EL_LOG \

win32:RC_FILE = main_icon/myapp.rc

SOURCES += \
    amktest/pointedit.cpp \
    autotest/header.cpp \
    autotest/model.cpp \
    autotest/tableview.cpp \
    connection.cpp \
    devices/autotest.cpp \
    devices/devices.cpp \
    devices/kds.cpp \
    devices/tester.cpp \
    kdsdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    pinmodel.cpp \
    testerdata.cpp \


HEADERS += \
    amktest/pointedit.h \
    autotest/header.h \
    autotest/model.h \
    autotest/tableview.h \
    connection.h \
    devices/autotest.h \
    devices/devices.h \
    devices/kds.h \
    devices/tester.h \
    kdsdialog.h \
    mainwindow.h \
    pinmodel.h \
    testerdata.h \


FORMS += \
        kdsdialog.ui \
        mainwindow.ui

include(MyProtokol/myprotokol.pri)
include(ElemerDevice/ElemerDevice.pri)

RESOURCES += \
    res/res.qrc
