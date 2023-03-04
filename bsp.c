
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
room_raycast (Room *r1, Room *r2, const int dir, int *const min, int *const max)
{   
    // dir 0 == x
    // dir 1 == y
    if (dir == 1)
    {
        int xmin = 0;
        int xmax = 0;
        int x;
        for (int i = 0; i <= r1->width; i++)
        {
            x = r1->pos.x + i;
            if (x >= r2->pos.x && x <= r2->pos.x + r2->width)
            {
                if (! xmin)
                    xmin = x;
                else
                    xmax = x;
            }
        }
        if (! xmax)
            xmax = x;

        if (xmin && xmax)
        {
            *min = xmin;
            *max = xmax;
            return 1;
        }
    }
    else if (dir == 0)
    {
        int ymin = 0;
        int ymax = 0;
        int y;
        for (int i = 0; i <= r1->height; i++)
        {
            y = r1->pos.y + i;
            if ((y >= r2->pos.y && y <= r2->pos.y + r2->height))
            {
                if (! ymin)
                    ymin = y;
                else
                    ymax = y;
            }
        }
        if (! ymax)
            ymax = y;

        if (ymin && ymax)
        {
            *min = ymin;
            *max = ymax;
            return 1;
        }
    }

    return 0;
}


Room *
room_create (const Vec2 pos, const int width, const int height)
{
    Room *room = malloc (sizeof (Room));
    room->pos = pos;
    room->width = width;
    room->height = height;
    room->count = room_count++;

    room->center = vec2 (pos.x + width / 2, pos.y + height / 2);

    room->next = NULL;

    room->color = 0x8b4513;
    return room;
}


int
signf (const int x, const int f)
{
    return f * ((x > 0) - (x < 0));
}


void
bsp_corridor (Rect *const rect)
{
    Room *left = room_closest_to_vec2 (rect->childLeft, rect->center);
    Room *right = room_closest_to_vec2 (rect->childRight, rect->center);
    //room_closest_to_vec2 (rect, rect->center, 0, &left, &right);
    /*
    printf ("[%p] [%p]\n", left, right);
    printf ("C: (%i, %i) (%i, %i) (%i, %i)\n",
            rect->center.x, rect->center.y,
            left->center.x, left->center.y,
            right->center.x, right->center.y);
    */

    int dx = right->center.x - left->center.x;
    int dy = right->center.y - left->center.y;

    //printf ("<%i> --> <%i>\n\tdx: %i\tdy: %i\n", left->count, right->count, dx, dy);
    //printf ("<%i> dx, dy: %i, %i\n", room_count, dx, dy);
    //rect->room = room_create (vec2 (left->center.x, left->center.y), dx, dy);

    int w = 1;
    /*
    if (abs (dx) == abs (dy))
    {
        rect->room = room_create (vec2 (left->center.x, left->center.y), dx, dy);
        rect->room->color = 0xff0000;
    }
    */
    if (abs (dx) >= abs (dy))
    { 
        int min;
        int max;
        if (room_raycast (left, right, 0, &min, &max))
            rect->room = room_create (vec2 (left->center.x, rng_between (min, max)), dx, w);
        else
        {
            rect->room = room_create (vec2 (left->center.x, left->center.y), dx + signf (dx, w), w);
            rect->room->next = room_create (vec2 (rect->room->pos.x + dx, rect->room->pos.y + w), w, dy + signf (dy, w));
        }
        rect->room->color = 0x524117; //rng_color_hex ();
    }
    else
    {
        int min;
        int max;
        if (room_raycast (left, right, 1, &min, &max))
        {
            //printf ("MIN: %i\tMAX: %i\n", min, max);
            rect->room = room_create (vec2 (rng_between (min, max), left->center.y), w, dy);
        }
        else
        {
            rect->room = room_create (vec2 (left->center.x, left->center.y), w, dy + signf (dy, w));
            rect->room->next = room_create (vec2 (rect->room->pos.x, rect->room->pos.y + dy), dx + signf (dx, w), w);
        }

        //rect->room->color = rng_color_hex ();
        rect->room->color = 0x524117; //rng_color_hex ();
    }

    //printf ("<%i, %i>\n", rect->room->center.x, rect->room->center.y);
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
        bsp_corridor (rect);
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
