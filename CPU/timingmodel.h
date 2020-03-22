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
    ncycle_t current_cycle;  // The current cycle when this event resides.
    ncycle_t execute_cycles;  // How many cycles this event get executed.
    ncycle_t start_cycle;  // The cycle when the event is created by NextPCGen.
    TimingEventState state;  // State of the event. see enums in lab2_def.h

    // During IF and ID stage
    mem_addr pc_addr;  // PC address of this instruction
    instruction *inst;  // Point to the instruction. see the structure in inst.h

    // During ID and EXE stage
    bool imm_is_zero_extend;  // If this is I-type, set if immediate is zero-extended.
    bool imm_is_sign_extend;  // If this is I-type, set if immediate is signed-extended.
    bool imm_is_32b;  // This is not a formal setting. I create it for storing PC in JAL.
    reg_word extended_imm;  // If imm_is_32b is true, this stores the 32-bit long immediate.
    int alu_op;  // Op for controlling ALU.

    // During ID, EXE and COMMIT stage
    // Go to MIPS green card for the meanings of some following fields.
    unsigned char rd, rs, rt, shamt;
    short imm;
    mem_addr target;
    bool inst_is_I_type;
    bool inst_is_J_type;
    bool inst_is_R_type;
    bool inst_is_load;  // Set if it is a load instruction
    bool inst_is_syscall;  // Set if it is a syscall
    short opcode;
    unsigned char reg_wb_id = 0;  // Which register does this instruction goes to write back.
    reg_word reg_wb_val;  // This stores the value to be written back to register.
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
    ncycle_t available_cycle;  // The minimum cycle when this timing component is idle (capable of receiving timing events).
    TimingComponent *parent = nullptr;  // Its parent. In lab2 this always points to the core.
    virtual void Issue(TimingEvent * event) = 0;  // When scheduler passes the event to this component, it calls this function. You need to implement your own Issue() if you will design a new component.
};

#endif // TIMINGMODEL_H