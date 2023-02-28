
#include "bsp.h"


Point
point (const int x, const int y)
{
    Point p = {x, y};
    return p;
}


float
point_distance (const Point p1, const Point p2)
{
    return sqrt (pow (p1.x - p2.x, 2) + pow (p1.y - p2.y, 2));
}


Rect *
rect_create (Rect *const parent, const Point pos, const int width, const int height)
{
    Rect *rect = malloc (sizeof (Rect));
    if (! rect)
        return NULL;

    rect->parent = parent;
    rect->childLeft = NULL;
    rect->childRight = NULL;
    rect->room = NULL;

    rect->color = rng_color_hex ();


    rect->pos = pos;
    rect->width = width;
    rect->height = height;
    rect->center = point (pos.x + width / 2, pos.y + height / 2);
    return rect;
}



void
bsp_divide (Rect *const rect)
{
    int d = rng_between (5, 13);
    int f1 = d / 2;
    int f2 = d - d / 2 - 1;

    int offset = 0; 

    if (rect->width > rect->height)
    {
        int x = rect->pos.x + rng_between (f1 * rect->width / d, f2 * rect->width / d);
        rect->childLeft = rect_create (rect, point (rect->pos.x + 1, rect->pos.y + 1),
                x - rect->pos.x - 2, rect->height - 2);
        rect->childRight = rect_create (rect, point (x + 1, rect->pos.y + 1),
                rect->width - x + rect->pos.x - 2, rect->height - 2);
        /*
        rect->childLeft = rect_create (rect, rect->pos,
                x - rect->pos.x, rect->height);
        rect->childRight = rect_create (rect, point (x + offset, rect->pos.y),
                rect->width - x + rect->pos.x - offset, rect->height);
        */
    }
    else
    {
        int y = rect->pos.y + rng_between (f1 * rect->height / d, f2 * rect->height / d);
        
        rect->childLeft = rect_create (rect, point (rect->pos.x + 1, rect->pos.y + 1),
                rect->width - 2, y - rect->pos.y - 2);
        rect->childRight = rect_create (rect, point (rect->pos.x + 1, y + 1),
                rect->width - 2, rect->height - y + rect->pos.y - 2);
        /*
        rect->childLeft = rect_create (rect, rect->pos,
                rect->width, y - rect->pos.y);
        rect->childRight = rect_create (rect, point (rect->pos.x, y + offset),
                rect->width, rect->height - y + rect->pos.y - offset);
        */
    }
}


Room *
room_closest_to_point (Rect *const rect, const Point point, const int iteration, Room **left, Room **right)
{   
    if (! rect)
        return NULL;

    Room *_left = room_closest_to_point (rect->childLeft, point, iteration + 1, NULL, NULL);
    Room *_right = room_closest_to_point (rect->childRight, point, iteration + 1, NULL, NULL);

    if (! iteration)
    {
        *left = _left;
        *right = _right;
        return NULL;
    }

    Room *tmp;

    if (_left && _right)
    {
        float dl = point_distance (_left->center, point);
        float dr = point_distance (_right->center, point);
        tmp = (dl <= dr) ? _left : _right;
    }
    else if (_left)
        tmp = _left;
    else if (_right)
        tmp = _right;
    else
        tmp = rect->room;

    if (iteration == 1)
    {
        float dc = point_distance (rect->room->center, point);
        float dt = point_distance (tmp->center, point);
        return (dc <= dt) ? rect->room : tmp;
    }
    else
        return tmp;

    return NULL;
}


int
room_raycast (Room *r1, Room *r2, const int dir)
{   
    // dir 0 == x
    // dir 1 == y
    if (dir == 1)
    {
        //printf ("X: %i, %i\n", r1->center.x, r2->center.x);
        for (int i = 0; i <= r1->width; i++)
        {
            int x = r1->pos.x + i;
            if (x >= r2->pos.x && x <= r2->pos.x + r2->width)
                return x;
        }
    }
    else if (dir == 0)
    {
        //printf ("Y: %i, %i\n", r1->center.y, r2->center.y);
        for (int i = 0; i <= r1->height; i++)
        {
            int y = r1->pos.y + i;
            if (y >= r2->pos.y && y <= r2->pos.y + r2->height)
                return y;
        }
    }

    return -1;
}


