/*  Yao-arch Lab 2
    Author: Yiwei Li (liyw19@tsinghua.edu.cn)
    Based on spim, a MIPS functional emulator.
    We apply a timing CPU core onto MIPS to record accurate cycle statistics.

*/
#ifndef TIMINGMODEL_H
#define TIMINGMODEL_H

#include <queue>
#include <typeinfo>
#include <iostream>

#include "inst.h"
#include "reg.h"
#include "mem.h"

#include "lab2_def.h"

class TimingEvent
{
  public:
    // We should really split this into different subclasses of timingevents (TimingFetchingEvent, TimingDecodingEvent, ...) for better understandability.
    // It is NOT required this year but if you are interested you can have a try.
    TimingEvent()
    {
        current_cycle = execute_cycles = start_cycle = 0;
        state = TES_Invalid;
        inst_is_I_type = inst_is_J_type = inst_is_R_type = inst_is_load = inst_is_syscall = false;
        imm_is_zero_extend = imm_is_sign_extend = imm_is_32b = false;
        opcode = 0;
        reg_wb_id = 0;
        inst = nullptr;
    }
    // During all events
    ncycle_t current_cycle;
    ncycle_t execute_cycles;
    ncycle_t start_cycle;
    TimingEventState state;

    // During IF and ID stage
    mem_addr pc_addr;
    instruction *inst;

    // During ID and EXE stage
    bool imm_is_zero_extend;
    bool imm_is_sign_extend;
    bool imm_is_32b;
    reg_word extended_imm;
    int alu_op;

    // During ID, EXE and COMMIT stage
    unsigned char rd, rs, rt, shamt;
    short imm;
    mem_addr target;
    bool inst_is_I_type;
    bool inst_is_J_type;
    bool inst_is_R_type;
    bool inst_is_load;
    bool inst_is_syscall;
    short opcode;
    unsigned char reg_wb_id = 0;
    reg_word reg_wb_val;
};

// You can implement your RAW confliction check here. Return true if the two instructions are conflicted.
bool confliction_check(TimingEvent* old_inst, TimingEvent* new_inst);

class Comp_TimingEvent {
  public:
    bool operator () (TimingEvent* &a, TimingEvent* &b) const
    {
        // Return true if the current cycle of b is lower than a.
        // You can put your RAW confliction detection code here, to avoid reading stale registers.
        return a->current_cycle > b->current_cycle;
    }
};

class TimingComponent {
  public:
    TimingComponent() {
        available_cycle = -1;
    }
    ncycle_t available_cycle;
    TimingComponent *parent = nullptr;
    virtual void Issue(TimingEvent * event) = 0;
};

#endif // TIMINGMODEL_H