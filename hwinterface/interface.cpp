#include "interface.h"

#include <QTimer>

AmkTester* amkTester = nullptr;
Amk* Kds = nullptr;
Amk* Hart = nullptr;
QThread thrd;
QSemaphore semafore;

Interface::Interface()
{
    if (!semafore.available()) {
        Hart = new Amk;
        Hart->moveToThread(&thrd);

        Kds = new Amk;
        Kds->moveToThread(&thrd);

        amkTester = new AmkTester;
        amkTester->moveToThread(&thrd);

        thrd.connect(&thrd, &QThread::finished, Hart, &QObject::deleteLater);
        thrd.connect(&thrd, &QThread::finished, Kds, &QObject::deleteLater);
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

AmkTester* Interface::tester() { return amkTester; }

Amk* Interface::kds() { return Kds; }

Amk* Interface::hart() { return Hart; }
