
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

    char design;

    int *parameters;
    int *seeds;
} Dungeon;



enum DungeonDesign {
    e_DungeonDesign_Catacomb = 0,
    e_DungeonDesign_Cave,
    e_DungeonDesign_CaveContinuous,
    e_DungeonDesign_SIZE_
};


enum DungeonParameters{
    e_DungeonParameter_Design = e_BspParameter_SIZE_,
    e_DungeonParameter_NumStairs,
    e_DungeonParameter_Decay,
    e_DungeonParameter_SIZE_
};


static const char *dungeonParameterKeys[] = {
    "design",
    "numStairs",
    "decay"
};


Dungeon Dungeon_create (const int numLevels, const int width, const int height);
void Dungeon_destroy (Dungeon dungeon);



#endif
