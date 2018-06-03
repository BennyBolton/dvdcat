#include "dvdcat.h"

#include "log.h"
#include "progress.h"

#include <stdio.h>
#include <dvdread/dvd_reader.h>
#include <dvdread/ifo_types.h>
#include <dvdread/ifo_read.h>



#ifndef DVDCAT_BLOCK_SIZE
#define DVDCAT_BLOCK_SIZE 1024
#endif



static int get_next_cell(pgc_t* chain, int* pos, int end, int angle) {
    int next_cell, block_type;

    block_type = chain->cell_playback[*pos - 1].block_type;
    if (block_type == BLOCK_TYPE_ANGLE_BLOCK) {
        (*pos) += angle - 1;
        for (next_cell = *pos; next_cell <= end; ++next_cell) {
            block_type = chain->cell_playback[next_cell - 1].block_type;
            if (block_type == BLOCK_MODE_LAST_CELL) {
                ++next_cell;
                break;
            }
        }
    } else {
        next_cell = *pos + 1;
    }

    return next_cell;
}


int dvdcat(options_t* options, writeable_t* stream) {
    // Do something along the lines of
    // https://github.com/hilbix/libdvdread-samples/blob/master/play_title.c
    dvd_reader_t* dvd = NULL;
    dvd_file_t* title = NULL;
    ifo_handle_t* video_manager = NULL;
    ifo_handle_t* vts_file = NULL;
    title_info_t title_info;
    ttu_t chapter_table;
    pgc_t* program_chain;
    ptt_info_t chapter_info;
    cell_playback_t cell_info;
    int count, read, ret_code = 0, cur_cell, start_cell, next_cell, end_cell;
    progress_t progress;
    int sector;
    unsigned char buffer[DVDCAT_BLOCK_SIZE * DVD_VIDEO_LB_LEN];

    log_info("Using device: %s\n", options->device);
    log_debug("Opening dvd\n");
    dvd = DVDOpen(options->device);
    if (!dvd) {
        log_error("error: Unable to open dvd: %s\n", options->device);
        goto error;
    }

    log_debug("Opening VMG info\n");
    video_manager = ifoOpen(dvd, 0);
    if (!video_manager) {
        log_error("error: Unable to open VMG info\n");
        goto error;
    }

    count = video_manager->tt_srpt->nr_of_srpts;
    log_debug("Number of titles on dvd: %d\n", count);
    log_info("Using title: %d\n", options->title);
    if (options->title > count) {
        log_error("error: Invalid title: %d\n", options->title);
        goto error;
    }
    title_info = video_manager->tt_srpt->title[options->title - 1];

    count = title_info.nr_of_angles;
    log_debug("Number of angles in title: %d\n", count);
    log_info("Using angle: %d\n", options->angle);
    if (options->angle > count) {
        log_error("error: Invalid angle: %d\n", options->angle);
        goto error;
    }

    log_debug("Opening VTS info for title\n");
    log_info("Using title set: %d\n", (int)title_info.title_set_nr);
    vts_file = ifoOpen(dvd, title_info.title_set_nr);
    if (!vts_file) {
        log_error("error: Unable to open VTS info for title set: %d\n",
            (int)title_info.title_set_nr);
        goto error;
    }

    log_debug("Determining cell range\n");
    chapter_table = vts_file->vts_ptt_srpt->title[title_info.vts_ttn - 1];
    count = chapter_table.nr_of_ptts;
    log_debug("Number of chapters in title: %d\n", count);
    if (options->chapter_end < 0) {
        options->chapter_end = count;
    }
    log_info("Using chapters: %d - %d\n",
        options->chapter_start, options->chapter_end);
    if (options->chapter_start > count) {
        log_error("error: Invalid chapter: %d\n", options->chapter_start);
        goto error;
    }
    if (options->chapter_end > count) {
        log_warn("warning: End chapter %d is out of range\n",
            options->chapter_end);
    }
    chapter_info = chapter_table.ptt[options->chapter_start - 1];
    program_chain = vts_file->vts_pgcit->pgci_srp[chapter_info.pgcn - 1].pgc;
    start_cell = program_chain->program_map[chapter_info.pgn - 1];
    end_cell = program_chain->nr_of_cells;
    log_debug("Number of cells in program chain: %d\n", end_cell);
    if (options->chapter_end < count) {
        chapter_info = chapter_table.ptt[options->chapter_end];
        count = program_chain->nr_of_programs;
        log_debug("Number of programs: %d\n", count);
        if (chapter_info.pgn <= count) {
            end_cell = program_chain->program_map[chapter_info.pgn - 1] - 1;
        } else {
            log_warn("warning: End chapter out of range of program chain\n");
        }
    }
    log_info("Using cells: %d - %d\n", start_cell, end_cell);

    log_debug("Opening title set data\n");
    title = DVDOpenFile(dvd, title_info.title_set_nr, DVD_READ_TITLE_VOBS);
    if (!title) {
        log_error("error: Unable to open title set: %d\n",
            (int)title_info.title_set_nr);
        goto error;
    }

    count = 0;
    for (next_cell = start_cell; next_cell <= end_cell;) {
        cur_cell = next_cell;
        next_cell = get_next_cell(program_chain, &cur_cell, end_cell, options->angle);
        cell_info = program_chain->cell_playback[cur_cell - 1];
        count += cell_info.last_sector - cell_info.first_sector + 1;
    }

    log_debug("Starting data stream\n");
    progress_start(&progress, DVD_VIDEO_LB_LEN * (long)count);
    for (next_cell = start_cell; next_cell <= end_cell;) {
        cur_cell = next_cell;
        next_cell = get_next_cell(program_chain, &cur_cell, end_cell, options->angle);

        cell_info = program_chain->cell_playback[cur_cell - 1];
        for (sector = cell_info.first_sector; sector < cell_info.last_sector;) {
            count = cell_info.last_sector - sector + 1;
            if (count > DVDCAT_BLOCK_SIZE) count = DVDCAT_BLOCK_SIZE;

            read = DVDReadBlocks(title, sector, count, buffer);
            if (read < count) {
                log_error("error: Read returned incomplete data at sector %d\n",
                    sector);
                log_error("  Expected %d blocks, got %d blocks\n", count, read);
                goto error;
            }

            writeable_write(stream, buffer, count * DVD_VIDEO_LB_LEN);
            sector += count;
            progress_update(&progress, count * DVD_VIDEO_LB_LEN);
        }
    }
    progress_finish(&progress);

    end:
    if (title) DVDCloseFile(title);
    if (vts_file) ifoClose(vts_file);
    if (video_manager) ifoClose(video_manager);
    if (dvd) DVDClose(dvd);
    return ret_code;

    error:
    ret_code = -1;
    goto end;
}