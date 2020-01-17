
// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

#include "timing_register.h"

void TimingRegister::Reset(ncycle_t & cycle) {
    for (int i = 0; i < R_LENGTH; i++) {
        Reg[i] = R[i];
    }
    availableCycle = MAX(cycle, availableCycle) + R_LENGTH * c_reg_write_latency;
    cycle = availableCycle;
}