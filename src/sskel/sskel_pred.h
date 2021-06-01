#pragma once

// predicates

namespace egl
{

    // Returns true if segments e0,e1 share the same supporting line
    M_INLINE bool are_edges_collinear(const Segment_2 &e0, const Segment_2 &e1);

    // Returns true if the supporting lines for segments e0,e1 are parallel (or the same)
    M_INLINE bool are_edges_parallel(const Segment_2 &e0, const Segment_2 &e1);

    // Returns true if the parallel segments are equally oriented.
    // Precondition: the segments must be parallel.
    M_INLINE bool are_parallel_edges_equally_oriented(const Segment_2 &e0, const Segment_2 &e1);

    // Returns true IFF segments e0,e1 share the same supporting line but do not overlap except at the vetices, and have the same orientation.
    // NOTE: If e1 goes back over e0 (a degenerate antenna or alley) this returns false.
    M_INLINE bool are_edges_orderly_collinear(const Segment_2 &e0, const Segment_2 &e1);

    M_INLINE Sign side_of_oriented_line(double a, double b, double c, double x, double y);

    // Returns true if the point aP is on the positive side of the line supporting the edge
    M_INLINE bool is_edge_facing_point(const Pnt2 &aP, const Segment_2 &aEdge);

    // Given a triple of oriented straight line segments: (e0,e1,e2) such that their offsets
    // at some distance intersects in a point (x,y), returns true if (x,y) is on the positive side of the line supporting aEdge
    M_INLINE bool is_edge_facing_offset_lines_isecC2(Trisegment_2_ptr const &tri, Segment_2 const &aEdge);

    // Given 3 oriented straight line segments: e0, e1, e2
    // returns true if there exist some positive offset distance 't' for which the
    // ledoubleward-offsets of their supporting lines intersect at a single point.
    //
    // NOTE: This function can handle the case of collinear and/or parallel segments.
    //
    // If two segments are collinear but equally oriented (that is, they share a degenerate vertex) the event exists and
    // is well defined, In that case, the degenerate vertex can be even a contour vertex or a skeleton node. If it is a skeleton
    // node, it is properly defined by the trisegment tree that corresponds to the node.
    // A trisegment tree stores not only the "current event" trisegment but also the trisegments for the ledouble/right seed vertices,
    // recursivey in case the seed vertices are skeleton nodes as well.
    // Those seeds are used to determine the actual position of the degenerate vertex in case of collinear edges (since that point is
    // not given by the collinear edges alone)
    // split event? when three edges insects at same point
    M_INLINE bool exist_offset_lines_isec(Trisegment_2_ptr const &tri, double aMaxTime = std::numeric_limits<double>::infinity());

    // Given 2 triples of oriented straight line segments: (m0,m1,m2) and (n0,n1,n2), such that
    // for each triple there exists distances 'mt' and 'nt' for which the offsets lines (at mt and nt resp.),
    // (m0',m1',m2') and (n0',n1',n2') intersect each in a single point; returns the relative order of mt w.r.t nt.
    // That is, indicates which offset triple intersects first (closer to the source lines)
    // PRECONDITION: There exist distances mt and nt for which each offset triple intersect at a single point.
    M_INLINE Comparison_result compare_offset_lines_isec_times(const Trisegment_2_ptr &m, const Trisegment_2_ptr &n);

    M_INLINE bool is_point_calculation_clearly_wrong(double const &t, Pnt2 const &p, Trisegment_2_ptr const &aTrisegment);

