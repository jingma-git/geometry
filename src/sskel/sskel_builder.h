#pragma once
#include "straight_skeleton.h"

namespace egl
{
    class Straight_skeleton_builder_2
    {
    public:
        typedef Straight_skeleton_builder_2 Self;
        typedef Straight_skeleton_2 SSkel;
        typedef std::shared_ptr<SSkel> SSkelPtr;

        typedef typename SSkel::Vertex_iterator Vertex_iterator;
        typedef typename SSkel::Halfedge_iterator Halfedge_iterator;
        typedef typename SSkel::Face_iterator Face_iterator;

        typedef Event_2 Event;
        typedef Edge_event_2 EdgeEvent;
        typedef Split_event_2 SplitEvent;
        typedef Pseudo_split_event_2 PseudoSplitEvent;

        class Event_compare : public std::binary_function<EventPtr, EventPtr, bool>
        {
        public:
            Event_compare(Self const *aBuilder) : mBuilder(aBuilder) {}

            // std::greater<int> makes smaller element on the top
            // Event with shorter time to intersect comes first
            bool operator()(EventPtr const &aA, EventPtr const &aB) const
            {
                return mBuilder->CompareEvents(aA, aB) == LARGER;
            }

        private:
            Self const *mBuilder;
        };

        typedef std::priority_queue<EventPtr, std::vector<EventPtr>, Event_compare> PQ;

        struct Vertex_data : public Ref_counted_base
        {
            Vertex_data(SVert *aVertex, Event_compare const &aComparer)
                : mVertex(aVertex),
                  mIsReflex(false),
                  mIsDegenerate(false),
                  mIsProcessed(false),
                  mIsExcluded(false),
                  mPrevInLAV(-1),
                  mNextInLAV(-1),
                  mNextSplitEventInMainPQ(false),
                  mSplitEvents(aComparer)
            {
            }

            SVert *mVertex;
            bool mIsReflex;
            bool mIsDegenerate;
            bool mIsProcessed;
            bool mIsExcluded;
            int mPrevInLAV;
            int mNextInLAV;

            bool mNextSplitEventInMainPQ;
            PQ mSplitEvents;
            Triedge<SHalfedge *> mTriedge; // Here, E0,E1 corresponds to the vertex (unlike *event* triedges)
            Trisegment_2_ptr mTrisegment;  // Skeleton nodes cache the full trisegment tree that defines the originating event
        };

        typedef boost::intrusive_ptr<Vertex_data> Vertex_data_ptr;

    public:
        Straight_skeleton_builder_2()
            : mEventCompare(this),
              mVertexID(0),
              mEdgeID(0),
              mFaceID(0),
              mEventID(0),
              mStepID(0),
              mPQ(mEventCompare),
              mSSkel(new SSkel())
        {
        }

        void enter_contour(const std::vector<Pnt2> &pts)
        {
            SHalfedge *lFirstCCWBorder = 0;
            SHalfedge *lPrevCCWBorder = 0;
            SHalfedge *lNextCWBorder = 0;

            SVert *lFirstVertex = 0;
            SVert *lPrevVertex = 0;

            for (size_t i = 0; i < pts.size(); ++i)
            {
                SHalfedge *lCCWBorder = mSSkel->edges_push_back(SHalfedge(mEdgeID), SHalfedge(mEdgeID + 1));
                SHalfedge *lCWBorder = lCCWBorder->opposite();
                mEdgeID += 2;

                mContourHalfedges.push_back(lCCWBorder);

                SVert *lVertex = mSSkel->vertices_push_back(SVert(mVertexID++, pts[i]));
                InitVertexData(lVertex);

                SFace *lFace = mSSkel->faces_push_back(SFace(mFaceID++));

                lCCWBorder->set_face(lFace);
                lFace->set_halfedge(lCCWBorder);

                lVertex->set_halfedge(lCCWBorder);
                lCCWBorder->set_vertex(lVertex);

                if (i == 0)
                {
                    lFirstVertex = lVertex;
                    lFirstCCWBorder = lCCWBorder;
                }
                else
                {
                    SetPrevInLAV(lVertex, lPrevVertex);
                    SetNextInLAV(lPrevVertex, lVertex);

                    SetVertexTriedge(lPrevVertex, Triedge<SHalfedge *>(lPrevCCWBorder, lCCWBorder));

                    lCWBorder->set_vertex(lPrevVertex); //ToDO: why

                    lCCWBorder->set_prev(lPrevCCWBorder);
                    lPrevCCWBorder->set_next(lCCWBorder);

                    lNextCWBorder->set_prev(lCCWBorder);
                    lCWBorder->set_next(lNextCWBorder);
                }

                lPrevVertex = lVertex;
                lPrevCCWBorder = lCCWBorder;
                lNextCWBorder = lCWBorder;
            }

            SetPrevInLAV(lFirstVertex, lPrevVertex);
            SetNextInLAV(lPrevVertex, lFirstVertex);

            SetVertexTriedge(lPrevVertex, Triedge<SHalfedge *>(lPrevCCWBorder, lFirstCCWBorder));
            lFirstCCWBorder->opposite()->set_vertex(lPrevVertex);

            lFirstCCWBorder->set_prev(lPrevCCWBorder);
            lPrevCCWBorder->set_next(lFirstCCWBorder);

            lNextCWBorder->set_prev(lFirstCCWBorder->opposite());
            lFirstCCWBorder->opposite()->set_next(lNextCWBorder);
        }

