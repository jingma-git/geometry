#include "../include/geometrycentral/surface_mesh.h"
#include "../include/geometrycentral/combining_hash_functions.h"

namespace geometrycentral
{
    namespace surface
    {
        SurfaceMesh::SurfaceMesh(bool useImplicitTwin) : useImplicitTwinFlag(useImplicitTwin) {}

        SurfaceMesh::SurfaceMesh(const std::vector<std::vector<size_t>> &polygons) : SurfaceMesh(polygons, {}) {}

        SurfaceMesh::SurfaceMesh(const std::vector<std::vector<size_t>> &polygons,
                                 const std::vector<std::vector<std::tuple<size_t, size_t>>> &twins)
            : useImplicitTwinFlag(false)
        {
            nFacesCount = polygons.size();
            nVerticesCount = 0;
            for (const std::vector<size_t> &poly : polygons)
            {
                GC_SAFETY_ASSERT(poly.size() >= 3, "faces must have degree >= 3");
                for (auto i : poly)
                {
                    nVerticesCount = std::max(nVerticesCount, i);
                }
            }
            nVerticesCount++; // 0-based means count is max+1

            // Pre-allocate face and vertex arrays
            vHalfedgeArr = std::vector<size_t>(nVerticesCount, INVALID_IND);
            fHalfedgeArr = std::vector<size_t>(nFacesCount, INVALID_IND);
            nVerticesCapacityCount = nVerticesCount;
            nVerticesFillCount = nVerticesCount;
            nFacesCapacityCount = nFacesCount;
            nFacesFillCount = nFacesCount;

            // Sanity check to detect unreferenced vertices
#ifndef NGC_SAFETY_CHECKS
            std::vector<char> vertUsed(nVerticesCount, false);
#endif
            // === Walk the faces, creating halfedges. For now, don't hook up any twin or edge pointers.
            for (size_t iFace = 0; iFace < nFacesCount; iFace++)
            {
                const std::vector<size_t> &poly = polygons[iFace];

                // Walk around the face
                size_t faceDegree = poly.size();
                size_t prevHeInd = INVALID_IND;
                size_t firstHeInd = INVALID_IND;
                for (size_t iFaceHe = 0; iFaceHe < faceDegree; iFaceHe++)
                {
                    size_t indTail = poly[iFaceHe];
                    size_t indTip = poly[(iFaceHe + 1) % faceDegree];

#ifndef NGC_SAFETY_CHECKS
                    vertUsed[indTail] = true;
#endif

                    // Get an index for this halfedge
                    std::tuple<size_t, size_t> heKey{indTail, indTip};
                    std::tuple<size_t, size_t> heTwinKey{indTip, indTail};

                    // Create a halfedge
                    size_t halfedgeInd = getNewHalfedge(true).getIndex();
                    // Fill arrays with nknown values and placeholders
                    heNextArr[halfedgeInd] = INVALID_IND;
                    heVertexArr[halfedgeInd] = indTail;
                    heFaceArr[halfedgeInd] = iFace;

                    // Hook up a bunch of pointers
                    vHalfedgeArr[indTail] = halfedgeInd;
                    if (iFaceHe == 0)
                    {
                        fHalfedgeArr[iFace] = halfedgeInd;
                        firstHeInd = halfedgeInd;
                    }
                    else
                    {
                        heNextArr[prevHeInd] = halfedgeInd;
                    }

                    prevHeInd = halfedgeInd;
                }
                heNextArr[prevHeInd] = firstHeInd; // hook up the first next() pointer, which we missed in the loop above
            }

#ifndef NGC_SAFETY_CHECKS
            // Look for any vertices which were unreferenced
            for (size_t iV = 0; iV < nVerticesCount; iV++)
            {
                GC_SAFETY_ASSERT(vertUsed[iV], "unreferenced vertex " + std::to_string(iV));
            }
#endif

            // Create edges and hook up twins
            if (twins.empty())
            {
                // If we've already seen this vertex pair at least once,
                // this holds index of the most recent halfedge encountered
                // incident on that edge.
                std::unordered_map<std::tuple<size_t, size_t>, size_t> edgeHistory;
                size_t iHe = 0;
                for (size_t iFace = 0; iFace < nFacesCount; iFace++)
                {
                    const std::vector<size_t> &poly = polygons[iFace];

                    // Walk around this face
                    size_t faceDegree = poly.size();
                    for (size_t iFaceHe = 0; iFaceHe < faceDegree; iFaceHe++)
                    {
                        size_t indTail = poly[iFaceHe];
                        size_t indTip = poly[(iFaceHe + 1) % faceDegree];

                        // Get a key for this edge
                        std::tuple<size_t, size_t> eKey{std::min(indTail, indTip), std::max(indTail, indTip)};

                        if (edgeHistory.find(eKey) == edgeHistory.end())
                        {
                            // This is the first time we've ever seen this edge, create a new edge object
                            size_t newEdgeInd = getNewEdge().getIndex();
                            heEdgeArr[iHe] = newEdgeInd;
                            heSiblingArr[iHe] = INVALID_IND;
                            heOrientArr[iHe] = true;
                            eHalfedgeArr[newEdgeInd] = iHe;
                            edgeHistory[eKey] = iHe;
                        }
                        else
                        {
                            // We've see this edge, connect it to its slibling
                            size_t iPrevHe = edgeHistory[eKey];
                            size_t iE = heEdgeArr[iPrevHe];
                            heEdgeArr[iHe] = iE;
                            heSiblingArr[iHe] = iPrevHe;
                            heOrientArr[iHe] = (heVertexArr[iHe] == heVertexArr[eHalfedgeArr[iE]]);
                            edgeHistory[eKey] = iHe;
                        }

                        iHe++;
                    }
                }

                // Complete the sibling cycle
                for (auto &entry : edgeHistory)
                {
                    size_t lastHe = entry.second;
                    // The edge that nevers connect to a sibling is a boundary edge
                    if (heSiblingArr[lastHe] == INVALID_IND)
                    {
                        heSiblingArr[lastHe] = lastHe;
                        continue;
                    }

                    size_t curHe = lastHe;
                    while (heSiblingArr[curHe] != INVALID_IND) // find the first he in sibling cycle
                    {
                        curHe = heSiblingArr[curHe];
                    }
                    heSiblingArr[curHe] = lastHe; // connect last to first
                }
            }
            else
            {
                // DisjointSets djSet
                throw std::runtime_error("not implemented");
            }

            // Build circulators around vertex:
            // the lowest-level operations which populate heVertIn(Out)Next(prev)Arr, vHeIn(Out)StartArr
            initializeHalfedgeNeighbors();
        }

