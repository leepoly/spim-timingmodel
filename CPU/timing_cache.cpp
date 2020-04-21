#include "timing_cache.h"

// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

namespace MemoryHierarchy {

void TimingCache::Access(mem_addr addr, bool is_write, reg_word &wrdata, mem_addr PC, ncycle_t &current_cycle, int store_data_size) {
    assert(ctrler);
    available_cycle = MAX(current_cycle, available_cycle);
    ctrler->Access(addr, is_write, wrdata, PC, store_data_size);
    current_cycle = available_cycle;
    ctrler->ProcessOffCritical(addr, is_write, PC);
    return;
}

int TimingCache::WriteMemoryData(mem_addr addr, char *data) {
    AddAccessLatency(c_memory_access_latency);
    addr = (addr >> c_addr_offset_bit) << c_addr_offset_bit;
    int cache_block_transaction_num = c_cache_line_size / sizeof(reg_word);
    reg_word tmp;
    for (int i = 0; i < cache_block_transaction_num; ++i) {
        memcpy(&tmp, data + i * sizeof(reg_word), sizeof(reg_word));
        // printf("\twrite mem addr:%lx val:%x\n", addr + i * sizeof(reg_word), tmp);
        memory->set_mem_word(addr + i * sizeof(reg_word), tmp);
    }
    return 0;
}

int TimingCache::FetchMemoryData(mem_addr addr, char *data) {
    AddAccessLatency(c_memory_access_latency);
    addr = (addr >> c_addr_offset_bit) << c_addr_offset_bit;
    // printf("fetch block:%x\n", addr);
    int cache_block_transaction_num = c_cache_line_size / sizeof(reg_word);
    reg_word tmp;
    for (int i = 0; i < cache_block_transaction_num; ++i) {
        tmp = memory->read_mem_word(addr + i * sizeof(reg_word));
        // printf("\tfetch addr:%lx %x\n", addr + i * sizeof(reg_word), tmp);
        memcpy(data + i * sizeof(reg_word), &tmp, sizeof(reg_word));
    }
    return 0;
}

TimingCache::TimingCache(TimingComponent * _parent) {
    core = _parent;
    available_cycle = 0;
    data_array = new MemoryHierarchy::TimingArray(c_cache_capacity, c_cache_line_size, c_data_array_latency, this);
    tag_array = new MemoryHierarchy::TimingArray(c_tag_array_size, c_tag_entry_size, c_tag_array_latency, this);
    ctrler = new CacheController(this);
    // If you have emulated an assembly with this message, your lab 3 part 1 (code merging) should be successful.
    printf("Timing Cache is enabled now.\n");
}

}
