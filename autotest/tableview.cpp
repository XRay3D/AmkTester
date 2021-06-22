#include "tableview.h"
#include "autotestmodel.h"
#include "header.h"

#include <QBoxLayout>
#include <QCheckBox>

TableView::TableView(QWidget* parent)
    : QTableView(parent) {

    setSelectionBehavior(SelectRows);
    setSelectionMode(SingleSelection);

    setDragEnabled(true);
    setDropIndicatorShown(true);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDefaultDropAction(Qt::MoveAction);
    setDragDropMode(QTableView::InternalMove);
    setDragDropOverwriteMode(false);
}

TableView::~TableView() { }

AutoTestModel* TableView::initCheckBox() {
    auto model = new AutoTestModel(this);
    setModel(model);

    auto vh = new Header(Qt::Vertical, model->data(), this);
    setVerticalHeader(vh);
    createCornerCheckBox(vh);

    connect(vh, &Header::onChecked, model, &AutoTestModel::onChecked);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    return model;
}

void TableView::createCornerCheckBox(Header* header) {
    auto cornerButton = findChild<QAbstractButton*>();
    if(cornerButton) {
        auto vl = new QHBoxLayout(cornerButton);
        auto checkBox = new QCheckBox(cornerButton);
        vl->addWidget(checkBox);
        vl->setMargin(0);
        vl->insertSpacing(0, 6);
        checkBox->setFocusPolicy(Qt::NoFocus);
        connect(checkBox, &QCheckBox::clicked, [header](bool checked) { header->setAll(checked); });
        connect(header, &Header::onCheckedV, [checkBox](const QVector<bool>& v) {
            static const Qt::CheckState chState[] {
                Qt::Unchecked,
                Qt::Unchecked,
                Qt::Checked,
                Qt::PartiallyChecked};
            checkBox->setCheckState(chState[v.contains(true) * 2 | v.contains(false) * 1]);
        });
    }
}
