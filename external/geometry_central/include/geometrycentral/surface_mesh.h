#pragma once
#include "halfedge_element_types.h"

#include <list>
#include <memory>
#include <vector>
namespace geometrycentral
{
    namespace surface
    {
        class SurfaceMesh
        {
        public:
            // Expansion callbacks
            // Argument is the new size of the element list. Elements up to this index may now be used (but _might_ not be
            // in use immediately).
            std::list<std::function<void(size_t)>> vertexExpandCallbackList;
            std::list<std::function<void(size_t)>> faceExpandCallbackList;
            std::list<std::function<void(size_t)>> edgeExpandCallbackList;
            std::list<std::function<void(size_t)>> halfedgeExpandCallbackList;

            // Mesh delete callbacks
            // (this unfortunately seems to be necessary; objects which have registered their callbacks above
            // need to know not to try to de-register them if the mesh has been deleted)
            std::list<std::function<void()>> meshDeleteCallbackList;

            // Build a halfedge mesh from polygons, with a list of 0-indexed vertices incident on each face, in CCW order.
            // Assumes that the vertex listing in polygons is dense; all indices from [0,MAX_IND) must appear in some face.
            // (some functions, like in meshio.h preprocess inputs to strip out unused indices).
            // The output will preserve the ordering of vertices and faces.
            SurfaceMesh(const std::vector<std::vector<size_t>> &polygons);

            // // like above, but with an FxD array input, e.g. Fx3 for triangle mesh or Fx4 for quads. T should be some integer type.
            // template <typename T>
            // SurfaceMesh(const Eigen::MatrixBase<T> &triangles);

            // Build a halfedge mesh from connectivity information (0-indexed as always)
            // - `polygons` is the usual vertex indices for each face
            // - `twins` is indices for the halfedge twin pointers. For each halfedge, holds the index of the twin face and
            // halfedge within that face. In each face, the 0'th halfedge goes from vert 0-->1. Use INVALID_IND for boundary.
            SurfaceMesh(const std::vector<std::vector<size_t>> &polygons,
                        const std::vector<std::vector<std::tuple<size_t, size_t>>> &twins);

            virtual ~SurfaceMesh();

            // Number of mesh elements of each type
            size_t nHalfedges() const;
            size_t nInteriorHalfedges() const;
            size_t nCorners() const;
            size_t nVertices() const;
            size_t nInteriorVertices(); // warning: O(n)
            size_t nEdges() const;
            size_t nFaces() const;
            size_t nBoundaryLoops() const;
            size_t nExteriorHalfedges() const;

            // Methods for range-based for loops
            // Example: for(Vertex v : mesh.vertices()) { ... }
            HalfedgeSet halfedges();
            HalfedgeInteriorSet interiorHalfedges();
            HalfedgeExteriorSet exteriorHalfedges();
            CornerSet corners();
            VertexSet vertices();
            EdgeSet edges();
            FaceSet faces();
            BoundaryLoopSet boundaryLoops();

            // Methods for accessing elements by index
            // only valid when the  mesh is compressed
            Halfedge halfedge(size_t index);
            Corner corner(size_t index);
            Vertex vertex(size_t index);
            Edge edge(size_t index);
            Face face(size_t index);
            BoundaryLoop boundaryLoop(size_t index);

            // Check capacity. Needed when implementing expandable containers for mutable meshes to ensure the contain can
            // hold a sufficient number of elements before the next resize event.
            size_t nHalfedgesCapacity() const;
            size_t nVerticesCapacity() const;
            size_t nEdgesCapacity() const;
            size_t nFacesCapacity() const;
            size_t nBoundaryLoopsCapacity() const;

            // Does this mesh use the implicit-twin convention in its connectivity arrays?
            // This is essentially an implementation detail, and external users _probably_ shouldn't touch it, but it can be
            // useful to know when manipulating the data structure.
            bool usesImplicitTwin() const;

        protected:
            // Constructor used by subclasses
            SurfaceMesh(bool useImplicitTwin = false);

