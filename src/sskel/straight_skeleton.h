#pragma once

namespace egl
{
    class Straight_skeleton_2
    {
    public:
        typedef typename std::list<SVert>::iterator Vertex_iterator;
        typedef typename std::list<SHalfedge>::iterator Halfedge_iterator;
        typedef typename std::list<SFace>::iterator Face_iterator;

        typedef typename std::list<SVert>::const_iterator Vertex_const_iterator;
        typedef typename std::list<SHalfedge>::const_iterator Halfedge_const_iterator;
        typedef typename std::list<SFace>::const_iterator Face_const_iterator;

    public:
        SVert *vertices_push_back(const SVert &v)
        {
            vertices_.push_back(v);
            return &vertices_.back();
        }

        SHalfedge *edges_push_back(const SHalfedge &h, const SHalfedge &g)
        {
            return add_edge(h, g);
        }

        SFace *faces_push_back(const SFace &f)
        {
            faces_.push_back(f);
            return &faces_.back();
        }

        void vertices_erase(SVert *v)
        {
            // my implementation is not optimal
            // the erasure takes linear time
            for (Vertex_iterator vit = vertices_.begin(); vit != vertices_.end(); ++vit)
            {
                if (vit->id() == v->id())
                {
                    vertices_.erase(vit);
                    break;
                }
            }
        }

        Vertex_iterator vertices_begin() { return vertices_.begin(); }
        Vertex_iterator vertices_end() { return vertices_.end(); }
        Halfedge_iterator halfedges_begin() { return halfedges_.begin(); }
        Halfedge_iterator halfedges_end() { return halfedges_.end(); }
        Face_iterator faces_begin() { return faces_.begin(); }
        Face_iterator faces_end() { return faces_.end(); }

        Vertex_const_iterator vertices_begin() const { return vertices_.begin(); }
        Vertex_const_iterator vertices_end() const { return vertices_.end(); }
        Halfedge_const_iterator halfedges_begin() const { return halfedges_.begin(); }
        Halfedge_const_iterator halfedges_end() const { return halfedges_.end(); }
        Face_const_iterator faces_begin() const { return faces_.begin(); }
        Face_const_iterator faces_end() const { return faces_.end(); }

    private:
        SHalfedge *add_edge(const SHalfedge &h, const SHalfedge &g)
        {
            halfedges_.push_back(h);
            Halfedge_iterator h2 = halfedges_.end();
            --h2;

            halfedges_.push_back(g);
            Halfedge_iterator g2 = halfedges_.end();
            --g2;

            h2->set_opposite(&*g2);
            g2->set_opposite(&*h2);
            return &*h2;
        }

        std::list<SVert> vertices_; // see Effective C++ Item38: model 'has-a' with composition
        std::list<SHalfedge> halfedges_;
        std::list<SFace> faces_;

        size_t nb_border_halfedges_;
        size_t nb_border_edges_;
    };
}