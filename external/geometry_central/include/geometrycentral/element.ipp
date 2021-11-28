#pragma once

namespace geometrycentral
{
    template <typename T, typename M>
    Element<T, M>::Element() {}

    template <typename T, typename M>
    Element<T, M>::Element(ParentMeshT *mesh_, size_t ind_) : mesh(mesh_), ind(ind_) {}

    template <typename T, typename M>
    inline bool Element<T, M>::operator==(const Element<T, M> &other) const { return ind == other.ind; }
    template <typename T, typename M>
    inline bool Element<T, M>::operator!=(const Element<T, M> &other) const { return !(*this == other); }
    template <typename T, typename M>
    inline bool Element<T, M>::operator>(const Element<T, M> &other) const { return ind > other.ind; }
    template <typename T, typename M>
    inline bool Element<T, M>::operator>=(const Element<T, M> &other) const { return ind >= other.ind; }
    template <typename T, typename M>
    inline bool Element<T, M>::operator<(const Element<T, M> &other) const { return ind < other.ind; }
    template <typename T, typename M>
    inline bool Element<T, M>::operator<=(const Element<T, M> &other) const { return ind <= other.ind; }

    template <typename T, typename M>
    size_t Element<T, M>::getIndex() const { return ind; }

    template <typename T, typename M>
    M *Element<T, M>::getMesh() const { return mesh; }
} // namespace geometrycentral

namespace std
{
    template <typename T, typename M>
    inline ostream &operator<<(ostream &output, const geometrycentral::Element<T, M> &e)
    {
        output << geometrycentral::typeShortName<T>() << "_" << e.getIndex();
        return output;
    }

    template <typename T, typename M>
    inline std::string to_string(const geometrycentral::Element<T, M> &e)
    {
        ostringstream output;
        output << e;
        return output.str();
    }
} // namespace std