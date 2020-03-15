#include "assert.h"
#include "syscall.h"
#include "timing_decoder.h"
#include "timing_core.h"

void TimingDecoder::Issue(TimingEvent *event) {
    event->opcode = OPCODE(event->inst);
    event->rs = RS(event->inst);
    event->rd = RD(event->inst);
    event->rt = RT(event->inst);
    event->imm = IMM(event->inst);
    event->target = TARGET(event->inst);

    event->inst_is_I_type = false;
    event->inst_is_J_type = false;
    event->inst_is_R_type = false;
    event->inst_is_load = false;

    switch (event->opcode) {
        case Y_JAL_OP:
            event->inst_is_I_type = true; // trick: we treat JAL as an I-type inst. We put instr_idx [25..0] into an extended 'imm' field and pass it to ALU.
            event->rt = 31;
            event->rs = 0;
            event->extended_imm = event->pc_addr + BYTES_PER_WORD;
            event->imm_is_32b = true;
            event->alu_op = ALUOP_ADD;
            break;
        case Y_ADDIU_OP:
            event->inst_is_I_type = true;
            event->imm_is_sign_extend = true;
            event->alu_op = ALUOP_ADD;
            break;
        case Y_LW_OP:
            event->inst_is_load = true;
            break;
        case Y_SYSCALL_OP:
            event->inst_is_syscall = true;
            break;
    }

    bool spim_continuable = true;
    if (event->inst_is_I_type) {
        event->reg_wb_id = event->rt;
    } else if (event->inst_is_load) {
        event->reg_wb_id = event->rt;
    } else if (event->inst_is_syscall) {
        reg_word val_rs = 0x0;
        core->regfile->Load(REG_RES, val_rs);
        if (val_rs == EXIT_SYSCALL) {
            // We reset this variable to prevent PC fetching after last instruction.
            spim_continuable = false;
        }
    }

    assert(event->state == TES_WaitDecoder);
    available_cycle = MAX(event->current_cycle, available_cycle) + c_decode_latency;
    event->state = TES_WaitExecutor;

    // This fetches the next pc
    if (spim_continuable)
        core->next_pc_gen->GenerateNextPC(event);

    event->execute_cycles += c_decode_latency;
    event->current_cycle = available_cycle;
}

TimingDecoder::TimingDecoder(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}