//#include "testerdata.h"
//#include "hwinterface/interface.h"
//#include "mytablemodel.h"

//#include <QDebug>
//#include <QHeaderView>
//#include <QPushButton>
//#include <QResizeEvent>
//#include <QVBoxLayout>

//TesterData::TesterData(QWidget* parent)
//    : QWidget(parent)
//{
//    setupUi(this);
//    connect(Interface::tester(), &Tester::measureReadyAll, this, &TesterData::SetValue);
//    connect(this, &TesterData::MeasurePin, Interface::tester(), &Tester::measureAll);
//    connect(&timer, &QTimer::timeout, [&]() {
//        if (s.tryAcquire()) {
//            //            static int i = 0;
//            emit MeasurePin(/*i++ % 11*/);
//        }
//    });

//    connect(pbStartStop, &QPushButton::clicked, [&](bool checked) {
//        if (Interface::tester()->ping())
//            if (checked) {
//                s.release();
//                timer.start(1);
//            } else {
//                timer.stop();
//            }
//        else {
//            pbStartStop->setChecked(false);
//        }
//    });
//}

//TesterData::~TesterData()
//{
//    timer.stop();
//}

//const PinsValue& TesterData::data() const { return m_pv; }

//void TesterData::setupUi(QWidget* Form)
//{
//    if (Form->objectName().isEmpty())
//        Form->setObjectName(QStringLiteral("Form"));
//    Form->resize(557, 301);

//    pbStartStop = new QPushButton(Form);
//    pbStartStop->setObjectName(QStringLiteral("pbStartStop"));
//    pbStartStop->setCheckable(true);

//    tableView = new QTableView(Form);
//    tableView->setObjectName(QStringLiteral("tableView"));

//    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    QVBoxLayout* verticalLayout = new QVBoxLayout(Form);
//    verticalLayout->setMargin(0);
//    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
//    verticalLayout->addWidget(pbStartStop);
//    verticalLayout->addWidget(tableView);
//    m_model = new MyTableModel(tableView);
//    tableView->setModel(m_model);

//    retranslateUi(Form);

//    QMetaObject::connectSlotsByName(Form);
//}

//void TesterData::retranslateUi(QWidget* Form)
//{
//    Form->setWindowTitle("Form");
//    pbStartStop->setText("pbStartStop");
//}

//void TesterData::SetValue(const PinsValue& value)
//{
//    m_pv = value;
//    for (int i = 0; i < 11; ++i)
//        m_model->setRawData({ static_cast<unsigned short>(value.data[i][0]),
//            static_cast<unsigned short>(value.data[i][1]),
//            static_cast<unsigned short>(value.data[i][2]),
//            static_cast<unsigned short>(value.data[i][3]),
//            static_cast<unsigned short>(value.data[i][4]),
//            static_cast<unsigned short>(value.data[i][5]),
//            static_cast<unsigned short>(value.data[i][6]),
//            static_cast<unsigned short>(value.data[i][7]),
//            static_cast<unsigned short>(value.data[i][8]),
//            static_cast<unsigned short>(value.data[i][9]),
//            static_cast<unsigned short>(value.data[i][10]),
//            static_cast<unsigned short>(i) });
//    s.release();
//}
