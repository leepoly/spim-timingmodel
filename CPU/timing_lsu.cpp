#include "timing_lsu.h"
#include "timing_core.h"
#include "assert.h"

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

    if (event->inst_is_Load) {
        availableCycle = MAX(event->currentCycle, availableCycle) + c_memory_access_latency;
        event->currentCycle = availableCycle;
        event->executeCycles += c_memory_access_latency;
    } else {
        availableCycle = MAX(event->currentCycle, availableCycle) + 1;
        event->currentCycle = availableCycle;
        event->executeCycles += 1; // still count 1 cycle although no actual memory request
    }

    assert(event->state == TES_WaitLSU);
    event->state = TES_WaitROB;
}

TimingLSU::TimingLSU(TimingComponent * _parent)
{
    availableCycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}