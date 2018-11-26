#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    void writeSettings();
    void readSettings();
};

#endif // MAINWINDOW_H
