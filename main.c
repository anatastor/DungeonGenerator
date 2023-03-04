
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
                ptr->width * arg.grid_size, ptr->height * arg.grid_size, 0x757575);//ptr->color);
        //render_quad (r, ptr->center.x * arg.grid_size, ptr->center.y * arg.grid_size, arg.grid_size, 0xff0000);
    }

    //printf ("C: (%i, %i)\n", rect->center.x, rect->center.y);
    //render_quad (r, rect->center.x * arg.grid_size, rect->center.y * arg.grid_size, arg.grid_size, 0x254117);
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
    // INIT
    argp_parse (&argp, argc, argv, 0, 0, NULL);
    rng_seed (SEED);
    Renderer *renderer = render_create (arg.map_width * arg.grid_size, arg.map_height * arg.grid_size);
    render_fill (renderer, 0x0c090a);

    Rect *head = rect_create (NULL, vec2 (1, 1), arg.map_width - 2, arg.map_height - 2);
    bsp (&head, arg.iterations, 0);
    draw_rect (renderer, head);
    draw_room (renderer, head);

    if (arg.flag_debug)
        print_koordinates (head);
    
    render_grid (renderer, arg.grid_size, 0x757575);
    cairo_set_line_width (renderer->cr, 5);
    //render_grid (renderer, 10 * arg.grid_size, 0xff0000);


    render_save (renderer, arg.output_file);
    renderer = render_destroy (renderer);
    return EXIT_SUCCESS;
}
