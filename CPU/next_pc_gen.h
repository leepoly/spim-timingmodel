#ifndef NEXT_PC_GEN_H
#define NEXT_PC_GEN_H

#include "timingmodel.h"

class TimingCore;

// Although NextPCGen is a combinational logic we still use TimingComponent as our base class.
class NextPCGen : public TimingComponent {
public:
    TimingCore * core = nullptr;
    NextPCGen(TimingComponent * _parent);

    void GenerateInitialEvent(mem_addr initial_PC);

    void GenerateNextPC(mem_addr pc_addr, reg_word extended_imm, mem_addr target, bool branch, bool jump, ncycle_t current_cycle);

    void Issue(TimingEvent * event) {}
};

#endif // NEXT_PC_GEN_H