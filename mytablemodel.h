#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include "hwinterface/tester.h"
#include <QAbstractTableModel>

class MyTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit MyTableModel(QObject* parent = nullptr);
    enum {
        ColumnCount = 11,
        RowCount = 11,
    };
    // QAbstractItemModel interface
    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

public slots:
    void setRawData(const QVector<quint16>& value);

private:
    int m_data[11][11];
};

#endif // MYTABLEMODEL_H
