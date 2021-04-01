#include "resistanceview.h"
#include <QDebug>
#include <QHeaderView>

ResistanceView::ResistanceView(QWidget* parent)
    : QTableView(parent) //
{
    auto model = new ResistanceModel(this);
    setModel(model);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ResistanceView::setPins(const Pins& pins) {
    static_cast<ResistanceModel*>(model())->setPins(pins);
}

Pins ResistanceView::pins() const {
    return static_cast<ResistanceModel*>(model())->pins();
}

void ResistanceView::showEvent(QShowEvent* event) {
    QTableView::showEvent(event);
    if(minimumSize().isNull()) {
        int size = verticalHeader()->width() + 20;
        for(int col = 0; col < Pins::Count; ++col)
            size += columnWidth(col);
        qDebug() << __FUNCTION__ << size;
        setMinimumSize({size, size});
    }
}

/////////////////////////////////////////////////////////////////
/// \brief ResistanceModel::ResistanceModel
/// \param parent
///
ResistanceModel::ResistanceModel(QObject* parent)
    : QAbstractTableModel(parent) {
}

void ResistanceModel::setPins(const Pins& pins) {
    m_data = pins;
    dataChanged(createIndex(0, 0), createIndex(RowCount - 1, ColumnCount - 1), {Qt::DisplayRole});
}

int ResistanceModel::rowCount(const QModelIndex&) const { return RowCount; }

int ResistanceModel::columnCount(const QModelIndex&) const { return ColumnCount; }

QVariant ResistanceModel::data(const QModelIndex& index, int role) const {
    switch(role) {
    case Qt::DisplayRole:
        if(m_data[index.row()][index.column()] > -1)
            return m_data[index.row()][index.column()];
        return QVariant();
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::BackgroundColorRole:
        if(m_data[index.row()][index.column()] > -1)
            return QColor(100, 255, 100);
        else if(index.row() < 6 && index.column() < 6)
            return QColor(220, 255, 220);
        else if(index.row() > 5 && index.column() > 5)
            return QColor(255, 220, 220);
        else
            return QColor(255, 255, 220);
    default:
        return QVariant();
    }
}

QVariant ResistanceModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
    const QStringList header{"K1", "K2", "K3", "K4", "-U", "+U", "+I", "-I", "mV", "V", "-V"};
    if(role == Qt::DisplayRole)
        return header.at(section);
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return QVariant();
}

Qt::ItemFlags ResistanceModel::flags(const QModelIndex&) const { return Qt::ItemIsEnabled; }
