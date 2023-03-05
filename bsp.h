
#ifndef _BSP_
#define _BSP_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "arg.h"
#include "rng.h"



typedef struct
{
    int x;
    int y;
} Vec2;


typedef struct Room
{
    Vec2 pos;
    int width;
    int height;

    int count;

    Vec2 center;

    int color;

    struct Room *next;
} Room;



typedef struct Rect
{
    Vec2 pos;
    int width;
    int height;

    Vec2 split;

    int color;

    Room *room;

    struct Rect *parent;
    struct Rect *childLeft;
    struct Rect *childRight;
} Rect;


Vec2 vec2 (const int, const int);



Rect *rect_create (Rect *const, const Vec2, const int, const int);

void bsp (Rect **rect, const int iteration, const int offset);


void rect_free (Rect *rect);


void bsp_to_map (Rect *const rect, char *map);


#endif 
