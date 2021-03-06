#pragma once
#include <Eigen/Dense>
#include <iostream>
namespace egl
{
    // done in column wise fanshion, which is #v1, #v2..., #vn
    template <typename DerivedV, typename DerivedF, typename DerivedEV,
              typename DerivedFE, typename DerivedEF>
    void edge_topology(
        const Eigen::MatrixBase<DerivedV> &V,
        const Eigen::MatrixBase<DerivedF> &F,
        Eigen::PlainObjectBase<DerivedEV> &EV, // edge_verts
        Eigen::PlainObjectBase<DerivedFE> &FE, // face2edge
        Eigen::PlainObjectBase<DerivedEF> &EF) // edge2face
    {
        using namespace std;

        size_t numV = V.cols();
        size_t numF = F.cols();
        assert(F.rows() == 3 && "Only support triangle mesh!");

        vector<vector<size_t>> emap; // v1 v2 face index_on_the_face
        for (size_t i = 0; i < numF; ++i)
        {
            for (size_t j = 0; j < 3; ++j)
            {
                vector<size_t> edge(4);
                edge[0] = F(j, i);
                edge[1] = F((j + 1) % 3, i);
                if (edge[0] > edge[1])
                    std::swap(edge[0], edge[1]);
                edge[2] = i;
                edge[3] = j;
                emap.push_back(edge);
            }
        }
        std::sort(emap.begin(), emap.end());

        size_t numE = 1; // the last one is always counted
        for (size_t i = 0; i < emap.size() - 1; ++i)
            if (!(emap[i][0] == emap[i + 1][0] && emap[i][1] == emap[i + 1][1]))
                numE++;

        EV = DerivedEV::Constant(2, numE, -1);
        FE = DerivedFE::Constant(3, numF, -1); // a face has 3 edges
        EF = DerivedEF::Constant(2, numE, -1);
        size_t ei = 0;
        for (size_t i = 0; i < emap.size(); ++i)
        {
            const auto &r = emap[i];
            if (i == emap.size() - 1 || !(emap[i][0] == emap[i + 1][0] && emap[i][1] == emap[i + 1][1]))
            {
                // border edge
                EV(0, ei) = r[0];
                EV(1, ei) = r[1];
                EF(0, ei) = r[2];
                FE(r[3], r[2]) = ei;
            }
            else
            {
                const auto &l = emap[i + 1];
                EV(0, ei) = r[0];
                EV(1, ei) = r[1];
                EF(0, ei) = r[2];
                EF(1, ei) = l[2];
                FE(r[3], r[2]) = ei;
                FE(l[3], l[2]) = ei;
                ++i; // skip the next edge
            }
            ++ei;
        }

        // Sort the relation EF, accordingly to EV
        // the first one is the face on the left of the edge
        for (size_t i = 0; i < EF.cols(); ++i)
        {
            bool flip = true;
            size_t fid = EF(0, i);
            for (size_t j = 0; j < 3; ++j)
            {
                if (F(j, fid) == EF(0, i) && F((j + 1) % 3, fid) == EF(1, i))
                {
                    flip = false;
                    break;
                }
            }
            if (flip)
            {
                typename DerivedEF::Scalar tmp = EF(0, i);
                EF(0, i) = EF(1, i);
                EF(1, i) = tmp;
            }
        }
    }

    template <typename DerivedV, typename DerivedF>
    class MeshTopology
    {
        typedef typename DerivedF::Scalar Index;

    public:
        MeshTopology(const Eigen::MatrixBase<DerivedV> &V,
                     const Eigen::MatrixBase<DerivedF> &F) : V_(V), F_(F)
        {
            edge_topology(V_, F_, edges, face2edge, edge2face);
        }

        bool is_boundary(size_t ei)
        {
            return edge2face(0, ei) == -1 || edge2face(1, ei) == -1;
        }

        template <typename DerivedE>
        void boundary_edges(Eigen::PlainObjectBase<DerivedE> &bnd_edges)
        {
            std::vector<size_t> bidxs;
            for (size_t i = 0; i < edges.cols(); ++i)
            {
                if (is_boundary(i))
                    bidxs.push_back(i);
            }
            bnd_edges.resize(2, bidxs.size());
            for (size_t i = 0; i < bnd_edges.cols(); ++i)
            {
                bnd_edges.col(i) = edges.col(bidxs[i]);
            }
        }

        template <typename DerivedE>
        void inner_edges(Eigen::PlainObjectBase<DerivedE> &inner_edges)
        {
            std::vector<size_t> in_idxs;
            for (size_t i = 0; i < edges.cols(); ++i)
            {
                if (!is_boundary(i))
                    in_idxs.push_back(i);
            }

            inner_edges.resize(2, in_idxs.size());
            for (size_t i = 0; i < inner_edges.cols(); ++i)
            {
                inner_edges.col(i) = edges.col(in_idxs[i]);
            }
        }

        Eigen::Matrix<Index, 2, -1> edges;     // 2 by #edges, record edge vertex
        Eigen::Matrix<Index, 3, -1> face2edge; // 3 by #faces, record face edges
        Eigen::Matrix<Index, 2, -1> edge2face; // 2 by #edges, record edge faces

    private:
        const Eigen::MatrixBase<DerivedV> &V_;
        const Eigen::MatrixBase<DerivedF> &F_;
    };
}
