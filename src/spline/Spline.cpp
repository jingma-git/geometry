#include "Spline.h"
#include <iostream>
namespace egl
{
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////

	static int comb(int x, int y)
	{
		int i, a, b;
		for (i = 2, a = 1; i <= x - y; i++)
			a *= i;
		for (i = y + 1, b = 1; i <= x; i++)
			b *= i;

		return b / a;
	}

	CSpline::CSpline()
	{
		m_bSplineGenerateMode = 0;
		ClearStatus();

		m_vA.clear();
		m_vB.clear();
		m_vC.clear();
		m_vK.clear();
		m_Mat[0].clear();
		m_Mat[1].clear();
		m_Mat[2].clear();

		m_bSplInitiated = false;
	}

	CSpline::CSpline(const CSpline &spl)
	{
		P = spl.P;
		m_mode = spl.m_mode;
		m_bSplineGenerateMode = spl.m_bSplineGenerateMode;
		m_vTangent = spl.m_vTangent;
		m_vTangentMagnitude0 = spl.m_vTangentMagnitude0;
		m_vTangentMagnitude1 = spl.m_vTangentMagnitude1;

		m_bSplineIsEdit = spl.m_bSplineIsEdit;
		m_nSelected = spl.m_nSelected;
		m_nMaped = spl.m_nMaped;
		m_nSelsizeline = spl.m_nSelsizeline;
		int num = P.size();

		m_vA.resize(num);
		m_vB.resize(num);
		m_vC.resize(num);

		m_vK.resize(num);
		m_Mat[0].resize(num);
		m_Mat[1].resize(num);
		m_Mat[2].resize(num);
		m_bSplInitiated = false;
	}

	CSpline::~CSpline()
	{
		ClearCtrlPoint();

		m_vA.clear();
		m_vB.clear();
		m_vC.clear();
		m_vK.clear();
		m_Mat[0].clear();
		m_Mat[1].clear();
		m_Mat[2].clear();
		m_bSplInitiated = false;
	}

	void CSpline::ChangeMode(SPLINE_MODE mode)
	{
		m_mode = mode;
		m_bSplInitiated = false;
	}

	void CSpline::ClearStatus()
	{
		m_bSplineIsEdit = false;
		m_nSelected = -1;
		m_nMaped = -1;
		m_nSelsizeline = -1;
		m_bSplInitiated = false;
	}

