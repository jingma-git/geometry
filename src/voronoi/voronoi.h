#pragma once
#include "macros.h"
#include "geom_entity.h"
#include <vector>
#include <Eigen/Eigen>
// http://nms.csail.mit.edu/~aklmiu/6.838/L7.pdf
// https://zhuanlan.zhihu.com/p/33896575
// https://github.com/JCash/voronoi

namespace Voro
{

    struct Vert;
    struct Edge;
    struct Halfedge;
    struct Face;

    struct Vert
    {
        // degree >= 3
        // A voronoi vertex is the center of an empty circle touching 3 or more sites
        int id_;
        Eigen::Vector3d point_;
        Halfedge *he_;
    };

    struct Halfedge
    {
        int id_;
        Vert *v_; // from vertex
        Edge *e_;
        Face *f_;

        Halfedge *prev_, *next_he_;
        Halfedge *pair;
    };

    struct Edge
    {
        // A voronoi edge is a subset of locus of points equidistant to two sites
        int id_;
        Vert *v0_, *v1_;
        Halfedge *he_;
    };

    struct Face // cell
    {
        int id_;
        Halfedge *he_begin_;
    };

    class HalfedgeDS // Halfedge Data Structure
    {
    private:
        std::vector<Vert *> verts_;
        std::vector<Halfedge *> halfedges_;
        std::vector<Edge *> edges_;
        std::vector<Face *> faces_;

    public:
    };
}

#ifdef WITH_INLINE
#include "voronoi.cpp"
#endif