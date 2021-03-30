#ifndef MEASURINGINTERFACE_H
#define MEASURINGINTERFACE_H

#include "autotest.h"
#include "kds.h"
#include "tester.h"

class HW {
    static inline Kds* m_kds2;
    static inline Kds* m_kds1;
    static inline Tester* m_amkTest;
    static inline AutoTest* m_autoTest;

    static inline QThread thread;
    static inline QSemaphore semafore;

public:
    HW();
    ~HW();
    static Tester* tester();
    static Kds* kds1();
    static Kds* kds2();
    static AutoTest* autoTest();
};

#endif // MEASURINGINTERFACE_H
