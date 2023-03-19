
#include "dungeon.h"

// ----- private functions -----

void
map_generate (char *const map, Vec2 pos, const int width, const int height)
{
    for (int i = 0; i < width * height; i++)
        map[i] = TileType_None;
    
    Rect *head = rect_create (NULL, vec2 (pos.x + 1, pos.y + 1), width - 2, height - 2);

    int numCorridors = arg.numCorridors;
    if (! numCorridors)
        numCorridors = (width > height) ? height / 15 : width / 15;

    bsp (&head, arg.iterations, numCorridors);

    map_from_bsp (head, map, width);
    map_create_walls (map, width, height);

    Rect_free (head);
}


void
generate_stairs (char *const map_h, char *const map_l, const int width, const int height, const int numStairs)
{
    int countValid = 0;
    char *mapValid = map_valid_stairs (map_h, map_l, width, height, &countValid);
    
    int prevStairPos = 0;
    for (int i = 0; i < numStairs; i++)
    {
        int rand = rng_between (i * countValid / numStairs, (i + 1) * countValid / numStairs - 1);
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
        }
        else if (prevStairPos && pos - prevStairPos >= width / 2)
        {
            prevStairPos = pos;
            map_h[pos] = TileType_Stair_Down;
            map_l[pos] = TileType_Stair_Up;
        }
    }

    free (mapValid);
}



// ----- public functions -----

Dungeon
Dungeon_create (const int numLevels, const int width, const int height)
{
    Dungeon dungeon = {
        width,
        height,
        numLevels,
        malloc (sizeof (char) * width * height * numLevels),
        DungeonDesign_Catacombs
    };
        
    int numStairs = 5;
    for (int i = 0; i < numLevels; i++)
    {   
        map_generate (&dungeon.map[i * width * height], vec2 (0, 0), width, height);
        if (i)
            generate_stairs (&dungeon.map[(i - 1) * width * height],
                    &dungeon.map[i * width * height], width, height, numStairs);
    }

    return dungeon;
}


void
Dungeon_destroy (Dungeon dungeon)
{   
    free (dungeon.map);
}

