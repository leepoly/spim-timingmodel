#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <stdarg.h>
#include <cassert>

class logger {
    public:
    logger(FILE* _fp) {
        fp = _fp;
        active = true;
    }

    int log(const char* str,...){
        if (active) {
            va_list args;
            va_start(args, str);
            int ret = vfprintf(fp, str, args);
            ret &= fprintf(fp, "\n");
            va_end(args);
            // fflush(fp);
            return ret;
        }
        return 0;
    }

    bool active;
    private:
    FILE* fp;
};

extern logger out, err, deb; // For stdout, stderr and debug

#endif //LOGGER_H