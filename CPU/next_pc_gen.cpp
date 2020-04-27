#include "next_pc_gen.h"
#include "timing_core.h"

void NextPCGen::GenerateInitialEvent(mem_addr initial_PC) {
    TimingEvent * event = new TimingEvent();
    event->state = TES_WaitFetcher;
    event->current_cycle = c_cpu_start_cycle; // set first c_cpu_start_cycle cycles to reset necessary components: cache, regfile.
    event->execute_cycles = 0;
    event->start_cycle = c_cpu_start_cycle;
    event->pc_addr = initial_PC;
    core->sched->enq(event);

    PC = initial_PC;
}

void NextPCGen::GenerateNextPC(mem_addr pc_addr, reg_word extended_imm, mem_addr target, bool branch, bool jump, ncycle_t current_cycle) {
    mem_addr next_pc = -1;
    if (branch)
        next_pc = pc_addr + BYTES_PER_WORD + (extended_imm << 2);
    else
        next_pc = pc_addr + BYTES_PER_WORD;
    if (jump)
        next_pc = (pc_addr & 0xf0000000) | (target << 2);  // PC[31:28] || Instruction[25:0] || 00

    if (next_pc == -1) {
        printf("Unsupported Jump or Branch opcode!\n");
        assert(false);
    }

    TimingEvent * new_event = new TimingEvent();
    new_event->state = TES_WaitFetcher;
    new_event->current_cycle = current_cycle;
    new_event->execute_cycles = 0;
    new_event->start_cycle = current_cycle;
    new_event->pc_addr = next_pc;
    core->sched->enq(new_event);
}

NextPCGen::NextPCGen(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}