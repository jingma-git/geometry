#pragma once

namespace geometrycentral
{
    namespace surface
    {
        class SurfaceMesh;
        class Vertex;
        class Corner; // parameterization
        class Edge;
        class Face;
        class BoundaryLoop;

        class Vertex : public Element<Vertex, SurfaceMesh>
        {
        };

    } // namespace surface
} // namespace geometrycentral