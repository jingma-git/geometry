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
    bool readSMAT(const string fname, SparseMatrix<T, whatever, IND> &m)
    {
        fstream readFile;
        readFile.open(fname, ios::binary | ios::in);
        if (readFile.is_open())
        {
            IND rows, cols, nnz, inSz, outSz;
            readFile.read((char *)&rows, sizeof(IND));
            readFile.read((char *)&cols, sizeof(IND));
            readFile.read((char *)&nnz, sizeof(IND));
            readFile.read((char *)&inSz, sizeof(IND));
            readFile.read((char *)&outSz, sizeof(IND));

            m.resize(rows, cols);
            m.makeCompressed();
            m.resizeNonZeros(nnz);

            readFile.read((char *)(m.valuePtr()), sizeof(T) * nnz);
            readFile.read((char *)(m.outerIndexPtr()), sizeof(IND) * outSz);
            readFile.read((char *)(m.innerIndexPtr()), sizeof(IND) * nnz);

            m.finalize();
            readFile.close();
            return true;
        } // file is open
        return false;
    }
};