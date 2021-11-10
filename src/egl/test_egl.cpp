
#include <egl/readOBJ.h>
#include <egl/edge_topology.h>
#include <egl/readDMAT.h>
#include <egl/writeDMAT.h>
#include <egl/readSMAT.h>
#include <egl/writeSMAT.h>
#include <egl/min_max_coeff.h>
#include <egl/cat.h>
#include <egl/make_cube.h>
#include <iostream>
using namespace Eigen;
using namespace std;
using namespace egl;

void test_edge_topology()
{
    MatrixXd V;
    MatrixXi F;

    egl::readOBJ("data/plane.obj", V, F);
    cout << V.rows() << ", " << V.cols() << endl;
    cout << F.rows() << ", " << F.cols() << endl;
    MatrixXi in_edges, bnd_edges;
    MeshTopology mesh(V, F);
    mesh.inner_edges(in_edges);
    mesh.boundary_edges(bnd_edges);
    cout << mesh.edges.cols() << " " << in_edges.cols() << " " << bnd_edges.cols() << endl;
}

void test_dmat()
{
    MatrixXd I = MatrixXd::Identity(10, 10);
    egl::writeDMAT("tmp.mat", I, false);
    MatrixXd tmp;
    egl::readDMAT("tmp.mat", tmp);
    cout << tmp << endl;
}

void test_smat()
{
    int rows, cols;
    rows = cols = 6;
    SparseMatrix<double> A(rows, cols), B;
    typedef Triplet<double> Trip;
    std::vector<Trip> trp, tmp;
    for (int i = 0; i < rows; ++i)
    {
        trp.emplace_back(i, i, i);
    }

    A.setFromTriplets(trp.begin(), trp.end());
    cout << A.nonZeros() << endl; // Prints 8
    cout << A.size() << endl;     // Prints 36
    cout << A << endl;            // Prints the matrix along with the sparse matrix stuff

    egl::writeSMAT("A.smat", A);
    egl::readSMAT("A.smat", B);
    cout << "B\n"
         << B << endl;
}

void test_min_max()
{
    int N = 6;
    SparseMatrix<double> A(N, N);
    std::vector<Triplet<double>> trpA;
    for (int i = 0; i < N; ++i)
    {
        trpA.emplace_back(i, i, i);
    }

    A.setFromTriplets(trpA.begin(), trpA.end());
    double minA, maxA;
    min_max_coeff(minA, maxA, A);
    cout << minA << ", " << maxA << endl;

    SparseMatrix<float> B(N, N);
    std::vector<Triplet<float>> trpB;
    for (int i = 0; i < N; ++i)
    {
        trpB.emplace_back(i, i, i);
    }

    B.setFromTriplets(trpB.begin(), trpB.end());
    float minB, maxB;
    min_max_coeff(minB, maxB, B);
    cout << minB << ", " << maxB << endl;

    min_max_coeff(minB, maxB, B.block(0, 0, 4, 4).eval());
    cout << minB << ", " << maxB << endl;
}

void test_cat()
{
    int rows, cols;
    rows = cols = 6;
    SparseMatrix<double> A(rows, cols), B, C;
    typedef Triplet<double> Trip;
    std::vector<Trip> trp, tmp;
    for (int i = 0; i < rows; ++i)
    {
        trp.emplace_back(i, i, i);
    }

    A.setFromTriplets(trp.begin(), trp.end());

    // egl::cat(1, A, A, A, B);
    egl::cat(2, A, A, A, C);
    // cout << B.toDense() << endl;
    cout << C.toDense() << endl;
}

void test_make_cube()
{
    MatrixXd V;
    MatrixXi F;

    make_cube(V, F);

    cout << V << endl;
}

int main()
{
    // test_edge_topology();
    // test_dmat();
    // test_smat();
    // test_min_max();
    // test_cat();
    // test_make_cube();
    return 0;
}