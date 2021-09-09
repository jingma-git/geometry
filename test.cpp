
#include <egl/readOBJ.h>
#include <iostream>
using namespace Eigen;
using namespace std;

int main()
{
    MatrixXd V;
    MatrixXi F;

    egl::readOBJ("data/plane.obj", V, F);
    cout << V.rows() << ", " << V.cols() << endl;
    cout << F.rows() << ", " << F.cols() << endl;
    return 0;
}