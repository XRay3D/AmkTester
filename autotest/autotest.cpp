#include "autotest.h"
#include "autotestmodel.h"

#include <QThread>
#include <devices/devices.h>

static int id = qRegisterMetaType<QVector<int>>("QVector<int>");

void AutoTest::setModel(AutoTestModel* model) { m_model = model; }

AutoTest::AutoTest(QObject* parent)
    : QThread(parent) { }

AutoTest::~AutoTest() { }

void AutoTest::run() {
    Devices::tester()->stop();
    int ctr{};
    for(auto& test : m_model->data()) {
        ++ctr;
        if(test.userActivity.size()) {
            mutex.lock();
            emit message(test.userActivity);
            QMutexLocker locker(&mutex);
        }

        if(!test.use)
            continue;

        if(test.setPoint1.Description.size())
            Devices::kds1()->setRelay(test.setPoint1.Parcel);
        if(test.setPoint2.Description.size())
            Devices::kds2()->setRelay(test.setPoint2.Parcel);

        if(isInterruptionRequested())
            break;

        msleep(50);

        Devices::tester()->measureAll();
        test.measured = Devices::tester()->pinsValue();
        test.ok = test.pattern == test.measured;

        auto index{m_model->index(ctr - 1, AutoTestModel:: TestResult)};
        emit m_model->dataChanged(index, index, {Qt::CheckStateRole, Qt::DecorationRole});
    }

    Devices::kds1()->setRelay(0);
    Devices::kds2()->setRelay(0);
}
