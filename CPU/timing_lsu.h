#ifndef TIMING_LSU_H
#define TIMING_LSU_H

#include "timingmodel.h"
#include "parser_yacc.h"

#define LOAD_INST(DEST_A, LD, MASK)           \
    {                                         \
        *(DEST_A) = (LD & (MASK));           \
    }

class TimingCore;

class TimingLSU : public TimingComponent
{
public:
    TimingCore * core;
    TimingLSU(TimingComponent * _parent);

    void Issue(TimingEvent * event);
};

#endif // TIMING_LSU_H