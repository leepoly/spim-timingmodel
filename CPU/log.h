#ifndef DEF_H
#define DEF_H

#include <assert.h>
#include <stdarg.h>

// FILE * logFdOut = stdout;

#define info(...) \
{ \
    fprintf(stderr, "[info] "); \
    fprintf(stdout, __VA_ARGS__); \
    fprintf(stdout, "\n"); \
    fflush(stdout); \
}

#define assert_msg(...) \
{ \
    fprintf(stderr, "[error] "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    fflush(stderr); \
    assert(false); \
}

#define assert_msg_ifnot(expr, ...) \
{ \
    if (!(expr)) {\
        fprintf(stderr, "[error] "); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        fflush(stderr); \
        assert(false); \
    }\
}

#endif  // DEF_H