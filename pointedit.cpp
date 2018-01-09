#include "pointedit.h"

#include <QDebug>

PointEdit::PointEdit(Point* point, QLineEdit* lineEdit, QWidget* parent)
    : QDialog(parent)
    , lineEdit(lineEdit)
    , point(point)
    , flag(false)

{
    setupUi(this);
    lastParcVal = point->Parcel.toInt();
    sbxParcel->setValue(point->Parcel.toInt());
    leDescription->setText(point->Description);
    connect(pbYes, &QPushButton::clicked, this, &PointEdit::Yes);
}

PointEdit::~PointEdit()
{
}

void PointEdit::setupUi(QWidget* PointEdit)
{
    if (PointEdit->objectName().isEmpty())
        PointEdit->setObjectName(QStringLiteral("PointEdit"));

    PointEdit->resize(255, 90);
    PointEdit->setMinimumSize(QSize(255, 90));
    PointEdit->setMaximumSize(QSize(255, 90));

    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(PointEdit->sizePolicy().hasHeightForWidth());
    PointEdit->setSizePolicy(sizePolicy);

    lblParcel = new QLabel(PointEdit);
    lblParcel->setObjectName(QStringLiteral("label_Parcel"));
    lblParcel->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    lblDescription = new QLabel(PointEdit);
    lblDescription->setObjectName(QStringLiteral("label_Description"));
    lblDescription->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    pbClean = new QPushButton(PointEdit);
    pbClean->setObjectName(QStringLiteral("pushButton_Clean"));

    pbYes = new QPushButton(PointEdit);
    pbYes->setObjectName(QStringLiteral("pushButton_Yes"));

    pbNo = new QPushButton(PointEdit);
    pbNo->setObjectName(QStringLiteral("pushButton_No"));

    leDescription = new QLineEdit(PointEdit);
    leDescription->setObjectName(QStringLiteral("lineEdit_Description"));

    line = new QFrame(PointEdit);
    line->setObjectName(QStringLiteral("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    sbxParcel = new QSpinBox(PointEdit);
    sbxParcel->setObjectName(QStringLiteral("spinBox_Parcel"));
    sbxParcel->setButtonSymbols(QAbstractSpinBox::NoButtons);
    sbxParcel->setMaximum(0xFFFF);

    gridLayout = new QGridLayout(PointEdit);
    gridLayout->setSpacing(4);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));
    gridLayout->setContentsMargins(6, 6, 6, 6);
    gridLayout->addWidget(lblParcel, 0, 0, 1, 1);
    gridLayout->addWidget(lblDescription, 1, 0, 1, 1);
    gridLayout->addWidget(pbClean, 3, 1, 1, 1);
    gridLayout->addWidget(pbYes, 3, 0, 1, 1);
    gridLayout->addWidget(pbNo, 3, 2, 1, 1);
    gridLayout->addWidget(leDescription, 1, 1, 1, 2);
    gridLayout->addWidget(line, 2, 0, 1, 3);
    gridLayout->addWidget(sbxParcel, 0, 1, 1, 2);

    QWidget::setTabOrder(pbNo, sbxParcel);
    QWidget::setTabOrder(sbxParcel, leDescription);
    QWidget::setTabOrder(pbYes, pbClean);

    retranslateUi(PointEdit);

    QObject::connect(pbClean, &QPushButton::clicked, leDescription, &QLineEdit::clear);
    QObject::connect(pbClean, &QPushButton::clicked, sbxParcel, &QSpinBox::clear);
    QObject::connect(pbNo, &QPushButton::clicked, PointEdit, &QWidget::hide);
    typedef void (QPushButton::*func)();
    QObject::connect(leDescription, &QLineEdit::returnPressed, pbYes, static_cast<func>(&QPushButton::setFocus));

    QMetaObject::connectSlotsByName(PointEdit);
}

void PointEdit::retranslateUi(QWidget* PointEdit)
{
    PointEdit->setWindowTitle("\320\240\320\265\320\264\320\260\320\272\321\202\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265 \321\202\320\276\321\207\320\272\320\270");
    lblParcel->setText("\320\237\320\276\321\201\321\213\320\273\320\272\320\260:");
    lblDescription->setText("\320\236\320\277\320\270\321\201\320\260\320\275\320\270\320\265:");
    pbClean->setText("\320\236\321\202\321\207\320\270\321\201\321\202\320\270\321\202\321\214");
    pbYes->setText("\320\224\320\260");
    pbNo->setText("\320\235\320\265\321\202");
}

void PointEdit::Yes()
{
    if (leDescription->text().isEmpty()) {
        leDescription->setText(QString().setNum(sbxParcel->value()));
    }
    lineEdit->setText(leDescription->text());
    point->Description = leDescription->text();
    point->Parcel = QString().setNum(sbxParcel->value());
    hide();
}

void PointEdit::on_spinBox_Parcel_valueChanged(int arg1)
{
    if (leDescription->text().isEmpty() || leDescription->text().toInt() == lastParcVal)
        leDescription->setText(QString().setNum(arg1));
    lastParcVal = arg1;
}
