#include "assert.h"
#include "timing_lsu.h"
#include "timing_core.h"

void TimingLSU::Issue(TimingEvent * event) {
    mem_word memory_read_data = 0x0;
    if (event->MemRead) {
        // The following is only correct for LW. You may add more control signals for implementing LHU, LBU, ...
        if (core->cache_enabled) {
            core->cache->Access(event->alu_result, false, memory_read_data, event->pc_addr, event->current_cycle, 4);
            reg_word correct_result;
            LOAD_INST(&correct_result,
                        read_mem_word(event->alu_result),
                        0xffffffff);
            printf("addr:%x your data: %x\tcorrect data:%x\n", event->alu_result, memory_read_data, correct_result);
            assert(memory_read_data == correct_result);
        } else {
            LOAD_INST(&memory_read_data,
                        read_mem_word(event->alu_result),
                        0xffffffff);
            printf("addr:%x your data: %x\n", event->alu_result, memory_read_data);
        }
    }
    // assert_msg_ifnot(event->MemToReg != -1, "instruction %s has not been fully implemented", inst_to_string(event->pc_addr));
    if (event->MemToReg == 0x0)
        event->reg_wb_data = event->alu_result;
    else if (event->MemToReg == 0x1)
        event->reg_wb_data = memory_read_data;

    if (event->MemRead || event->MemWrite) {
        if (core->cache_enabled) {
            available_cycle = MAX(event->current_cycle, available_cycle);  // available_cycle is updated by timing cache
        } else {
            available_cycle = MAX(event->current_cycle, available_cycle) + c_memory_access_latency;
            event->current_cycle = available_cycle;
            event->execute_cycles += c_memory_access_latency;
        }
    } else {
        // this stage still count 1 cycle even when no memory request
        available_cycle = MAX(event->current_cycle, available_cycle) + 1;
        event->current_cycle = available_cycle;
        event->execute_cycles += 1;
    }

    assert(event->state == TES_WaitLSU);
    event->state = TES_WaitROB;
}

TimingLSU::TimingLSU(TimingComponent * _parent) {
    available_cycle = 0;
    core = dynamic_cast<TimingCore *>(_parent);
}