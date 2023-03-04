
#ifndef _ARG_H_
#define _ARG_H_

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>


static char doc[] = "A programm for generating 2d dungeons.";

struct Arguments
{   
    char flag_debug;
    int iterations;
    int grid_size;
    int map_width;
    int map_height;
    char *output_file;
};


extern struct Arguments arg;


static struct argp_option options[] = {
    {"debug", 'D', 0, 0, "Debug mode with additional output"},
    {"iterations", 'i', "iterations", 0, "Number of subdivisions (default: 7)"},
    {"map-size", 500, "WIDTHxHEIGHT", 0, "Map size in WIDTHxHEIGHT squares (default: 40x30)"},
    {"grid-size", 501, "pixels", 0, "Size of one square in pixels (default: 25)"},
    {"output", 'o', "FILE", 0, "Output to FILE (default: 'out.png')"},
    {0}
};



error_t parse_opt (int key, char *arg, struct argp_state *state);

static struct argp argp = { options, parse_opt, NULL, doc };


#endif