        void InitVertexData(SVert *aV)
        {
            mVertexData.push_back(Vertex_data_ptr(new Vertex_data(aV, mEventCompare)));
        }

        Vertex_data &GetVertexData(SVert *aV)
        {
            return *mVertexData[aV->id()];
        }

        const Vertex_data &GetVertexData(SVert *aV) const
        {
            return *mVertexData[aV->id()];
        }

        SVert *GetVertex(int aIdx)
        {
            return mVertexData[aIdx]->mVertex;
        }

        SVert *GetPrevInLAV(SVert *aV)
        {
            return GetVertex(GetVertexData(aV).mPrevInLAV);
        }

        SVert *GetNextInLAV(SVert *aV)
        {
            return GetVertex(GetVertexData(aV).mNextInLAV);
        }

        void SetPrevInLAV(SVert *aV, SVert *aPrev)
        {
            GetVertexData(aV).mPrevInLAV = aPrev->id();
        }

        void SetNextInLAV(SVert *aV, SVert *aPrev)
        {
            GetVertexData(aV).mNextInLAV = aPrev->id();
        }

        // Null if aV is a contour node
        const Triedge<SHalfedge *> &GetVertexTriedge(SVert *aV) const
        {
            return GetVertexData(aV).mTriedge;
        }

        void SetVertexTriedge(SVert *aV, Triedge<SHalfedge *> const &aTriedge)
        {
            GetVertexData(aV).mTriedge = aTriedge;
        }

        void Exclude(SVert *aV)
        {
            GetVertexData(aV).mIsExcluded = true;
        }

        bool IsExcluded(SVert *aV) const
        {
            return GetVertexData(aV).mIsExcluded;
        }

        void SetIsReflex(SVert *aV)
        {
            GetVertexData(aV).mIsReflex = true;
        }

        bool IsReflex(SVert *aV)
        {
            return GetVertexData(aV).mIsReflex;
        }

        void SetIsDegenerate(SVert *aV)
        {
            GetVertexData(aV).mIsDegenerate = true;
        }

        bool IsDegenerate(SVert *aV)
        {
            return GetVertexData(aV).mIsDegenerate;
        }

        void SetIsProcessed(SVert *aV)
        {
            GetVertexData(aV).mIsProcessed = true;
        }

        bool IsConvex(SVert *aV)
        {
            Vertex_data const &lData = GetVertexData(aV);
            return !lData.mIsReflex && !lData.mIsDegenerate;
        }

        bool IsProcessed(SVert *aV)
        {
            return GetVertexData(aV).mIsProcessed;
        }

        bool IsProcessed(EventPtr aEvent)
        {
            return IsProcessed(aEvent->seed0()) || IsProcessed(aEvent->seed1());
        }

        // Null if aV is a contour or infinite node
        Trisegment_2_ptr const &GetTrisegment(SVert *aV) const
        {
            return GetVertexData(aV).mTrisegment;
        }

        void SetTrisegment(SVert *aV, Trisegment_2_ptr const &aTrisegment)
        {
            GetVertexData(aV).mTrisegment = aTrisegment;
        }

        void SetBisectorSlope(SHalfedge *aBisector, Sign aSlope)
        {
            aBisector->set_slope(aSlope);
        }

        void Run()
        {
            InitPhase();
            Propagate();
            // FinishUp(); //ToDO
        }

