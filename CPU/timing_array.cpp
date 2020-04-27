#include "timing_cache.h"

// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

namespace MemoryHierarchy {

int TimingArray::Get(unsigned int data_ptr, char output_data[]) {
    // printf("Read data Latency increase %x cycles\n", read_access_cycle);
    assert((data_ptr <= (array_size - block_size)));
    memcpy(output_data, array + data_ptr, block_size);
    cache->AddAccessLatency(read_access_cycle);
    return 0;
}

int TimingArray::Set(unsigned int data_ptr, const char input_data[]) {
    // printf("Write data Latency increase %x cycles\n", write_access_cycle);
    assert((data_ptr <= (array_size - block_size)));
    memcpy(array + data_ptr, input_data, block_size);
    cache->AddAccessLatency(write_access_cycle);
    return 0;
}

}
