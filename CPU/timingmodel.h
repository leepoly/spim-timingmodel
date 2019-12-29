#ifndef TIMINGMODEL_H
#define TIMINGMODEL_H

#include <cstdint>
#include "spim.h"
#include "mem.h"
#include "inst.h"
#include "reg.h"

#include "log.h"

#include <queue>
#include <typeinfo>
#include <iostream>

typedef uint64_t ncycle_t;

class TimingComponent;
class TimingEvent;
class FetchingEvent;
class DecodingEvent;
class ExecutingEvent;
class TimingFetcher;
class TimingDecoder;
class TimingExecutor;
class TimingCore;
class Scheduler;

ncycle_t c_fetch_latency = 1;
ncycle_t c_decode_latency = 3;
ncycle_t c_executor_latency = 10;

class TimingComponent {
public:
    ncycle_t availableCycle;
    TimingComponent * parent = nullptr;

    TimingComponent() {
        availableCycle = -1;
    }
};

class TimingEvent {
public:
    ncycle_t currentCycle;
    ncycle_t executeCycles;

    TimingComponent* comp = nullptr; //belong to which timing component
    void (*issue) (TimingEvent, TimingEvent *&);

    TimingEvent() {
        currentCycle = 0;
        executeCycles = 0;
    }

    bool operator < (const TimingEvent & a) const {
        //TODO: we need to check dependencies here to choose a best issuable event
        return a.currentCycle < currentCycle;
    }
};

class TimingCore : public TimingComponent {
public:
    TimingFetcher * fetcher = nullptr;
    TimingDecoder * decoder = nullptr;
    TimingExecutor * executor = nullptr;

    Scheduler * sched = nullptr;
    
    TimingCore();
};

class FetchingEvent : public TimingEvent {
public:
    /* input */
    mem_addr pc_addr;
    /* output */
    instruction * fetch_inst;

    FetchingEvent(mem_addr _pc_addr, TimingComponent * _comp);
};

class DecodingEvent : public TimingEvent {
public:
    instruction * inst;

    DecodingEvent(instruction * _inst, TimingComponent * _comp);
};

class ExecutingEvent : public TimingEvent {
public:
    int opcode;
    int rd, rs, rt;
    reg_word imm;

    ExecutingEvent(TimingComponent * _comp);
};

class TimingFetcher : public TimingComponent {
public:
    TimingFetcher(TimingComponent * _parent) {
        availableCycle = 0;
        parent = _parent;
    }
};
//FIXME: wierd c++ pointer to member function syntax... how to fix
// class TimingFetcher : public TimingComponent {
// public:
    void Fetch(FetchingEvent in_event, DecodingEvent *& out_event) {
        out_event = new DecodingEvent(read_mem_inst (in_event.pc_addr), (TimingComponent *)((TimingCore *)(in_event.comp->parent))->decoder);

        in_event.comp->availableCycle = MAX(in_event.currentCycle, in_event.comp->availableCycle) + c_fetch_latency;
        out_event->currentCycle = in_event.comp->availableCycle;
        out_event->executeCycles = in_event.executeCycles + c_fetch_latency;
    }
// };

class TimingDecoder : public TimingComponent {
public:
    TimingDecoder(TimingComponent * _parent) {
        availableCycle = 0;
        parent = _parent;
    }
};

void Decode(DecodingEvent in_event, ExecutingEvent *& out_event) {
        out_event = new ExecutingEvent((TimingComponent *)((TimingCore *)(in_event.comp->parent))->executor);
        out_event->opcode = OPCODE(in_event.inst);
        out_event->rs = RS(in_event.inst);
        out_event->rd = RD(in_event.inst);
        out_event->rt = RT(in_event.inst);
        out_event->imm = IMM(in_event.inst);

        in_event.comp->availableCycle = MAX(in_event.currentCycle, in_event.comp->availableCycle) + c_decode_latency;
        out_event->currentCycle = in_event.comp->availableCycle;
        out_event->executeCycles = in_event.executeCycles + c_fetch_latency;
    }

class TimingExecutor : public TimingComponent {
public:
    TimingExecutor(TimingComponent * _parent) {
        availableCycle = 0;
        parent = _parent;
    }
};

    void Execute(ExecutingEvent event, TimingEvent *& dummy) {
        //TODO: call spim to implement function
        dummy = nullptr;

        event.comp->availableCycle = MAX(event.currentCycle, event.comp->availableCycle) + c_executor_latency;
        event.currentCycle = event.comp->availableCycle;
        event.executeCycles = event.executeCycles + c_executor_latency;

        // std::cout << "at cycle: " << event.currentCycle << " inst finished with executed for " << event.executeCycles << "cycles." << std::endl;
    }

FetchingEvent::FetchingEvent(mem_addr _pc_addr, TimingComponent * _comp) {
    pc_addr = _pc_addr;
    issue = (void (*) (TimingEvent, TimingEvent*& ))&(Fetch);
    comp = _comp;
}

DecodingEvent::DecodingEvent(instruction * _inst, TimingComponent * _comp) {
    inst = _inst;
    issue = (void (*) (TimingEvent, TimingEvent*& ))&(Decode);
    comp = _comp;
}

ExecutingEvent::ExecutingEvent(TimingComponent * _comp) {
    issue = (void (*) (TimingEvent, TimingEvent*& ))&(Execute);
    comp = _comp;
}

class Scheduler {
public:
    std::priority_queue<TimingEvent *> queue;

    void enq(TimingEvent * event) {
        // info("at cycle:%lu enqueue (%s)", event->currentCycle, typeid(*event).name());
        queue.push(event);
    }
     
    void deq() {
        if (isEmpty()) return;
        TimingEvent * event = queue.top();
        // info("at cycle:%lu dequeue (%s)", event->currentCycle, typeid(*event).name());
        queue.pop();
        TimingEvent * out_event = nullptr;
        event->issue(*event, out_event); // issue this event to corresponding component and do their task
        // TODO: add dependencies here!
        // info("at cycle:%lu going to enq (%s)", out_event.currentCycle, typeid(out_event).name());
        if (out_event) {
            enq(out_event);
        }
        delete event;
    }

    bool isEmpty() {
        return queue.empty();
    }
};

TimingCore::TimingCore() {
    fetcher = new TimingFetcher(this);
    decoder = new TimingDecoder(this);
    executor = new TimingExecutor(this);

    sched = new Scheduler();
}

#endif  // TIMINGMODEL_H