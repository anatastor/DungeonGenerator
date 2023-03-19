
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
    DE_Erosion = 0,
    DE_Dilatation = 1
};


void map_from_bsp (Rect *const rect, char *const map, const int width);

void map_create_walls (char *const map, const int width, const int height);



void map_dilatation_erosion (char **map, const int width, const int height, enum DilatationErosion);


char *map_compare (const char *const map_1, const char *const map_2, const int size);


char *map_valid_stairs (char *const map_l, char *const map_h, const int width, const int height, int *const countValid);


void map_decay_step (char *const map, const int width, const int height, const int pos, const int steps);

void map_fprint (FILE *const fp, const char *const map, const int width, const int height);

#endif 
