#pragma once
#include <algorithm>

namespace egl
{
    namespace cgal
    {
        // Convert Polyhedron to std::vector<vector<Float>> V, vector<vector<Int>> F
        // dim: the maximum number of vertices a polyhedron have
        template <class HDS, class Float, class Int>
        void polyhedron2VF(const HDS &g,
                           std::vector<std::vector<Float>> &V,
                           std::vector<std::vector<Int>> &F,
                           Int &dim)
        {
            typedef typename boost::property_map<HDS, CGAL::vertex_point_t>::type VertexPointMap;
            typedef typename boost::property_traits<VertexPointMap>::value_type Point_3;
            typedef typename boost::graph_traits<HDS>::vertex_descriptor Vd;
            typedef typename HDS::Face_const_iterator FIter;
            typedef typename HDS::Halfedge_around_facet_const_circulator Halfedge_facet_circulator;

            using namespace CGAL;

            auto vmap = get(boost::vertex_point, g);
            auto fmap = get(boost::face_index, g);
            size_t numV = num_vertices(g);
            size_t numF = num_faces(g);
            V.resize(numV, std::vector<Float>(3));
            F.resize(numF, std::vector<Int>());
            Int vid = 0;
            for (Vd vd : vertices(g))
            {
                const Point_3 &p = vmap[vd];
                V[vid][0] = p.x();
                V[vid][1] = p.y();
                V[vid][2] = p.z();
                ++vid;
            }

            Int fid = 0;
            dim = -1;
            for (FIter fit = g.facets_begin(); fit != g.facets_end(); ++fit)
            {
                Halfedge_facet_circulator j = fit->facet_begin();
                // Facets in polyhedral surfaces are at least triangles.
                Int fsize = CGAL::circulator_size(j);
                F[fid].reserve(fsize);
                if (fsize > dim)
                    dim = fsize;
                do
                {
                    Int vid = std::distance(g.vertices_begin(), j->vertex());
                    F[fid].push_back(vid);
                } while (++j != fit->facet_begin());

                ++fid;
            }
        }

        // Convert a halfedge datastructure eg. a polyhedron
        // to Eigen::Matrix V (Vertices), F (Cells)
        template <class HDS, class DerivedV, class DerivedF>
        void polyhedron2VF(const HDS &g,
                           Eigen::PlainObjectBase<DerivedV> &V,
                           Eigen::PlainObjectBase<DerivedF> &F)
        {
            typedef typename DerivedV::Scalar Float;
            typedef typename DerivedF::Scalar Int;
            using namespace std;
            vector<vector<Float>> vlist;
            vector<vector<Int>> flist;
            Int dim;
            polyhedron2VF(g, vlist, flist, dim);
            cout << __FILE__ << " " << __LINE__ << ": V=" << vlist.size() << ", F=" << flist.size() << " dim=" << dim << endl;
            V.resize(3, vlist.size());
            for (Int i = 0; i < vlist.size(); ++i)
            {
                V(0, i) = vlist[i][0];
                V(1, i) = vlist[i][1];
                V(2, i) = vlist[i][2];
            }
            F.resize(dim, flist.size());
            for (Int i = 0; i < flist.size(); ++i)
            {
                Int j = 0;
                for (; j < flist[i].size(); ++j)
                {
                    F(j, i) = flist[i][j];
                }
                for (; j < dim; ++j)
                {
                    F(j, i) = -1;
                }
            }
        }
    }
}