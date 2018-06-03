#ifndef __DVDCAT_LOG_H__
#define __DVDCAT_LOG_H__


typedef enum _log_level_t log_level_t;

enum _log_level_t {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
};


void set_log_level(log_level_t level);

void log_debug(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_warn(const char* fmt, ...);
void log_error(const char* fmt, ...);


#endif // __DVDCAT_LOG_H__