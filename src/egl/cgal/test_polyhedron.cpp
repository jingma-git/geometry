#include <iostream>

#include <egl/cgal/readOBJ.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Polygon_mesh_processing/connected_components.h>

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Pnt3;

typedef CGAL::Polyhedron_3<K, CGAL::Polyhedron_items_with_id_3> Polyhedron;

using namespace std;
namespace PMP = CGAL::Polygon_mesh_processing;

void test_readOBJ()
{
    Polyhedron mesh;
    egl::cgal::readOBJ("data/character.obj", mesh);
    cout << mesh.size_of_vertices() << ", " << mesh.size_of_facets() << endl;
}

void test_split_connComp()
{
    Polyhedron mesh;
    egl::cgal::readOBJ("data/character.obj", mesh);
    cout << mesh.size_of_vertices() << ", " << mesh.size_of_facets() << endl;

    std::vector<Polyhedron> comps;
    PMP::split_connected_components(mesh, comps);
    cout << "num_comps: " << comps.size() << endl;
}

int main()
{
    // test_readOBJ();
    test_split_connComp();
    return 0;
}