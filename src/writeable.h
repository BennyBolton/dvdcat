#ifndef __DVDCAT_WRITEABLE_H__
#define __DVDCAT_WRITEABLE_H__


#include <stdio.h>
#include <stdlib.h>



typedef struct _writeable_t writeable_t;

struct _writeable_t {
    void* stream;
    size_t (*write)(void* stream, const void* data, size_t size);
    void (*close)(void* stream);
};


void writeable_from_file(writeable_t* stream, FILE* file);

size_t writeable_write(writeable_t* stream, void* data, size_t size);
void writeable_close(writeable_t* stream);


#endif // __DVDCAT_WRITEABLE_H__