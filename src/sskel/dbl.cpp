#include "dbl.h"

namespace egl
{
    M_INLINE bool is_equal(double a, double b)
    {
        return fabs(a - b) < DBL_EPS;
    }

    M_INLINE bool is_not_equal(double a, double b)
    {
        return fabs(a - b) > DBL_EPS;
    }

    M_INLINE bool is_zero(double x)
    {
        return fabs(x) < DBL_EPS;
    }

    M_INLINE bool is_not_zero(double x)
    {
        return fabs(x) > DBL_EPS;
    }

}