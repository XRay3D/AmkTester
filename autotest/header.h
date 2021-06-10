#pragma once

#include "autotestmodel.h"

#include <QHeaderView>

class Header : public QHeaderView {
    Q_OBJECT
    std::vector<ModelData>& m_data;
    QVector<bool> m_checked;

public:
    Header(Qt::Orientation orientation, std::vector<ModelData>& m_data, QWidget* parent = nullptr);
    ~Header() override;

    enum {
        XOffset = 5,
        DelegateSize = 16
    };

    void setAll(bool checked);
    void togle(int index);
    void setSingle(int index);

    static QRect getRect(const QRect& rect);

signals:
    void onCheckedV(const QVector<bool>&);
    void onChecked(int);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override;

private:
    mutable QVector<QRect> m_checkRect;
    QPoint mousePos;
};
