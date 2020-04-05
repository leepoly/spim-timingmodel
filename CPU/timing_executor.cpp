#include "assert.h"
#include "timing_executor.h"
#include "timing_core.h"

void TimingExecutor::Issue(TimingEvent * event) {
    event->alu_result = core->alu->Execute(event->alu_src_1, event->alu_src_2, event->ALUOp);

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