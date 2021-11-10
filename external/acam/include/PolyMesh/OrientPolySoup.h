#pragma once

// fix orientations when read vertices[], polygons[]

class OrientPolySoup
{
public:
    typedef std::pair<size_t, size_t> Edge; // first: v0, second: v1
    typedef std::map<Edge, size_t> EdgeMap; // key: edge, value: face id

    vector<vector<double>> verts;
    vector<vector<size_t>> faces;
    EdgeMap edges;
    std::set<Edge> singular_edges; // non-manifold edges

    void fill_edge_map(); // find singular edges

    void orient(); // walk through non-manifold edges,
    // if the face's orientation is different from before, inverse-orient
};