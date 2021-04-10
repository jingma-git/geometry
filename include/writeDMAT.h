#pragma once
#include <Eigen/Core>
#include <string>
#include <vector>
#include <cstdio>

namespace egl
{
    // Write a matrix using ascii dmat file type
    //
    // Template:
    //   Mat  matrix type that supports .rows(), .cols(), operator(i,j)
    // Inputs:
    //   file_name  path to .dmat file
    //   W  eigen matrix containing to-be-written coefficients
    //   ascii  write ascii file {true}
    // Returns true on success, false on error
    //
    template <typename DerivedW>
    inline bool writeDMAT(
        const std::string file_name,
        const Eigen::MatrixBase<DerivedW> &W,
        const bool ascii = true)
    {
        FILE *fp = fopen(file_name.c_str(), "wb");
        if (fp == NULL)
        {
            fprintf(stderr, "IOError: writeDMAT() could not open %s...", file_name.c_str());
            return false;
        }
        if (ascii)
        {
            // first line contains number of rows and number of columns
            fprintf(fp, "%d %d\n", (int)W.cols(), (int)W.rows());
            // Loop over columns slowly
            for (int j = 0; j < W.cols(); j++)
            {
                // loop over rows (down columns) quickly
                for (int i = 0; i < W.rows(); i++)
                {
                    fprintf(fp, "%0.17lg\n", (double)W(i, j));
                }
            }
        }
        else
        {
            // write header for ascii
            fprintf(fp, "0 0\n");
            // first line contains number of rows and number of columns
            fprintf(fp, "%d %d\n", (int)W.cols(), (int)W.rows());
            // reader assumes the binary part is double precision
            Eigen::MatrixXd Wd = W.template cast<double>();
            fwrite(Wd.data(), sizeof(double), Wd.size(), fp);
        }
        fclose(fp);
        return true;
    }
}