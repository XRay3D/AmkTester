#pragma once

#include "autotest/autotest.h"
#include "kds.h"
#include "tester.h"

class Devices {
    static inline AutoTest* m_autoTest;
    static inline Kds* m_kds1;
    static inline Kds* m_kds2;
    static inline Tester* m_amkTest;

    static inline QThread thread;
    static inline QSemaphore semafore;

public:
    Devices();
    ~Devices();

    static AutoTest* autoTest();
    static Kds* kds1();
    static Kds* kds2();
    static Tester* tester();
};
