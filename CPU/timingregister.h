#ifndef TIMINGREGISTER_H
#define TIMINGREGISTER_H

#include "reg.h"
#include "inst.h"
#include "mem.h"
#include "logger.h"

class TimingRegister {
    public:
        TimingRegister(){
            reset();
        }

        reg_word reg[R_LENGTH];
        int reg_used[R_LENGTH];
        // Use the above to handle raw-dependency
        
        void load(int id, reg_word& content) {
            content = reg[id];
            deb.log("REG: load, id: %d, content %x", id, content);
        }

        void store(int id, const reg_word& content) {
            if ( id > 0 && id < R_LENGTH) {
                reg[id] = content; 
                deb.log("REG: store, id: %d, content %x", id, content);
            }
        }

        void reset(){
            for ( int id = 0; id < R_LENGTH; ++id ) {
                reg[id] = 0;
            } 
            out.log("REG: reset register");
        }
};

#endif //TIMINGREGISTER_H
