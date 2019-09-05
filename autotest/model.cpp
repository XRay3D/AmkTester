#include "model.h"

#include <QColor>
#include <QFile>

const int id2 = qRegisterMetaType<QVector<int>>("QVector<int>");

Model::Model(QObject* parent, const QVector<bool>* hChecked, const QVector<bool>* vChecked)
    : QAbstractTableModel(parent)
    , m_hChecked(hChecked)
    , m_vChecked(vChecked)
{
    QFile file("Data.data");
    if (file.open(QFile::ReadOnly)) {
        QDataStream in(&file);
        in >> m_row;
        in >> m_column;
        in >> m_data;
        in >> m_verticalHeader;
        //        if (m_hChecked)
        //            in >> (*const_cast<QVector<bool>*>(m_hChecked));
        //        if (m_hChecked)
        //            in >> (*const_cast<QVector<bool>*>(m_hChecked));
    }
}

Model::~Model()
{
    QFile file("Data.data");
    if (file.open(QFile::WriteOnly)) {
        QDataStream out(&file);
        out << m_row;
        out << m_column;
        out << m_data;
        out << m_verticalHeader;
        //        if (m_hChecked)
        //            out << *m_hChecked;
        //        if (m_hChecked)
        //            out << *m_hChecked;
    }
}

int Model::rowCount(const QModelIndex& /*parent*/) const { return m_row; }

int Model::columnCount(const QModelIndex& /*parent*/) const { return m_column; }

QVariant Model::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return static_cast<bool>(m_data[index.row()][index.column()].data);
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::BackgroundColorRole:
        return m_data[index.row()][index.column()].data ? QColor(220, 255, 220) : QColor(255, 220, 220);
    default:
        return QVariant();
    }
}

bool Model::setData(const QModelIndex& index, const QVariant& value, int role)
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        m_data[index.row()][index.column()].data = value.toInt();
        return true;
    default:
        return false;
    }
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{

    switch (role) {
    case Qt::DisplayRole:
        return QString(orientation == Qt::Horizontal
                ? "Ch " + QString::number(++section)
                : (m_vChecked
                          ? "       " + m_verticalHeader.at(section)
                          : m_verticalHeader.at(section))); // spase to checkbox
    case Qt::TextAlignmentRole:
        return orientation == Qt::Horizontal
            ? static_cast<int>(Qt::AlignCenter)
            : static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags Model::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEditable;
    bool enabled = false;
    if (m_hChecked && m_vChecked)
        enabled = m_hChecked->value(index.column()) & m_vChecked->value(index.row());
    else if (m_hChecked)
        enabled = m_hChecked->value(index.column());
    else if (m_vChecked)
        enabled = m_vChecked->value(index.row());
    else
        enabled = true;
    return flags | (enabled ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags);
}

void Model::onChecked(int index, int orientation)
{
    if (orientation == Qt::Horizontal)
        emit dataChanged(createIndex(0, index), createIndex(m_row - 1, index), { Qt::DisplayRole });
    else
        emit dataChanged(createIndex(index, 0), createIndex(index, m_column - 1), { Qt::DisplayRole });
}

void Model::setRowCount(int row, const QString& name)
{
    if (m_row < row && insertRows(m_row, row - 1)) {
        m_row = row;
        m_verticalHeader.append(name);
    } else if (m_row > row && removeRows(row, m_row - 1)) {
        m_row = row;
    }
}

void Model::setColumnCount(int column)
{
    if (m_column < column && insertColumns(m_column, column - 1))
        m_column = column;
    else if (m_column > column && removeColumns(column, m_column - 1))
        m_column = column;
}

bool Model::insertRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(parent, row, count);
    m_data.resize(count + 1);
    for (QVector<Data>& data : m_data)
        data.resize(m_column);
    endInsertRows();
    return true;
}

bool Model::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, count);
    m_data.resize(row);
    for (QVector<Data>& data : m_data)
        data.resize(m_column);
    endRemoveRows();
    return true;
}

bool Model::insertColumns(int column, int count, const QModelIndex& parent)
{
    beginInsertColumns(parent, column, count);
    for (QVector<Data>& data : m_data)
        data.resize(count + 1);
    endInsertColumns();
    return true;
}

bool Model::removeColumns(int column, int count, const QModelIndex& parent)
{
    beginRemoveColumns(parent, column, count);
    for (QVector<Data>& data : m_data)
        data.resize(column);
    endRemoveColumns();
    return true;
}

void Model::test(int row, int col, const PinsValue& value)
{
    m_data[row][col].test(value);
    emit dataChanged(createIndex(row, col), createIndex(row, col), { Qt::DisplayRole });
}
