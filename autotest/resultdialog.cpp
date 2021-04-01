#include "resultdialog.h"
#include "ui_resultdialog.h"

#include <pinmodel.h>

ResultDialog::ResultDialog(const Pins& ref, const Pins& result, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ResultDialog) //
{
    ui->setupUi(this);

    auto setup = [](QTableView* tv, const Pins& pins) {
        auto model = new PinModel(tv);
        model->setPins(pins);
        tv->setModel(model);
        tv->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tv->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    };

    setup(ui->tvRef, ref);
    setup(ui->tvResult, result);
}

ResultDialog::~ResultDialog() {
    delete ui;
}

void ResultDialog::changeEvent(QEvent* e) {
    QDialog::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
