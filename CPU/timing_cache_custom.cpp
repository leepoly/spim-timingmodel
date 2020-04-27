#include "timing_cache_custom.h"
namespace MemoryHierarchy {

// return true and offset is the id of the victim way. return false if refusing eviction.
bool CacheController::EvictCacheline(mem_addr addr, bool is_write, mem_addr PC, int &offset) {
    // TODO: implement a FIFO replacement policy
    return false; // FIXME: This refuses to allocate any cacheline. Remove me first.
    // Here I provide pseudo code for implementing a FIFO replacement policy. Other policy should be similar.
    // 1. tag entry = GetTag(tag_ptr, entry)
    // 2. entry->fifo_ptr = (entry->fifo_ptr + 1) % c_asso_num
    // 3. if fifo_ptr points to a Valid & Dirty data block:
    //    tmp_data = GetData(offset)
    //    WriteMemoryData(address_in_memory, tmp_data)
    // 4. TagSet(entry)
    // 5. return true.
    int idx = 0x0;  // FIXME:
    char buf[c_tag_entry_size];
    // When we get an entry, the current cycle of this request increases accordingly.
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    // We force the type of our result to be a (CacheTagEntry*) type
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    // Now we can control the copy of our entry conveniently
    entry->fifo_ptr = entry->fifo_ptr;  // FIXME:
    offset = 0x0;  // FIXME:
    if (entry->valid[entry->fifo_ptr] && entry->dirty[entry->fifo_ptr]) {
        // write back
        char data_buf[c_cache_line_size];
        cache->data_array->Get(offset, data_buf);
        // We call writeback method provided by TimingCache, to write a cacheline to memory
        cache->WriteMemoryData(entry->tag[entry->fifo_ptr], data_buf);
    }
    entry->valid[entry->fifo_ptr] = false;
    // Do not forget to update tag array with this new tag entry copy
    cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    return true;
}

// called this to update tag entry after allocating a new cache block, or a hit
bool CacheController::UpdateMetadata(mem_addr addr, bool is_write, mem_addr PC, int offset) {
    int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    int way = 0x0;  // FIXME:
    entry->valid[way] = entry->valid[way];  // FIXME:
    entry->dirty[way] = entry->dirty[way];  // FIXME:
    entry->tag[way] = entry->tag[way]; // FIXME:
    cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    return false;
}

// check if the addr is a hit. data is set to hit cacheline and hit_offset is its data array pointer.
void CacheController::AccessTagArray(mem_addr addr, mem_addr PC, bool &is_hit, cache_line &data, int &hit_offset) {
    int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    is_hit = false;
    // TODO: Return hit_offset and data if there is a hit
    // Hint: hit_offset = (hit_idx * c_asso_num + hit_way) * c_cache_line_size
    hit_offset = 0x0;  // FIXME:
    cache->data_array->Get(hit_offset, data);
}

// Only need to set Valid to 0
void CacheController::ResetTagArray() {
    CacheTagEntry *entry = nullptr;
    char buf[c_tag_entry_size];
    for (int idx = 0; idx < c_cache_set_num; ++idx) {
        cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
        entry = (CacheTagEntry *)buf;
        for (int way = 0; way < c_asso_num; ++way)
            entry->valid[way] = false;
        cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    }
}

void CacheController::Reset() {
    ResetTagArray();
}

// Merge wdata (one word) into a cacheline
void CacheController::MergeBlock(char* blk, reg_word wdata, int offset, int store_data_size) {
    memcpy(blk + offset, &wdata, store_data_size);
}

// Handling requests from TimingCache
void CacheController::Access(mem_addr addr, bool is_write, reg_word &wrdata, mem_addr PC, int store_data_size) {
    printf("ack a request addr:%lx is_write:%x wrdata:%x\n", addr, is_write, wrdata);
    cache_line data_blk;
    bool is_hit;
    int hit_offset;  // the offset of the hit cacheline
    AccessTagArray(addr, PC, is_hit, data_blk, hit_offset);
    // TODO: Complete every possible access path
    if (is_hit) {
        printf("hit, dataptr:%x\n", hit_offset);
        if (!is_write) {
            // FIXME: Put one word from hit cacheline to wrdata
        } else {
            // FIXME: Merge wdata (one word) with hit cacheline and write to data_array
        }
        UpdateMetadata(addr, is_write, PC, hit_offset);
    } else {
        int evict_line_offset = -1;
        // select one candidate from current set
        bool eviction_accepted = EvictCacheline(addr, is_write, PC, evict_line_offset);
        printf("miss, evict ptr:%x\n", evict_line_offset);
        // fetch this cacheline from outer memory
        printf("fetch remote data\n");
        cache->FetchMemoryData(addr, (char *)&data_blk);
        if (is_write) {
            // FIXME: Merge wdata (one word) with fetched cacheline
        }
        if (eviction_accepted) {
            assert(evict_line_offset >= 0 && evict_line_offset <= (c_cache_capacity - c_cache_line_size));
            // FIXME: if the cache approves to evict, write fetched data_blk to the victim location
            printf("write to victim write data arr\n");
        } else {
            if (is_write) {
                // if the cache refuses to allocate, write back to memory
                cache->WriteMemoryData(addr, (char *)&data_blk);
            }
        }
        if (!is_write) {
            // return read data from fetched data_blk
            memcpy(&wrdata, (char *)(&data_blk) + (addr & c_addr_cache_line_mask), sizeof(reg_word));
        }
    }
}

// Display your own stats
void CacheController::DisplayStats() {}

// Initilization. Set a default block access granularity. Reset tag array.
CacheController::CacheController(MemoryHierarchy::TimingCache *cache_) {
    cache = cache_;
    if (cache->data_array)
        cache->data_array->SetBlockSize(c_cache_line_size);
    if (cache->tag_array)
        cache->tag_array->SetBlockSize(c_tag_entry_size);
    ResetTagArray();
}

}
