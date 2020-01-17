#include "timing_executor.h"
#include "timing_core.h"
#include "assert.h"

void TimingExecutor::Issue(TimingEvent * event)
{
    reg_word a = 0x0, b = 0x0;
    if (event->inst_is_I_type) {
        core->regfile->Load(event->rs, a);
        if (event->imm_is_32b)
            b = event->extended_imm;
        else if (event->imm_is_sign_extend)
            b = (short)(event->imm);
    } else if (event->inst_is_R_type) {

    }
    event->reg_wb_val = core->alu->Execute(a, b, event->alu_op);

    availableCycle = MAX(event->currentCycle, availableCycle) + c_executor_latency;
    event->currentCycle = availableCycle;
    event->executeCycles += c_executor_latency;

    assert(event->state == TES_WaitExecutor);
    event->state = TES_WaitLSU;
}

TimingExecutor::TimingExecutor(TimingComponent * _parent)
{
    availableCycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}