#ifndef ALU_H
#define ALU_H

#include "reg.h"

enum ALUOp {
    // You should append more ALUOp, and realize your different ALU operations.
    ALUOP_ADD
};

class ALU
{
public:
    reg_word Execute(reg_word a, reg_word b, int ALUOp) {
        switch (ALUOp) {
            case ALUOP_ADD:
                return a + b;
            default:
                return 0x0;
        }
    }
};

#endif // ALU_H
