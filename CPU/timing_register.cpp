#include "timing_register.h"

void TimingRegister::Reset(ncycle_t & cycle) {
    for (int i = 0; i < R_LENGTH; i++) {
        // Reset both timing register and SPIM register to initial register file.
        reg[i] = R[i];
        reg_used[i] = 0;
        // printf("%d: %x %x\n", i, Reg[i], reg_used[i]);
    }
    available_cycle = MAX(cycle, available_cycle) + R_LENGTH * c_reg_write_latency;
    cycle = available_cycle;
    printf("reset register\n");
}