        SurfaceMesh::~SurfaceMesh()
        {
            for (auto &f : meshDeleteCallbackList)
            {
                f();
            }
        }

        Halfedge SurfaceMesh::getNewHalfedge(bool isInterior)
        {
            if (usesImplicitTwin())
            {
                throw std::logic_error("cannot construct a single new halfedge with implicit twin convention");
            }

            if (nHalfedgesFillCount >= nHalfedgesFillCount)
            {
                size_t newHalfedgeCapacity = std::max(nHalfedgesCapacityCount * 2, (size_t)1);

                heNextArr.resize(newHalfedgeCapacity);
                heVertexArr.resize(newHalfedgeCapacity);
                heFaceArr.resize(newHalfedgeCapacity);

                if (!usesImplicitTwin())
                { // must enter this case, see test above
                    heSiblingArr.resize(newHalfedgeCapacity);
                    heEdgeArr.resize(newHalfedgeCapacity);
                    heOrientArr.resize(newHalfedgeCapacity);
                    heVertInNextArr.resize(newHalfedgeCapacity);
                    heVertInPrevArr.resize(newHalfedgeCapacity);
                    heVertOutNextArr.resize(newHalfedgeCapacity);
                    heVertOutPrevArr.resize(newHalfedgeCapacity);
                }

                nHalfedgesCapacityCount = newHalfedgeCapacity;
                // Invoke relevant callback functions
                for (auto &f : halfedgeExpandCallbackList)
                {
                    f(newHalfedgeCapacity);
                }
            }

            nHalfedgesFillCount++;
            nHalfedgesCount++;
            if (isInterior)
            {
                nInteriorHalfedgesCount++;
            }

            modificationTick++;
            isCompressedFlag = false;

            return Halfedge(this, nHalfedgesFillCount - 1);
        }

        Edge SurfaceMesh::getNewEdge()
        {
            if (usesImplicitTwin())
            {
                throw std::logic_error("cannot construct a single new edge with implicit twin convention");
            }

            if (nEdgesFillCount >= nEdgesCapacityCount)
            {
                size_t newEdgeCapacity = std::max(nEdgesCapacityCount * 2, (size_t)1);

                nEdgesCapacityCount = newEdgeCapacity;
                if (!usesImplicitTwin())
                { // must enter this case, see test above
                    eHalfedgeArr.resize(newEdgeCapacity);
                }
                // Invoke relevant callback functions
                for (auto &f : edgeExpandCallbackList)
                {
                    f(newEdgeCapacity);
                }
            }

            nEdgesFillCount++;
            nEdgesCount++;

            modificationTick++;
            isCompressedFlag = false;
            return Edge(this, nEdgesFillCount - 1);
        }

