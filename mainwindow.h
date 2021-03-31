#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QJsonArray>

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void start(Model* model);
    void stop();
    void measurePins();

private slots:
    void on_pbPing_clicked();

private:
    enum { SetCount = 12 };
    Point m_points[2][SetCount];
    QJsonArray jsonArray;
    QVector<QLineEdit*> le;
    QVector<QPushButton*> pb;

    int lastIndex{};

    bool eventFilter(QObject* obj, QEvent* event) override;

    void cbxTypeIndexChanged(int index);

    void loadSets();
    void saveSets();

    void message(const QString&);
    void readSettings();
    void switchSlot();
    void writeSettings();
};

#endif // MAINWINDOW_H
