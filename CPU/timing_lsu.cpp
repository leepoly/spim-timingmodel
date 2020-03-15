#include "assert.h"
#include "timing_lsu.h"
#include "timing_core.h"

void TimingLSU::Issue(TimingEvent * event) {
    reg_word val_rs = 0x0, val_rt = 0x0;
    core->regfile->Load(event->rs, val_rs);
    core->regfile->Load(event->rt, val_rt);
    switch (event->opcode) {
        case Y_LW_OP:
            LOAD_INST(&event->reg_wb_val,
                        read_mem_word(val_rs + event->imm),
                        0xffffffff);
            break;
    }

    if (event->inst_is_load) {
        available_cycle = MAX(event->current_cycle, available_cycle) + c_memory_access_latency;
        event->current_cycle = available_cycle;
        event->execute_cycles += c_memory_access_latency;
    } else {
        // this stage still count 1 cycle even when no memory request
        available_cycle = MAX(event->current_cycle, available_cycle) + 1;
        event->current_cycle = available_cycle;
        event->execute_cycles += 1;
    }

    assert(event->state == TES_WaitLSU);
    event->state = TES_WaitROB;
}

TimingLSU::TimingLSU(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}