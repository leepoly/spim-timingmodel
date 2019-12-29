/*  Yao-arch Lab1
    Author: Yiwei Li (liyw19@tsinghua.edu.cn)
    Based on spim, a MIPS functional emulator.
    We added several machine model to evaluate code performance by estimating running time on those machine model.

*/

#include "statistics.h"
#include "parser_yacc.h"

queue<instruction> inst_history_window;
unsigned int estimated_cycle_br;
unsigned int estimated_cycle_mem;
unsigned int estimated_cycle_reg;
unsigned int num_inst_br;
unsigned int num_inst_mem;
unsigned int num_inst_reg;

void hw_enq(instruction inst) {
    if (inst_history_window.size() == MAX_WINDOW) {
        inst_history_window.pop();
    }
    inst_history_window.push(inst);
}

void update_latency_multicyclecore(instruction * inst) {
    //under this circumstance, the core is essentially a State Machine. Latency of an inst on a single core = BaseCycle(inst)
    if (opcode_is_load_store(inst->opcode)) {
        estimated_cycle_mem += CYCLE_MEM_ACCESS; // if inst is a LD/ST, basecycle is 100 (random would be better)
        num_inst_mem ++;
    }
    else if (opcode_is_branch(inst->opcode)) {
        estimated_cycle_br += 3; // if inst is a conditional branch: FE -> DE -> EXE
        num_inst_br ++;
    }
    else if (opcode_is_jump(inst->opcode)) {
        estimated_cycle_br += 2; // if inst is a conditional branch: FE -> DE
        num_inst_br ++;
    }
    else if (inst->opcode == Y_NOP_POP) {
        estimated_cycle_reg += 2; // if inst is a NOP: FE -> DE
        num_inst_reg ++;
    }
    else {
        estimated_cycle_reg += 5; // FE -> DE -> EXE -> MEM -> WB
        num_inst_reg ++;
    }
}

void update_latency_pipelinestall(instruction * inst) {
    // Latency of an inst on nobypass 5-stage core = BaseCycle(inst) + control_stall(tail) + raw_stall(inst, queue)
    // if inst is a LD/ST, basecycle is 100 (random would be better), otherwise 1
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
        if (cnt < 5) {
            if (RT(&cur_inst) == RS(inst) || RT(&cur_inst) == RD(inst)) {
                raw_stall = 5-cnt;
            }
        }

        if (tmp_queue.size() == 1) {
            if (opcode_is_branch(inst->opcode)) {
                control_stall = 2; //conditional jump penalty: FE, DE
            } else if (opcode_is_jump(inst->opcode)) {
                control_stall = 1; //unconditional jump penalty: FE
            }
        }

        cnt --; tmp_queue.pop();
    }
    int inst_cycle = basecycle + raw_stall + control_stall;

    if (opcode_is_load_store(inst->opcode)) {
        estimated_cycle_mem += inst_cycle;
        num_inst_mem ++;
    }
    else if (opcode_is_branch(inst->opcode)) {
        estimated_cycle_br += inst_cycle;
        num_inst_br ++;
    }
    else if (opcode_is_jump(inst->opcode)) {
        estimated_cycle_br += inst_cycle;
        num_inst_br ++;
    }
    else {
        estimated_cycle_reg += inst_cycle;
        num_inst_reg ++;
    }
}

void print_stats() {
    cout << "statistics of instructions" << endl;
    cout << "branch inst\t\t#" << num_inst_br << "\testimated cycle\t" << estimated_cycle_br << endl;
    cout << "memory inst\t\t#" << num_inst_mem << "\testimated cycle\t" << estimated_cycle_mem << endl;
    cout << "register inst\t\t#" << num_inst_reg << "\testimated cycle\t" << estimated_cycle_reg << endl;
    cout << "total\t\t\t#" << num_inst_br+num_inst_mem+num_inst_reg << "\testimated cycle\t" << estimated_cycle_br+estimated_cycle_mem+estimated_cycle_reg << endl << endl;
}

void reset_stats() {
    estimated_cycle_br = estimated_cycle_mem = estimated_cycle_reg = 0;
    num_inst_br = num_inst_mem = num_inst_reg = 0;
}
