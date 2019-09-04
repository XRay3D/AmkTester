#include "hwinterface/interface.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("XrSoft");
    a.setApplicationName("AmkTest");

    Interface i;
    Q_UNUSED(i)

    MainWindow w;
    w.show();

    return a.exec();
}
