#ifndef TESTER_H
#define TESTER_H

#include <QSemaphore>
#include <QTableView>
#include <QTimer>
#include <QWidget>

namespace Ui {
class TesterData;
}

class MyTableModel;
class MyTableWidget;
class QPushButton;

class TesterData : public QWidget {
    Q_OBJECT

public:
    explicit TesterData(QWidget* parent = nullptr);
    ~TesterData() override;

    int** data() const;

signals:
    void MeasurePin(int pin);

private:
    Ui::TesterData* ui;
    QTimer timer;

    QPushButton* pbStartStop;
    QTableView* tableView;
    MyTableModel* m_model;
    QSemaphore s;

    void setupUi(QWidget* Form);
    void retranslateUi(QWidget* Form);
    void SetValue(const QVector<quint16>& value);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent* event) override;
};

#endif // TESTER_H
