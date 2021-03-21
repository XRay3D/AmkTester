#ifndef MEASURINGINTERFACE_H
#define MEASURINGINTERFACE_H

#include "autotest.h"
#include "kds.h"
#include "tester.h"

class HW {
public:
    HW();
    ~HW();
    static Tester* tester();
    static Kds_* kds1();
    static Kds_* kds2();
    static AutoTest* autoTest();
};

#endif // MEASURINGINTERFACE_H
