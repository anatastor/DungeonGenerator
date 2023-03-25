
#include "bsp.h"

/*** private global variables ***/

int p_bspDebug = 0;
int p_roomCount = 0;
int p_bsp_parameters[e_BspParameters_Size] = {0};


/*** private functions ***/

void
bsp_divide (Rect *const rect)
{
    int d = rng_between (5, 13);
    int f1 = d / 2;
    int f2 = d - d / 2 - 1;
    
    int offset = p_bsp_parameters[e_BspParameters_RectOffset];

    if (rect->width >= rect->height)
        rect->split.x = rect->pos.x + rng_between (f1 * rect->width / d, f2 * rect->width / d);
    else
        rect->split.y = rect->pos.y + rng_between (f1 * rect->height / d, f2 * rect->height / d);

    rect->childLeft = rect_create (rect, rect->pos,
            (rect->split.x) ? rect->split.x - rect->pos.x : rect->width,
            (rect->split.y) ? rect->split.y - rect->pos.y : rect->height);

    int x = (rect->split.x) ? rect->split.x + offset : rect->pos.x;
    int y = (rect->split.y) ? rect->split.y + offset : rect->pos.y;
    x = (x < 0) ? 0 : x;
    y = (y < 0) ? 0 : y;

    rect->childRight = rect_create (rect,
            vec2 (x, y),
            (rect->split.x) ? rect->width - rect->split.x + rect->pos.x - offset : rect->width,
            (rect->split.y) ? rect->height - rect->split.y + rect->pos.y - offset : rect->height);
}


Room *
room_create (const Vec2 pos, const int width, const int height)
{
    Room *room = malloc (sizeof (Room));

    room->num = p_roomCount++;

    room->pos = pos;
    room->width = width ? width : 1;
    room->height = height ? height : 1;

    // guarantee pos to always be the top left
    if (width < 0)
    {
        room->pos.x = pos.x + width;
        room->width = -width;
    }
    if (height < 0)
    {
        room->pos.y = pos.y + height;
        room->height = -height;
    }

    room->center = vec2 (pos.x + width / 2, pos.y + height / 2);

    room->next = NULL;
    
    if (p_bspDebug)
    {
        printf ("[%i]\n\tTopLeft: (%i, %i)\n\tw: %i\th: %i\n", room->num,
                room->pos.x, room->pos.y,
                room->width, room->height);
    }
    return room;
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

    rect->pos = pos;
    rect->width = width;
    rect->height = height;
    rect->split = vec2 (0, 0);

    return rect;
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
    
    if (curr)
    {
        float dt = vec2_distance (tmp->center, vec2);
        float dc = vec2_distance (curr->center, vec2);
        return (dc <= dt) ? curr : tmp;
    }

    return tmp;
}


