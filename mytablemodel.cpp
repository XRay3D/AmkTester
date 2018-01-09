#include "mytablemodel.h"
#include <QColor>
#include <QDebug>
#include <QMutex>

MyTableModel::MyTableModel(QObject* parent)
    : QAbstractTableModel(parent)
    , val(QVector<QVector<int> >(11, QVector<int>(11, -1)))
{
}

void MyTableModel::setValue(const QVector<quint16>& value)
{
    static QMutex mutex;
    QMutexLocker mutexLocker(&mutex);
    int num(value[ColumnCount]);
    QVector<int> rez(ColumnCount, -1);
    for (int l = num - 1, r = num + 1; l > -1 || r < ColumnCount; --l, ++r) {
        float u1 = value[num];
        if (l > -1 && value[l] > 10) {
            float u2 = value[l];
            if (u1 > u2)
                rez[l] = ((u1 - u2) / u2) * 1000;
            else
                rez[l] = ((u2 - u1) / u1) * 1000;
            if (rez[num] < 0)
                ++rez[num];
        }
        if (r < 11 && value[r] > 10) {
            float u2 = value[r];
            if (u1 > u2)
                rez[r] = ((u1 - u2) / u2) * 1000;
            else
                rez[r] = ((u2 - u1) / u1) * 1000;
            if (rez[num] < 0)
                ++rez[num];
        }
    }
    for (int i = 0; i < ColumnCount; ++i) {
        setData(createIndex(num, i, nullptr), rez[i]);
    }
    dataChanged(createIndex(num, 0, nullptr), createIndex(num, 10, nullptr));
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
        if (val[index.row()][index.column()] > -1)
            return val[index.row()][index.column()];
        else
            return "";
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::BackgroundColorRole:
        if (val[index.row()][index.column()] > -1)
            return QColor(100, 255, 100);
        else if (index.row() < 6 && index.column() < 6)
            return QColor(200, 255, 200);
        else if (index.row() > 5 && index.column() > 5)
            return QColor(255, 200, 200);
        else
            return QColor(255, 255, 200);
    default:
        return QVariant();
    }
}

QVariant MyTableModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    const QStringList header({ "K1", "K2", "K3", "K4", "+10V", "-10V", "+I", "-I", "+MV", "+V", "-MV/V" });
    if (role == Qt::DisplayRole)
        return header.at(section);
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return QVariant();
}

bool MyTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::EditRole) {
        val[index.row()][index.column()] = value.toInt();
    }
    return true;
}

Qt::ItemFlags MyTableModel::flags(const QModelIndex& /*index*/) const
{
    return Qt::ItemIsEnabled;
}
