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
extern unsigned int estimated_cycle_br;
extern unsigned int estimated_cycle_mem;
extern unsigned int estimated_cycle_reg;
extern unsigned int num_inst_br;
extern unsigned int num_inst_mem;
extern unsigned int num_inst_reg;

extern void hw_enq(instruction inst);
extern void update_latency_multicyclecore (instruction * );
extern void update_latency_pipelinestall (instruction *);
extern void reset_stats();
extern void print_stats();

const unsigned int MAX_WINDOW = 5; // enough for 5-stage core
const unsigned int CYCLE_MEM_ACCESS = 100;
const unsigned int CYCLE_MULTI = 19; // assume a 2-bit Booth encoding: 15 addtions with rest 4 stages
const unsigned int CYCLE_DIV = 36; // assume a divider completed by 32 right shifting with rest 4 stages