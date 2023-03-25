
#include "vec2.h"


Vec2
vec2 (const int x, const int y)
{
    Vec2 v = {x, y};
    return v;
}


float
vec2_distance (const Vec2 v1, const Vec2 v2)
{
    return sqrt (pow (v1.x - v2.x, 2) + pow (v1.y - v2.y, 2));
}
