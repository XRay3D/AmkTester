#include "pointedit.h"

#include <QDebug>
#include <QGridLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

PointEdit::PointEdit(Point& point, QLineEdit* lineEdit, QWidget* parent)
    : QDialog(parent)
    , lineEdit(lineEdit)
    , point(point)
    , flag(false)

{
    setupUi(this);
    lastParcVal = point.Parcel;
    sbxParcel->setValue(point.Parcel);
    leDescription->setText(point.Description);
    connect(pbYes, &QPushButton::clicked, this, &PointEdit::Yes);
}

PointEdit::~PointEdit() {
}

void PointEdit::setupUi(QWidget* pointEdit) {
    if(pointEdit->objectName().isEmpty())
        pointEdit->setObjectName(QStringLiteral("PointEdit"));

    pointEdit->resize(255, 90);
    pointEdit->setMinimumSize(QSize(255, 90));
    pointEdit->setMaximumSize(QSize(255, 90));

    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(pointEdit->sizePolicy().hasHeightForWidth());
    pointEdit->setSizePolicy(sizePolicy);

    lblParcel = new QLabel(pointEdit);
    lblParcel->setObjectName(QStringLiteral("label_Parcel"));
    lblParcel->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    lblDescription = new QLabel(pointEdit);
    lblDescription->setObjectName(QStringLiteral("label_Description"));
    lblDescription->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    pbClean = new QPushButton(pointEdit);
    pbClean->setObjectName(QStringLiteral("pushButton_Clean"));

    pbYes = new QPushButton(pointEdit);
    pbYes->setObjectName(QStringLiteral("pushButton_Yes"));

    pbNo = new QPushButton(pointEdit);
    pbNo->setObjectName(QStringLiteral("pushButton_No"));

    leDescription = new QLineEdit(pointEdit);
    leDescription->setObjectName(QStringLiteral("lineEdit_Description"));

    line = new QFrame(pointEdit);
    line->setObjectName(QStringLiteral("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    sbxParcel = new QSpinBox(pointEdit);
    sbxParcel->setObjectName(QStringLiteral("spinBox_Parcel"));
    sbxParcel->setButtonSymbols(QAbstractSpinBox::NoButtons);
    sbxParcel->setMaximum(0xFFFF);

    gridLayout = new QGridLayout(pointEdit);
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

    pointEdit->setWindowTitle("Редактирование точки");
    lblParcel->setText("Посылка:");
    lblDescription->setText("Описание:");
    pbClean->setText("Отчистить");
    pbYes->setText("Да");
    pbNo->setText("Нет");

    QObject::connect(pbClean, &QPushButton::clicked, leDescription, &QLineEdit::clear);
    QObject::connect(pbClean, &QPushButton::clicked, sbxParcel, &QSpinBox::clear);
    QObject::connect(pbNo, &QPushButton::clicked, pointEdit, &QWidget::hide);
    typedef void (QPushButton::*func)();
    QObject::connect(leDescription, &QLineEdit::returnPressed, pbYes, static_cast<func>(&QPushButton::setFocus));

    QMetaObject::connectSlotsByName(pointEdit);
}

void PointEdit::Yes() {
    if(leDescription->text().isEmpty()) {
        leDescription->setText(QString().setNum(sbxParcel->value()));
    }
    if(lineEdit)
        lineEdit->setText(leDescription->text());
    point.Description = leDescription->text();
    point.Parcel = sbxParcel->value();
    hide();
}

void PointEdit::on_spinBox_Parcel_valueChanged(int arg1) {
    if(leDescription->text().isEmpty() || leDescription->text().toUInt() == lastParcVal)
        leDescription->setText(QString().setNum(arg1));
    lastParcVal = arg1;
}

Point::Point(const QJsonObject& object)
    : Parcel(object["value"].toInt())
    , Description(object["name"].toString()) {
}
