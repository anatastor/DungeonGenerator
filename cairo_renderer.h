
#ifndef _cairo_renderer_
#define _cairo_renderer_


#include <stdio.h>
#include <stdlib.h>

#include <cairo.h>


typedef struct
{
    cairo_surface_t *surface;
    cairo_t *cr;
    int width;
    int height;
} Renderer;


Renderer *render_create (const int, const int);
Renderer *render_destroy (Renderer *);

void render_setColor (Renderer *, const int);

void render_render (Renderer *);


void render_fill (Renderer *, const int);
void render_line (Renderer *, const int, const int, const int, const int, const int, const int);
void render_rect (Renderer *r, const int, const int, const int, const int, const int);
void render_quad (Renderer *r, const int, const int, const int, const int);

void render_grid (Renderer *r, const int, const int);




void render_save (Renderer *, const char *);



#endif
