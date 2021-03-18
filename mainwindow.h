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
    void on_pbAutoMeasure_clicked(bool checked);

private:
    PointEdit::Point m_points[2][12];
    QJsonArray jsonArray;
    QVector<QLineEdit*> le;
    QVector<QPushButton*> pb;
    QSemaphore pinSemaphore;
    enum { Json };
    const int saveFormat = Json;

    int lastIndex {};
    int timerId {};

    bool eventFilter(QObject* obj, QEvent* event) override;

    void cbxTypeIndexChanged(int index);
    void loadSettings();
    void message(const QString&);
    void readSettings();
    void saveSettings();
    void switchSlot();
    void writeSettings();

    // QObject interface
protected:
    void timerEvent(QTimerEvent* event) override;
};

#endif // MAINWINDOW_H
