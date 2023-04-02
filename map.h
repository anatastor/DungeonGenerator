
#ifndef _MAP_H_
#define _MAP_H_

#include <stdio.h>

#include "bsp.h"


enum TileType {
    TileType_None = 0,
    TileType_Floor,
    TileType_Wall,
    TileType_Stair_Up,
    TileType_Stair_Down,
    TileType_Decay
};


enum DilatationErosion {
    e_Erosion = 0,
    e_Dilatation = 1
};


void map_generate (char *const map, Vec2 pos, const int width, const int height,
        const int mapWidth, const int mapHeight, int *const parameters);

void map_create_walls (char *const map, const int width, const int height);



void map_dilatation_erosion (char **map, const int width, const int height, enum DilatationErosion);
void map_dilatation (char *const map, const int width, const int height);


char *map_compare (const char *const map_1, const char *const map_2, const int size);


char *map_valid_stairs (char *const map_l, char *const map_h, const int width, const int height, int *const countValid);


void map_decay_step (char *const map, const int width, const int height, const int pos, const int steps);
void map_drunken_dwarf_step (char *const map, const int width, const int height, const int pos, const int steps);

void map_fprint (FILE *const fp, const char *const map, const int width, const int height);

#endif 
