#include "writeable.h"



static size_t file_write(void* stream, const void* data, size_t size) {
    FILE* file = (FILE*)stream;
    return fwrite(data, 1, size, file);
}


static void file_close(void* stream) {
    FILE* file = (FILE*)stream;
    fclose(file);
}


void writeable_from_file(writeable_t* stream, FILE* file) {
    stream->stream = file;
    stream->write = &file_write;
    stream->close = &file_close;
}


size_t writeable_write(writeable_t* stream, void* data, size_t size) {
    if (stream && stream->write) {
        return stream->write(stream->stream, data, size);
    }
    return 0;
}


void writeable_close(writeable_t* stream) {
    if (stream && stream->close) {
        stream->close(stream->stream);
    }
}