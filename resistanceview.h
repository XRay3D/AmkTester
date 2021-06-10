#pragma once

#include <QTableView>
#include <devices/resistancematrix.h>

class ResistanceView : public QTableView {
    Q_OBJECT
public:
    explicit ResistanceView(QWidget* parent = nullptr);
    void setPins(const ResistanceMatrix& pins);
    ResistanceMatrix pins() const;

    void showEvent(QShowEvent* event);
};

class ResistanceModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit ResistanceModel(QObject* parent = nullptr);
    enum {
        ColumnCount = ResistanceMatrix::Size,
        RowCount = ResistanceMatrix::Size,
    };
    // QAbstractItemModel interface
    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void setPins(const ResistanceMatrix& pins);
    ResistanceMatrix pins() const { return m_data; }

private:
    ResistanceMatrix m_data;
};
