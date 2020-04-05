#include "scheduler.h"
#include "timing_core.h"
#include "assert.h"

void Scheduler::printSched() {
    printf("size:%lu ", event_vec->size());
    for (unsigned int i = 0; i < event_vec->size(); i++) {
        printf(", 0x%x %x", event_vec->at(i)->pc_addr, event_vec->at(i)->state);
    }
    printf("\n");
}

void Scheduler::deq() {
    if (is_empty())
        return;
    TimingEvent *event = event_vec->front();
    event_vec->pop_front();

    if (event_vec->size() > 0) {
        auto min_event_id = event_vec->begin();
        for (auto event_id = event_vec->begin(); event_id != event_vec->end(); event_id++) {
            if (comp(*min_event_id, *event_id)) {
                min_event_id = event_id;
            }
        }
        TimingEvent * tmp = event_vec->front();
        event_vec->front() = *min_event_id;
        *min_event_id = tmp;
    }

    // printf("%lu: addr: %p state:%x scycle:%d\n", event->current_cycle, event->pc_addr, event->state, event->start_cycle);
    // printSched();

    switch (event->state) {
        case TES_WaitFetcher:
            core->fetcher->Issue(event);
            enq(event);
            break;
        case TES_WaitDecoder:
            // printf("\tDecode %s\n", event->inst->source_line);
            core->decoder->Issue(event);
            enq(event);
            break;
        case TES_WaitExecutor:
            core->executor->Issue(event);
            enq(event);
            break;
        case TES_WaitLSU:
            core->lsu->Issue(event);
            enq(event);
            break;
        case TES_WaitROB:
            core->rob->Issue(event);
            enq(event);
            break;
        case TES_Committed:
            delete event;
            break;
        default:
            printf("Unknown event type!\n");
            assert(false);
    }
}

Scheduler::Scheduler(TimingComponent * _parent)
{
    event_vec = new std::deque<TimingEvent *>;
    core = dynamic_cast<TimingCore *>(_parent);
}