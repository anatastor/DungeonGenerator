
#include "rng.h"


static unsigned long _rng_next = 1;


void
rng_seed (const unsigned seed)
{
    _rng_next = seed;
}


int
rng (void)
{
    int a = 1103515245;
    int c = 12345;
    int m = 2147483648;
    _rng_next = (_rng_next * a + c) % m;
    int ret = (int) _rng_next;
    ret = ret & (~ (1 << sizeof (int) * 8 - 1)); // correct output to 0 - ...
    return ret;
}

int
rng_between (const int min, const int max)
{
    if (min >= max)
        return min;
    return min + (rng () % (max - min));
}


float
rng_pct (void)
{
    return (float) rng_between (0, 1000000000) / 1000000000.0;
}


int
rng_color_hex (void)
{
    int r = rng_between (0, 255);
    int g = rng_between (0, 255);
    int b = rng_between (0, 255);
    return b + (g << 8) + (r << 16);
}

