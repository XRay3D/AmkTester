#ifndef TESTER_H
#define TESTER_H

#include "mytablemodel.h"

#include <QTimer>
#include <QWidget>
#include <QSemaphore>
#include <QTableView>

namespace Ui {
class Tester;
}

class QPushButton;
class MyTableWidget;

class TESTER : public QWidget {
    Q_OBJECT

public:
    explicit TESTER(QWidget* parent = 0);
    ~TESTER();

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

    //    void MeasurePin();
    //    int m_pin = 0;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // TESTER_H
