#include "mainwindow.h"
#include "hwinterface/interface.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("XrSoft");
    a.setApplicationName("AmkTester");

    Interface i;
    Q_UNUSED(i)

    MainWindow w;
    w.show();

    return a.exec();
}
