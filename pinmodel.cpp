#include "pinmodel.h"
#include <QColor>
#include <QDebug>
#include <QMutex>

PinModel::PinModel(QObject* parent)
    : QAbstractTableModel(parent) {
}

void PinModel::setPins(const Pins& value) {
    m_data = value;
    dataChanged(createIndex(0, 0), createIndex(RowCount - 1, ColumnCount - 1), {Qt::DisplayRole});
}

int PinModel::rowCount(const QModelIndex&) const { return RowCount; }

int PinModel::columnCount(const QModelIndex&) const { return ColumnCount; }

QVariant PinModel::data(const QModelIndex& index, int role) const {
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

QVariant PinModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
    const QStringList header{"K1", "K2", "K3", "K4", "-U", "+U", "+I", "-I", "mV", "V", "-V"};
    if(role == Qt::DisplayRole)
        return header.at(section);
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return QVariant();
}

Qt::ItemFlags PinModel::flags(const QModelIndex& /*index*/) const {
    return Qt::ItemIsEnabled;
}
