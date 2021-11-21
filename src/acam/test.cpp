#include <PolyMesh/IOManager.h>

using namespace std;
using namespace acamcad;
using namespace polymesh;

void print_mesh(const PolyMesh &mesh)
{
    cout << "V:" << mesh.numVertices() << endl;
    for (CVertexIter vit = mesh.const_vertices_begin(); vit != mesh.const_vertices_end(); ++vit)
    {
        MVert *v = *vit;
        cout << *v << endl;
    }

    cout << "F:" << mesh.numPolygons() << endl;
    for (CHalfEdgeIter hit = mesh.const_halfedge_begin(); hit != mesh.const_halfedge_end(); ++hit)
    {
        MHalfedge *h = *hit;
        cout << *h << endl;
    }
}

int main()
{
    PolyMesh mesh;
    MVert *vds[5];
    vds[0] = mesh.addVertex(MPoint3(0, 0, 0));
    vds[1] = mesh.addVertex(MPoint3(-1, 1, 0));
    vds[2] = mesh.addVertex(MPoint3(-1, -1, 0));
    vds[3] = mesh.addVertex(MPoint3(1, -1, 0));
    vds[4] = mesh.addVertex(MPoint3(1, 1, 0));

    std::vector<MVert *> fvs0, fvs1, fvs2;
    fvs0.push_back(vds[0]);
    fvs0.push_back(vds[1]);
    fvs0.push_back(vds[2]);
    fvs1.push_back(vds[0]);
    fvs1.push_back(vds[2]);
    fvs1.push_back(vds[3]);
    fvs2.push_back(vds[0]);
    fvs2.push_back(vds[3]);
    fvs2.push_back(vds[4]);

    MPolyFace *f[3];
    f[0] = mesh.addPolyFace(fvs0);
    f[1] = mesh.addPolyFace(fvs1);
    f[2] = mesh.addPolyFace(fvs2);
    mesh.deletePolyFace(f[1]);
    mesh.delete_isolated_edges();

    print_mesh(mesh);
    return 0;
}