#ifndef TIMING_CORE_H
#define TIMING_CORE_H

#include "timingmodel.h"
#include "timing_fetcher.h"
#include "timing_decoder.h"
#include "timing_executor.h"
#include "timing_register.h"
#include "next_pc_gen.h"
#include "timing_lsu.h"
#include "timing_rob.h"
#include "scheduler.h"
#include "alu.h"

class TimingCore : public TimingComponent
{
public:
    TimingFetcher *fetcher = nullptr;
    TimingDecoder *decoder = nullptr;
    TimingExecutor *executor = nullptr;
    TimingRegister *regfile = nullptr;
    NextPCGen *next_pc_gen = nullptr;
    TimingLSU *lsu = nullptr;
    TimingROB *rob = nullptr;
    ALU *alu = nullptr;

    Scheduler *sched = nullptr;

    bool spim_continuable = true; // continuability reported by spim

    TimingCore()
    {
        fetcher = new TimingFetcher(this);
        decoder = new TimingDecoder(this);
        executor = new TimingExecutor(this);
        next_pc_gen = new NextPCGen(this);
        lsu = new TimingLSU(this);
        rob = new TimingROB(this);
        regfile = new TimingRegister(this);
        sched = new Scheduler(this);
        alu = new ALU();

        ncycle_t dummy;
        regfile->Reset(dummy);

        spim_continuable = true;
    }

    void Issue(TimingEvent * event) {}
};

#endif // TIMING_CORE_H