        void InitPhase()
        {
            CreateContourBisectors();
            CreateInitialEvents();
        }

        void CreateContourBisectors()
        {
            for (Vertex_iterator it = mSSkel->vertices_begin(); it != mSSkel->vertices_end(); ++it)
            {
                SVert *v = &(*it);
                mGLAV.push_back(v);
                SVert *lPrev = GetPrevInLAV(v);
                SVert *lNext = GetNextInLAV(v);

                Orientation lOrientation = orientation(lPrev->point(), v->point(), lNext->point());
                if (lOrientation == COLLINEAR)
                {
                    SetIsDegenerate(v);
                }
                else if (lOrientation == RIGHT_TURN)
                {
                    SetIsReflex(v);
                    mReflexVertices.push_back(v);
                }

                SHalfedge lOB(mEdgeID++), lIB(mEdgeID++);
                SHalfedge *lOBisector = mSSkel->edges_push_back(lOB, lIB);
                SHalfedge *lIBisector = lOBisector->opposite();
                lOBisector->set_face(v->halfedge()->face());
                lIBisector->set_face(v->halfedge()->next()->face());
                lIBisector->set_vertex(v);

                SHalfedge *lIBorder = v->halfedge();
                SHalfedge *lOBorder = v->halfedge()->next();
                lIBorder->set_next(lOBisector);
                lOBisector->set_prev(lIBorder);
                lOBorder->set_prev(lIBisector);
                lIBisector->set_next(lOBorder);
            }

            for (Face_iterator fit = mSSkel->faces_begin(); fit != mSSkel->faces_end(); ++fit)
            {
                SHalfedge *lBorder = fit->halfedge();
                SHalfedge *lLBisector = lBorder->prev();
                SHalfedge *lRBisector = lBorder->next();

                SVert *lInfNode = mSSkel->vertices_push_back(SVert(mVertexID++));
                InitVertexData(lInfNode);
                assert(lInfNode->has_null_point());

                lRBisector->set_next(lLBisector);
                lLBisector->set_prev(lRBisector);

                lRBisector->set_vertex(lInfNode);

                lInfNode->set_halfedge(lRBisector);

                SetBisectorSlope(lRBisector, POSITIVE);
                SetBisectorSlope(lLBisector, NEGATIVE);

                cout << "close face " << *fit
                     << " with ficticious vertex " << *lInfNode
                     << " lRBisector=" << *lRBisector
                     << " lLBisector=" << *lLBisector << endl;
            }
        }

        void CreateInitialEvents()
        {
            const Triedge<SHalfedge *> cNull_triedge;
            for (Vertex_iterator vit = mSSkel->vertices_begin(); vit != mSSkel->vertices_end(); ++vit)
            {
                SVert *v = &(*vit);
                std::cout << "--------------------------------------------------create event for " << *v << std::endl;
                if (!v->has_infinite_time())
                {
                    UpdatePQ(v, cNull_triedge);
                }
            }
        }

        void UpdatePQ(SVert *aNode, const Triedge<SHalfedge *> &aPrevEventTriedge)
        {
            SVert *lPrev = GetPrevInLAV(aNode);
            SVert *lNext = GetNextInLAV(aNode);

            SHalfedge *lOBisector_P = lPrev->primary_bisector();
            SHalfedge *lOBisector_C = aNode->primary_bisector();
            SHalfedge *lOBisector_N = lNext->primary_bisector();

            // if (AreBisectorsCoincident(lOBisector_C, lOBisector_P)) //ToDO
            // {
            //     HandleSimultaneousEdgeEvent(aNode, lPrev);
            // }
            // else if (AreBisectorsCoincident(lOBisector_C, lOBisector_N))
            // {
            //     HandleSimultaneousEdgeEvent(aNode, lNext);
            // }
            // else
            {
                CollectNewEvents(aNode, aPrevEventTriedge);
            }
        }

