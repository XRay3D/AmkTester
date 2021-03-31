#include "mainwindow.h"
#include "devices/devices.h"
#include "kdsdialog.h"
#include "pinmodel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QSettings>
#include <QShortcut>
#include <QTimer>
#include <ranges>
//QTabWidget* tw;

struct JsonData {
    JsonData() { }
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , le(24)
    , pb(24) {
    setupUi(this);
    auto model = new PinModel(tvPins);
    tvPins->setModel(model);
    tvPins->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tvPins->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
        layAmk1->addRow(pb[i /*      */] = new QPushButton(QMainWindow::centralWidget()), le[i /*      */] = new QLineEdit(this));
        layAmk2->addRow(pb[i + SetCount] = new QPushButton(QMainWindow::centralWidget()), le[i + SetCount] = new QLineEdit(this));

        le[i /*      */]->installEventFilter(this);
        le[i + SetCount]->installEventFilter(this);

        pbSetter(pb[i /*      */], i, "pushButton_%1_0", 0);
        pbSetter(pb[i + SetCount], i, "pushButton_%1_1", 1);
    }

    auto ports{QSerialPortInfo::availablePorts().toVector()};
    std::ranges::sort(ports, {}, [](const QSerialPortInfo& i1) { return i1.portName().mid(3).toInt(); });
    for(const QSerialPortInfo& pi : ports) {
        cbxPortAmk1->addItem(pi.portName());
        cbxPortAmk2->addItem(pi.portName());
        cbxPortTester->addItem(pi.portName());
    }

    connect(Devices::autoTest(), &AutoTest::finished, [this] { pbTest->setChecked(false); });
    connect(Devices::autoTest(), &AutoTest::message, this, &MainWindow::message);

    connect(Devices::tester(), &Tester::measureReadyAll, model, &PinModel::setDataA);
    connect(pbAutoMeasure, &QPushButton::toggled, Devices::tester(), &Tester::startStop);

    connect(pbClear, &QPushButton::clicked, tableView, &TableView::clear);
    connect(pbTest, &QPushButton::clicked, [&](bool checked) { checked ? emit start(tableView->model()) : emit stop(); });

    connect(pbAmkSettings1, &QPushButton::clicked, [] {
        KdsDialog d(Devices::kds1());
        d.exec();
    });

    connect(pbAmkSettings2, &QPushButton::clicked, [] {
        KdsDialog d(Devices::kds2());
        d.exec();
    });

    connect(this, &MainWindow::measurePins, Devices::tester(), &Tester::measureAll);
    connect(this, &MainWindow::start, Devices::autoTest(), &AutoTest::start, Qt::QueuedConnection);
    connect(this, &MainWindow::stop, Devices::autoTest(), &AutoTest::stop, Qt::DirectConnection);

    loadSets();
    readSettings();

    QTimer::singleShot(10, this, &MainWindow::on_pbPing_clicked);
}

MainWindow::~MainWindow() {
    writeSettings();
}

void MainWindow::writeSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("cbxAmk", cbxPortAmk1->currentText());
    settings.setValue("cbxHart", cbxPortAmk2->currentText());
    settings.setValue("cbxTester", cbxPortTester->currentText());
    settings.setValue("cbType1", cbxAmkSet1->currentIndex());
    settings.setValue("cbType2", cbxAmkSet2->currentIndex());
    settings.endGroup();
}

void MainWindow::readSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    cbxPortAmk1->setCurrentText(settings.value("cbxAmk").toString());
    cbxPortAmk2->setCurrentText(settings.value("cbxHart").toString());
    cbxPortTester->setCurrentText(settings.value("cbxTester").toString());
    cbxAmkSet1->setCurrentIndex(settings.value("cbType1").toInt());
    cbxAmkSet2->setCurrentIndex(settings.value("cbType2").toInt());
    settings.endGroup();
}

