#include "kdsdatamodel.h"
#include "devices/devices.h"

KdsDataModel::KdsDataModel(Kds* kds, int cols, QObject* parent)
    : QAbstractTableModel(parent)
    , cols(cols)
    , kds(kds) {
}

KdsDataModel::~KdsDataModel() { }

int KdsDataModel::rowCount(const QModelIndex&) const { return rows; }

int KdsDataModel::columnCount(const QModelIndex&) const { return cols; }

QVariant KdsDataModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole || role == Qt::EditRole)
        return static_cast<double>(m_answerData[index.row()][index.column()]);
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return {};
}

bool KdsDataModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(role == Qt::EditRole) {
        if(m_answerData[index.row()][index.column()] != value.toFloat()
           && kds->fileOpen()
           && kds->fileSeek(index.column() * 5 * 4 + index.row() * 4)
           && kds->fileWrite(value.toFloat())) {
            m_answerData[index.row()][index.column()] = value.toFloat();
            emit dataChanged(index, index, {role});
            return true;
        }
    } else if(role == Qt::DisplayRole) {
        m_answerData[index.row()][index.column()] = value.toFloat();
        emit dataChanged(index, index, {role});
        return true;
    }
    return {};
}

QVariant KdsDataModel::headerData(int section, Qt::Orientation orientation, int role) const {
    static auto hd = {
        "0 Ом",
        "40 Ом",
        "80 Ом",
        "160 Ом",
        "320 Ом",
    };
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal)
            return ++section;
        else
            return *(hd.begin() + section);
    } else if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return {};
}

Qt::ItemFlags KdsDataModel::flags(const QModelIndex&) const {
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void KdsDataModel::setColumnCount(int columnCount) {
    if(columnCount < 1 || columnCount == cols) {
        return;
    } else if(columnCount > cols) {
        beginInsertColumns({}, cols, columnCount - 1);
        cols = columnCount;
        endInsertColumns();
    } else if(columnCount < cols) {
        beginRemoveColumns({}, columnCount, cols - 1);
        cols = columnCount;
        endRemoveColumns();
    }
}