        void CollectNewEvents(SVert *aNode, const Triedge<SHalfedge *> &aPrevEventTriedge)
        {
            // At non-reflex internal angles (<PI): edges contract
            // At reflex internal angles (>PI): edges expand
            // In degenerate case of angle PI: edge neither contracts nor expands

            // A Straight Skeleton is the trace of the 'grassfire propagation' that corresponds to the inward move of all the vertices
            // of a polygon along their angular bisectors.
            // Since vertices are the common endpoints of contour edges, the propagation corresponds to contour edges moving inward,
            // shrinking and expanding as neccesasry to keep the vertices along the angular bisectors.
            // At each instant in time the current location of vertices (and edges) describe the current 'Offset polygon'
            // (with at time zero corresponds to the input polygon).
            //
            // An 'edge wavefront' is a moving contour edge.
            // A 'vertex wavefront' is the wavefront of two consecutive edge wavefronts (sharing a moving vertex).
            //
            // An 'Event' is the coallision of 2 wavefronts.
            // Each event changes the topology of the shrinking polygon; that is, at the event, the current polygon differs from the
            // inmediately previous polygon in the number of vertices.
            //
            // If 2 vertex wavefronts sharing a common edge collide, the event is called an edge event. At the time of the event, the current
            // polygon doex not have the common edge anynmore, and the two vertices become one. This new 'skeleton' vertex generates a new
            // vertex wavefront which can further collide with other wavefronts, producing for instance, more edge events.
            //
            // If a refex vertex wavefront collide with an edge wavefront, the event is called a split event. At the time of the event, the current
            // polygon is split in two unconnected polygons, each one containing a portion of the edge hit and split by the reflex wavefront.
            //
            // If 2 reflex wavefronts collide each other, the event is called a vertex event. At the time of the event, the current polygon
            // is split in two unconnected polygons. Each one contains a different combination of the colliding reflex edges. That is, if the
            // wavefront (edgea,edgeb) collides with (edgec,edged), the two resulting polygons will contain (edgea,edgec) and (edgeb,edged).
            // Furthermore, one of the new vertices can be a reflex vertex generating a reflex wavefront which can further produce more split or
            // vertex events (or edge events of course)
            //
            // Each vertex wavefront (reflex or not) results in one and only one event from a set of possible events.
            // It can result in a edge event against the vertex wavefronts emerging from the adjacent vertices (in the current polygon, not
            // in the input polygon); or it can result in a split event (or vertex event) against any other wavefront in the rest of
            // current polygon.

            // Adjacent vertices in the current polygon containing aNode (called LAV)

            SVert *lPrev = GetPrevInLAV(aNode);
            SVert *lNext = GetNextInLAV(aNode);

            if (IsReflex(aNode))
            {
                CollectSplitEvents(aNode, aPrevEventTriedge); //ToDO
            }

            EventPtr lLEdgeEvent = FindEdgeEvent(lPrev, aNode, aPrevEventTriedge);
            EventPtr lREdgeEvent = FindEdgeEvent(aNode, lNext, aPrevEventTriedge);

            bool lAcceptL = !!lLEdgeEvent;
            bool lAcceptR = !!lREdgeEvent;

            if (lAcceptL)
            {
                InsertEventInPQ(lLEdgeEvent);
                cout << "lAcceptL: " << *lLEdgeEvent << endl;
            }

            if (lAcceptR)
            {
                InsertEventInPQ(lREdgeEvent);
                cout << "lAcceptR: " << *lREdgeEvent << endl;
            }
        }

        EventPtr FindEdgeEvent(SVert *aLNode, SVert *aRNode, Triedge<SHalfedge *> const &aPrevEventTriedge)
        {
            EventPtr rResult;
            Triedge<SHalfedge *> lTriedge = GetVertexTriedge(aLNode) & GetVertexTriedge(aRNode);

            if (lTriedge.is_valid() && lTriedge != aPrevEventTriedge)
            {
                Trisegment_2_ptr lTrisegment = CreateTrisegment(lTriedge, aLNode, aRNode);
                if (ExistEvent(lTrisegment))
                {
                    Comparison_result lLNodeD = CompareEvents(lTrisegment, aLNode);
                    Comparison_result lRNodeD = CompareEvents(lTrisegment, aRNode);

                    if (lLNodeD != SMALLER && lRNodeD != SMALLER)
                    {
                        rResult = EventPtr(new EdgeEvent(lTriedge, lTrisegment, aLNode, aRNode));
                    }
                    else
                    {
                        M_DEBUG << "Edge event: " << lTriedge << " is in the past. Compared to L=" << aLNode->id() << " to R=" << lRNodeD;
                    }
                }
            }
            return rResult;
        }

        Trisegment_2_ptr CreateTrisegment(Triedge<SHalfedge *> const &aTriedge) const
        {
            Trisegment_2_ptr r = construct_trisegment(CreateSegment(aTriedge.e0()),
                                                      CreateSegment(aTriedge.e1()),
                                                      CreateSegment(aTriedge.e2()));
            return r;
        }

