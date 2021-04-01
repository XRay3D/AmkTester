#include "devices/devices.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    QIcon::setThemeName("Breeze");

    a.setOrganizationName("XrSoft");
    a.setApplicationName("AmkTest");

    Devices i;
    Q_UNUSED(i)

    MainWindow w;
    w.show();

    return a.exec();
}
