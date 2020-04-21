#ifndef LAB3_H
#define LAB3_H

namespace MemoryHierarchy {

/* basic definitions */
typedef char cache_line[16 * sizeof(reg_word)];
const int c_addr_cache_line_mask = 0x3f;  // get the least 6 bits
const int c_addr_offset_bit = 6;  // log2(c_cache_line_size)
const int c_cache_capacity = 64 * K;  // capacity (size of data array) is 64KB
const int c_cache_line_size = 16 * sizeof(reg_word);  // cache line size if 64B
const int c_cache_block_num = c_cache_capacity / c_cache_line_size;


}

#endif // LAB3_H