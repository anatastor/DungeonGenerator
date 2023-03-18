
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
render_line (Renderer *r, const int x1, const int y1, const int x2, const int y2, const int w, const int hex)
{   
    cairo_set_line_width (r->cr, w);
    render_setColor (r, hex);
    cairo_move_to (r->cr, x1, y1);
    cairo_line_to (r->cr, x2, y2);
    cairo_stroke (r->cr);
    cairo_set_line_width (r->cr, 2);
}


void
render_rect (Renderer *r, const int x, const int y, const int w, const int h, const int hex)
{
    render_setColor (r, hex);
    cairo_rectangle (r->cr, x, y, w, h);
    cairo_fill (r->cr);
}

void
render_quad (Renderer *r, const int x, const int y, const int size, const int color)
{
    render_rect (r, x, y, size, size, color);
}


void
render_grid (Renderer *r, const int size, const int color)
{
    render_setColor (r, color);
    for (int x = 0; x < r->width / size; x++)
        _render_line (r, x * size, 0, x * size, r->height);

    for (int y = 0; y < r->height / size; y++)
        _render_line (r, 0, y * size, r->width, y * size); 

    cairo_stroke (r->cr);
}


void
render_map (Renderer *r, const char *const map, const int width, const int height, const int size)
{
    static const int colors[] = {
        0x0c090a,
        0x757575,
        0x008000,
        0x800000
    };
    render_fill (r, 0x0c090a);
    for (int i = 0; i < width * height; i++)
        /*
        if (i / width == 0 ||
            i / width == height - 1 ||
            i % width == 0 ||
            i % width == height - 1)
            render_quad (r, i % width * size, i / height * size, size, 0x0c090A);
        else
        */
        render_quad (r, i % width * size, i / width * size, size, colors[map[i]]);
}


void
render_save (Renderer *r, const char *const path)
{
    cairo_surface_write_to_png (r->surface, path);
}


