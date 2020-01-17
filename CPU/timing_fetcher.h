#ifndef TIMING_FETCHER_H
#define TIMING_FETCHER_H

#include "timingmodel.h"

class TimingFetcher : public TimingComponent
{
public:
    void Issue(TimingEvent * event)
    {
        event->inst = read_mem_inst(event->pc_addr);
        // now we don't consider inst access latency, i.e., a ideal I-cache

        availableCycle = MAX(event->currentCycle, availableCycle) + c_fetch_latency;
        event->currentCycle = availableCycle;
        event->executeCycles += c_fetch_latency;

        // assert(event->state == TES_WaitFetcher);
        event->state = TES_WaitDecoder;
    }

    TimingFetcher(TimingComponent * _parent)
    {
        availableCycle = 0;
    }
};

#endif // TIMING_FETCHER_H
