#ifndef __FIXED_POINT_H__
#define __FIXED_POINT_H__

#include <stdint.h>

typedef uint32_t fixed_point_t;

#define FIXED_POINT_FRACTIONAL_BITS 16

inline fixed_point_t double_to_fixed(double input)
{
    return (fixed_point_t)(input * (1 << FIXED_POINT_FRACTIONAL_BITS));
}

inline double fixed_to_double(fixed_point_t input)
{
    return ((double)input / (double)(1 << FIXED_POINT_FRACTIONAL_BITS));
}

#endif // __FIXED_POINT_H__