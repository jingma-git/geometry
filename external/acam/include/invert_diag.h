#pragma once
#include <eigen3/Eigen/Sparse>
namespace egl
{
    template <typename DerivedX, typename MatY>
    inline void invert_diag(
        const Eigen::SparseCompressedBase<DerivedX> &X,
        MatY &Y)
    {
        typedef typename DerivedX::Scalar Scalar;
        // http://www.alecjacobson.com/weblog/?p=2552
        if ((void *)&Y != (void *)&X) // judge whether X, Y is the same matrix
        {
            Y = X;
        }
        // Iterate over outside
        for (int k = 0; k < Y.outerSize(); ++k)
        {
            // Iterate over inside
            for (typename MatY::InnerIterator it(Y, k); it; ++it)
            {
                if (it.col() == it.row())
                {
                    Scalar v = it.value();
                    assert(v != 0);
                    v = ((Scalar)1.0) / v;
                    Y.coeffRef(it.row(), it.col()) = v;
                }
            }
        }
    }
};
