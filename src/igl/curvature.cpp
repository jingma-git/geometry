#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>
#include <igl/gaussian_curvature.h>
#include <igl/readOFF.h>
#include <igl/readOBJ.h>
#include <igl/writeDMAT.h>

#include <iostream>

using namespace Eigen;
using namespace std;

int main(int argc, char *argv[])
{
     MatrixXd V;
     MatrixXi F;
     string str = argv[1];
     string postfix = str.substr(str.find_last_of('.'));
     if (postfix == ".off")
          igl::readOFF(argv[1], V, F);
     else if (postfix == ".obj")
          igl::readOBJ(argv[1], V, F);
     else
     {
          std::cerr << "File Format does not support!" << std::endl;
          return 0;
     }
     //  V.array() *= 100; // test the relationship between curvature and mesh size: curvature == 1./ (mesh_size)^2

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
     igl::writeDMAT(argv[3], K, true);
     return 0;
}

// ./build/igl_curvature data/RigNet/13/0.off output/RigNet/13/0.H  output/RigNet/13/0.K
