#ifndef LAB2DEF_H
#define LAB2DEF_H

// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

#include <cstdint>

typedef unsigned long long ncycle_t;

const ncycle_t c_fetch_latency = 1;
const ncycle_t c_decode_latency = 1;
const ncycle_t c_executor_latency = 1;
const ncycle_t c_reg_write_latency = 1;
const ncycle_t c_memory_access_latency = 150;
const ncycle_t c_cpu_start_cycle = 1000;

enum TimingEventState
{
    TES_Invalid,
    TES_WaitFetcher, // generated a valid event with only PC provided
    TES_WaitDecoder,
    TES_WaitExecutor,
    TES_WaitLSU,
    TES_WaitROB,
    TES_Committed
};

#endif