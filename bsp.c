
#include "bsp.h"


int room_count = 0;


Vec2
vec2 (const int x, const int y)
{
    Vec2 p = {x, y};
    return p;
}


float
vec2_distance (const Vec2 p1, const Vec2 p2)
{
    return sqrt (pow (p1.x - p2.x, 2) + pow (p1.y - p2.y, 2));
}


Rect *
rect_create (Rect *const parent, const Vec2 pos, const int width, const int height)
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
    rect->split = vec2 (0, 0);
    rect->center = vec2 (pos.x + width / 2, pos.y + height / 2);
    return rect;
}



void
bsp_divide (Rect *const rect)
{
    int d = rng_between (5, 13);
    int f1 = d / 2;
    int f2 = d - d / 2 - 1;

    int offset = 2; 

    if (rect->width >= rect->height)
        rect->split.x = rect->pos.x + rng_between (f1 * rect->width / d, f2 * rect->width / d);
    else
        rect->split.y = rect->pos.y + rng_between (f1 * rect->height / d, f2 * rect->height / d);

    rect->childLeft = rect_create (rect, rect->pos,
            (rect->split.x) ? rect->split.x - rect->pos.x : rect->width,
            (rect->split.y) ? rect->split.y - rect->pos.y : rect->height);

    rect->childRight = rect_create (rect,
            vec2 ((rect->split.x) ? rect->split.x + offset : rect->pos.x,
            (rect->split.y) ? rect->split.y + offset : rect->pos.y),
            (rect->split.x) ? rect->width - rect->split.x + rect->pos.x - offset : rect->width,
            (rect->split.y) ? rect->height - rect->split.y + rect->pos.y - offset : rect->height);
}


Room *
room_closest_to_vec2 (Rect *const rect, const Vec2 vec2)
{
    if (! rect)
        return NULL;

    Room *curr = rect->room;

    Room *left = room_closest_to_vec2 (rect->childLeft, vec2);
    Room *right = room_closest_to_vec2 (rect->childRight, vec2);

    Room *tmp;
    if (left && right)
    {
        float dl = vec2_distance (left->center, vec2);
        float dr = vec2_distance (right->center, vec2);
        tmp = (dl <= dr) ? left : right;
    }
    else
        return rect->room;
    
    float dt = vec2_distance (tmp->center, vec2);
    float dc = vec2_distance (curr->center, vec2);

    return (dc <= dt) ? curr : tmp;
}


int
room_overlap (Room *r1, Room *r2, const int dir, int *const min, int *const max)
{   
    // dir 0 == x
    // dir 1 == y
     
    int _min;
    int _max;
    if (dir)
    {
        // compare x positions
        _min = (r1->pos.x >= r2->pos.x) ? r1->pos.x : r2->pos.x;
        _max = (r1->pos.x + r1->width <= r2->pos.x + r2->width)
            ? r1->pos.x + r1->width
            : r2->pos.x + r2->width;
    }
    else
    {   
        // compare y positions
        _min = (r1->pos.y >= r2->pos.y) ? r1->pos.y : r2->pos.y;
        _max = (r1->pos.y + r1->height <= r2->pos.y + r2->height)
            ? r1->pos.y + r1->height
            : r2->pos.y + r2->height;
    }

    if (_min > _max - 1)
        return 0;
    else
    {
        *min = _min;
        *max = _max - 1;
        return 1;
    }

    return 0;
}


void
room_set (Room *const room, const Vec2 pos, const int width, const int height)
{
    room->pos = pos;
    room->width = width;
    room->height = height;

    room->center = vec2 (pos.x + width / 2, pos.y + height / 2);

    room->next = NULL;

    room->color = 0x8b4513;
}


Room *
room_create (const Vec2 pos, const int width, const int height)
{
    Room *room = malloc (sizeof (Room));
    room_set (room, pos, width, height);
    room->count = room_count++;
    return room;
}


void
room_set_from_vec2 (Room *const room, const Vec2 p1, const Vec2 p2)
{
    int xmin = (p1.x <= p2.x) ? p1.x : p2.x;
    int xmax = (p1.x >= p2.x) ? p1.x : p2.x;
    int ymax = (p1.y >= p2.y) ? p1.y : p2.y;
    int ymin = (p1.y <= p2.y) ? p1.y : p2.y;

    room_set (room, vec2 (xmin, ymin), xmax - xmin, ymax - ymin);
}


Room *
room_create_from_vec2 (const Vec2 p1, const Vec2 p2)
{       
    int xmin = (p1.x <= p2.x) ? p1.x : p2.x;
    int xmax = (p1.x >= p2.x) ? p1.x : p2.x;
    int ymax = (p1.y >= p2.y) ? p1.y : p2.y;
    int ymin = (p1.y <= p2.y) ? p1.y : p2.y;

    return room_create (vec2 (xmin, ymin), xmax - xmin, ymax - ymin);
}


