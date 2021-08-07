#include "sskel_cons.h"

namespace egl
{

    M_INLINE Trisegment_2_ptr construct_trisegment(Segment_2 const &e0,
                                                   Segment_2 const &e1,
                                                   Segment_2 const &e2)
    {
        Trisegment_collinearity lCollinearity = trisegment_collinearity(e0, e1, e2);
        return Trisegment_2_ptr(new Trisegment_2(e0, e1, e2, lCollinearity));
    }

    M_INLINE boost::optional<Pnt2> construct_offset_lines_isecC2(Trisegment_2_ptr const &tri)
    {
        assert(tri->collinearity() != TRISEGMENT_COLLINEARITY_ALL);

        return tri->collinearity() == TRISEGMENT_COLLINEARITY_NONE ? construct_normal_offset_lines_isecC2(tri)
                                                                   : construct_degenerate_offset_lines_isecC2(tri);
    }

    M_INLINE boost::optional<Pnt2> construct_normal_offset_lines_isecC2(Trisegment_2_ptr const &tri)
    {
        boost::optional<Line_2> l0 = compute_normalized_line_ceoffC2(tri->e0());
        boost::optional<Line_2> l1 = compute_normalized_line_ceoffC2(tri->e1());
        boost::optional<Line_2> l2 = compute_normalized_line_ceoffC2(tri->e2());

        double x = 0.0, y = 0.0;
        bool ok = false;
        if (l0 && l1 && l2)
        {
            double den = l0->a() * l2->b() -
                         l0->a() * l1->b() -
                         l1->a() * l2->b() +
                         l2->a() * l1->b() +
                         l0->b() * l1->a() -
                         l0->b() * l2->a();

            if (!is_zero(den))
            {
                double numX = l0->b() * l2->c() -
                              l0->b() * l1->c() -
                              l1->b() * l2->c() +
                              l2->b() * l1->c() +
                              l1->b() * l0->c() -
                              l2->b() * l0->c();

                double numY = l0->a() * l2->c() -
                              l0->a() * l1->c() -
                              l1->a() * l2->c() +
                              l2->a() * l1->c() +
                              l1->a() * l0->c() -
                              l2->a() * l0->c();

                ok = true;

                x = numX / den;
                y = -numY / den;
            }
        }

        return boost::make_optional(ok, Pnt2(x, y));
    }

    M_INLINE boost::optional<Pnt2> construct_degenerate_offset_lines_isecC2(Trisegment_2_ptr const &tri)
    {

        double x(0.0), y(0.0);

        boost::optional<Line_2> l0 = compute_normalized_line_ceoffC2(tri->collinear_edge());
        boost::optional<Line_2> l2 = compute_normalized_line_ceoffC2(tri->non_collinear_edge());

        boost::optional<Pnt2> q = compute_degenerate_seed_pointC2(tri);
        //ToDO:

        bool ok = false;

        if (l0 && l2 && q)
        {
            double num = std::numeric_limits<double>::infinity();
            double den = std::numeric_limits<double>::infinity();

            double px, py;
            line_project_pointC2(l0->a(), l0->b(), l0->c(), q->x(), q->y(), px, py);

            if (!is_zero(l0->b())) // Non-vertical
            {
                num = (l2->a() * l0->b() - l0->a() * l2->b()) * px + l0->b() * l2->c() - l2->b() * l0->c();
                den = (l0->a() * l0->a() - 1) * l2->b() + (1 - l2->a() * l0->a()) * l0->b();
            }
            else
            {
                num = (l2->a() * l0->b() - l0->a() * l2->b()) * py - l0->a() * l2->c() + l2->a() * l0->c();
                den = l0->a() * l0->b() * l2->b() - l0->b() * l0->b() * l2->a() + l2->a() - l0->a();
            }

            if (!is_zero(den))
            {
                x = px + l0->a() * num / den;
                y = py + l0->b() * num / den;
                ok = !std::isinf(x) && !std::isinf(y);
            }
        }

        return boost::make_optional(ok, Pnt2(x, y));
    }

    M_INLINE boost::optional<double> compute_offset_lines_isec_timeC2(Trisegment_2_ptr const &tri)
    {
        // assert(tri->collinearity() != TRISEGMENT_COLLINEARITY_ALL);
        return tri->collinearity() == TRISEGMENT_COLLINEARITY_NONE ? compute_normal_offset_lines_isec_timeC2(tri)
                                                                   : compute_degenerate_offset_lines_isec_timeC2(tri);
    }

    M_INLINE boost::optional<Pnt2> compute_degenerate_seed_pointC2(Trisegment_2_ptr const &tri)
    {
        return compute_seed_pointC2(tri, tri->degenerate_seed_id());
    }

    M_INLINE boost::optional<Pnt2> compute_seed_pointC2(Trisegment_2_ptr const &tri, typename Trisegment_2::SEED_ID sid)
    {
        boost::optional<Pnt2> p;
        switch (sid)
        {
        case Trisegment_2::LEFT:

            p = tri->child_l() ? construct_offset_lines_isecC2(tri->child_l()) // this can recurse
                               : compute_oriented_midpoint(tri->e0(), tri->e1());
            break;

        case Trisegment_2::RIGHT:

            p = tri->child_r() ? construct_offset_lines_isecC2(tri->child_r()) // this can recurse
                               : compute_oriented_midpoint(tri->e1(), tri->e2());
            break;

        case Trisegment_2::UNKNOWN:

            p = compute_oriented_midpoint(tri->e0(), tri->e2());

            break;
        }

        return p;
    }

