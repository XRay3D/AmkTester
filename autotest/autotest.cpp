#include "autotest.h"
#include "autotestmodel.h"

#include <QThread>
#include <devices/devices.h>

#include "range/v3/view/iota.hpp"
#include "range/v3/view/zip.hpp"
namespace rv = ranges::views;

//static int id = qRegisterMetaType<std::vector<int>>("std::vector<int>");

void AutoTest::setModel(AutoTestModel* model) { m_model = model; }

AutoTest::AutoTest(QObject* parent)
    : QThread(parent) { }

AutoTest::~AutoTest() { }

void AutoTest::run() {
    Devices::tester()->stop();

    QString lastUserActivity;
    for(auto&& [ctr, test] : rv::zip(rv::iota(0), m_model->data())) {

        if(!test.use)
            continue;

        if(test.userActivity.size() && lastUserActivity != test.userActivity) {
            mutex.lock();
            emit message(test.userActivity);
            QMutexLocker locker(&mutex);
        }

        lastUserActivity = test.userActivity;

        if(test.setPoint1.Description.size())
            Devices::kds1()->setRelay(test.setPoint1.Parcel);
        if(test.setPoint2.Description.size())
            Devices::kds2()->setRelay(test.setPoint2.Parcel);

        if(isInterruptionRequested())
            break;

        msleep(500);

        Devices::tester()->measureAll();
        test.measured = Devices::tester()->resistance();
        test.ok = (test.pattern == test.measured);

        auto index {m_model->index(ctr, AutoTestModel::TestResult)};
        emit m_model->dataChanged(index, index, {Qt::CheckStateRole, Qt::DecorationRole});
    }
    if(Devices::kds1()->isConnected())
        Devices::kds1()->setRelay(0);
    if(Devices::kds2()->isConnected())
        Devices::kds2()->setRelay(0);
}