        Trisegment_2_ptr CreateTrisegment(Triedge<SHalfedge *> const &aTriedge, SVert *aLSeed) const
        {
            Trisegment_2_ptr r = CreateTrisegment(aTriedge);
            r->set_child_l(GetTrisegment(aLSeed));
            return r;
        }

        Trisegment_2_ptr CreateTrisegment(Triedge<SHalfedge *> const &aTriedge,
                                          SVert *aLSeed, SVert *aRSeed) const
        {
            Trisegment_2_ptr r = CreateTrisegment(aTriedge);
            r->set_child_l(GetTrisegment(aLSeed));
            r->set_child_r(GetTrisegment(aRSeed));
            return r;
        }

        Segment_2 CreateSegment(SHalfedge *aH) const
        {
            Pnt2 s = aH->opposite()->vertex()->point();
            Pnt2 t = aH->vertex()->point();
            return Segment_2(s, t);
        }

        bool ExistEvent(Trisegment_2_ptr const &aS)
        {
            return exist_offset_lines_isec(aS);
        }

        Comparison_result CompareEvents(Trisegment_2_ptr const &aTrisegment, SVert *aSeedNode) const
        {
            bool is_skel = aSeedNode->is_skeleton();
            bool is_inf = aSeedNode->has_infinite_time();
            return aSeedNode->is_skeleton() ? aSeedNode->has_infinite_time() ? SMALLER
                                                                             : CompareEvents(aTrisegment, GetTrisegment(aSeedNode))
                                            : LARGER;
        }

        Comparison_result CompareEvents(Trisegment_2_ptr const &aA, Trisegment_2_ptr const &aB) const
        {
            return compare_offset_lines_isec_times(aA, aB);
        }

        Comparison_result CompareEvents(EventPtr const &aA, EventPtr const &aB) const
        {
            return aA->triedge() != aB->triedge() ? CompareEvents(aA->trisegment(), aB->trisegment()) : EQUAL;
        }

        void InsertEventInPQ(EventPtr aEvent)
        {
            mPQ.push(aEvent);
        }

        void CollectSplitEvents(SVert *aNode, Triedge<SHalfedge *> const &aPrevEventTriedge)
        {
            // lLBorder and lRBorder are the consecutive contour edges forming the reflex wavefront.
            Triedge<SHalfedge *> const &lTriedge = GetVertexTriedge(aNode);

            SHalfedge *lLBorder = lTriedge.e0();
            SHalfedge *lRBorder = lTriedge.e1();

            for (std::vector<SHalfedge *>::iterator i = mContourHalfedges.begin(); i != mContourHalfedges.end(); ++i)
            {
                SHalfedge *lOpposite = *i;

                if (lOpposite != lLBorder && lOpposite != lRBorder)
                {
                    Triedge lEventTriedge(lLBorder, lRBorder, lOpposite);

                    if (lEventTriedge != aPrevEventTriedge)
                    {
                        CollectSplitEvent(aNode, lEventTriedge);
                    }
                }
            }
        }

        void CollectSplitEvent(SVert *aNode, Triedge<SHalfedge *> const &aTriedge)
        {
            if (IsOppositeEdgeFacingTheSplitSeed(aNode, aTriedge.e2()))
            {
                Trisegment_2_ptr lTrisegment = CreateTrisegment(aTriedge, aNode);

                if (lTrisegment->collinearity() != TRISEGMENT_COLLINEARITY_02 && ExistEvent(lTrisegment))
                {
                    if (CompareEvents(lTrisegment, aNode) != SMALLER)
                    {
                        EventPtr lEvent = EventPtr(new SplitEvent(aTriedge, lTrisegment, aNode));
                        AddSplitEvent(aNode, lEvent);
                        std::cout << *aNode << " splitEvent=" << *lEvent << std::endl;
                    }
                }
            }
        }

        void AddSplitEvent(SVert *aV, EventPtr aEvent)
        {
            GetVertexData(aV).mSplitEvents.push(aEvent);
        }

        bool IsOppositeEdgeFacingTheSplitSeed(SVert *aSeed, SHalfedge *aOpposite) const
        {
            if (aSeed->is_skeleton())
                return is_edge_facing_offset_lines_isecC2(GetTrisegment(aSeed), CreateSegment(aOpposite));
            else
                return is_edge_facing_point(aSeed->point(), CreateSegment(aOpposite));
        }

