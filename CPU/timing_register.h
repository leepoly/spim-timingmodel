#ifndef TIMING_REGISTER_H
#define TIMING_REGISTER_H

// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

#include <assert.h>
#include "timingmodel.h"

class TimingRegister : public TimingComponent {
  public:
    TimingRegister(TimingComponent* _parent) {
        available_cycle = 0;
    }

    reg_word reg[R_LENGTH];
    int reg_used[R_LENGTH]; // You can use this bool array to handle raw-dependency

    void Load(int id, reg_word& content) {
        content = reg[id];
    }

    void Store(int id, reg_word content) {
        if (id > 0) reg[id] = content;
        printf("\treg id:%d write:0x%x\n", id, content);
    }

    void Reset(ncycle_t & cycle);

    int CheckCorrectness() {
        for (int i = 0; i < R_LENGTH; i++) {
            if (R[i] != reg[i]) {
                printf("\tError reg%d: 0x%x should be 0x%x\n", i, reg[i], R[i]);
                assert(false);
                return -1;
            }
        }
        return 0;
    }

    void Issue(TimingEvent* event) {}
};

#endif // TIMING_REGISTER_H