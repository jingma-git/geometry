#include "surface_mesh.h"

namespace geometrycentral
{
    namespace surface
    {
        // Connectivity
        inline size_t SurfaceMesh::heNext(size_t iHe) const { return heNextArr[iHe]; }
        inline size_t SurfaceMesh::heTwin(size_t iHe) const { return usesImplicitTwin() ? heTwinImplicit(iHe) : heSiblingArr[iHe]; }

        inline size_t SurfaceMesh::heSibling(size_t iHe) const { return usesImplicitTwin() ? heTwinImplicit(iHe) : heSiblingArr[iHe]; }
        inline size_t SurfaceMesh::heNextIncomingNeighbor(size_t iHe) const
        {
            return usesImplicitTwin() ? heTwinImplicit(heNextArr[iHe]) : heVertInNextArr[iHe];
        }
        inline size_t SurfaceMesh::heNextOutgoingNeighbor(size_t iHe) const
        {
            return usesImplicitTwin() ? heNextArr[heTwinImplicit(iHe)] : heVertOutNextArr[iHe];
        }
        inline size_t SurfaceMesh::heEdge(size_t iHe) const { return usesImplicitTwin() ? heEdgeImplicit(iHe) : heEdgeArr[iHe]; }
        inline size_t SurfaceMesh::heVertex(size_t iHe) const { return heVertexArr[iHe]; }
        inline size_t SurfaceMesh::heFace(size_t iHe) const { return heFaceArr[iHe]; }
        inline bool SurfaceMesh::heOrientation(size_t iHe) const { return usesImplicitTwin() ? (iHe % 2) == 0 : heOrientArr[iHe]; }
        inline size_t SurfaceMesh::eHalfedge(size_t iE) const { return usesImplicitTwin() ? eHalfedgeImplicit(iE) : eHalfedgeArr[iE]; }
        inline size_t SurfaceMesh::vHalfedge(size_t iV) const { return vHalfedgeArr[iV]; }
        inline size_t SurfaceMesh::fHalfedge(size_t iF) const { return fHalfedgeArr[iF]; }

        // Implicit relationships
        inline bool SurfaceMesh::usesImplicitTwin() const { return useImplicitTwinFlag; }
        inline size_t SurfaceMesh::heTwinImplicit(size_t iHe) { return iHe ^ 1; }  // static
        inline size_t SurfaceMesh::heEdgeImplicit(size_t iHe) { return iHe / 2; }  // static
        inline size_t SurfaceMesh::eHalfedgeImplicit(size_t iE) { return 2 * iE; } // static

        // Other getters
        inline bool SurfaceMesh::heIsInterior(size_t iHe) const { return !faceIsBoundaryLoop(heFaceArr[iHe]); }
        inline bool SurfaceMesh::faceIsBoundaryLoop(size_t iF) const { return iF >= nFacesFillCount; }
        inline size_t SurfaceMesh::faceIndToBoundaryLoopInd(size_t iF) const { return nFacesCapacityCount - 1 - iF; }
        inline size_t SurfaceMesh::boundaryLoopIndToFaceInd(size_t iB) const { return nFacesCapacityCount - 1 - iB; }

        // Detect dead elements
        inline bool SurfaceMesh::vertexIsDead(size_t iV) const { return vHalfedgeArr[iV] == INVALID_IND; }
        inline bool SurfaceMesh::halfedgeIsDead(size_t iHe) const { return heNextArr[iHe] == INVALID_IND; }
        inline bool SurfaceMesh::edgeIsDead(size_t iE) const { return usesImplicitTwin() ? heNextArr[eHalfedgeImplicit(iE)] == INVALID_IND : eHalfedgeArr[iE] == INVALID_IND; }
        inline bool SurfaceMesh::faceIsDead(size_t iF) const { return fHalfedgeArr[iF] == INVALID_IND; }

        // Methods for getting number of mesh elements
        inline size_t SurfaceMesh::nHalfedges() const { return nHalfedgesCount; }
        inline size_t SurfaceMesh::nInteriorHalfedges() const { return nInteriorHalfedgesCount; }
        inline size_t SurfaceMesh::nExteriorHalfedges() const { return nHalfedgesCount - nInteriorHalfedgesCount; }
        inline size_t SurfaceMesh::nCorners() const { return nInteriorHalfedgesCount; }
        inline size_t SurfaceMesh::nVertices() const { return nVerticesCount; }
        inline size_t SurfaceMesh::nEdges() const { return nEdgesCount; }
        inline size_t SurfaceMesh::nFaces() const { return nFacesCount; }
        inline size_t SurfaceMesh::nBoundaryLoops() const { return nBoundaryLoopsCount; }

        // Methods for iterating over mesh elements w/ range-based for loops ===========
        inline VertexSet SurfaceMesh::vertices() { return VertexSet(this, 0, nVerticesFillCount); }
        inline HalfedgeSet SurfaceMesh::halfedges() { return HalfedgeSet(this, 0, nHalfedgesFillCount); }
        inline HalfedgeInteriorSet SurfaceMesh::interiorHalfedges() { return HalfedgeInteriorSet(this, 0, nHalfedgesFillCount); }
        inline HalfedgeExteriorSet SurfaceMesh::exteriorHalfedges() { return HalfedgeExteriorSet(this, 0, nHalfedgesFillCount); }
        inline CornerSet SurfaceMesh::corners() { return CornerSet(this, 0, nHalfedgesFillCount); }
        inline EdgeSet SurfaceMesh::edges() { return EdgeSet(this, 0, nEdgesFillCount); }
        inline FaceSet SurfaceMesh::faces() { return FaceSet(this, 0, nFacesFillCount); }
        inline BoundaryLoopSet SurfaceMesh::boundaryLoops() { return BoundaryLoopSet(this, 0, nBoundaryLoopsFillCount); }

        // Methods for accessing elements by index =====================================
        // Note that these are only valid when the mesh is compressed.
        inline Vertex SurfaceMesh::vertex(size_t index) { return Vertex(this, index); }
        inline Halfedge SurfaceMesh::halfedge(size_t index) { return Halfedge(this, index); }
        inline Corner SurfaceMesh::corner(size_t index) { return Corner(this, index); }
        inline Edge SurfaceMesh::edge(size_t index) { return Edge(this, index); }
        inline Face SurfaceMesh::face(size_t index) { return Face(this, index); }
        inline BoundaryLoop SurfaceMesh::boundaryLoop(size_t index) { return BoundaryLoop(this, index); }
    }
}