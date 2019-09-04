#include "amktest.h"
#include <QComboBox>
#include <QFile>
#include <QFormLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLineEdit>
#include <QPushButton>
#include <hwinterface/interface.h>

AmkTest::AmkTest(QWidget* parent)
    : QWidget(parent)
    , le(12, nullptr)
    , pb(12, nullptr)
{
    setupUi();
    loadSettings();
}

AmkTest::~AmkTest()
{
    saveSettings();
}

void AmkTest::cbxTypeIndexChanged(int index)
{
    saveSettings();
    lastIndex = index;
    int i = 0;
    for (QJsonValue value : jsonArray[lastIndex].toObject()["data"].toArray()) {
        const QJsonObject object2(value.toObject());
        m_points[i].Parcel = object2["value"].toString();
        m_points[i].Description = object2["name"].toString();
        le[i]->setText(m_points[i].Description);
        ++i;
    }
}

void AmkTest::loadSettings()
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
                       "{\"name\":\"1\",\"value\":\"2\"},"
                       "{\"name\":\"2\",\"value\":\"4\"},"
                       "{\"name\":\"3\",\"value\":\"8\"},"
                       "{\"name\":\"4\",\"value\":\"16\"},"
                       "{\"name\":\"5\",\"value\":\"32\"},"
                       "{\"name\":\"6\",\"value\":\"64\"},"
                       "{\"name\":\"7\",\"value\":\"128\"},"
                       "{\"name\":\"8\",\"value\":\"256\"},"
                       "{\"name\":\"9\",\"value\":\"512\"},"
                       "{\"name\":\"10\",\"value\":\"1024\"},"
                       "{\"name\":\"11\",\"value\":\"2048\"},"
                       "{\"name\":\"12\",\"value\":\"4096\"}],\"name\":\"HC\"},"
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
    //qDebug() << jsonArray << err.errorString();

    for (QJsonValue value : jsonArray) {
        QJsonObject object(value.toObject());
        cbType->addItem(object["name"].toString());
    }
    int i = 0;
    for (QJsonValue value : jsonArray[lastIndex].toObject()["data"].toArray()) {
        QJsonObject object2(value.toObject());
        m_points[i].Parcel = object2["value"].toString();
        m_points[i].Description = object2["name"].toString();
        le[i]->setText(m_points[i].Description);
        ++i;
    }

    connect(cbType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AmkTest::cbxTypeIndexChanged);
}

void AmkTest::saveSettings()
{
    QFile saveFile(saveFormat == Json ? QStringLiteral("data.json") : QStringLiteral("data.dat"));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    if (cbType->count() > jsonArray.count()) {
        QJsonObject levelObject;
        levelObject["name"] = cbType->itemText(lastIndex);
        QJsonArray array;
        for (PointEdit::Point& p : m_points) {
            QJsonObject jsonObject;
            jsonObject["name"] = p.Description;
            jsonObject["value"] = p.Parcel;
            array.append(jsonObject);
        }

        levelObject["data"] = array;
        jsonArray.append(levelObject);
    } else {
        QJsonObject levelObject;
        levelObject["name"] = cbType->itemText(lastIndex);
        QJsonArray array;
        for (PointEdit::Point& p : m_points) {
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

void AmkTest::setupUi()
{
    if (objectName().isEmpty())
        setObjectName(QStringLiteral("AMK"));

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->setSpacing(6);
    formLayout->setContentsMargins(11, 11, 11, 11);
    formLayout->setObjectName(QStringLiteral("formLayout"));
    formLayout->setHorizontalSpacing(4);
    formLayout->setVerticalSpacing(4);
    formLayout->setContentsMargins(0, 0, 0, 0);

    cbType = new QComboBox(this);
    cbType->setObjectName(QStringLiteral("cbType"));
    cbType->setEditable(true);
    formLayout->setWidget(0, QFormLayout::SpanningRole, cbType);

    int i = 0;
    for (QPushButton*& pushButton : pb) {
        pushButton = new QPushButton(this);
        pushButton->setCheckable(true);
        pushButton->setMinimumSize(0, 0);
        pushButton->setObjectName(QString("pushButton_%1").arg(i));
        connect(pushButton, &QPushButton::clicked, this, &AmkTest::switchSlot);
        formLayout->setWidget(++i, QFormLayout::LabelRole, pushButton);
    }
    i = 0;
    for (QLineEdit*& lineEdit : le) {
        lineEdit = new QLineEdit(this);
        lineEdit->installEventFilter(this);
        lineEdit->setEnabled(true);
        lineEdit->setObjectName(QString("lineEdit_%1").arg(i));
        lineEdit->setReadOnly(true);
        formLayout->setWidget(++i, QFormLayout::FieldRole, lineEdit);
    }

    QMetaObject::connectSlotsByName(this);
}

void AmkTest::switchSlot()
{
    for (QPushButton* pushButton : pb)
        pushButton->setChecked(false);
    QPushButton* pushButton = qobject_cast<QPushButton*>(sender());
    pushButton->setChecked(true);
    m_numPoint = pb.indexOf(pushButton);
    Amk* ptr = !fl ? Interface::kds1() : Interface::kds2();
    if (ptr->setOut(0, m_points[m_numPoint].Parcel.toInt()))
        emit message("Прибором КДС успешно переключен!");
    else
        emit message("Нет связи с прибором КДС!");
}

void AmkTest::setFl(bool value)
{
    int i = 0;
    fl = value;
    for (QPushButton* pushButton : pb) {
        if (!value) {
            pushButton->setText(QString("F%1").arg(i + 1));
            pushButton->setShortcut(Qt::Key_F1 + i);
        } else {
            pushButton->setText(QString("Shift+F%1").arg(i + 1));
            pushButton->setShortcut(QKeySequence(QString("Shift+F%1").arg(i + 1)));
        }
        ++i;
    }
}

bool AmkTest::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        m_numPoint = le.indexOf(reinterpret_cast<QLineEdit*>(obj));
        if (m_numPoint > -1) {
            PointEdit pe(&m_points[m_numPoint], le[m_numPoint], this);
            pe.exec();
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}
