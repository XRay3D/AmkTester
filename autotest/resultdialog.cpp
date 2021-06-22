#include "resultdialog.h"
#include "devices/resistancematrix.h"
#include "ui_resultdialog.h"

#include <QShortcut>

ResultDialog::ResultDialog(const ResistanceMatrix& ref, const ResistanceMatrix& result, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ResultDialog) //
{
    ui->setupUi(this);
    ui->tvRef->setPins(ref);
    reinterpret_cast<ResistanceModel*>(ui->tvRef->model())->setEditable(true);
    ui->tvResult->setPins(result);
    connect(ui->tvRef->model(), &QAbstractItemModel::dataChanged, [&ref, this] {
        const_cast<ResistanceMatrix&>(ref) = reinterpret_cast<ResistanceModel*>(ui->tvRef->model())->pins();
    });
    auto shortcut = new QShortcut(QKeySequence::Delete, this);
    connect(shortcut, &QShortcut::activated, [this] {
        for(auto&& index : ui->tvRef->selectionModel()->selectedIndexes())
            ui->tvRef->model()->setData(index, -1);
    });
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
