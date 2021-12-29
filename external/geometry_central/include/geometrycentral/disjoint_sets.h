#pragma once

#include <vector>

namespace geometrycentral
{
    class DisjointSets
    {
    public:
        DisjointSets(size_t n_);
        void merge(size_t x, size_t y);
        void print();

    private:
        size_t find(size_t x);

        size_t n;
        std::vector<size_t> rank;
        std::vector<size_t> parent;
    };
}