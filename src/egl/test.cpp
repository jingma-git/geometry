
#include <egl/readOBJ.h>
#include <egl/edge_topology.h>
#include <egl/readDMAT.h>
#include <egl/writeDMAT.h>
#include <egl/readSMAT.h>
#include <egl/writeSMAT.h>
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

int main()
{
    // test_edge_topology();
    // test_dmat();
    test_smat();
    return 0;
}