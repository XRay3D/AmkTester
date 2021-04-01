#include "mainwindow.h"
#include "autotest/autotestmodel.h"
#include "devices/devices.h"
#include "kdsdialog.h"
#include "pinmodel.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QSettings>
#include <QShortcut>
#include <QTimer>
#include <QToolBar>
#include <ranges>

#include <autotest/resultdialog.h>
//QTabWidget* tw;

struct JsonData {
    JsonData() { }
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui{new Ui::MainWindow}
    , leDescription(24)
    , pb(24) //
{
    ui->setupUi(this);

    { // menu File
        auto menu = menuBar()->addMenu("File");
        menu->addAction(
            QIcon(), "New",
            [this] { testFileNane.clear(); testModel->clear(); }, QKeySequence::New);
        menu->addSeparator();

        menu->addAction(
            QIcon(), "Save",
            [this] { if(testFileNane.isEmpty()) testFileNane = QFileDialog::getSaveFileName(this, "Save", "", "Tests (*.tst)"); testModel->save(testFileNane); },
            QKeySequence::Save);
        menu->addAction(
            QIcon(), "Save As",
            [this] { testFileNane = QFileDialog::getSaveFileName(this, "Save As", testFileNane, "Tests (*.tst)"); testModel->save(testFileNane); },
            QKeySequence::SaveAs);
        menu->addAction(
            QIcon(), "Open",
            [this] { testFileNane = QFileDialog::getOpenFileName(this, "Open", testFileNane, "Tests (*.tst)"); testModel->load(testFileNane); },
            QKeySequence::Open);
        menu->addSeparator();
        menu->addAction(QIcon(), "Quit", this, &MainWindow::close, QKeySequence("Ctrl+Q"));
    }
    { // menu Справка
        auto menu = menuBar()->addMenu("Справка");
        menu->addAction(QIcon(), "О Qt", [] { qApp->aboutQt(); });
    }
    { // toolBar Connection
        auto toolBar = addToolBar("Connection");
        toolBar->addAction(QIcon(), "Ping", this, &MainWindow::ping);
        toolBar->addAction(QIcon(), "Update Ports", this, &MainWindow::updatePorts);
    }

    pinModel = new PinModel(ui->tvPins);
    testModel = ui->tvAuto->initCheckBox();

    setupTvPins();
    setupTvAuto();

    auto pbSetter = [this](QPushButton* pb, int i, const QString objName, bool fl) {
        auto shortcut = new QShortcut(this); // Инициализируем объект
        shortcut->setKey(fl ? QString("Shift+F%1").arg(i + 1) : QString("F%1").arg(i + 1)); // Устанавливаем код клавиши
        connect(shortcut, &QShortcut::activated, pb, &QPushButton::click); // цепляем обработчик нажатия клавиши

        pb->setCheckable(true);
        pb->setMinimumSize(0, 0);
        pb->setMinimumSize(70, 0);
        pb->setObjectName(QString(objName).arg(i));
        pb->setText(fl ? QString("Shift+F%1").arg(i + 1) : QString("F%1").arg(i + 1));
        connect(pb, &QPushButton::clicked, this, &MainWindow::switchSlot);
    };

    for(int i = 0; i < SetCount; ++i) {
        ui->layAmk1->addRow(pb[i /* */] = new QPushButton(QMainWindow::centralWidget()), leDescription[i /* */] = new QLineEdit(this));
        ui->layAmk2->addRow(pb[i + SetCount] = new QPushButton(QMainWindow::centralWidget()), leDescription[i + SetCount] = new QLineEdit(this));

        leDescription[i /* */]->installEventFilter(this);
        leDescription[i + SetCount]->installEventFilter(this);

        pbSetter(pb[i /* */], i, "pushButton_%1_0", 0);
        pbSetter(pb[i + SetCount], i, "pushButton_%1_1", 1);
    }

    updatePorts();

    connect(ui->pbAmkSettings1, &QPushButton::clicked, [] { KdsDialog(Devices::kds1()).exec(); });
    connect(ui->pbAmkSettings2, &QPushButton::clicked, [] { KdsDialog(Devices::kds2()).exec(); });

    connect(ui->pbMeasure, &QPushButton::toggled, Devices::tester(), &Tester::startStop);

    loadSets();
    readSettings();

    QTimer::singleShot(10, this, &MainWindow::ping);
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
    ui->cbxPortAmk1->setCurrentText(settings.value("cbxAmk").toString());
    ui->cbxPortAmk2->setCurrentText(settings.value("cbxHart").toString());
    ui->cbxPortTester->setCurrentText(settings.value("cbxTester").toString());
    ui->cbxAmkSet1->setCurrentIndex(settings.value("cbType1").toInt());
    ui->cbxAmkSet2->setCurrentIndex(settings.value("cbType2").toInt());

    testFileNane = settings.value("testFileNane").toString();
    testModel->load(testFileNane);
    settings.endGroup();
}

void MainWindow::writeSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("cbxAmk", ui->cbxPortAmk1->currentText());
    settings.setValue("cbxHart", ui->cbxPortAmk2->currentText());
    settings.setValue("cbxTester", ui->cbxPortTester->currentText());
    settings.setValue("cbType1", ui->cbxAmkSet1->currentIndex());
    settings.setValue("cbType2", ui->cbxAmkSet2->currentIndex());

