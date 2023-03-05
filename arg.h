
#ifndef _ARG_H_
#define _ARG_H_

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>


static char doc[] = "A programm for generating 2d dungeons.";

struct Arguments
{   
    char flag_debug;
    char flag_color;
    int seed;
    int iterations;
    int grid_size;
    int map_width;
    int map_height;
    char corridor_width;
    int numCorridors;
    int roomOffset;
    int roomSize;
    int minRoomSize;
    char *output_file;
};


extern struct Arguments arg;


static struct argp_option options[] = {
    {"debug", 'D', 0, 0, "Debug mode with additional output"},
    {"color", 'C', 0, 0, "Draw rooms and corridors in diffrent colors"},
    {"seed", 's', "seed", 0, "Seed for the generation"},
    {"iterations", 'i', "iterations", 0, "Number of subdivisions (default: 7)"},
    {"map-size", 500, "WIDTHxHEIGHT", 0, "Map size in WIDTHxHEIGHT squares (default: 40x30)"},
    {"grid-size", 501, "pixels", 0, "Size of one square in pixels (default: 25)"},
    {"corridor-width", 502, "width", 0, "Width of the corridors in Squares"},
    {"number-of-corridors", 503, "number", 0,
        "Number of corridors for the top level, decreases by 1 each level to a minimu of 1"},
    {"roomSize", 504, "size", 0, "Number of Rolls for the Room size "
        "size > 0 leads to greater rooms while size < 0 leads to overall smaller rooms (default: 0)"},
    {"roomOffset", 505, "offset", 0, "offset to the edges of the rectangle (default: 1)"},
    {"output", 'o', "FILE", 0, "Output to FILE (default: 'out.png')"},
    {0}
};



error_t parse_opt (int key, char *arg, struct argp_state *state);

static struct argp argp = { options, parse_opt, NULL, doc };


#endif
