#include "testerdata.h"
#include "hwinterface/interface.h"
#include "mytablemodel.h"

#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>

TesterData::TesterData(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(Interface::tester(), &Tester::measureReady, this, &TesterData::SetValue);
    connect(this, &TesterData::MeasurePin, Interface::tester(), &Tester::measurePin);
    connect(&timer, &QTimer::timeout, [&]() {
        if (s.tryAcquire()) {
            static int i = 0;
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

TesterData::~TesterData()
{
    timer.stop();
}

int** TesterData::data() const { return m_model->getData(); }

void TesterData::setupUi(QWidget* Form)
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
    m_model = new MyTableModel(tableView);
    tableView->setModel(m_model);

    retranslateUi(Form);

    QMetaObject::connectSlotsByName(Form);
}

void TesterData::retranslateUi(QWidget* Form)
{
    Form->setWindowTitle("Form");
    pbStartStop->setText("pbStartStop");
}

void TesterData::SetValue(const QVector<quint16>& value)
{
    m_model->setRawData(value);
    s.release();
}
#include <QHeaderView>

void TesterData::resizeEvent(QResizeEvent* event)
{
    QFont f;
    f.setPixelSize(std::min(tableView->height(), tableView->width()) / 25);
    tableView->setFont(f);
    QWidget::resizeEvent(event);
}
