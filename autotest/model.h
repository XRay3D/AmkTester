#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include <QAbstractTableModel>
#include <QDataStream>

struct Data {
    Data() {}
    QString parcel1;
    QString parcel2;
    QString userMessage;
    int patern[11][11];
    bool check = false;
    int data = 0;

    friend QDataStream& operator<<(QDataStream& stream, const Data& d)
    {
        stream << d.parcel1;
        stream << d.parcel2;
        stream << d.userMessage;
        stream << d.check;
        stream << d.data;
        stream.writeRawData(reinterpret_cast<const char*>(&d.patern), sizeof(int) * 11 * 11);
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, Data& d)
    {
        stream >> d.parcel1;
        stream >> d.parcel2;
        stream >> d.userMessage;
        stream >> d.check;
        stream >> d.data;
        stream.readRawData(reinterpret_cast<char*>(&d.patern), sizeof(int) * 11 * 11);
        return stream;
    }
};

class Model : public QAbstractTableModel {
    Q_OBJECT
    friend class TableView;

public:
    Model(QObject* parent = nullptr, const QVector<bool>* hChecked = nullptr, const QVector<bool>* vChecked = nullptr);
    ~Model() override;
    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void onChecked(int index, int orientation);

    void setRowCount(int row, const QString& name = "");
    void setColumnCount(int column);

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;

private:
    QStringList m_verticalHeader;
    QVector<QVector<Data>> m_data;
    const QVector<bool>* m_hChecked;
    const QVector<bool>* m_vChecked;
    int m_row = 0;
    int m_column = 0;
};

#endif // MYTABLEMODEL_H
