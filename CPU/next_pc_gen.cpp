#include "next_pc_gen.h"
#include "timing_core.h"

void NextPCGen::GenerateInitialEvent(mem_addr initial_PC) {
    TimingEvent * event = new TimingEvent();
    event->state = TES_WaitFetcher;
    event->current_cycle = 0;
    event->execute_cycles = 0;
    event->start_cycle = 0;
    event->pc_addr = initial_PC;
    core->sched->enq(event);

    PC = initial_PC;
}

void NextPCGen::GenerateNextPC(TimingEvent * event) {
    mem_addr next_pc = 0x0;
    reg_word val_rs = 0x0, val_rt = 0x0;
    reg_word imm_extend = SIGN_EX(event->imm);
    switch (event->opcode) {
        case Y_JAL_OP:
            JUMP_INST_GENPC(((event->pc_addr & 0xf0000000) | event->target << 2), next_pc);
            break;
        default:
            next_pc = event->pc_addr + BYTES_PER_WORD;
    }
    if (next_pc == 0x0) {
        printf("Unsupported Jump or Branch opcode!\n");
        assert(false);
    }

    TimingEvent * new_event = new TimingEvent();
    new_event->state = TES_WaitFetcher;
    new_event->current_cycle = event->current_cycle;
    new_event->execute_cycles = 0;
    new_event->start_cycle = event->current_cycle;
    new_event->pc_addr = next_pc;
    core->sched->enq(new_event);
}

NextPCGen::NextPCGen(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}