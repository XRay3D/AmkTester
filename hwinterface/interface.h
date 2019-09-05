#ifndef MEASURINGINTERFACE_H
#define MEASURINGINTERFACE_H

#include "autotest.h"
#include "kds.h"
#include "tester.h"

class Interface {
public:
    Interface();
    ~Interface();
    static Tester* tester();
    static Amk* kds1();
    static Amk* kds2();
    static AutoTest* at();
};

#endif // MEASURINGINTERFACE_H
