#ifndef CACHE_CONTROLLER_H
#define CACHE_CONTROLLER_H

#include "timing_cache.h"
#include "lab3_def.h"

namespace MemoryHierarchy {
class TimingCache;

// address translation:
// |----tag 23b ----|---idx 5b---|-offset5b-|
const int c_asso_num = 2;
struct CacheTagEntry {
    bool valid[c_asso_num];
    bool dirty[c_asso_num];
    short fifo_ptr = 0;
    mem_addr tag[c_asso_num];
};

// define more constants
const int c_cache_set_num = c_cache_block_num / c_asso_num;  // 32
const int c_addr_idx_mask = 0x3e0;
const int c_tag_entry_size = sizeof(CacheTagEntry);  // 8B for each cache line
const int c_tag_array_size = c_tag_entry_size * c_cache_set_num;  // 8B * 4 * 32 = 512B < capacity

class CacheController {
private:
    MemoryHierarchy::TimingCache *cache;
    void AccessTagArray(mem_addr addr, mem_addr PC, bool &is_hit, cache_line &data, int &hit_offset);
    void MergeBlock(char* blk, reg_word wdata, int offset, int store_data_size);
    bool EvictCacheline(mem_addr addr, bool is_write, mem_addr PC, int &offset);
    bool UpdateMetadata(mem_addr addr, bool is_write, mem_addr PC, int offset);
    // void Addr2IdxWay(mem_addr addr, int &index, int &way);
public:
    void Access(mem_addr addr, bool is_write, reg_word &wrdata, mem_addr PC, int store_data_size);
    void ResetTagArray();
    // Process off the critical operation. Now you can solve somethind not urgent, e.g., writing cacheline back to memory. This still increments available_cycle but may not influence current instruction.
    void ProcessOffCritical(mem_addr addr, bool is_write, mem_addr PC) {}
    CacheController(MemoryHierarchy::TimingCache *cache_);
    void DisplayStats();
    // define your own functions

};

}

#endif // CACHE_CONTROLLER_H