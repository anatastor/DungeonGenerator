
#ifndef _BSP_
#define _BSP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#include "rng.h"
#include "vec2.h"


enum 
{
    e_BspParameter_Iterations = 0,
    e_BspParameter_RoomSize,
    e_BspParameter_MinRoomSize,
    e_BspParameter_RoomOffset,
    e_BspParameter_RectOffset,
    e_BspParameter_CorridorWidth,
    e_BspParameter_NumCorridors,
    e_BspParameter_SIZE_
};


static const char *bsp_parameter_keys[] = {
    "iterations",
    "roomSize",
    "minRoomSize",
    "roomOffset",
    "rectOffset",
    "corridorWidth",
    "numCorridors"
};




typedef struct Room
{
    Vec2 pos;
    int width;
    int height;

    int num;

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


void bsp_set_debug ();
void bsp_set_parameters (int *const parameters);


Rect *rect_create (Rect *const, const Vec2, const int, const int);

void bsp (Rect **rect, const int iteration, const int offset);


void Rect_free (Rect *rect);


#endif 
