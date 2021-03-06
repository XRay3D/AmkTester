#pragma once

#include <QAbstractTableModel>
#include <qglobal.h>
#include <set>

#include "devices/tester.h"
#include "pointedit.h"

struct ModelData {
    ResistanceMatrix pattern;
    ResistanceMatrix measured;
    Point setPoint1;
    Point setPoint2;
    QString userActivity;
    bool ok = false;
    bool use = true;

    friend QDataStream& operator<<(QDataStream& stream, const ModelData& d) {
        stream << d.pattern;
        stream << d.measured;
        stream << d.setPoint1;
        stream << d.setPoint2;
        stream << d.userActivity;
        stream << d.ok;
        stream << d.use;
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, ModelData& d) {
        stream >> d.pattern;
        stream >> d.measured;
        stream >> d.setPoint1;
        stream >> d.setPoint2;
        stream >> d.userActivity;
        stream >> d.ok;
        stream >> d.use;
        return stream;
    }
};

class AutoTestModel : public QAbstractTableModel {
    Q_OBJECT

    std::vector<ModelData> m_data;
    mutable std::set<int> moveRows;
    bool inserted {};

signals:

public:
    explicit AutoTestModel(QObject* parent);

    enum {
        Description1,
        Description2,
        UserActivity,
        Pattern,
        TestResult,
        ColumnCount,
    };

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex& = {}) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

    // Drag & Drop
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    Qt::DropActions supportedDropActions() const override;

    Point& point(const QModelIndex& index);
    void appendTest(const ResistanceMatrix& pattern, const Point& setPoint1, const Point& setPoint2);
    void setPattern(const QModelIndex& index, const ResistanceMatrix& pattern);
    std::vector<ModelData>& data();

    void save(const QString name);
    void load(const QString name);
    void clear();

    void onChecked(int index);
};
