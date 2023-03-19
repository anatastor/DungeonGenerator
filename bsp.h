
#ifndef _BSP_
#define _BSP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "arg.h"
#include "rng.h"


struct BspOptions
{
    int iterations;
    int corridorWidth;
    int roomSize;
    int minRoomSize;
    int roomOffset;
};

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

    Vec2 center;

    struct Room *next;
} Room;



typedef struct Rect
{
    Vec2 pos;
    int width;
    int height;

    Vec2 split;

    Room *room;

    struct Rect *parent;
    struct Rect *childLeft;
    struct Rect *childRight;
} Rect;



enum Stuff
{
    e_empty = 0,
    e_room,
    e_stair_up,
    e_stair_down,
    e_wall
};


void bsp_set_option (char *const name, int value);


Vec2 vec2 (const int, const int);



Rect *rect_create (Rect *const, const Vec2, const int, const int);

void bsp (Rect **rect, const int iteration, const int offset);


void Rect_free (Rect *rect);


#endif 
