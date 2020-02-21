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

private slots:
    void on_pbPing_clicked();

private:
    void writeSettings();
    void readSettings();
    void message(const QString&);

    void switchSlot();

    QVector<QLineEdit*> le;
    QVector<QPushButton*> pb;

    PointEdit::Point m_points[2][12];
    enum { Json };
    const int saveFormat = Json;
    QJsonArray jsonArray;
    int lastIndex{};

    void cbxTypeIndexChanged(int index);
    void loadSettings();
    void saveSettings();
    //    void setupUi();
    //    void switchSlot();
    bool eventFilter(QObject* obj, QEvent* event);
};

#endif // MAINWINDOW_H
