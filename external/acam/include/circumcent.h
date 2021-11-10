#pragma once

namespace egl
{
    template <typename Vector3>
    inline Vector3 circumcent(const Vector3 &a, const Vector3 &b, const Vector3 &c)
    {
        // https://gamedev.stackexchange.com/questions/60630/how-do-i-find-the-circumcenter-of-a-triangle-in-3d
        auto ac = c - a, ab = b - a;
        auto abXac = ab.cross(ac), abXacXab = abXac.cross(ab), acXabXac = ac.cross(abXac);
        return a + (abXacXab * ac.squaredNorm() + acXabXac * ab.squaredNorm()) / (2.0 * abXac.squaredNorm());
    }

};