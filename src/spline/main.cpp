#include "Spline.h"
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace egl;
using namespace std;
int main()
{
    CSpline spline;
    spline.ChangeMode(CSpline::SPLINE_MODE::SPLMODE_SPLINE);
    spline.AddCtrlPoint(Pnt3(10, 10, 0));
    spline.AddCtrlPoint(Pnt3(30, -10, 0));
    spline.AddCtrlPoint(Pnt3(50, 10, 0));
    spline.AddCtrlPoint(Pnt3(70, -10, 0));
    spline.AddCtrlPoint(Pnt3(90, 10, 0));
    spline.AddCtrlPoint(Pnt3(110, -10, 0));

    std::vector<std::vector<Pnt3>> polylines;
    ConvertSplineToPolyLines(spline, polylines);
    std::vector<Pnt3> polyline;
    GetPolyLine(polylines, polyline);
    cv::Mat img(500, 500, CV_8UC3, cv::Scalar(255, 255, 255));

    for (int i = 0; i < polyline.size() - 1; ++i)
    {
        cout << i << ": " << polyline[i].transpose() << endl;
        const Pnt3 &v0 = polyline[i];
        const Pnt3 &v1 = polyline[i + 1];
        cv::Point p0(v0.x(), v0.y() + 250);
        cv::Point p1(v1.x(), v1.y() + 250);
        cv::circle(img, p0, 3, cv::Scalar(0, 0, 255), -1);
        cv::line(img, p0, p1, cv::Scalar(0, 0, 0), 2);
    }
    const Pnt3 &v = polyline.back();
    cv::circle(img, cv::Point(v.x(), v.y() + 250), 3, cv::Scalar(0, 0, 255), -1);
    cv::imwrite("src/spline/spline.png", img);
    return 0;
}