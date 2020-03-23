#ifndef TIMING_FETCHER_H
#define TIMING_FETCHER_H

#include "assert.h"
#include "timingmodel.h"

class TimingFetcher : public TimingComponent {
  public:
    void Issue(TimingEvent* event) {
        event->inst = read_mem_inst(event->pc_addr);
        // Currently we don't consider the long latency of instruction fetching (i.e., an ideal I-cache).

        available_cycle = MAX(event->current_cycle, available_cycle) + c_fetch_latency;
        event->current_cycle = available_cycle;
        event->execute_cycles += c_fetch_latency;

        assert(event->state == TES_WaitFetcher);
        event->state = TES_WaitDecoder;
    }

    TimingFetcher(TimingComponent * _parent) {
        available_cycle = 0;
    }
};

#endif // TIMING_FETCHER_H
