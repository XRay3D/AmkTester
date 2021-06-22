#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "autotest/resultdialog.h"
#include "devices/devices.h"
#include "kdsdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui {new Ui::MainWindow} //
{
    ui->setupUi(this);

    { // menu File
        auto menu = menuBar()->addMenu("&Файл");
        menu->addAction(
            QIcon::fromTheme("document-new"), "Новый",
            [this] { testFileNane.clear(); testModel->clear(); },
            QKeySequence::New);
        menu->addAction(
            QIcon::fromTheme("document-open"), "Открыть...",
            [this] { testFileNane = QFileDialog::getOpenFileName(this, "Open", testFileNane, "Tests (*.tst)"); testModel->load(testFileNane); },
            QKeySequence::Open);

        menu->addAction(
            QIcon::fromTheme("document-save"), "Сохранить",
            [this] { if(testFileNane.isEmpty()) testFileNane = QFileDialog::getSaveFileName(this, "Save", "", "Tests (*.tst)"); testModel->save(testFileNane); },
            QKeySequence::Save);

        menu->addAction(
            QIcon::fromTheme("document-save-as"), "Сохранить как...",
            [this] { testFileNane = QFileDialog::getSaveFileName(this, "Save As", testFileNane, "Tests (*.tst)"); testModel->save(testFileNane); },
            QKeySequence::SaveAs);

        menu->addSeparator();
        menu->addAction(QIcon::fromTheme("application-exit"), "Выход", this, &MainWindow::close, QKeySequence("Ctrl+Q"));
    }
    { // menu Справка
        auto menu = menuBar()->addMenu("Справка");
        menu->addAction(QIcon::fromTheme("QtProject-designer"), "О Qt", [] { qApp->aboutQt(); });
    }
    { // toolBar Connection
        toolBarConnection = addToolBar("Connection");
        toolBarConnection->setObjectName("toolBarConnection");
        toolBarConnection->addAction(QIcon::fromTheme("network-connect"), "Проверка связи", this, &MainWindow::ping);
        toolBarConnection->addAction(QIcon::fromTheme("view-refresh"), "Обновить списки портов", this, &MainWindow::updatePorts);
        connect(ui->dwCommunication, &QDockWidget::visibilityChanged, toolBarConnection, &QToolBar::setVisible);
    }

    testModel = ui->tvAuto->initCheckBox();

    connect(Devices::tester(), &Tester::resistanceReady, ui->tvPins, &ResistanceView::setPins);

    setupTvAuto();

    updatePorts();

    { // pbAmkSettings 1 2
        connect(ui->pbAmkSettings1, &QPushButton::clicked, [] { KdsDialog(Devices::kds1()).exec(); });
        connect(ui->pbAmkSettings2, &QPushButton::clicked, [] { KdsDialog(Devices::kds2()).exec(); });
        ui->pbAmkSettings1->setIcon(QIcon::fromTheme("configure-shortcuts"));
        ui->pbAmkSettings2->setIcon(QIcon::fromTheme("configure-shortcuts"));
    }

    { // pbMeasure
        connect(ui->pbMeasure, &QPushButton::toggled, Devices::tester(), &Tester::startStop);
        connect(ui->pbMeasure, &QPushButton::toggled, [this](bool checked) { ui->pbMeasure->setIcon(QIcon::fromTheme(checked ? "media-playback-stop" : "media-playback-start")); });
        ui->pbMeasure->setIcon(QIcon::fromTheme("media-playback-start"));
        connect(ui->dwMan, &QDockWidget::visibilityChanged, ui->pbMeasure, &QPushButton::setChecked);
    }

    sets.loadSets();

    auto setupSets = [this](QComboBox* cbxAmkSet, RelaySet* relaySet, int fl) {
        relaySet->setupUi(fl);
        relaySet->setSets(&sets);
        cbxAmkSet->addItems(sets.sets());
        connect(cbxAmkSet, qOverload<int>(&QComboBox::currentIndexChanged), relaySet, &RelaySet::cbxTypeIndexChanged);
        connect(relaySet, &RelaySet::showMessage, statusBar(), &QStatusBar::showMessage);
    };
    setupSets(ui->cbxAmkSet1, ui->relaySet1, 0);
    setupSets(ui->cbxAmkSet2, ui->relaySet2, 1);

    readSettings();

    QTimer::singleShot(200, this, &MainWindow::ping);
}

