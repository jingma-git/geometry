#pragma once
#include <fstream>

namespace egl
{
    namespace cgal
    {
        template <class HDS>
        int writeOFF(const std::string path,
                     const HDS &hds)
        {
            std::ofstream out(path);
            if (out.fail())
                return 1;
            out << hds;
            out.close();

            return 0;
        }
    }
}