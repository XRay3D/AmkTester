#include "resultdialog.h"
#include "ui_resultdialog.h"

ResultDialog::ResultDialog(const ResistanceMatrix& ref, const ResistanceMatrix& result, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ResultDialog) //
{
    ui->setupUi(this);
    ui->tvRef->setPins(ref);
    ui->tvResult->setPins(result);
}

ResultDialog::~ResultDialog() { delete ui; }

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
