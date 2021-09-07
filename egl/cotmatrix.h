#pragma once
#include "squared_edge_length.h"
#include "doublearea.h"

#include <vector>
#include <iostream>
using namespace std;

namespace egl
{
    template <typename DerivedV, typename DerivedF, typename DerivedC>
    inline void cotmatrix_entries(
        const Eigen::MatrixBase<DerivedV> &V,
        const Eigen::MatrixBase<DerivedF> &F,
        Eigen::PlainObjectBase<DerivedC> &C)
    {
        size_t num_ele = F.cols();
        using namespace Eigen;
        using namespace std;

        switch (F.rows())
        {
        case 3: // triangle
        {
            DerivedV l2;
            squared_edge_length(V, F, l2);
            DerivedV dblA;
            doublearea(V, F, dblA);
            C.resize(3, num_ele);
            // a^2 + b^2 - c^2 = 2*a*b*cos(theta)
            // dblA = a*b*sin(theta)
            for (size_t i = 0; i < num_ele; ++i)
            {
                C(0, i) = (l2(1, i) + l2(2, i) - l2(0, i)) / dblA(i) / 4.0; //why divided by 4, should not be divided by 2
                C(1, i) = (l2(2, i) + l2(0, i) - l2(1, i)) / dblA(i) / 4.0;
                C(2, i) = (l2(0, i) + l2(1, i) - l2(2, i)) / dblA(i) / 4.0;
            }
            break;
        }
        case 4: // tet
        {
            break;
        }
        default:
        {
        }
            assert(false && "cotmatrix.h Simplex size not support!");
        }
    }

    template <typename DerivedV, typename DerivedF, typename Scalar>
    inline void cotmatrix(
        const Eigen::MatrixBase<DerivedV> &V,
        const Eigen::MatrixBase<DerivedF> &F,
        Eigen::SparseMatrix<Scalar> &L)
    {
        using namespace Eigen;
        size_t num_ele = F.cols();
        L.resize(V.cols(), V.cols());

        Matrix<int, -1, 2> edges;

        switch (F.rows())
        {
        case 3: // triangle
        {
            L.reserve(10 * V.cols());
            edges.resize(3, 2);
            edges << 1, 2,
                2, 0,
                0, 1;

            DerivedV C; // 3 x num_ele
            cotmatrix_entries(V, F, C);
            vector<Triplet<Scalar>> IJV;
            for (int i = 0; i < num_ele; ++i)
            {
                for (int e = 0; e < edges.rows(); ++e)
                {
                    int src = F(edges(e, 0), i);
                    int dst = F(edges(e, 1), i);
                    IJV.emplace_back(src, dst, C(e, i));
                    IJV.emplace_back(dst, src, C(e, i));
                    IJV.emplace_back(src, src, -C(e, i));
                    IJV.emplace_back(dst, dst, -C(e, i));
                }
            }
            L.setFromTriplets(IJV.begin(), IJV.end());
            break;
        }
        case 4: // tet
        {
            break;
        }
        default:
        {
        }
            assert(false && "cotmatrix.h Simplex size not support!");
        }
    }
}