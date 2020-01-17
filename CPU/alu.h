#ifndef ALU_H
#define ALU_H

#include "reg.h"

enum ALUOPs {
    ALUOP_ADD
};

class ALU
{
public:
    reg_word Execute(reg_word a, reg_word b, int alu_op) {
        switch (alu_op) {
            case ALUOP_ADD:
                return a + b;
            default:
                return 0;
        }
    }
};

#endif // ALU_H
