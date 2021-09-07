#pragma once
#include <Eigen/Eigen>
namespace egl
{
    template <typename VT, typename FT, typename LT>
    inline void squared_edge_length(
        const Eigen::MatrixBase<VT> &V,
        const Eigen::MatrixBase<FT> &F,
        Eigen::PlainObjectBase<LT> &L)
    {
        size_t num_ele = F.cols();
        using namespace Eigen;
        switch (F.rows())
        {
        case 2:
            L.resize(1, F.cols());
#pragma omp parallel for
            for (size_t i = 0; i < num_ele; ++i)
                L(0, i) = (V.col(F(1, i)) - V.col(F(0, i))).squaredNorm();
            break;
        case 3:
            break;
        case 4:
            break;
        default:
            assert(false && "squared_edge_length.h simplex size not support!");
        }
    }
}
