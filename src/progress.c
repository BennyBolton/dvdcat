#include "progress.h"

#include "log.h"



#ifndef DVDCAT_PROGRESS_DELAY
#define DVDCAT_PROGRESS_DELAY 500
#endif

#ifndef DVDCAT_ETA_DELAY
#define DVDCAT_ETA_DELAY 2000
#endif


static long time_diff(const struct timespec* then, const struct timespec* now) {
    long diff = (long)(now->tv_sec - then->tv_sec) * 1000L;
    return diff + (now->tv_nsec - then->tv_nsec) / 1000000L;
}


static void output_progress(progress_t* progress) {
    long diff = time_diff(&progress->start_time, &progress->last_update);
    int speed = diff > 0 ? (int)(progress->progress / diff) : 0;
    log_info("Progress: %3d%%, %d MB / %d MB, %d KB/s, Elapsed: %02d:%02d",
        100 * progress->progress / progress->total,
        progress->progress / 1000000L, progress->total / 1000000L, speed,
        (diff / 60000L) % 60L, (diff / 1000L) % 60L);
    if (progress->progress > 0 && diff > DVDCAT_ETA_DELAY) {
        diff = diff * ((progress->total - progress->progress) / (float)progress->progress);
        log_info(", Remaining: %02d:%02d\r",
            (diff / 60000L) % 60L, (diff / 1000L) % 60L);
    } else {
        log_info("\r");
    }
}


void progress_start(progress_t* progress, long total) {
    clock_gettime(CLOCK_MONOTONIC, &progress->start_time);
    progress->last_update = progress->start_time;
    progress->progress = 0;
    progress->total = total;
    output_progress(progress);
}


void progress_update(progress_t* progress, long ammount) {
    struct timespec now;

    progress->progress += ammount;
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (time_diff(&progress->last_update, &now) >= DVDCAT_PROGRESS_DELAY) {
        progress->last_update = now;
        output_progress(progress);
    }
}


void progress_finish(progress_t* progress) {
    clock_gettime(CLOCK_MONOTONIC, &progress->last_update);
    progress->progress = progress->total;
    output_progress(progress);
    log_info("\n");
}