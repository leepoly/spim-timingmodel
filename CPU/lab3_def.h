#ifndef LAB3_H
#define LAB3_H

namespace MemoryHierarchy {

/* basic definitions */
const int c_addr_cache_line_mask = 0x1f;  // get the least 5 bits
const int c_addr_offset_bit = 5;  // log2(c_cache_line_size)
const int c_cache_capacity = 1 * K;  // capacity (size of data array) is 1KB
const int c_cache_line_size = 8 * sizeof(reg_word);  // cache line size if 32B
typedef char cache_line[8 * sizeof(reg_word)];
const int c_cache_block_num = c_cache_capacity / c_cache_line_size;  // 64 cachelines
const int c_tag_arr_capacity = 512;  // capacity (size of tag array) is 512B
const int c_tag_entry_size_maximum = 32;  // maximum tag entry size is 32B

}

#endif // LAB3_H