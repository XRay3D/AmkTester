#pragma once

#include "pointedit.h"

#include <QJsonArray>
#include <QWidget>

class RelaySetHolder;
class RelaySet : public QWidget {
    Q_OBJECT

public:
    explicit RelaySet(QWidget* parent = nullptr);
    bool eventFilter(QObject* obj, QEvent* event) override;

    void setSets(RelaySetHolder* newSets);
    void cbxTypeIndexChanged(int index);
    void setupUi(int fl);
    const Point& currentPoint() const;

signals:
    void showMessage(const QString&, int = {});

private:
    enum { SetCount = 12 };
    Point m_points[SetCount];
    QVector<QLineEdit*> leDescription;
    QVector<QPushButton*> buttons;
    //    QComboBox* cbxAmkSet;
    RelaySetHolder* sets;
    bool setsChanged {};
    int lastIndex {};

    void switchSlot();
};