	////----------------------------------------------------------------------
	//// name:		GetPoint
	//// function:	Get point based on t
	//// argument:	int i, double t
	//// return:		Pnt3
	//// author:
	//// date:
	//// update:	    1) set tagent of control point
	////              2) generate point of spline based on tangent mode,i.e the tagent has been set directly
	////				Note: call GetPoint is not very effency, we'd better add a variable to save discrete point of spline
	////				and calculate this in class spline.
	//// author:		xinghsihai
	//// date:
	////----------------------------------------------------------------------
	//Vec CSpline::GetPoint(int i, double t)
	//{
	//	Pnt3			v;
	//	int				size = 0;
	//
	//	size = static_cast<int>(P.size());
	//
	//	//------------------------------------------------------------------
	////	if (this->m_vTangent.size() != this->P.size())
	////	{
	//	if(m_mode == SPLMODE_CLOSED_SPLINE)
	//	{
	//		m_vTangent.resize(size + 1);
	//		m_vTangentMagnitude0.resize(size + 1);
	//		m_vTangentMagnitude1.resize(size + 1);
	//	}
	//	else
	//	{
	//		this->m_vTangent.resize(size);
	//		m_vTangentMagnitude0.resize(size);
	//		m_vTangentMagnitude1.resize(size);
	//	}
	//
	//    if ((m_mode != SPLMODE_SPLINE && m_mode != SPLMODE_CLOSED_SPLINE))
	//	{
	//		this->m_bSplineGenerateMode = 0; //normal mode
	//	}
	//
	//	//----------------------------------------------------------------
	//	if (this->m_bSplineGenerateMode != 1) //normal mode
	//	{
	//		if (m_mode == SPLMODE_SPLINE)
	//		{
	//			if ((P[i+1] - P[i]).norm() < Math::Epsilon)
	//			{
	//				v = P[i];
	//			}
	//			else
	//			{
	//				Pnt3		Pd0, Pd1;
	//				if (P.size() > 2)
	//				{
	//					if (i == 0) 							Pd0 = (P[1] + (P[1] - (P[0]+P[2])*0.5f)*0.5f) - P[0];
	//					else									Pd0 = ((P[i] - P[i-1]) + (P[i+1] - P[i])) * 0.5f;
	//
	//					if ((i + 2) >= GetCtrlPointCount())		Pd1 = P[i+1]-(P[i]+(P[i] - (P[i-1] + P[i+1])*0.5f)*0.5f);
	//					else									Pd1 = ((P[i+1] - P[i]) + (P[i+2] - P[i+1])) * 0.5f;
	//
	//					m_vTangent[i]				= Pd0.Normalize(); //xing add, set tangent of control point i,
	//					m_vTangent[i+1]				= Pd1.Normalize(); //xing add, set tangent of control point i+1
	//					m_vTangentMagnitude0[i]		= Pd0.norm();
	//					m_vTangentMagnitude1[i]		= Pd0.norm();
	//					m_vTangentMagnitude0[i+1]	= Pd1.norm();
	//					m_vTangentMagnitude1[i+1]	= Pd1.norm();
	//
	//					v = ((t-1) * (t-1) * (2*t + 1)) * P[i] +
	//						(t * t * (3 - 2*t))         * P[i+1] +
	//						((1-t) * (1-t) * t)         * Pd0 +
	//						((t-1) * t * t)             * Pd1;
	//				} else {
	//					m_vTangent[i]				= (P[i+1] - P[i]).Normalize(); //xing add, set tangent of control point i,
	//					m_vTangent[i+1]				= (P[i+1] - P[i]).Normalize(); //xing add, set tangent of control point i+1
	//					m_vTangentMagnitude0[i]		= (P[i+1] - P[i]).norm();
	//					m_vTangentMagnitude1[i]		= (P[i+1] - P[i]).norm();
	//					m_vTangentMagnitude0[i+1]	= (P[i+1] - P[i]).norm();
	//					m_vTangentMagnitude1[i+1]	= (P[i+1] - P[i]).norm();
	//					v = (1.0f - t) * P[i] + t * P[i+1];
	//				}
	//			}
	//		}
	//		else if (m_mode == SPLMODE_CLOSED_SPLINE)
	//		{
	//			int pim1 = i-1;
	//			int pi   = i;
	//			int pip1 = i+1;
	//			int pip2 = i+2;
	//			if (pim1 < 0) pim1 = GetCtrlPointCount()-1;
	//			if (pi   >= GetCtrlPointCount())				pi   = pi  -GetCtrlPointCount();
	//			if (pip1 >= GetCtrlPointCount())				pip1 = pip1-GetCtrlPointCount();
	//			if (pip2 >= GetCtrlPointCount())				pip2 = pip2-GetCtrlPointCount();
	//
	//			if ((P[pip1] - P[pi]).norm() < Math::Epsilon)
	//			{
	//				v = P[i];
	//			}
	//			else
	//			{
	//				Vec Pd0, Pd1;
	//
	//				Pd0 = ((P[pi  ] - P[pim1]) + (P[pip1] - P[pi  ])) * 0.5f;
	//				Pd1 = ((P[pip1] - P[pi  ]) + (P[pip2] - P[pip1])) * 0.5f;
	//
	//				m_vTangent[i]				= Pd0.Normalize(); //xing add, set tangent of control point i
	//				m_vTangentMagnitude0[i]		= Pd0.norm();
	//				m_vTangentMagnitude1[i]		= Pd0.norm();
	//
	//				if(i <  GetCtrlPointCount())
	//				{
	//					m_vTangent[i+1]				= Pd1.Normalize(); //xing add, set tangent of control point i+1
	//					m_vTangentMagnitude0[i+1]	= Pd1.norm();
	//					m_vTangentMagnitude1[i+1]	= Pd1.norm();
	//				}
	//
	//				v = ((t-1) * (t-1) * (2*t + 1)) * P[pi] +
	//					(t * t * (3 - 2*t))         * P[pip1] +
	//					((1-t) * (1-t) * t)         * Pd0 +
	//					((t-1) * t * t)             * Pd1;
	//			}
	//		} else if (m_mode == SPLMODE_BSPLINE) {
	//			double f = 0.5f * ( 1.0f - t ) * ( 1.0f - t ) ;
	//			double g = ( 1.0f - t ) * t + 0.5f ;
	//			double h = 0.5f * t * t ;
	//			int index0 = i - 1;
	//			int index1 = i;
	//			int index2 = i + 1;
	//			v.x = P[index0].x * f + P[index1].x * g + P[index2].x * h ;
	//			v.y = P[index0].y * f + P[index1].y * g + P[index2].y * h ;
	//			v.z = P[index0].z * f + P[index1].z * g + P[index2].z * h ;
	//		} else if (m_mode == SPLMODE_CLOSED_BSPLINE) {
	//			double f = 0.5f * ( 1.0f - t ) * ( 1.0f - t ) ;
	//			double g = ( 1.0f - t ) * t + 0.5f ;
	//			double h = 0.5f * t * t ;
	//			int index0 = i - 1;
	//			int index1 = i;
	//			int index2 = i + 1;
	//			if (index0 < 0) index0 = GetCtrlPointCount()-1;
	//			if (index2 >= GetCtrlPointCount()) index2 = 0;
	//			v.x = P[index0].x * f + P[index1].x * g + P[index2].x * h ;
	//			v.y = P[index0].y * f + P[index1].y * g + P[index2].y * h ;
	//			v.z = P[index0].z * f + P[index1].z * g + P[index2].z * h ;
	//		} else if (m_mode == SPLMODE_BEZIER) {
	//			v.x = 0.0f;
	//			v.y = 0.0f;
	//			v.z = 0.0f;
	//			unsigned long n = GetCtrlPointCount() - 1;
	//			for(int j = 0; j < GetCtrlPointCount(); j++){
	//				v.x += double(comb(n, j) * pow(t, j) * pow((1 - t), (int)(n - j)) * P[j].x);	//
	//				v.y += double(comb(n, j) * pow(t, j) * pow((1 - t), (int)(n - j)) * P[j].y);
	//				v.z += double(comb(n, j) * pow(t, j) * pow((1 - t), (int)(n - j)) * P[j].z);
	//			}
	//		} else if (m_mode == SPLMODE_LINER) {
	//			v = (1.0f - t) * P[i] + t * P[i+1];
	//		}
	//	}
	//	else //tangent mode
	//	{
	//		if (P.size() > 2)
	//		{
	//			int pip1 = i+1;
	//			if (pip1 >= GetCtrlPointCount())				pip1 = pip1-GetCtrlPointCount();
	//			v = ((t-1) * (t-1) * (2*t + 1)) * P[i] +
	//				(t * t * (3 - 2*t))         * P[pip1] +
	//				((1-t) * (1-t) * t)         * m_vTangentMagnitude1[i]    * m_vTangent[i] +
	//				((t-1) * t * t)             * m_vTangentMagnitude0[pip1] * m_vTangent[pip1];
	//		}
	//		else
	//		{
	//			if (i < 1)
	//				v = (1.0f - t) * P[i] + t * P[i+1];
	//			//m_bSplineGenerateMode = 0;
	//		}
	//	}
	//	return v;
	//}

