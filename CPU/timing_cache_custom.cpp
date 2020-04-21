#include "timing_cache_custom.h"
namespace MemoryHierarchy {

// void CacheController::Offset2IdxWay(int offset, int &index, int &way) {
//     int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
//     int way = (offset / c_cache_line_size) % c_asso_num;

// }

// For simplicity we use a FIFO replacement policy
bool CacheController::EvictCacheline(mem_addr addr, bool is_write, mem_addr PC, int &offset) {
    // return false; // debug
    int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    // CacheTagEntry *entry = (CacheTagEntry *) ((char *)tag_array + idx * sizeof(CacheTagEntry));
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    entry->fifo_ptr = (entry->fifo_ptr + 1) % c_asso_num;
    offset = (idx * c_asso_num + entry->fifo_ptr) * c_cache_line_size;
    if (entry->valid[entry->fifo_ptr] && entry->dirty[entry->fifo_ptr]) {
        // write back
        char data_buf[c_cache_line_size];
        cache->data_array->Get(offset, data_buf);
        cache->WriteMemoryData(entry->tag[entry->fifo_ptr], data_buf);
    }
    entry->valid[entry->fifo_ptr] = false;
    cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    printf("evict.\n");
    return true;
}

// allocate a new cache block
bool CacheController::UpdateMetadata(mem_addr addr, bool is_write, mem_addr PC, int offset) {
    int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    // CacheTagEntry *entry = (CacheTagEntry *) ((char *)tag_array + idx * sizeof(CacheTagEntry));
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    int way = (offset / c_cache_line_size) % c_asso_num;
    entry->valid[way] = true;
    entry->dirty[way] = entry->dirty[way] | is_write;
    entry->tag[way] = addr >> c_addr_offset_bit << c_addr_offset_bit;
    cache->tag_array->Set(idx * sizeof(CacheTagEntry), buf);
    printf("UpdateMeta tagptr:%lx idx:%x, way:%x, tag:%x V:%x D:%x\n", idx * sizeof(CacheTagEntry), idx, way, entry->tag[way], entry->valid[way], entry->dirty[way]);
    offset = (idx * c_asso_num + entry->fifo_ptr) * c_cache_line_size;
    return false;
}

void CacheController::AccessTagArray(mem_addr addr, mem_addr PC, bool &is_hit, cache_line &data, int &hit_offset) {
    int idx = (addr & c_addr_idx_mask) >> c_addr_offset_bit;
    mem_addr line_addr = addr >> c_addr_offset_bit << c_addr_offset_bit;
    char buf[c_tag_entry_size];
    cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
    // CacheTagEntry *entry = (CacheTagEntry *) ((char *)tag_array + idx * sizeof(CacheTagEntry));
    CacheTagEntry *entry = (CacheTagEntry *)buf;
    is_hit = false;
    printf("Access tag ptr:%ux idx:%x\n", idx * sizeof(CacheTagEntry), idx);
    for (int way = 0; way < c_asso_num; ++way) {
        printf("\tway%x: tag:%x V:%x D:%x\n", way, entry->tag[way], entry->valid[way], entry->dirty[way]);
    }
    for (int way = 0; way < c_asso_num; ++way) {
        if (entry->valid[way] && entry->tag[way] == line_addr) {
            is_hit = true;
            hit_offset = (idx * c_asso_num + way) * c_cache_line_size;
            // memcpy(&data, data_array + hit_offset, c_cache_line_size);
            cache->data_array->Get(hit_offset, data);
            break;
        }
    }
}

void CacheController::ResetTagArray() {
    CacheTagEntry *entry = nullptr;
    char buf[c_tag_entry_size];
    for (int idx = 0; idx < c_cache_set_num; ++idx) {
        cache->tag_array->Get(idx * sizeof(CacheTagEntry), buf);
        entry = (CacheTagEntry *)buf;
        // entry = (CacheTagEntry *) ((char *)tag_array + idx * sizeof(CacheTagEntry));
        // printf("reset set%d: %p\n", idx, entry);
        for (int way = 0; way < c_asso_num; ++way)
            entry->valid[way] = false;
    }
}

void CacheController::MergeBlock(char* blk, reg_word wdata, int offset, int store_data_size) {
    memcpy(blk + offset, &wdata, store_data_size);
    // printf("\tmerge: addr:%x data:%x\n", blk+offset, wdata);
}

void CacheController::Access(mem_addr addr, bool is_write, reg_word &wrdata, mem_addr PC, int store_data_size) {
    printf("ack a request addr:%lx is_write:%x wrdata:%x\n", addr, is_write, wrdata);
    cache_line data_blk;
    bool is_hit;
    int hit_offset;  // the offset of the hit cacheline
    AccessTagArray(addr, PC, is_hit, data_blk, hit_offset);
    if (is_hit) {
        printf("hit, ptr:%x\n", hit_offset);
        if (!is_write) {
            // Put one word from hit cacheline to wrdata
            memcpy(&wrdata, (char *)(&data_blk) + (addr & c_addr_cache_line_mask), sizeof(reg_word));
        } else {
            // Merge wdata (one word) with hit cacheline and write to data_array
            MergeBlock((char *)data_blk, wrdata, addr & c_addr_cache_line_mask, store_data_size);
            cache->data_array->Set(hit_offset, data_blk);
            // memcpy(data_array + hit_offset, &data_blk, c_cache_line_size);
        }
        UpdateMetadata(addr, is_write, PC, hit_offset);
    } else {
        int evict_line_offset = -1;
        // select one candidate from current set
        bool eviction_accepted = EvictCacheline(addr, is_write, PC, evict_line_offset);
        printf("miss, evict ptr:%x\n", evict_line_offset);
        // fetch this cacheline from outer memory
        cache->FetchMemoryData(addr, (char *)&data_blk);
        if (is_write) {
            MergeBlock((char *)data_blk, wrdata, addr & c_addr_cache_line_mask, store_data_size);
        }
        if (eviction_accepted) {
            assert(evict_line_offset >= 0 && evict_line_offset < (c_cache_capacity - c_cache_line_size));
            // if the cache approves to evict, write fetched data_blk to the victim location
            cache->data_array->Set(evict_line_offset, data_blk);
            // memcpy(data_array + evict_line_offset, &data_blk, c_cache_line_size);
            UpdateMetadata(addr, is_write, PC, evict_line_offset);
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

CacheController::CacheController(MemoryHierarchy::TimingCache *cache_) {
    cache = cache_;
    ResetTagArray();
    if (cache->data_array)
        cache->data_array->SetBlockSize(c_cache_line_size);
    if (cache->tag_array)
        cache->tag_array->SetBlockSize(c_tag_entry_size);
}

}