        void Propagate()
        {
            cout << "Propagating events..." << endl;
            for (;;)
            {
                InsertNextSplitEventsInPQ(); //

                if (!mPQ.empty())
                {
                    EventPtr lEvent = PopEventFromPQ();

                    if (lEvent->type() != Event::cEdgeEvent)
                        AllowNextSplitEvent(lEvent->seed0()); // mVertexData[aNode]'s mNextSplitEventInMainPQ = false

                    if (!IsProcessed(lEvent))
                    {
                        SetEventTimeAndPoint(*lEvent);

                        switch (lEvent->type())
                        {
                        case Event::cEdgeEvent:
                        {
                            HandleEdgeEvent(lEvent);
                            break;
                        }
                        case Event::cSplitEvent:
                            // HandleSplitOrPseudoSplitEvent(lEvent); // ToDO
                            break;
                        case Event::cPseudoSplitEvent:
                            // HandlePseudoSplitEvent(lEvent); //ToDO
                            break;
                        }

                        ++mStepID;
                    }
                }
                else
                    break;
            }
        }

        void InsertNextSplitEventsInPQ()
        {
            for (std::vector<SVert *>::iterator vit = mReflexVertices.begin(); vit != mReflexVertices.end(); ++vit)
            {
                SVert *v = *vit;
                if (!IsProcessed(v))
                    InsertNextSplitEventInPQ(v);
            }
        }

        void InsertNextSplitEventInPQ(SVert *v)
        {
            EventPtr lSplitEvent = PopNextSplitEvent(v);
            if (!!lSplitEvent)
                InsertEventInPQ(lSplitEvent);
        }

        EventPtr PopNextSplitEvent(SVert *aV)
        {
            EventPtr rEvent;
            Vertex_data &lData = GetVertexData(aV);
            if (!lData.mNextSplitEventInMainPQ)
            {
                PQ &lPQ = lData.mSplitEvents;
                if (!lPQ.empty())
                {
                    rEvent = lPQ.top();
                    lPQ.pop();
                    lData.mNextSplitEventInMainPQ = true;
                }
            }
            return rEvent;
        }

        EventPtr PopEventFromPQ()
        {
            EventPtr rR = mPQ.top();
            mPQ.pop();
            return rR;
        }

        void AllowNextSplitEvent(SVert *aV)
        {
            GetVertexData(aV).mNextSplitEventInMainPQ = false;
        }

        void SetEventTimeAndPoint(Event &aE)
        {
            boost::optional<double> lTime = compute_offset_lines_isec_timeC2(aE.trisegment());
            boost::optional<Pnt2> lP = construct_offset_lines_isecC2(aE.trisegment());
            if (lTime && lP)
                aE.SetTimeAndPoint(*lTime, *lP);
            else
                assert(false && "No Intersection Time and Pnt2 found for Event!");
        }

        void HandleEdgeEvent(EventPtr aEvent)
        {
            EdgeEvent &lEvent = dynamic_cast<EdgeEvent &>(*aEvent);

            if (IsValidEdgeEvent(lEvent))
            {
                SVert *lLSeed = lEvent.seed0();
                SVert *lRSeed = lEvent.seed1();

                SVert *lNewNode = ConstructEdgeEventNode(lEvent);

                SHalfedge *lLOBisector = lLSeed->primary_bisector();
                SHalfedge *lROBisector = lRSeed->primary_bisector();
                SHalfedge *lLIBisector = lLOBisector->opposite();
                SHalfedge *lRIBisector = lROBisector->opposite();

                SVert *lRIFicNode = lROBisector->vertex();
                SVert *lLOFicNode = lLOBisector->vertex();

                CrossLink(lLOBisector, lNewNode);

                Link(lROBisector, lNewNode);

                CrossLinkFwd(lROBisector, lLIBisector); // //ToDO make a face lROBisector->lNewNode->lLIBisector

                SHalfedge *lDefiningBorderA = lNewNode->halfedge()->defining_contour_edge();
                SHalfedge *lDefiningBorderB = lNewNode->halfedge()->opposite()->prev()->opposite()->defining_contour_edge();
                SHalfedge *lDefiningBorderC = lNewNode->halfedge()->opposite()->prev()->defining_contour_edge();

                lNewNode->set_event_triedge(lEvent.triedge());

                Triedge<SHalfedge *> lTri(lDefiningBorderA, lDefiningBorderB, lDefiningBorderC);

                SetVertexTriedge(lNewNode, lTri);

                SetBisectorSlope(lLSeed, lNewNode);
                SetBisectorSlope(lRSeed, lNewNode);

                if (lLOFicNode->has_infinite_time())
                {
                    SHalfedge *lNOBisector = mSSkel->edges_push_back(SHalfedge(mEdgeID), SHalfedge(mEdgeID + 1));

                    SHalfedge *lNIBisector = lNOBisector->opposite();
                    mEdgeID += 2;

                    CrossLinkFwd(lNOBisector, lLOBisector->next());
                    CrossLinkFwd(lRIBisector->prev(), lNIBisector);

                    CrossLink(lNOBisector, lLOFicNode);

                    SetBisectorSlope(lNOBisector, POSITIVE);
                    SetBisectorSlope(lNIBisector, NEGATIVE);

                    CrossLinkFwd(lNIBisector, lRIBisector);
                    CrossLinkFwd(lLOBisector, lNOBisector);

                    Link(lNOBisector, lLOBisector->face());
                    Link(lNIBisector, lRIBisector->face());

                    Link(lNIBisector, lNewNode);

                    cout << "Ficticius N" << lRIFicNode->id() << " erased." << endl;
                    EraseNode(lRIFicNode);

                    SetupNewNode(lNewNode);

                    UpdatePQ(lNewNode, lEvent.triedge());
                }
                else
                {
                    assert(false && "This is a multiple node (A node with these defining edges already exist in the LAV)");
                }
            }
        }

