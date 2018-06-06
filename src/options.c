#include "options.h"

#include "log.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>



static options_t default_options = {
    .device = "/dev/dvd",
    .title = 1,
    .angle = 1,
    .chapter_start = 1,
    .chapter_end = -1,
};

static struct option cmd_options[] = {
    { "device",   required_argument, NULL, 'd' },
    { "title",    required_argument, NULL, 't' },
    { "angle",    required_argument, NULL, 'a' },
    { "chapters", required_argument, NULL, 'c' },
    { "quiet",    no_argument,       NULL, 'q' },
    { "verbose",  no_argument,       NULL, 'V' },

    { "help",     no_argument,       NULL, 'h' },
    { "version",  no_argument,       NULL, 'v' },

    { 0 }
};

static const char* help_string =
    "Usage: dvdcat [options]\n"
    "\n"
    "Write dvd contents to stdout\n"
    "\n"
    "Options:\n"
    "  -d --device <device>     Block device or path to dvd contents\n"
    "                           Defaults to /dev/dvd\n"
    "  -t --title <title>       Title id to use. Defaults to 1\n"
    "  -a --angle <angle>       Angle index to use. Defaults to 1\n"
    "  -c --chapters <chapters> Chapters to output, either as a single chapter, or\n"
    "                           an inclusive range in the form start-[end]\n"
    "                           Defaults to all chapters\n"
    "  -q --quiet               Only output warnings and errors\n"
    "  -V --verbose             Output debug logs\n"
    "  -h --help                Show this message\n"
    "  -v --verseion            Show version\n"
    "\n"
    "Examples:\n"
    "  dvdcat -d /media/dvd     Write first title of the mounted dvd\n"
    "  dvdcat -t 3 -c 4         Write only chapter 4 of the third title\n"
    "  dvdcat -t 3 -c 4-6       Write chapters 4 through 6 of the third title\n"
    ;


int options_parse(options_t* opts, int argc, char* argv[]) {
    int val;
    char* end;

    *opts = default_options;
    while (1) {
        val = getopt_long(argc, argv, "d:t:a:c:qVhv", cmd_options, NULL);
        if (val < 0) break;

        switch (val) {
            case 'd':
                opts->device = optarg;
                break;

            case 't':
                opts->title = strtol(optarg, &end, 10);
                if (end == optarg || (end && end[0]) || opts->title < 1) {
                    log_error("Title must be a positive number\n");
                    return 1;
                }
                break;

            case 'a':
                opts->angle = strtol(optarg, &end, 10);
                if (end == optarg || (end && end[0]) || opts->angle < 1) {
                    log_error("Angle must be a positive number\n");
                    return 1;
                }
                break;

            case 'c':
                opts->chapter_start = strtol(optarg, &end, 10);
                if (end && end[0] != 0 && end[0] != '-') opts->chapter_start = 0;
                if (end == optarg || opts->chapter_start < 1) {
                    log_error("Chapters must be a positive number or range\n");
                    return 1;
                }
                if (end && end[0]) {
                    if (end[1]) {
                        opts->chapter_end = strtol(end + 1, &end, 10);
                        if ((end && end[0]) || opts->chapter_end < opts->chapter_start) {
                            log_error("Chapters must be a positive number or range\n");
                            return 1;
                        }
                    } else {
                        opts->chapter_end = -1;
                    }
                } else {
                    opts->chapter_end = opts->chapter_start;
                }
                break;

            case 'q':
                set_log_level(LOG_LEVEL_WARN);
                break;

            case 'V':
                set_log_level(LOG_LEVEL_DEBUG);
                break;

            case 'h':
            case '?':
                fputs(help_string, stderr);
                return 1;

            case 'v':
                fprintf(stderr, "dvdcat %s\n", DVDCAT_VERSION);
                return 1;
        }
    }
    return 0;
}