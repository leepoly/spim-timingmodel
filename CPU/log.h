#ifndef DEF_H
#define DEF_H

#include <assert.h>
#include <stdarg.h>

FILE * logFdOut = stdout;

#define info(...) \
{ \
    fprintf(stderr, "[info] "); \
    fprintf(logFdOut, __VA_ARGS__); \
    fprintf(logFdOut, "\n"); \
    fflush(logFdOut); \
}

#define assert_msg(...) \
{ \
    fprintf(stderr, "[error] "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    fflush(stderr); \
    assert(false); \
}

#endif  // DEF_H