    M_INLINE boost::optional<double> compute_normal_offset_lines_isec_timeC2(Trisegment_2_ptr const &tri)
    {
        // DETAILS:
        //
        // An offset line is given by:
        //
        //   a*x(t) + b*y(t) + c - t = 0
        //
        // were 't > 0' being to the left of the line.
        // If 3 such offset lines intersect at the same offset distance, the intersection 't',
        // or 'time', can be computed solving for 't' in the linear system formed by 3 such equations.
        // The result is :
        //
        //  t = a2*b0*c1 - a2*b1*c0 - b2*a0*c1 + b2*a1*c0 + b1*a0*c2 - b0*a1*c2
        //      ---------------------------------------------------------------
        //             -a2*b1 + a2*b0 + b2*a1 - b2*a0 + b1*a0 - b0*a1 ;

        std::cout << "e0=" << tri->e0() << " e1=" << tri->e1() << " e2=" << tri->e2() << std::endl;
        boost::optional<Line_2> l0 = compute_normalized_line_ceoffC2(tri->e0());
        boost::optional<Line_2> l1 = compute_normalized_line_ceoffC2(tri->e1());
        boost::optional<Line_2> l2 = compute_normalized_line_ceoffC2(tri->e2());

        double t = std::numeric_limits<double>::infinity();
        if (l0 && l1 && l2)
        {
            double num = (l2->a() * l0->b() * l1->c()) -
                         (l2->a() * l1->b() * l0->c()) -
                         (l2->b() * l0->a() * l1->c()) +
                         (l2->b() * l1->a() * l0->c()) +
                         (l1->b() * l0->a() * l2->c()) -
                         (l0->b() * l1->a() * l2->c());

            double den = (-l2->a() * l1->b()) +
                         (l2->a() * l0->b()) +
                         (l2->b() * l1->a()) -
                         (l2->b() * l0->a()) +
                         (l1->b() * l0->a()) -
                         (l0->b() * l1->a());

            t = num / den;
        }
        return boost::make_optional(!std::isinf(t) && !std::isnan(t), t);
    }

    M_INLINE boost::optional<double> compute_degenerate_offset_lines_isec_timeC2(Trisegment_2_ptr const &tri)
    {
        // DETAILS:
        //
        // For simplicity, assume e0,e1 are the collinear edges.
        //
        //   (1)
        //   The bisecting line of e0 and e1 is a line perpendicular to e0 (and e1)
        //   which passes through 'q': the degenerate offset vertex (e0*,e1*)
        //   This "degenerate" bisecting line is given by:
        //
        //     B0(t) = p + t*[l0.a,l0.b]
        //
        //   where p is the projection of q along l0 and l0.a,l0.b are the _normalized_ line coefficients for e0 (or e1 which is the same)
        //   Since [a,b] is a _unit_ vector pointing perpendicularly to the left of e0 (and e1);
        //   any point B0(k) is at a distance k from the line supporting e0 and e1.
        //
        //   (2)
        //   The bisecting line of e0 and e2 is given by the following SEL
        //
        //    l0.a*x(t) + l0.b*y(t) + l0.c + t = 0
        //    l2.a*x(t) + l2.b*y(t) + l2.c + t = 0
        //
        //   where (l0.a,l0.b,l0.c) and (l2.a,l2.b,l0.c) are the normalized line coefficientes of e0 and e2 resp.
        //
        //     B1(t)=[x(t),y(t)]
        //
        //   (3)
        //   These two bisecting lines B0(t) and B1(t) intersect (if they do) in a single point 'p' whose distance
        //   to the lines supporting the 3 edges is exactly 't' (since those expressions are precisely parametrized in a distance)
        //   Solving the following vectorial equation:
        //
        //     [x(y),y(t)] = q + t*[l0.a,l0.b]
        //
        //   for t gives the result we want.

        bool ok = false;

        boost::optional<Line_2> l0 = compute_normalized_line_ceoffC2(tri->collinear_edge());
        boost::optional<Line_2> l2 = compute_normalized_line_ceoffC2(tri->non_collinear_edge());

        boost::optional<Pnt2> q = compute_degenerate_seed_pointC2(tri);

        double num(0.0), den(0.0);

        if (l0 && l2 && q)
        {
            double px, py;
            line_project_pointC2(l0->a(), l0->b(), l0->c(), q->x(), q->y(), px, py);

            if (!is_zero(l0->b())) // Non-vertical
            {
                num = (l2->a() * l0->b() - l0->a() * l2->b()) * px + l0->b() * l2->c() - l2->b() * l0->c();
                den = (l0->a() * l0->a() - 1) * l2->b() + (1 - l2->a() * l0->a()) * l0->b();
            }
            else
            {
                num = (l2->a() * l0->b() - l0->a() * l2->b()) * py - l0->a() * l2->c() + l2->a() * l0->c();
                den = l0->a() * l0->b() * l2->b() - l0->b() * l0->b() * l2->a() + l2->a() - l0->a();
            }
        }

        double t = num / den;
        return boost::make_optional(!std::isinf(t) && !std::isnan(t), t);
    }

    M_INLINE Pnt2 compute_oriented_midpoint(Segment_2 const &e0, Segment_2 const &e1)
    {
        double delta01 = squared_distance(e0.target(), e1.source());
        double delta10 = squared_distance(e1.target(), e0.source());

        if (delta01 <= delta10)
            return (e0.target() + e1.source()) * 0.5;
        else
            return (e1.target() + e0.source()) * 0.5;
    }
}