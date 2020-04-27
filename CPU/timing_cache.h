/*  Yao-arch Lab 3
    Author: Yiwei Li (liyw19@tsinghua.edu.cn)
    Based on spim, a MIPS functional emulator.
    In lab 3, we insert a timing cache to optimize data access path.
*/

// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

#ifndef TIMING_CACHE_H
#define TIMING_CACHE_H

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
    static const ncycle_t c_data_array_latency = 2;
    static const ncycle_t c_tag_array_latency = 2;

    MemoryHierarchy::TimingArray* data_array = nullptr;
    MemoryHierarchy::TimingArray* tag_array = nullptr;
public:
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

    ncycle_t GetAvaiableCycle() {
        return available_cycle;
    }

    int FetchMemoryData(mem_addr addr, char *data);
    int WriteMemoryData(mem_addr addr, char *data);
    void Reset();
    TimingCache(TimingComponent * _parent);
    void DisplayStats();
    void Issue(TimingEvent * event) {}

private:
    TimingComponent *core;
    Memory *memory;
    CacheController *ctrler = nullptr;

    unsigned int s_served_read_access = 0x0;
    unsigned int s_served_write_access = 0x0;
    unsigned int s_miss_write_access = 0x0;
    unsigned int s_miss_read_access = 0x0;
    ncycle_t s_miss_cycles = 0x0;
    ncycle_t s_total_cycles = 0x0;
    ncycle_t s_background_cycles = 0x0;
};

}

#endif // TIMING_CACHE_H