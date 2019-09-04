#include "mytablemodel.h"
#include <QColor>
#include <QDebug>
#include <QMutex>

MyTableModel::MyTableModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_data{
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    }
{
}

void MyTableModel::setRawData(const QVector<quint16>& value)
{
    //static QMutex mutex;
    //QMutexLocker mutexLocker(&mutex);
    //qDebug() << value;
    const int row = value[ColumnCount];
    int rez[ColumnCount]{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    int count = 0;
    for (
        int column = row - 1, index = row + 1;
        column > -1 || index < ColumnCount;
        --column, ++index) {

        float value1 = value[row];
        if (column > -1 && value[column] > 10) { // 10 this anti-noise
            float value2 = value[column];
            ++count;
            if (value1 > value2)
                rez[column] = static_cast<int>(((value1 - value2) / value2) * 1000); // 1000 опорное сопротивление
            else
                rez[column] = static_cast<int>(((value2 - value1) / value1) * 1000); // 1000 опорное сопротивление

            if (rez[row] < 0)
                ++rez[row];
        }
        if (index < 11 && value[index] > 10) { // 10 this anti-noise
            float value2 = value[index];
            ++count;
            if (value1 > value2)
                rez[index] = static_cast<int>(((value1 - value2) / value2) * 1000); // 1000 опорное сопротивление
            else
                rez[index] = static_cast<int>(((value2 - value1) / value1) * 1000); // 1000 опорное сопротивление

            if (rez[row] < 0)
                ++rez[row];
        }
    }

    for (int column = 0; column < ColumnCount; ++column) {
        if (count && rez[column] > 0 && column < row)
            rez[column] /= count;
        m_data[row][column] = rez[column];
    }

    dataChanged(createIndex(row, 0), createIndex(row, 10), { Qt::DisplayRole });
}

int MyTableModel::rowCount(const QModelIndex& /*parent*/) const
{
    return RowCount;
}

int MyTableModel::columnCount(const QModelIndex& /*parent*/) const
{
    return ColumnCount;
}

QVariant MyTableModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        if (m_data[index.row()][index.column()] > -1)
            return m_data[index.row()][index.column()];
        return QVariant();
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::BackgroundColorRole:
        if (m_data[index.row()][index.column()] > -1)
            return QColor(100, 255, 100);
        else if (index.row() < 6 && index.column() < 6)
            return QColor(220, 255, 220);
        else if (index.row() > 5 && index.column() > 5)
            return QColor(255, 220, 220);
        else
            return QColor(255, 255, 220);
    default:
        return QVariant();
    }
}

QVariant MyTableModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    const QStringList header({ "K1", "K2", "K3", "K4", "-U", "+U", "+I", "-I", "mV", "V", "-V" });
    if (role == Qt::DisplayRole)
        return header.at(section);
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return QVariant();
}

Qt::ItemFlags MyTableModel::flags(const QModelIndex& /*index*/) const
{
    return Qt::ItemIsEnabled;
}