	//----------------------------------------------------------------------
	// name:		GetPoint
	// function:	Get point based on t, tension
	// argument:	int i, double t, double tension
	// return:		Pnt3
	// author:
	// date:
	// update:	    1) set tagent of control point
	//              2) generate point of spline based on tangent mode,i.e the tagent has been set directly
	//				Note: call GetPoint is not very effency, we'd better add a variable to save discrete point of spline
	//				and calculate this in class spline.
	// author:
	// date:
	//----------------------------------------------------------------------
	Pnt3 CSpline::GetPoint(int i, double t, double tension)
	{
		Pnt3 v;
		int size = 0;

		size = static_cast<int>(P.size());

		if (m_mode == SPLMODE_CLOSED_SPLINE)
		{
			m_vTangent.resize(size + 1);
			m_vTangentMagnitude0.resize(size + 1);
			m_vTangentMagnitude1.resize(size + 1);
		}
		else
		{
			this->m_vTangent.resize(size);
			m_vTangentMagnitude0.resize(size);
			m_vTangentMagnitude1.resize(size);
		}

		if ((m_mode != SPLMODE_SPLINE && m_mode != SPLMODE_CLOSED_SPLINE))
		{
			this->m_bSplineGenerateMode = 0; //normal mode
		}

		//----------------------------------------------------------------
		if (this->m_bSplineGenerateMode != 1) //normal mode
		{
			if (m_mode == SPLMODE_SPLINE)
			{
				v = GetPointA(i, t);
			}
			else if (m_mode == SPLMODE_CLOSED_SPLINE)
			{
				v = GetPointA(i, t);
			}
			else if (m_mode == SPLMODE_BSPLINE)
			{
				double f = 0.5f * (1.0f - t) * (1.0f - t);
				double g = (1.0f - t) * t + 0.5f;
				double h = 0.5f * t * t;
				int index0 = i - 1;
				int index1 = i;
				int index2 = i + 1;
				v = P[index0] * f + P[index1] * g + P[index2] * h;
			}
			else if (m_mode == SPLMODE_CLOSED_BSPLINE)
			{
				double f = 0.5f * (1.0f - t) * (1.0f - t);
				double g = (1.0f - t) * t + 0.5f;
				double h = 0.5f * t * t;
				int index0 = i - 1;
				int index1 = i;
				int index2 = i + 1;
				if (index0 < 0)
					index0 = GetCtrlPointCount() - 1;
				if (index2 >= GetCtrlPointCount())
					index2 = 0;
				v = P[index0] * f + P[index1] * g + P[index2] * h;
			}
			else if (m_mode == SPLMODE_BEZIER)
			{
				v.setZero();
				unsigned long n = GetCtrlPointCount() - 1;
				for (int j = 0; j < GetCtrlPointCount(); j++)
				{
					v += comb(n, j) * pow(t, j) * pow((1 - t), (int)(n - j)) * P[j];
				}
			}
			else if (m_mode == SPLMODE_LINER)
			{
				v = (1.0f - t) * P[i] + t * P[i + 1];
			}
		}
		else //tangent mode
		{
			if (P.size() > 2)
			{
				int pip1 = i + 1;
				if (pip1 >= GetCtrlPointCount())
					pip1 = pip1 - GetCtrlPointCount();

				v = ((t - 1) * (t - 1) * (2 * t + 1)) * P[i] +
					(t * t * (3 - 2 * t)) * P[pip1] +
					((1 - t) * (1 - t) * t) * m_vTangentMagnitude1[i] * m_vTangent[i] +
					((t - 1) * t * t) * m_vTangentMagnitude0[pip1] * m_vTangent[pip1];
			}
			else
			{
				if (i < 1)
					v = (1.0f - t) * P[i] + t * P[i + 1];
			}
		}
		return v;
	}

	//----------------------------------------------------------------------
	// name:		AddCtrlPoint
	// function:	add ctrl point of spline
	// argument:	v: the point will be added
	// return:		void
	// author:		unknown
	// date:		unknown
	// update:
	// author:		VIEW_TOY
	// date:		04/20/2007
	//----------------------------------------------------------------------
	void CSpline::AddCtrlPoint(const Pnt3 &v)
	{
		P.push_back(v);
		m_bSplInitiated = false;
	}

	void CSpline::SetCtrlPoint(int index, const Pnt3 &v)
	{
		GetCtrlPoint(index) = v;
		m_bSplInitiated = false;
	}

