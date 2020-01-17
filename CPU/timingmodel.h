#ifndef TIMINGMODEL_H
#define TIMINGMODEL_H

#include "lab2_def.h"
#include "inst.h"
#include "reg.h"
#include "mem.h"

#include <queue>
#include <typeinfo>
#include <iostream>

class TimingEvent
{
public:
    // TODO: we need split this into different kind of timingevents
    // During all events
    ncycle_t currentCycle;
    ncycle_t executeCycles;
    ncycle_t startCycle;
    TimingEventState state;
    bool stalled;

    // in FE and DE stage
    mem_addr pc_addr;
    instruction *inst;

    // in DE and EXE stage
    bool imm_is_zero_extend;
    bool imm_is_sign_extend;
    bool imm_is_32b;
    reg_word extended_imm;
    bool sa_used; // In R-type, use sa (r[rd] = r[rt] op sa) instead of reg (r[rd] = r[rt] op r[rs])
    int alu_op;

    // in DE, EXE and COMMIT stage
    unsigned char rd, rs, rt, shamt;
    short imm;
    mem_addr target;
    bool inst_is_I_type;
    bool inst_is_J_type;
    bool inst_is_R_type;
    bool inst_is_Load;
    bool inst_is_Syscall;
    short opcode;
    unsigned char reg_wb_id = 0;
    reg_word reg_wb_val;

    // in EXE and COMMIT stage

    // in LSU and COMMIT stage

    TimingEvent()
    {
        currentCycle = 0;
        executeCycles = 0;
        startCycle = 0;
        state = TES_Invalid;

        inst_is_I_type = inst_is_J_type = inst_is_R_type = inst_is_Load = inst_is_Syscall = false;
        imm_is_zero_extend = imm_is_sign_extend = imm_is_32b = false;

        opcode = 0;
        inst = nullptr;
    }

};

bool confliction_check(TimingEvent* old_inst, TimingEvent* new_inst);

class Comp_TimingEvent
{
public:
    bool operator () (TimingEvent* &a, TimingEvent* &b) const
    {
        // true: pq chooses b. b is less than a
        return a->currentCycle > b->currentCycle;
    }
};

class TimingComponent
{
public:
    ncycle_t availableCycle;
    TimingComponent *parent = nullptr;

    TimingComponent()
    {
        availableCycle = -1;
    }

    virtual void Issue(TimingEvent * event) = 0;
};

#endif // TIMINGMODEL_H