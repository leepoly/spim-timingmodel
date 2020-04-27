#ifndef TIMING_CORE_H
#define TIMING_CORE_H

// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

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
#include "timing_cache.h"
#include "timing_mem.h"

class TimingCore : public TimingComponent {
  public:
    TimingFetcher *fetcher = nullptr;
    TimingDecoder *decoder = nullptr;
    TimingExecutor *executor = nullptr;
    TimingLSU *lsu = nullptr;
    TimingROB *rob = nullptr;
    NextPCGen *next_pc_gen = nullptr;
    TimingRegister *regfile = nullptr;
    ALU *alu = nullptr;
    Scheduler *sched = nullptr;
    MemoryHierarchy::TimingCache *cache = nullptr;
    MemoryHierarchy::Memory *mem = nullptr;
    bool cache_enabled = false;

    ncycle_t s_total_cycle = 0x0;
    unsigned int s_total_inst = 0x0;

    TimingCore(bool cache_enabled_)
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
        if (cache_enabled_) {
            // Separate memory when cache enabled
            this->cache_enabled = cache_enabled_;
            cache = new MemoryHierarchy::TimingCache(this);
            mem = new MemoryHierarchy::Memory();
            cache->SetMemory(mem);
        }

        // we move all reset calling to spim-utils.cpp. Don't do it here.
    }

    void Reset() {
        ncycle_t dummy;
        regfile->Reset(dummy);
        if (cache_enabled) {
            if (mem) {
                mem->Reset();
                available_cycle = MAX(available_cycle, cache->GetAvaiableCycle());
            }
            if (cache) cache->Reset();
        }
    }

    void DisplayStats() {
        printf("---statistics from TimingCore---\n");
        printf("\t Total cycles used: %d\n", s_total_cycle);
        printf("\t Total instruction numbers: %d\n", s_total_inst);
    }

    void Issue(TimingEvent * event) {}
};

#endif // TIMING_CORE_H