	//----------------------------------------------------------------------
	// name:		DelCtrlPoint
	// function:	delete control point
	// argument:	index: the index of the point which will be deleted
	// return:		void
	// author:		unknown
	// date:		unknown
	// update:
	// author:
	// date:		04/20/2007
	//----------------------------------------------------------------------
	void CSpline::DelCtrlPoint(int index)
	{
		int i;
		int cnt = 0;
		std::vector<Pnt3> tmp = P;
		int size = GetCtrlPointCount();

		if (size - 1 > 0)
		{
			P.resize(size - 1);

			for (i = 0; i < size; i++)
			{
				if (i != index)
				{
					P[cnt] = tmp[i];
					cnt++;
				}
			}
		}
		else
		{
			ClearCtrlPoint();
		}

		//-------------------------------------------

		std::vector<Pnt3> tangent;
		std::vector<double> m0;
		std::vector<double> m1;

		tangent = m_vTangent;
		m0 = m_vTangentMagnitude0;
		m1 = m_vTangentMagnitude1;
		cnt = 0;
		size = static_cast<int>(m_vTangent.size());

		if (size > 1 && m_bSplineGenerateMode)
		{
			m_vTangent.resize(size - 1);
			m_vTangentMagnitude0.resize(size - 1);
			m_vTangentMagnitude1.resize(size - 1);

			for (i = 0; i < size; i++)
			{
				if (i != index)
				{
					m_vTangent[cnt] = tangent[i];
					m_vTangentMagnitude0[cnt] = m0[i];
					m_vTangentMagnitude1[cnt] = m1[i];
					cnt++;
				}
			}
		}
		m_bSplInitiated = false;
	}

	//---------------------------------------------------------------
	// Name:
	// Description:
	// Argument:
	//         :
	// Return:
	// Author:
	// Date:
	// Modified by:
	// Updated date: 2007/10/24
	//----------------------------------------------------------------
	void CSpline::InsertCtrlPoint(int idx, Pnt3 vt)
	{
		SPLINE_MODE iMode = GetMode();
		if (iMode == CSpline::SPLMODE_CLOSED_BEZIER || iMode == CSpline::SPLMODE_CLOSED_BSPLINE || iMode == CSpline::SPLMODE_CLOSED_SPLINE)
		{
			P.push_back(P.back());
		}
		if (idx < 0 || idx > size() - 2)
		{
			return;
		}
		int i = 0;
		std::vector<Pnt3> tmp = P;
		P.clear();
		for (i = 0; i <= idx; i++)
		{
			P.push_back(tmp[i]);
		}
		P.push_back(vt);
		for (i = idx + 1; i < static_cast<int>(tmp.size()); i++)
		{
			P.push_back(tmp[i]);
		}

		//------ followes code should be checked if we want to use tangent line to control spline shape
		std::vector<Pnt3> tangent;
		std::vector<double> m0;
		std::vector<double> m1;

		tangent = m_vTangent;
		m0 = m_vTangentMagnitude0;
		m1 = m_vTangentMagnitude1;

		if (m_vTangent.size() > 1 && m_bSplineGenerateMode)
		{
			m_vTangent.clear();
			m_vTangentMagnitude0.clear();
			m_vTangentMagnitude1.clear();

			for (i = 0; i < idx; i++)
			{
				m_vTangent.push_back(tangent[i]);
				m_vTangentMagnitude0.push_back(m0[i]);
				m_vTangentMagnitude1.push_back(m1[i]);
			}

			Pnt3 vtTan = ((vt - tmp[idx]) + (tmp[idx + 1] - vt)) / 2;
			vtTan.normalize();

			double fm0 = (tmp[idx + 1] - tmp[idx]).norm() / 2;
			double fm1 = fm0;
			AddTangent(vtTan, fm0, fm1);

			for (i = idx + 1; i < static_cast<int>(tangent.size()); i++)
			{
				m_vTangent.push_back(tangent[i]);
				m_vTangentMagnitude0.push_back(m0[i]);
				m_vTangentMagnitude1.push_back(m1[i]);
			}
		}
		if (iMode == CSpline::SPLMODE_CLOSED_BEZIER || iMode == CSpline::SPLMODE_CLOSED_BSPLINE || iMode == CSpline::SPLMODE_CLOSED_SPLINE)
		{
			P.pop_back();
		}
		m_bSplInitiated = false;
	}

	void CSpline::ClearCtrlPoint()
	{
		P.clear();
		//-----------------------------------

		m_vTangent.clear();
		m_vTangentMagnitude0.clear();
		m_vTangentMagnitude1.clear();
		//----------------------------------
		m_bSplInitiated = false;
	}

	Pnt3 CSpline::GetPointByLength(double len)
	{
		double sumlen = 0;
		Pnt3 r;

		if (len >= 0)
		{
			Pnt3 prevp = GetCtrlPoint(0);
			r = prevp;

			for (int i = 1; i < GetCtrlPointCount() * 2; i++)
			{
				int p = i;
				if (p >= GetCtrlPointCount())
					p -= GetCtrlPointCount();

				Pnt3 &v = GetCtrlPoint(p);
				double l = (v - prevp).norm();

				if (sumlen + l < len)
				{
					sumlen += l;
					r = v;
				}
				else
				{
					int pm1 = p - 1;
					if (pm1 < 0)
						pm1 = GetCtrlPointCount() - 1;
					double t = (len - sumlen) / l;
					r = GetCtrlPoint(pm1) * (1 - t) + GetCtrlPoint(p) * t;
					break;
				}
				prevp = v;
			}
		}
		else
		{
			int p = 0;
			Pnt3 prevp = GetCtrlPoint(0);
			r = prevp;
			len = -len;

			for (int i = 0; i < GetCtrlPointCount() - 1; i++)
			{
				p--;
				if (p < 0)
					p = GetCtrlPointCount() - 1;

				Pnt3 &v = GetCtrlPoint(p);
				double l = (v - prevp).norm();

				if (sumlen + l < len)
				{
					sumlen += l;
					r = v;
				}
				else
				{
					int pp1 = p + 1;
					if (pp1 >= GetCtrlPointCount())
						pp1 = 0;
					double t = (len - sumlen) / l;
					r = GetCtrlPoint(pp1) * (1 - t) + GetCtrlPoint(p) * t;
					break;
				}
				prevp = v;
			}
		}

		return r;
	}

