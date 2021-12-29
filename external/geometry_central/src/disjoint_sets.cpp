#include "../include/geometrycentral/disjoint_sets.h"
#include <iostream>
#include <iterator>

namespace geometrycentral
{
    DisjointSets::DisjointSets(size_t n_)
        : n(n_), parent(n_ + 1), rank(n_ + 1)
    {
        for (size_t i = 0; i <= n; ++i)
        {
            rank[i] = 0;
            parent[i] = i; // initially each element make a set
        }
    }

    size_t DisjointSets::find(size_t x)
    {
        if (x != parent[x])
            parent[x] = find(parent[x]);
        return parent[x];
    }

    void DisjointSets::merge(size_t x, size_t y)
    {
        x = find(x);
        y = find(y);

        if (rank[x] > rank[y])
            parent[y] = x;
        else
            parent[x] = y;

        if (rank[x] == rank[y])
            rank[y]++;
    }

    void DisjointSets::print()
    {
        std::copy(parent.begin(), parent.end(), std::ostream_iterator<int>(std::cout));
        std::cout << " ";
        std::copy(rank.begin(), rank.end(), std::ostream_iterator<int>(std::cout));
        std::cout << "\n";
    }

}