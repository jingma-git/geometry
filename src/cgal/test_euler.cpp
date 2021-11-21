#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/boost/graph/Euler_operations.h>

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Pnt3;
typedef CGAL::Polyhedron_3<K, CGAL::Polyhedron_items_with_id_3> Polyhedron;
typedef Polyhedron::HalfedgeDS HDS;
typedef typename boost::graph_traits<Polyhedron>::vertex_descriptor Vd;
typedef typename boost::graph_traits<Polyhedron>::halfedge_descriptor Hd;
typedef typename boost::graph_traits<Polyhedron>::edge_descriptor Ed;
typedef typename boost::graph_traits<Polyhedron>::face_descriptor Fd;
typedef typename boost::graph_traits<Polyhedron>::vertex_iterator VIter;
typedef typename boost::graph_traits<Polyhedron>::halfedge_iterator HIter;
typedef typename boost::graph_traits<Polyhedron>::edge_iterator EIter;
typedef typename boost::property_map<Polyhedron, boost::vertex_index_t>::type VertexIndexMap;
typedef typename boost::property_map<Polyhedron, boost::vertex_point_t>::type VertexPointMap;
typedef typename boost::property_map<Polyhedron, boost::halfedge_index_t>::type HalfedgeIndexMap;
typedef typename boost::property_map<Polyhedron, boost::face_index_t>::type FaceIndexMap;

using namespace std;
using namespace CGAL;
namespace PMP = CGAL::Polygon_mesh_processing;

void assign_id(Polyhedron &pm)
{
    int i = 0;
    for (Vd vd : vertices(pm))
    {
        vd->id() = i++;
    }

    i = 0;
    for (Hd hd : halfedges(pm))
    {
        hd->id() = i++;
    }

    i = 0;
    for (Fd fd : faces(pm))
    {
        fd->id() = i++;
    }
}

void print_graph(Polyhedron &pm)
{
    using namespace std;
    cout << "V=" << num_vertices(pm) << " E=" << num_edges(pm) << " F=" << num_faces(pm) << endl;
    cout << "Vert" << endl;
    VertexPointMap vpm = get(boost::vertex_point, pm);
    for (Vd vd : vertices(pm))
    {
        cout << "v" << vd->id() << ": " << vpm[vd] << endl;
    }
    cout << endl;
    cout << "Edge" << endl;
    int i = 0;
    for (Ed ed : edges(pm))
    {
        Hd hd = halfedge(ed, pm);
        Vd v0 = source(hd, pm);
        Vd v1 = target(hd, pm);
        cout << "e" << i << " h" << hd->id() << " v" << v0->id() << "-v" << v1->id() << endl;
        i++;
    }
    cout << endl;
    cout << "Face" << endl;
    for (Fd fd : faces(pm))
    {
        cout << "f" << fd->id() << ":";
        Hd fhd = halfedge(fd, pm);
        for (Vd vd : vertices_around_face(fhd, pm))
        {
            cout << "v" << vd->id() << " ";
        }

        cout << " ";
        for (Hd hd : halfedges_around_face(fhd, pm))
        {
            cout << "h" << hd->id() << " ";
        }
        cout << endl;
    }
}

// A modifier creating a triangle with the incremental builder.
template <class HDS>
class BuildGraph : public CGAL::Modifier_base<HDS>
{
public:
    BuildGraph() {}
    void operator()(HDS &hds)
    {
        // Postcondition: hds is a valid polyhedral surface.
        CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
        B.begin_surface(10, 10, 40);
        typedef typename HDS::Vertex Vertex;
        typedef typename Vertex::Point Point;
        B.add_vertex(Point(-2, 1, 0));
        B.add_vertex(Point(-3, 0, 0));
        B.add_vertex(Point(-2, -1, 0));
        B.add_vertex(Point(0, -1, 0));
        B.add_vertex(Point(2, -1, 0));
        B.add_vertex(Point(3, 0, 0));
        B.add_vertex(Point(2, 1, 0));
        B.add_vertex(Point(0, 1, 0));
        B.add_vertex(Point(-1, 0, 0));
        B.add_vertex(Point(1, 0, 0));

        // f0
        B.begin_facet();
        B.add_vertex_to_facet(0);
        B.add_vertex_to_facet(1);
        B.add_vertex_to_facet(8);
        B.end_facet();

        // f1
        B.begin_facet();
        B.add_vertex_to_facet(8);
        B.add_vertex_to_facet(1);
        B.add_vertex_to_facet(2);
        B.end_facet();

        // f2
        B.begin_facet();
        B.add_vertex_to_facet(8);
        B.add_vertex_to_facet(2);
        B.add_vertex_to_facet(3);
        B.end_facet();

        // f3
        B.begin_facet();
        B.add_vertex_to_facet(8);
        B.add_vertex_to_facet(3);
        B.add_vertex_to_facet(9);
        B.end_facet();

        // f4
        B.begin_facet();
        B.add_vertex_to_facet(9);
        B.add_vertex_to_facet(3);
        B.add_vertex_to_facet(4);
        B.end_facet();

        // f5
        B.begin_facet();
        B.add_vertex_to_facet(9);
        B.add_vertex_to_facet(4);
        B.add_vertex_to_facet(5);
        B.end_facet();

        // f6
        B.begin_facet();
        B.add_vertex_to_facet(9);
        B.add_vertex_to_facet(5);
        B.add_vertex_to_facet(6);
        B.end_facet();

        // f7
        B.begin_facet();
        B.add_vertex_to_facet(9);
        B.add_vertex_to_facet(6);
        B.add_vertex_to_facet(7);
        B.end_facet();

        // f8
        B.begin_facet();
        B.add_vertex_to_facet(9);
        B.add_vertex_to_facet(7);
        B.add_vertex_to_facet(8);
        B.end_facet();

        // f9
        B.begin_facet();
        B.add_vertex_to_facet(8);
        B.add_vertex_to_facet(7);
        B.add_vertex_to_facet(0);
        B.end_facet();

        B.end_surface();
    }
};

void make_graph(Polyhedron &pm)
{
    BuildGraph<HDS> builder;
    pm.delegate(builder);
}

int main()
{
    Polyhedron pm;
    // PMP::IO::read_polygon_mesh("data/square2trigs.obj", pm);
    make_graph(pm);
    assign_id(pm);
    print_graph(pm);
    cout << endl;

    int i = 0;
    for (Ed ed : edges(pm))
    {
        // if (i == 8) // delete middle edge
        // {
        //     Euler::collapse_edge(ed, pm);
        //     break;
        // }
        if (i == 0) // delete border edge
        {
            Euler::collapse_edge(ed, pm);
            break;
        }
        i++;
    }
    cout << "after collapse e1..........." << endl;
    print_graph(pm);
    return 0;
}