	//,2007.1.24
	bool CSpline::GetPointByLength(double len, Pnt3 &vt)
	{
		//-------------------Ye Xinghui modified 2007-03-31---------------
		//if(len<=0){
		//	return false;
		//}
		if (len < 0)
		{
			return false;
		}
		if (len == 0)
		{
			vt = GetCtrlPoint(0);
			return true;
		}
		//-------------------end modify------------------
		int size = GetCtrlPointCount() - 1;
		if (GetMode() == CSpline::SPLMODE_CLOSED_SPLINE)
		{
			size++;
		}
		double sumlen = 0.0;
		Pnt3 prevp = p(0);
		Pnt3 pt;
		bool bIsPtFind = false;
		for (int i = 0; i < size; i++)
		{
			for (double t = 0; t < 1.01f; t += 0.1f)
			{
				pt = GetPoint(i, t);
				sumlen += (pt - prevp).norm();
				if (sumlen > len)
				{
					double scl = (sumlen - len) / (pt - prevp).norm();
					vt = prevp * scl + pt * (1 - scl);
					bIsPtFind = true;
					break;
				}
				prevp = pt;
			}
			if (bIsPtFind)
			{
				break;
			}
		}
		return bIsPtFind;
	}

	double CSpline::GetLength(int n)
	{
		double sumlen = 0;
		Pnt3 prevp = GetCtrlPoint(0);
		for (int i = 1; i <= n; i++)
		{
			int p = i;
			if (p >= GetCtrlPointCount())
				p -= GetCtrlPointCount();
			Pnt3 &v = GetCtrlPoint(p);
			sumlen += (v - prevp).norm();
			prevp = v;
			//_dump << sumlen << " ";
		}
		//_dump << "\n";

		return sumlen;
	}

	// ,2007.1.13
	double CSpline::GetSplLenBetweenTwoPts(int iFirPtIdx, int iSecPtIdx)
	{
		int i;
		double t, len = 0.0;
		Pnt3 p;
		Pnt3 plist[1000];
		int size = GetCtrlPointCount() - 1;

		int np = 0;
		if (GetMode() == CSpline::SPLMODE_CLOSED_SPLINE)
		{
			size++;
		}
		if (iFirPtIdx < 0)
		{
			iFirPtIdx = 0;
		}
		if (iSecPtIdx > size)
		{
			iSecPtIdx = size;
		}
		if (iFirPtIdx > iSecPtIdx)
		{
			int iTemp = iFirPtIdx;
			iFirPtIdx = iSecPtIdx;
			iSecPtIdx = iTemp;
		}
		for (i = iFirPtIdx; i < iSecPtIdx; i++)
		{
			for (t = 0; t < 1.01f; t += 0.1f)
			{
				p = GetPoint(i, t);
				plist[np] = p;

				if (np > 0)
				{
					if ((p - plist[np - 1]).norm() > 0.001)
						np++;
				}
				else
					np++;
			}
		}
		for (i = 0; i < np - 1; i++)
		{
			len += (plist[i + 1] - plist[i]).norm();
		}
		return len;
	}

	CSpline &CSpline::operator=(const CSpline &src)
	{
		ClearCtrlPoint();
		for (int i = 0; i < src.GetCtrlPointCount(); i++)
		{
			AddCtrlPoint(src.GetCtrlPoint(i));
		}
		m_mode = src.GetMode();

		m_bSplineIsEdit = src.m_bSplineIsEdit;
		m_nSelected = src.m_nSelected;
		m_nMaped = src.m_nMaped;
		m_nSelsizeline = src.m_nSelsizeline;
		//--------------------------------
		m_bSplineGenerateMode = src.m_bSplineGenerateMode;
		for (int i = 0; i < src.GetTangentCount(); i++)
		{
			this->m_vTangent.push_back(src.GetTangentOfCtrlPnt(i));
			this->m_vTangentMagnitude0.push_back(src.GetMagnitudeOfTangent(i, 0));
			this->m_vTangentMagnitude1.push_back(src.GetMagnitudeOfTangent(i, 1));
		}
		//--------------------------------
		return (*this);
	}

	//----------------------------------------------------------------------
	// name:		SetSplineGenerateMd
	// function:	set spline generate mode, it is only for spline
	//						0:normal mode;
	//						1:set the tangent of the control point directly for spline
	// argument:	int iMode: spline generate mode
	// return:		void
	//----------------------------------------------------------------------
	void CSpline::SetSplineGenerateMd(int iMode)
	{
		this->m_bSplineGenerateMode = iMode;
		m_bSplInitiated = false;
	}

	//----------------------------------------------------------------------
	// name:		GetSplineGenerateMd
	// function:	Get spline generate mode, it is only for spline
	//						0:normal mode;
	//						1:set the tangent of the control point directly for spline
	// argument:	void
	// return:		int
	//----------------------------------------------------------------------
	int CSpline::GetSplineGenerateMd(void) const
	{
		return this->m_bSplineGenerateMode;
	}

	//----------------------------------------------------------------------
	// name:		SetMagnitudeOfTangent
	// function:	set the magnitude of control point
	// argument:	int i: control point index
	//				int j: 0: set magnitude0, 1: set magnitude1
	//				double magnitude:
	// return:		void
	//----------------------------------------------------------------------
	void CSpline::SetMagnitudeOfTangent(int i, int j, double fMagnitude)
	{
		if (j == 0)
		{
			m_vTangentMagnitude0[i] = fMagnitude;
		}
		else
		{
			m_vTangentMagnitude1[i] = fMagnitude;
		}
		m_bSplInitiated = false;
	}

