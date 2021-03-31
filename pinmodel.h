#ifndef PINMODEL_H
#define PINMODEL_H

#include "devices/tester.h"
#include <QAbstractTableModel>

class PinModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit PinModel(QObject* parent = nullptr);
    enum {
        ColumnCount = Pins::Count,
        RowCount = Pins::Count,
    };
    // QAbstractItemModel interface
    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

public slots:
    void setDataA(const Pins& value);

private:
    Pins m_data;
};

#endif // PINMODEL_H
