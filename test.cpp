
#include <egl/readOBJ.h>
#include <egl/edge_topology.h>
#include <egl/readDMAT.h>
#include <egl/writeDMAT.h>
#include <iostream>
using namespace Eigen;
using namespace std;
using namespace egl;

int main()
{
    // MatrixXd V;
    // MatrixXi F;

    // egl::readOBJ("data/plane.obj", V, F);
    // cout << V.rows() << ", " << V.cols() << endl;
    // cout << F.rows() << ", " << F.cols() << endl;
    // MatrixXi in_edges, bnd_edges;
    // MeshTopology mesh(V, F);
    // mesh.inner_edges(in_edges);
    // mesh.boundary_edges(bnd_edges);
    // cout << mesh.edges.cols() << " " << in_edges.cols() << " " << bnd_edges.cols() << endl;
    MatrixXd I = MatrixXd::Identity(10, 10);
    egl::writeDMAT("tmp.mat", I, false);
    MatrixXd tmp;
    egl::readDMAT("tmp.mat", tmp);
    cout << tmp << endl;
    return 0;
}