	//----------------------------------------------------------------------
	// name:		GetMagnitudeOfTangent
	// function:	Get the magnitude of control point
	// argument:	int i: control point index
	//				int j: 0: set magnitude0, 1: set magnitude1
	// return:		double:  magnitude value
	//----------------------------------------------------------------------
	double CSpline::GetMagnitudeOfTangent(int i, int j) const
	{
		if (j == 0)
		{
			return m_vTangentMagnitude0[i];
		}
		else
		{
			return m_vTangentMagnitude1[i];
		}
	}

	//----------------------------------------------------------------------
	// name:		AddTangent
	// function:	when insert a control point, we need to add its tangent
	// argument:	vTangent: normalize vector of tangent
	//				fMagnitude0: magnitude0
	// return:		fMagnitude1: magnitude1
	//----------------------------------------------------------------------
	void CSpline::AddTangent(Pnt3 vTangent, double fMagnitude0, double fMagnitude1)
	{
		int size = static_cast<int>(P.size());
		if (size > 0)
		{
			m_vTangent.resize(size);
			m_vTangentMagnitude0.resize(size);
			m_vTangentMagnitude1.resize(size);

			m_vTangent[size - 1] = vTangent;
			m_vTangentMagnitude0[size - 1] = fMagnitude0;
			m_vTangentMagnitude1[size - 1] = fMagnitude1;
		}
		m_bSplInitiated = false;
	}

	void CSpline::Generate()
	{
		int i;

		int num = P.size();

		m_vA.resize(num);
		m_vB.resize(num);
		m_vC.resize(num);

		m_vK.resize(num);
		m_Mat[0].resize(num);
		m_Mat[1].resize(num);
		m_Mat[2].resize(num);

		double AMag, AMagOld;

		// vector A
		for (i = 0; i <= (num - 2); i++)
		{
			m_vA[i] = P[i + 1] - P[i];
		}

		AMagOld = m_vA[0].norm();

		if (AMagOld < 0.0001)
			AMagOld = 0.0001;

		for (i = 0; i <= (num - 3); i++)
		{
			AMag = m_vA[i + 1].norm();

			if (AMag < 0.0001)
				AMag = 0.0001;

			m_vK[i] = AMagOld / AMag;
			AMagOld = AMag;
		}
		m_vK[num - 2] = 1.0f;

		// Matrix
		for (i = 1; i <= num - 2; i++)
		{
			m_Mat[0][i] = 1.0f;
			m_Mat[1][i] = 2.0f * m_vK[i - 1] * (1.0f + m_vK[i - 1]);
			m_Mat[2][i] = m_vK[i - 1] * m_vK[i - 1] * m_vK[i];
		}

		m_Mat[1][0] = 2.0f;
		m_Mat[2][0] = m_vK[0];
		m_Mat[0][num - 1] = 1.0f;
		m_Mat[1][num - 1] = 2.0f * m_vK[num - 2];

		for (i = 1; i <= num - 2; i++)
		{
			m_vB[i] = 3.0f * (m_vA[i - 1] + m_vK[i - 1] * m_vK[i - 1] * m_vA[i]);
		}

		m_vB[0] = 3.0f * m_vA[0];
		m_vB[num - 1] = 3.0f * m_vA[num - 2];

		MatrixSolve(m_vB);

		for (i = 0; i <= num - 2; i++)
		{
			m_vC[i] = m_vK[i] * m_vB[i + 1];
		}

		m_bSplInitiated = true;

		return;
	}

	void CSpline::MatrixSolve(std::vector<Pnt3> &vB)
	{
		int i, j;

		int num = P.size();

		std::vector<Pnt3> Work, WorkB;
		Work.resize(num);
		WorkB.resize(num);

		for (i = 0; i < num; i++)
		{
			if (fabs(m_Mat[1][i]) < 0.0001)
			{
				if (m_Mat[1][i] < 0)
					m_Mat[1][i] = -0.0001;
				else
					m_Mat[1][i] = 0.0001;
			}
		}

		for (i = 0; i <= num - 1; i++)
		{
			Work[i] = vB[i] / m_Mat[1][i];
			WorkB[i] = Work[i];
		}

		for (j = 0; j < 10; j++)
		{ ///  need convergence judge
			Work[0] = (vB[0] - m_Mat[2][0] * WorkB[1]) / m_Mat[1][0];
			for (int i = 1; i < num - 1; i++)
			{
				Work[i] = (vB[i] - m_Mat[0][i] * WorkB[i - 1] - m_Mat[2][i] * WorkB[i + 1]) / m_Mat[1][i];
			}
			Work[num - 1] = (vB[num - 1] - m_Mat[0][num - 1] * WorkB[num - 2]) / m_Mat[1][num - 1];

			for (i = 0; i <= num - 1; i++)
			{
				WorkB[i] = Work[i];
			}
		}

		for (i = 0; i <= num - 1; i++)
		{
			vB[i] = Work[i];
		}

		return;
	}

