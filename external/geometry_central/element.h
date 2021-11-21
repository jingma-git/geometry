#pragma once

namespace geometrycentral
{
    // CRTP pattern: compile-time polymorphism
    template <typename T, typename M>
    class Element
    {
    public:
        using ParentMeshT = M;
        Element();
        Element(ParentMeshT *mesh, size_t ind);

        inline bool operator==(const Element<T, M> &other) const;
        inline bool operator!=(const Element<T, M> &other) const;
        inline bool operator>(const Element<T, M> &other) const;
        inline bool operator>=(const Element<T, M> &other) const;
        inline bool operator<(const Element<T, M> &other) const;
        inline bool operator<=(const Element<T, M> &other) const;

        size_t getIndex() const;
        ParentMeshT *getMesh() const;
        bool isDead() const;

    protected:
        ParentMeshT *mesh = nullptr;
        size_t ind = INVALID_IND;
        friend struct std::hash<Element<T, M>>;
    };
} // namespace geometrycentral

namespace std
{
    template <typename T, typename M>
    std::ostream &operator<<(std::ostream &output, const geometrycentral::Element<T, M> &e);
    template <typename T, typename M>
    std::string to_string(const geometrycentral::Element<T, M> &e);
} // namespace std

#include "element.cpp"