#include "autotest.h"
#include "hwinterface/interface.h"
#include <QThread>
#include <autotest/model.h>

AutoTest::AutoTest(QObject* parent)
    : QObject(parent)
{
}

AutoTest::~AutoTest() {}

void AutoTest::start(Model* model)
{
    m_model = model;
    if (!m_model->m_vChecked->contains(true) || !m_model->m_hChecked->contains(true)) {
        emit message("No selected!");
        sem.acquire(sem.available());
        emit finished();
        return;
    }

    for (int c = 0; c < m_model->columnCount(); ++c) {
        {
            if (!m_model->m_hChecked->at(c)) // skip unused
                continue;
        }
        {
            emit message(QString("Set Channel %1").arg(c + 1));
            sem.acquire();
            if (sem.tryAcquire()) {
                emit finished();
                return;
            }
        }
        for (int r = 0; r < m_model->rowCount(); ++r) {
            {
                if (!m_model->m_vChecked->at(r)) // skip unused
                    continue;
            }
            qDebug() << r << c;
            Interface::kds1()->setOut(0, m_model->m_data[r][c].parcel1.toInt());
            Interface::kds2()->setOut(0, m_model->m_data[r][c].parcel2.toInt());
            thread()->msleep(500);
            //Interface::tester()->measureAll();
            Interface::tester()->measureAll();
            thread()->msleep(500);
            m_model->test(r, c, Interface::tester()->pinsValue());
            if (thread()->isInterruptionRequested()) {
                return;
            }
        }
    }
    emit finished();
}

void AutoTest::stop()
{
    thread()->requestInterruption(); //sem.release();
}
