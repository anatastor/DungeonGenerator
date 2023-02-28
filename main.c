
#include <stdio.h>
#include <stdlib.h>
 
#include "rng.h"
#include "cairo_renderer.h"
#include "bsp.h"

#define GRID    25
#define WIDTH   120
#define HEIGHT  80


#define _OUT    "out.png"


void
rect_draw (Renderer *r, Rect *rect)
{
    if (! rect)
        return;

    render_rect (r, rect->posX * GRID, rect->posY * GRID,
            rect->width * GRID, rect->height * GRID, rect->color);

    rect_draw (r, rect->left);
    rect_draw (r, rect->right);
}


void
rect_draw_rooms (Renderer *r, Rect *rect)
{
    if (! rect)
        return;

    rect_draw_rooms (r, rect->left);
    rect_draw_rooms (r, rect->right);

    for (Room *ptr = rect->room; ptr; ptr = ptr->next)
    {
        //printf ("\n%i, %i, %i, %i\n(%i, %i)\n", ptr->posX, ptr->posY, ptr->width, ptr->height, ptr->centerX, ptr->centerY);
        render_rect (r, ptr->posX * GRID, ptr->posY * GRID,
                ptr->width * GRID, ptr->height * GRID, ptr->color);
                //ptr->width * GRID, ptr->height * GRID, 0x808080);
        //render_line (r, ptr->posX * GRID, ptr->posY * GRID, (ptr->posX + ptr->width) * GRID, (ptr->posY + ptr->height) * GRID, 0x9f000f);
        /*
        render_quad (r, ptr->posX * GRID, ptr->posY * GRID, GRID, 0x9f000f);
        render_quad (r, (ptr->posX + ptr->width) * GRID, (ptr->posY + ptr->height) * GRID, GRID, 0x9f000f);
        */
    }
}


int
main (int argc, char** argv)
{
    rng_seed (12355312);
    Renderer *renderer = render_create (GRID * WIDTH, GRID * HEIGHT);
    
    render_fill (renderer, 0x0c090a);
    //render_fill (render, 0xffffff);
    
    Rect *head = rect_create (NULL, 20, 1, WIDTH - 22, HEIGHT - 2);
    bsp (head, 7, 13);

    create_rooms (head, 7);
    //rect_draw (renderer, head);
    create_corridors (head);
    rect_draw_rooms (renderer, head);

    render_grid (renderer, GRID, 0x757575);
    
    render_save (renderer, _OUT);
    
    rect_free (head);
    renderer = render_destroy (renderer);
    return EXIT_SUCCESS;
}