            // // Construct directly from internal arrays
            // SurfaceMesh(const std::vector<size_t> &heNextArr, const std::vector<size_t> &heVertexArr,
            //             const std::vector<size_t> &heFaceArr, const std::vector<size_t> &vHalfedgeArr,
            //             const std::vector<size_t> &fHalfedgeArr, const std::vector<size_t> &heSiblingArr,
            //             const std::vector<size_t> &heEdgeArr, const std::vector<char> &heOrientArr,
            //             const std::vector<size_t> &eHalfedgeArr, size_t nBoundaryLoopFillCount);

            // = Core arrays which hold the connectivity
            // Note: it should always be true that heFace.size() == nHalfedgesCapacityCount, but any elements after
            // nHalfedgesFillCount will be valid indices (in the std::vector sense), but contain uninitialized data. Similarly,
            // any std::vector<> indices corresponding to deleted elements will hold meaningless values.
            std::vector<size_t> heNextArr;
            std::vector<size_t> heVertexArr;
            std::vector<size_t> heFaceArr;
            std::vector<size_t> vHalfedgeArr;
            std::vector<size_t> fHalfedgeArr;

            // (note: three more of these below for when not using implicit twin)

            // Does this mesh use the implicit-twin convention in its connectivity arrays?
            //
            // If true, heSibling, heEdge, eHalfedge, and heVert_ are all empty arrays, and these values are computed implicitly
            // from arithmetic on the indices. A consequence is that the mesh can only represent edge-manifold, oriented surfaces.
            //
            // If false, the above arrays are all populated and used for connectivty. As a consequence, the resulting mesh might
            // not be manifold/oriented, and extra care is needed for some routines.
            const bool useImplicitTwinFlag;

            std::vector<size_t> heSiblingArr;
            std::vector<size_t> heEdgeArr; // true if the halfedge has the same orientation as its edge
            std::vector<size_t> heOrientArr;
            std::vector<size_t> eHalfedgeArr;

            // These form a doubly-linked list of the halfedges around each vertex, providing the richer data needed to iterate
            // around vertices in a nonmanifold mesh. These encode connectivity, but are redundant given the other arrays above,
            // so they don't need to be serialized (etc). Note the removeFromVertexLists() and addToVertexLists() below to
            // simplify maintaining these internally.
            // Similar to CSRMatrix, vHeIn(Out)Arr record outer iteration indices (indices of halfedges)
            //                       heVertIn(Out)Next(Prev) record inner iteration indices (indices of halfedges)
            std::vector<size_t> heVertInNextArr;
            std::vector<size_t> heVertInPrevArr;
            std::vector<size_t> vHeInStartArr;
            std::vector<size_t> heVertOutNextArr;
            std::vector<size_t> heVertOutPrevArr;
            std::vector<size_t> vHeOutStartArr;

            // Element connectivity
            size_t heNext(size_t iHe) const;                 // he.next()
            size_t heTwin(size_t iHe) const;                 // he.twin()
            size_t heSibling(size_t iHe) const;              // he.sibling()
            size_t heNextIncomingNeighbor(size_t iHe) const; // he.nextIncomingNeighbor()
            size_t heNextOutgoingNeighbor(size_t iHe) const; // he.nextOutgoingNeighbor()
            size_t heEdge(size_t iHe) const;                 // he.edge()
            size_t heVertex(size_t iHe) const;               // he.vertex()
            size_t heFace(size_t iHe) const;                 // he.face()
            bool heOrientation(size_t iHe) const;            // he.orientation()
            size_t eHalfedge(size_t iE) const;               // e.halfedge()
            size_t vHalfedge(size_t iV) const;               // v.halfedge()
            size_t fHalfedge(size_t iF) const;               // f.halfedge()

            // Implicit connectivity relationships
            static size_t heTwinImplicit(size_t iHe);   // he.twin()
            static size_t heEdgeImplicit(size_t iHe);   // he.edge()
            static size_t eHalfedgeImplicit(size_t iE); // e.halfedge()

            // Other implicit relationships
            bool heIsInterior(size_t iHe) const;
            bool faceIsBoundaryLoop(size_t iF) const;
            size_t faceIndToBoundaryLoopInd(size_t iF) const;
            size_t boundaryLoopIndToFaceInd(size_t iF) const;

            // Auxilliary arrays which cache other useful information

