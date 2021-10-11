#include <iostream>

#include <egl/cgal/readOBJ.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
using namespace std;

void test_readOBJ()
{
    typedef CGAL::Simple_cartesian<double> K;
    typedef K::Point_3 Pnt3;

    typedef CGAL::Polyhedron_3<K, CGAL::Polyhedron_items_with_id_3> Polyhedron;

    Polyhedron mesh;
    egl::cgal::readOBJ("data/character.obj", mesh);
    cout << mesh.size_of_vertices() << ", " << mesh.size_of_facets() << endl;
}

int main()
{
    test_readOBJ();
    return 0;
}