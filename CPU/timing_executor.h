#ifndef TIMING_EXECUTOR_H
#define TIMING_EXECUTOR_H

#include "timingmodel.h"

class TimingCore;

class TimingExecutor : public TimingComponent {
  public:
    TimingCore * core;
    TimingExecutor(TimingComponent * _parent);

    void Issue(TimingEvent * event);
};

#endif // TIMING_EXECUTOR_H