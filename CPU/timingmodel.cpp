#include <cstdlib>
#include "timingmodel.h"

/** Known bugs
 *
 *  1. Finish calculate only after all stage has no event
 */

void StageIF::init(mem_addr initial_pc, StageID * id_stage, TimingModel *model) {
    this->IN_next_pc = this->REG_cur_pc = initial_pc;
    this->me_node = IF_Stage;
    this->id_interface = id_stage;
    this->model = model;
}

bool StageIF::issue(uint64_t cur_cycle) {
    // I think general steps are as follows:
    // Cycle can be used to check correctness, as well as improving performance while long memory access latency.
    if (cur_cycle < this->avail_cycle) {
        // This may occur frequently due to stall
        //err.log("IF: cur_cycle less than avail_cycle");
        //abort();
        return true;
    }
    this->avail_cycle = cur_cycle + c_if_latency;
    if (this->avail_cycle >= id_interface->avail_cycle)
        this->OUT_valid = true;
    else
        this->OUT_valid = false;
    if (!this->OUT_valid) return true; // No need to verify in_valid here

    // Simulate all [wiring] operations, including input wires of next stage.
    id_interface->IN_inst = read_mem_inst(this->REG_cur_pc);
    id_interface->IN_DEBUG_pc = this->REG_cur_pc;
    id_interface->IN_valid = true;
    deb.log("%d Fetch %x", cur_cycle, this->REG_cur_pc);
    // Update inner registers.
    mem_addr next_jump_pc = 0x0;
    if (this->recvSignal(ID_Stage, "next_pc", next_jump_pc)) {
        this->IN_next_pc = next_jump_pc;
    } else {
        this->IN_next_pc += BYTES_PER_WORD;
    }

    // TODO: or, we put all register assignment to timing_core.tick(). And all wire calculations to timing_core.calculate()
    this->REG_cur_pc = this->IN_next_pc;

    this->IN_valid = false;

    static int fetched_inst = 0;
    fetched_inst++;
    return fetched_inst < 5; // Only fetch 5 instructions
}

void StageID::init(StageEXE * exe_stage, StageIF * if_stage, TimingModel *model) {
    this->exe_interface = exe_stage;
    this->if_interface = if_stage;
    this->model = model;
    this->me_node = ID_Stage;
}

void StageID::issue(uint64_t cur_cycle) {
    if (cur_cycle < this->avail_cycle) {
        //err.log("ID: cur_cycle less than avail_cycle");
        //abort();
        return;
    }
    this->avail_cycle = cur_cycle + c_id_latency;
    if (this->avail_cycle >= exe_interface->avail_cycle)
        this->OUT_valid = true;
    else
        this->OUT_valid = false;
    if (!this->IN_valid | !this->OUT_valid) return;

    exe_interface->IN_opcode = OPCODE(this->IN_inst);
    exe_interface->IN_rs = RS(this->IN_inst);
    exe_interface->IN_rd = RD(this->IN_inst);
    exe_interface->IN_rt = RT(this->IN_inst);
    exe_interface->IN_imm = IMM(this->IN_inst);
    exe_interface->IN_DEBUG_pc = this->IN_DEBUG_pc;
    exe_interface->IN_valid = true;
    deb.log("%d Decode %x", cur_cycle, this->IN_DEBUG_pc);

    this->IN_valid = false;
    // exe_interface->REG_DEBUG_pc = this->IN_DEBUG_pc;

    static int decoded_inst = 0;
    decoded_inst++;
    if (decoded_inst == 2) {
        deb.log("assume a jump at pc 0x%x, to address 0x%x", this->IN_DEBUG_pc, 0x400080);
        // fetcher_interface->IN_next_pc = 0x400080;
        this->sendSignal(IF_Stage, "next_pc", 0x400080);
    }
}

void StageEXE::init(StageMEM * mem_stage, TimingModel *model) {
    this->mem_interface = mem_stage;
    this->model = model;
}

void StageEXE::issue(uint64_t cur_cycle) {
    if (cur_cycle < this->avail_cycle) {
        //err.log("EXE: cur_cycle less than avail_cycle");
        //abort();
        return;
    }
    this->avail_cycle = cur_cycle + c_exe_latency;
    if (this->avail_cycle >= mem_interface->avail_cycle)
        this->OUT_valid = true;
    else
        this->OUT_valid = false;
    if (!this->IN_valid | !this->OUT_valid) return;

    mem_interface->IN_DEBUG_pc = this->IN_DEBUG_pc;
    mem_interface->IN_addr = 0x0;
    mem_interface->IN_valid = true;

    this->IN_valid = false;

    deb.log("%d Execute %x", cur_cycle, this->IN_DEBUG_pc);
}

void StageMEM::init(StageWB * wb_stage, TimingModel *model) {
    this->wb_interface = wb_stage;
    this->model = model;
}

void StageMEM::issue(uint64_t cur_cycle) {
    if (cur_cycle < this->avail_cycle) {
        //err.log("MEM: cur_cycle less than avail_cycle");
        //abort();
        return;
    }
    this->avail_cycle = cur_cycle + 2;
    // Notice! no mem for now
    if (this->avail_cycle >= wb_interface->avail_cycle)
        this->OUT_valid = true;
    else
        this->OUT_valid = false;
    if (!this->IN_valid | !this->OUT_valid) return;

    wb_interface->IN_DEBUG_pc = this->IN_DEBUG_pc;
    wb_interface->IN_wb_reg_id = 0x0;
    wb_interface->IN_wb_reg_value = 0x0;
    wb_interface->IN_valid = true;

    this->IN_valid = false;

    deb.log("%d Memory %x", cur_cycle, this->IN_DEBUG_pc);
}

void StageWB::init(TimingModel *model) {
    this->model = model;
}

void StageWB::issue(uint64_t cur_cycle) {
    if (cur_cycle < this->avail_cycle) {
        //err.log("WB: cur_cycle less than avail_cycle");
        //abort();
        return;
    }
    this->avail_cycle = cur_cycle + c_wb_latency;

    if (!this->IN_valid) return;

    this->IN_valid = false;

    deb.log("%d Writeback %x", cur_cycle, this->IN_DEBUG_pc);
}

void TimingModel::calculate() {
    // This part is for Simulation. Students do not need to completely understand it. It is also
    // better to be put in another file for readability.
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
    if_stage->init(initial_pc, id_stage, this);
    id_stage->init(exe_stage, if_stage, this);
    exe_stage->init(mem_stage, this);
    mem_stage->init(wb_stage, this);
    wb_stage->init(this);

    reg = new TimingRegister();
    mailboxes[ID_Stage][IF_Stage] = new MailBox();
}
