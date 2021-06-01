#pragma once

namespace egl
{
    class Line_2
    {
    private:
        double a_, b_, c_;

    public:
        Line_2() {}
        Line_2(double a, double b, double c) : a_(a), b_(b), c_(c) {}

        double a() const { return a_; }
        double b() const { return b_; }
        double c() const { return c_; }
    };

    class Segment_2
    {
    private:
        Pnt2 p0_, p1_;

    public:
        Segment_2() {}
        Segment_2(const Pnt2 &p0, const Pnt2 &p1) : p0_(p0), p1_(p1) {}

        const Pnt2 &source() const { return p0_; }
        const Pnt2 &target() const { return p1_; }
        Vec2 direction() const { return p1_ - p0_; }
        Segment_2 opposite() const { return Segment_2(p1_, p0_); }
    };
    inline std::ostream &operator<<(std::ostream &out, const Segment_2 &e)
    {
        out << "[" << e.source().transpose() << "," << e.target().transpose() << "]";
        return out;
    }

    M_INLINE void line_from_point(const double &px, const double &py,
                                  const double &qx, const double &qy,
                                  double &a, double &b, double &c);

    // whether three points lies in the same line
    // returns true if r lies in the line p->q
    M_INLINE bool collinear(const Pnt2 &p, const Pnt2 &q, const Pnt2 &r);

    // Given an oriented 2D straight line segment 'e', computes the normalized coefficients (a,b,c) of the
    // supporting line.
    // POSTCONDITION: [a,b] is the ledoubleward normal _unit_ (a^2+b^2=1) vector.
    // return true if Line is valid false if overflow happens
    M_INLINE boost::optional<Line_2> compute_normalized_line_ceoffC2(const Segment_2 &e);

    M_INLINE Orientation orientation(double px, double py, double qx, double qy, double rx, double ry);

    M_INLINE Orientation orientation(const Pnt2 &p, const Pnt2 &q, const Pnt2 &r);

    M_INLINE Orientation orientation(double ux, double uy, double vx, double vy);

    M_INLINE Orientation orientation(const Vec2 &u, const Vec2 &v);

    M_INLINE double squared_distance(const Pnt2 &p, const Pnt2 &q);

    M_INLINE void line_project_pointC2(const double &la, const double &lb, const double &lc,
                                       const double &px, const double &py,
                                       double &x, double &y);

    // px, py: point
    // (sx, sy)---->(tx, ty): line
    M_INLINE double squared_distance_from_point_to_lineC2(double const &px, double const &py,
                                                          double const &sx, double const &sy,
                                                          double const &tx, double const &ty);

    // (la, lb): la*x+lb*y+lc=0 the line
    // (px, py): point on the line
    // (a, b, c): perpendicular line a*x+b*y+c=0 passing (px, py) of line (la, lb)
    M_INLINE void perpendicular_through_pointC2(const double &la, const double &lb,
                                                const double &px, const double &py,
                                                double &a, double &b, double &c);
}

#ifdef WITH_INLINE
#include "geom_2.cpp"
#endif