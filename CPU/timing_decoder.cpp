#include "syscall.h"
#include "timing_decoder.h"
#include "timing_core.h"
#include "log.h"

void TimingDecoder::Issue(TimingEvent *event) {
    short opcode = OPCODE(event->inst);
    unsigned char rs = RS(event->inst);
    unsigned char rd = RD(event->inst);
    unsigned char rt = RT(event->inst);
    short imm = IMM(event->inst);
    mem_addr target = TARGET(event->inst);

    // Here are local control signals. You should specify in your code what their values mean, as I did.
    // ALUSrc: -1 -> invalid, 0/1 -> following lecture slides, 2 -> alu_src_b = PC+4, for JAL
    int ALUSrc = -1;
    // RegDst: -1 -> invalid, 0/1 -> following lecture slides, 2-> wb_id = $ra, for JAL
    int RegDst = -1;
    // Branch follows lecture slides
    bool Branch = false;
    // Jump follows lecture slides
    bool Jump = false;

    switch (opcode) {
        case Y_JAL_OP:
            rs = 0;  // trick: we force rs to 0. This leads ALU to calculate (0 + (PC+4))
            event->MemRead = false;
            event->MemToReg = 0x0;
            event->RegWrite = true;
            ALUSrc = 0x2;  // Look! We just extended and defined our own control signal different from professor's material. This is the fascinating point of processor design: everyone has their own design.
            RegDst = 0x2;
            event->ALUOp = ALUOP_ADD;
            Jump = true;
            break;
        case Y_ADDIU_OP:
            event->MemRead = false;
            event->MemToReg = 0x0;
            event->RegWrite = true;
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_ADD;
            break;
        case Y_LW_OP:
            event->MemRead = true;
            event->MemToReg = 0x1;
            event->RegWrite = true;
            ALUSrc = 0x1;
            RegDst = 0x0;
            event->ALUOp = ALUOP_ADD;
            break;
        case Y_SLL_OP:
            // sll is not correctly implemented.
            // we only make sure NOP can be correctly emulated.
            ALUSrc = 0x0;
            RegDst = 0x0;
            event->MemToReg = 0x0;
            rt = 0;
            break;
        // Implement other instructions here
        case Y_SYSCALL_OP:
            event->inst_is_syscall = true;
            break;
    }

    // printf("\tdecoding addr:%p inst:%s\n", event->pc_addr, inst_to_string(event->pc_addr));
    bool keep_fetch_nextpc = true;
    core->regfile->Load(rs, event->alu_src_1);

    // assert_msg_ifnot((ALUSrc != -1), "instruction %s has not been fully implemented", inst_to_string(event->pc_addr));
    if (ALUSrc == 0x0)
        core->regfile->Load(rt, event->alu_src_2);
    else if (ALUSrc == 0x1)
        event->alu_src_2 = (short)imm;
    else if (ALUSrc == 0x2)
        event->alu_src_2 = event->pc_addr + 2 * BYTES_PER_WORD;

    // assert_msg_ifnot((RegDst != -1), "instruction %s has not been fully implemented", inst_to_string(event->pc_addr));
    if (RegDst == 0x0)
        event->reg_wb_id = rt;
    else if (RegDst == 0x1)
        event->reg_wb_id = rd;
    else if (RegDst == 0x2)
        event->reg_wb_id = 31;  // $ra

    if (event->inst_is_syscall) {
        reg_word val_rs = 0x0;
        core->regfile->Load(REG_RES, val_rs);
        if (val_rs == EXIT_SYSCALL) {
            // We reset this variable to prevent PC fetching after last instruction.
            keep_fetch_nextpc = false;
        }
    }

    assert(event->state == TES_WaitDecoder);
    available_cycle = MAX(event->current_cycle, available_cycle) + c_decode_latency;
    event->state = TES_WaitExecutor;

    // This fetches the next pc
    if (keep_fetch_nextpc)
        core->next_pc_gen->GenerateNextPC(event->pc_addr, imm, target, Branch, Jump, event->current_cycle);

    event->execute_cycles += c_decode_latency;
    event->current_cycle = available_cycle;
}

TimingDecoder::TimingDecoder(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}