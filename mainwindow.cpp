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

    wAmk1->setFl(false);
    wAmk2->setFl(true);

    for (const QSerialPortInfo& pi : QSerialPortInfo::availablePorts()) {
        cbxAmk->addItem(pi.portName());
        cbxHart->addItem(pi.portName());
        cbxTester->addItem(pi.portName());
    }

    connect(wAmk1, &AMK::message, statusBar_, &QStatusBar::showMessage);
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

    settings.setValue("cbType1", wAmk1->cbType->currentIndex());
    settings.setValue("cbType2", wAmk2->cbType->currentIndex());
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

    wAmk1->cbType->setCurrentIndex(settings.value("cbType1").toInt());
    wAmk2->cbType->setCurrentIndex(settings.value("cbType2").toInt());
    settings.endGroup();
}

void MainWindow::on_pbPing_clicked()
{
    QString str;
    if (!Interface::kds1()->Ping(cbxAmk->currentText()))
        str += "Amk!\n";

    if (!Interface::kds2()->Ping(cbxHart->currentText()))
        str += "Hart!\n";

    if (!Interface::tester()->Ping(cbxTester->currentText()))
        str += "Tester!\n";

    if (!str.isEmpty())
        QMessageBox::critical(this, "", str);
}
