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
extern unsigned int estimated_time;

extern void hw_enq(instruction inst); 
extern int get_latency_singlecyclecore (instruction * );

const unsigned int MAX_WINDOW = 5; // enough for 5-stage core
const unsigned int CYCLE_MEM_ACCESS = 100;
