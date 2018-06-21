#include "log.h"

#include <stdarg.h>
#include <stdio.h>



static log_level_t log_level = LOG_LEVEL_INFO;


void set_log_level(log_level_t level) {
    log_level = level;
}


int log_debug(const char* fmt, ...) {
    va_list args;
    int ret;

    if (log_level > LOG_LEVEL_DEBUG) return 0;

    va_start(args, fmt);
    ret = vfprintf(stderr, fmt, args);
    va_end(args);
    return ret;
}


int log_info(const char* fmt, ...) {
    va_list args;
    int ret;

    if (log_level > LOG_LEVEL_INFO) return 0;

    va_start(args, fmt);
    ret = vfprintf(stderr, fmt, args);
    va_end(args);
    return ret;
}


int log_warn(const char* fmt, ...) {
    va_list args;
    int ret;

    if (log_level > LOG_LEVEL_WARN) return 0;

    va_start(args, fmt);
    ret = vfprintf(stderr, fmt, args);
    va_end(args);
    return ret;
}


int log_error(const char* fmt, ...) {
    va_list args;
    int ret;

    if (log_level > LOG_LEVEL_ERROR) return 0;

    va_start(args, fmt);
    ret = vfprintf(stderr, fmt, args);
    va_end(args);
    return ret;
}

