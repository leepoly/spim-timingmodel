#include "timing_cache.h"

// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

namespace MemoryHierarchy {

void TimingCache::Access(mem_addr addr, bool is_write, reg_word &wrdata, mem_addr PC, ncycle_t &current_cycle, int store_data_size) {
    assert(ctrler);
    // update cache time to lateste event time
    current_cycle = available_cycle = MAX(current_cycle, available_cycle);
    // pass the request to controller
    ctrler->Access(addr, is_write, wrdata, PC, store_data_size);
    // update statistical counter
    assert(available_cycle > current_cycle);
    if (available_cycle - current_cycle >= c_memory_access_latency) {
        // if we spend less than c_memory_access_latency cycles, we believe it is a hit, otherwise it is a miss
        if (is_write) {
            s_miss_write_access++;
        } else {
            s_miss_read_access++;
        }
        s_miss_cycles += available_cycle - current_cycle;
    }
    s_total_cycles += available_cycle - current_cycle;
    if (is_write) {
        s_served_write_access++;
    } else {
        s_served_read_access++;
    }
    current_cycle = available_cycle;
    // after sending reply, timing cache can do something in the background
    ncycle_t before_background_cycle = available_cycle;
    ctrler->ProcessOffCritical(addr, is_write, PC);
    s_background_cycles += available_cycle - before_background_cycle;
    return;
}

int TimingCache::WriteMemoryData(mem_addr addr, char *data) {
    int cache_block_transaction_num = c_cache_line_size / sizeof(reg_word);
    AddAccessLatency(c_memory_access_latency + (cache_block_transaction_num - 1) * 1);  // ack latency + transfer latency. Assume the bus carries 4B per cycle and ack with first packet saves one transaction number.
    addr = (addr >> c_addr_offset_bit) << c_addr_offset_bit;
    reg_word tmp;
    for (int i = 0; i < cache_block_transaction_num; ++i) {
        memcpy(&tmp, data + i * sizeof(reg_word), sizeof(reg_word));
        memory->set_mem_word(addr + i * sizeof(reg_word), tmp);
    }
    return 0;
}

int TimingCache::FetchMemoryData(mem_addr addr, char *data) {
    int cache_block_transaction_num = c_cache_line_size / sizeof(reg_word);
    AddAccessLatency(c_memory_access_latency + (cache_block_transaction_num - 1) * 1);  // ack latency + transfer latency. Assume the bus carries 4B per cycle and ack with first packet saves one transaction number.
    addr = (addr >> c_addr_offset_bit) << c_addr_offset_bit;
    reg_word tmp;
    for (int i = 0; i < cache_block_transaction_num; ++i) {
        tmp = memory->read_mem_word(addr + i * sizeof(reg_word));
        memcpy(data + i * sizeof(reg_word), &tmp, sizeof(reg_word));
    }
    return 0;
}

void TimingCache::DisplayStats() {
    printf("---statistics from TimingCache---\n");
    printf("\t Number of served write/read requests: %d/%d\n", s_served_write_access, s_served_read_access);
    printf("\t Number of served miss write/read requests: %d/%d\n", s_miss_write_access, s_miss_read_access);
    printf("\t Miss rate: %2.2f%\n", 100.0 * (s_miss_write_access + s_miss_read_access) / (s_served_write_access + s_served_read_access));
    printf("\t Cycles on handling miss events: %d\n", s_miss_cycles);
    printf("\t Cycles on handling all events: %d\n", s_total_cycles);
    printf("\t Cycles of off-critical path events: %d\n", s_background_cycles);
    ctrler->DisplayStats();
}

void TimingCache::Reset() {
    ctrler->Reset();
    printf("reset cache\n");
}

TimingCache::TimingCache(TimingComponent * _parent) {
    core = _parent;
    available_cycle = 0;
    data_array = new MemoryHierarchy::TimingArray(c_cache_capacity, c_cache_line_size, c_data_array_latency, this);
    tag_array = new MemoryHierarchy::TimingArray(c_tag_arr_capacity, c_tag_entry_size_maximum, c_tag_array_latency, this);
    ctrler = new CacheController(this);
    // If you have emulated an assembly with this message, your lab 3 part 1 (code merging) should be successful.
    printf("Timing Cache is enabled now.\n");
}

}
