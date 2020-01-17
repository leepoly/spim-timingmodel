#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "timingmodel.h"

class TimingCore;

class Scheduler : public TimingComponent
{
public:
    std::deque<TimingEvent *> * event_vec = nullptr;
    TimingCore * core = nullptr;
    Comp_TimingEvent comp;

    void enq(TimingEvent *event)
    {
        // info("at cycle:%lu enqueue (%s)", event->currentCycle, typeid(*event).name());
        if (event_vec->size() == 0) {
            event_vec->push_front(event);
            return;
        }
        TimingEvent * min_event = event_vec->front();
        if (comp(min_event, event)) { // choose event
            event_vec->push_front(event);
        } else {
            event_vec->push_back(event);
        }
        // queue.push(event);
    }

    void deq();

    bool is_empty()
    {
        return (event_vec->size() == 0);
        // return queue.empty();
    }

    Scheduler(TimingComponent * _parent);

    void Issue(TimingEvent * event) {}

    void printSched();
};

#endif // SCHEDULER_H