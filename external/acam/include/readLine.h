#pragma once
#include <vector>
#include <string>
#include <fstream>

namespace egl
{
    template <class Point>
    inline void readLine(const std::string fname, std::vector<Point> &line_pts)
    {
        using namespace std;
        std::ifstream is(fname);
        if (!is.is_open())
        {
            cout << "cannot open " << fname << endl;
            return;
        }
        string line;
        getline(is, line);
        int pts_size, dim;
        sscanf(line.c_str(), "%d %d", &pts_size, &dim);
        for (int i = 0; i < pts_size; i++)
        {
            getline(is, line);
            double x, y, z;
            if (dim == 2)
            {
                sscanf(line.c_str(), "%lf %lf", &x, &y);
                line_pts.push_back(Point(x, y, z));
            }
            else if (dim == 3)
            {
                sscanf(line.c_str(), "%lf %lf %lf", &x, &y, &z);
                line_pts.push_back(Point(x, y, z));
            }
        }
        is.close();
    }
};