    settings.setValue("testFileNane", testFileNane);
    settings.endGroup();
}

void MainWindow::message(const QString& msg) {
    QMessageBox::information(this, "", msg);
    Devices::autoTest()->mutex.unlock();
}

void MainWindow::cbxTypeIndexChanged(int index) {
    if(index < 0)
        return;
    saveSets();
    lastIndex = index;
    int idx{};
    auto array{jsonArray[lastIndex].toObject()["data"].toArray()};
    bool amkNum = sender() == ui->cbxAmkSet1;
    for(auto value : array) {
        m_points[amkNum][idx] = Point(value.toObject());
        leDescription[idx + (amkNum ? 0 : SetCount)]->setText(m_points[amkNum][idx].Description);
        ++idx;
    }
}

void MainWindow::loadSets() {
    QFile file(QStringLiteral("data.json"));
A:
    if(!file.open(QIODevice::ReadOnly)) {
        QFile saveFile(QStringLiteral("data.json"));
        if(!saveFile.open(QIODevice::WriteOnly)) {
            qWarning() << __FUNCTION__ << saveFile.errorString();
            return;
        }
        saveFile.write("[{\"data\":["
                       "{\"name\":\"0Ом\",\"value\":128},"
                       "{\"name\":\"40Ом\",\"value\":256},"
                       "{\"name\":\"80Ом\",\"value\":16},"
                       "{\"name\":\"160Ом\",\"value\":32},"
                       "{\"name\":\"320Ом\",\"value\":64},"
                       "{\"name\":\"+10В\",\"value\":512},"
                       "{\"name\":\"-10В\",\"value\":1536},"
                       "{\"name\":\"+IРМТ\",\"value\":2},"
                       "{\"name\":\"-IРМТ\",\"value\":1026},"
                       "{\"name\":\"+UРМТ\",\"value\":1},"
                       "{\"name\":\"-UРМТ\",\"value\":1025},"
                       "{\"name\":\"0\",\"value\":0}],\"name\":\"КДС\"},"
                       "{\"data\":["
                       "{\"name\":\"20мА-0Ом\",\"value\":640},"
                       "{\"name\":\"20мА-400Ом\",\"value\":768},"
                       "{\"name\":\"20мА-500Ом\",\"value\":528},"
                       "{\"name\":\"20мА-1кОм\",\"value\":544},"
                       "{\"name\":\"20мА-2кОм\",\"value\":576},"
                       "{\"name\":\"5мА-0Ом\",\"value\":132},"
                       "{\"name\":\"5мА-400Ом\",\"value\":260},"
                       "{\"name\":\"5мА-500Ом\",\"value\":20},"
                       "{\"name\":\"5мА-1кОм\",\"value\":36},"
                       "{\"name\":\"5мА-2кОм\",\"value\":68},"
                       "{\"name\":\"0\",\"value\":0},"
                       "{\"name\":\"0\",\"value\":0}],\"name\":\"MN\"},"
                       "{\"data\":["
                       "{\"name\":\"1\",\"value\":1},"
                       "{\"name\":\"2\",\"value\":2},"
                       "{\"name\":\"3\",\"value\":4},"
                       "{\"name\":\"4\",\"value\":8},"
                       "{\"name\":\"5\",\"value\":16},"
                       "{\"name\":\"6\",\"value\":32},"
                       "{\"name\":\"7\",\"value\":64},"
                       "{\"name\":\"8\",\"value\":128},"
                       "{\"name\":\"9\",\"value\":256},"
                       "{\"name\":\"10\",\"value\":512},"
                       "{\"name\":\"11\",\"value\":1024},"
                       "{\"name\":\"12\",\"value\":2048}],\"name\":\"HC\"},"
                       "{\"data\":["
                       "{\"name\":\"+IТМ\",\"value\":8},"
                       "{\"name\":\"-IТМ\",\"value\":1032},"
                       "{\"name\":\"+UТМ\",\"value\":4},"
                       "{\"name\":\"-UТМ\",\"value\":1028},"
                       "{\"name\":\"+10В\",\"value\":512},"
                       "{\"name\":\"-10В\",\"value\":1536},"
                       "{\"name\":\"+IРМТ\",\"value\":2},"
                       "{\"name\":\"-IРМТ\",\"value\":1026},"
                       "{\"name\":\"+UРМТ\",\"value\":1},"
                       "{\"name\":\"-UРМТ\",\"value\":1025},"
                       "{\"name\":\"0\",\"value\":0},"
                       "{\"name\":\"0\",\"value\":0}],\"name\":\"КДС старый\"},"
                       "{\"data\":["
                       "{\"name\":\"1\",\"value\":1},"
                       "{\"name\":\"2\",\"value\":2},"
                       "{\"name\":\"4\",\"value\":4},"
                       "{\"name\":\"8\",\"value\":8},"
                       "{\"name\":\"16\",\"value\":16},"
                       "{\"name\":\"32\",\"value\":32},"
                       "{\"name\":\"64\",\"value\":64},"
                       "{\"name\":\"128\",\"value\":128},"
                       "{\"name\":\"256\",\"value\":256},"
                       "{\"name\":\"512\",\"value\":512},"
                       "{\"name\":\"1024\",\"value\":1024},"
                       "{\"name\":\"0\",\"value\":0}],\"name\":\"TEST\"}]");
        qWarning() << __FUNCTION__ << file.errorString();
        goto A;
    }

    QByteArray array(file.readAll());
    QJsonParseError err;
    QJsonDocument loadDoc(QJsonDocument::fromJson(array, &err));
    jsonArray = loadDoc.array();
    qDebug() << err.errorString();

    for(QJsonValue value : jsonArray) {
        QJsonObject object(value.toObject());
        ui->cbxAmkSet1->addItem(object["name"].toString());
        ui->cbxAmkSet2->addItem(object["name"].toString());
    }

    int idxCtr = 0;

    for(QJsonValue value : jsonArray[lastIndex].toObject()["data"].toArray()) {
        QJsonObject object2(value.toObject());
        m_points[0][idxCtr] = Point(value.toObject());
        m_points[1][idxCtr] = Point(value.toObject());
        leDescription[idxCtr /* */]->setText(m_points[0][idxCtr].Description);
        leDescription[idxCtr + SetCount]->setText(m_points[1][idxCtr].Description);
        ++idxCtr;
    }

    connect(ui->cbxAmkSet1, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::cbxTypeIndexChanged);
    connect(ui->cbxAmkSet2, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::cbxTypeIndexChanged);
}

