#pragma once

#include <CGAL/IO/OBJ_reader.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>

namespace egl
{
    namespace cgal
    {
        void readOBJ(const std::string fname)
        {
            // std::ifstream input(fname.c_str());
            // std::vector<Point> points_ref;
            // std::vector<std::vector<std::size_t>> faces_ref;
            // if (!input || !CGAL::read_OBJ(input, points_ref, faces_ref))
            // {
            //     return;
            // }

            // PMP::orient_polygon_soup(points_ref, faces_ref); // optional if your mesh is not correctly oriented
            // PMP::polygon_soup_to_polygon_mesh(points_ref, faces_ref, cmesh);
            // // cout << "input mesh: vertices=" << cmesh.number_of_vertices() << " faces=" << cmesh.number_of_faces() << endl;
        }
    }
}