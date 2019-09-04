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

    static QList<QSerialPortInfo> pl(QSerialPortInfo::availablePorts());
    std::sort(pl.begin(), pl.end(), [](const QSerialPortInfo& i1, const QSerialPortInfo& i2) { return i1.portName().mid(3).toInt() < i2.portName().mid(3).toInt(); });
    for (const QSerialPortInfo& pi : pl) {
        cbxAmk->addItem(pi.portName());
        cbxHart->addItem(pi.portName());
        cbxTester->addItem(pi.portName());
    }

    connect(wAmk1, &AmkTest::message, statusBar_, &QStatusBar::showMessage);
    connect(wAmk2, &AmkTest::message, statusBar_, &QStatusBar::showMessage);

    tableView->initCheckBox();
    connect(pushButton, &QPushButton::clicked, [this] {
        tableView->addRow(wAmk1->m_points[wAmk1->m_numPoint].Description);
    });
    connect(pushButton_2, &QPushButton::clicked, [this] {
        tableView->model()->setColumnCount(tableView->model()->columnCount() + 1);
    });
    connect(pushButton_3, &QPushButton::clicked, [this] {
        tableView->setPattern(
            widget_2->data(),
            wAmk1->m_points[wAmk1->m_numPoint],
            wAmk2->m_points[wAmk2->m_numPoint]);
    });

    //    connect(&timer, &QTimer::timeout, [&]() {
    //        if (s.tryAcquire()) {
    //            static int i = 0;
    //            emit MeasurePin(i++ % 11);
    //        }
    //    });

    //    connect(pushButton_4, &QPushButton::clicked, [&](bool checked) {
    //        if (Interface::tester()->Ping())
    //            if (checked) {
    //                s.release();
    //                timer.start(1);
    //            } else {
    //                timer.stop();
    //            }
    //        else {
    //            pushButton_4->setChecked(false);
    //        }
    //    });

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
