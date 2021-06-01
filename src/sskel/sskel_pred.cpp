#include "sskel_pred.h"

namespace egl
{
    M_INLINE Trisegment_collinearity trisegment_collinearity(const Segment_2 &e0,
                                                             const Segment_2 &e1,
                                                             const Segment_2 &e2)
    {
        bool is_01 = are_edges_orderly_collinear(e0, e1);
        bool is_02 = are_edges_orderly_collinear(e0, e2);
        bool is_12 = are_edges_orderly_collinear(e1, e2);

        if (is_01 & !is_02 & !is_12)
        {
            return TRISEGMENT_COLLINEARITY_01;
        }
        else if (is_02 & !is_01 & !is_12)
        {
            return TRISEGMENT_COLLINEARITY_02;
        }
        else if (is_12 & !is_01 & !is_02)
        {
            return TRISEGMENT_COLLINEARITY_12;
        }
        else if (!is_01 & !is_12 & !is_02)
        {
            return TRISEGMENT_COLLINEARITY_NONE;
        }
        else
        {
            return TRISEGMENT_COLLINEARITY_ALL;
        }
    }

    M_INLINE bool are_edges_collinear(const Segment_2 &e0, const Segment_2 &e1)
    {
        return collinear(e0.source(), e0.target(), e1.source()) &
               collinear(e0.source(), e0.target(), e1.target());
    }

    M_INLINE bool are_edges_parallel(const Segment_2 &e0, const Segment_2 &e1)
    {
        Vec2 dir0 = e0.target() - e0.source();
        Vec2 dir1 = e1.target() - e1.source();
        return is_equal(dir0.x() * dir1.y(), dir0.y() * dir1.x());
    }

    M_INLINE bool are_parallel_edges_equally_oriented(const Segment_2 &e0, const Segment_2 &e1)
    {
        return (e0.target() - e0.source()).dot(e1.target() - e1.source()) > 0;
    }

    M_INLINE bool are_edges_orderly_collinear(const Segment_2 &e0, const Segment_2 &e1)
    {
        return are_edges_collinear(e0, e1) & are_parallel_edges_equally_oriented(e0, e1);
    }

    M_INLINE Sign side_of_oriented_line(double a, double b, double c, double x, double y)
    {
        double res = a * x + b * y + c;
        if (is_zero(res))
        {
            return EQUAL;
        }
        else if (res > 0)
        {
            return POSITIVE;
        }
        else
        {
            return NEGATIVE;
        }
    }

    // Returns true if the point aP is on the positive side of the line supporting the edge
    M_INLINE bool is_edge_facing_point(const Pnt2 &p, const Segment_2 &e)
    {
        double a, b, c;
        line_from_point(e.source().x(), e.source().y(),
                        e.target().x(), e.target().y(),
                        a, b, c);
        return a * p.x() + b * p.y() + c > 0;
    }

    M_INLINE bool is_edge_facing_offset_lines_isecC2(Trisegment_2_ptr const &tri, Segment_2 const &aEdge)
    {
        boost::optional<Pnt2> p = construct_offset_lines_isecC2(tri);
        if (p)
            return is_edge_facing_point(*p, aEdge);
        else
            return false;
    }

    M_INLINE bool exist_offset_lines_isec(Trisegment_2_ptr const &tri, double aMaxTime)
    {
        // ToDO: use aMaxTime
        if (tri->collinearity() != TRISEGMENT_COLLINEARITY_ALL)
        {
            boost::optional<double> t = compute_offset_lines_isec_timeC2(tri);
            if (t)
            {
                if (*t > 0)
                    return true;
            }
        }
        else
        {
            printf("All edges are collinear. No intersection exist!\n");
        }

        return false;
    }

    M_INLINE Comparison_result compare_offset_lines_isec_times(const Trisegment_2_ptr &m, const Trisegment_2_ptr &n)
    {
        boost::optional<double> mt = compute_offset_lines_isec_timeC2(m);
        boost::optional<double> nt = compute_offset_lines_isec_timeC2(n);

        assert((mt && nt) && " The compared offset Triseg must have intersection point!");
        if (*mt < *nt)
        {
            return SMALLER;
        }
        else if (is_equal(*mt, *nt))
        {
            return EQUAL;
        }
        else
        {
            return LARGER;
        }
    }

