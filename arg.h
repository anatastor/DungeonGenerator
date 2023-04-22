
#ifndef _ARG_H_
#define _ARG_H_

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cstr.h"
#include "bsp.h"

#define BUFFER_SIZE 1048


static char doc[] = "A programm for generating 2d dungeons.";

struct Arguments
{   
    char flag_debug;
    int seed;
    int iterations;
    int gridSize;
    int mapWidth;
    int mapHeight;
    char corridorWidth;
    int numCorridors;
    int roomOffset;
    int roomSize;
    int minRoomSize;
    char *outfile;
    int numLevels;
    int dungeonDesign;
    float dungeonDecay;
    int rectOffset;
    char levelData[BUFFER_SIZE];
    int roomChance;
};



extern struct Arguments arg;


static struct argp_option options[] = {
    {"debug", 'D', 0, 0, "Debug mode"},
    {"seed", 's', "seed", 0, "Seed for the generation"},
    {"iterations", 'i', "iterations", 0, "Number of subdivisions (default: 7)"},
    {"mapSize", 500, "WIDTHxHEIGHT", 0, "Map size in WIDTHxHEIGHT squares (default: 40x30)"},
    {"gridSize", 501, "pixels", 0, "Size of one tile in pixels (default: 25)"},
    {"corridorWidth", 502, "width", 0, "Width of the corridors in tiles"},
    {"numCorridors", 503, "number", 0,
        "Number of corridors for the top level, decreases by 1 each level to a minimum of 1"},
    {"roomSize", 504, "int", 0, "Number of Rolls for the Room size "
        "size > 0 leads to greater rooms while size < 0 leads to overall smaller rooms (default: 0)"},
    {"roomOffset", 505, "offset", 0, "offset to the edges of the rectangle (default: 1)"},
    {"output", 'o', "FILE", 0, "Output to FILE (default: 'out.png')"},
    {"numLevels", 'l', "NUMLEVELS", 0, "number of levels to generate (default: 1)"},
    {"design", 506, "Design", 0, "Design Preset"},
    {"decay", 507, "Decay", 0, "Decay of the Map (default: 0.0)"},
    {"rectOffset", 508, "RectOffset", 0, "Offset of Rectangles (default: 0)"},
    {"minRoomSize", 509, "size", 0, "minimal size of a room (default: 5)"},
    {"levelConfig", 'L', "level data", 0, "configuration for separate levels in form of a list "
        "levels are separated by ';' and entries for one level are separated by ',' "
        "each level is expected to start with the level id starting from 0 "
        "if no options for a level are given, the global options will be used, "
        "thus it is possible to further customize only one out of a number of levels"},
    {"roomChance", 510, "pct (0 - 100)", 0, "Chance to place a room in pct (0 - 100)"},
    {0}
};



error_t parse_opt (int key, char *arg, struct argp_state *state);

static struct argp argp = { options, parse_opt, NULL, doc };


#endif