void MainWindow::message(const QString& msg) {
    if(QMessageBox::information(this, "", msg, QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
        Devices::autoTest()->sem.release();
    else
        Devices::autoTest()->sem.release(2);
}

void MainWindow::cbxTypeIndexChanged(int index) {
    saveSets();
    lastIndex = index;
    int ctr{};
    if(sender() == cbxAmkSet1) {
        for(QJsonValue value : jsonArray[lastIndex].toObject()["data"].toArray()) {
            m_points[0][ctr] = Point(value.toObject());
            le[ctr]->setText(m_points[0][ctr].Description);
            ++ctr;
        }
    } else {
        for(QJsonValue value : jsonArray[lastIndex].toObject()["data"].toArray()) {
            m_points[1][ctr] = Point(value.toObject());
            le[ctr + SetCount]->setText(m_points[1][ctr].Description);
            ++ctr;
        }
    }
}

void MainWindow::loadSets() {
    QFile loadFile(QStringLiteral("data.json"));
A:
    if(!loadFile.open(QIODevice::ReadOnly)) {
        QFile saveFile(QStringLiteral("data.json"));
        if(!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open save file.");
            return;
        }
        saveFile.write("[{\"data\":["
                       "{\"name\":\"0Ом\",\"value\":\"128\"},"
                       "{\"name\":\"40Ом\",\"value\":\"256\"},"
                       "{\"name\":\"80Ом\",\"value\":\"16\"},"
                       "{\"name\":\"160Ом\",\"value\":\"32\"},"
                       "{\"name\":\"320Ом\",\"value\":\"64\"},"
                       "{\"name\":\"+10В\",\"value\":\"512\"},"
                       "{\"name\":\"-10В\",\"value\":\"1536\"},"
                       "{\"name\":\"+IРМТ\",\"value\":\"2\"},"
                       "{\"name\":\"-IРМТ\",\"value\":\"1026\"},"
                       "{\"name\":\"+UРМТ\",\"value\":\"1\"},"
                       "{\"name\":\"-UРМТ\",\"value\":\"1025\"},"
                       "{\"name\":\"0\",\"value\":\"0\"}],\"name\":\"КДС\"},"
                       "{\"data\":["
                       "{\"name\":\"20мА-0Ом\",\"value\":\"640\"},"
                       "{\"name\":\"20мА-400Ом\",\"value\":\"768\"},"
                       "{\"name\":\"20мА-500Ом\",\"value\":\"528\"},"
                       "{\"name\":\"20мА-1кОм\",\"value\":\"544\"},"
                       "{\"name\":\"20мА-2кОм\",\"value\":\"576\"},"
                       "{\"name\":\"5мА-0Ом\",\"value\":\"132\"},"
                       "{\"name\":\"5мА-400Ом\",\"value\":\"260\"},"
                       "{\"name\":\"5мА-500Ом\",\"value\":\"20\"},"
                       "{\"name\":\"5мА-1кОм\",\"value\":\"36\"},"
                       "{\"name\":\"5мА-2кОм\",\"value\":\"68\"},"
                       "{\"name\":\"0\",\"value\":\"0\"},"
                       "{\"name\":\"0\",\"value\":\"0\"}],\"name\":\"MN\"},"
                       "{\"data\":["
                       "{\"name\":\"1\",\"value\":\"1\"},"
                       "{\"name\":\"2\",\"value\":\"2\"},"
                       "{\"name\":\"3\",\"value\":\"4\"},"
                       "{\"name\":\"4\",\"value\":\"8\"},"
                       "{\"name\":\"5\",\"value\":\"16\"},"
                       "{\"name\":\"6\",\"value\":\"32\"},"
                       "{\"name\":\"7\",\"value\":\"64\"},"
                       "{\"name\":\"8\",\"value\":\"128\"},"
                       "{\"name\":\"9\",\"value\":\"256\"},"
                       "{\"name\":\"10\",\"value\":\"512\"},"
                       "{\"name\":\"11\",\"value\":\"1024\"},"
                       "{\"name\":\"12\",\"value\":\"2048\"}],\"name\":\"HC\"},"
                       "{\"data\":["
                       "{\"name\":\"+IТМ\",\"value\":\"8\"},"
                       "{\"name\":\"-IТМ\",\"value\":\"1032\"},"
                       "{\"name\":\"+UТМ\",\"value\":\"4\"},"
                       "{\"name\":\"-UТМ\",\"value\":\"1028\"},"
                       "{\"name\":\"+10В\",\"value\":\"512\"},"
                       "{\"name\":\"-10В\",\"value\":\"1536\"},"
                       "{\"name\":\"+IРМТ\",\"value\":\"2\"},"
                       "{\"name\":\"-IРМТ\",\"value\":\"1026\"},"
                       "{\"name\":\"+UРМТ\",\"value\":\"1\"},"
                       "{\"name\":\"-UРМТ\",\"value\":\"1025\"},"
                       "{\"name\":\"0\",\"value\":\"0\"},"
                       "{\"name\":\"0\",\"value\":\"0\"}],\"name\":\"КДС старый\"},"
                       "{\"data\":["
                       "{\"name\":\"1\",\"value\":\"1\"},"
                       "{\"name\":\"2\",\"value\":\"2\"},"
                       "{\"name\":\"4\",\"value\":\"4\"},"
                       "{\"name\":\"8\",\"value\":\"8\"},"
                       "{\"name\":\"16\",\"value\":\"16\"},"
                       "{\"name\":\"32\",\"value\":\"32\"},"
                       "{\"name\":\"64\",\"value\":\"64\"},"
                       "{\"name\":\"128\",\"value\":\"128\"},"
                       "{\"name\":\"256\",\"value\":\"256\"},"
                       "{\"name\":\"512\",\"value\":\"512\"},"
                       "{\"name\":\"1024\",\"value\":\"1024\"},"
                       "{\"name\":\"0\",\"value\":\"0\"}],\"name\":\"TEST\"}]");
        qWarning("Couldn't open save file.");
        goto A;
    }

    QByteArray array(loadFile.readAll());
    QJsonParseError err;
    QJsonDocument loadDoc(QJsonDocument::fromJson(array, &err));

    jsonArray = loadDoc.array();
    qDebug() << err.errorString();

    for(QJsonValue value : jsonArray) {
        QJsonObject object(value.toObject());
        cbxAmkSet1->addItem(object["name"].toString());
        cbxAmkSet2->addItem(object["name"].toString());
    }

    int i = 0;

    for(QJsonValue value : jsonArray[lastIndex].toObject()["data"].toArray()) {
        QJsonObject object2(value.toObject());
        m_points[0][i] = Point(value.toObject());
        m_points[1][i] = Point(value.toObject());
        le[i /*      */]->setText(m_points[0][i].Description);
        le[i + SetCount]->setText(m_points[1][i].Description);
        ++i;
    }

    connect(cbxAmkSet1, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::cbxTypeIndexChanged);
    connect(cbxAmkSet2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::cbxTypeIndexChanged);
}

void MainWindow::saveSets() {
    QFile saveFile(QStringLiteral("data.json"));

    if(!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << saveFile.errorString();
        return;
    }

    if(cbxAmkSet1->count() > jsonArray.count()) {
        QJsonObject levelObject;
        levelObject["name"] = cbxAmkSet1->itemText(lastIndex);
        QJsonArray array;
        for(Point& p : m_points[0]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }
        levelObject["data"] = array;
        jsonArray.append(levelObject);
    } else {
        QJsonObject levelObject;
        levelObject["name"] = cbxAmkSet1->itemText(lastIndex);
        QJsonArray array;
        for(Point& p : m_points[0]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }
        levelObject["data"] = array;
        jsonArray[lastIndex] = levelObject;
    }
    if(cbxAmkSet2->count() > jsonArray.count()) {
        QJsonObject levelObject;
        levelObject["name"] = cbxAmkSet2->itemText(lastIndex);
        QJsonArray array;
        for(Point& p : m_points[1]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }

        levelObject["data"] = array;
        jsonArray.append(levelObject);
    } else {
        QJsonObject levelObject;
        levelObject["name"] = cbxAmkSet2->itemText(lastIndex);
        QJsonArray array;
        for(Point& p : m_points[1]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }
        levelObject["data"] = array;
        jsonArray[lastIndex] = levelObject;
    }

    QJsonDocument saveDoc(jsonArray);
    saveFile.write(saveDoc.toJson());
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if(event->type() == QEvent::MouseButtonDblClick) {
        int m_numPoint = le.indexOf(reinterpret_cast<QLineEdit*>(obj));
        if(m_numPoint > -1) {
            PointEdit pe(&m_points[m_numPoint > 11 ? 1 : 0][m_numPoint % SetCount], le[m_numPoint], this);
            pe.exec();
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
        for(int i = 0; i < SetCount; ++i)
            if(m_numPoint != i)
                pb[i]->setChecked(false);
            else
                pushButton->setChecked(true);
        if(Devices::kds1()->setRelay(m_points[0][m_numPoint % SetCount].Parcel.toInt()))
            //emit message("Прибором КДС успешно переключен!");
            return;
    } else {
        for(int i = SetCount; i < 24; ++i)
            if(m_numPoint != i)
                pb[i]->setChecked(false);
            else
                pushButton->setChecked(true);
        if(Devices::kds2()->setRelay(m_points[1][m_numPoint % SetCount].Parcel.toInt()))
            //emit message("Прибором КДС успешно переключен!");
            return;
    }
    //emit message("Нет связи с прибором КДС!");
}

void MainWindow::on_pbPing_clicked() {
    for(bool en = Devices::kds1()->ping(cbxPortAmk1->currentText());
        auto w : gbxKds1->findChildren<QWidget*>())
        w->setEnabled(en);

    for(bool en = Devices::kds2()->ping(cbxPortAmk2->currentText());
        auto w : gbxKds2->findChildren<QWidget*>())
        w->setEnabled(en);

    for(bool en = Devices::tester()->ping(cbxPortTester->currentText());
        auto w : gbxTest->findChildren<QWidget*>())
        w->setEnabled(en);

    cbxPortAmk1->setEnabled(true);
    cbxPortAmk2->setEnabled(true);
    cbxPortTester->setEnabled(true);
}