    M_INLINE bool is_point_calculation_clearly_wrong(double const &t, Pnt2 const &p, Trisegment_2_ptr const &aTrisegment)
    {
        bool rR = false;

        if (is_not_zero(t))
        {
            Segment_2 const &e0 = aTrisegment->e0();
            Segment_2 const &e1 = aTrisegment->e1();
            Segment_2 const &e2 = aTrisegment->e2();

            Pnt2 const &e0s = e0.source();
            Pnt2 const &e0t = e0.target();

            Pnt2 const &e1s = e1.source();
            Pnt2 const &e1t = e1.target();

            Pnt2 const &e2s = e2.source();
            Pnt2 const &e2t = e2.target();

            double const very_short(0.1);
            double const very_short_squared = very_short * very_short;

            double l0 = squared_distance(e0s, e0t);
            double l1 = squared_distance(e1s, e1t);
            double l2 = squared_distance(e2s, e2t);

            bool e0_is_not_very_short = l0 > very_short_squared;
            bool e1_is_not_very_short = l1 > very_short_squared;
            bool e2_is_not_very_short = l2 > very_short_squared;

            double d0 = squared_distance_from_point_to_lineC2(p.x(), p.y(), e0s.x(), e0s.y(), e0t.x(), e0t.y());
            double d1 = squared_distance_from_point_to_lineC2(p.x(), p.y(), e1s.x(), e1s.y(), e1t.x(), e1t.y());
            double d2 = squared_distance_from_point_to_lineC2(p.x(), p.y(), e2s.x(), e2s.y(), e2t.x(), e2t.y());

            double tt = t * t;

            bool e0_is_clearly_wrong = e0_is_not_very_short && is_not_equal(d0, tt);
            bool e1_is_clearly_wrong = e1_is_not_very_short && is_not_equal(d1, tt);
            bool e2_is_clearly_wrong = e2_is_not_very_short && is_not_equal(d2, tt);

            rR = e0_is_clearly_wrong || e1_is_clearly_wrong || e2_is_clearly_wrong;
        }

        return rR;
    }

    M_INLINE Oriented_side
    oriented_side_of_event_point_wrt_bisectorC2(Trisegment_2_ptr const &event,
                                                Segment_2 const &e0, Segment_2 const &e1,
                                                Trisegment_2_ptr const &v01_event, // can be null
                                                bool primary_is_0)
    {
        boost::optional<Pnt2> op = construct_offset_lines_isecC2(event);

        boost::optional<Line_2> ol0 = compute_normalized_line_ceoffC2(e0);
        boost::optional<Line_2> ol1 = compute_normalized_line_ceoffC2(e1);

        assert(op && "Instesection point doesn't exist for Triseg Event!\n");
        assert(ol0 && "Line formula doesn't exist for e0!\n");
        assert(ol1 && "Line formula doesn't exist for e1!\n");

        const Pnt2 &p = *op;
        const Line_2 &l0 = *ol0;
        const Line_2 &l1 = *ol1;

        if (are_edges_parallel(e0, e1)) // Degenerate bisector //ToDO
        {
            Pnt2 v01 = e1.source();
            if (v01_event)
            {
                boost::optional<Pnt2> ov01 = construct_offset_lines_isecC2(v01_event);
                assert(ov01 && "No intersection found for v01_event!");
                v01 = *ov01;
            }

            //  cout << "v01=" << v01.transpose() << (v01_event ? " (from skelton node)" : "") << endl;

            // (a,b,c) is a line perpedincular to the primary edge through v01.
            // If e0 and e1 are collinear this line is the actual perpendicular bisector.
            //
            // If e0 and e1 are parallel but not collinear (then neccesarrily facing each other) this line
            // is NOT the bisector, but the serves to determine the side of the point (projected along the primary ege) w.r.t vertex v01.

            double a, b, c;
            perpendicular_through_pointC2(primary_is_0 ? l0.a() : l1.a(), primary_is_0 ? l0.b() : l1.b(), v01.x(), v01.y(), a, b, c);
            return side_of_oriented_line(a, b, c, p.x(), p.y());
            //  cout << "Point is at " << rResult << " side of degenerate bisector through v01 " << v01.transpose() << endl;
        }
        else // Valid (non-degenerate) angular bisector
        {
            double sd_p_l0 = l0.a() * p.x() + l0.b() * p.y() + l0.c();
            double sd_p_l1 = l1.a() * p.x() + l1.b() * p.y() + l1.c();

            if (is_equal(sd_p_l0, sd_p_l1))
            {
                return ON_ORIENTED_BOUNDARY;
            }
            else
            {
                // Reflex bisector?
                if (l0.a() * l1.b() < l1.a() * l0.b()) // on reflex bisector
                {
                    return (sd_p_l0 < sd_p_l1) ? ON_NEGATIVE_SIDE
                                               : ON_POSITIVE_SIDE;
                }
                else // on convex bisector
                {
                    return (sd_p_l0 > sd_p_l1) ? ON_NEGATIVE_SIDE
                                               : ON_POSITIVE_SIDE;
                }
            }
        }
    }
}