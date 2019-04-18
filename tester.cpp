#include "tester.h"
//#include "ui_tester.h"
#include "hwinterface/interface.h"

#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>

TESTER::TESTER(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(Interface::tester(), &AmkTester::measureReady, this, &TESTER::SetValue);
    connect(this, &TESTER::MeasurePin, Interface::tester(), &AmkTester::measurePin);
    connect(&timer, &QTimer::timeout, [&]() {
        if (s.tryAcquire()) {
            static int i = 0;
            //            if (i > 10)
            //                i = 0;
            emit MeasurePin(i++ % 11);
        }
    });

    connect(pbStartStop, &QPushButton::clicked, [&](bool checked) {
        if (Interface::tester()->Ping())
            if (checked) {
                s.release();
                timer.start(1);
            } else {
                timer.stop();
            }
        else {
            pbStartStop->setChecked(false);
        }
    });
}

TESTER::~TESTER()
{
    timer.stop();
}

void TESTER::setupUi(QWidget* Form)
{
    if (Form->objectName().isEmpty())
        Form->setObjectName(QStringLiteral("Form"));
    Form->resize(557, 301);

    pbStartStop = new QPushButton(Form);
    pbStartStop->setObjectName(QStringLiteral("pbStartStop"));
    pbStartStop->setCheckable(true);

    tableView = new QTableView(Form);
    tableView->setObjectName(QStringLiteral("tableView"));

    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QVBoxLayout* verticalLayout = new QVBoxLayout(Form);
    verticalLayout->setMargin(0);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->addWidget(pbStartStop);
    verticalLayout->addWidget(tableView);
    model = new MyTableModel(tableView);
    tableView->setModel(model);

    retranslateUi(Form);

    QMetaObject::connectSlotsByName(Form);
}

void TESTER::retranslateUi(QWidget* Form)
{
    Form->setWindowTitle("Form");
    pbStartStop->setText("pbStartStop");
}

void TESTER::SetValue(const QVector<quint16>& value)
{
    model->setData(value);
    s.release();
}
#include <QHeaderView>

void TESTER::resizeEvent(QResizeEvent* event)
{
    QFont f;
    f.setPixelSize(tableView->height() / 24);
    tableView->setFont(f);
    QWidget::resizeEvent(event);
}
