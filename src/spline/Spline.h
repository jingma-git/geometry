// Spline.h:
//
//////////////////////////////////////////////////////////////////////

#ifndef _SPLINE_H__INCLUDED_
#define _SPLINE_H__INCLUDED_

#include <Eigen/Eigen>

namespace egl
{
	typedef Eigen::Vector3d Pnt3;
	typedef Eigen::Vector3d Vec3;

	class CSpline
	{
	public:
		bool m_bSplineIsEdit;
		int m_nSelected;
		int m_nMaped;
		int m_nSelsizeline;

	protected:
		int m_bSplineGenerateMode; //set spline generate mode: 0:normal mode;
		//							1:set the tangent of the control point directly for spline
		//							2:seam boundary
		std::vector<Pnt3> m_vTangent;			  //arry for tangent
		std::vector<double> m_vTangentMagnitude0; //the tangent of each control point i have two magnitude,
		//							magnitude0 influrence spline (i-1, i)
		std::vector<double> m_vTangentMagnitude1; //							magnitude0 influrence spline (i, i+1)

	public:
		//For to edit spline by change tangent of control point, add those two function
		Pnt3 GetTangentOfCtrlPnt(int i) { return this->m_vTangent[i]; }
		Pnt3 GetTangentOfCtrlPnt(int i) const { return this->m_vTangent[i]; }
		void SetTangentOfCtrlPnt(Pnt3 &vTangent, int i) { this->m_vTangent[i] = vTangent; }
		void SetSplineGenerateMd(int iMode);
		int GetSplineGenerateMd(void) const;
		int GetTangentCount() const { return static_cast<int>(m_vTangent.size()); }
		void SetMagnitudeOfTangent(int i, int j, double fMagnitude); //get the magnitude of control point
		double GetMagnitudeOfTangent(int i, int j) const;
		void AddTangent(Pnt3 vTangent, double fMagnitude0, double fMagnitude1);

		void SetCtrlPointArr(const std::vector<Pnt3> &ptsArr) { P = ptsArr; }
		std::vector<Pnt3> &GetCtrlPointArr() { return P; }
		void ClearStatus();

	public:
		enum SPLINE_MODE
		{
			SPLMODE_LINER = 0,
			SPLMODE_SPLINE,
			SPLMODE_BSPLINE,
			SPLMODE_BEZIER,
			SPLMODE_CLOSED_SPLINE,
			SPLMODE_CLOSED_BSPLINE,
			SPLMODE_CLOSED_BEZIER,
		};

	public:
		CSpline();
		CSpline(const CSpline &spl);

		virtual ~CSpline();

		void ChangeMode(SPLINE_MODE mode);

		SPLINE_MODE GetMode() const { return m_mode; }

		void AddCtrlPoint(const Pnt3 &v);

		void DelCtrlPoint(int index);

		void InsertCtrlPoint(int idx, Pnt3 vt);

		void SetCtrlPoint(int index, const Pnt3 &v);

		void ClearCtrlPoint();

		Pnt3 &GetCtrlPoint(int i) { return P[i]; }

		const Pnt3 &GetCtrlPoint(int i) const { return P[i]; }

		int GetCtrlPointCount() const { return static_cast<int>(P.size()); }

		void resize(int sz) { P.resize(sz); }

		int size() const { return static_cast<int>(P.size()); }

		Pnt3 &p(int i) { return P[i]; }

		const Pnt3 &p(int i) const { return P[i]; }
		Pnt3 &pBack() { return P.back(); }
		const Pnt3 &pBack() const { return P.back(); }

		//Pnt3 GetPoint(int i, double t);

		//get point by cardinal type  , 0.0<t<1.0, 0.0<tension<1.0
		Pnt3 GetPoint(int i, double t, double tension = 0.0);

		Pnt3 GetPointByLength(double len);

		double GetLength(int n);

		bool GetPointByLength(double len, Pnt3 &vt);
		double GetSplLenBetweenTwoPts(int iFirPtIdx, int iSecPtIdx);
		CSpline &operator=(const CSpline &src);

	protected:
		std::vector<Pnt3> P;
		SPLINE_MODE m_mode;

		//###>>>
	public: // For generating a good shape of a b-spline
		std::vector<Pnt3> m_vA;
		std::vector<Pnt3> m_vB;
		std::vector<Pnt3> m_vC;
		std::vector<double> m_vK;
		std::vector<double> m_Mat[3];

		bool m_bSplInitiated;

	public:
		void Generate();

		void MatrixSolve(std::vector<Pnt3> &vB);

		// closed cubic spline ////////////////////
		void GenerateClosedSpline();

		// tridiagonal matrix + elements of [0][0], [N-1][N-1] ////
		void MatrixSolveEX(std::vector<Pnt3> &vB);
		Pnt3 GetPointA(int i, double t);
	};

	double GetAngleOf2Vector(const Vec3 &v1, const Vec3 &v2);
	void ConvertSplineToPolyLines(CSpline &spline, std::vector<std::vector<Pnt3>> &sketchPolyLines);
	void GetPolyLine(std::vector<std::vector<Pnt3>> &sketchPolyLines, std::vector<Pnt3> &polyline);
}

#endif
