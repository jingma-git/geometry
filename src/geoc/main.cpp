#include <geometrycentral/surface_mesh.h>
#include <geometrycentral/simple_polygon_mesh.h>

using namespace geometrycentral;
using namespace geometrycentral::surface;
using namespace std;

std::unique_ptr<SurfaceMesh> mesh;

void printStat(SurfaceMesh &sm)
{

    M_INFO("#V:" << sm.nVertices() << " #F:" << sm.nFaces() << " #E:" << sm.nEdges());
    cout << "vertices" << endl;
    for (Vertex v : sm.vertices())
    {
        cout << v << endl;
    }

    cout << "halfedges" << endl;
    for (Halfedge h : sm.halfedges())
    {
        cout << h << " from " << h.vertex() << endl;
    }

    cout << "edges" << endl;
    for (Edge e : sm.edges())
    {
        cout << e << " " << e.firstVertex() << "-" << e.secondVertex() << endl;
    }

    cout << "faces" << endl;
    for (Face f : sm.faces())
    {
        cout << f << " from " << f.halfedge() << endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Command ./test_geoc path/to/.obj" << std::endl;
        return -1;
    }

    SimplePolygonMesh inputMesh(argv[1]);
    mesh.reset(new SurfaceMesh(inputMesh.polygons));
    printStat(*mesh);

    // circulators
    cout << "V1's adjacent vertices" << endl;
    Vertex v1 = mesh->vertex(1);
    for (Vertex adjV : v1.adjacentVertices())
    {
        cout << adjV << endl;
    }
    cout << "V1's incoming halfedges" << endl;
    for (Halfedge h : v1.incomingHalfedges())
    {
        cout << h << endl;
    }
    cout << "V1's outcoming halfedges" << endl;
    for (Halfedge h : v1.outgoingHalfedges())
    {
        cout << h << endl;
    }
    cout << "V1's edges" << endl;
    for (Edge e : v1.adjacentEdges())
    {
        cout << e << endl;
    }
    cout << "V1's faces" << endl;
    for (Face f : v1.adjacentFaces())
    {
        cout << f << endl;
    }
    return 0;
}