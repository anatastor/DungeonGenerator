
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
map_from_bsp (Rect *const rect, char *const map, const int width)
{       
    if (! rect) return;

    map_from_bsp (rect->childLeft, map, width);
    map_from_bsp (rect->childRight, map, width);

    for (Room *room = rect->room; room; room = room->next)
        p_map_generate_room (room, map, width);
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
    if (type == DE_Dilatation)
        tmp = p_map_dilatation (*map, width, height);
    else if (type == DE_Erosion)
        tmp = p_map_erosion (*map, width, height);

    if (tmp)
    {
        free (*map);
        *map = tmp;
    }
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
    
    map_dilatation_erosion (&tmp, width, height, DE_Erosion);
    map_dilatation_erosion (&tmp, width, height, DE_Dilatation);

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
map_fprint (FILE *const fp, const char *const map, const int width, const int height)
{
    static const char clist[] = " +X/\\";
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
