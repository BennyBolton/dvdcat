#include "dvdcat.h"
#include "options.h"
#include "log.h"
#include "writeable.h"

#include <stdlib.h>



int main(int argc, char* argv[]) {
    int code;
    options_t opts;
    writeable_t stream;

    code = options_parse(&opts, argc, argv);
    if (code) return code;

    writeable_from_file(&stream, stdout);

    code = !dvdcat(&opts, &stream);
    writeable_close(&stream);

    return code;
}