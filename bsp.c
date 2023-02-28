
#include "bsp.h"


Rect
rect_init (const int posX, const int posY, const int width, const int height)
{
    Rect rect;
    rect.posX = posX;
    rect.posY = posY;
    rect.width = width;
    rect.height = height;

    rect.color = rng_color_hex ();

    rect.parent = NULL;
    rect.left = NULL;
    rect.right = NULL;

    rect.room = NULL;
    return rect;
}


Rect *
rect_create (Rect *const parent, const int posX, const int posY, const int width, const int height)
{
    Rect *rect = malloc (sizeof (Rect));

    rect->posX = posX;
    rect->posY = posY;
    rect->width = width;
    rect->height = height;

    rect->color = rng_color_hex ();

    rect->parent = parent;
    rect->left = NULL;
    rect->right = NULL;

    rect->room = NULL;
}


void
room_free (Room *room)
{
    if (! room)
        return;
    room_free (room->next);
    free (room);
}


void
rect_free (Rect *rect)
{
    if (! rect)
        return;

    rect_free (rect->left);
    rect_free (rect->right);

    room_free (rect->room);
    free (rect);
}


void rect_check_size (Rect **rect, const int min)
{
    if ((*rect)->width < min || (*rect)->height < min)
    {
        free (*rect);
        *rect = NULL;
    }
}


void
bsp_split_v (Rect *rect, const int min)
{   
    int d = rng_between (3, 7);
    int x = rect->posX + rng_between (rect->width / d, 3 * rect->width / d);
    rect->left = rect_create (rect, rect->posX, rect->posY,
            x - rect->posX, rect->height);
    rect->right = rect_create (rect, x, rect->posY,
            rect->width - x + rect->posX, rect->height);
    /*
    rect->left = rect_create (rect, rect->posX + 1, rect->posY + 1,
            x - rect->posX - 2, rect->height - 2);
    rect->right = rect_create (rect, x + 1, rect->posY + 1,
            rect->width - x + rect->posX - 2, rect->height - 2);
    */
}


void
bsp_split_h (Rect *rect, const int min)
{
    int d = rng_between (3, 7);
    int y = rect->posY + rng_between (rect->height / d, 3 * rect->height / d);
    rect->left = rect_create (rect, rect->posX, rect->posY,
            rect->width, y - rect->posY);
    rect->right = rect_create (rect, rect->posX, y,
            rect->width, rect->height - y + rect->posY);
    /*
    rect->left = rect_create (rect, rect->posX + 1, rect->posY + 1,
            rect->width - 2, y - rect->posY - 2);
    rect->right = rect_create (rect, rect->posX + 1, y + 1,
            rect->width - 2, rect->height - y + rect->posY - 2);
    */
}


void
bsp (Rect *rect, const int min, const int iterations)
{
    if (! rect || ! iterations)
        return;
    
    if (rect->width > rect->height)
        bsp_split_v (rect, min);
    else
        bsp_split_h (rect, min);

    rect_check_size (&rect->left, min);
    rect_check_size (&rect->right, min);

    bsp (rect->left, min, iterations - 1);
    bsp (rect->right, min, iterations - 1);
}



Room *
room_create (const int x, const int y, const int w, const int h)
{
    Room *r = malloc (sizeof (Room));
    r->posX = x;
    r->posY = y;
    r->width = w;
    r->height = h;

    r->centerX = x + w / 2;
    r->centerY = y + h / 2;

    r->color = 0x808080;

    r->next = NULL;
    return r;
}


void
create_rooms (Rect *rect, const int min)
{   
    if (! rect)
        return;

    create_rooms (rect->left, min);
    create_rooms (rect->right, min);
    
    if (! rect->left && ! rect->right)
    {
        int w = rng_between (min, rect->width - 2);
        int h = rng_between (min, rect->height - 2);
        int x = rng_between (rect->posX, rect->posX + rect->width - w);
        int y = rng_between (rect->posY, rect->posY + rect->height - h);
        rect->room = room_create (x, y, w, h);
        //rect->room = room_create (x + 1, y + 1, w - 2, h - 2);
        //rect->room = room_create (rect->posX + 1, rect->posY + 1, 3, 3);
        return;
    }

    if (rect->left && rect->right)
        return;
    
    if (rect->parent && rect->left)
    {
        if (rect->parent->left == rect)
            rect->parent->left = rect->left;
        else
            rect->parent->right = rect->left;

        free (rect);
    }
    else if (rect->parent && rect->right)
    {
        if (rect->parent->left == rect)
            rect->parent->left = rect->right;
        else
            rect->parent->right = rect->right;

        free (rect);
    }
}


float
room_distance (Room *r1, Room *r2)
{
    //printf ("%p\t%p\n", r1, r2);
    return sqrt (pow (r1->centerX - r2->centerX, 2) + pow (r1->centerY - r2->centerY, 2));
}


Room *
shortestDistance (Rect *rect, Room *src)
{
    if (! rect)
        return NULL;
    
    Room *left = shortestDistance (rect->left, src);
    Room *right = shortestDistance (rect->right, src);
    if (left && right)
    {
        if (room_distance (src, left) < room_distance (src, right))
            return left;
        else
            return right;
    }
    else if (left)
        return left;
    else if (right)
        return right;
    else
        return rect->room;

    return NULL;
}


int
signf (const int x, const int f)
{
    return f * ((x > 0) - (x < 0));
}


Room *
corridor_create (Room *r1, Room *r2)
{
    int dx = r2->centerX - r1->centerX;
    int dy = r2->centerY - r1->centerY;
    
    int w = 1;

    Room *r;

    if (abs (dx) > abs (dy))
    {
        r = room_create (r1->centerX, r1->centerY, dx + signf (dx, w), w);
        if (abs (dy))
            r->next = room_create (r->posX + dx, r->posY + w, w, dy + signf (dy, w));
    }
    else
    {
        r = room_create (r1->centerX, r1->centerY, w, dy + signf (dy, w));
        if (abs (dx))
            r->next = room_create (r->posX + w, r->posY + dy, dx + signf (dx, w), w);
    }
    
    /*
    r->color = 0x8b4513;
    if (r->next)
        //r->next->color = r->color;
        r->next->color = 0xff8040;
    */

    return r;
}


void
create_corridors (Rect *rect)
{
    if (! rect->left && ! rect->right)
        return;

    if (rect->left)
        create_corridors (rect->left);
    if (rect->right)
        create_corridors (rect->right);

    Room *shortest = shortestDistance (rect->right, rect->left->room);
    rect->room = corridor_create (rect->left->room, shortest);
}


