#include "PolyMesh/PolyMesh_Base.h"

namespace acamcad
{
	namespace polymesh
	{

		void MVert::adjustOutgoingHalfedge()
		{
			using namespace std;
			if (isIsolated())
				return;

			MHalfedge *begin = halfEdge();
			MHalfedge *he = begin;

			do
			{
				// cout << *he << endl;
				if (he->isBoundary())
				{
					// cout << "set halfedge" << endl;
					setHalfedge(he);
					return;
				}
				he = he->pair()->next();
			} while (he != begin);
		}

		int MPolyFace::updatePolyNum()
		{
			MHalfedge *he = he_begin_;
			polynum_ = 0;
			do
			{
				polynum_++;
				he = he->next();
			} while (he != he_begin_);
			return polynum_;
		}

		void MPolyFace::updataNormal()
		{
			MHalfedge *he = he_begin_;
			MVert *v0 = he->fromVertex();
			MVert *v1 = he->toVertex();
			MVector3 v01 = v1->position() - v0->position();
			he = he->prev();
			MVert *v2 = he->fromVertex();
			MVector3 v02 = v2->position() - v0->position();
			normal_ = cross(v01, v02);
			normal_.normalize();
		}

		void MPolyFace::update()
		{
			updatePolyNum();
			updataNormal();
		}

		MPoint3 MPolyFace::getFaceCenter()
		{
			assert(he_begin_ != nullptr);

			MHalfedge *he = he_begin_;

			MPoint3 getCenter(0, 0, 0);
			polynum_ = 0;
			do
			{
				MVert *fv = he->toVertex();
				getCenter += fv->position();
				polynum_++;
				he = he->next();
			} while (he != he_begin_);

			getCenter = MPoint3(getCenter.x() / polynum_, getCenter.y() / polynum_, getCenter.z() / polynum_);
			return getCenter;
		}

	} // namespace polymesh
} // namespace acamcad