
#ifndef _DUNGEON_H_
#define _DUNGEON_H_

#include "arg.h"
#include "cstr.h"
#include "bsp.h"
#include "map.h"



typedef struct
{
    int width;
    int height;
    int numLevels;
    char *map;

    enum DungeonDesign {
        DungeonDesign_Catacombs = 0,
        DungeonDesign_Catacombs2,
        DungeonDesign_Tower,
    } design;

    int *bspParameters;
} Dungeon;



Dungeon Dungeon_create (const int numLevels, const int width, const int height);
void Dungeon_destroy (Dungeon dungeon);



#endif
