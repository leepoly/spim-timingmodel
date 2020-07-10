#ifndef LOG_H
#define LOG_H

#include <assert.h>
#include <stdarg.h>

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

#endif  // LOG_H