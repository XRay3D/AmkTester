#include "tester.h"
//#include "ui_tester.h"
#include "hwinterface/interface.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QDebug>

TESTER::TESTER(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(Interface::tester(), &AmkTester::SetValue, this, &TESTER::SetValue);
    connect(this, &TESTER::MeasurePin, Interface::tester(), &AmkTester::MeasurePin);
    connect(&timer, &QTimer::timeout, [&]() {
        if (s.tryAcquire()) {
            static int i = 0;
            if (i > 10)
                i = 0;
            emit MeasurePin(i++ /* % 11*/);
        }
    });

    connect(pbStartStop, &QPushButton::clicked, [&](bool checked) {
        if (Interface::tester()->Ping())
            if (checked) {
                s.release();
                timer.start(1);
            }
            else {
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
    model->setValue(value);
    s.release();
}
#include <QHeaderView>

void TESTER::resizeEvent(QResizeEvent* event)
{
    QSize size(
        tableView->size().width() - tableView->verticalHeader()->width() - 2,
        tableView->size().height() - tableView->horizontalHeader()->height() - 2);

    QVector<int> h(12, floor(size.height() / 11.0));
    h.last() = size.height() - h.first() * 11;

    QVector<int> w(12, floor(size.width() / 11.0));
    w.last() = size.width() - w.first() * 11;

    int i = 0;
    while (h.last()-- > 0) {
        ++h[i++ % 11];
    }
    i = 0;
    while (w.last()-- > 0) {
        ++w[i++ % 11];
    }
    for (int i = 0; i < 11; ++i) {
        tableView->setRowHeight(i, h[i]);
        tableView->setColumnWidth(i, w[i]);
    }

    QWidget::resizeEvent(event);
}
