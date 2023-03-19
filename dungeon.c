
#include "dungeon.h"

// ----- private functions -----

int
generate_stairs (char *const map_h, char *const map_l, const int width, const int height, const int numStairs)
{
    int countValid = 0;
    char *mapValid = map_valid_stairs (map_h, map_l, width, height, &countValid);

    if (countValid == 0) return 0;
    
    int prevStairPos = 0;
    int stairCount = 0;
    for (int i = 0; i < numStairs; i++)
    {
        int rand = rng_between (i * countValid / numStairs + 1, (i + 1) * countValid / numStairs - 1);
        int pos = 0;
        for (int count = 0; pos < width * height; pos++)
        {
            count += mapValid[pos];
            if (count == rand) break;
        }

        if (! prevStairPos)
        {
            prevStairPos = pos;
            map_h[pos] = TileType_Stair_Down;
            map_l[pos] = TileType_Stair_Up;
            stairCount++;
        }
        else if (prevStairPos && pos - prevStairPos >= width / 2)
        {
            prevStairPos = pos;
            map_h[pos] = TileType_Stair_Down;
            map_l[pos] = TileType_Stair_Up;
            stairCount++;
        }
    }

    free (mapValid);

    return stairCount;
}


void
Dungeon_generate_design_catacomb (Dungeon dungeon)
{
    int numStairs = 5;
    int f1 = 7;
    for (int i = 0; i < dungeon.numLevels; i++)
    {   
        int width = rng_between ((f1 - 2) * dungeon.width / f1, (f1 - 1) * dungeon.width / f1);
        int height = rng_between ((f1 - 2) * dungeon.height / f1, (f1 - 1) * dungeon.height / f1);

        Vec2 pos = vec2 (rng_between (0, dungeon.width - width),
                rng_between (0, dungeon.height - height));

        map_generate (&dungeon.map[i * dungeon.width * dungeon.height],
                pos, width, height,
                dungeon.width, dungeon.height);
        
        // TODO Catch Error when no stairs could be generated
        if (i)
            generate_stairs (&dungeon.map[(i - 1) * dungeon.width * dungeon.height],
                    &dungeon.map[i * dungeon.width * dungeon.height],
                    dungeon.width, dungeon.height, numStairs);
    }
}


void
Dungeon_generate_design (Dungeon dungeon)
{
    switch (dungeon.design)
    {
        case DungeonDesign_Catacombs:
            Dungeon_generate_design_catacomb (dungeon);
            break;

        default:
            Dungeon_generate_design_catacomb (dungeon);
            break;
    }
}


void
Dungeon_decay (Dungeon dungeon)
{       
    if (arg.dungeonDecay == 0.0) return;

    int pos;
    int decay = (int) (arg.dungeonDecay * 1000);
    int steps = decay / 3;
    for (int i = 0; i < dungeon.numLevels; i++)
    {
        char *map = &dungeon.map[i * dungeon.width * dungeon.height];
        for (int j = 0; j < decay / 13; j++)
        {
            do
                pos = rng () % dungeon.width + rng () % dungeon.height * dungeon.width;
            while (map[pos] != TileType_Floor);

            map_decay_step (map, dungeon.width, dungeon.height, pos, steps);
        }
    }
}



// ----- public functions -----

Dungeon
Dungeon_create (const int numLevels, const int width, const int height)
{
    Dungeon dungeon = {
        width,
        height,
        numLevels,
        calloc (numLevels * width * height, sizeof (char)),
        arg.dungeonDesign
    };

    Dungeon_generate_design (dungeon);
    Dungeon_decay (dungeon);

    return dungeon;
}


void
Dungeon_destroy (Dungeon dungeon)
{   
    free (dungeon.map);
}

