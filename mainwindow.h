#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pointedit.h"
#include <QJsonArray>
#include <QMainWindow>

class PinModel;
class AutoTestModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    //    void start(Model* model);
    //    void stop();
    //    void measurePins();

private:
    Ui::MainWindow* ui;
    enum { SetCount = 12 };
    Point m_points[2][SetCount];
    QJsonArray jsonArray;
    QVector<QLineEdit*> leDescription;
    QVector<QPushButton*> pb;

    PinModel* pinModel;
    AutoTestModel* testModel;
    QAction* actionTest;

    QToolBar* toolBarAutomatic;

    QString testFileNane;

    void setupTvPins();
    void setupTvAuto();

    int lastIndex{};

    bool eventFilter(QObject* obj, QEvent* event) override;

    void cbxTypeIndexChanged(int index);
    void onActionTestTriggered(bool checked = false);
    void finished() { onActionTestTriggered(); }
    void updatePorts();
    void ping();

    void loadSets();
    void saveSets();

    void readSettings();
    void writeSettings();

    void message(const QString&);
    void switchSlot();

    // QMainWindow interface
public:
    QMenu* createPopupMenu() override;
};

#endif // MAINWINDOW_H
