#ifndef TIMING_ROB_H
#define TIMING_ROB_H

#include "timingmodel.h"

class TimingCore;

class TimingROB : public TimingComponent
{
public:
    TimingCore * core;
    TimingROB(TimingComponent * _parent);

    void Issue(TimingEvent * event);
};

#endif // TIMING_ROB_H