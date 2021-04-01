#include "mainwindow.h"
#include "autotest/autotestmodel.h"
#include "devices/devices.h"
#include "kdsdialog.h"
#include "ui_mainwindow.h"

#include <QContextMenuEvent>
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
        toolBarConnection->addAction(QIcon::fromTheme(""), "Ping", this, &MainWindow::ping);
        toolBarConnection->addAction(QIcon::fromTheme("update-none"), "Update Ports", this, &MainWindow::updatePorts);
        connect(ui->dwCommunication, &QDockWidget::visibilityChanged, toolBarConnection, &QToolBar::setVisible);
    }

    testModel = ui->tvAuto->initCheckBox();

    connect(Devices::tester(), &Tester::measureReadyAll, ui->tvPins, &ResistanceView::setPins);

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
    while(!file.open(QIODevice::ReadOnly)) {
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
        toolBarAutomatic->addAction(QIcon::fromTheme(""), "Add Test", [this] {
            std::pair<Point, Point> pair;
            for(auto button : qAsConst(pb)) {
                if(button->isChecked()) {
                    if(int index = pb.indexOf(button); index < SetCount)
                        pair.first = m_points[0][index];
                    else
                        pair.second = m_points[1][index - SetCount];
                }
            }
            testModel->appendTest(ui->tvPins->pins(), pair.first, pair.second);
            ui->tvAuto->selectRow(testModel->rowCount() - 1);
        });
        toolBarAutomatic->addAction(QIcon::fromTheme(""), "Set Pattern", [this] {
            for(auto& index : ui->tvAuto->selectionModel()->selectedIndexes())
                testModel->setPattern(index, ui->tvPins->pins());
        });
        actionTest = toolBarAutomatic->addAction(QIcon::fromTheme(""), "Remove row", [this] {
            testModel->removeRow(ui->tvAuto->currentIndex().row());
        });

        {
            actionTest = toolBarAutomatic->addAction(QIcon::fromTheme(""), "Test", this, &MainWindow::onActionTestTriggered);
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
    actionTest->setChecked(checked);
}

void MainWindow::updatePorts() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    {
        ui->cbxPortTester->setCurrentText(settings.value("cbxPortTester").toString());
        ui->cbxPortAmk1->setCurrentText(settings.value("cbxPortAmk1").toString());
        ui->cbxPortAmk2->setCurrentText(settings.value("cbxPortAmk2").toString());
        ui->cbxPortBaud1->setCurrentIndex(settings.value("cbxPortBaud1").toInt());
        ui->cbxPortBaud2->setCurrentIndex(settings.value("cbxPortBaud2").toInt());
    }

    auto ports{QSerialPortInfo::availablePorts().toVector()};
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
            auto baudStr{QString::number(baud)};
            ui->cbxPortBaud1->addItem(baudStr, baud);
            ui->cbxPortBaud2->addItem(baudStr, baud);
        }

    {
        ui->cbxPortTester->setCurrentText(settings.value("cbxPortTester").toString());
        ui->cbxPortAmk1->setCurrentText(settings.value("cbxPortAmk1").toString());
        ui->cbxPortAmk2->setCurrentText(settings.value("cbxPortAmk2").toString());
        ui->cbxPortBaud1->setCurrentIndex(settings.value("cbxPortBaud1").toInt());
        ui->cbxPortBaud2->setCurrentIndex(settings.value("cbxPortBaud2").toInt());
    }

    settings.endGroup();
}

void MainWindow::ping() {

    bool en[]{Devices::tester()->ping(ui->cbxPortTester->currentText()),
              Devices::kds1()->ping(ui->cbxPortAmk1->currentText(), ui->cbxPortBaud1->currentData().toInt()),
              Devices::kds2()->ping(ui->cbxPortAmk2->currentText(), ui->cbxPortBaud2->currentData().toInt())};
    ui->gbxTest->setEnabled(en[0]);
    ui->gbxKds1->setEnabled(en[1]);
    ui->gbxKds2->setEnabled(en[2]);

    ui->lblPortTester->setText(en[0] ? QString{"Подключено"} : Devices::tester()->port()->errorString());
    ui->lblPortAmk1->setText(en[1] ? QString::number(Devices::kds1()->getData(Kds::SerNum)) : Devices::kds1()->port()->errorString());
    ui->lblPortAmk2->setText(en[2] ? QString::number(Devices::kds2()->getData(Kds::SerNum)) : Devices::kds2()->port()->errorString());
}

QMenu* MainWindow::createPopupMenu() {
    auto menu = QMainWindow::createPopupMenu();
    menu->removeAction(toolBarAutomatic->toggleViewAction());
    menu->removeAction(toolBarConnection->toggleViewAction());
    return menu;
}
