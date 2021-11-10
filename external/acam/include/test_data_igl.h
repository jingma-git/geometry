#pragma once
#include <Eigen/Eigen>
// my favorite test cases
namespace egl
{
    using namespace std;

    inline void test_mesh0(Eigen::MatrixXd &V, Eigen::MatrixXi &F)
    {

        // four right-triangles
        V.resize(5, 3);
        V << 0, 0, 0,
            1, 1, 0,
            -1, 1, 0,
            -1, -1, 0,
            1, -1, 0;
        F.resize(4, 3);
        F << 0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 1;
    }

    inline void test_mesh1(Eigen::MatrixXd &V, Eigen::MatrixXi &F)
    {

        // four triangles: T0, T2: 120 degree (obtuse), T1, T3: equi-lateral
        double l = sqrt(3);
        V.resize(5, 3);
        V << 0, 0, 0,
            l, 1, 0,
            -l, 1, 0,
            -l, -1, 0,
            l, -1, 0;
        F.resize(4, 3);
        F << 0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 1;
    }
};