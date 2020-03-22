/*  Yao-arch Lab1
    Author: Yiwei Li (liyw19@tsinghua.edu.cn)
    Based on spim, a MIPS functional emulator.
    We added several machine model to evaluate code performance by estimating running time on those machine model.

*/

#include <iostream>
#include <cstdlib>
#include <queue>
#include "inst.h"
using namespace std;


extern queue<instruction> inst_history_window;
struct TimingModel{
    unsigned long long estimated_cycle_br;
    unsigned long long estimated_cycle_mem;
    unsigned long long estimated_cycle_reg;
    unsigned long long num_inst_br;
    unsigned long long num_inst_mem;
    unsigned long long num_inst_reg;
};
extern TimingModel multicycle_tm, pipeline_tm, pipelinebypass_tm;

extern void hw_enq(instruction inst);
extern void update_latency_multicyclecore(instruction * , TimingModel &);
extern void update_latency_pipeline(instruction *, TimingModel &, bool raw_bypass_enable = false);
extern void reset_stats();
extern void print_stats(TimingModel);

const unsigned int MAX_WINDOW = 5; // enough for 5-stage core
const unsigned int CYCLE_MEM_ACCESS = 150;
const unsigned int CYCLE_MULTI = 19; // assume a 2-bit Booth encoding: 15 addtions with rest 4 stages
const unsigned int CYCLE_DIV = 36; // assume a divider completed by 32 right shifting with rest 4 stages