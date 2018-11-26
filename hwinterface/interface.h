#ifndef MEASURINGINTERFACE_H
#define MEASURINGINTERFACE_H

#include "amk_tester.h"
#include "kds.h"

class Interface {
public:
    Interface();
    ~Interface();
    static AmkTester* tester();
    static Amk *kds();
    static Amk *hart();
};

#endif // MEASURINGINTERFACE_H
