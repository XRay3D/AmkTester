#pragma once

#include <QTableView>
#include <devices/pins.h>

class ResistanceView : public QTableView {
    Q_OBJECT
public:
    explicit ResistanceView(QWidget* parent = nullptr);
    void setPins(const Pins& pins);
    Pins pins() const;

    void showEvent(QShowEvent* event);
};

class ResistanceModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit ResistanceModel(QObject* parent = nullptr);
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

    void setPins(const Pins& pins);
    Pins pins() const { return m_data; }

private:
    Pins m_data;
};
