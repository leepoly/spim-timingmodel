/*  Yao-arch Lab1
    Author: Yiwei Li (liyw19@tsinghua.edu.cn)
    Based on spim, a MIPS functional emulator.
    We added several machine model to evaluate code performance by estimating running time on those machine model.

*/

#include "statistics.h"
#include "parser_yacc.h"

queue<instruction> inst_history_window;
TimingModel multicycle_tm, pipeline_tm, pipelinebypass_tm;

void hw_enq(instruction inst) {
    if (inst_history_window.size() == MAX_WINDOW) {
        inst_history_window.pop();
    }
    inst_history_window.push(inst);
}

void update_latency_multicyclecore(instruction * inst, TimingModel & tm) {
    //under this circumstance, the core is essentially a State Machine. Latency of an inst on a single core = BaseCycle(inst)
    if (opcode_is_load_store(inst->opcode)) {
        tm.estimated_cycle_mem += CYCLE_MEM_ACCESS; // if inst is a LD/ST, basecycle is 100 (random would be better)
        tm.num_inst_mem ++;
    }
    else if (opcode_is_branch(inst->opcode)) {
        tm.estimated_cycle_br += 3; // if inst is a conditional branch: FE -> DE -> EXE
        tm.num_inst_br ++;
    }
    else if (opcode_is_jump(inst->opcode)) {
        tm.estimated_cycle_br += 2; // if inst is a unconditional branch: FE -> DE
        tm.num_inst_br ++;
    }
    else if (inst->opcode == Y_NOP_POP) {
        tm.estimated_cycle_reg += 2; // if inst is a NOP: FE -> DE
        tm.num_inst_reg ++;
    }
    else if (inst->opcode == Y_MULT_OP || inst->opcode == Y_MULTU_OP) {
        tm.estimated_cycle_reg += CYCLE_MULTI; // if inst is a multiplication, more cycle for complex execution
        tm.num_inst_reg ++;
    }
    else if (inst->opcode == Y_DIV_OP || inst->opcode == Y_DIVU_OP) {
        tm.estimated_cycle_reg += CYCLE_DIV; // if inst is a division, more cycle for complex execution
        tm.num_inst_reg ++;
    }
    else {
        tm.estimated_cycle_reg += 5; // FE -> DE -> EXE -> MEM -> WB
        tm.num_inst_reg ++;
    }
}

void update_latency_pipeline(instruction * inst, TimingModel & tm, bool raw_bypass_enable) {
    // Latency of an inst on nobypass 5-stage core = BaseCycle(inst) + control_stall(tail) + raw_stall(inst, queue)
    // if inst is a LD/ST, basecycle is CYCLE_MEM_ACCESS (random would be better), otherwise 1 (1-way issue)
    // if tail inst is a conditional branch, this stall penalty would be 2, for a unconditional branch, stall would be 1, otherwise no stall
    queue<instruction> tmp_queue(inst_history_window);
    int cnt = 5;
    int basecycle = 1;
    if (opcode_is_load_store(inst->opcode))
        basecycle = CYCLE_MEM_ACCESS;
    else if (inst->opcode == Y_MULT_OP || inst->opcode == Y_MULTU_OP)
        basecycle = CYCLE_MULTI;
    else if (inst->opcode == Y_DIV_OP || inst->opcode == Y_DIVU_OP)
        basecycle = CYCLE_DIV;

    int raw_stall = 0;
    int control_stall = 0;
    while (cnt > 0 && tmp_queue.size() > 0) {
        instruction cur_inst = tmp_queue.front();
        if (cnt < 3) {
            if ((RD(&cur_inst) == RS(inst) && RS(inst) > 0) || (RD(&cur_inst) == RT(inst) && RT(inst) > 0)) {
                raw_stall = 3 - cnt; // stalled due to data read-after-write
            }
        }

        if (tmp_queue.size() == 1) {
            if (opcode_is_branch(inst->opcode)) {
                control_stall = 2; // conditional jump penalty: FE, DE
            } else if (opcode_is_jump(inst->opcode)) {
                control_stall = 1; // unconditional jump penalty: FE
            }
        }

        cnt --; tmp_queue.pop();
    }
    int inst_cycle = basecycle + control_stall;
    if (!raw_bypass_enable) inst_cycle += raw_stall;

    if (opcode_is_load_store(inst->opcode)) {
        tm.estimated_cycle_mem += inst_cycle;
        tm.num_inst_mem ++;
    }
    else if (opcode_is_branch(inst->opcode)) {
        tm.estimated_cycle_br += inst_cycle;
        tm.num_inst_br ++;
    }
    else if (opcode_is_jump(inst->opcode)) {
        tm.estimated_cycle_br += inst_cycle;
        tm.num_inst_br ++;
    }
    else {
        tm.estimated_cycle_reg += inst_cycle;
        tm.num_inst_reg ++;
    }
}

void print_stats(TimingModel tm) {
    cout << "statistics of instructions" << endl;
    cout << "branch inst.\t\t#" << tm.num_inst_br << "\testimated cycle\t" << tm.estimated_cycle_br << endl;
    cout << "memory inst.\t\t#" << tm.num_inst_mem << "\testimated cycle\t" << tm.estimated_cycle_mem << endl;
    cout << "register inst.\t\t#" << tm.num_inst_reg << "\testimated cycle\t" << tm.estimated_cycle_reg << endl;
    cout << "total\t\t\t#" << tm.num_inst_br + tm.num_inst_mem + tm.num_inst_reg << "\testimated cycle\t" << tm.estimated_cycle_br + tm.estimated_cycle_mem + tm.estimated_cycle_reg << endl << endl;
}

void reset_stats() {
    pipeline_tm.estimated_cycle_br = 0;
    pipeline_tm.estimated_cycle_mem = 0;
    pipeline_tm.estimated_cycle_reg = 0;
    pipeline_tm.num_inst_br = 0;
    pipeline_tm.num_inst_mem = 0;
    pipeline_tm.num_inst_reg = 0;
    // memset(&multicycle_tm, 0, sizeof(multicycle_tm));
    // memset(&pipeline_tm, 0, sizeof(pipeline_tm));
    // memset(&pipelinebypass_tm, 0, sizeof(pipelinebypass_tm));
}
