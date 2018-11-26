#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include <QAbstractTableModel>

class MyTableModel : public QAbstractTableModel {
    Q_OBJECT

    int m_data[11][11];

public:
    explicit MyTableModel(QObject* parent = nullptr);
    enum {
        ColumnCount = 11,
        RowCount = 11,
    };

signals:

public slots:
    void setData(const QVector<quint16>& value);

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
};

#endif // MYTABLEMODEL_H
