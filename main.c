
#include <stdio.h>
#include <stdlib.h>

#include "arg.h"
#include "rng.h"
#include "cairo_renderer.h"
#include "bsp.h"


void
fprint_map (FILE *const fp, const char *const map)
{
    static const char clist[] = " +^!";
    for (int i = 0; i < arg.mapWidth * arg.mapHeight; i++)
    {
        if (i / arg.mapWidth == 0 ||
            i / arg.mapWidth == arg.mapHeight - 1 ||
            i % arg.mapWidth == 0 ||
            i % arg.mapWidth == arg.mapWidth - 1)
            fprintf (fp, "#");
        else
            fprintf (fp, "%c", clist[(int) map[i]]);

        if (i % arg.mapWidth == arg.mapWidth - 1)
            fprintf (fp, "\n");
    }
}


char
map_check_neighbors_row (const char *const map, const int index)
{
    char sum = 0;
    for (int i = index - 1; i < index + 2; i++)
        sum += map[i];
    return sum;
}
    

char
map_check_neighbors (char *map, int index)
{
    if (index / arg.mapWidth == 0) return 0; // ignore y = 0
    if (index / arg.mapWidth == arg.mapHeight - 1) return 0; // ignore y = mapHeight
    if (index % arg.mapWidth == 0) return 0; // ignore x = 0
    if (index % arg.mapWidth == arg.mapWidth - 1) return 0; // ignore x = mapWidth

    char sum = map_check_neighbors_row (map, index);
    sum += map_check_neighbors_row (map, index - arg.mapWidth);
    sum += map_check_neighbors_row (map, index + arg.mapWidth);

    return sum;
}


int
map_erosion (char **map)
{
    int count = 0;
    char *tmp = malloc (sizeof (char) * arg.mapWidth * arg.mapHeight);
    for (int i = 0; i < arg.mapWidth * arg.mapHeight; i++)
    {
        char res = (map_check_neighbors (*map, i) == 9) ? 1 : 0;
        count += res;
        tmp[i] = res;
    }

    free (*map);
    *map = tmp;

    return count;
}


int
map_dilatation (char **map)
{   
    int count = 0;
    char *tmp = malloc (sizeof (char) * arg.mapWidth * arg.mapHeight);
    for (int i = 0; i < arg.mapWidth * arg.mapHeight; i++)
    {
        char res = (map_check_neighbors (*map, i) > 0) ? 1 : 0;
        count += res;
        tmp[i] = res;
    }

    free (*map);
    *map = tmp;

    return count;
}


int
map_validStairs (char *const map_top, char *const map_bottom, char **map_b)
{
    int count = 0;
    char *tmp = malloc (sizeof (char) * arg.mapWidth * arg.mapHeight);
    for (int i = 0; i < arg.mapWidth * arg.mapHeight; i++)
    {   
        tmp[i] = 0;
        if (! (*map_b)[i]) continue;

        char res_top = map_check_neighbors (map_top, i);
        if (! res_top || res_top % 3) continue;
        char res_bottom = map_check_neighbors (map_bottom, i);
        if (! res_bottom || res_bottom % 3) continue;

        tmp[i] = 1;
        count++;
    }

    free (*map_b);
    *map_b = tmp;
    return count;
}



char *
map_compare (const char *const map1, const char *const map2)
{   
    /*
     * creates a new map layer containing True or False if both given maps share a room
     * on the same index
     */
    char *ret = malloc (sizeof (char) * arg.mapWidth * arg.mapHeight);

    for (int i = 0; i < arg.mapWidth * arg.mapHeight; i++)
        ret[i] = (map1[i] == e_room && map2[i] == e_room) ? 1 : 0;

    return ret;
}


int
main (int argc, char **argv)
{   
    /**+ INIT ***/
    argp_parse (&argp, argc, argv, 0, 0, NULL);
    rng_seed (arg.seed);
    Renderer *renderer = render_create (arg.mapWidth * arg.gridSize, arg.mapHeight * arg.gridSize);
    render_fill (renderer, 0x0c090a);

    Rect *head = rect_create (NULL, vec2 (1, 1), arg.mapWidth - 2, arg.mapHeight - 2);
    bsp (&head, arg.iterations, arg.numCorridors);
    
    Rect *head2 = rect_create (NULL, vec2 (1, 1), arg.mapWidth - 2, arg.mapHeight - 2);
    bsp (&head2, arg.iterations, arg.numCorridors);

    char *map = map_from_bsp (head, NULL);
    char *map2 = map_from_bsp (head2, NULL);

    char *map_b = map_compare (map, map2); // new array containing 1 on each position both maps have rooms

    map_erosion (&map_b);
    map_dilatation (&map_b);
    int countValid = map_validStairs (map, map2, &map_b);

    int stairCount = 5;
    int prevStairPos = 0;
    for (int i = 0; i < stairCount; i++)
    {
        int rand = rng_between (i * countValid / stairCount, (i + 1) * countValid / stairCount - 1);
        int pos = 0;
        for (int count = 0; pos < arg.mapWidth * arg.mapHeight; pos++)
        {
            count += map_b[pos];
            if (count == rand) break;
        }

        if (! prevStairPos)
        {
            prevStairPos = pos;
            map[pos] = e_stair_down;
            map2[pos] = e_stair_up;
        }
        else if (prevStairPos && pos - prevStairPos >= arg.mapWidth / 2)
        {
            map[pos] = e_stair_down;
            map2[pos] = e_stair_up;
            prevStairPos = pos;
        }
    }

    fprint_map (stdout, map_b);
    fprint_map (stdout, map);
    fprint_map (stdout, map2);
    
    render_map (renderer, map_b, arg.mapWidth, arg.mapHeight, arg.gridSize);
    render_grid (renderer, arg.gridSize, 0x757575);
    render_save (renderer, "map_b.png");

    render_map (renderer, map, arg.mapWidth, arg.mapHeight, arg.gridSize);
    render_grid (renderer, arg.gridSize, 0x757575);
    render_save (renderer, "map.png");

    render_map (renderer, map2, arg.mapWidth, arg.mapHeight, arg.gridSize);
    render_grid (renderer, arg.gridSize, 0x757575);
    render_save (renderer, "map2.png");

    /*** FREE & DESTROY ***/
    free (map);
    free (map2);
    free (map_b);
    rect_free (head);
    head = NULL;

    renderer = render_destroy (renderer);
    return EXIT_SUCCESS;
}
