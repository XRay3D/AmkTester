#include "tableview.h"
#include "header.h"

#include <QCheckBox>

TableView::TableView(QWidget* parent)
    : QTableView(parent)
{
}

TableView::~TableView() {}

Model* TableView::model() const { return m_model; }

void TableView::addRow(const QString& name)
{
    if (m_model->columnCount() <= 0)
        m_model->setColumnCount(1);
    m_model->setRowCount(m_model->rowCount() + 1, name);
}

void TableView::setPattern(int** pData, const PointEdit::Point& pt1, const PointEdit::Point& pt2)
{
    for (const QModelIndex& index : selectedIndexes()) {
        Data& data = m_model->m_data[index.row()][index.column()];
        data.parcel1 = pt1.Parcel;
        data.parcel2 = pt2.Parcel;
        memcpy(data.patern, pData, sizeof(int) * 11 * 11);
    }
}

void TableView::init()
{
    setModel(m_model = new Model(this));
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void TableView::initCheckBox()
{
    auto vh = new Header(Qt::Vertical, this);
    setVerticalHeader(vh);
    createCornerCheckBox(vh);

    setModel(m_model = new Model(this, nullptr, vh->checked()));
    connect(vh, &Header::onChecked, m_model, &Model::onChecked);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void TableView::initRadioButton()
{
    auto hh = new Header(Qt::Horizontal, this);
    setHorizontalHeader(hh);

    setModel(m_model = new Model(this, hh->checked()));
    connect(hh, &Header::onChecked, m_model, &Model::onChecked);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void TableView::initCheckBoxRadioButton()
{
    auto hh = new Header(Qt::Horizontal, this);
    auto vh = new Header(Qt::Vertical, this);

    setHorizontalHeader(hh);
    setVerticalHeader(vh);
    createCornerCheckBox(vh);

    setModel(m_model = new Model(this, hh->checked(), vh->checked()));
    connect(hh, &Header::onChecked, m_model, &Model::onChecked);
    connect(vh, &Header::onChecked, m_model, &Model::onChecked);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void TableView::createCornerCheckBox(Header* header)
{
    auto cornerButton = findChild<QAbstractButton*>();
    if (cornerButton) {
        auto checkBox = new QCheckBox(cornerButton);
        checkBox->setFocusPolicy(Qt::NoFocus);
        checkBox->setGeometry(Header::getRect(cornerButton->rect()).translated(1, -4));
        connect(checkBox, &QCheckBox::clicked, [header](bool checked) { header->setAll(checked); });
        connect(header, &Header::onCheckedV, [checkBox](const QVector<bool>& v, int) {
            static const Qt::CheckState chState[]{
                Qt::Unchecked,
                Qt::Unchecked,
                Qt::Checked,
                Qt::PartiallyChecked
            };
            checkBox->setCheckState(chState[v.contains(true) * 2 | v.contains(false) * 1]);
        });
    }
}
