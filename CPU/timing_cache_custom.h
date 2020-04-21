#ifndef CACHE_CONTROLLER_H
#define CACHE_CONTROLLER_H

#include "timing_cache.h"
#include "lab3_def.h"

namespace MemoryHierarchy {
class TimingCache;

// address translation:
// |----tag 18b ----|---idx 8b---|-offset6b-|
const int c_asso_num = 4;
struct CacheTagEntry {
    bool valid[c_asso_num];
    bool dirty[c_asso_num];
    short fifo_ptr = 0;
    mem_addr tag[c_asso_num];  // TODO: use bit-level field
};

// define more constants
const int c_cache_set_num = c_cache_block_num / c_asso_num;  // 256
const int c_addr_idx_mask = 0x3fb;
const int c_tag_entry_size = sizeof(CacheTagEntry);  // 8B for each cache line
const int c_tag_array_size = c_tag_entry_size * c_cache_set_num;  // TODO: 18b tag + 1b dirty + 1b valid + 2b lru <= 3B each cacheline

class CacheController {
private:
    MemoryHierarchy::TimingCache *cache;
    void AccessTagArray(mem_addr addr, mem_addr PC, bool &is_hit, cache_line &data, int &hit_offset);
    void MergeBlock(char* blk, reg_word wdata, int offset, int store_data_size);
    bool EvictCacheline(mem_addr addr, bool is_write, mem_addr PC, int &offset);
    bool UpdateMetadata(mem_addr addr, bool is_write, mem_addr PC, int offset);
public:
    void Access(mem_addr addr, bool is_write, reg_word &wrdata, mem_addr PC, int store_data_size);
    void ResetTagArray();
    // Process off the critical operation. Now you can solve somethind not urgent, e.g., writing cacheline back to memory. This still increments available_cycle but may not influence current instruction.
    void ProcessOffCritical(mem_addr addr, bool is_write, mem_addr PC) {}
    CacheController(MemoryHierarchy::TimingCache *cache_);
    // define your own functions

};

}

#endif // CACHE_CONTROLLER_H