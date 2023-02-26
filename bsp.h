
#ifndef _bsp_
#define _bsp_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rng.h"


typedef struct Room
{
    int posX;
    int posY;
    int width;
    int height;

    int centerX;
    int centerY;

    int color;

    struct Room *next;
} Room;


typedef struct Rect
{
    int posX;
    int posY;
    int width;
    int height;
    int color;

    struct Rect *parent;
    struct Rect *left;
    struct Rect *right;
    
    Room *room;
} Rect;


Rect rect_init (const int, const int, const int, const int);
Rect *rect_create (Rect *const, const int, const int, const int, const int);

void rect_free (Rect *rect);



void bsp (Rect *, const int, const int);

Room *room_create (const int, const int, const int, const int);


void create_rooms (Rect *, const int);
void create_corridors (Rect *);


#endif 
