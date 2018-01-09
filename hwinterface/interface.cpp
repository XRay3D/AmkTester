#include "interface.h"

#include <QTimer>

AmkTester* amkTester = nullptr;
KDS* Kds = nullptr;
QThread thrd;
QSemaphore semafore;

Interface::Interface()
{
    if (!semafore.available()) {
        amkTester = new AmkTester;
        Kds = new KDS;

        amkTester->moveToThread(&thrd);
        Kds->moveToThread(&thrd);

        thrd.connect(&thrd, &QThread::finished, amkTester, &QObject::deleteLater);
        thrd.start(QThread::NormalPriority);
    }
    semafore.release();
}

Interface::~Interface()
{
    semafore.acquire();
    if (!semafore.available()) {
        thrd.quit();
        thrd.wait();
    }
}

AmkTester* Interface::tester()
{
    return amkTester;
}

KDS* Interface::kds()
{
    return Kds;
}
