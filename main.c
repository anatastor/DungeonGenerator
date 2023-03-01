
#include <stdio.h>
#include <stdlib.h>

#include "rng.h"
#include "cairo_renderer.h"
#include "bsp.h"

#define GRID    10
#define WIDTH   200
#define HEIGHT  200

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
        render_rect (r, rect->pos.x * GRID, rect->pos.y * GRID,
                rect->width * GRID, rect->height * GRID, rect->color);
    if (0)
        render_line (r, rect->pos.x * GRID, rect->pos.y * GRID,
                (rect->pos.x + rect->width) * GRID, (rect->pos.y + rect->height) * GRID,
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
        render_rect (r, ptr->pos.x * GRID, ptr->pos.y * GRID,
                ptr->width * GRID, ptr->height * GRID, 0x757575);//ptr->color);
        //render_quad (r, ptr->center.x * GRID, ptr->center.y * GRID, GRID, 0xff0000);
    }

    //printf ("C: (%i, %i)\n", rect->center.x, rect->center.y);
    //render_quad (r, rect->center.x * GRID, rect->center.y * GRID, GRID, 0x254117);
}

void
print_koordinates (Rect *rect)
{
    if (! rect)
        return;

    print_koordinates (rect->childLeft);
    print_koordinates (rect->childRight);

    printf ("<%i> Top-Left (%i, %i) + (%i, %i)\n\tMitte (%i, %i)\n",
            rect->room->count,
            rect->room->pos.x, rect->room->pos.y,
            rect->room->width, rect->room->height,
            rect->room->center.x, rect->room->center.y);
}


int
main (int argc, char **argv)
{
    rng_seed (SEED);
    Renderer *renderer = render_create (WIDTH * GRID, HEIGHT * GRID);
    render_fill (renderer, 0x0c090a);

    Rect *head = rect_create (NULL, point (1, 1), WIDTH - 2, HEIGHT - 2);
    bsp (&head, atoi (argv[1]), atoi (argv[2]));
    //draw_rect (renderer, head);
    draw_room (renderer, head);

    print_koordinates (head);
    
    render_grid (renderer, GRID, 0x757575);
    cairo_set_line_width (renderer->cr, 5);
    //render_grid (renderer, 10 * GRID, 0xff0000);


    render_save (renderer, OUTFILE);
    renderer = render_destroy (renderer);
    return EXIT_SUCCESS;
}
