#ifndef MEASURINGINTERFACE_H
#define MEASURINGINTERFACE_H

#include "tester.h"
#include "kds.h"

class Interface {
public:
    Interface();
    ~Interface();
    static Tester* tester();
    static Amk* kds1();
    static Amk* kds2();
};

#endif // MEASURINGINTERFACE_H
