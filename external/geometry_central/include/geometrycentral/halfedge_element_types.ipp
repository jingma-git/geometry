#include "halfedge_element_types.h"

namespace geometrycentral
{
    namespace surface
    {
        // ================        Vertex          ==================
        // ==========================================================

        // Constructors
        // (see note in header, these should be inherited but aren't due to compiler issues)
        inline Vertex::Vertex() {}
        inline Vertex::Vertex(SurfaceMesh *mesh_, size_t ind_) : Element(mesh_, ind_) {}

        // Navigators
        inline Halfedge Vertex::halfedge() const { return Halfedge(mesh, mesh->vHalfedge(ind)); }
        inline Corner Vertex::corner() const { return halfedge().corner(); }
        inline bool Vertex::isDead() const { return mesh->vertexIsDead(ind); }

        // Circulators
        inline NavigationSetBase<VertexAdjacentVertexNavigator> Vertex::adjacentVertices() const
        {
            return NavigationSetBase<VertexAdjacentVertexNavigator>(VertexNeighborIteratorState(halfedge(), mesh->usesImplicitTwin()));
        }
        inline NavigationSetBase<VertexIncomingHalfedgeNavigator> Vertex::incomingHalfedges() const
        {
            return NavigationSetBase<VertexIncomingHalfedgeNavigator>(halfedge().prevOrbitFace());
        }
        inline NavigationSetBase<VertexOutgoingHalfedgeNavigator> Vertex::outgoingHalfedges() const
        {
            return NavigationSetBase<VertexOutgoingHalfedgeNavigator>(halfedge());
        }
        inline NavigationSetBase<VertexAdjacentFaceNavigator> Vertex::adjacentFaces() const
        {
            return NavigationSetBase<VertexAdjacentFaceNavigator>(halfedge());
        }
        inline NavigationSetBase<VertexAdjacentEdgeNavigator> Vertex::adjacentEdges() const
        {
            return NavigationSetBase<VertexAdjacentEdgeNavigator>(VertexNeighborIteratorState(halfedge(), mesh->usesImplicitTwin()));
        }
        inline NavigationSetBase<VertexAdjacentCornerNavigator> Vertex::adjacentCorners() const
        {
            return NavigationSetBase<VertexAdjacentCornerNavigator>(halfedge());
        }

        // == Range iterators
        inline bool VertexRangeF::elementOkay(const SurfaceMesh &mesh, size_t ind)
        {
            return !mesh.vertexIsDead(ind);
        }

        // ================        Halfedge        ==================
        // ==========================================================

        // Constructors
        inline Halfedge::Halfedge() {}
        inline Halfedge::Halfedge(SurfaceMesh *mesh_, size_t ind_) : Element(mesh_, ind_) {}

        // Navigators
        inline Halfedge Halfedge::twin() const { return Halfedge(mesh, mesh->heSibling(ind)); }
        inline Halfedge Halfedge::sibling() const { return Halfedge(mesh, mesh->heSibling(ind)); }
        inline Halfedge Halfedge::next() const { return Halfedge(mesh, mesh->heNext(ind)); }
        inline Vertex Halfedge::vertex() const { return Vertex(mesh, mesh->heVertex(ind)); }
        inline Vertex Halfedge::tipVertex() const { return next().vertex(); }
        inline Vertex Halfedge::tailVertex() const { return vertex(); }
        inline Halfedge Halfedge::nextOutgoingNeighbor() const { return Halfedge(mesh, mesh->heNextOutgoingNeighbor(ind)); }
        inline Halfedge Halfedge::nextIncomingNeighbor() const { return Halfedge(mesh, mesh->heNextIncomingNeighbor(ind)); }
        inline Edge Halfedge::edge() const { return Edge(mesh, mesh->heEdge(ind)); }
        inline Face Halfedge::face() const { return Face(mesh, mesh->heFace(ind)); }
        inline Corner Halfedge::corner() const { return Corner(mesh, ind); }
        inline bool Halfedge::isDead() const { return mesh->halfedgeIsDead(ind); }

        // Properties
        inline bool Halfedge::isInterior() const { return mesh->heIsInterior(ind); }
        inline bool Halfedge::orientation() const { return mesh->heOrientation(ind); }

        // Super-navigators
        inline Halfedge Halfedge::prevOrbitFace() const
        {
            Halfedge currHe = *this;
            while (true)
            {
                Halfedge nextHe = currHe.next();
                if (nextHe == *this)
                    break;
                currHe = nextHe;
            }
            return currHe;
        }
        inline Halfedge Halfedge::prevOrbitVertex() const
        {
            Halfedge currHe = twin();
            while (true)
            {
                Halfedge nextHe = currHe.next();
                if (nextHe == *this)
                    break;
                currHe = nextHe.twin();
            }
            return currHe;
        }

