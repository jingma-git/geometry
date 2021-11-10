#pragma once

#include <Eigen/Eigen>
#include "PolyMesh/PolyMesh.h"
#include "circumcent.h"

namespace egl
{
    using namespace acamcad;
    using namespace acamcad::polymesh;

    inline void massmatrix_mixed_voronoi(const PolyMesh &mesh, Eigen::SparseMatrix<double> &M)
    {
        //        v0
        //       *  *
        //      *    *
        //     *  c   *
        //    *        *
        //   *          *
        //  v1**********v2
        // a0 = area(c, v0, v1)/2, a1 = area(c, v1, v2)/2, a2 = area(c, v2, v0)/2

        int m = mesh.numPolygons();
        int n = mesh.numVertices();

        M.reserve(n);
        M.resize(n, n);

        typedef Eigen::Triplet<double> T;
        vector<T> mass;
        mass.reserve(m * 3);

        for (CFaceIter it = mesh.const_polyfaces_begin(); it != mesh.const_polyfaces_end(); it++)
        {
            MHalfedge *hd = (*it)->halfEdge();
            MVert *v0 = hd->fromVertex(), *v1 = hd->toVertex(), *v2 = hd->prev()->fromVertex();

            const MPoint3 &p0 = v0->position();
            const MPoint3 &p1 = v1->position();
            const MPoint3 &p2 = v2->position();
            MVector3 v01 = p1 - p0;
            MVector3 v12 = p2 - p1;
            MVector3 v20 = p0 - p2;
            MVector3 v02 = -v20;
            if (vectorAngle(v01, v02) > M_PI / 2)
            {
                double area = 0.5 * norm(cross(v01, v02));
                mass.push_back(T(v0->index(), v0->index(), 0.5 * area));
                mass.push_back(T(v1->index(), v1->index(), 0.25 * area));
                mass.push_back(T(v2->index(), v2->index(), 0.25 * area));
            }
            else if (vectorAngle(v12, -v01) > M_PI / 2)
            {
                double area = 0.5 * norm(cross(v01, v02));
                mass.push_back(T(v0->index(), v0->index(), 0.25 * area));
                mass.push_back(T(v1->index(), v1->index(), 0.5 * area));
                mass.push_back(T(v2->index(), v2->index(), 0.25 * area));
            }
            else if (vectorAngle(v20, -v12) > M_PI / 2)
            {
                double area = 0.5 * norm(cross(v01, v02));
                mass.push_back(T(v0->index(), v0->index(), 0.25 * area));
                mass.push_back(T(v1->index(), v1->index(), 0.25 * area));
                mass.push_back(T(v2->index(), v2->index(), 0.5 * area));
            }
            else // non-obtuse triangle
            {
                MPoint3 cent = circumcent(p0, p1, p2);
                MPoint3 c01 = (p0 + p1) * 0.5;
                MPoint3 c12 = (p1 + p2) * 0.5;
                MPoint3 c20 = (p2 + p0) * 0.5;
                double a0 = v01.norm() * (c01 - cent).norm() * 0.25; //  TriangleArea(edge*height/2) / 2
                double a1 = v12.norm() * (c12 - cent).norm() * 0.25;
                double a2 = v20.norm() * (c20 - cent).norm() * 0.25;
                mass.push_back(T(v0->index(), v0->index(), a0 + a2));
                mass.push_back(T(v1->index(), v1->index(), a0 + a1));
                mass.push_back(T(v2->index(), v2->index(), a1 + a2));
            }
        }
        M.setFromTriplets(mass.begin(), mass.end());
    }

    inline void massmatrix_barycentric(const PolyMesh &mesh, Eigen::SparseMatrix<double> &M)
    {
        int m = mesh.numPolygons();
        int n = mesh.numVertices();

        M.reserve(n);
        M.resize(n, n);

        typedef Eigen::Triplet<double> T;
        vector<T> mass;
        mass.reserve(m * 3);

        for (CFaceIter it = mesh.const_polyfaces_begin(); it != mesh.const_polyfaces_end(); it++)
        {
            MHalfedge *hd = (*it)->halfEdge();
            MVert *v0 = hd->fromVertex(), *v1 = hd->toVertex(), *v2 = hd->prev()->fromVertex();

            const MPoint3 &p0 = v0->position();
            const MPoint3 &p1 = v1->position();
            const MPoint3 &p2 = v2->position();
            MVector3 v01 = p1 - p0;
            MVector3 v02 = p2 - p0;

            double area = 0.5 * norm(cross(v01, v02)) / 3;
            mass.push_back(T(v0->index(), v0->index(), area));
            mass.push_back(T(v1->index(), v1->index(), area));
            mass.push_back(T(v2->index(), v2->index(), area));
        }
        M.setFromTriplets(mass.begin(), mass.end());
    }
};