#include <cstdlib>
#include "timingmodel.h"

void StageIF::init(mem_addr initial_pc, StageID * id_stage, TimingModel *model) {
    this->IN_next_pc = this->REG_cur_pc = initial_pc;
    this->decoder_interface = id_stage;
    this->model = model;
}

bool StageIF::issue(uint64_t cur_cycle) {
    // I think general steps are as follows:
    // Cycle can be used to check correctness, as well as improving performance while long memory access latency.
    if (cur_cycle < this->avail_cycle) {
        err.log("IF: cur_cycle less than avail_cycle"); 
        abort();
    }
    this->avail_cycle = cur_cycle + c_if_latency;
    // Simulate all [wiring] operations, including input wires of next stage.
    decoder_interface->IN_inst = read_mem_inst(this->REG_cur_pc);
    decoder_interface->IN_DEBUG_pc = this->REG_cur_pc;
    decoder_interface->IN_valid = true;
    deb.log("%d Fetch %x\n", cur_cycle, this->REG_cur_pc);
    // Update inner registers.
    this->IN_next_pc += BYTES_PER_WORD;
    this->REG_cur_pc = this->IN_next_pc;

    static int fetched_inst = 0;
    fetched_inst++;
    return fetched_inst < 5; // Only fetch 5 instructions
}

void StageID::init(StageEXE * exe_stage, StageIF * if_stage, TimingModel *model) {
    this->executor_interface = exe_stage;
    this->fetcher_interface = if_stage;
    this->model = model;
}

void StageID::issue(uint64_t cur_cycle) {
    if (cur_cycle < this->avail_cycle) {
        err.log("ID: cur_cycle less than avail_cycle");
        abort();
    }
    this->avail_cycle = cur_cycle + c_id_latency;

    if (!this->IN_valid) return;
    executor_interface->IN_opcode = OPCODE(this->IN_inst);
    executor_interface->IN_rs = RS(this->IN_inst);
    executor_interface->IN_rd = RD(this->IN_inst);
    executor_interface->IN_rt = RT(this->IN_inst);
    executor_interface->IN_imm = IMM(this->IN_inst);
    executor_interface->IN_DEBUG_pc = this->IN_DEBUG_pc;
    executor_interface->IN_valid = true;
    deb.log("%d Decode %x\n", cur_cycle, this->IN_DEBUG_pc);

    static int decoded_inst = 0;
    decoded_inst++;
    if (decoded_inst == 2) {
        deb.log("assume a jump at pc 0x%x, to address 0x%x\n", this->IN_DEBUG_pc, 0x400080);
        fetcher_interface->IN_next_pc = 0x400080;
    }
}

void StageEXE::init(StageMEM * mem_stage, TimingModel *model) {
    this->mem_interface = mem_stage;
    this->model = model;
}

void StageEXE::issue(uint64_t cur_cycle) {
    if (cur_cycle < this->avail_cycle) {
        err.log("EXE: cur_cycle less than avail_cycle");
        abort();
    }
    this->avail_cycle = cur_cycle + c_exe_latency;

    if (!this->IN_valid) return;
    mem_interface->IN_DEBUG_pc = this->IN_DEBUG_pc;
    mem_interface->IN_addr = 0x0;
    mem_interface->IN_valid = true;
    deb.log("%d Execute %x\n", cur_cycle, this->IN_DEBUG_pc);
}

void StageMEM::init(StageWB * wb_stage, TimingModel *model) {
    this->wb_interface = wb_stage;
    this->model = model;
}

void StageMEM::issue(uint64_t cur_cycle) {
    if (cur_cycle < this->avail_cycle) {
        err.log("MEM: cur_cycle less than avail_cycle");
        abort();
    }
    this->avail_cycle = cur_cycle + c_no_mem_latency;

    if (!this->IN_valid) return;
    wb_interface->IN_DEBUG_pc = this->IN_DEBUG_pc;
    wb_interface->IN_wb_reg_id = 0x0;
    wb_interface->IN_wb_reg_value = 0x0;
    wb_interface->IN_valid = true;
    deb.log("%d Memory %x\n", cur_cycle, this->IN_DEBUG_pc);
}

void StageWB::init(TimingModel *model) {
    this->model = model;
}

void StageWB::issue(uint64_t cur_cycle) {
    if (cur_cycle < this->avail_cycle) {
        err.log("WB: cur_cycle less than avail_cycle");
        abort();
    }
    this->avail_cycle = cur_cycle + c_wb_latency;

    if (!this->IN_valid) return;
    deb.log("%d Writeback %x\n", cur_cycle, this->IN_DEBUG_pc);
}

void TimingModel::calculate() {
    // You can change this only if you are playing
    // Or you are implementing OoO
    wb_stage->issue(cur_cycle);
    mem_stage->issue(cur_cycle);
    exe_stage->issue(cur_cycle);
    id_stage->issue(cur_cycle);
    has_next_cycle = if_stage->issue(cur_cycle);
}

void TimingModel::init(mem_addr initial_pc) {
    has_next_cycle = true;
    cur_cycle = 0;
    if_stage = new StageIF();
    id_stage = new StageID();
    exe_stage = new StageEXE();
    mem_stage = new StageMEM();
    wb_stage = new StageWB();
    if_stage->init(initial_pc, id_stage,this);
    id_stage->init(exe_stage, if_stage,this);
    exe_stage->init(mem_stage,this);
    mem_stage->init(wb_stage,this);
    wb_stage->init(this);

    reg = new TimingRegister();
}
