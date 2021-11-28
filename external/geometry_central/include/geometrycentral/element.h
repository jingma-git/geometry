#pragma once
#include "utilities.h"
#include <list>
#include <functional>

namespace geometrycentral
{

    // === Templated helper functions
    // Each element type must provide specializations for all of these functions
    // clang-format off
// Current count of this element in the mesh
template <typename E> size_t nElements(typename E::ParentMeshT* mesh) { return INVALID_IND; }

// Capacity of element type in mesh (containers should be at least this big before next resize)
template <typename E> size_t elementCapacity(typename E::ParentMeshT* mesh) { return INVALID_IND; }

// Canonical index for this element (not always an enumeration)
template <typename E> size_t dataIndexOfElement(typename E::ParentMeshT* mesh, E e) { return INVALID_IND; }

// The set type used to iterate over all elements of this type
template <typename E> struct ElementSetType { typedef std::tuple<> type; }; // nonsense default value

template <typename E> typename ElementSetType<E>::type iterateElements(typename E::ParentMeshT* mesh) { return INVALID_IND; }

template <typename E> std::list<std::function<void(size_t)>>& getExpandCallbackList(typename E::ParentMeshT* mesh);

template <typename E> std::list<std::function<void(const std::vector<size_t>&)>>& getPermuteCallbackList(typename E::ParentMeshT* mesh);

template <typename E> std::string typeShortName() { return "X"; }


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

#include "element.ipp"