#pragma once

#include "macros.h"
#include <Eigen/Eigen>

class Ray
{
private:
    Eigen::Vector3d src_;
    Eigen::Vector3d dir_;

public:
    Ray(const Eigen::Vector3d &src, const Eigen::Vector3d &dir) : src_(src), dir_(dir) {}
    const Eigen::Vector3d &source() const { return src_; }
    const Eigen::Vector3d &direction() const { return dir_; }
    Eigen::Vector3d point(double dis) const { return src_ + dis * dir_; }
};

class Line
{
private:
    // a*x + b*y + c = 0
    double a_, b_, c_;

public:
    Line(double a, double b, double c) : a_(a), b_(b), c_(c) {}
    Line(const Ray &ray)
    {
        a_ = ray.direction()[0];
        b_ = ray.direction()[1];
        c_ = -(a_ * ray.source().x() + b_ * ray.source().y());
    }

    double a() const { return a_; }
    double b() const { return b_; }
    double c() const { return c_; }
};

// https://stackoverflow.com/questions/20677795/how-do-i-compute-the-intersection-point-of-two-lines
M_INLINE bool intersect(const Line &l1, const Line &l2, Eigen::Vector3d &pnt);

#ifdef WITH_INLINE
#include "geom_entity.cpp"
#endif