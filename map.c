
#include "map.h"

// ----- private functions -----

void
p_map_generate_room (Room *const room, char *const map, const int width)
{
    if (! room) return;
    for (int x = room->pos.x; x < room->pos.x + room->width; x++)
        for (int y = room->pos.y; y < room->pos.y + room->height; y++)
            map[y * width + x] = TileType_Floor;
}


void
p_map_from_bsp (Rect *const rect, char *const map, const int width)
{       
    if (! rect) return;

    p_map_from_bsp (rect->childLeft, map, width);
    p_map_from_bsp (rect->childRight, map, width);

    for (Room *room = rect->room; room; room = room->next)
        p_map_generate_room (room, map, width);
}



char
p_map_check_neighbors_row (const char *const map, const int index)
{
    char sum = 0;
    for (int i = index - 1; i < index + 2; i++)
        sum += map[i];
    return sum;
}
    

char
p_map_check_neighbors (const char *const map, const int width, const int height, int index)
{
    if (index / width == 0) return 0; // ignore y = 0
    if (index / width == height - 1) return 0; // ignore y = mapHeight
    if (index % width == 0) return 0; // ignore x = 0
    if (index % width == width - 1) return 0; // ignore x = mapWidth

    char sum = p_map_check_neighbors_row (map, index);
    sum += p_map_check_neighbors_row (map, index - width);
    sum += p_map_check_neighbors_row (map, index + width);

    return sum;
}


char *
p_map_erosion (char *map, const int width, const int height)
{   
    char *tmp = malloc (sizeof (char) * width * height);
    for (int i = 0; i < width * height; i++)
        tmp[i] = (p_map_check_neighbors (map, width, height, i) == 9) ? 1 : 0;

    return tmp;
}


char *
p_map_dilatation (char *map, const int width, const int height)
{  
    char *tmp = malloc (sizeof (char) * width * height);
    for (int i = 0; i < width * height; i++)
        tmp[i] = (p_map_check_neighbors (map, width, height, i) > 0) ? 1 : 0;

    return tmp;
}




// ----- public functions -----

void
map_generate (char *const map, Vec2 pos, const int width, const int height,
        const int mapWidth, const int mapHeight, int *const parameters)
{
    Rect *head = rect_create (NULL, vec2 (pos.x + 1, pos.y + 1), width - 2, height - 2);

    int numCorridors = parameters[e_BspParameter_NumCorridors];
    if (! numCorridors)
        numCorridors = (width > height) ? height / 20 : width / 20;

    bsp (&head, parameters[e_BspParameter_Iterations], numCorridors);

    p_map_from_bsp (head, map, mapWidth);
    map_create_walls (map, mapWidth, mapHeight);

    Rect_free (head);
}


void
map_create_walls (char *const map, const int width, const int height)
{   
    char *dilatation = p_map_dilatation (map, width, height);
    for (int i = 0; i < width * height; i++)
        if (map[i] ^ dilatation[i])
            map[i] = TileType_Wall;

    free (dilatation);
}


void
map_dilatation_erosion (char **map, const int width, const int height, enum DilatationErosion type)
{   
    char *tmp = NULL;
    if (type == e_Dilatation)
        tmp = p_map_dilatation (*map, width, height);
    else if (type == e_Erosion)
        tmp = p_map_erosion (*map, width, height);

    if (tmp)
    {
        free (*map);
        *map = tmp;
    }
}


void
map_dilatation (char *const map, const int width, const int height)
{
    char *tmp = NULL;
    tmp = p_map_dilatation (map, width, height);
        
    //memcpy (map, tmp, sizeof (int) * width * height);
    for (int i = 0; i < width * height; i++)
        map[i] = tmp[i];

    free (tmp);
}


char *
map_compare (const char *const map_1, const char *const map_2, const int size)
{   
    char *ret = malloc (sizeof (char) * size);
    for (int i = 0; i < size; i++)
        ret[i] = (map_1[i] == TileType_Floor && map_2[i] == TileType_Floor) ? 1 : 0;

    return ret;
}


char *
map_valid_stairs (char *const map_h, char *const map_l, const int width, const int height, int *const countValid)
{
    char *tmp = map_compare (map_h, map_l, width * height);
    
    map_dilatation_erosion (&tmp, width, height, e_Erosion);
    map_dilatation_erosion (&tmp, width, height, e_Dilatation);

    int count = 0;
    char *mapValid = malloc (sizeof (char) * width * height);
    for (int i = 0; i < width * height; i++)
    {   
        mapValid[i] = 0;
        if (! tmp[i]) continue;

        char res_h = p_map_check_neighbors (map_h, width, height, i);
        if (! res_h || res_h % 3) continue;
        char res_l = p_map_check_neighbors (map_l, width, height, i);
        if (! res_l || res_l % 3) continue;

        mapValid[i] = 1;
        count ++;
    }
    
    free (tmp);

    if (countValid)
        *countValid = count;

    return mapValid;
}


void
map_decay_step (char *const map, const int width, const int height, const int pos, const int steps)
{
    if (! steps) return;

    if (pos / width == 0) return; // ignore y = 0
    if (pos / width == height - 1) return; // ignore y = mapHeight
    if (pos % width == 0) return; // ignore x = 0
    if (pos % width == width - 1) return; // ignore x = mapWidth
    
    if (map[pos] == TileType_Wall || map[pos] == TileType_None)
    //if (map[pos] < 3)
        map[pos] = TileType_Decay;
    
    for (int i = 0; i < rng () % 13; i++) ;
    switch (rng () % 4)
    {
        case 0:
            map_decay_step (map, width, height, pos - width, steps - 1);
            break;

        case 1:
            map_decay_step (map, width, height, pos + 1, steps - 1);
            break;

        case 2:
            map_decay_step (map, width, height, pos - width, steps - 1);
            break;

        case 3:
            map_decay_step (map, width, height, pos - 1, steps - 1);
            break;
    }
}


void
map_drunken_dwarf_step (char *const map, const int width, const int height, const int pos, const int steps)
{
    if (! steps) return;

    // set new seed at certain iterations
    if (steps % 13 == 0) rng_seed (steps / 13 << 16 | pos * steps);
    if (steps % 97 == 0) rng_seed (steps / 97 << 16 | pos * steps);

    if (pos / width <= 0) return; // ignore y = 0
    if (pos / width >= height - 1) return; // ignore y = mapHeight
    if (pos % width <= 0) return; // ignore x = 0
    if (pos % width >= width - 1) return; // ignore x = mapWidth
    
    map[pos] = TileType_Floor;

    int r = rng () % 256;
    int weights[] = {192, 128, 64, 0};
    int offset[] = {-width, +1, +width, -1};
    //int offset[] = {-3 * width, +3, +3 * width, -3};
    for (int i = 0; i < 4; i++)
        if (r >= weights[i])
        {
            map_drunken_dwarf_step (map, width, height, pos + offset[i], steps - 1);
            break;
        }
}


void
map_fprint (FILE *const fp, const char *const map, const int width, const int height)
{
    static const char clist[] = " +X/\\%";
    for (int i = 0; i < width * height; i++)
    {   
        if (i / width == 0 ||
            i / width == height - 1 ||
            i % width == 0 ||
            i % width == width - 1)
            fprintf (fp, "*");
        else
            fprintf (fp, "%c", clist[(int) map[i]]);

        if (i % width == width - 1)
            fprintf (fp, "\n");
    }
}