	// closed cubic spline ////////////////////
	void CSpline::GenerateClosedSpline()
	{
		int i;
		double AMag, AMagOld, AMag0;

		int num = P.size();

		m_vA.resize(num);
		m_vB.resize(num);
		m_vC.resize(num);

		m_vK.resize(num);
		m_Mat[0].resize(num);
		m_Mat[1].resize(num);
		m_Mat[2].resize(num);

		// vector A
		for (int i = 0; i <= num - 2; i++)
		{
			m_vA[i] = P[i + 1] - P[i];
		}

		m_vA[num - 1] = P[0] - P[num - 1];

		// k
		AMag0 = AMagOld = m_vA[0].norm();

		if (AMag0 < 0.0001)
			AMag0 = 0.0001;

		for (i = 0; i <= num - 2; i++)
		{
			AMag = m_vA[i + 1].norm();

			if (AMag < 0.0001)
				AMag = 0.0001;

			m_vK[i] = AMagOld / AMag;
			AMagOld = AMag;
		}
		m_vK[num - 1] = AMagOld / AMag0;

		// Matrix
		for (i = 1; i <= num - 1; i++)
		{
			m_Mat[0][i] = 1.0f;
			m_Mat[1][i] = 2.0f * m_vK[i - 1] * (1.0f + m_vK[i - 1]);
			m_Mat[2][i] = m_vK[i - 1] * m_vK[i - 1] * m_vK[i];
		}
		m_Mat[0][0] = 1.0f;
		m_Mat[1][0] = 2.0f * m_vK[num - 1] * (1.0f + m_vK[num - 1]);
		m_Mat[2][0] = m_vK[num - 1] * m_vK[num - 1] * m_vK[0];

		for (i = 1; i <= num - 1; i++)
		{
			m_vB[i] = 3.0f * (m_vA[i - 1] + m_vK[i - 1] * m_vK[i - 1] * m_vA[i]);
		}

		m_vB[0] = 3.0f * (m_vA[num - 1] + m_vK[num - 1] * m_vK[num - 1] * m_vA[0]);

		MatrixSolveEX(m_vB);

		for (i = 0; i <= num - 2; i++)
		{
			m_vC[i] = m_vK[i] * m_vB[i + 1];
		}

		m_vC[num - 1] = m_vK[num - 1] * m_vB[0];

		m_bSplInitiated = true;

		return;
	}

	// tridiagonal matrix + elements of [0][0], [N-1][N-1] ////
	void CSpline::MatrixSolveEX(std::vector<Pnt3> &vB)
	{
		int i, j;

		std::vector<Pnt3> Work, WorkB;

		int num = P.size();

		Work.resize(num);
		WorkB.resize(num);

		for (i = 0; i < num; i++)
		{
			if (fabs(m_Mat[1][i]) < 0.0001)
			{
				if (m_Mat[1][i] < 0)
					m_Mat[1][i] = -0.0001;
				else
					m_Mat[1][i] = 0.0001;
			}
		}

		for (i = 0; i <= num - 1; i++)
		{
			Work[i] = vB[i] / m_Mat[1][i];
			WorkB[i] = Work[i];
		}

		for (j = 0; j < 10; j++)
		{ // need judge of convergence
			Work[0] = (vB[0] - m_Mat[0][0] * WorkB[num - 1] - m_Mat[2][0] * WorkB[1]) / m_Mat[1][0];

			for (int i = 1; i < num - 1; i++)
			{
				Work[i] = (vB[i] - m_Mat[0][i] * WorkB[i - 1] - m_Mat[2][i] * WorkB[i + 1]) / m_Mat[1][i];
			}

			Work[num - 1] = (vB[num - 1] - m_Mat[0][num - 1] * WorkB[num - 2] - m_Mat[2][num - 1] * WorkB[0]) / m_Mat[1][num - 1];

			for (i = 0; i <= num - 1; i++)
			{
				WorkB[i] = Work[i];
			}
		}

		for (i = 0; i <= num - 1; i++)
		{
			vB[i] = Work[i];
		}

		return;
	}

	Pnt3 CSpline::GetPointA(int i, double t)
	{
		if (m_bSplInitiated == false)
		{
			if (m_mode == SPLMODE_SPLINE)
				Generate();
			else if (m_mode == SPLMODE_CLOSED_SPLINE)
				GenerateClosedSpline();
		}

		Pnt3 pt;
		double f, g, h;

		f = t * t * (3.0f - 2.0f * t);
		g = t * (t - 1.0f) * (t - 1.0f);
		h = t * t * (t - 1.0f);

		pt = P[i] + m_vA[i] * f + m_vB[i] * g + m_vC[i] * h;

		return pt;
	}