int
room_overlap (Room *r1, Room *r2, const int dir, int *const min, int *const max)
{   
    // dir 0 == x
    // dir 1 == y
     
    int _min = 0;
    int _max = 0;
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


Room *
bsp_corridor_overlap (Room *const left, Room *const right)
{   
    int min = 0;
    int max = 0;

    int cw = p_bsp_parameters[e_BspParameters_CorridorWidth];


    if (room_overlap (left, right, 0, &min, &max))
    {
        // rooms are overlapping in y direction
        if (p_bspDebug)
            printf ("overlap Y %i, %i, %i\n", min, max, cw);

        int dx = right->pos.x - left->pos.x - left->width;
        if (dx == 0) return NULL; // rooms are already overlapping
            
        // TODO corridor not only in the middle
        if (max > min + cw)
            return room_create (vec2 (left->pos.x + left->width, min + (max - min - cw) / 2),
                    dx, (max - min > cw) ? cw : max - min + 1);
        else
            return room_create (vec2 (left->pos.x + left->width, min + 1),
                    dx, max - min - 1);
    }
    
    if (room_overlap (left, right, 1, &min, &max))
    {
        // rooms are overlapping in x direction
        if (p_bspDebug)
            printf ("overlap X: %i, %i, %i\n", min, max, cw);

        int dy = right->pos.y - left->pos.y - left->height;
        if (dy == 0) return NULL; // rooms are already overlapping
            
        // TODO corridor not only in the middle
        if (max > min + cw)
            return room_create (vec2 (min + (max - min - cw) / 2, left->pos.y + left->height),
                    (max - min > cw) ? cw : max - min + 1, dy);
        else
            return room_create (vec2 (min + 1, left->pos.y + left->height),
                    max - min - 1, dy);
    }

    return NULL;
}


void
bsp_corridor (Rect *const rect, const Vec2 p)
{
    Room *left = room_closest_to_vec2 (rect->childLeft, p);
    // pick room from right tree closest to result of left tree
    Room *right = room_closest_to_vec2 (rect->childRight, left->center); 

    int cw = p_bsp_parameters[e_BspParameters_CorridorWidth];

    Room **room;
    Room *tmp;
    if (! rect->room)
        room = &rect->room;
    else
    {
        for (tmp = rect->room; tmp && tmp->next; tmp = tmp->next) ;
        room = &tmp->next;
    }
    
    if (p_bspDebug)
        printf ("[%i] to [%i] via (%i)\n", left->num, right->num, p_roomCount);

    *room = bsp_corridor_overlap (left, right);
    if (*room) return;

    // constructing L-shaped corridor
    int dx = right->center.x - left->center.x;
    int dy = right->center.y - left->center.y;
    if (p_bspDebug)
        printf ("L:\t dx: %i\tdy: %i\n", dx, dy);

    if (abs (dx) >= abs (dy))
    {   
        int x = (dx > 0) ? left->pos.x + left->width : left->pos.x; 

        if (left->height > cw)
            *room = room_create (vec2 (x, left->pos.y + (left->height - cw) / 2),
                    right->pos.x + right->width - x, cw);
        else
            *room = room_create (vec2 (x, left->pos.y + 1),
                    right->pos.x + right->width - x, left->height - 2);

        (*room)->next = bsp_corridor_overlap (*room, right);
    }
    else
    {   
        int y = (dy > 0) ? left->pos.y + left->height : left->pos.y;

        if (left->width > cw)
            *room = room_create (vec2 (left->pos.x + (left->width - cw) / 2, y),
                    cw, right->pos.y + right->height - y);
        else
            *room = room_create (vec2 (left->pos.x + 1, y),
                    left->width - 2, right->pos.y + right->height - y);

        (*room)->next = bsp_corridor_overlap (*room, right);
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

/*** global functions ***/


void
bsp_set_debug () {p_bspDebug = 1;}


void
bsp_set_parameters (int *parameters)
{   
    for (int i = 0; i < e_BspParameters_Size; i++)
        p_bsp_parameters[i] = parameters[i];
}


void
bsp (Rect **r, const int iteration, const int numCorridors)
{    
    if (! r || ! *r)
        return;
    Rect *rect = *r;

    int minRoomSize = p_bsp_parameters[e_BspParameters_MinRoomSize];
    int roomSize = p_bsp_parameters[e_BspParameters_RoomSize];
    int roomOffset = p_bsp_parameters[e_BspParameters_RoomOffset];

    if (rect->width < minRoomSize + roomOffset ||
            rect->height < minRoomSize + roomOffset)
    {
        free (*r);
        *r = NULL;
        return;
    }

    if (iteration)
    {
        bsp_divide (rect);
        // TODO if (! rect->childLeft && ! rect->childRight)

        bsp (&(rect->childLeft), iteration - 1, (numCorridors - 2 > 1) ? numCorridors - 2 : 1);
        bsp (&(rect->childRight), iteration - 1, (numCorridors - 2 > 1) ? numCorridors - 2 : 1);
    }

    if (rect->childLeft && rect->childRight) // build corridor and exit
    {
        int pos = rect->split.x > 0 ? rect->split.x : rect->split.y;
        int top = rect->split.x > 0 ? rect->pos.y : rect->pos.x;
        int size = rect->split.x > 0 ? rect->width : rect->height;
        Room *prevLeft = NULL;
        Room *prevRight = NULL;
        for (int i = 0; i < numCorridors; i++)
        {
            int rand = rng_between (top + i * size / numCorridors, top + (i + 1) * size / numCorridors);

            Vec2 vec = vec2 (0, 0);
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
        int w = rng_between (minRoomSize, rect->width - 2 * roomOffset);
        int h = rng_between (minRoomSize, rect->height - 2 * roomOffset);
        for (int i = 0; i < abs (roomSize); i++)
        {
            int w1 = rng_between (minRoomSize, rect->width - 2 * roomOffset);
            int h1 = rng_between (minRoomSize, rect->height - 2 * roomOffset);

            if (roomSize > 0)
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
                    rng_between (rect->pos.x + roomOffset, rect->width - w - roomOffset),
                    rng_between (rect->pos.y + roomOffset, rect->height - h - roomOffset)),
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
Rect_free (Rect *rect)
{
    if (! rect)
        return;
    Rect_free (rect->childLeft);
    Rect_free (rect->childRight);

    room_free (rect->room);
    free (rect);
}

