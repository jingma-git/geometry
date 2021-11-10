#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>
#include <igl/gaussian_curvature.h>
#include <igl/readOFF.h>
#include <igl/writeDMAT.h>

#include <iostream>

using namespace Eigen;
using namespace std;

int main(int argc, char *argv[])
{
    MatrixXd V;
    MatrixXi F;
    igl::readOFF(argv[1], V, F);

    MatrixXd HN;
    SparseMatrix<double> L, M, Minv;
    igl::cotmatrix(V, F, L);
    igl::massmatrix(V, F, igl::MASSMATRIX_TYPE_VORONOI, M);
    igl::invert_diag(M, Minv);
    HN = -Minv * (L * V);
    VectorXd H = HN.rowwise().norm(); // up to sign

    cout << "num_verts: " << V.rows() << "\n"
         << "min_mean_curvature: " << H.minCoeff() << "\n"
         << "max_mean_curvature: " << H.maxCoeff() << "\n"
         << "avg_mean_curvature: " << H.mean() << "\n";

    igl::writeDMAT(argv[2], H, true);

    VectorXd K;
    igl::gaussian_curvature(V, F, K);
    K = (Minv * K).eval();
    cout << "\n"
         << "min_gauss_curvature: " << K.minCoeff() << "\n"
         << "max_gauss_curvature: " << K.maxCoeff() << "\n"
         << "avg_gauss_curvature: " << K.mean() << "\n";
    igl::writeDMAT(argv[3], H, true);
    return 0;
}

// ./build/igl_curvature data/RigNet/13/0.off output/RigNet/13/0.curv
// ./build/igl_curvature data/RigNet/13/1.off output/RigNet/13/1.curv
// ./build/igl_curvature data/RigNet/13/2.off output/RigNet/13/2.curv