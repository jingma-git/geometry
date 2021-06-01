#include "geom_2.h"

#include "dbl.h" // ToBeRemove

namespace egl
{
    M_INLINE void line_from_point(const double &px, const double &py,
                                  const double &qx, const double &qy,
                                  double &a, double &b, double &c)
    {
        // The horizontal and vertical line get a special treatment
        // in order to make the intersection code robust for doubles
        if (py == qy)
        {
            a = 0;
            if (qx > px)
            {
                b = 1;
                c = -py;
            }
            else if (qx == px)
            {
                b = 0;
                c = 0;
            }
            else
            {
                b = -1;
                c = py;
            }
        }
        else if (px == qx)
        {
            b = 0;
            if (qy > py)
            {
                a = -1;
                c = px;
            }
            else if (qy == py)
            {
                a = 0;
                c = 0;
            }
            else
            {
                a = 1;
                c = -px;
            }
        }
        else
        {
            a = py - qy;
            b = qx - px;
            c = -px * a - py * b;
        }
    }

    M_INLINE bool collinear(const Pnt2 &p, const Pnt2 &q, const Pnt2 &r)
    {
        return is_equal((q.x() - p.y()) * (r.y() - p.y()), (r.x() - p.x()) * (q.y() - p.y()));
    }

    M_INLINE boost::optional<Line_2> compute_normalized_line_ceoffC2(const Segment_2 &e)
    {
        bool flag = true;
        double a = 0., b = 0., c = 0.;
        if (e.source().y() == e.target().y())
        {
            a = 0;
            if (e.target().x() > e.source().x())
            {
                b = 1;
                c = -e.source().y();
            }
            else if (e.target().x() == e.source().x())
            {
                b = 0;
                c = 0;
            }
            else
            {
                b = -1;
                c = e.source().y();
            }
        }
        else if (e.target().x() == e.source().y())
        {
            b = 0;
            if (e.target().y() > e.source().y())
            {
                a = -1;
                c = e.source().x();
            }
            else if (e.target().y() == e.source().y())
            {
                a = 0;
                c = 0;
            }
            else
            {
                a = 1;
                c = -e.source().x();
            }
        }
        else
        {
            double sa = e.source().y() - e.target().y();
            double sb = e.target().x() - e.source().x();

            double l2 = (sa * sa) + (sb * sb);
            if (l2 < DBL_EPS)
            {
                flag = false;
            }
            else
            {
                double l = std::sqrt(l2);
                a = sa / l;
                b = sb / l;
                c = -e.source().x() * a - e.source().y() * b;
            }
        }
        return boost::make_optional(flag, Line_2(a, b, c));
    }

    M_INLINE Orientation orientation(double px, double py, double qx, double qy, double rx, double ry)
    {
        double pqx = qx - px;
        double pqy = qy - py;
        double prx = rx - px;
        double pry = ry - py;

        double det = pqx * pry - pqy * prx;

        // Then semi-static filter.
        double maxx = std::abs(pqx);
        double maxy = std::abs(pqy);

        double aprx = std::abs(prx);
        double apry = std::abs(pry);

        if (maxx < aprx)
            maxx = aprx;

        if (maxy < apry)
            maxy = apry;

        // Sort them
        if (maxx > maxy)
            std::swap(maxx, maxy);

        // Protect against underflow in the computation of eps.
        if (maxx < 1e-146) /* sqrt(min_double/eps) */
        {
            if (maxx == 0)
                return ZERO;
        }
        // Protect against overflow in the computation of det.
        else if (maxy < 1e153) /* sqrt(max_double [hadamard]/2) */
        {
            double eps = 8.8872057372592798e-16 * maxx * maxy;
            if (det > eps)
                return POSITIVE;
            if (det < -eps)
                return NEGATIVE;
        }
    }

    M_INLINE Orientation orientation(const Pnt2 &p, const Pnt2 &q, const Pnt2 &r)
    {
        return orientation(p.x(), p.y(), q.x(), q.y(), r.x(), r.y());
    }

    M_INLINE Orientation orientation(double ux, double uy, double vx, double vy)
    {
        double uxvy = ux * vy;
        double uyvx = uy * vx;
        if (is_equal(uxvy, uyvx))
        {
            return ZERO;
        }
        else if (uxvy > uyvx)
        {
            return POSITIVE;
        }
        else
        {
            return NEGATIVE;
        }
    }

    M_INLINE Orientation orientation(const Vec2 &u, const Vec2 &v)
    {
    }

    M_INLINE double squared_distance(const Pnt2 &p, const Pnt2 &q)
    {
        return (p - q).squaredNorm();
    }

    M_INLINE void line_project_pointC2(const double &la, const double &lb, const double &lc,
                                       const double &px, const double &py,
                                       double &x, double &y)
    {

        if (is_zero(la)) // horizontal line
        {
            x = px;
            y = -lc / lb;
        }
        else if (is_zero(lb)) // vertical line
        {
            x = -lc / la;
            y = py;
        }
        else
        {
            double ab = la / lb, ba = lb / la, ca = lc / la;
            y = (-px + ab * py - ca) / (ba + ab);
            x = -ba * y - ca;
        }
    }

    M_INLINE double squared_distance_from_point_to_lineC2(double const &px, double const &py,
                                                          double const &sx, double const &sy,
                                                          double const &tx, double const &ty)
    {
        double ldx = tx - sx;
        double ldy = ty - sy;
        double rdx = sx - px;
        double rdy = sy - py;

        double n = (ldx * rdy - rdx * ldy) * (ldx * rdy - rdx * ldy);
        double d = ldx * ldx + ldy * ldy;
        assert(!is_zero(d) && "line is degenerate to a single point!");
        return n / d;
    }

    M_INLINE void perpendicular_through_pointC2(const double &la, const double &lb,
                                                const double &px, const double &py,
                                                double &a, double &b, double &c)
    {
        a = -lb;
        b = la;
        c = lb * px - la * py;
    }
}