    // Given an event trisegment and two oriented straight line segments e0 and e1, returns the oriented side of the event point
    // w.r.t the (positive) bisector [e0,e1].
    //
    // The (positive) bisector [e0,e1] is a ray starting at the vertex (e0,e1) (called "v01")
    //
    // If e0,e1 are consecutive in the input polygon, v01 = e0.target() = e1.source().
    // If they are not consecutive in the input polygon they must had become consecutive at some known previous event. In this
    // case, the point of the "v01_event" trisegment intersection determines v01 which is to the position of the very first
    // vertex shared between e0,e1 (at the time they first become consecutive).
    //
    // A point *exactly on* the bisector is an offset vertex (e0*,e1*) (that is, belongs to both e0* and e1*).
    // A point to the positive side of the bisector belongs to e0* but not e1*
    // A point to the negative side of the bisector belongs to e1* but not e0*
    //
    // If e0,e1 intersect in a single point the bisector is an angular bisector.
    //
    // One of e0 or e1 is considered the primary edge.
    //
    // If e0,e1 are parallel a line perpendicular to the primary edge passing through "v01" is used "as bisector".
    //
    // If e0,e1 are collinear then this perpendicular line is a perpendicular bisector of the two segments.
    //
    // If e0,e1 are parallel but not collinear then the actual bisector is an equidistant line parallel to e0 and e1.
    // e0* and e1* overlap and are known to be connected sharing a known vertex v01, which is somewhere along the parallel
    // line which is the bisector of e0 and e1.
    // Given a line perpendicular to e0 through v01, a point to its positive side belongs to e0* while a point to its negative side does not.
    // Given a line perpendicular to e1 through v01, a point to its negative side belongs to e1* while a point to its positive side does not.
    //
    // This predicate is used to determine the validity of a split or edge event.
    //
    // A split event is the coallision of a reflex wavefront and some opposite offset egde. Unless the three segments
    // don't actually collide (there is no event), the split point is along the supporting line of the offset edge.
    // Testing its validity amounts to determining if the split point is inside the closed offset segment instead of
    // the two open rays before and adoubleer the offset segment endpoints.
    // The offset edge is bounded by its previous and next adjacent edges at the time of the event. Thus, the bisectors
    // of this edge and its previous/next adjacent edges (at the time of the event) detemine the offset vertices that
    // bound the opposite edge.
    // If the opposite edge is 'e' and its previous/next edges are "preve"/"nexte" then the split point is inside the offset
    // egde if it is NOT to the positive side of [preve,e] *and* NOT to the negative side o [e,nexte].
    // (so this predicate answer half the question, at one and other side independenty).
    // If the split point is exacty over any of this bisectors then the split point ocurres exactly and one (or both) endpoints
    // of the opposite edge (so it is a pseudo-split event since the opposite edge is not itself split in two halfeves)
    // When this predicate is called to test (prev,e), e is the primary edge but since it is  pass as e1, primary_is_0=false.
    // This causes the case of parallel but not collinear edges to return positive when the split point is before the source point of e*
    // (a positive result means invalid split).
    // Likewise, primary_is_0 must be true when testing (e,nexte) to return negative if the split point is past the target endpoint of e*.
    // (in the other cases there is no need to discrminate which is 'e' in the call since the edjes do not overlap).
    //
    // An edge event is a coallision of three *consecutive* edges, say, e1,e2 and e3.
    // The coallision causes e2 (the edge in the middle) to collapse and e1,e3 to become consecutive and form a new vertex.
    // In all cases there is an edge before e1, say e0, and adoubleer e3, say e4.
    // Testing for the validity of an edge event amounts to determine that (e1,e3) (the new vertex) is not before (e0,e1) nor
    // past (e3,e4).
    // Thus, and edge event is valid if the new vertex NOT to the positive side of [e0,e1] *and* NOT to the negative side o [e3,e4].
    //
    // PRECONDITIONS:
    //   There exist a single point 'p' corresponding to the event as given by the trisegment
    //   e0 and e1 are known to be consectuve at the time of the event (even if they are not consecutive in the input polygon)
    //   If e0 and e1 are not consecutive in the input, v01_event is the event that defined they very first offset vertex.
    //   If e0 and e1 are consecutive, v01_event is null.
    M_INLINE Oriented_side
    oriented_side_of_event_point_wrt_bisectorC2(Trisegment_2_ptr const &event,
                                                Segment_2 const &e0, Segment_2 const &e1,
                                                Trisegment_2_ptr const &v01_event, // can be null
                                                bool primary_is_0);

}

#ifdef WITH_INLINE
#include "sskel_pred.cpp"
#endif