        bool IsValidEdgeEvent(EdgeEvent const &aEvent)
        {
            bool rResult = false;

            SVert *lLSeed = aEvent.seed0();
            SVert *lRSeed = aEvent.seed1();

            SVert *lPrevLSeed = GetPrevInLAV(lLSeed);
            SVert *lNextRSeed = GetNextInLAV(lRSeed);

            if (lPrevLSeed != lNextRSeed)
            {
                SHalfedge *lPrevE0 = GetEdgeEndingAt(lPrevLSeed);
                SHalfedge *lE0 = aEvent.triedge().e0();
                SHalfedge *lE2 = aEvent.triedge().e2();
                SHalfedge *lNextE2 = GetEdgeStartingAt(lNextRSeed);

                Oriented_side lLSide = EventPointOrientedSide(aEvent, lPrevE0, lE0, lPrevLSeed, false);
                Oriented_side lRSide = EventPointOrientedSide(aEvent, lE2, lNextE2, lNextRSeed, true);

                bool lLSideOK = (lLSide != ON_POSITIVE_SIDE);
                bool lRSideOK = (lRSide != ON_NEGATIVE_SIDE);

                if (!lLSideOK)
                {
                    cout << "Invalid edge event: " << aEvent.triedge()
                         << " NewNode is before E" << lE0->id() << " source N" << lPrevLSeed->id();
                }

                if (!lRSideOK)
                {
                    cout << "Invalid edge event: " << aEvent.triedge()
                         << " NewNode is past E" << lE2->id() << " target N" << lNextRSeed->id();
                }

                rResult = lLSideOK && lRSideOK;
            }
            else
            {
                // Triangle collapse. No need to test explicitely.
                rResult = true;
            }
            return rResult;
        }

        SHalfedge *GetEdgeEndingAt(SVert *aV)
        {
            return GetVertexTriedge(aV).e0();
        }

        SHalfedge *GetEdgeStartingAt(SVert *aV)
        {
            return GetEdgeEndingAt(GetNextInLAV(aV));
        }

        Oriented_side EventPointOrientedSide(Event const &aEvent,
                                             SHalfedge *aE0,
                                             SHalfedge *aE1,
                                             SVert *aV01, bool aE0isPrimary) const
        {

            return oriented_side_of_event_point_wrt_bisectorC2(aEvent.trisegment(),
                                                               CreateSegment(aE0),
                                                               CreateSegment(aE1),
                                                               GetTrisegment(aV01), // Can be null
                                                               aE0isPrimary);
        }

