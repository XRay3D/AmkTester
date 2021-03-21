#include "mainwindow.h"
#include "hwinterface/interface.h"
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
#include <ranges>
//QTabWidget* tw;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , le(24)
    , pb(24)
{
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

    for (int i = 0; i < 12; ++i) {
        flKds0->addRow(pb[i + 00] = new QPushButton(QMainWindow::centralWidget()), le[i + 00] = new QLineEdit(this));
        flKds1->addRow(pb[i + 12] = new QPushButton(QMainWindow::centralWidget()), le[i + 12] = new QLineEdit(this));

        le[i + 00]->installEventFilter(this);
        le[i + 12]->installEventFilter(this);

        pbSetter(pb[i + 00], i, "pushButton_%1_0", 0);
        pbSetter(pb[i + 12], i, "pushButton_%1_1", 1);
    }

    auto ports(QSerialPortInfo::availablePorts().toVector());
    std::ranges::sort(ports, {}, [](const QSerialPortInfo& i1) { return i1.portName().mid(3).toInt(); });
    for (const QSerialPortInfo& pi : ports) {
        cbxPortAmk->addItem(pi.portName());
        cbxPortHart->addItem(pi.portName());
        cbxPortTester->addItem(pi.portName());
    }

    //    tableView->initCheckBox2();
    //    connect(pushButton, &QPushButton::clicked, [this] {
    //        tableView->addRow(cbxAmk->m_points[cbxAmk->m_numPoint].Description);
    //    });
    //    connect(pushButton_3, &QPushButton::clicked, [this] {
    //        tableView->setPattern(
    //            widget_2->data(),
    //            wAmk1->m_points[wAmk1->m_numPoint],
    //            wAmk2->m_points[wAmk2->m_numPoint]);
    //    });

    connect(HW::autoTest(), &AutoTest::finished, [this] { pbTest->setChecked(false); });
    connect(HW::autoTest(), &AutoTest::message, this, &MainWindow::message);

    connect(HW::tester(), &Tester::measureReadyAll, [model, this](const PinsValue& value) {
        for (int i = 0; i < 11; ++i)
            model->setRawData({ //
                static_cast<unsigned short>(value.data[i][0]),
                static_cast<unsigned short>(value.data[i][1]),
                static_cast<unsigned short>(value.data[i][2]),
                static_cast<unsigned short>(value.data[i][3]),
                static_cast<unsigned short>(value.data[i][4]),
                static_cast<unsigned short>(value.data[i][5]),
                static_cast<unsigned short>(value.data[i][6]),
                static_cast<unsigned short>(value.data[i][7]),
                static_cast<unsigned short>(value.data[i][8]),
                static_cast<unsigned short>(value.data[i][9]),
                static_cast<unsigned short>(value.data[i][10]),
                static_cast<unsigned short>(i) });
        pinSemaphore.tryAcquire();
    });

    connect(pbClear, &QPushButton::clicked, tableView, &TableView::clear);
    connect(pbTest, &QPushButton::clicked, [&](bool checked) { checked ? emit start(tableView->model()) : emit stop(); });

    connect(pbSettings1, &QPushButton::clicked, [] {
        KdsDialog d(HW::kds1());
        d.exec();
    });

    connect(pbSettings2, &QPushButton::clicked, [] {
        KdsDialog d(HW::kds2());
        d.exec();
    });

    connect(this, &MainWindow::measurePins, HW::tester(), &Tester::measureAll);
    connect(this, &MainWindow::start, HW::autoTest(), &AutoTest::start, Qt::QueuedConnection);
    connect(this, &MainWindow::stop, HW::autoTest(), &AutoTest::stop, Qt::DirectConnection);

    loadSettings();
    readSettings();

    QTimer::singleShot(10, this, &MainWindow::on_pbPing_clicked);
}

MainWindow::~MainWindow()
{
    if (timerId)
        killTimer(timerId);
    thread()->msleep(100);
    writeSettings();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("cbxAmk", cbxPortAmk->currentText());
    settings.setValue("cbxHart", cbxPortHart->currentText());
    settings.setValue("cbxTester", cbxPortTester->currentText());
    settings.setValue("cbType1", comboBox->currentIndex());
    settings.setValue("cbType2", comboBox_2->currentIndex());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    cbxPortAmk->setCurrentText(settings.value("cbxAmk").toString());
    cbxPortHart->setCurrentText(settings.value("cbxHart").toString());
    cbxPortTester->setCurrentText(settings.value("cbxTester").toString());
    comboBox->setCurrentIndex(settings.value("cbType1").toInt());
    comboBox_2->setCurrentIndex(settings.value("cbType2").toInt());
    settings.endGroup();
}

