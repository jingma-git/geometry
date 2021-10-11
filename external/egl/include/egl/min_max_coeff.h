#pragma once

#include <Eigen/Sparse>

namespace egl
{
    template <typename T, int ORDER, typename IND>
    void min_max_coeff(T &min_coeff,
                       T &max_coeff,
                       const Eigen::SparseMatrix<T, ORDER, IND> &A)
    {
        using namespace Eigen;
        min_coeff = std::numeric_limits<T>::max();
        max_coeff = std::numeric_limits<T>::min();
        size_t sz = A.outerSize();
        for (int k = 0; k < A.outerSize(); ++k)
        {
            for (typename Eigen::SparseMatrix<T, ORDER, IND>::InnerIterator it(A, k); it; ++it)
            {
                if (it.value() < min_coeff)
                    min_coeff = it.value();
                if (it.value() > max_coeff)
                    max_coeff = it.value();
            }
        }
    }
}
