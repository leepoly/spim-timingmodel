#ifndef TIMING_REGISTER_H
#define TIMING_REGISTER_H

// DO NOT MODIFY DO NOT MODIFY DO NOT MODIFY
// This file will be overwritten after uploaded.

#include <assert.h>
#include "timingmodel.h"

class TimingRegister : public TimingComponent
{
public:
    TimingRegister(TimingComponent * _parent)
    {
        availableCycle = 0;
    }

    reg_word Reg[R_LENGTH];
    bool reg_used[R_LENGTH]; // you can use a bool array to handle raw-dependency

    void Load(int id, reg_word & content) {
        content = Reg[id];
    }

    void Store(int id, reg_word content) {
        if (id > 0) Reg[id] = content;
        printf("\treg id:%d write:0x%x\n", id, content);
    }

    void Reset(ncycle_t & cycle);

    int check_correct() {
        for (int i = 0; i < R_LENGTH; i++) {
            if (R[i] != Reg[i]) {
                printf("\tError reg%d: 0x%x should be 0x%x\n", i, Reg[i], R[i]);
                assert(false);
                return -1;
            }
        }
        return 0;
    }

    void Issue(TimingEvent * event) {}
};

#endif // TIMING_REGISTER_H