
#include "cairo_renderer.h"



Renderer *
render_create (const int width, const int height)
{
    Renderer *r = calloc (sizeof (Renderer), 1);
    r->surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
    r->cr = cairo_create (r->surface);
    cairo_set_line_width (r->cr, 2);

    r->width = width;
    r->height = height;
    return r;
}


Renderer *
render_destroy (Renderer *r)
{
    cairo_destroy (r->cr);
    cairo_surface_destroy (r->surface);
    free (r);
    return NULL;
}


void
render_setColor (Renderer *r, const int hex)
{
    unsigned _r = (hex >> 16) & 255;
    unsigned _g = (hex >> 8) & 255;
    unsigned _b = hex & 255;
    cairo_set_source_rgb (r->cr, (double) _r / 255.0, (double) _g / 255.0, (double) _b / 255.0);
}


void
render_render (Renderer *r)
{
    cairo_stroke (r->cr);
}


void
render_fill (Renderer *r, const int hex)
{
    render_setColor (r, hex);
    cairo_paint (r->cr);
}


void
_render_line (Renderer *r, const int x1, const int y1, const int x2, const int y2)
{
    cairo_move_to (r->cr, x1, y1);
    cairo_line_to (r->cr, x2, y2);
}


void
render_line (Renderer *r, const int x1, const int y1, const int x2, const int y2, const int hex)
{
    render_setColor (r, hex);
    cairo_move_to (r->cr, x1, y1);
    cairo_line_to (r->cr, x2, y2);
    cairo_stroke (r->cr);
}


void
render_rect (Renderer *r, const int x, const int y, const int w, const int h, const int hex)
{
    render_setColor (r, hex);
    cairo_rectangle (r->cr, x, y, w, h);
    cairo_fill (r->cr);
}

void
render_quad (Renderer *r, const int x, const int y, const int s, const int hex)
{
    render_rect (r, x, y, s, s, hex);
}


void
render_grid (Renderer *r, const int size, const int hex)
{
    render_setColor (r, hex);
    for (int x = 0; x < r->width / size; x++)
        _render_line (r, x * size, 0, x * size, r->height);

    for (int y = 0; y < r->height / size; y++)
        _render_line (r, 0, y * size, r->width, y * size); 

    cairo_stroke (r->cr);
}


void
render_save (Renderer *r, const char *path)
{
    cairo_surface_write_to_png (r->surface, path);
}


