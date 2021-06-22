#include "autotestmodel.h"

#include <QFile>
#include <QMimeData>

static const QString mimeType {QStringLiteral("application/AutoTestModelData")};

QDataStream& operator<<(QDataStream& stream, const std::vector<ModelData>& vector) {
    int size {static_cast<int>(vector.size())};
    stream << size;
    for(auto& data : vector)
        stream << data;
    return stream;
}
QDataStream& operator>>(QDataStream& stream, std::vector<ModelData>& vector) {
    int size {};
    stream >> size;
    vector.resize(size);
    for(auto& data : vector)
        stream >> data;
    return stream;
}

AutoTestModel::AutoTestModel(QObject* parent)
    : QAbstractTableModel(parent) //
{
}

int AutoTestModel::rowCount(const QModelIndex&) const { return static_cast<int>(m_data.size()); }

int AutoTestModel::columnCount(const QModelIndex&) const { return ColumnCount; }

QVariant AutoTestModel::data(const QModelIndex& index, int role) const {
    auto& data = m_data[index.row()];
    if(role == Qt::DisplayRole)
        switch(index.column()) {
        case Description1:
            return data.setPoint1.Description;
        case Description2:
            return data.setPoint2.Description;
        case UserActivity:
            return data.userActivity;
        default:
            return {};
        }
    else if(role == Qt::EditRole && index.column() == UserActivity)
        return data.userActivity;
    else if(role == Qt::DecorationRole)
        switch(index.column()) {
        case TestResult:
            return data.measured.toIcon({33, 33});
        case Pattern:
            return data.pattern.toIcon({33, 33});
        default:
            return {};
        }
    else if(role == Qt::BackgroundColorRole && index.column() == TestResult)
        return data.ok ? QColor(127, 255, 127) : QColor(255, 127, 127);
    else if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return {};
}

bool AutoTestModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    auto& data = m_data[index.row()];
    if(role == Qt::EditRole && index.column() == UserActivity) {
        data.userActivity = value.toString();
        return true;
    }
    return {};
}

QVariant AutoTestModel::headerData(int section, Qt::Orientation orientation, int role) const {
    static const auto hd = {
        "Описание 1",
        "Описание 2",
        "Ручные\nдействия",
        "Оп.",
        "Изм.",
    };
    static const auto hdtt = {
        "Описание 1",
        "Описание 2",
        "Ручные действия",
        "Опорныне значения",
        "Измеренные значения",
    };

    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal)
            return hd.begin()[section];
        else
            return QString("      %1").arg(++section);
    } else if(role == Qt::ToolTipRole) {
        if(orientation == Qt::Horizontal)
            return hdtt.begin()[section];
        else
            return QString("%1").arg(++section);
    } else if(role == Qt::TextAlignmentRole) {
        if(orientation == Qt::Horizontal)
            return Qt::AlignCenter;
        else
            return {Qt::AlignVCenter | Qt::AlignRight};
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags AutoTestModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;

    flags |= Qt::ItemIsDragEnabled;
    if(!index.isValid())
        flags |= Qt::ItemIsDropEnabled;

    if(index.column() == UserActivity)
        flags |= Qt::ItemIsEditable;
    // else if(index.column() == Pattern)
    flags |= Qt::ItemIsSelectable;

    return flags;
}

bool AutoTestModel::insertRows(int row, int count, const QModelIndex& /*parent*/) {
    qDebug(__FUNCTION__);
    qDebug() << row << count;
    if(row < 0)
        return inserted = {};
    beginInsertRows({}, row, row + count - 1);
    m_data.insert(m_data.begin() + row, *(m_data.begin() + *moveRows.begin()));
    endInsertRows();
    return inserted = true;
}

bool AutoTestModel::removeRows(int row, int count, const QModelIndex& /*parent*/) {
    qDebug(__FUNCTION__);
    if(!inserted)
        return {};
    inserted = false;
    beginRemoveRows({}, row, row + count - 1);
    if(count == 1)
        m_data.erase(m_data.begin() + row);
    else
        m_data.erase(m_data.begin() + row, m_data.begin() + row + count - 1);
    endRemoveRows();
    return true;
}

QMimeData* AutoTestModel::mimeData(const QModelIndexList& indexes) const {
    qDebug(__FUNCTION__);
    qDebug() << indexes;
    moveRows.clear();
    for(auto&& index : indexes)
        moveRows.emplace(index.row());
    return QAbstractTableModel::mimeData(indexes);
}

Qt::DropActions AutoTestModel::supportedDropActions() const { return Qt::MoveAction /*| Qt::TargetMoveAction*/; }

void AutoTestModel::appendTest(const ResistanceMatrix& pattern, const Point& setPoint1, const Point& setPoint2) {
    beginInsertRows({}, static_cast<int>(m_data.size()), static_cast<int>(m_data.size()));
    m_data.emplace_back(pattern, ResistanceMatrix {}, setPoint1, setPoint2);
    endInsertRows();
}

void AutoTestModel::setPattern(const QModelIndex& index, const ResistanceMatrix& pattern) {
    m_data[index.row()].pattern = pattern;
    emit dataChanged(index, index, {Qt::DecorationRole});
}

Point& AutoTestModel::point(const QModelIndex& index) {
    return !index.column() ? m_data[index.row()].setPoint1 : m_data[index.row()].setPoint2;
}

std::vector<ModelData>& AutoTestModel::data() { return m_data; }

void AutoTestModel::save(const QString name) {
    QFile file(name);
    if(file.open(QFile::WriteOnly)) {
        QDataStream stream(&file);
        stream << m_data;
    } else {
        qDebug() << file.errorString();
    }
}

void AutoTestModel::load(const QString name) {
    QFile file(name);
    if(file.open(QFile::ReadOnly)) {
        clear();
        QDataStream stream(&file);
        stream >> m_data;
        if(m_data.size()) {
            beginInsertRows({}, 0, m_data.size() - 1);
            endInsertRows();
        }
    } else {
        qDebug() << file.errorString();
    }
}

void AutoTestModel::clear() {
    if(m_data.size()) {
        beginRemoveRows({}, 0, m_data.size() - 1);
        m_data.clear();
        endRemoveRows();
    }
}

void AutoTestModel::onChecked(int index) {
    emit dataChanged(createIndex(index, 0), createIndex(index, ColumnCount - 1), {Qt::DisplayRole});
}
