#include "interface.h"

static Kds_* Hart = nullptr;
static Kds_* Kds = nullptr;
static Tester* AmkTest = nullptr;
static AutoTest* AT = nullptr;

static QThread thrd;
static QSemaphore semafore;

HW::HW()
{
    if (!semafore.available()) {

        AmkTest = new Tester;
        AmkTest->moveToThread(&thrd);

        Hart = new Kds_;
        Hart->moveToThread(&thrd);

        Kds = new Kds_;
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

HW::~HW()
{
    semafore.acquire();
    if (!semafore.available()) {
        thrd.quit();
        thrd.wait();
    }
}

Tester* HW::tester() { return AmkTest; }

Kds_* HW::kds1() { return Kds; }

Kds_* HW::kds2() { return Hart; }

AutoTest* HW::autoTest() { return AT; }