        SVert *ConstructEdgeEventNode(EdgeEvent &aEvent)
        {
            SVert *lLSeed = aEvent.seed0();
            SVert *lRSeed = aEvent.seed1();

            SVert *lNewNode = mSSkel->vertices_push_back(SVert(mVertexID++, aEvent.point(), aEvent.time(), false, false));
            InitVertexData(lNewNode);

            mGLAV.push_back(lNewNode);

            SetTrisegment(lNewNode, aEvent.trisegment());

            SetIsProcessed(lLSeed);
            SetIsProcessed(lRSeed);
            mGLAV.remove(lLSeed);
            mGLAV.remove(lRSeed);

            SVert *lLPrev = GetPrevInLAV(lLSeed);
            SVert *lRNext = GetNextInLAV(lRSeed);

            SetPrevInLAV(lNewNode, lLPrev);
            SetNextInLAV(lLPrev, lNewNode);

            SetNextInLAV(lNewNode, lRNext);
            SetPrevInLAV(lRNext, lNewNode);
            return lNewNode;
        }

        void Link(SHalfedge *aH, SFace *aF)
        {
            aH->set_face(aF);
        }

        void Link(SHalfedge *aH, SVert *aV)
        {
            aH->set_vertex(aV);
        }

        void Link(SVert *aV, SHalfedge *aH)
        {
            aV->set_halfedge(aH);
        }

        void CrossLinkFwd(SHalfedge *aPrev, SHalfedge *aNext)
        {
            aPrev->set_next(aNext);
            aNext->set_prev(aPrev);
        }

        void CrossLink(SHalfedge *aH, SVert *aV)
        {
            Link(aH, aV);
            Link(aV, aH);
        }

        void SetBisectorSlope(SVert *aA, SVert *aB)
        {
            SHalfedge *lOBisector = aA->primary_bisector();
            SHalfedge *lIBisector = lOBisector->opposite();

            assert(!aA->is_contour() || !aB->is_contour());

            if (aA->is_contour())
            {
                SetBisectorSlope(lOBisector, POSITIVE);
                SetBisectorSlope(lIBisector, NEGATIVE);
            }
            else if (aB->is_contour())
            {
                SetBisectorSlope(lOBisector, NEGATIVE);
                SetBisectorSlope(lIBisector, POSITIVE);
            }
            else
            {
                if (aA->has_infinite_time())
                {
                    assert(!aB->has_infinite_time());

                    SetBisectorSlope(lOBisector, NEGATIVE);
                    SetBisectorSlope(lIBisector, POSITIVE);
                }
                else if (aB->has_infinite_time())
                {
                    assert(!aA->has_infinite_time());

                    SetBisectorSlope(lOBisector, NEGATIVE);
                    SetBisectorSlope(lIBisector, POSITIVE);
                }
                else
                {
                    assert(!aA->has_infinite_time());
                    assert(!aB->has_infinite_time());

                    Sign lSlope = CompareEvents(GetTrisegment(aB), GetTrisegment(aA));
                    SetBisectorSlope(lOBisector, lSlope);
                    SetBisectorSlope(lIBisector, opposite(lSlope));
                }
            }
        }

        void EraseNode(SVert *aNode)
        {
            mSSkel->vertices_erase(aNode);
            // aNode->reset_id__internal__(-aNode->id()); // ToDO: maybe invalid
        }

        void SetupNewNode(SVert *aNode)
        {
            // In an edge-edge anihiliation the current polygon becomes
            // a two-node degenerate chain collapsed into a single point
            if (GetPrevInLAV(aNode) != GetNextInLAV(aNode))
            {
                SHalfedge *lLE = GetEdgeEndingAt(aNode);
                SHalfedge *lRE = GetEdgeStartingAt(aNode);

                Vec2 lLV = CreateVector(lLE);
                Vec2 lRV = CreateVector(lRE);

                Orientation lOrientation = orientation(lLV, lRV);
                if (lOrientation == COLLINEAR)
                {
                    SetIsDegenerate(aNode);
                }
                else if (lOrientation == RIGHT_TURN)
                {
                    mReflexVertices.push_back(aNode);
                    SetIsReflex(aNode);
                }
            }
        }

        Vec2 CreateVector(SHalfedge *aH) const
        {
            Pnt2 s = aH->opposite()->vertex()->point();
            Pnt2 t = aH->vertex()->point();
            return t - s;
        }

        void FinishUp()
        {
        }

    private:
        SSkelPtr mSSkel;
        int mVertexID;
        int mEdgeID;
        int mFaceID;
        int mEventID;
        int mStepID;

        Event_compare mEventCompare;
        PQ mPQ;

        std::vector<Vertex_data_ptr> mVertexData;

        std::vector<SVert *> mReflexVertices;
        std::vector<SHalfedge *> mContourHalfedges;

        std::list<SVert *> mGLAV; // linked_active_vertices
    };
}