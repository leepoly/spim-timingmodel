/*  Yao-arch Lab1
    Author: Yiwei Li (liyw19@tsinghua.edu.cn)
    Based on spim, a MIPS functional emulator.
    We added several machine model to evaluate code performance by estimating running time on those machine model.

*/

#include "statistics.h"
#include "parser_yacc.h"

queue<instruction> inst_history_window;
unsigned int estimated_time = 0;

void hw_enq(instruction inst) {
    if (inst_history_window.size() == MAX_WINDOW) {
        inst_history_window.pop();
    }
    inst_history_window.push(inst);
}

int get_latency_singlecyclecore(instruction * inst) {
    //under this circumstance, the core is essentially a State Machine. Latency of an inst on a single core = BaseCycle(inst)
    if (opcode_is_load_store(inst->opcode)) return CYCLE_MEM_ACCESS; // if inst is a LD/ST, basecycle is 100 (random would be better)
    if (opcode_is_branch(inst->opcode)) return 3; // if inst is a conditional branch: FE -> DE -> EXE
    if (opcode_is_jump(inst->opcode)) return 2; // if inst is a conditional branch: FE -> DE
    if (inst->opcode == Y_NOP_POP) return 2; // if inst is a NOP: FE -> DE
    return 5; // FE -> DE -> EXE -> MEM -> WB
}

int get_latency_pipelinecore_nobypass(instruction * inst) {
    //Latency of an inst on nobypass 5-stage core = BaseCycle(inst) + control_stall(tail) + raw_stall(inst, queue)
    // if inst is a LD/ST, basecycle is 100 (random would be better), otherwise 1
    // if tail inst is a conditional branch, this stall penalty would be 2, for a unconditional branch, stall would be 1, otherwise no stall
  return -1;

}
