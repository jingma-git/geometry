#include <geometrycentral/surface_mesh.h>
#include <geometrycentral/simple_polygon_mesh.h>
#include <geometrycentral/disjoint_sets.h>

using namespace geometrycentral;
using namespace geometrycentral::surface;
using namespace std;

std::unique_ptr<SurfaceMesh> mesh;

void printStat(SurfaceMesh &sm)
{

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

int testNewSurfaceMesh(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Command ./test_geoc path/to/.obj" << std::endl;
        return -1;
    }

    SimplePolygonMesh inputMesh(argv[1]);
    mesh.reset(new SurfaceMesh(inputMesh.polygons));
    M_INFO("#V:" << mesh->nVertices() << " #F:" << mesh->nFaces() << " #E:" << mesh->nEdges());
    if (mesh->nVertices() < 10)
    {
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

        // face circulators
        Face f0 = mesh->face(0);
        cout << "f0's adjacent faces" << std::endl;
        for (Face f : f0.adjacentFaces())
        {
            cout << f << endl;
        }
    }

    std::vector<Face> comps = mesh->getConnectedComponents();
    M_INFO("comps:" << comps.size());
    for (Face f : comps)
    {
        cout << "comp: " << f << endl;
    }
    return 0;
}

void testDisjointSet()
{
    DisjointSets S(6);
    S.merge(0, 1);
    S.print();
    S.merge(1, 2);
    S.print();
    S.merge(3, 2);
    S.print();
    S.merge(0, 3);
    S.print();
    S.merge(4, 5);
    S.print();
}

int main(int argc, char *argv[])
{
    testNewSurfaceMesh(argc, argv);
    // testDisjointSet();
    return 0;
}