void MainWindow::message(const QString& msg)
{
    if (QMessageBox::information(this, "", msg, QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
        HW::autoTest()->sem.release();
    else
        HW::autoTest()->sem.release(2);
}

void MainWindow::cbxTypeIndexChanged(int index)
{
    saveSettings();
    lastIndex = index;
    int i = 0;
    if (sender() == comboBox) {
        for (QJsonValue value : jsonArray[lastIndex].toObject()["data"].toArray()) {
            m_points[0][i] = PointEdit::Point(value.toObject());
            le[i]->setText(m_points[0][i].Description);
            ++i;
        }
    } else {
        for (QJsonValue value : jsonArray[lastIndex].toObject()["data"].toArray()) {
            m_points[1][i] = PointEdit::Point(value.toObject());
            le[i + 12]->setText(m_points[1][i].Description);
            ++i;
        }
    }
}

void MainWindow::loadSettings()
{
    QFile loadFile(saveFormat == Json ? QStringLiteral("data.json") : QStringLiteral("data.dat"));
A:
    if (!loadFile.open(QIODevice::ReadOnly)) {
        QFile saveFile(saveFormat == Json ? QStringLiteral("data.json") : QStringLiteral("data.dat"));
        if (!saveFile.open(QIODevice::WriteOnly)) {
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
                       "{\"name\":\"0\",\"value\":\"0\"}],\"name\":\"КДСстарый\"},"
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
    QJsonDocument loadDoc(saveFormat == Json ? QJsonDocument::fromJson(array, &err) : QJsonDocument::fromBinaryData(array));

    jsonArray = loadDoc.array();
    qDebug() << err.errorString();

    for (QJsonValue value : jsonArray) {
        QJsonObject object(value.toObject());
        comboBox->addItem(object["name"].toString());
        comboBox_2->addItem(object["name"].toString());
    }
    int i = 0;
    for (QJsonValue value : jsonArray[lastIndex].toObject()["data"].toArray()) {
        QJsonObject object2(value.toObject());
        m_points[0][i] = PointEdit::Point(value.toObject());
        m_points[1][i] = PointEdit::Point(value.toObject());
        le[i + 00]->setText(m_points[0][i].Description);
        le[i + 12]->setText(m_points[1][i].Description);
        ++i;
    }

    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::cbxTypeIndexChanged);
    connect(comboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::cbxTypeIndexChanged);
}

void MainWindow::saveSettings()
{
    QFile saveFile(saveFormat == Json ? QStringLiteral("data.json") : QStringLiteral("data.dat"));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    if (comboBox->count() > jsonArray.count()) {
        QJsonObject levelObject;
        levelObject["name"] = comboBox->itemText(lastIndex);
        QJsonArray array;
        for (PointEdit::Point& p : m_points[0]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }

        levelObject["data"] = array;
        jsonArray.append(levelObject);
    } else {
        QJsonObject levelObject;
        levelObject["name"] = comboBox->itemText(lastIndex);
        QJsonArray array;
        for (PointEdit::Point& p : m_points[0]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }
        levelObject["data"] = array;
        jsonArray[lastIndex] = levelObject;
    }

    if (comboBox_2->count() > jsonArray.count()) {
        QJsonObject levelObject;
        levelObject["name"] = comboBox_2->itemText(lastIndex);
        QJsonArray array;
        for (PointEdit::Point& p : m_points[1]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }

        levelObject["data"] = array;
        jsonArray.append(levelObject);
    } else {
        QJsonObject levelObject;
        levelObject["name"] = comboBox_2->itemText(lastIndex);
        QJsonArray array;
        for (PointEdit::Point& p : m_points[1]) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }
        levelObject["data"] = array;
        jsonArray[lastIndex] = levelObject;
    }

    QJsonDocument saveDoc(jsonArray);
    saveFile.write(saveFormat == Json ? saveDoc.toJson() : saveDoc.toBinaryData());
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        int m_numPoint = le.indexOf(reinterpret_cast<QLineEdit*>(obj));
        if (m_numPoint > -1) {
            PointEdit pe(&m_points[m_numPoint > 11 ? 1 : 0][m_numPoint % 12], le[m_numPoint], this);
            pe.exec();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MainWindow::switchSlot()
{
    QPushButton* pushButton = qobject_cast<QPushButton*>(sender());
    int m_numPoint = pb.indexOf(pushButton);
    qDebug() << "m_numPoint" << m_numPoint;
    if (m_numPoint < 12) {
        for (int i = 0; i < 12; ++i)
            if (m_numPoint != i)
                pb[i]->setChecked(false);
            else
                pushButton->setChecked(true);
        if (HW::kds1()->setRelay(m_points[0][m_numPoint % 12].Parcel.toInt()))
            //emit message("Прибором КДС успешно переключен!");
            return;
    } else {
        for (int i = 12; i < 24; ++i)
            if (m_numPoint != i)
                pb[i]->setChecked(false);
            else
                pushButton->setChecked(true);
        if (HW::kds2()->setRelay(m_points[1][m_numPoint % 12].Parcel.toInt()))
            //emit message("Прибором КДС успешно переключен!");
            return;
    }
    //emit message("Нет связи с прибором КДС!");
}

void MainWindow::on_pbPing_clicked()
{
    for (bool en = HW::kds1()->ping(cbxPortAmk->currentText());
         auto w : gbxKds1->findChildren<QWidget*>())
        w->setEnabled(en);

    for (bool en = HW::kds2()->ping(cbxPortHart->currentText());
         auto w : gbxKds2->findChildren<QWidget*>())
        w->setEnabled(en);

    for (bool en = HW::tester()->ping(cbxPortTester->currentText());
         auto w : gbxTest->findChildren<QWidget*>())
        w->setEnabled(en);

    cbxPortAmk->setEnabled(true);
    cbxPortHart->setEnabled(true);
    cbxPortTester->setEnabled(true);
}

void MainWindow::on_pbAutoMeasure_clicked(bool checked)
{
    if (checked) {
        timerId = startTimer(10);
    } else {
        killTimer(timerId);
        timerId = 0;
    }
}

void MainWindow::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == timerId) {
        if (!pinSemaphore.available()) {
            pinSemaphore.release();
            emit measurePins();
        }
    }
}
