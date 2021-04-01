#include "autotestmodel.h"

#include <QFile>

QDataStream& operator<<(QDataStream& stream, const std::vector<ModelData>& vector) {
    int size{static_cast<int>(vector.size())};
    stream << size;
    for(auto& data : vector)
        stream << data;
    return stream;
}
QDataStream& operator>>(QDataStream& stream, std::vector<ModelData>& vector) {
    int size{};
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
        "Description1",
        "Description2",
        "UserActivity",
        "Ref",
        "Test",
    };

    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal)
            return hd.begin()[section];
        else
            return QString("      %1").arg(++section);
    } else if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags AutoTestModel::flags(const QModelIndex& index) const {
    if(index.column() == UserActivity)
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    else if(index.column() == Pattern)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return Qt::ItemIsEnabled;
}

bool AutoTestModel::removeRows(int row, int, const QModelIndex&) {
    beginRemoveRows({}, row, row);
    m_data.erase(m_data.begin() + row);
    endRemoveRows();
    return true;
}

void AutoTestModel::appendTest(const Pins& pattern, const Point& setPoint1, const Point& setPoint2) {
    beginInsertRows({}, static_cast<int>(m_data.size()), static_cast<int>(m_data.size()));
    m_data.emplace_back(pattern, Pins{}, setPoint1, setPoint2);
    endInsertRows();
}

void AutoTestModel::setPattern(const QModelIndex& index, const Pins& pattern) {
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
