#include <PolyMesh/PolyMesh.h>
#include <PolyMesh/IOManager.h>

using namespace acamcad::polymesh;
using namespace std;

int main(int argc, char *argv[])
{
    const std::string path = argc > 1 ? argv[1] : "data/13.obj";
    PolyMesh *pm = new PolyMesh;
    loadMesh(path, pm);
    cout << pm->numVertices() << ", " << pm->numPolygons() << endl;

    delete pm;
    return 0;
}