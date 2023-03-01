
#ifndef _BSP_
#define _BSP_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rng.h"


#define MIN_ROOM_SIZE 5
#define MIN_RECT_SIZE MIN_ROOM_SIZE + 2


typedef struct
{
    int x;
    int y;
} Point;


typedef struct Room
{
    Point pos;
    int width;
    int height;

    int count;

    Point center;

    int color;

    struct Room *next;
} Room;


typedef struct Rect
{
    Point pos;
    int width;
    int height;

    Point center;

    int color;

    Room *room;

    struct Rect *parent;
    struct Rect *childLeft;
    struct Rect *childRight;
} Rect;


Point point (const int, const int);

Rect *rect_create (Rect *const, const Point, const int, const int);

void bsp (Rect **, const int, const int);


#endif 