MainWindow::~MainWindow() {
    writeSettings();
    delete ui;
}

void MainWindow::readSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    ui->cbxPortTester->setCurrentText(settings.value("cbxTester").toString());

    ui->cbxPortAmk1->setCurrentText(settings.value("cbxPortAmk1").toString());
    ui->cbxPortAmk2->setCurrentText(settings.value("cbxPortAmk2").toString());

    ui->cbxAmkSet1->setCurrentIndex(settings.value("cbxAmkSet1").toInt());
    ui->cbxAmkSet2->setCurrentIndex(settings.value("cbxAmkSet2").toInt());

    ui->cbxPortBaud1->setCurrentIndex(settings.value("cbxPortBaud1").toInt());
    ui->cbxPortBaud2->setCurrentIndex(settings.value("cbxPortBaud2").toInt());

    testFileNane = settings.value("testFileNane").toString();
    testModel->load(testFileNane);
    settings.endGroup();
}

void MainWindow::writeSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("cbxTester", ui->cbxPortTester->currentText());

    settings.setValue("cbxPortAmk1", ui->cbxPortAmk1->currentText());
    settings.setValue("cbxPortAmk2", ui->cbxPortAmk2->currentText());

    settings.setValue("cbxAmkSet1", ui->cbxAmkSet1->currentIndex());
    settings.setValue("cbxAmkSet2", ui->cbxAmkSet2->currentIndex());

    settings.setValue("cbxPortBaud1", ui->cbxPortBaud1->currentIndex());
    settings.setValue("cbxPortBaud2", ui->cbxPortBaud2->currentIndex());

    settings.setValue("testFileNane", testFileNane);
    settings.endGroup();
}

void MainWindow::message(const QString& msg) {
    QMessageBox::information(this, "", msg);
    Devices::autoTest()->mutex.unlock();
}

void MainWindow::setupTvAuto() {
    ui->tvAuto->setModel(testModel);
    ui->tvAuto->setIconSize({33, 33});
    ui->tvAuto->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tvAuto->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tvAuto->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tvAuto->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tvAuto->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    { // toolBarAutomatic
        toolBarAutomatic = addToolBar("Automatic");
        toolBarAutomatic->setObjectName("toolBarAutomatic");
        toolBarAutomatic->addAction(QIcon::fromTheme("list-add"), "Добавить тест", [this] {
            testModel->appendTest(ui->tvPins->pins(), ui->relaySet1->currentPoint(), ui->relaySet2->currentPoint());
            ui->tvAuto->selectRow(testModel->rowCount() - 1);
        });
        actionTest = toolBarAutomatic->addAction(QIcon::fromTheme("list-remove"), "Удалить тест", [this] {
            testModel->removeRow(ui->tvAuto->currentIndex().row());
        });
        toolBarAutomatic->addSeparator();
        toolBarAutomatic->addAction(QIcon::fromTheme("games-config-board"), "Установить шаблон", [this] {
            for(auto& index : ui->tvAuto->selectionModel()->selectedIndexes())
                testModel->setPattern(index, ui->tvPins->pins());
        });
        toolBarAutomatic->addSeparator();
        {
            actionTest = toolBarAutomatic->addAction(QIcon::fromTheme("media-playback-start"), "Запустить тест", this, &MainWindow::onActionTestTriggered);
            actionTest->setCheckable(true);
            actionTest->setObjectName("actionTest");
        }
    }
    connect(ui->dwAuto, &QDockWidget::visibilityChanged, toolBarAutomatic, &QToolBar::setVisible);

    connect(ui->tvAuto, &QTableView::doubleClicked, [this](const QModelIndex& index) {
        if(index.column() < AutoTestModel::UserActivity)
            PointEdit(testModel->point(index), nullptr).exec();
        else if(index.column() > AutoTestModel::UserActivity) {
            auto& md = testModel->data()[index.row()];
            ResultDialog(md.pattern, md.measured).exec();
        }
    });

    connect(Devices::autoTest(), &AutoTest::message, this, &MainWindow::message);
    connect(Devices::autoTest(), &QThread::finished, this, &MainWindow::finished);
}

