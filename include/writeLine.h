#pragma once
#include <vector>
#include <string>
namespace egl
{
    // dim: 2 for 2D point, 3 for 3D point
    template <class Point>
    inline void writeLine(const std::string fname, const std::vector<Point> &line_pts, int dim)
    {
        FILE *fptr = fopen(fname.c_str(), "w");
        if (fptr == nullptr)
        {
            printf("cannot write to file %s\n", fname.c_str());
            return;
        }
        fprintf(fptr, "%d %d\n", line_pts.size(), dim);
        for (const Point &p : line_pts)
        {
            if (dim == 2)
            {
                fprintf(fptr, "%g %g\n", p.x(), p.y());
            }
            else if (dim == 3)
            {
                fprintf(fptr, "%g %g %g\n", p.x(), p.y(), p.z());
            }
        }
        fclose(fptr);
    }
};