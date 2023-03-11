
#include <stdio.h>
#include <stdlib.h>

#include "arg.h"
#include "rng.h"
#include "cairo_renderer.h"
#include "bsp.h"


#define OUTFILE "out.png"

#define SEED    0x12351


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
map_print (FILE *fp, char **map, const int width, const int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (x == 0 || y == 0 || x == width - 1 || y == height - 1)
                fprintf (fp, "#");
            else
                fprintf (fp, "%c", map[x][y] ? 'X' : ' ');
        }
        fprintf (fp, "\n");
    }
}


void
map_free (char **map, const int width)
{   
    for (int i = 0; i < width; i++)
        free (map[i]);
    free (map);
}


char
checkPixels (char **map, const int x, const int y)
{
    char sum = 0;
    for (int i = x - 1; i < x + 2; i++)
        for (int j = y - 1; j < y + 2; j++)
            sum += map[i][j];
    return sum == 9 ? 1 : 0;
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
    
    char **map1 = map_from_bsp (head, NULL);
    char **map2 = map_from_bsp (head2, NULL);

    char **map3 = calloc (arg.mapWidth, sizeof (char *));
    for (int i = 0; i < arg.mapWidth; i++)
        map3[i] = calloc (arg.mapHeight, sizeof (char));

    char **map4 = calloc (arg.mapWidth, sizeof (char *));
    for (int i = 0; i < arg.mapWidth; i++)
        map4[i] = calloc (arg.mapHeight, sizeof (char));

    for (int x = 0; x < arg.mapWidth; x++)
        for (int y = 0; y < arg.mapHeight; y++)
        {
            map3[x][y] = map1[x][y] & map2[x][y];
            map4[x][y] = ! (map1[x][y] | map2[x][y]);
        }

    char **map5 = calloc (arg.mapWidth, sizeof (char *));
    for (int i = 0; i < arg.mapWidth; i++)
        map5[i] = calloc (arg.mapHeight, sizeof (char));

    for (int x = 1; x < arg.mapWidth - 1; x++)
        for (int y = 1; y < arg.mapHeight - 1; y++)
        {
            map5[x][y] = checkPixels (map3, x, y);
        }


    FILE *fp = fopen ("out.txt", "w");
    fprintf (fp, "Map 1\n");
    map_print (fp, map1, arg.mapWidth, arg.mapHeight);

    fprintf (fp, "\nMap 2\n");
    map_print (fp, map2, arg.mapWidth, arg.mapHeight);

    fprintf (fp, "\nMap 3 = Map 1 & Map 2 (Überschneidung beider Ebenen)\n");
    map_print (fp, map3, arg.mapWidth, arg.mapHeight);

    fprintf (fp, "\nMap 4 = Map 1 | Map 2 (freier Raum zw. beiden Ebenen)\n");
    map_print (fp, map4, arg.mapWidth, arg.mapHeight);

    fprintf (fp, "\nMap 5 (erosion of map 3)\n");
    map_print (fp, map5, arg.mapWidth, arg.mapHeight);

    fclose (fp);

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
    rect_free (head);
    head = NULL;

    map_free (map1, arg.mapWidth);
    map_free (map2, arg.mapWidth);
    map_free (map3, arg.mapWidth);

    renderer = render_destroy (renderer);
    return EXIT_SUCCESS;
}
