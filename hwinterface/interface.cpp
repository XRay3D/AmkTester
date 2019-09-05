#include "interface.h"

static Amk* Hart = nullptr;
static Amk* Kds = nullptr;
static Tester* AmkTest = nullptr;
static AutoTest* AT = nullptr;

static QThread thrd;
static QSemaphore semafore;

Interface::Interface()
{
    if (!semafore.available()) {

        AmkTest = new Tester;
        AmkTest->moveToThread(&thrd);

        Hart = new Amk;
        Hart->moveToThread(&thrd);

        Kds = new Amk;
        Kds->moveToThread(&thrd);

        AT = new AutoTest;
        AT->moveToThread(&thrd);

        thrd.connect(&thrd, &QThread::finished, AT, &QObject::deleteLater);
        thrd.connect(&thrd, &QThread::finished, AmkTest, &QObject::deleteLater);
        thrd.connect(&thrd, &QThread::finished, Hart, &QObject::deleteLater);
        thrd.connect(&thrd, &QThread::finished, Kds, &QObject::deleteLater);
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

Tester* Interface::tester() { return AmkTest; }

Amk* Interface::kds1() { return Kds; }

Amk* Interface::kds2() { return Hart; }

AutoTest* Interface::at() { return AT; }
