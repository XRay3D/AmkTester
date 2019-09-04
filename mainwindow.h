#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbPing_clicked();

private:
    void writeSettings();
    void readSettings();

    QTimer timer;
    QSemaphore s;
};

#endif // MAINWINDOW_H
