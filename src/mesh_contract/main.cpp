#include <iostream>
#include <ctime>
#include <Eigen/Eigen>

#include <PolyMesh/PolyMesh.h>
#include <PolyMesh/IOManager.h>
#include <cotmatrix.h>
#include <massmatrix.h>
#include <writeSMAT.h>
#include <readSMAT.h>
#include <test_data.h>

using namespace std;
using namespace Eigen;
using namespace acamcad;
using namespace acamcad::polymesh;

void fillDiag(SparseMatrix<double> &M, const vector<double> &vals)
{
    cout << "--------fill start!" << endl;
    for (int k = 0; k < M.outerSize(); k++)
    {
        for (SparseMatrix<double>::InnerIterator it(M, k); it; ++it)
        {
            it.valueRef() = vals[k];
            cout << k << " " << it.value() << " " << vals[k] << " " << it.valueRef() << endl;
        }
    }
    cout << "--------fill done!" << endl;
}

void contract()
{
    MatrixXd V(4, 3);
    SimplicialCholesky<SparseMatrix<double>> cholesky;
    typedef Triplet<double> T;
    vector<T> coff;
    for (int i = 0; i < 4; i++)
        coff.push_back(T(i, i, 1));
    SparseMatrix<double> Q;
    Q.resize(4, 4);
    Q.setFromTriplets(coff.begin(), coff.end());
    cout << Q.toDense() << endl;
    vector<double> vals(4, 2);
    fillDiag(Q, vals);
    cout << Q.toDense() << endl;
    for (int i = 0; i < vals.size(); i++)
    {
        vals[i] = 3;
    }
    fillDiag(Q, vals);
    cout << Q.toDense() << endl;
}

// connectivity surgery
void edge_collapse()
{
}

// embedding improvement
void impr_embed()
{
}

void testCotmatrix()
{
    PolyMesh mesh;
    loadMesh("example/hw9/dragon.obj", &mesh);
    // egl::test_mesh0(mesh);

    Eigen::SparseMatrix<double> M, Ltmp, Ltmpcp, L;
    egl::massmatrix_mixed_voronoi(mesh, M);
    clock_t start = clock();
    egl::cotmatrix_halfedge_impl(mesh, Ltmp);
    cout << "cotmatrix halfedge takes " << (clock() - start) / (CLOCKS_PER_SEC) << endl;
    egl::writeSMAT("output/Ltmp.smat", Ltmp);
    cout << "readErr=" << (Ltmp - Ltmpcp).sum() << endl;
    start = clock();
    egl::cotmatrix(mesh, L);
    cout << "cotmatrix  takes " << (clock() - start) / (CLOCKS_PER_SEC) << endl;
    if (mesh.numVertices() < 12)
    {
        cout << "Ltmp=\n"
             << Ltmp.toDense() << endl;
        cout << "L=\n"
             << L.toDense() << endl;
    }
    double err = abs((Ltmp - L).sum());
    if (err > 1e-15)
    {
        cout << "oops cotmatrix is wrong! err=" << err << endl;
    }
}

void testMassMatrix()
{
    PolyMesh mesh;
    loadMesh("example/hw9/dragon.obj", &mesh);
    // egl::test_mesh1(mesh);

    Eigen::SparseMatrix<double> M, Mbary;
    egl::massmatrix_mixed_voronoi(mesh, M);
    if (M.rows() < 15)
    {
        cout << M.toDense() << endl;
    }
    egl::writeSMAT("output/mass_voro.smat", M);

    egl::massmatrix_barycentric(mesh, Mbary);
    if (Mbary.rows() < 15)
    {
        cout << "Mbary" << endl;
        cout << Mbary.toDense() << endl;
    }
    egl::writeSMAT("output/mass_bary.smat", Mbary);
}

int main()
{
    testMassMatrix();
    return 0;
}