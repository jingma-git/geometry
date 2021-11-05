#pragma once
#include <fstream>
namespace egl
{
    namespace cgal
    {

        template <class Graph>
        bool readOFF(const std::string fname, Graph &g)
        {
            std::ifstream input(fname);
            if (input.fail())
            {
                printf("Cannot open %s!\n", fname.c_str());
                return false;
            }
            input >> g;
            input.close();
            return true;
        }
    }
}