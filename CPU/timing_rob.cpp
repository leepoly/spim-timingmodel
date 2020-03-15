#include "timing_rob.h"

#include "assert.h"
#include "syscall.h"
#include "run.h"
#include "timing_core.h"

void TimingROB::Issue(TimingEvent * event) {
    // simulate spim at the same time, for comparison
    mem_addr old_pc = PC;
    run_spim(event->pc_addr, 1, true /* display */);

    // handle reg-related inst
    if (!event->inst_is_syscall) {
        core->regfile->Store(event->reg_wb_id, event->reg_wb_val);
    } else {
        // handle syscall
        reg_word v0_val;
        core->regfile->Load(0x2, v0_val);
        if (v0_val >= 1 && v0_val <= 4) {
            // printf, do nothing due to shared console
        } else if (v0_val == READ_INT_SYSCALL) {
            // read integer: copy results from R[REG_RES] to our corresponding reg
            core->regfile->Store(REG_RES, R[REG_RES]);
        }
    }

    available_cycle = MAX(event->current_cycle, available_cycle) + c_reg_write_latency;
    event->current_cycle = available_cycle;
    event->execute_cycles = event->execute_cycles + c_reg_write_latency;

    assert(event->state == TES_WaitROB);
    event->state = TES_Committed;

    // check the consistency of PC and regfile
    if (old_pc != event->pc_addr) {
        printf("\tError pc: 0x%x should be 0x%x\n", event->pc_addr, PC);
        assert(false);
    }
    if (core->regfile->CheckCorrectness() == 0)
        printf("\taddr: %x ROB correct\n", event->pc_addr);
}

TimingROB::TimingROB(TimingComponent * _parent)
{
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}