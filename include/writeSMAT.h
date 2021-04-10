#pragma once
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <iostream>
#include <fstream>

namespace egl
{

    using namespace Eigen;
    using namespace std;

    template <typename T, int whatever, typename IND>
    bool writeSMAT(const string fname, SparseMatrix<T, whatever, IND> &m)
    {
        typedef Triplet<int> Trip;
        std::vector<Trip> res;
        int sz = m.nonZeros();
        m.makeCompressed();

        fstream writeFile;
        writeFile.open(fname, ios::binary | ios::out);

        if (writeFile.is_open())
        {
            IND rows, cols, nnzs, outS, innS;
            rows = m.rows();
            cols = m.cols();
            nnzs = m.nonZeros();
            outS = m.outerSize();
            innS = m.innerSize();

            writeFile.write((const char *)&(rows), sizeof(IND));
            writeFile.write((const char *)&(cols), sizeof(IND));
            writeFile.write((const char *)&(nnzs), sizeof(IND));
            writeFile.write((const char *)&(innS), sizeof(IND));
            writeFile.write((const char *)&(outS), sizeof(IND));

            writeFile.write((const char *)(m.valuePtr()), sizeof(T) * m.nonZeros());
            writeFile.write((const char *)(m.outerIndexPtr()), sizeof(IND) * m.outerSize());
            writeFile.write((const char *)(m.innerIndexPtr()), sizeof(IND) * m.nonZeros());

            writeFile.close();
            return true;
        }
        return false;
    }
};