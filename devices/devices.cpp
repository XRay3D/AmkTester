#include "devices.h"

Devices::Devices()
{
    if (!semafore.available()) {
        QObject* objArray[] {
            m_amkTest = new Tester,
            m_autoTest = new AutoTest,
            m_kds1 = new Kds,
            m_kds2 = new Kds,
        };

        for (auto obj : objArray) {
            obj->moveToThread(&thread);
            thread.connect(&thread, &QThread::finished, obj, &QObject::deleteLater);
        }

        thread.start(QThread::NormalPriority);
    }
    semafore.release();
}

Devices::~Devices()
{
    semafore.acquire();
    if (!semafore.available()) {
        thread.quit();
        thread.wait();
    }
}

Tester* Devices::tester() { return m_amkTest; }

Kds* Devices::kds1() { return m_kds1; }

Kds* Devices::kds2() { return m_kds2; }

AutoTest* Devices::autoTest() { return m_autoTest; }