        // Range iterators
        inline bool HalfedgeRangeF::elementOkay(const SurfaceMesh &mesh, size_t ind)
        {
            return !mesh.halfedgeIsDead(ind);
        }
        inline bool HalfedgeInteriorRangeF::elementOkay(const SurfaceMesh &mesh, size_t ind)
        {
            return !mesh.halfedgeIsDead(ind) && mesh.heIsInterior(ind);
        }
        inline bool HalfedgeExteriorRangeF::elementOkay(const SurfaceMesh &mesh, size_t ind)
        {
            return !mesh.halfedgeIsDead(ind) && !mesh.heIsInterior(ind);
        }

        // ================        Corner          ==================
        // ==========================================================

        // Constructors
        inline Corner::Corner() {}
        inline Corner::Corner(SurfaceMesh *mesh_, size_t ind_) : Element(mesh_, ind_) {}

        // Navigators
        inline Halfedge Corner::halfedge() const { return Halfedge(mesh, ind); }
        inline Vertex Corner::vertex() const { return halfedge().vertex(); }
        inline Face Corner::face() const { return halfedge().face(); }
        inline bool Corner::isDead() const { return halfedge().isDead(); }

        // Range iterators
        inline bool CornerRangeF::elementOkay(const SurfaceMesh &mesh, size_t ind)
        {
            return !mesh.halfedgeIsDead(ind) && mesh.heIsInterior(ind);
        }

        // ================          Edge          ==================
        // ==========================================================

        // Constructors
        inline Edge::Edge() {}
        inline Edge::Edge(SurfaceMesh *mesh_, size_t ind_) : Element(mesh_, ind_) {}

        // Navigator
        inline Halfedge Edge::halfedge() const { return Halfedge(mesh, mesh->eHalfedge(ind)); }
        inline Vertex Edge::otherVertex(Vertex v) const
        {
            if (halfedge().tailVertex() == v)
            {
                return halfedge().tipVertex();
            }
            else
            {
                return halfedge().tailVertex();
            }
        }
        inline Vertex Edge::firstVertex() const { return halfedge().tailVertex(); }
        inline Vertex Edge::secondVertex() const { return halfedge().tipVertex(); }
        inline std::array<Halfedge, 4> Edge::diamondBoundary() const
        {
            Halfedge h = halfedge();
            Halfedge hN = h.next();
            Halfedge hNN = hN.next();

            Halfedge hT = h.sibling();
            Halfedge hTN = hT.next();
            Halfedge hTNN = hTN.next();

#ifndef NGC_SAFETY_CHECKS
            if (hT == h || !hT.isInterior())
                throw std::runtime_error("cannot construct diamondBoundary() of boundary edge");
            if (hT.sibling() != h)
                throw std::runtime_error("cannot construct diamondBoundary() of nonmanifold edge");
            if (hNN.next() != h)
                throw std::runtime_error("cannot construct diamondBoundary() for non-triangular face");
            if (hTNN.next() != hT)
                throw std::runtime_error("cannot construct diamondBoundary() for non-triangular face");
#endif

            return std::array<Halfedge, 4>{hN, hNN, hTN, hTNN};
        }
        inline bool Edge::isDead() const { return mesh->edgeIsDead(ind); }

        // Range iterators
        inline bool EdgeRangeF::elementOkay(const SurfaceMesh &mesh, size_t ind)
        {
            return !mesh.edgeIsDead(ind);
        }
        // ================          Face          ==================
        // ==========================================================

        // Constructors
        inline Face::Face() {}
        inline Face::Face(SurfaceMesh *mesh_, size_t ind_) : Element(mesh_, ind_) {}
        // inline Face::Face(const DynamicElement<Face>& e) : Element(e.getMesh(), e.getIndex()) {}

        // Navigators
        inline Halfedge Face::halfedge() const { return Halfedge(mesh, mesh->fHalfedge(ind)); }
        inline BoundaryLoop Face::asBoundaryLoop() const
        {
            GC_SAFETY_ASSERT(isBoundaryLoop(), "face must be boundary loop to call asBoundaryLoop()")
            return BoundaryLoop(mesh, mesh->faceIndToBoundaryLoopInd(ind));
        }
        inline bool Face::isDead() const { return mesh->faceIsDead(ind); }

        // Properties
        inline bool Face::isTriangle() const
        {
            Halfedge he = halfedge();
            return he == he.next().next().next();
        }

        inline bool Face::isBoundaryLoop() const { return mesh->faceIsBoundaryLoop(ind); }

        // == Range iterators
        inline bool FaceRangeF::elementOkay(const SurfaceMesh &mesh, size_t ind)
        {
            return !mesh.faceIsDead(ind);
        }
        // ================     Boundary Loop      ==================
        // ==========================================================

        // Constructors
        inline BoundaryLoop::BoundaryLoop() {}
        inline BoundaryLoop::BoundaryLoop(SurfaceMesh *mesh_, size_t ind_) : Element(mesh_, ind_) {}

