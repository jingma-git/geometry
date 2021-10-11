#pragma once
#include <fstream>
#include <CGAL/IO/OBJ_reader.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
namespace egl
{
    namespace cgal
    {

        template <class Graph>
        void readOBJ(const std::string fname, Graph &g)
        {
            typedef typename boost::property_map<Graph, CGAL::vertex_point_t>::type VertexPointMap;
            typedef typename boost::property_traits<VertexPointMap>::value_type Point_3;
            namespace PMP = CGAL::Polygon_mesh_processing;

            std::ifstream input(fname.c_str());
            std::vector<Point_3> points_ref;
            std::vector<std::vector<std::size_t>> faces_ref;
            if (!input.is_open() || !CGAL::read_OBJ(input, points_ref, faces_ref))
            {
                return;
            }

            PMP::orient_polygon_soup(points_ref, faces_ref); // optional if your mesh is not correctly oriented
            PMP::polygon_soup_to_polygon_mesh(points_ref, faces_ref, g);
        }
    }
}