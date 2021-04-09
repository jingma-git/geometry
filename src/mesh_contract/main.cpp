#include <iostream>
#include <PolyMesh/PolyMesh.h>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;

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

int main()
{
    contract();
    return 0;
}