        void SurfaceMesh::initializeHalfedgeNeighbors()
        {
            std::vector<size_t> vertexIterationCacheVertexStartIn;
            std::vector<size_t> vertexIterationCacheHeIndexIn;
            generateVertexIterationCache(vertexIterationCacheHeIndexIn, vertexIterationCacheVertexStartIn, true, true);

            std::vector<size_t> vertexIterationCacheVertexStartOut;
            std::vector<size_t> vertexIterationCacheHeIndexOut;
            generateVertexIterationCache(vertexIterationCacheHeIndexOut, vertexIterationCacheVertexStartOut, false, true);

            heVertInNextArr.resize(nHalfedgesCapacityCount);
            heVertInPrevArr.resize(nHalfedgesCapacityCount);
            vHeInStartArr.resize(nVerticesCapacityCount);
            heVertOutNextArr.resize(nHalfedgesCapacityCount);
            heVertOutPrevArr.resize(nHalfedgesCapacityCount);
            vHeOutStartArr.resize(nVerticesCapacityCount);

            for (Vertex v : vertices())
            {
                size_t iV = v.getIndex();
                // vertex incoming halfedges
                {
                    size_t rangeStartIn = vertexIterationCacheVertexStartIn[iV];
                    size_t rangeEndIn = vertexIterationCacheVertexStartIn[iV + 1];
                    vHeInStartArr[iV] = vertexIterationCacheHeIndexIn[rangeStartIn];
                    for (size_t i = rangeStartIn; i < rangeEndIn; ++i)
                    {
                        size_t iHe = vertexIterationCacheHeIndexIn[i];
                        size_t iNextHe = vertexIterationCacheHeIndexIn[(i + 1 - rangeStartIn) % (rangeEndIn - rangeStartIn) + rangeStartIn];
                        heVertInNextArr[iHe] = iNextHe;
                        heVertInPrevArr[iNextHe] = iHe;
                    }
                }

                // vertex outcoming halfedges
                {
                    size_t rangeStartOut = vertexIterationCacheVertexStartOut[iV];
                    size_t rangeEndOut = vertexIterationCacheVertexStartOut[iV + 1];
                    vHeOutStartArr[iV] = vertexIterationCacheHeIndexOut[rangeStartOut];
                    for (size_t i = rangeStartOut; i < rangeEndOut; ++i)
                    {
                        size_t iHe = vertexIterationCacheHeIndexOut[i];
                        size_t iNextHe = vertexIterationCacheHeIndexOut[(i + 1 - rangeStartOut) % (rangeEndOut - rangeStartOut) + rangeStartOut];
                        heVertOutNextArr[iHe] = iNextHe;
                        heVertOutPrevArr[iNextHe] = iHe;
                    }
                }
            }
        }

        void SurfaceMesh::generateVertexIterationCache(std::vector<size_t> &vertexIterationCacheHeIndex,
                                                       std::vector<size_t> &vertexIterationCacheVertexStart, bool incoming,
                                                       bool skipDead)
        {
            std::vector<size_t> vDegree(nVerticesFillCount, 0);
            for (size_t iHe = 0; iHe < nHalfedgesFillCount; ++iHe)
            {
                if (skipDead && halfedgeIsDead(iHe))
                    continue;

                size_t iV = incoming ? heVertex(heNext(iHe)) : heVertex(iHe);
                vDegree[iV]++;
            }

            // Build up vertex circulation index: outer iterator, sperate vertex from vertex
            vertexIterationCacheVertexStart.resize(nHalfedgesFillCount + 1, 0);
            size_t heSum = 0;
            for (size_t iV = 0; iV < nVerticesFillCount; ++iV)
            {
                vertexIterationCacheVertexStart[iV] = heSum;
                heSum += vDegree[iV];
            }
            vertexIterationCacheVertexStart[nVerticesFillCount] = heSum;

            // Build inner circulator around each vertex
            std::vector<size_t> vertexIterationCache = vertexIterationCacheVertexStart;
            vertexIterationCacheHeIndex.resize(nHalfedgesFillCount);
            for (size_t iHe = 0; iHe < nHalfedgesFillCount; ++iHe)
            {
                if (skipDead && halfedgeIsDead(iHe))
                    continue;

                size_t iV = incoming ? heVertex(heNext(iHe)) : heVertex(iHe);
                size_t outInd = vertexIterationCache[iV];
                vertexIterationCacheHeIndex[outInd] = iHe; // put halfeddge iHe on position outInd
                vertexIterationCache[iV]++;
            }
        }
    }
}