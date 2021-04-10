#pragma once
#include "PolyMesh/PolyMesh.h"
#include <Eigen/Eigen>
#include <iostream>

namespace egl
{
    using namespace acamcad;
    using namespace acamcad::polymesh;
    using namespace std;

    // only works for triangle mesh now
    inline void cotmatrix_entries(const PolyMesh &mesh, Eigen::MatrixXd &cot)
    {
        using namespace std;
        int m = mesh.numPolygons();
        cot.resize(m, 3);
        for (CFaceIter it = mesh.const_polyfaces_begin(); it != mesh.const_polyfaces_end(); it++)
        {
            MPolyFace *f = *it;
            int i = f->index();
            int e = 0;
            std::cout << *f << endl;
            for (FaceHalfEdgeIter hit = mesh.fhe_iter(f); hit.isValid(); hit++)
            {
                MHalfedge *hd = *hit;
                MVert *v0 = hd->fromVertex(), *v1 = hd->toVertex(), *v2 = hd->prev()->fromVertex();
                MVector3 v20 = v0->position() - v2->position();
                MVector3 v21 = v1->position() - v2->position();
                double sina = norm(cross(v20, v21));
                cot(i, e) = sina / dot(v20, v21);
                assert(std::isnan(cot(i, e)) == false && "Degenerate face!");
                e++;
            }
        }
    }

    inline void cotmatrix_halfedge_impl(const PolyMesh &mesh,
                                        Eigen::SparseMatrix<double> &L)
    {
        // http://rodolphe-vaillant.fr/?e=69
        // should be very careful about boundary edge!!!
        // * means arrow from vi to vj
        //         vi
        //       /  | \
        //     va   |  vb
        //       \  * /
        //         vj
        int n = mesh.numVertices();
        typedef Eigen::Triplet<double> T;
        std::vector<T> cot;
        for (CVertexIter it = mesh.const_vertices_begin(); it != mesh.const_vertices_end(); it++)
        {
            MVert *vi = (*it);
            int i = vi->index();

            for (VertexOHalfEdgeIter hit = mesh.voh_iter(vi); hit.isValid(); hit++)
            {
                MHalfedge *hd = *hit;

                MVert *vj = (*hit)->toVertex();
                int j = vj->index();

                double cota = 0.0;
                if (!(*hit)->pair()->isBoundary())
                {
                    MVert *va = (*hit)->pair()->prev()->fromVertex();
                    MVector3 vai = vi->position() - va->position();
                    MVector3 vaj = vj->position() - va->position();
                    double sina = norm(cross(vai, vaj));
                    cota = dot(vai, vaj) / sina;
                    if (std::isnan(cota))
                    {
                        std::cout << "cosa=" << dot(vai, vaj) << " sina=" << sina << " cota=" << cota << std::endl;
                        assert(false && "Degenerate face!");
                    }
                }

                double cotb = 0.0;
                if (!(*hit)->isBoundary())
                {
                    MVert *vb = (*hit)->prev()->fromVertex();
                    MVector3 vbi = vi->position() - vb->position();
                    MVector3 vbj = vj->position() - vb->position();
                    double sinb = norm(cross(vbi, vbj));
                    cotb = dot(vbi, vbj) / sinb;

                    if (std::isnan(cotb))
                    {
                        std::cout << "cosb=" << dot(vbi, vbj) << " sinb=" << sinb << " cotb=" << cotb << std::endl;
                        assert(false && "Degenerate face!");
                    }
                }

                double cot_ab = 0.5 * (cota + cotb);
                cot.push_back(T(i, i, -cot_ab));
                cot.push_back(T(i, j, cot_ab));
            }
        }

        L.resize(n, n);
        L.setFromTriplets(cot.begin(), cot.end());
    }

    inline void uniform_lapmatrix_halfedge_impl(const PolyMesh &mesh,
                                                Eigen::SparseMatrix<double> &L)
    {
        // * means arrow from vi to vj
        //         vi
        //       /  | \
        //     va   |  vb
        //       \  * /
        //         vj
        int n = mesh.numVertices();
        typedef Eigen::Triplet<double> T;
        std::vector<T> cot;
        for (CVertexIter it = mesh.const_vertices_begin(); it != mesh.const_vertices_end(); it++)
        {
            MVert *vi = (*it);
            int i = vi->index();

            for (VertexOHalfEdgeIter hit = mesh.voh_iter(vi); hit.isValid(); hit++)
            {
                MVert *vj = (*hit)->toVertex();
                int j = vj->index();
                cot.push_back(T(i, i, -1));
                cot.push_back(T(i, j, 1));
            }
        }

        L.resize(n, n);
        L.setFromTriplets(cot.begin(), cot.end());
    }

    inline void cotmatrix(const PolyMesh &mesh, Eigen::SparseMatrix<double> &L)
    {
        // L(X) = div grad(X)
        int n = mesh.numVertices();
        L.resize(n, n);
        L.reserve(n * 10);
        typedef Eigen::Triplet<double> T;
        std::vector<T> Lcot;
        Lcot.reserve(n * 4);
        for (CFaceIter it = mesh.const_polyfaces_begin(); it != mesh.const_polyfaces_end(); it++)
        {
            MPolyFace *f = *it;
            // cout << "o------------" << *f << endl;
            int i = f->index();
            for (FaceHalfEdgeIter hit = mesh.fhe_iter(f); hit.isValid(); hit++)
            {
                MHalfedge *hd = *hit;
                MVert *v0 = hd->fromVertex(), *v1 = hd->toVertex(), *v2 = hd->prev()->fromVertex();
                int i = v0->index();
                int j = v1->index();
                MVector3 v20 = v0->position() - v2->position();
                MVector3 v21 = v1->position() - v2->position();
                double sina = norm(cross(v20, v21));
                double cota = 0.5 * dot(v20, v21) / sina;
                // cout << *f << " " << *v0 << "->" << *v1 << " cot" << *v2 << "=" << cota << endl;
                assert(std::isnan(cota) == false && "Degenerate face!");
                Lcot.push_back(T(i, i, -cota));
                Lcot.push_back(T(j, j, -cota));
                Lcot.push_back(T(i, j, cota));
                Lcot.push_back(T(j, i, cota));
            }
        }
        L.setFromTriplets(Lcot.begin(), Lcot.end());
    }
};