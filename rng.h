
#ifndef _rng_
#define _rng_



void rng_seed (const unsigned);
int rng (void);
int rng_between (const int, const int);

float rng_pct (void);

int rng_color_hex (void);


#endif
