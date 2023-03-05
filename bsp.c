
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
    //rect->center = vec2 (pos.x + width / 2, pos.y + height / 2);
    return rect;
}



void
bsp_divide (Rect *const rect)
{
    int d = rng_between (5, 13);
    int f1 = d / 2;
    int f2 = d - d / 2 - 1;

    int offset = 0; 

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
        
    int min;
    int max;
    if (room_overlap (left, right, 0, &min, &max))
    {
        // rooms are parallel in y direction
        int dx = right->pos.x - left->pos.x - left->width;
        if (max > min + 1)
            *room = room_create (vec2 (left->pos.x + left->width, min + (max - min) / 2),
                    dx, arg.corridor_width);
        else
            *room = room_create (vec2 (left->pos.x + left->width, min),
                    dx, arg.corridor_width);

        return;
    }
    else if (room_overlap (left, right, 1, &min, &max))
    {
        // rooms are parallel in x direction
        int dy = right->pos.y - left->pos.y - left->height;
        if (max > min + 1)
            *room = room_create (vec2 (min + (max - min) / 2, left->pos.y + left->height),
                    arg.corridor_width, dy);
        else
            *room = room_create (vec2 (min, left->pos.y + left->height),
                    arg.corridor_width, dy);

        return;
    }

    // rooms are not parallel
    // constructing L-shaped corridor
    int dx = right->center.x - left->center.x;
    int dy = right->center.y - left->center.y;
    if (arg.flag_debug)
        printf ("\tdx: %i\tdy: %i\n", dx, dy);

    if (abs (dx) >= abs (dy))
    {   
        int x = (dx > 0) ? left->pos.x + left->width : left->pos.x;
        *room = room_create_from_vec2 (
                vec2 (x, left->center.y),
                vec2 (x + 100 * dx, left->center.y + arg.corridor_width));

        room_overlap (*room, right, 1, &min, &max);
        if (arg.flag_debug)
        {
            printf ("\t(%i, %i) -> (%i, %i)\n",
                    (*room)->pos.x, (*room)->pos.y,
                    (*room)->pos.x + (*room)->width,
                    (*room)->pos.y + (*room)->height);
            printf ("\tX: min: %i; max: %i\n", min, max);
        }

        int posx = rng_between (min, max);
        room_set_from_vec2 (*room,
                vec2 (x, left->center.y),
                vec2 (posx, left->center.y + arg.corridor_width));
        
        (*room)->next = room_create_from_vec2 (
                vec2 (posx, (*room)->pos.y + (dy > 0 ? 0 : 1)),
                vec2 (posx + arg.corridor_width, dy > 0 ? right->pos.y : right->pos.y + right->height));
    }
    else
    {   
        int y = (dy > 0) ? left->pos.y + left->height : left->pos.y;
        *room = room_create_from_vec2 (
                vec2 (left->center.x, y),
                vec2 (left->center.x + arg.corridor_width, y + 100 * dy));

        room_overlap (*room, right, 0, &min, &max);
        if (arg.flag_debug)
        {
            printf ("\t(%i, %i) -> (%i, %i)\n",
                    (*room)->pos.x, (*room)->pos.y,
                    (*room)->pos.x + (*room)->width,
                    (*room)->pos.y + (*room)->height);
            printf ("\tY: min: %i; max: %i\n", min, max);
        }

        int posy = rng_between (min, max);
        room_set_from_vec2 (*room,
                vec2 (left->center.x, y),
                vec2 (left->center.x + arg.corridor_width, posy));

        (*room)->next = room_create_from_vec2 (
                vec2 ((*room)->pos.x + (dx > 0 ? 0 : 1), posy),
                vec2 (dx > 0 ? right->pos.x : right->pos.x + right->width, posy + arg.corridor_width));
    }   

    (*room)->color = 0xFFA500;
    (*room)->next->color = 0x008000;
}


void
bsp (Rect **r, const int iteration, const int numCorridors)
{    
    if (! r || ! *r)
        return;
    Rect *rect = *r;

    if (rect->width < arg.minRoomSize + 2 || rect->height < arg.minRoomSize + 2)
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

        bsp (&(rect->childLeft), iteration - 1, (numCorridors - 2 > 1) ? numCorridors - 2 : 1);
        bsp (&(rect->childRight), iteration - 1, (numCorridors - 2 > 1) ? numCorridors - 2 : 1);
    }

    if (rect->childLeft && rect->childRight) // build corridor and exit
    {
        // additional corridors to connect the top nodes
        int pos = rect->split.x > 0 ? rect->split.x : rect->split.y;
        int top = rect->split.x > 0 ? rect->pos.y : rect->pos.x;
        int size = rect->split.x > 0 ? rect->width : rect->height;
        Room *prevLeft = NULL;
        Room *prevRight = NULL;
        for (int i = 0; i < numCorridors; i++)
        {
            int rand = rng_between (top + i * size / numCorridors, top + (i + 1) * size / numCorridors);

            Vec2 vec;
            if (rect->split.x)
                vec = vec2 (pos, rand);
            else
                vec = vec2 (rand, pos);

            Room *left = room_closest_to_vec2 (rect->childLeft, vec);
            Room *right = room_closest_to_vec2 (rect->childRight, left->center);

            if (prevLeft == left && prevRight == right)
                continue;

            bsp_corridor (rect, vec);

            prevLeft = left;
            prevRight = right;
        }

        return;
    }
    else if (! rect->childLeft && ! rect->childRight) // end node --> create new room
    {
        int w = rng_between (arg.minRoomSize, rect->width - 2 * arg.roomOffset);
        int h = rng_between (arg.minRoomSize, rect->height - 2 * arg.roomOffset);
        for (int i = 0; i < abs (arg.roomSize); i++)
        {
            int w1 = rng_between (arg.minRoomSize, rect->width - 2 * arg.roomOffset);
            int h1 = rng_between (arg.minRoomSize, rect->height - 2 * arg.roomOffset);

            if (arg.roomSize > 0)
            {
                // advantage
                w = (w1 > w) ? w1 : w;
                h = (h1 > h) ? h1 : h;
            }
            else
            {
                // disadvantage
                w = (w1 < w) ? w1 : w;
                h = (h1 < h) ? h1 : h;
            }
        }

        rect->room = room_create (vec2 (
                    rng_between (rect->pos.x + arg.roomOffset, rect->width - w - arg.roomOffset),
                    rng_between (rect->pos.y + arg.roomOffset, rect->height - h - arg.roomOffset)),
                w, h);
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

    rect_free (rect->childLeft);
    rect_free (rect->childRight);

    rect->childLeft = NULL;
    rect->childRight = NULL;

    room_free (rect->room);
    rect->room = NULL;

    free (rect);
}


void
room_to_map (Room *room)
{
    return;
}


void
bsp_to_map (Rect *const rect, char *map)
{
    if (! rect)
        return;

    static int width;
    static int height;
    if (! rect->parent)
    {
        width = rect->width;
        height = rect->height;
    }

    bsp_to_map (rect->childLeft, map);
    bsp_to_map (rect->childRight, map);

    for (Room *room = rect->room; room; room = room->next)
    {
        room_to_map (room);
    }
}




