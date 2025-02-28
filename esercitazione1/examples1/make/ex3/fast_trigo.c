#include "fast_trigo.h"

// ERROR: Functions are redefined here!
#define fastSin_func
#define fastCos_func
float fast_sin(float x)
{
    return sinf(x);
}

float fast_cos(float x)
{
    return cosf(x);
}
