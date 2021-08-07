#pragma once

namespace egl
{

    // This record encapsulates the defining contour halfedges for a node (both contour and skeleton)
    template <class Handle_>
    class Triedge
    {
    public:
        typedef Handle_ Handle;
        typedef Triedge<Handle> Self;
        Triedge() {}

        // Contour nodes (input polygon vertices) have only 2 defining contour edges
        Triedge(Handle aE0, Handle aE1)
        {
            mE[0] = aE0;
            mE[1] = aE1;
            // mE[2] gets default constructed, i.e., "null".
        }

        // Skeleton nodes (offset polygon vertices) have 3 defining contour edges
        Triedge(Handle aE0, Handle aE1, Handle aE2)
        {
            mE[0] = aE0;
            mE[1] = aE1;
            mE[2] = aE2;
        }

        Handle e(unsigned idx) const
        {
            return mE[idx];
        }

        Handle e0() const { return e(0); }
        Handle e1() const { return e(1); }
        Handle e2() const { return e(2); }

        bool is_valid() const { return e0() != 0 && e1() != 0; }

        bool is_contour() const { return e2() == 0; }
        bool is_skeleton() const { return e2() != 0; }

        // ToDO:
        bool is_contour_terminal() const { return e0() == e1(); }
        bool is_skeleton_terminal() const { return e0() == e1() || e1() == e2(); }

        // Returns true if the triedges store the same 3 halfedges (in any order)
        friend bool operator==(Self const &x, Self const &y)
        {
            return x.number_of_unique_edges() == y.number_of_unique_edges() && CountInCommon(x, y) == x.number_of_unique_edges();
        }

        friend bool operator!=(Self const &x, Self const &y) { return !(x == y); }

        friend Self operator&(Self const &x, Self const &y)
        {
            //ToDO:
            return Self(x.e0(), x.e1(), (x.e0() == y.e0() || x.e1() == y.e0()) ? y.e1() : y.e0());
        }

    private:
        // returns 1 if aE is one of the halfedges stored in this triedge, 0 otherwise.
        int contains(Handle aE) const
        {
            return aE == e0() || aE == e1() || aE == e2() ? 1 : 0;
        }

        int number_of_unique_edges() const
        {
            return is_contour() ? (is_contour_terminal() ? 1 : 2) : (is_skeleton_terminal() ? 2 : 3);
        }

        // Returns the number of common halfedges in the two triedges x and y
        static int CountInCommon(Self const &x, Self const &y)
        {
            Handle lE[3];

            int lC = 1;

            lE[0] = y.e0();

            if (y.e0() != y.e1())
                lE[lC++] = y.e1();

            if (y.e0() != y.e2() && y.e1() != y.e2())
                lE[lC++] = y.e2();

            return x.contains(lE[0]) + x.contains(lE[1]) + (lC > 2 ? x.contains(lE[2]) : 0);
        }

    private:
        Handle mE[3];
    };

    enum Trisegment_collinearity
    {
        TRISEGMENT_COLLINEARITY_NONE,
        TRISEGMENT_COLLINEARITY_01,
        TRISEGMENT_COLLINEARITY_12,
        TRISEGMENT_COLLINEARITY_02,
        TRISEGMENT_COLLINEARITY_ALL
    };

    inline char const *trisegment_collinearity_to_string(Trisegment_collinearity c)
    {
        switch (c)
        {
        case TRISEGMENT_COLLINEARITY_NONE:
            return "<>";
        case TRISEGMENT_COLLINEARITY_01:
            return "<0,1>";
        case TRISEGMENT_COLLINEARITY_12:
            return "<1,2>";
        case TRISEGMENT_COLLINEARITY_02:
            return "<0,2>";
        case TRISEGMENT_COLLINEARITY_ALL:
            return "<0,1,2>";
        }

        return "!!UNKNOWN COLLINEARITY!!";
    }

    class Trisegment_2 : public Ref_counted_base
    {
    public:
        typedef boost::intrusive_ptr<Trisegment_2> Self_ptr;

        Trisegment_2(const Segment_2 &aE0,
                     const Segment_2 &aE1,
                     const Segment_2 &aE2,
                     Trisegment_collinearity aCollinearity)

        {
            mCollinearity = aCollinearity;

            mE[0] = aE0;
            mE[1] = aE1;
            mE[2] = aE2;

            switch (aCollinearity)
            {
            case TRISEGMENT_COLLINEARITY_01:
                mCSIdx = 0;
                mNCSIdx = 2;
                break;
            case TRISEGMENT_COLLINEARITY_02:
                mCSIdx = 0;
                mNCSIdx = 1;
                break;
            case TRISEGMENT_COLLINEARITY_12:
                mCSIdx = 1;
                mNCSIdx = 0;
                break;
            case TRISEGMENT_COLLINEARITY_NONE:
                mCSIdx = mNCSIdx = (std::numeric_limits<unsigned>::max)();
                break;
            case TRISEGMENT_COLLINEARITY_ALL:
                mCSIdx = mNCSIdx = (std::numeric_limits<unsigned>::max)();
                break;
            }
        }

        Trisegment_collinearity collinearity() const { return mCollinearity; }
        const Segment_2 &e(unsigned idx) const { return mE[idx]; }
        const Segment_2 &e0() const { return e(0); }
        const Segment_2 &e1() const { return e(1); }
        const Segment_2 &e2() const { return e(2); }

        // If 2 out of the 3 edges are collinear they can be reclassified as 1 collinear edge (any of the 2) and 1 non-collinear.
        // These methods returns the edges according to that classification.
        // PRECONDITION: Exactly 2 out of 3 edges are collinear
        const Segment_2 &collinear_edge() const { return e(mCSIdx); }
        const Segment_2 &non_collinear_edge() const { return e(mNCSIdx); }

        Self_ptr child_l() const { return mChildL; }
        Self_ptr child_r() const { return mChildR; }

        void set_child_l(Self_ptr const &aChild) { mChildL = aChild; } //ToDO:
        void set_child_r(Self_ptr const &aChild) { mChildR = aChild; } //ToDO:

        enum SEED_ID
        {
            LEFT,
            RIGHT,
            UNKNOWN
        };

        // Indicates which of the seeds is collinear for a normal collinearity case.
        // PRECONDITION: The collinearity is normal.
        SEED_ID degenerate_seed_id() const
        {
            Trisegment_collinearity c = collinearity();

            return c == TRISEGMENT_COLLINEARITY_01 ? LEFT : c == TRISEGMENT_COLLINEARITY_12 ? RIGHT
                                                                                            : UNKNOWN;
        }

    private:
        Segment_2 mE[3];
        Trisegment_collinearity mCollinearity;
        unsigned mCSIdx, mNCSIdx; // CS: colinear segment, NCS: non-colinear segment
        Self_ptr mChildL;
        Self_ptr mChildR;
    };

    typedef typename Trisegment_2::Self_ptr Trisegment_2_ptr;

}