            // Track element counts (can't rely on rawVertices.size() after deletions have made the list sparse). These are the
            // actual number of valid elements, not the size of the buffer that holds them.
            size_t nHalfedgesCount = 0;
            size_t nInteriorHalfedgesCount = 0;
            size_t nEdgesCount = 0;
            size_t nVerticesCount = 0;
            size_t nFacesCount = 0;
            size_t nBoundaryLoopsCount = 0;

            // == Track the capacity and fill size of our buffers.
            // These give the capacity of the currently allocated buffer.
            // Note that this is _not_ defined to be std::vector::capacity(), it's the largest size such that arr[i] is legal (aka
            // arr.size()).
            size_t nVerticesCapacityCount = 0;
            size_t nHalfedgesCapacityCount = 0; // will always be even if implicit twin
            size_t nEdgesCapacityCount = 0;     // will always be even if implicit twin
            size_t nFacesCapacityCount = 0;     // capacity for faces _and_ boundary loops

            // These give the number of filled elements in the currently allocated buffer. This will also be the maximal index of
            // any element (except the weirdness of boundary loop faces). As elements get marked dead, nVerticesCount decreases
            // but nVertexFillCount does not (etc), so it denotes the end of the region in the buffer where elements have been
            // stored.
            size_t nVerticesFillCount = 0;
            size_t nHalfedgesFillCount = 0; // must always be even if implicit twin
            size_t nEdgesFillCount = 0;
            size_t nFacesFillCount = 0;         // where the real faces stop, and empty/boundary loops begin
            size_t nBoundaryLoopsFillCount = 0; // remember, these fill from the back of the face buffer

            // The mesh is _compressed_ if all of the index spaces are dense. E.g. if thare are |V| vertices, then the vertices
            // are densely indexed from 0 ... |V|-1 (and likewise for the other elements). The mesh can become not-compressed as
            // deletions mark elements with tombstones--this is how we support constant time deletion.
            // Call compress() to re-index and return to usual dense indexing.
            bool isCompressedFlag = true;

            uint64_t modificationTick = 1; // Increment every time the mesh is mutated in any way. Used to track staleness.

            // Used to resize the halfedge mesh. Expands and shifts vectors as necessary.
            // Vertex getNewVertex();
            // Halfedge getNewEdgeTriple(bool onBoundary); // Create an edge and two halfedges.
            // Returns e.halfedge() from the newly created edge
            Halfedge getNewHalfedge(bool isInterior);
            Edge getNewEdge();
            // Face getNewFace();
            // BoundaryLoop getNewBoundaryLoop();
            // void expandFaceStorage(); // helper used in getNewFace() and getNewBoundaryLoop()

            // Detect dead elements
            bool vertexIsDead(size_t iV) const;
            bool halfedgeIsDead(size_t iHe) const;
            bool edgeIsDead(size_t iE) const;
            bool faceIsDead(size_t iF) const;

            // Helper function to assist circulation around vertex
            void initializeHalfedgeNeighbors();
            // Build a flat array for iterating around a vertex, before the mesh structure is complete.
            // For vertex iV, vertexIterationCacheHeIndex holds the
            // indices of the halfedges outgoing/incoming to the vertex, in the range from vertexIterationCacheVertexStart[iV] to
            // vertexIterationCacheVertexStart[iV+1]
            void generateVertexIterationCache(std::vector<size_t> &vertexIterationCacheHeIndex,
                                              std::vector<size_t> &vertexIterationCacheVertexStart, bool incoming,
                                              bool skipDead = true);

            // Elements need direct access in to members to traverse
            friend class Vertex;
            friend class Halfedge;
            friend class Corner;
            friend class Edge;
            friend class Face;
            friend class BoundaryLoop;

            friend struct VertexRangeF;
            friend struct HalfedgeRangeF;
            friend struct HalfedgeInteriorRangeF;
            friend struct HalfedgeExteriorRangeF;
            friend struct CornerRangeF;
            friend struct EdgeRangeF;
            friend struct FaceRangeF;
            friend struct BoundaryLoopRangeF;
        };
    }
}

#include "halfedge_logic_templates.ipp"
#include "halfedge_element_types.ipp"
#include "surface_mesh.ipp"