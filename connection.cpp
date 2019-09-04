#include "connection.h"
#include <QStackedWidget>
#include <QtSerialPort>
#include <QMessageBox>
#include <hwinterface/interface.h>

extern QTabWidget* tw;

CONNECTION::CONNECTION(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    UpdateCbxPort();
    connect(pbUpdateCbxPort, &QPushButton::clicked, this, &CONNECTION::UpdateCbxPort);
    QSettings settings;
    cbxPortAmk->setCurrentIndex(settings.value("cbxPortAmk", 0).toInt());
    cbxPortTest->setCurrentIndex(settings.value("cbxPortTest", 0).toInt());

    QTimer::singleShot(100, Qt::CoarseTimer, [=]() { CheckConnection(); });
}

CONNECTION::~CONNECTION()
{
    QSettings settings;
    settings.setValue("cbxPortAmk", cbxPortAmk->currentIndex());
    settings.setValue("cbxPortTest", cbxPortTest->currentIndex());
}

void CONNECTION::setupUi(QWidget* Form)
{
    if (Form->objectName().isEmpty())
        Form->setObjectName(QStringLiteral("Form"));
    Form->resize(557, 301);

    gbxAmk = new QGroupBox(Form);
    gbxAmk->setObjectName(QStringLiteral("gbxAmk"));

    verticalLayout = new QVBoxLayout(gbxAmk);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    cbxPortAmk = new QComboBox(gbxAmk);
    cbxPortAmk->setObjectName(QStringLiteral("cbPortAmk"));

    //    cbxBaudAmk = new QComboBox(gbxAmk);
    //    cbxBaudAmk->setObjectName(QStringLiteral("cbBaudAmk"));
    //    cbxBaudAmk->addItems(QString("300,600,1200,2400,4800,9600,19200").split(','));

    //    sbAddrAmk = new QSpinBox(gbxAmk);
    //    sbAddrAmk->setObjectName(QStringLiteral("sbAddrAmk"));
    //    sbAddrAmk->setRange(0, 255);

    pbPingAmk = new QPushButton(gbxAmk);
    pbPingAmk->setObjectName(QStringLiteral("pbPingAmk"));
    connect(pbPingAmk, &QPushButton::clicked, this, &CONNECTION::CheckConnectionAmk);

    verticalLayout->addWidget(cbxPortAmk);
    //    verticalLayout->addWidget(cbxBaudAmk);
    //    verticalLayout->addWidget(sbAddrAmk);
    verticalLayout->addWidget(pbPingAmk);
    verticalLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    gbxTest = new QGroupBox(Form);
    gbxTest->setObjectName(QStringLiteral("gbxTest"));

    cbxPortTest = new QComboBox(gbxTest);
    cbxPortTest->setObjectName(QStringLiteral("cbPortTest"));

    pbPingTest = new QPushButton(gbxTest);
    pbPingTest->setObjectName(QStringLiteral("pbPingTest"));
    connect(pbPingTest, &QPushButton::clicked, this, &CONNECTION::CheckConnectionTest);

    pbUpdateCbxPort = new QPushButton(gbxTest);
    pbUpdateCbxPort->setObjectName(QStringLiteral("pbUpdateTestPort"));

    verticalLayout_2 = new QVBoxLayout(gbxTest);
    verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
    verticalLayout_2->addWidget(cbxPortTest);
    verticalLayout_2->addWidget(pbPingTest);
    verticalLayout_2->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    gridLayout = new QGridLayout(Form);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));
    gridLayout->addWidget(gbxAmk, 0, 0, 1, 1);
    gridLayout->addWidget(gbxTest, 0, 1, 1, 1);
    gridLayout->addWidget(pbUpdateCbxPort, 1, 0, 1, 2);

    retranslateUi(Form);

    //    QMetaObject::connectSlotsByName(Form);
}

void CONNECTION::retranslateUi(QWidget* Form)
{
    Form->setWindowTitle("Form");
    gbxAmk->setTitle("AMK");
    pbPingAmk->setText("pbPingAmk");
    gbxTest->setTitle("TESTER");
    pbPingTest->setText("pbPingTest");
    pbUpdateCbxPort->setText("pbUpdateTestPort");
}

void CONNECTION::UpdateCbxPort()
{
    cbxPortAmk->clear();
    cbxPortTest->clear();
    foreach (QSerialPortInfo portInfo, QSerialPortInfo::availablePorts()) {
        cbxPortAmk->addItem(portInfo.portName());
        if (portInfo.manufacturer().contains("FTDI", Qt::CaseInsensitive))
            cbxPortTest->addItem(portInfo.portName());
    }
    qDebug() << parent();
}

void CONNECTION::CheckConnection()
{
    if (CheckConnectionTest()) {
    }
    if (CheckConnectionAmk()) {
    }
}

bool CONNECTION::CheckConnectionAmk()
{
    if (!Interface::kds1()->Ping(cbxPortAmk->currentText()))
        QMessageBox::warning(nullptr, "kds", "kds");
    return Interface::kds1()->IsConnected();
}

bool CONNECTION::CheckConnectionTest()
{
    if (!Interface::tester()->Ping(cbxPortTest->currentText()))
        QMessageBox::warning(nullptr, "tester", "tester");
    return Interface::tester()->IsConnected();
}

void CONNECTION::showEvent(QShowEvent* /*event*/)
{
    //    static int i = 0;
    //    if (!i) {
    //        static_cast<QStackedWidget*>(parent())->setCurrentIndex(1);
    //        static_cast<QTabWidget*>(static_cast<QStackedWidget*>(parent())->parent())->update();
    //        ++i;
    //    }
    //    qDebug() << i;
    // tw->setCurrentIndex(1);
    //        static_cast<QStackedWidget*>(parent())->setCurrentIndex(1);
}
