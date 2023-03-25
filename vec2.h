
#ifndef _VEC2_H_
#define _VEC2_H_


#include <math.h>


typedef struct
{
    int x;
    int y;
} Vec2;


Vec2 vec2 (const int x, const int y);
float vec2_distance (const Vec2 v1, const Vec2 v2);


#endif
