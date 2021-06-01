#pragma once

namespace egl
{
    class Event_2 : public Ref_counted_base
    {
    public:
        enum Type
        {
            cEdgeEvent,
            cSplitEvent,
            cPseudoSplitEvent
        };

        Event_2(const Triedge<SHalfedge *> &aTriedge, const Trisegment_2_ptr &aTrisegment)
            : mTriedge(aTriedge), mTrisegment(aTrisegment)
        {
        }

        virtual ~Event_2() {}

        virtual Type type() const = 0;
        virtual SVert *seed0() const = 0;
        virtual SVert *seed1() const = 0;

        const Triedge<SHalfedge *> &triedge() const { return mTriedge; }
        const Trisegment_2_ptr &trisegment() const { return mTrisegment; }
        const Pnt2 &point() const { return mP; }
        double time() const { return mTime; }

        void SetTimeAndPoint(double aTime, const Pnt2 &aP)
        {
            mTime = aTime;
            mP = aP;
        }

        virtual void dump(std::ostream &ss) const { ss << mTriedge; };

    private:
        Triedge<SHalfedge *> mTriedge;
        Trisegment_2_ptr mTrisegment;
        Pnt2 mP;
        double mTime;
    };

    class Edge_event_2 : public Event_2
    {
    public:
        Edge_event_2(const Triedge<SHalfedge *> &aTriedge,
                     const Trisegment_2_ptr &aTrisegment,
                     SVert *aLSeed,
                     SVert *aRSeed)
            : Event_2(aTriedge, aTrisegment),
              mLSeed(aLSeed), mRSeed(aRSeed)
        {
        }

        virtual Type type() const { return this->cEdgeEvent; }
        virtual SVert *seed0() const { return mLSeed; }
        virtual SVert *seed1() const { return mRSeed; }

        virtual void dump(std::ostream &ss) const
        {
            Event_2::dump(ss);
            ss << " (LSeed=" << mLSeed->id() << " RSeed=" << mRSeed->id() << ')';
        };

    private:
        SVert *mLSeed;
        SVert *mRSeed;
    };

    class Split_event_2 : public Event_2
    {
    public:
        Split_event_2(const Triedge<SHalfedge *> &aTriedge,
                      Trisegment_2_ptr const &aTrisegment,
                      SVert *aSeed)
            : Event_2(aTriedge, aTrisegment), mSeed(aSeed)
        {
        }

        virtual Type type() const { return this->cSplitEvent; }
        virtual SVert *seed0() const { return mSeed; }
        virtual SVert *seed1() const { return mSeed; }

        SVert *opposite_rnode() const { return mOppR; }
        void set_opposite_rnode(SVert *aOppR) { mOppR = aOppR; }

        virtual void dump(std::ostream &ss) const
        {
            Event_2::dump(ss);
            ss << " (Seed=" << mSeed->id() << " OppBorder=" << this->triedge().e2()->id() << ')';
        }

    private:
        SVert *mSeed;
        SVert *mOppR;
    };

    class Pseudo_split_event_2 : public Event_2
    {
    public:
        Pseudo_split_event_2(const Triedge<SHalfedge *> &aTriedge,
                             const Trisegment_2_ptr &aTrisegment,
                             SVert *aSeed0, SVert *aSeed1,
                             bool aOppositeIs0)
            : Event_2(aTriedge, aTrisegment),
              mSeed0(aSeed0), mSeed1(aSeed1),
              mOppositeIs0(aOppositeIs0)
        {
        }

        virtual Type type() const { return this->cPseudoSplitEvent; }
        virtual SVert *seed0() const { return mSeed0; }
        virtual SVert *seed1() const { return mSeed1; }

        bool opposite_node_is_seed_0() const { return mOppositeIs0; }
        bool is_at_source_vertex() const { return opposite_node_is_seed_0(); }
        SVert *opposite_seed() const { return opposite_node_is_seed_0() ? seed0() : seed1(); }

        virtual void dump(std::ostream &ss) const
        {
            Event_2::dump(ss);
            ss << " ("
               << "Seed0=" << mSeed0->id() << (mOppositeIs0 ? " {Opp} " : " ")
               << "Seed1=" << mSeed1->id() << (!mOppositeIs0 ? " {Opp}" : "")
               << ")";
        }

    private:
        SVert *mSeed0;
        SVert *mSeed1;
        bool mOppositeIs0;
    };

    typedef boost::intrusive_ptr<Event_2> EventPtr;

    inline std::ostream &operator<<(std::ostream &ss, Event_2 const &e)
    {
        ss << "[";
        e.dump(ss);
        ss << " p=(" << e.point().x() << "," << e.point().y() << ") t=" << e.time() << "] "
           << trisegment_collinearity_to_string(e.trisegment()->collinearity());
        return ss;
    }
}