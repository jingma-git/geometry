#include "voronoi.h"
#include "halfplane.h"
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;

namespace hp = Halfplane_intersect;
typedef hp::Pnt2 Pnt2;

void gen_rnd_pnts(std::vector<Pnt2> &pnts, int n, int w, int h)
{
    pnts.resize(n);
    int num_pts = w * h;
    std::unordered_map<int, bool> is_visit;
    int i = 0;
    while (i < n)
    {
        int idx = rand() % num_pts;
        if (!is_visit[idx])
        {
            int y = idx / w;
            int x = idx - w * y;
            pnts[i] = Pnt2(x, h - 1 - y);
            is_visit[idx] = true;
            cout << i << ": " << pnts[i].transpose() << endl;
            ++i;
        }
    }
}

void draw_pnts(cv::Mat &img, const std::vector<Pnt2> &pnts)
{
    for (auto it = pnts.begin(); it != pnts.end(); ++it)
        cv::circle(img, cv::Point(it->x(), it->y()), 3, cv::Scalar(0, 0, 255), -1);
}

void draw_cells(cv::Mat &img, const std::vector<std::vector<Pnt2>> &cells)
{
    int n = static_cast<int>(cells.size());
    std::vector<std::vector<cv::Point>> contours;
    contours.resize(n);
    for (int i = 0; i < n; ++i)
    {
        contours[i].resize(cells[i].size());
        for (int j = 0; j < static_cast<int>(cells[i].size()); ++j)
        {
            const Pnt2 &p = cells[i][j];
            contours[i][j] = cv::Point(p.x(), p.y());
        }
    }

    for (int i = 0; i < n; ++i)
    {
        cv::Scalar color(rand() % 255, rand() % 255, rand() % 255);
        cv::drawContours(img, contours, i, color, 1);
    }
}

int main()
{
    int n = 10;
    int w = 500;
    int h = 500;
    cv::Mat img(h, w, CV_8UC3, cv::Scalar(255, 255, 255));
    std::vector<Pnt2> pnts;
    gen_rnd_pnts(pnts, n, w, h);

    std::vector<std::vector<Pnt2>> cells;
    hp::hp_voro(pnts, cells, w, h);
    // Voro::HalfedgeDS voro;
    // Voro::halfplane_voro(pnts, voro);
    draw_cells(img, cells);
    draw_pnts(img, pnts);
    cv::imwrite("voro.jpg", img);
    return 0;
}