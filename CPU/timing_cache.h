#ifndef TIMING_CACHE_H
#define TIMING_CACHE_H

/* SPIM-T lab3
liyiwei: liyw19@mais.tsinghua.edu.cn
*/

// DO NOT MODIFY

#include "assert.h"
#include "timingmodel.h"
#include "timing_mem.h"
#include "timing_cache_custom.h"
#include "timing_array.h"
#include "lab3_def.h"

class TimingCore;
class Memory;

namespace MemoryHierarchy {
class TimingArray;
class CacheController;

class TimingCache : public TimingComponent {
public:
    static const ncycle_t c_data_array_latency = 25;
    static const ncycle_t c_tag_array_latency = 25;

    MemoryHierarchy::TimingArray* data_array = nullptr;
    MemoryHierarchy::TimingArray* tag_array = nullptr;
public:
    void Issue(TimingEvent * event) {}

    // Access: input memory address, is_write, the PC causes this access, current cycle of this PC.
    //      wrdata: when (!is_write), it outputs the extracted read data. When (is_write), it is an input consisting of wdata
    //      store_data_size: SW -> 4B; SH -> 2B; SB -> 1B, memory load does not use it
    void Access(mem_addr addr, bool is_write, reg_word &wrdata, mem_addr PC, ncycle_t &current_cycle, int store_data_size);

    void SetMemory(Memory *mem_) {
        memory = mem_;
    }

    void AddAccessLatency(unsigned int lat) {
        available_cycle += lat;
    }

    int FetchMemoryData(mem_addr addr, char *data);

    int WriteMemoryData(mem_addr addr, char *data);

    TimingCache(TimingComponent * _parent);

private:
    TimingComponent *core;
    Memory *memory;
    CacheController *ctrler = nullptr;

};

}

#endif // TIMING_CACHE_H