#ifndef NEXT_PC_GEN_H
#define NEXT_PC_GEN_H

#include "timingmodel.h"

class TimingCore;

class NextPCGen : public TimingComponent // although NextPCGen is a combinational logic
{
public:
    TimingCore * core = nullptr;
    NextPCGen(TimingComponent * _parent);

    void GenerateInitialEvent(mem_addr initial_PC);

    void GenerateNextPC(TimingEvent * event);

    void Issue(TimingEvent * event) {}
};

#endif // NEXT_PC_GEN_H