Room *
room_create (const Point pos, const int width, const int height)
{
    Room *room = malloc (sizeof (Room));
    room->pos = pos;
    room->width = width;
    room->height = height;

    room->center = point (pos.x + width / 2, pos.y + height / 2);

    room->next = NULL;

    room->color = 0x8b4513;
    return room;
}

void
bsp_corridor (Rect *const rect)
{
    Room *left = NULL;
    Room *right = NULL;
    room_closest_to_point (rect, rect->center, 0, &left, &right);


    printf ("[%p] [%p]\n", left, right);
    printf ("C: (%i, %i) (%i, %i) (%i, %i)\n",
            rect->center.x, rect->center.y,
            left->center.x, left->center.y,
            right->center.x, right->center.y);

    int dx = right->center.x - left->center.x;
    int dy = right->center.y - left->center.y;

    printf ("[%i, %i]\n", dx, dy);
    rect->room = room_create (point (left->center.x, left->center.y), 2, 2);
    
    if (abs (dx) >= abs (dy))
    {   
        rect->room->color = 0xff0000;
        /*
        int y = room_raycast (left, right, 0);
        if (y >= 0) // direct connection possible
            rect->room = room_create (point (left->center.x, y), dx, 1);
        */
    }
    else
    {
        /*
        int x = room_raycast (left, right, 1);
        if (x >= 0)
            rect->room = room_create (point (x, left->center.y), 1, dy);
        */
    }

    printf ("<%i, %i>\n", rect->room->center.x, rect->room->center.y);
}


void
bsp (Rect **r, const int iteration, const int offset)
{    
    if (! r || ! *r)
        return;
    Rect *rect = *r;

    if (rect->width < MIN_RECT_SIZE || rect->height < MIN_RECT_SIZE)
    {
        free (*r);
        *r = NULL;
        return;
    }

    if (iteration)
    {
        bsp_divide (rect);
        int color = rng_color_hex ();
        rect->childLeft->color = color;
        rect->childRight->color = color;

        bsp (&(rect->childLeft), iteration - 1, offset);
        bsp (&(rect->childRight), iteration - 1, offset);
    }

    if (rect->childLeft && rect->childRight) // build corridor and exit
    {
        // get the room of each subtree which is closest to the other trees center
        bsp_corridor (rect);
        return;
    }
    else if (! rect->childLeft && ! rect->childRight) // end node --> create new room
    {
        rect->room = malloc (sizeof (Room));
        int w1 = rng_between (MIN_ROOM_SIZE, rect->width - 2 * offset);
        int w2 = rng_between (MIN_ROOM_SIZE, rect->width - 2 * offset);
        rect->room->width = (w1 > w2) ? w1 : w2;

        int h1 = rng_between (MIN_ROOM_SIZE, rect->height - 2 * offset);
        int h2 = rng_between (MIN_ROOM_SIZE, rect->height - 2 * offset);
        rect->room->height = (h1 > h2) ? h1 : h2;

        rect->room->pos = point (rng_between (rect->pos.x + offset, rect->width - rect->room->width - offset),
                rng_between (rect->pos.y + offset, rect->height - rect->room->height - offset));

        rect->room->center = point (rect->room->pos.x + rect->room->width / 2,
                rect->room->pos.y + rect->room->height / 2);
        
        rect->room->color = 0xc0c0c0;

        rect->room->next = NULL;
    }   
    else if (iteration && rect->parent) // only one node missing --> update parent
    {
        Rect **tmp = (rect->parent->childLeft == rect)
            ? &rect->parent->childLeft
            : &rect->parent->childRight;

        if (rect->childLeft && ! rect->childRight)
            *tmp = rect->childLeft;
        else if (! rect->childLeft && rect->childRight)
            *tmp = rect->childRight;
        free (rect);
    }
}
