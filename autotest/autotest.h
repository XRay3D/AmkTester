#pragma once

#include "autotestmodel.h"

#include <QThread>

class Model;

class AutoTest : public QThread {
    Q_OBJECT

    AutoTestModel* m_model;

signals:
    void message(const QString&);

public:
    AutoTest(QObject* parent = nullptr);
    virtual ~AutoTest();

    void setModel(AutoTestModel* model);
    QMutex mutex;

    // QThread interface
protected:
    void run() override;
};
