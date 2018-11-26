#include "mainwindow.h"
#include "hwinterface/interface.h"

#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>

//QTabWidget* tw;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    widget->setFl(false);
    widget_3->setFl(true);

    for (const QSerialPortInfo& pi : QSerialPortInfo::availablePorts()) {
        cbxAmk->addItem(pi.portName());
        cbxHart->addItem(pi.portName());
        cbxTester->addItem(pi.portName());
    }

    connect(pbPing, &QPushButton::clicked, [this] {
        if (!Interface::kds()->Ping(cbxAmk->currentText()))
            QMessageBox::critical(this, "", "Amk");

        if (!Interface::hart()->Ping(cbxHart->currentText()))
            QMessageBox::critical(this, "", "Hart");

        if (!Interface::tester()->Ping(cbxTester->currentText()))
            QMessageBox::critical(this, "", "Tester");
    });

    readSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("cbxAmk", cbxAmk->currentIndex());
    settings.setValue("cbxHart", cbxHart->currentIndex());
    settings.setValue("cbxTester", cbxTester->currentIndex());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    cbxAmk->setCurrentIndex(settings.value("cbxAmk").toInt());
    cbxHart->setCurrentIndex(settings.value("cbxHart").toInt());
    cbxTester->setCurrentIndex(settings.value("cbxTester").toInt());
    settings.endGroup();
}