int
signf (const int x, const int f)
{
    return f * ((x > 0) - (x < 0));
}


void
bsp_corridor (Rect *const rect, const Vec2 p)
{
    Room *left = room_closest_to_vec2 (rect->childLeft, p);
    // pick room from right tree closest to result of left tree
    Room *right = room_closest_to_vec2 (rect->childRight, left->center); 


    Room **room;
    Room *tmp;
    if (! rect->room)
        room = &rect->room;
    else
    {
        for (tmp = rect->room; tmp && tmp->next; tmp = tmp->next) ;
        room = &tmp->next;
    }

    
    if (arg.flag_debug)
        printf ("[%i] to [%i] via (%i)\n", left->count, right->count, room_count);
        
    int w = 1;
    int min;
    int max;
    if (room_overlap (left, right, 0, &min, &max))
    {
        // rooms do overlap in x direction
        int dx = right->pos.x - left->pos.x - left->width;
        if (max > min + 1)
            *room = room_create (vec2 (left->pos.x + left->width, min + (max - min) / 2),
                    dx, w);
        else
            *room = room_create (vec2 (left->pos.x + left->width, min),
                    dx, w);

        return;
    }
    else if (room_overlap (left, right, 1, &min, &max))
    {
        // rooms do overlap in y direction
        int dy = right->pos.y - left->pos.y - left->height;
        if (max > min + 1)
            *room = room_create (vec2 (min + (max - min) / 2, left->pos.y + left->height),
                    w, dy);
        else
            *room = room_create (vec2 (min, left->pos.y + left->height),
                    w, dy);

        return;
    }

    // rooms do not overlapp
    // constructing L-shaped corridor

    int dx = right->center.x - left->center.x;
    int dy = right->center.y - left->center.y;
    if (arg.flag_debug)
        printf ("dx: %i\tdy: %i\n", dx, dy);

    if (abs (dx) >= abs (dy))
    {   
        int x = (dx > 0) ? left->pos.x + left->width : left->pos.x;
        *room = room_create_from_vec2 (
                vec2 (x, left->center.y),
                vec2 (x + 100 * dx, left->center.y + w));

        room_overlap (rect->room, right, 1, &min, &max);

        int posx = rng_between (min, max);
        room_set_from_vec2 (*room,
                vec2 (x, left->center.y),
                vec2 (posx, left->center.y + w));
        
        (*room)->next = room_create_from_vec2 (
                vec2 (posx, rect->room->pos.y + (dy > 0 ? 0 : 1)),
                vec2 (posx + w, dy > 0 ? right->pos.y : right->pos.y + right->height));
    }
    else
    {   
        int y = (dy > 0) ? left->pos.y + left->height : left->pos.y;
        *room = room_create_from_vec2 (
                vec2 (left->center.x, y),
                vec2 (left->center.x + w, y + 100 * dy));

        room_overlap (rect->room, right, 0, &min, &max);

        int posy = rng_between (min, max);
        room_set_from_vec2 (*room,
                vec2 (left->center.x, y),
                vec2 (left->center.x + w, posy));

        (*room)->next = room_create_from_vec2 (
                vec2 (rect->room->pos.x + (dx > 0 ? 0 : 1), posy),
                vec2 (dx > 0 ? right->pos.x : right->pos.x + right->width, posy + w));
    }
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
        // TODO if (! rect->childLeft && ! rect->childRight)
        rect->childLeft->color = color;
        rect->childRight->color = color;

        bsp (&(rect->childLeft), iteration - 1, offset);
        bsp (&(rect->childRight), iteration - 1, offset);
    }

    if (rect->childLeft && rect->childRight) // build corridor and exit
    {
        // TODO call bsp_corridor multiple times with diffrent points on higher iteration levels
        bsp_corridor (rect, rect->center);
        return;
    }
    else if (! rect->childLeft && ! rect->childRight) // end node --> create new room
    {
        rect->room = malloc (sizeof (Room));
        rect->room->count = room_count++;
        int w1 = rng_between (MIN_ROOM_SIZE, rect->width - 2 * offset);
        int w2 = rng_between (MIN_ROOM_SIZE, rect->width - 2 * offset);
        rect->room->width = (w1 > w2) ? w1 : w2;

        int h1 = rng_between (MIN_ROOM_SIZE, rect->height - 2 * offset);
        int h2 = rng_between (MIN_ROOM_SIZE, rect->height - 2 * offset);
        rect->room->height = (h1 > h2) ? h1 : h2;

        rect->room->pos = vec2 (rng_between (rect->pos.x + offset, rect->width - rect->room->width - offset),
                rng_between (rect->pos.y + offset, rect->height - rect->room->height - offset));

        rect->room->center = vec2 (rect->room->pos.x + rect->room->width / 2,
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
