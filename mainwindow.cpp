#include "mainwindow.h"
#include "hwinterface/interface.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>
//QTabWidget* tw;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , le(24)
    , pb(24)
{
    setupUi(this);

    auto pbSetter = [this](QPushButton* pb, int i, const QString objName, bool fl) {
        pb->setObjectName(QString(objName).arg(i));
        pb->setCheckable(true);
        pb->setMinimumSize(0, 0);
        pb->setShortcut(fl ? QKeySequence { QString("Shift+F%1").arg(i + 1) } : QKeySequence { Qt::Key_F1 + i });
        pb->setText(pb->shortcut().toString());
        connect(pb, &QPushButton::clicked, this, &MainWindow::switchSlot);
    };

    for (int i = 0; i < 12; ++i) {
        flKds0->addRow(pb[i + 00] = new QPushButton(this), le[i + 00] = new QLineEdit(this));
        flKds1->addRow(pb[i + 12] = new QPushButton(this), le[i + 12] = new QLineEdit(this));

        le[i + 00]->installEventFilter(this);
        le[i + 12]->installEventFilter(this);

        pbSetter(pb[i + 00], i, "pushButton_%1_0", 0);
        pbSetter(pb[i + 12], i, "pushButton_%1_1", 1);
    }

    static QList<QSerialPortInfo> pl(QSerialPortInfo::availablePorts());
    std::sort(pl.begin(), pl.end(), [](const QSerialPortInfo& i1, const QSerialPortInfo& i2) {
        return i1.portName().mid(3).toInt() < i2.portName().mid(3).toInt();
    });
    for (const QSerialPortInfo& pi : pl) {
        cbxAmk->addItem(pi.portName());
        cbxHart->addItem(pi.portName());
        cbxTester->addItem(pi.portName());
    }

    tableView->initCheckBox2();
    connect(pushButton, &QPushButton::clicked, [this] {
        tableView->addRow(cbxAmk->m_points[cbxAmk->m_numPoint].Description);
    });
    connect(pushButton_3, &QPushButton::clicked, [this] {
        tableView->setPattern(
            widget_2->data(),
            wAmk1->m_points[wAmk1->m_numPoint],
            wAmk2->m_points[wAmk2->m_numPoint]);
    });

    connect(pushButton_2, &QPushButton::clicked, tableView, &TableView::clear);
    connect(this, &MainWindow::start, Interface::at(), &AutoTest::start, Qt::QueuedConnection);
    connect(this, &MainWindow::stop, Interface::at(), &AutoTest::stop, Qt::DirectConnection);
    connect(Interface::at(), &AutoTest::finished, [this] { pushButton_4->setChecked(false); });
    connect(Interface::at(), &AutoTest::message, this, &MainWindow::message);
    connect(pushButton_4, &QPushButton::clicked, [&](bool checked) {
        if (checked)
            emit start(tableView->model());
        else
            emit stop();
    });
    loadSettings();
    readSettings();
    on_pbPing_clicked();
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
    cbxAmk->setCurrentIndex(settings.value("cbxAmk").toInt());
    cbxHart->setCurrentIndex(settings.value("cbxHart").toInt());
    cbxTester->setCurrentIndex(settings.value("cbxTester").toInt());
    comboBox->setCurrentIndex(settings.value("cbType1").toInt());
    comboBox_2->setCurrentIndex(settings.value("cbType2").toInt());
    settings.endGroup();
}

void MainWindow::message(const QString& msg)
{
    if (QMessageBox::information(this, "", msg, QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
        Interface::at()->sem.release();
    else
        Interface::at()->sem.release(2);
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

//void MainWindow::setupUi()
//{
//    if (objectName().isEmpty())
//        setObjectName(QStringLiteral("AMK"));
//    QFormLayout* formLayout = new QFormLayout(this);
//    formLayout->setSpacing(6);
//    formLayout->setContentsMargins(11, 11, 11, 11);
//    formLayout->setObjectName(QStringLiteral("formLayout"));
//    formLayout->setHorizontalSpacing(4);
//    formLayout->setVerticalSpacing(4);
//    formLayout->setContentsMargins(0, 0, 0, 0);
//    cbType = new QComboBox(this);
//    cbType->setObjectName(QStringLiteral("cbType"));
//    cbType->setEditable(true);
//    formLayout->setWidget(0, QFormLayout::SpanningRole, cbType);
//    int i = 0;
//    for (QPushButton*& pushButton : pb) {
//        pushButton = new QPushButton(this);
//        pushButton->setCheckable(true);
//        pushButton->setMinimumSize(0, 0);
//        pushButton->setObjectName(QString("pushButton_%1").arg(i));
//        connect(pushButton, &QPushButton::clicked, this, &AmkTest::switchSlot);
//        formLayout->setWidget(++i, QFormLayout::LabelRole, pushButton);
//    }
//    i = 0;
//    for (QLineEdit*& lineEdit : le) {
//        lineEdit = new QLineEdit(this);
//        lineEdit->installEventFilter(this);
//        lineEdit->setEnabled(true);
//        lineEdit->setObjectName(QString("lineEdit_%1").arg(i));
//        lineEdit->setReadOnly(true);
//        formLayout->setWidget(++i, QFormLayout::FieldRole, lineEdit);
//    }
//    QMetaObject::connectSlotsByName(this);
//}
//void MainWindow::switchSlot()
//{
//    for (QPushButton* pushButton : pb)
//        pushButton->setChecked(false);
//    QPushButton* pushButton = qobject_cast<QPushButton*>(sender());
//    pushButton->setChecked(true);
//    m_numPoint = pb.indexOf(pushButton);
//    Amk* ptr = !fl ? Interface::kds1() : Interface::kds2();
//    if (ptr->setOut(0, m_points[m_numPoint].Parcel.toInt()))
//        emit message("Прибором КДС успешно переключен!");
//    else
//        emit message("Нет связи с прибором КДС!");
//}

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
        if (Interface::kds1()->setOut(0, m_points[0][m_numPoint % 12].Parcel.toInt()))
            //emit message("Прибором КДС успешно переключен!");
            return;
    } else {
        for (int i = 12; i < 24; ++i)
            if (m_numPoint != i)
                pb[i]->setChecked(false);
            else
                pushButton->setChecked(true);
        if (Interface::kds2()->setOut(0, m_points[1][m_numPoint % 12].Parcel.toInt()))
            //emit message("Прибором КДС успешно переключен!");
            return;
    }
    //emit message("Нет связи с прибором КДС!");
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
