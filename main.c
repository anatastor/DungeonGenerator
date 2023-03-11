
#include <stdio.h>
#include <stdlib.h>

#include "arg.h"
#include "rng.h"
#include "cairo_renderer.h"
#include "bsp.h"


void
draw_rect (Renderer *r, Rect *rect)
{   
    if (! rect)
        return;
   
    if (0)
        printf ("(%i, %i) + (%i, %i) = (%i, %i)\n[%p]\n", rect->pos.x, rect->pos.y,
                rect->width, rect->height,
                rect->pos.x + rect->width, rect->pos.y + rect->height,
                rect->room);
    if (1)
        render_rect (r, rect->pos.x * arg.grid_size, rect->pos.y * arg.grid_size,
                rect->width * arg.grid_size, rect->height * arg.grid_size, rect->color);
    if (0)
        render_line (r, rect->pos.x * arg.grid_size, rect->pos.y * arg.grid_size,
                (rect->pos.x + rect->width) * arg.grid_size, (rect->pos.y + rect->height) * arg.grid_size,
                7, 0x9f000f);



    draw_rect (r, rect->childLeft);
    draw_rect (r, rect->childRight);
}


void
draw_room (Renderer *r, Rect *rect)
{   
    if (! rect)
        return;

    draw_room (r, rect->childLeft);
    draw_room (r, rect->childRight);

    for (Room *ptr = rect->room; ptr; ptr = ptr->next)
    {   
        //printf ("R: (%i, %i)\n", rect->room->center.x, rect->room->center.y);
        render_rect (r, ptr->pos.x * arg.grid_size, ptr->pos.y * arg.grid_size,
                ptr->width * arg.grid_size, ptr->height * arg.grid_size, arg.flag_color ? ptr->color : 0x757575);
        if (arg.flag_debug)
            render_quad (r, ptr->center.x * arg.grid_size, ptr->center.y * arg.grid_size, arg.grid_size, 0xff0000);
    }
}


void
print_koordinates (Rect *rect)
{
    if (! rect)
        return;

    print_koordinates (rect->childLeft);
    print_koordinates (rect->childRight);

    for (Room *room = rect->room; room; room = room->next)
    {
        printf ("<%i> Top-Left (%i, %i) + %i, %i = (%i, %i)\n\tMitte (%i, %i)\n",
                room->count,
                room->pos.x, room->pos.y,
                room->width, room->height,
                room->pos.x + room->width, room->pos.y + room->height,
                room->center.x, room->center.y);
    }
}


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
    Renderer *renderer = render_create (arg.mapWidth * arg.grid_size, arg.mapHeight * arg.grid_size);
    render_fill (renderer, 0x0c090a);

    Rect *head = rect_create (NULL, vec2 (1, 1), arg.mapWidth - 2, arg.mapHeight - 2);
    bsp (&head, arg.iterations, arg.numCorridors);
    
    Rect *head2 = rect_create (NULL, vec2 (1, 1), arg.mapWidth - 2, arg.mapHeight - 2);
    if (arg.flag_debug)
        printf ("Map 2\n");
    bsp (&head2, arg.iterations, arg.numCorridors);

    if (arg.flag_debug)
        draw_rect (renderer, head);

    draw_room (renderer, head);
    //draw_room (renderer, head2);
    
    char *map = map_from_bsp (head, NULL);
    char *map2 = map_from_bsp (head2, NULL);

    char *map_b = map_compare (map, map2); // new array containing 1 on each position both maps have rooms

    map_erosion (&map_b);
    int countValid = map_dilatation (&map_b);

    int stairCount = 5;
    int prevStairPos = 0;
    for (int i = 0; i < stairCount; i++)
    {
        int rand = rng_between (i * countValid / stairCount, (i + 1) * countValid / stairCount - 1);
        int pos;
        for (int count = 0, pos = 0; pos < arg.mapWidth * arg.mapHeight; pos++)
            if ((count += map_b[pos]) == rand) break;

        if (prevStairPos && pos - prevStairPos >= arg.mapWidth / 2)
        {
            map[pos] = e_stair_down;
            map2[pos] = e_stair_up;
            prevStairPos = pos;
        }
    }


    fprint_map (stdout, map_b);
    fprint_map (stdout, map);
    fprint_map (stdout, map2);

    if (arg.flag_debug)
    {
        print_koordinates (head);
        printf ("Map 2\n");
        print_koordinates (head2);
    }
    
    render_grid (renderer, arg.grid_size, 0x757575);
    if (arg.flag_debug)
    {
        cairo_set_line_width (renderer->cr, 5);
        render_grid (renderer, 10 * arg.grid_size, 0xff0000);
    }

    render_save (renderer, arg.output_file);
    
    /*** FREE & DESTROY ***/
    free (map);
    free (map2);
    free (map_b);
    rect_free (head);
    head = NULL;

    renderer = render_destroy (renderer);
    return EXIT_SUCCESS;
}
