#ifndef TIMINGMODEL_H
#define TIMINGMODEL_H

#include <cstdint>
#include "spim.h"
#include "reg.h"
#include "inst.h"
#include "mem.h"

#include "logger.h"

#include "lab_defs.h"
#include "timingregister.h"

#include <queue>
#include <typeinfo>
#include <iostream>


/** @Zenithal
 Since we have discussed we should build a unified and hardware-oriented timing
 model, I am here to present the framework.

 Since the model is unifed, we do not use the event and component point of
 view, instead we treat each stage as a function and the execution order of 
 these functions is fixed (For simple stall and full-forward), meanwhile 
 if someone wants to implement out-of-order execution, they may change the 
 order and alter other components themself.

 Since the model is hardware-oriented, instead of event, which is rather high
 level abstraction, we use class variables to simulate registers in hardware.

 For those interested in the previous framework, you may check the older commit,
 namely the tag:lab2-lecstyle-v1.1 to understand the `event` and `componet`
 talked above.
 **/

/* NOTE (Yiwei Li):
Suggestions on the code style:
1. Avoiding extra spaces
2. 

*/

class StageIF;
class StageID;
class StageEXE;
class StageMEM;
class StageWB;

class TimingModel {
private:
    uint64_t cur_cycle;
    StageIF *if_stage;
    StageID *id_stage;
    StageEXE *exe_stage;
    StageMEM *mem_stage;
    StageWB *wb_stage;
public:
    bool has_next_cycle;

    void init(mem_addr initial_pc);

    // Functions below are all implemented in cpp,
    // and some of them is partially implemented.
    void calculate();

    void tick() {
        cur_cycle++;
        deb.log("Now at cycle %d\n", cur_cycle);
    }

    TimingRegister *reg;
};

class Stage {
public:
    // Only Calculate under valid Input
    bool IN_valid = false; // IN_valid is for initial a few steps
    bool OUT_valid = false; // OUT_valid is for big mem latency
    uint64_t avail_cycle = 0;
    TimingModel *model;
};

class StageIF : public Stage {
public:
    // Input IO
    mem_addr IN_next_pc;
    // Method
    bool issue(uint64_t cur_cycle);
    void init(mem_addr initial_pc, StageID * id_stage, TimingModel *model);
private:
    // Inner Registers
    mem_addr REG_cur_pc;
    // pointer to the next stage to modify its input variables
    StageID * id_interface;
};

class StageID : public Stage {
public:
    // Input IO
    instruction * IN_inst;
    mem_addr IN_DEBUG_pc;
    // Method
    void issue(uint64_t cur_cycle);
    void init(StageEXE * exe_stage, StageIF * if_stage, TimingModel *model);
private:
    // Inner Registers
    // pointer to the next stage to modify its input variables
    StageEXE * exe_interface;
    // TODO: normally we do not allow out-of-order stage communication. Mailbox is used for safe signal propagation.
    StageIF * if_interface;
};

class StageEXE : public Stage {
public:
    int IN_opcode;
    int IN_rd, IN_rs, IN_rt;
    reg_word IN_imm;
    mem_addr IN_DEBUG_pc;

    void issue(uint64_t cur_cycle);
    void init(StageMEM * mem_stage, TimingModel *model);
private:
    StageMEM * mem_interface;
};

class StageMEM : public Stage {
public:
    mem_addr IN_addr;
    mem_addr IN_DEBUG_pc;

    void issue(uint64_t cur_cycle);
    void init(StageWB * wb_stage, TimingModel *model);
private:
    StageWB * wb_interface;
};

class StageWB : public Stage {
public:
    uint32_t IN_wb_reg_id;
    reg_word IN_wb_reg_value;
    mem_addr IN_DEBUG_pc;

    void issue(uint64_t cur_cycle);
    void init(TimingModel *model);
private:
};

#endif  // TIMINGMODEL_H
