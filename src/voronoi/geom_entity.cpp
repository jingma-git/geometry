#include "geom_entity.h"

// https://stackoverflow.com/questions/19754949/what-is-a-standard-way-to-compare-float-with-zero
M_INLINE bool intersect(const Line &l1, const Line &l2, Eigen::Vector3d &pnt)
{
    double det = l1.a() * l2.b() - l1.b() * l2.a();
    if (det == 0)
    {
        return false;
    }
    else
    {
        double detA = l1.c() * l2.b() - l1.b() * l2.c();
        double detB = l1.a() * l2.c() - l1.c() * l2.a();
        pnt[0] = detA / det;
        pnt[1] = detB / det;
        pnt[2] = 0;
        return true;
    }
}