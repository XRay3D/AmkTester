#ifndef TESTER_H
#define TESTER_H

#include "mytablemodel.h"

#include <QSemaphore>
#include <QTableView>
#include <QTimer>
#include <QWidget>

namespace Ui {
class Tester;
}

class QPushButton;
class MyTableWidget;

class TESTER : public QWidget {
    Q_OBJECT

public:
    explicit TESTER(QWidget* parent = nullptr);
    ~TESTER() override;

signals:
    void MeasurePin(int pin);

private:
    Ui::Tester* ui;
    QTimer timer;

    QPushButton* pbStartStop;
    QTableView* tableView;
    MyTableModel* model;
    QSemaphore s;

    void setupUi(QWidget* Form);
    void retranslateUi(QWidget* Form);
    void SetValue(const QVector<quint16>& value);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent* event) override;
};

#endif // TESTER_H
