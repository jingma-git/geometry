#pragma once

namespace egl
{
    const double DBL_EPS = 1e-14;
    const double FLOAT_EPS = 1e-7;

    M_INLINE bool is_equal(double a, double b);
    M_INLINE bool is_not_equal(double a, double b);
    M_INLINE bool is_zero(double x);
    M_INLINE bool is_not_zero(double x);
}

#ifdef WITH_INLINE
#include "dbl.cpp"
#endif