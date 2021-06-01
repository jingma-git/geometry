#pragma once

namespace egl
{
    class SVert;
    class SHalfedge;
    class SFace;

    class SFace
    {
    public:
        SFace() : mID(-1), mHE(0) {}
        SFace(int id) : mID(id), mHE(0) {}

        int id() const { return mID; }
        SHalfedge *halfedge() { return mHE; }
        SHalfedge *halfedge() const { return mHE; }

        void set_halfedge(SHalfedge *aHE) { mHE = aHE; }
        void reset_id(int aID) { mID = aID; }

    private:
        int mID;
        SHalfedge *mHE;
    };

    class SHalfedge
    {
    public:
        SHalfedge() : mID(-1), mOpp(0), mNxt(0), mPrv(0), mV(0), mF(0), mSlope(ZERO)
        {
        }

        SHalfedge(int id) : mID(id), mOpp(0), mNxt(0), mPrv(0), mV(0), mF(0), mSlope(ZERO)
        {
        }

        SHalfedge(int id, Sign slope) : mID(id), mOpp(0), mNxt(0), mPrv(0), mV(0), mF(0), mSlope(slope)
        {
        }

        int id() const { return mID; }
        SHalfedge *opposite() { return mOpp; }
        SHalfedge *opposite() const { return mOpp; }
        SHalfedge *next() { return mNxt; }
        SHalfedge *next() const { return mNxt; }
        SHalfedge *prev() { return mPrv; }
        SHalfedge *prev() const { return mPrv; }
        SVert *vertex() { return mV; }
        SVert *vertex() const { return mV; }
        SFace *face() { return mF; }
        SFace *face() const { return mF; }
        Sign slope() const { return mSlope; }

        void set_opposite(SHalfedge *h) { mOpp = h; }
        void set_next(SHalfedge *h) { mNxt = h; }
        void set_prev(SHalfedge *h) { mPrv = h; }
        void set_vertex(SVert *w) { mV = w; }
        void set_face(SFace *g) { mF = g; }
        void set_slope(Sign aSlope) { mSlope = aSlope; }

        bool is_border() const { return mF == 0; }
        bool is_bisector() const
        {
            return !this->is_border() && !this->opposite()->is_border();
        }

        bool is_inner_bisector() const;

        bool has_null_segment() const;

        bool has_infinite_time() const;

        SHalfedge *defining_contour_edge() const { return this->face()->halfedge(); }
        SHalfedge *defining_contour_edge() { return this->face()->halfedge(); }

    private:
        int mID;
        SHalfedge *mOpp;
        SHalfedge *mNxt;
        SHalfedge *mPrv;
        SVert *mV;
        SFace *mF;
        Sign mSlope;
    };

    class SVert
    {
        enum Flags
        {
            IsSplitBit = 0x01,
            HasInfiniteTimeBit = 0x02
        };

    public:
        SVert() : mID(-1),
                  mHE(0),
                  mTime(0.0),
                  mFlags(0)
        {
        }

        // Infinite vertex
        SVert(int id) : mID(id),
                        mHE(0),
                        mTime(std::numeric_limits<double>::max()),
                        mFlags(HasInfiniteTimeBit)
        {
        }

        // Contour vertex
        SVert(int id, const Pnt2 &aP)
            : mID(id),
              mHE(0),
              mP(aP),
              mTime(0.0),
              mFlags(0)
        {
        }

        // Skeleton vertex, corresponding to a split or edge event.
        SVert(int aID, const Pnt2 &aP, double aTime, bool aIsSplit, bool aHasInfiniteTime)
            : mID(aID),
              mHE(0),
              mP(aP),
              mTime(aTime),
              mFlags((aIsSplit ? IsSplitBit : 0) | (aHasInfiniteTime ? HasInfiniteTimeBit : 0))
        {
        }

        int id() const { return mID; }
        double time() const { return mTime; }
        bool has_infinite_time() const { return (mFlags & HasInfiniteTimeBit) == HasInfiniteTimeBit; }
        bool has_null_point() const { return has_infinite_time(); }
        bool is_split() const { return (mFlags & IsSplitBit) == IsSplitBit; }
        SHalfedge *primary_bisector() const { return halfedge()->next(); }
        SHalfedge *primary_bisector() { return halfedge()->next(); }

        bool is_skeleton() const { return halfedge()->is_bisector(); }
        bool is_contour() const { return !halfedge()->is_bisector(); }

        const Pnt2 &point() const { return mP; }
        SHalfedge *halfedge() { return mHE; }
        SHalfedge *halfedge() const { return mHE; }
        const Triedge<SHalfedge *> &event_triedge() const { return mEventTriedge; }

        void set_halfedge(SHalfedge *aHE) { mHE = aHE; }
        void set_event_triedge(const Triedge<SHalfedge *> &aTriedge) { mEventTriedge = aTriedge; }

        void reset_id__internal__(int aID) { mID = aID; }
        void reset_point__internal__(Pnt2 const &aP) { mP = aP; }

    private:
        int mID;
        SHalfedge *mHE; // halfedge from this vertex
        Pnt2 mP;

        Triedge<SHalfedge *> mEventTriedge;
        double mTime;
        unsigned char mFlags;
    };

    inline bool SHalfedge::is_inner_bisector() const
    {
        return !this->vertex()->is_contour() && !this->opposite()->vertex()->is_contour();
    }

    inline bool SHalfedge::has_null_segment() const { return this->vertex()->has_null_point(); }

    inline bool SHalfedge::has_infinite_time() const { return this->vertex()->has_infinite_time(); }

    inline std::ostream &operator<<(std::ostream &out, const SVert &v)
    {
        out << "v" << v.id() << "["
            << v.point().transpose() << ", "
            << " h=" << (v.halfedge() ? "h" + std::to_string(v.halfedge()->id()) : "null") << "]";
        return out;
    }

    inline std::ostream &operator<<(std::ostream &out, const SHalfedge &h)
    {
        out << "h" << h.id() << "["
            << " v=" << (h.vertex() ? "v" + std::to_string(h.vertex()->id()) : "null")
            << " prev=" << (h.prev() ? "h" + std::to_string(h.prev()->id()) : "null")
            << " nxt=" << (h.next() ? "h" + std::to_string(h.next()->id()) : "null")
            << " f=" << (h.face() ? "f" + std::to_string(h.face()->id()) : "null")
            << "]";
        return out;
    }

    inline std::ostream &operator<<(std::ostream &out, const SFace &f)
    {
        out << "f" << f.id() << "["
            << " h=" << (f.halfedge() ? "h" + std::to_string(f.halfedge()->id()) : "null") << "]";
        return out;
    }

    inline std::ostream &operator<<(std::ostream &out, const Triedge<SHalfedge *> &tri)
    {
        out << "{"
            << "h" << tri.e0()->id() << ","
            << "h" << tri.e1()->id() << ","
            << "h" << tri.e2()->id() << "}";
        return out;
    }

}