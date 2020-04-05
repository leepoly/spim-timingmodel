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
        inst_is_syscall = false;
        reg_wb_id = 0;
        inst = nullptr;
    }
    // During all events
    ncycle_t current_cycle;  // The current cycle when this event resides.
    ncycle_t execute_cycles;  // How many cycles this event get executed.
    ncycle_t start_cycle;  // The cycle when the event is created by NextPCGen.
    TimingEventState state;  // State of the event. see enums in lab2_def.h

    // IF -> ID stage
    mem_addr pc_addr;  // PC address of this instruction
    instruction *inst;  // Point to the instruction. see the structure in inst.h

    // ID -> EX stage
    reg_word alu_src_1;
    reg_word alu_src_2;
    int ALUOp;  // Op for controlling ALU.

    // EX -> MEM stage
    reg_word alu_result;

    // ID -> MEM stage
    int MemToReg = -1;
    bool MemRead = false;
    bool MemWrite = false;

    // ID -> WB stage
    bool RegWrite = false;
    bool inst_is_syscall;  // Set if it is a syscall

    // ID -> WB stage
    unsigned char reg_wb_id = 0;  // Which register does this instruction goes to write back.

    // MEM -> WB stage
    reg_word reg_wb_data;  // This stores the value to be written back to register.
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