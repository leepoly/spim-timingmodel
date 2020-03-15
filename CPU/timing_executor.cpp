#include "assert.h"
#include "timing_executor.h"
#include "timing_core.h"

void TimingExecutor::Issue(TimingEvent * event) {
    reg_word a = 0x0, b = 0x0;
    if (event->inst_is_I_type) {
        core->regfile->Load(event->rs, a);
        if (event->imm_is_32b)
            b = event->extended_imm;
        else if (event->imm_is_sign_extend)
            b = (short)event->imm;
    }
    event->reg_wb_val = core->alu->Execute(a, b, event->alu_op);

    available_cycle = MAX(event->current_cycle, available_cycle) + c_executor_latency;
    event->current_cycle = available_cycle;
    event->execute_cycles += c_executor_latency;

    assert(event->state == TES_WaitExecutor);
    event->state = TES_WaitLSU;
}

TimingExecutor::TimingExecutor(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}