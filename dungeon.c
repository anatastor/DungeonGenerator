
#include "dungeon.h"

// ----- private functions -----

int
p_Dungeon_generate_stairs (char *const map_h, char *const map_l,
        const int width, const int height, const int numStairs)
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
p_Dungeon_decay (Dungeon dungeon)
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

            map_drunken_dwarf_step (map, dungeon.width, dungeon.height, pos, steps);
        }
    }
}


int
p_find_key (char *key)
{
    if (! key) return e_DungeonParameter_SIZE_;
    
    int i = 0;
    for (; i < e_BspParameter_SIZE_; i++)
        if (strcmp (key, bsp_parameter_keys[i]) == 0) return i;
    
    for (; i < e_DungeonParameter_SIZE_; i++)
        if (strcmp (key, dungeonParameterKeys[i - e_BspParameter_SIZE_]) == 0) return i;
    
    return e_DungeonParameter_SIZE_;
}


void
p_dungeon_parse_level (char *str, int level, int *data)
{
    if (! str) return;

    char *last;
    char *first = cstr_get (str, &last, ',');
    do
    {
        char *tmp = strchr (first, '=');
        if (tmp)
            *tmp = '\0';
        else
            continue;

        int index = p_find_key (first);
        if (index == e_DungeonParameter_SIZE_)
        {
            fprintf (stderr, "no option found for key '%s'\n", first);
            continue;
        }

        // TODO Catch Errors
        int value = atoi (tmp + 1);
        data [index + level * e_DungeonParameter_SIZE_] = value;

        first = cstr_get (last, &last, ',');
    }
    while (first);
}


void
p_dungeon_parse_levels (char *str, int *data, const int numLevels)
{
    if (! str) return;
    if (*str == '\0') return;

    char *last;
    char *first = cstr_get (str, &last, ';');
    do
    {
        int level;
        sscanf (first, "%i,%s,", &level, first);

        if (arg.flag_debug)
            printf ("parsing Level %i: %s\n", level, first);

        if (level < numLevels)
            p_dungeon_parse_level (first, level, data);

        first = cstr_get (last, &last, ';');
    }
    while (first);

}


void
p_Dungeon_generate_level_catacomb (char *const map,
        const int mapWidth, const int mapHeight, int *const parameters)
{
    int width = rng_between (5 * mapWidth / 7, 6 * mapWidth / 7);
    int height = rng_between (5 * mapHeight / 7, 6 * mapHeight / 7);
    
    Vec2 pos = vec2 (rng_between (0, mapWidth - width),
            rng_between (0, mapHeight - height));

    bsp_set_parameters (parameters);

    map_generate (map, pos, width, height,
            mapWidth, mapHeight, parameters);
}


void
p_Dungeon_generate_level_cave (char *const map,
        const int mapWidth, const int mapHeight, int *const parameters)
{   
    // TODO set numDwarves and steps relativ to map size
    int numDwarves = 13;
    int steps = 9973;
    for (int i = 0; i < numDwarves; i++)
    {
        int pos = rng () % (mapWidth * mapHeight);
        map_drunken_dwarf_step (map, mapWidth, mapHeight, pos, steps);
    }
    
    // dilatation for bigger corridors
    map_dilatation (map, mapWidth, mapHeight);
}


void
p_Dungeon_generate_levels (Dungeon dungeon)
{
    for (int i = 0; i < dungeon.numLevels; i++)
    {
        rng_seed (dungeon.seeds[i]);
        char design = dungeon.parameters[e_DungeonParameter_Design + i * e_DungeonParameter_SIZE_];

        switch (design)
        {
            case e_DungeonDesign_Catacomb:
                printf ("%i\tCatacomb\n", i);
                p_Dungeon_generate_level_catacomb (&dungeon.map[i * dungeon.width * dungeon.height],
                        dungeon.width, dungeon.height, &dungeon.parameters[i * e_DungeonParameter_SIZE_]);
                break;

            case e_DungeonDesign_Cave:
                printf ("%i\tCave\n", i);
                p_Dungeon_generate_level_cave (&dungeon.map[i * dungeon.width * dungeon.height],
                        dungeon.width, dungeon.height, &dungeon.parameters[i * e_DungeonParameter_SIZE_]);
                break;

            default:
                break;
        }

        int numStairs = dungeon.parameters[e_DungeonParameter_NumStairs + i * e_DungeonParameter_SIZE_];
        if (i)
            p_Dungeon_generate_stairs (&dungeon.map[(i - 1) * dungeon.width * dungeon.height],
                    &dungeon.map[i * dungeon.width * dungeon.height],
                    dungeon.width, dungeon.height, numStairs);
    }
}


void
p_Dungeon_generate (Dungeon dungeon)
{
    int defaultParameters[] = {
        arg.iterations,
        arg.roomSize,
        arg.minRoomSize,
        arg.roomOffset,
        arg.roomChance, // pct roomChance
        arg.rectOffset,
        arg.corridorWidth,
        arg.numCorridors,
        e_DungeonDesign_Catacomb,
        5 // numStairs
    };

    // set global parameters
    for (int i = 0; i < dungeon.numLevels * e_DungeonParameter_SIZE_; i++)
    {
        int index = i % e_DungeonParameter_SIZE_;
        dungeon.parameters[i] = defaultParameters[index];
    }

    p_dungeon_parse_levels (arg.levelData, dungeon.parameters, dungeon.numLevels);

    // set seeds per level
    dungeon.seeds = malloc (sizeof (int) * dungeon.numLevels);
    for (int i = 0; i < dungeon.numLevels; i++)
        dungeon.seeds[i] = rng ();

    p_Dungeon_generate_levels (dungeon);

    //Dungeon_generate_design (dungeon);
    //Dungeon_decay (dungeon);
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
        arg.dungeonDesign,
        NULL,
    };

    dungeon.parameters = malloc (sizeof (int) * e_DungeonParameter_SIZE_ * dungeon.numLevels);

    p_Dungeon_generate (dungeon);

    return dungeon;
}


void
Dungeon_destroy (Dungeon dungeon)
{   
    free (dungeon.seeds);
    free (dungeon.parameters);
    free (dungeon.map);
}

