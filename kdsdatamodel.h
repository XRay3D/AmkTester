#pragma once

#include <QAbstractTableModel>

class Kds;

class KdsDataModel : public QAbstractTableModel {
    friend class KdsDialog;

    static constexpr int rows = 5;
    int cols = 16;
    float m_answerData[5][16] = {};
    Kds* const kds;

public:
    KdsDataModel(Kds* kds, int cols, QObject* parent);
    ~KdsDataModel();

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex&) const override;

    void setColumnCount(int columnCount);
};
