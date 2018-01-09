#include "amk.h"
#include <QDebug>
#include <QSettings>
#include "hwinterface/interface.h"

AMK::AMK(QWidget* parent)
    : QWidget(parent)
    , le(12, nullptr)
    , pb(12, nullptr)
{
    setupUi(this);
    loadSettings();
}

AMK::~AMK()
{
    saveSettings();
}

void AMK::setupUi(QWidget* Form)
{
    if (objectName().isEmpty())
        setObjectName(QStringLiteral("AMK"));

    QFormLayout* formLayout = new QFormLayout(this);
    formLayout->setSpacing(6);
    formLayout->setContentsMargins(11, 11, 11, 11);
    formLayout->setObjectName(QStringLiteral("formLayout"));
    formLayout->setHorizontalSpacing(4);
    formLayout->setVerticalSpacing(4);
    formLayout->setContentsMargins(6, 6, 6, 6);

    cbType = new QComboBox(this);
    cbType->setObjectName(QStringLiteral("cbType"));
    cbType->setEditable(true);
    formLayout->setWidget(0, QFormLayout::SpanningRole, cbType);

    int index;
    index = 1;

    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    for (int i = 0, i2 = 1; i < pb.size(); ++i, ++i2) {
        QPushButton*& with = pb[i];
        with = new QPushButton(this);
        with->setObjectName(QString("pushButton_%1").arg(i2));
        sizePolicy.setHeightForWidth(with->sizePolicy().hasHeightForWidth());
        with->setSizePolicy(sizePolicy);
        with->setMinimumSize(QSize(62, 0));
        with->setMaximumSize(QSize(40, 16777215));
        with->setText(QString("F%1").arg(i2));
        formLayout->setWidget(i2, QFormLayout::LabelRole, with);
        with->setShortcut(Qt::Key_F1 + i);
        connect(with, &QPushButton::clicked, [this, i]() { SwitchSlot(i); });
    }

    for (int i = 0, i2 = 1; i < le.size(); ++i, ++i2) {
        QLineEdit*& with = le[i];
        with = new QLineEdit(this);
        with->setObjectName(QString("lineEdit_%1").arg(index));
        with->setEnabled(true);
        with->setReadOnly(true);
        with->installEventFilter(this);
        formLayout->setWidget(i2, QFormLayout::FieldRole, with);
    }
    QMetaObject::connectSlotsByName(this);
}

void AMK::saveSettings()
{
    QSettings settings("KdsSwitcher.ini", QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    settings.beginWriteArray("PointTypes");
    for (int i = 0; i < cbType->count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("Type", cbType->itemText(i));
    }
    settings.endArray();

    settings.beginGroup(QString("PointType"));
    settings.setValue("CurrentType", cbType->currentIndex());
    settings.endGroup();

    settings.beginWriteArray(QString("Points%1").arg(m_lastPointType));
    for (int i = 0; i < 12; ++i) {
        settings.setArrayIndex(i);
        settings.setValue("Parcel", m_points[i].Parcel);
        settings.setValue("Description", m_points[i].Description);
    }

    m_lastPointType = cbType->currentIndex() + 1;

    settings.endArray();
}

void AMK::loadSettings()
{
    QSettings settings("KdsSwitcher.ini", QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    typedef void (QComboBox::*F)(int);
    disconnect(cbType, static_cast<F>(&QComboBox::currentIndexChanged), this, &AMK::CbTypeCurrentIndexChanged);
    int size = settings.beginReadArray("PointTypes");
    cbType->clear();
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        cbType->addItem(settings.value("Type").toString());
    }
    settings.endArray();

    settings.beginGroup(QString("PointType"));
    cbType->setCurrentIndex(settings.value("CurrentType", 0).toInt());
    settings.endGroup();
    connect(cbType, static_cast<F>(&QComboBox::currentIndexChanged), this, &AMK::CbTypeCurrentIndexChanged);

    m_lastPointType = cbType->currentIndex() + 1;

    size = settings.beginReadArray(QString("Points%1").arg(m_lastPointType));
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_points[i].Parcel = settings.value("Parcel", "").toString();
        m_points[i].Description = settings.value("Description", "").toString();
        le[i]->setText(m_points[i].Description);
    }
    settings.endArray();
}

void AMK::SwitchSlot(int pointNum)
{
    m_numPoint = pointNum;
    if (Interface::kds()->setOut(0, m_points[m_numPoint].Parcel.toInt()))
        qDebug() << "Прибором КДС успешно переключен!";
    else
        qDebug() << "Нет связи с прибором КДС!";
}

bool AMK::eventFilter(QObject* obj, QEvent* event)
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

void AMK::CbTypeCurrentIndexChanged(int index)
{
    saveSettings();
    loadSettings();
}
