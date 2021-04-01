#ifndef MEASURINGINTERFACE_H
#define MEASURINGINTERFACE_H

#include "autotest/autotest.h"
#include "kds.h"
#include "tester.h"

class Devices {
    static inline Kds* m_kds2;
    static inline Kds* m_kds1;
    static inline Tester* m_amkTest;
    static inline AutoTest* m_autoTest;

    static inline QThread thread;
    static inline QSemaphore semafore;

public:
    Devices();
    ~Devices();
    static Tester* tester();
    static Kds* kds1();
    static Kds* kds2();
    static AutoTest* autoTest();
};

#endif // MEASURINGINTERFACE_H