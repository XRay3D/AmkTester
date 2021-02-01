QT += core gui serialport widgets

TARGET = AmkTester
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:RC_FILE = main_icon/myapp.rc

SOURCES += \
#    amktest/amktest.cpp \
    amktest/pointedit.cpp \
    autotest/header.cpp \
#    autotest/main.cpp \
    autotest/model.cpp \
    autotest/tableview.cpp \
    connection.cpp \
    hwinterface/autotest.cpp \
    hwinterface/interface.cpp \
    hwinterface/kds.cpp \
    hwinterface/tester.cpp \
    main.cpp \
    mainwindow.cpp \
    testdata/testdata.cpp \
    mytablemodel.cpp \
    testerdata.cpp




HEADERS += \
#    amktest/amktest.h \
    amktest/pointedit.h \
    autotest/header.h \
    autotest/model.h \
    autotest/tableview.h \
    connection.h \
    hwinterface/autotest.h \
    hwinterface/common_interfaces.h \
    hwinterface/interface.h \
    hwinterface/kds.h \
    hwinterface/tester.h \
    mainwindow.h \
    testdata/testdata.h \
    mytablemodel.h \
    testerdata.h


FORMS += \
        mainwindow.ui

include(../MyProtokol/myprotokol.pri)
include(../elemer_ascii/elemer_ascii.pri)
