#pragma once

namespace egl
{
    // Constructs a Trisegment_2 which stores 3 edges (segments) such that
    // if two of them are collinear, they are put first, as e0, e1.
    // Stores also the number of collinear edges. which should be 0 or 2.
    //
    // If the collinearity test is indeterminate for any pair of edges the
    // resulting sorted trisegment is itself indeterminate
    // (encoded as a collinear count of -1)
    M_INLINE Trisegment_collinearity trisegment_collinearity(const Segment_2 &e0,
                                                             const Segment_2 &e1,
                                                             const Segment_2 &e2);

    M_INLINE Trisegment_2_ptr construct_trisegment(Segment_2 const &e0,
                                                   Segment_2 const &e1,
                                                   Segment_2 const &e2);

    M_INLINE boost::optional<Pnt2> construct_offset_lines_isecC2(Trisegment_2_ptr const &tri);

    // Given 3 oriented line segments e0, e1 and e2
    // such that their offsets at a certian distance intersect in a single point,
    // returns the coordinates (x,y) of such a point.
    //
    // PRECONDITIONS:
    // None of e0, e1 and e2 are collinear (but two of them can be parallel)
    // The line coefficients must be normalized: a^2+b^2==1 and (a,b) being the ledoubleward normal vector
    // The offsets at a certain distance do intersect in a single point.
    //
    // POSTCONDITION: In case of overflow an empty optional is returned.
    M_INLINE boost::optional<Pnt2> construct_normal_offset_lines_isecC2(Trisegment_2_ptr const &tri);

    // Given 3 oriented line segments e0, e1 and e2
    // such that their offsets at a certian distance intersect in a single point,
    // returns the coordinates (x,y) of such a point.
    // two and only two of the edges are collinear, not neccesarily consecutive but with the same orientaton
    //
    // PRECONDITIONS:
    // The line coefficients must be normalized: a^2+b^2==1 and (a,b) being the ledoubleward normal vector
    // The offsets at a certain distance do intersect in a single point.
    //
    // POSTCONDITION: In case of overflow an empty optional is returned.
    M_INLINE boost::optional<Pnt2> construct_degenerate_offset_lines_isecC2(Trisegment_2_ptr const &tri);

    M_INLINE boost::optional<double> compute_offset_lines_isec_timeC2(Trisegment_2_ptr const &tri);

    // Given the trisegment tree for an event which is known to have a normal collinearity returns the seed point
    // of the degenerate seed.
    // A normal collinearity occurs when e0,e1 or e1,e2 are collinear.
    M_INLINE boost::optional<Pnt2> compute_degenerate_seed_pointC2(Trisegment_2_ptr const &tri);

    // Given 3 oriented straight line segments: e0, e1, e2 and the corresponding offseted segments: e0*, e1* and e2*,
    // returns the point of the ledouble or right seed (offset vertex) (e0*,e1*) or (e1*,e2*)
    //
    // If the current event (defined by e0,e1,e2) is a propagated event, that is, it follows from a previous event,
    // the seeds are skeleten nodes and are given by non-null trisegments.
    // If the current event is an initial event the seeds are contour vertices and are given by null trisegmets.
    //
    // If a seed is a skeleton node, its point has to be computed from the trisegment that defines it.
    // That trisegment is exactly the trisegment tree that defined the previous event which produced the skeleton node
    // (so the trisegment tree is basically a lazy representation of the seed point).
    //
    // If a seed is a contour vertex, its point is then simply the target endoint of e0 or e1 (for the ledouble/right seed).
    //
    // This method returns the specified seed point (ledouble or right)
    //
    // NOTE: Split events involve 3 edges but only one seed, the ledouble (that is, only e0*,e1* is connected before the event).
    // The trisegment tree for a split event has always a null right child even if the event is not an initial event
    // (in which case its ledouble child won't be null).
    // If you ask for the right child point for a trisegment tree corresponding to a split event you will just get e1.target()
    // which is nonsensical for a non initial split event.
    //
    // NOTE: There is an abnormal collinearity case which ocurrs when e0 and e2 are collinear.
    // In this case, these lines do not correspond to an offset vertex (because e0* and e2* are never consecutive before the event),
    // so the degenerate seed is neither the ledouble or the right seed. In this case, the SEED ID for the degenerate pseudo seed is UNKOWN.
    // If you request the point of such degenerate pseudo seed the oriented midpoint bettwen e0 and e2 is returned.
    M_INLINE boost::optional<Pnt2> compute_seed_pointC2(Trisegment_2_ptr const &tri, typename Trisegment_2::SEED_ID sid);

    // Given 3 oriented straight line segments: e0, e1, e2
    // returns the OFFSET DISTANCE (n/d) at which the offsetted lines
    // intersect at a single point, IFF such intersection exist.
    // If the lines intersect to the ledouble, the returned distance is positive.
    // If the lines intersect to the right, the returned distance is negative.
    // If the lines do not intersect, for example, for collinear edges, or parallel edges but with the same orientation,
    // returns 0 (the actual distance is undefined in this case, but 0 is a usefull return)
    // PRECONDITION: None of e0, e1 and e2 are collinear (but two of them can be parallel)
    // NOTE: The segments (e0,e1,e2) are stored in the argument as the trisegment st.event()
    M_INLINE boost::optional<double> compute_normal_offset_lines_isec_timeC2(Trisegment_2_ptr const &tri);

    // Given 3 oriented straight line segments: e0, e1, e2
    // such that two and only two of these edges are collinear, not neccesarily consecutive but with the same orientaton;
    // returns the OFFSET DISTANCE (n/d) at which a line perpendicular to the collinear edge passing through
    // the degenerate seed point intersects the offset line of the non collinear edge
    M_INLINE boost::optional<double> compute_degenerate_offset_lines_isec_timeC2(Trisegment_2_ptr const &tri);

    // Given two oriented straight line segments e0 and e1 such that e-next follows e-prev, returns
    // the coordinates of the midpoint of the segment between e-prev and e-next.
    // NOTE: the edges can be oriented e0->e1 or e1->e0
    M_INLINE Pnt2 compute_oriented_midpoint(Segment_2 const &e0, Segment_2 const &e1);

}

#ifdef WITH_INLINE
#include "sskel_cons.cpp"
#endif