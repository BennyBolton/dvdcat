#ifndef __DVDCAT_PROGRESS_H__
#define __DVDCAT_PROGRESS_H__


#include <time.h>


typedef struct _progress_t progress_t;

struct _progress_t {
    long progress, total;
    struct timespec start_time, last_update;
    int line_length;
};


void progress_start(progress_t* progress, long total);
void progress_update(progress_t* progress, long ammount);
void progress_finish(progress_t* progress);


#endif // __DVDCAT_PROGRESS_H__