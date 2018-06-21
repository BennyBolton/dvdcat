#include "log.h"

#include <stdarg.h>
#include <stdio.h>



static log_level_t log_level = LOG_LEVEL_INFO;


void set_log_level(log_level_t level) {
    log_level = level;
}


void log_debug(const char* fmt, ...) {
    va_list args;

    if (log_level > LOG_LEVEL_DEBUG) return;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}


void log_info(const char* fmt, ...) {
    va_list args;

    if (log_level > LOG_LEVEL_INFO) return;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}


void log_warn(const char* fmt, ...) {
    va_list args;

    if (log_level > LOG_LEVEL_WARN) return;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}


void log_error(const char* fmt, ...) {
    va_list args;

    if (log_level > LOG_LEVEL_ERROR) return;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

