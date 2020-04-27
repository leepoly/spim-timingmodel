#ifndef TIMING_MEM_H
#define TIMING_MEM_H

#include "timing_cache.h"

class TimingCore;

namespace MemoryHierarchy {
    #define BYTES_TO_INST(N) (((N) + BYTES_PER_WORD - 1) / BYTES_PER_WORD * sizeof(instruction *))

class Memory {
public:
    void make_memory(int text_size, int data_size, int data_limit,
                 int stack_size, int stack_limit, int k_text_size,
                 int k_data_size, int k_data_limit);
    void * mem_reference(mem_addr addr);
    instruction *read_mem_inst(mem_addr addr);
    reg_word read_mem_word(mem_addr addr);
    void set_mem_word(mem_addr addr, reg_word value);
    mem_word bad_mem_read(mem_addr addr, int mask);
    void bad_mem_write(mem_addr addr, mem_word value, int mask);
    void expand_stack(int addl_bytes);
    void Reset();
    Memory();
    ~Memory();
private:
    instruction **text_seg;
    mem_addr text_top;
    mem_word *data_seg;
    short *data_seg_h;     /* Points to same vector as DATA_SEG */
    BYTE_TYPE *data_seg_b; /* Ditto */
    mem_addr data_top;
    mem_word *stack_seg;
    short *stack_seg_h;     /* Points to same vector as STACK_SEG */
    BYTE_TYPE *stack_seg_b; /* Ditto */
    mem_addr stack_bot;
    instruction **k_text_seg;
    mem_addr k_text_top;
    mem_word *k_data_seg;
    short *k_data_seg_h;
    BYTE_TYPE *k_data_seg_b;
    mem_addr k_data_top;
    int32 data_size_limit, stack_size_limit, k_data_size_limit;

    int mem_text_seg_size = 0x0;
    int mem_data_seg_size = 0x0;
    int mem_stack_seg_size = 0x0;
    int mem_k_data_seg_size = 0x0;
    int mem_k_text_seg_size = 0x0;
};

}
#endif // TIMING_CACHE_H