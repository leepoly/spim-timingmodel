#ifndef TIMINGDECODER_H
#define TIMINGDECODER_H

#include "timingmodel.h"

#include "inst.h"
#include "parser_yacc.h"

#define BRANCH_INST_GENPC(TEST, TARGET, NULLIFY, PC)   \
    {                                                  \
        if (TEST)                                      \
        {                                              \
            mem_addr target = (TARGET);                \
            /* we do not consider delay slot */        \
            JUMP_INST_GENPC(target, PC)                \
        }                                              \
        else if (NULLIFY)                              \
        {                                              \
            /* If test fails and nullify bit set, skip \
           instruction in delay slot. */               \
            PC += BYTES_PER_WORD;                      \
        }                                              \
    }

#define JUMP_INST_GENPC(TARGET, PC)                    \
    {                                                  \
        /* we do not consider delay slot */            \
        /* -4 since PC is bumped after this inst */    \
        PC = (TARGET);                                 \
    }

class TimingCore;

class TimingDecoder : public TimingComponent {
  public:
    TimingCore * core = nullptr;
    TimingDecoder(TimingComponent * _parent);
    void Issue(TimingEvent * event);
};

#endif // TIMINGDECODER_H