	void ConvertSplineToPolyLines(CSpline &spline, std::vector<std::vector<Pnt3>> &sketchPolyLines)
	{
		sketchPolyLines.clear();
		int cptCount = spline.GetCtrlPointCount();

		if (cptCount < 2)
			return;
		spline.m_bSplInitiated = false;

		int i, j;
		if (cptCount == 2)
		{
			std::vector<Pnt3> polyline;
			polyline.push_back(spline.GetCtrlPoint(0));
			polyline.push_back(spline.GetCtrlPoint(1));
			sketchPolyLines.push_back(polyline);
		}
		else
		{
			int clineCount = cptCount - 1;
			if (spline.GetMode() == spline.SPLMODE_CLOSED_SPLINE)
			{
				clineCount = cptCount;
			}
			double Len = 0.0;
			for (i = 0; i < clineCount; i++)
			{
				Len += (spline.GetCtrlPoint(i) - spline.GetCtrlPoint((i + 1) % cptCount)).norm();
			}
			for (i = 0; i < clineCount; i++)
			{

				double clineLength = (spline.GetCtrlPoint(i) - spline.GetCtrlPoint((i + 1) % cptCount)).norm();
				int divCount = (int)(clineLength / Len * clineCount * 10 + 3); // insert more points

				// ------------------------------------------compute tension-------------------------------------------------------
				double curLength = (spline.GetCtrlPoint(i) - spline.GetCtrlPoint((i + 1) % cptCount)).norm();
				double ratio = 1.0;
				// ------------------tension 1------------------------
				// if length_raitio is larger, the tension is smaller
				// compared with next segment
				if (i < clineCount - 1)
				{
					double nextLength = (spline.GetCtrlPoint((i + 1) % cptCount) - spline.GetCtrlPoint((i + 2) % cptCount)).norm();
					double ratioTemp = curLength / nextLength;
					if (ratioTemp < 0.9)
					{
						ratio = ratioTemp;
					}
				}
				else
				{
					if (spline.GetMode() == spline.SPLMODE_CLOSED_SPLINE)
					{
						double nextLength = (spline.GetCtrlPoint((i + 1) % cptCount) - spline.GetCtrlPoint((i + 2) % cptCount)).norm();
						double ratioTemp = curLength / nextLength;
						if (ratioTemp < 0.9)
						{
							ratio = ratioTemp;
						}
					}
				}

				// compared with previous segment
				if (i > 0)
				{
					double preLength = (spline.GetCtrlPoint(i - 1) - spline.GetCtrlPoint(i)).norm();
					double ratioTemp = curLength / preLength;
					if (ratioTemp < 0.9 && ratioTemp < ratio)
					{
						ratio = ratioTemp;
					}
				}
				else
				{
					if (spline.GetMode() == spline.SPLMODE_CLOSED_SPLINE)
					{
						double preLength = (spline.GetCtrlPoint(cptCount - 1) - spline.GetCtrlPoint(0)).norm();
						double ratioTemp = curLength / preLength;
						if (ratioTemp < 0.9 && ratioTemp < ratio)
						{
							ratio = ratioTemp;
						}
					}
				}

				double tension1 = 0.0;
				tension1 = 1 - sqrt(sin(0.5 * M_PI * ratio));
				// ------------------tension 2------------------------
				double ang = M_PI * 176. / 180.;
				// compared with next angle
				if (i < clineCount - 1)
				{
					double angTemp = GetAngleOf2Vector(spline.GetCtrlPoint(i) - spline.GetCtrlPoint(i + 1),
													   spline.GetCtrlPoint((i + 2) % cptCount) - spline.GetCtrlPoint(i + 1));
					if (angTemp < ang)
					{
						ang = angTemp;
					}
				}
				else
				{
					if (spline.GetMode() == spline.SPLMODE_CLOSED_SPLINE)
					{
						double angTemp = GetAngleOf2Vector(spline.GetCtrlPoint(cptCount - 1) - spline.GetCtrlPoint(0),
														   spline.GetCtrlPoint(1) - spline.GetCtrlPoint(0));
						if (angTemp < ang)
						{
							ang = angTemp;
						}
					}
				}
				// compared with previous angle
				if (i > 0)
				{
					double angTemp = GetAngleOf2Vector(spline.GetCtrlPoint(i - 1) - spline.GetCtrlPoint(i),
													   spline.GetCtrlPoint((i + 1) % cptCount) - spline.GetCtrlPoint(i));
					if (angTemp < ang)
					{
						ang = angTemp;
					}
				}
				else
				{
					if (spline.GetMode() == spline.SPLMODE_CLOSED_SPLINE)
					{
						double angTemp = GetAngleOf2Vector(spline.GetCtrlPoint(cptCount - 1) - spline.GetCtrlPoint(0),
														   spline.GetCtrlPoint(1) - spline.GetCtrlPoint(0));
						if (angTemp < ang)
						{
							ang = angTemp;
						}
					}
				}
				// if angle is larger, the tension is smaller
				double tension2 = 0.0;
				tension2 = 1 - sqrt(sin(0.5 * ang));

				double tension = 0.0;
				tension = tension1 > tension2 ? tension1 : tension2;
				std::vector<Pnt3> polyline;
				// cout << __FILE__ << " " << __LINE__ << " divCount=" << divCount << " tension=" << tension << endl;
				for (j = 0; j <= divCount; j++)
				{

					polyline.push_back(spline.GetPoint(i, j / static_cast<double>(divCount), tension));
				}
				sketchPolyLines.push_back(polyline);
			}
		}
	}

	void GetPolyLine(std::vector<std::vector<Pnt3>> &sketchPolyLines, std::vector<Pnt3> &polyline)
	{
		polyline.clear();
		polyline.push_back(sketchPolyLines.front().front());
		int n = sketchPolyLines.size();
		for (int i = 0; i < n; i++)
		{
			std::vector<Pnt3> &polylineTmp = sketchPolyLines[i];
			for (int j = 1; j < polylineTmp.size(); j++)
			{
				if ((polylineTmp[j] - polyline.back()).norm() > 1.0e-4)
				{
					polyline.push_back(polylineTmp[j]);
				}
			}
		}
	}

	// a.dot(b) = |a||b|cos(theta)
	double GetAngleOf2Vector(const Vec3 &v1, const Vec3 &v2)
	{
		double len1, len2, r, val, cos, angle;
		len1 = sqrt(v1.x() * v1.x() + v1.y() * v1.y() + v1.z() * v1.z());
		len2 = sqrt(v2.x() * v2.x() + v2.y() * v2.y() + v2.z() * v2.z());
		r = v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();

		val = len1 * len2;
		if (fabs(val) < 1.0e-6)
		{
			angle = 0;
			return M_PI / 2.0f;
		}
		else
		{
			cos = r / val;
			if (cos > 1.0)
			{
				return 0.0f;
			}
			else if (cos < -1.0)
			{
				return M_PI;
			}
			angle = acos(cos);
		}
		return angle;
	}
} // namespace egl