
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


void
fprint_map (FILE *const fp, const char *const map)
{
    for (int i = 0; i < arg.mapWidth * arg.mapHeight; i++)
    {
        fprintf (fp, "%c", map[i] == e_room ? 'X' : ' ');
        if (i % arg.mapWidth == arg.mapWidth - 1)
            fprintf (fp, "\n");
    }
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
    char *map_f = malloc (sizeof (char) * arg.mapWidth * arg.mapHeight);

    // Karten vergleichen
    for (int i = 0; i < arg.mapWidth * arg.mapHeight; i++)
        map_f[i] = map[i] & map2[i];

    fprint_map (stdout, map);

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

    renderer = render_destroy (renderer);
    return EXIT_SUCCESS;
}