void MainWindow::saveSets() {
    QFile saveFile(QStringLiteral("data_.json"));

    if(!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << __FUNCTION__ << saveFile.errorString();
        return;
    }

    {
        QJsonObject levelObject;
        levelObject["name"] = ui->cbxAmkSet1->itemText(lastIndex);
        QJsonArray array;
        for(Point& p : m_points[0]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }
        levelObject["data"] = array;

        if(ui->cbxAmkSet1->count() > jsonArray.count())
            jsonArray.append(levelObject);
        else
            jsonArray[lastIndex] = levelObject;
    }
    {
        QJsonObject levelObject;
        levelObject["name"] = ui->cbxAmkSet2->itemText(lastIndex);
        QJsonArray array;
        for(Point& p : m_points[1]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }
        levelObject["data"] = array;
        if(ui->cbxAmkSet2->count() > jsonArray.count())
            jsonArray.append(levelObject);
        else
            jsonArray[lastIndex] = levelObject;
    }

    QJsonDocument saveDoc(jsonArray);
    saveFile.write(saveDoc.toJson());
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if(event->type() == QEvent::MouseButtonDblClick) {
        int m_numPoint = leDescription.indexOf(reinterpret_cast<QLineEdit*>(obj));
        if(m_numPoint > -1) {
            PointEdit(m_points[m_numPoint > 11 ? 1 : 0][m_numPoint % SetCount], leDescription[m_numPoint], this).exec();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MainWindow::switchSlot() {
    QPushButton* pushButton = qobject_cast<QPushButton*>(sender());
    int m_numPoint = pb.indexOf(pushButton);
    qDebug() << "m_numPoint" << m_numPoint;
    if(m_numPoint < SetCount) {
        for(int i{}; i < SetCount; ++i)
            if(m_numPoint != i)
                pb[i]->setChecked(false);
            else
                pushButton->setChecked(true);
        if(Devices::kds1()->setRelay(m_points[0][m_numPoint % SetCount].Parcel)) {
            statusBar()->showMessage("Прибором КДС 1 успешно переключен!");
            return;
        }
    } else {
        for(int i{SetCount}; i < 24; ++i)
            if(m_numPoint != i)
                pb[i]->setChecked(false);
            else
                pushButton->setChecked(true);
        if(Devices::kds2()->setRelay(m_points[1][m_numPoint % SetCount].Parcel)) {
            statusBar()->showMessage("Прибором КДС 2 успешно переключен!");
            return;
        }
    }
    statusBar()->showMessage("Нет связи с прибором КДС!");
}

void MainWindow::setupTvPins() {
    ui->tvPins->setModel(pinModel);
    ui->tvPins->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tvPins->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(Devices::tester(), &Tester::measureReadyAll, pinModel, &PinModel::setPins);
}

void MainWindow::setupTvAuto() {
    ui->tvAuto->setModel(testModel);
    ui->tvAuto->setIconSize({33, 33});
    ui->tvAuto->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tvAuto->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tvAuto->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tvAuto->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tvAuto->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    auto toolBar = addToolBar("Automatic");
    toolBar->addAction(QIcon(), "Add Test", [this] {
        std::pair<Point, Point> pair;
        for(auto b : pb) {
            if(b->isChecked()) {
                if(int index = pb.indexOf(b); index < SetCount)
                    pair.first = m_points[0][index];
                else
                    pair.second = m_points[1][index - SetCount];
            }
        }
        testModel->appendTest(pinModel->pins(), pair.first, pair.second);
        ui->tvAuto->selectRow(testModel->rowCount() - 1);
    });
    toolBar->addAction(QIcon(), "Set Pattern", [this] {
        for(auto& index : ui->tvAuto->selectionModel()->selectedIndexes())
            testModel->setPattern(index, pinModel->pins());
    });
    actionTest = toolBar->addAction(QIcon(), "Test", this, &MainWindow::onActionTestTriggered);
    actionTest->setCheckable(true);
    actionTest->setObjectName("actionTest");

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
    actionTest->setChecked(checked);
}

void MainWindow::updatePorts() {
    auto ports{QSerialPortInfo::availablePorts().toVector()};
    std::ranges::sort(ports, {}, [](const QSerialPortInfo& i1) { return i1.portName().mid(3).toInt(); });
    for(const QSerialPortInfo& pi : ports) {
        ui->cbxPortAmk1->addItem(pi.portName());
        ui->cbxPortAmk2->addItem(pi.portName());
        if(pi.manufacturer().contains("FTDI"))
            ui->cbxPortTester->addItem(pi.portName());
    }
}

void MainWindow::ping() {
    for(bool en = Devices::kds1()->ping(ui->cbxPortAmk1->currentText());
        auto w : ui->gbxKds1->findChildren<QWidget*>())
        w->setEnabled(en);

    for(bool en = Devices::kds2()->ping(ui->cbxPortAmk2->currentText());
        auto w : ui->gbxKds2->findChildren<QWidget*>())
        w->setEnabled(en);

    for(bool en = Devices::tester()->ping(ui->cbxPortTester->currentText());
        auto w : ui->gbxTest->findChildren<QWidget*>())
        w->setEnabled(en);

    ui->cbxPortAmk1->setEnabled(true);
    ui->cbxPortAmk2->setEnabled(true);
    ui->cbxPortTester->setEnabled(true);
}
