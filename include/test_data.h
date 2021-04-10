#pragma once

#include "PolyMesh/PolyMesh.h"
#include <vector>
#include <cmath>
// my favorite test cases
namespace egl
{
    using namespace acamcad;
    using namespace acamcad::polymesh;
    using namespace std;

    inline void test_mesh0(PolyMesh &mesh)
    {

        // four right-triangles
        MVert *v0 = mesh.addVertex(0, 0, 0);
        MVert *v1 = mesh.addVertex(1, 1, 0);
        MVert *v2 = mesh.addVertex(-1, 1, 0);
        MVert *v3 = mesh.addVertex(-1, -1, 0);
        MVert *v4 = mesh.addVertex(1, -1, 0);

        vector<MVert *> fvs0({v0, v1, v2});
        vector<MVert *> fvs1({v0, v2, v3});
        vector<MVert *> fvs2({v0, v3, v4});
        vector<MVert *> fvs3({v0, v4, v1});
        mesh.addPolyFace(fvs0);
        mesh.addPolyFace(fvs1);
        mesh.addPolyFace(fvs2);
        mesh.addPolyFace(fvs3);
    }

    inline void test_mesh1(PolyMesh &mesh)
    {

        // four triangles: T0, T2: 120 degree (obtuse), T1, T3: equi-lateral
        double l = sqrt(3);
        MVert *v0 = mesh.addVertex(0, 0, 0);
        MVert *v1 = mesh.addVertex(l, 1, 0);
        MVert *v2 = mesh.addVertex(-l, 1, 0);
        MVert *v3 = mesh.addVertex(-l, -1, 0);
        MVert *v4 = mesh.addVertex(l, -1, 0);

        vector<MVert *> fvs0({v0, v1, v2});
        vector<MVert *> fvs1({v0, v2, v3});
        vector<MVert *> fvs2({v0, v3, v4});
        vector<MVert *> fvs3({v0, v4, v1});
        mesh.addPolyFace(fvs0);
        mesh.addPolyFace(fvs1);
        mesh.addPolyFace(fvs2);
        mesh.addPolyFace(fvs3);
    }

};