void MainWindow::onActionTestTriggered(bool checked) {
    if(checked) {
        Devices::autoTest()->setModel(testModel);
        Devices::autoTest()->start();
    } else if(Devices::autoTest()->isRunning()) {
        Devices::autoTest()->requestInterruption();
        Devices::autoTest()->wait();
    }
    actionTest->setIcon(QIcon::fromTheme(checked ? "media-playback-stop" : "media-playback-start"));
    actionTest->setChecked(checked);
}

void MainWindow::updatePorts() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    {
        ui->cbxPortTester->setCurrentText(settings.value("cbxPortTester").toString());
        ui->cbxPortAmk1->setCurrentText(settings.value("cbxPortAmk1").toString());
        ui->cbxPortAmk2->setCurrentText(settings.value("cbxPortAmk2").toString());
    }

    auto ports {QSerialPortInfo::availablePorts().toVector()};
    std::ranges::sort(ports, {}, [](const QSerialPortInfo& i1) { return i1.portName().midRef(3).toInt(); });
    ui->cbxPortAmk1->clear();
    ui->cbxPortAmk2->clear();
    ui->cbxPortTester->clear();

    for(auto& pi : qAsConst(ports)) {
        ui->cbxPortAmk1->addItem(pi.portName());
        ui->cbxPortAmk2->addItem(pi.portName());
        if(pi.manufacturer().contains("FTDI"))
            ui->cbxPortTester->addItem(pi.portName());
    }
    if(!ui->cbxPortBaud1->count())
        for(int baud : Elemer::stdBauds) {
            auto baudStr {QString::number(baud)};
            ui->cbxPortBaud1->addItem(baudStr, baud);
            ui->cbxPortBaud2->addItem(baudStr, baud);
        }

    {
        ui->cbxPortTester->setCurrentText(settings.value("cbxPortTester").toString());
        ui->cbxPortAmk1->setCurrentText(settings.value("cbxPortAmk1").toString());
        ui->cbxPortAmk2->setCurrentText(settings.value("cbxPortAmk2").toString());
    }

    settings.endGroup();
}

void MainWindow::ping() {
    // NOTE uncomet pings
    bool en[] {
        Devices::tester()->ping(ui->cbxPortTester->currentText()),
        Devices::kds1()->ping(ui->cbxPortAmk1->currentText(), ui->cbxPortBaud1->currentData().toInt()),
        Devices::kds2()->ping(ui->cbxPortAmk2->currentText(), ui->cbxPortBaud2->currentData().toInt()),
    };
    ui->gbxTest->setEnabled(en[0]);
    ui->gbxKds1->setEnabled(en[1]);
    ui->gbxKds2->setEnabled(en[2]);

    ui->lblPortTester->setText(en[0] ? QString {"Подключено"} : Devices::tester()->port()->errorString());
    ui->lblPortAmk1->setText(en[1] ? QString("Зав. № %1").arg(Devices::kds1()->getData(Kds::SerNum)) : Devices::kds1()->port()->errorString());
    ui->lblPortAmk2->setText(en[2] ? QString("Зав. № %1").arg(Devices::kds2()->getData(Kds::SerNum)) : Devices::kds2()->port()->errorString());
}

QMenu* MainWindow::createPopupMenu() {
    auto menu = QMainWindow::createPopupMenu();
    menu->removeAction(toolBarAutomatic->toggleViewAction());
    menu->removeAction(toolBarConnection->toggleViewAction());
    return menu;
}
