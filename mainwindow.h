#pragma once

#include "relaysetholder.h"
#include <QMainWindow>

class AutoTestModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    // QMainWindow interface
    QMenu* createPopupMenu() override;

private:
    Ui::MainWindow* ui;

    AutoTestModel* testModel;
    QAction* actionTest;

    QToolBar* toolBarAutomatic;
    QToolBar* toolBarConnection;

    QString testFileNane;

    RelaySetHolder sets;

    void setupTvAuto();

    void onActionTestTriggered(bool checked = false);
    void finished() { onActionTestTriggered(); }
    void updatePorts();
    void ping();

    void readSettings();
    void writeSettings();

    void message(const QString&);
};
