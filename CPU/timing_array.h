#ifndef TIMING_ARRAY_H
#define TIMING_ARRAY_H

// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

#include "assert.h"
#include "timing_cache.h"


namespace MemoryHierarchy {
class TimingCache;

class TimingArray {
private:
    unsigned int array_size = 0x0;
    unsigned int read_access_cycle = -1;
    unsigned int write_access_cycle = -1;
    int max_block_size = 0x0;
    int block_size = 0x0;
    TimingCache *cache = nullptr;
    char* array;
    ~TimingArray() {
        free(array);
    }

public:
    TimingArray(int array_size_, int max_block_size_, unsigned int access_cycle_, MemoryHierarchy::TimingCache *cache_) {
        array_size = array_size_;
        // for simplicity we assume it has symmetry read and write latency.
        read_access_cycle = access_cycle_;
        write_access_cycle = access_cycle_;
        max_block_size = max_block_size_;
        block_size = max_block_size_;
        cache = cache_;
        array = (char*)malloc(array_size);
        assert(array);
    }

    // Controller acquires a data (data block or tag entry) to output_data. Update cache->available_cycle accordingly. Return 0 if everything is ok.
    int Get(unsigned int data_ptr, char output_data[]);
    int Set(unsigned int data_ptr, const char input_data[]);
    void SetBlockSize(int block_size_) {
        if (block_size_ > max_block_size) {
            printf("[warning] block size 0x%xB cannot be over capacity 0x%xB. set block size to 0x%xB\n", block_size_, max_block_size, max_block_size);
            block_size_ = max_block_size;
        }
        block_size = block_size_;
    }
};

}

#endif // TIMING_ARRAY_H