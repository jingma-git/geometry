#pragma once

#include <Eigen/Eigen>
#include <deque>
#include <iostream>

// https://cp-algorithms.com/geometry/halfplane-intersection.html
namespace Halfplane_intersect
{
    typedef Eigen::Vector2d Pnt2;
    typedef Eigen::Vector2d Vec2;
    const double eps = 1e-9, inf = 1e9;

    inline double cross(const Vec2 &a, const Vec2 &b)
    {
        return a.x() * b.y() - a.y() * b.x();
    }

    class Halfplane_2
    {
    private:
        Pnt2 src_; // passing Pnt2 of the line
        Vec2 dir_; // direction of the line
        double angle_;

    public:
        Halfplane_2() {}
        Halfplane_2(const Pnt2 &src, const Vec2 &dir, double angle)
            : src_(src), dir_(dir), angle_(angle)
        {
        }

        Halfplane_2(const Pnt2 &src, const Pnt2 &dst)
            : src_(src), dir_(dst - src)
        {
            angle_ = atan2(dir_.y(), dir_.x());
        }

        const Pnt2 &src() const { return src_; }
        const Vec2 &dir() const { return dir_; }

        // check if Pnt2 p is outside of this halfplane
        bool out(const Pnt2 &p) const
        {
            return cross(dir_, p - src_) < -eps;
        }

        // Comparator for sorting.
        // If the angle of both half-planes is equal, the leftmost one should go first.
        bool operator<(const Halfplane_2 &plane) const
        {
            if (fabs(angle_ - plane.angle_) < eps)
                return cross(dir_, plane.src_ - src_) < 0;
            return angle_ < plane.angle_;
        }

        // We use equal comparator for std::unique to easily remove parallel half-planes.
        bool operator==(const Halfplane_2 &e) const
        {
            return fabs(angle_ - e.angle_) < eps;
        }
    };

    inline Pnt2 intersect(const Halfplane_2 &s, const Halfplane_2 &t)
    {
        long double alpha = cross((t.src() - s.src()), t.dir()) / cross(s.dir(), t.dir());
        return s.src() + (s.dir() * alpha);
    }

    inline void hp_intersect(std::vector<Halfplane_2> &H,
                             std::vector<Pnt2> &cell,
                             std::vector<Pnt2> box = {
                                 Pnt2(inf, inf),
                                 Pnt2(-inf, inf),
                                 Pnt2(-inf, -inf),
                                 Pnt2(inf, -inf)})
    {

        for (int i = 0; i < 4; i++)
        { // Add bounding box half-planes.
            Halfplane_2 aux(box[i], box[(i + 1) % 4]);
            H.push_back(aux);
        }

        // Sort and remove duplicates
        std::sort(H.begin(), H.end());
        H.erase(std::unique(H.begin(), H.end()), H.end());

        std::deque<Halfplane_2> dq;
        int len = 0;
        for (int i = 0; i < int(H.size()); i++)
        {

            // Remove from the back of the deque while last half-plane is redundant
            while (len > 1 && H[i].out(intersect(dq[len - 1], dq[len - 2])))
            {
                dq.pop_back();
                --len;
            }

            // Remove from the front of the deque while first half-plane is redundant
            while (len > 1 && H[i].out(intersect(dq[0], dq[1])))
            {
                dq.pop_front();
                --len;
            }

            // Add new half-plane
            dq.push_back(H[i]);
            ++len;
        }

        // Final cleanup: Check half-planes at the front against the back and vice-versa
        while (len > 2 && dq[0].out(intersect(dq[len - 1], dq[len - 2])))
        {
            dq.pop_back();
            --len;
        }

        while (len > 2 && dq[len - 1].out(intersect(dq[0], dq[1])))
        {
            dq.pop_front();
            --len;
        }

        // Report empty intersection if necessary
        if (len < 3)
            return;

        // Reconstruct the convex polygon from the remaining half-planes.
        cell.resize(len);
        for (int i = 0; i + 1 < len; i++)
        {
            cell[i] = intersect(dq[i], dq[i + 1]);
        }
        cell.back() = intersect(dq[len - 1], dq[0]);
    }

    // https://cp-algorithms.com/geometry/halfplane-intersection.html
    // every half-plane allows the region to the left side of its direction vector
    // ToDO: this implementation is wrong!!!
    void hp_voro(const std::vector<Pnt2> &sites, std::vector<std::vector<Pnt2>> &cells, int w, int h)
    {
        std::vector<Pnt2> box = {
            Pnt2(w, h),
            Pnt2(0, h),
            Pnt2(0, 0),
            Pnt2(w, 0)};

        size_t n = sites.size();
        cells.resize(n);
        for (size_t i = 0; i < n; ++i)
        {
            std::vector<Halfplane_2> hps;
            hps.reserve(n - 1);
            for (size_t j = i + 1; j < n; ++j)
            {
                Pnt2 mid = (sites[i] + sites[j]) / 2;
                Vec2 vij = sites[j] - sites[i];
                Vec2 dir(-vij.y(), vij.x());
                hps.push_back(Halfplane_2(mid, dir, atan2(dir.y(), dir.x())));
            }
            hp_intersect(hps, cells[i], box);
            std::cout << i << " cell=" << cells[i].size() << std::endl;
            for (size_t j = 0; j < cells[i].size(); ++j)
            {
                std::cout << "pnt" << j << ": " << cells[i][j].transpose() << std::endl;
            }
            std::cout << std::endl;
        }
    }
}