        inline Halfedge BoundaryLoop::halfedge() const { return asFace().halfedge(); }
        inline Face BoundaryLoop::asFace() const { return Face(mesh, mesh->boundaryLoopIndToFaceInd(ind)); }
        inline bool BoundaryLoop::isDead() const { return asFace().isDead(); }

        // == Range iterators
        inline bool BoundaryLoopRangeF::elementOkay(const SurfaceMesh &mesh, size_t ind)
        {
            return !mesh.faceIsDead(mesh.boundaryLoopIndToFaceInd(ind));
        }

        // == Navigation iterators

        // Iterating around vertices on a nonmanifold/nonoriented mesh is hard.
        // Our data structure offers iteration around the outgoing halfedges from each vertex,
        // which takes care of outoing halfedges.
        // Furthermore one thing we know is that for every face in which the vertex appears,
        // there is at least one incoming and one outgoing halfedge.
        //  We can use this to find the faces and outgoing halfedges.  For edges and vertices,
        // In both the mannifold and nonmanifold case, if a vertex appears in a face multiple times,
        // (aka its a Delta-complex), then these iterators will return elements multiple times.

        inline VertexNeighborIteratorState::VertexNeighborIteratorState(Halfedge currHe_, bool useImplicitTwin_) : useImplicitTwin(useImplicitTwin_), currHe(currHe_), firstHe(currHe_) {}

        inline void VertexNeighborIteratorState::advance()
        {
            if (useImplicitTwin)
            {
                currHe = currHe.nextOutgoingNeighbor();
            }
            else
            {
                if (!processingIncoming)
                {
                    currHe = currHe.nextOutgoingNeighbor();
                    if (currHe == firstHe) // switch to processing incoming if needed
                    {
                        processingIncoming = true;
                        currHe = currHe.prevOrbitFace();
                        firstHe = currHe;
                    }
                }
                else
                {
                    currHe = currHe.nextIncomingNeighbor();
                    if (currHe == firstHe) // switch back to processing outcoming if needed
                    {
                        processingIncoming = false;
                        currHe = currHe.next();
                        firstHe = currHe;
                    }
                }
            }
        }

        inline bool VertexNeighborIteratorState::isHalfedgeCanonical() const
        {
            // TODO I _think_ that this leads to different Delta-complex behavior on implicit twin vs. without wrt yielding
            // elements multiple times when there is a self-edge...
            if (useImplicitTwin)
            {
                return true;
            }
            else
            {
                return currHe == currHe.edge().halfedge();
            }
        }

        inline bool VertexNeighborIteratorState::operator==(const VertexNeighborIteratorState &rhs) const
        {
            return currHe == rhs.currHe && processingIncoming == rhs.processingIncoming;
        }

        inline void VertexAdjacentVertexNavigator::advance() { currE.advance(); }
        inline bool VertexAdjacentVertexNavigator::isValid() const { return currE.isHalfedgeCanonical(); }
        inline Vertex VertexAdjacentVertexNavigator::getCurrent() const
        {
            if (currE.useImplicitTwin || !currE.processingIncoming)
            {
                return currE.currHe.next().vertex();
            }
            else
            {
                return currE.currHe.vertex();
            }
        }

        inline void VertexIncomingHalfedgeNavigator::advance() { currE = currE.nextIncomingNeighbor(); }
        inline bool VertexIncomingHalfedgeNavigator::isValid() const { return true; }
        inline Halfedge VertexIncomingHalfedgeNavigator::getCurrent() const { return currE; }

        inline void VertexOutgoingHalfedgeNavigator::advance() { currE = currE.nextOutgoingNeighbor(); }
        inline bool VertexOutgoingHalfedgeNavigator::isValid() const { return true; }
        inline Halfedge VertexOutgoingHalfedgeNavigator::getCurrent() const { return currE; }

        inline void VertexAdjacentCornerNavigator::advance() { currE = currE.nextOutgoingNeighbor(); }
        inline bool VertexAdjacentCornerNavigator::isValid() const { return currE.isInterior(); }
        inline Corner VertexAdjacentCornerNavigator::getCurrent() const { return currE.corner(); }

        inline void VertexAdjacentEdgeNavigator::advance() { currE.advance(); }
        inline bool VertexAdjacentEdgeNavigator::isValid() const { return currE.isHalfedgeCanonical(); }
        inline Edge VertexAdjacentEdgeNavigator::getCurrent() const { return currE.currHe.edge(); }

        inline void VertexAdjacentFaceNavigator::advance() { currE = currE.nextOutgoingNeighbor(); }
        inline bool VertexAdjacentFaceNavigator::isValid() const { return currE.isInterior(); }
        inline Face VertexAdjacentFaceNavigator::getCurrent() const { return currE.face(); }
    }

}