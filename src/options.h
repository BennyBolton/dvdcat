#ifndef __DVDCAT_OPTIONS_H__
#define __DVDCAT_OPTIONS_H__


typedef struct _options_t options_t;

struct _options_t {
    const char* device;
    int title;
    int angle;
    int chapter_start;
    int chapter_end;
};


int options_parse(options_t* opts, int argc, char* argv[]);


#endif // __DVDCAT_OPTIONS_H__