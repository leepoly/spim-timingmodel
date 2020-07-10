#ifndef LABDEFS_H
#define LABDEFS_H
#include <cstdint>

typedef uint64_t ncycle_t;

// Latency for each stage
const ncycle_t c_if_latency = 1;
const ncycle_t c_id_latency = 1;
const ncycle_t c_exe_latency = 1;
const ncycle_t c_no_mem_latency = 1;
const ncycle_t c_mem_latency = 150;
const ncycle_t c_wb_latency = 1;

#endif
