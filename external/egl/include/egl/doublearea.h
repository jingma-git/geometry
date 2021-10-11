#pragma once
namespace egl
{
    template <typename DerivedV, typename DerivedF, typename DerivedA>
    inline void doublearea(
        const Eigen::MatrixBase<DerivedV> &V,
        const Eigen::MatrixBase<DerivedF> &F,
        Eigen::PlainObjectBase<DerivedA> &dblA)
    {
        using namespace Eigen;
        const size_t m = F.cols();
        switch (F.rows())
        {
        case 3:
        {
            dblA = DerivedA::Zero(1, m);
            for (size_t i = 0; i < m; ++i)
            {
                Vector3d e01 = V.col(F(1, i)) - V.col(F(0, i));
                Vector3d e02 = V.col(F(2, i)) - V.col(F(0, i));
                dblA(0, i) = (e01.cross(e02)).norm();
            }
            break;
        }
        default:
            assert(false && "Simplex size not support!");
        }
    }
}