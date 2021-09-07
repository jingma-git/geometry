#include <egl/cotmatrix.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <iostream>
#include <unordered_map>
#include <random>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef K::Point_2 Point;
typedef Delaunay::Vertex_handle Vertex_handle;
typedef Delaunay::Face_handle Face_handle;

typedef Eigen::Matrix<double, -1, -1> matd_t;
typedef Eigen::Matrix<size_t, -1, -1> mati_t;

using namespace Eigen;
using namespace std;

void generate_mesh(const std::string strategy,
                   const size_t pts_num,
                   matd_t &nods,
                   mati_t &tris)
{
    const size_t sq_num = static_cast<size_t>(std::sqrt(pts_num));

    std::vector<Point> pts;
    if (strategy == "random")
    {
        std::mt19937 gen;
        std::uniform_real_distribution<double> dis(0.0, 1.0);

        pts.push_back(Point(0, 0));
        pts.push_back(Point(1, 0));
        pts.push_back(Point(1, 1));
        pts.push_back(Point(0, 1));

        //-> sample others
        size_t curr_size = pts.size();
        for (size_t i = curr_size; i < pts_num; ++i)
        {

            double x = dis(gen), y = dis(gen);
            pts.emplace_back(Point(x, y));
        }
    }
    else if (strategy == "regular")
    {
        double dx = 1.0 / double(sq_num - 1);
        for (size_t i = 0; i < sq_num; ++i)
            for (size_t j = 0; j < sq_num; ++j)
                pts.push_back(Point(i * dx, j * dx));
    }

    std::unordered_map<Vertex_handle, size_t> v2i;
    Delaunay dt;
    dt.insert(pts.begin(), pts.end());

    int count = 0;
    nods.resize(2, pts.size());
    for (Vertex_handle v : dt.finite_vertex_handles())
    {
        Point p = v->point();
        v2i[v] = count;
        nods(0, count) = p.x();
        nods(1, count) = p.y();
        ++count;
    }
    cout << "verts=" << nods.cols() << endl;
    // cout << nods.transpose() << endl;

    cout << "faces=" << dt.number_of_faces() << endl;
    tris.resize(3, dt.number_of_faces());
    count = 0;
    for (Face_handle f : dt.finite_face_handles())
    {
        tris(0, count) = v2i[f->vertex(0)];
        tris(1, count) = v2i[f->vertex(1)];
        tris(2, count) = v2i[f->vertex(2)];
        // cout << "f" << count << ": " << tris.col(count).transpose() << endl;
        ++count;
    }
}

void gradient(const matd_t &nods, const mati_t &tris, SparseMatrix<double> &G_)
{
    // return G: #dim x #tris by #nods:
    //      nod0, nod1, ... , nodn
    //Tri0_x
    //Tri0_y
    //Tri1_x
    //Tri1_y
    //...
    //Trin_x
    //Trin_y
    assert(tris.rows() == 3 && nods.rows() == 2);

    matd_t ref = matd_t::Zero(2, 3);
    ref(0, 1) = ref(1, 2) = 1;
    matd_t Dm = ref.rightCols(2) - ref.leftCols(2);

    Dm = Dm.inverse();

    matd_t B = matd_t::Zero(3, 2);
    B(0, 0) = B(0, 1) = -1;
    B(1, 0) = B(2, 1) = 1;

    G_.resize(2 * tris.cols(), nods.cols()); //#Dim*#faces, #verts
    {
        vector<Triplet<double>> trips;
        for (size_t i = 0; i < tris.cols(); ++i)
        {
            matd_t Ds(2, 2);
            Ds.col(0) = nods.col(tris(1, i)) - nods.col(tris(0, i));
            Ds.col(1) = nods.col(tris(2, i)) - nods.col(tris(1, i));
            matd_t F = Ds * Dm;
            F = F.inverse();

            matd_t g = (B * F).transpose(); // 2x3
            for (size_t p = 0; p < g.rows(); ++p)
            {
                for (size_t q = 0; q < g.cols(); ++q)
                {
                    trips.emplace_back(Triplet<double>(2 * i + p, tris(q, i), g(p, q)));
                }
            }
        }
        G_.setFromTriplets(trips.begin(), trips.end());
    }
}

void laplace(const matd_t &nods, const mati_t &tris, SparseMatrix<double> &L)
{
    const int dim = nods.rows();
    const size_t num_ele = tris.cols();
    SparseMatrix<double> A, G; // area, gradient
    A.reserve(num_ele * dim);

    matd_t ref = matd_t::Zero(2, 3);
    ref(0, 1) = ref(1, 2) = 1;
    matd_t Dm = ref.rightCols(2) - ref.leftCols(2);

    Dm = Dm.inverse();

    matd_t B = matd_t::Zero(3, 2);
    B(0, 0) = B(0, 1) = -1;
    B(1, 0) = B(2, 1) = 1;

    G.resize(2 * tris.cols(), nods.cols()); //#Dim*#faces, #verts
    A.resize(2 * tris.cols(), 2 * tris.cols());
    {
        vector<Triplet<double>> trips, atrips;
        for (size_t i = 0; i < tris.cols(); ++i)
        {
            matd_t Ds(2, 2);
            Ds.col(0) = nods.col(tris(1, i)) - nods.col(tris(0, i));
            Ds.col(1) = nods.col(tris(2, i)) - nods.col(tris(1, i));
            matd_t F = Ds * Dm;
            F = F.inverse();

            matd_t g = (B * F).transpose(); // 2x3
            for (size_t p = 0; p < g.rows(); ++p)
            {
                for (size_t q = 0; q < g.cols(); ++q)
                {
                    trips.emplace_back(Triplet<double>(2 * i + p, tris(q, i), g(p, q)));
                }
            }
            double area = 0.5 * Ds.determinant();
            atrips.emplace_back(2 * i, 2 * i, area);
            atrips.emplace_back(2 * i + 1, 2 * i + 1, area);
        }
        G.setFromTriplets(trips.begin(), trips.end());
        A.setFromTriplets(atrips.begin(), atrips.end());
    }
    L = G.transpose() * A * G;
}

int main()
{
    SparseMatrix<double> G, L;
    matd_t V;
    mati_t F;
    generate_mesh("random", 10201, V, F); //9, 121, 10201
    laplace(V, F, L);                     //semi-positive
    // cout << L.toDense() << endl;

    SparseMatrix<double> Lap; // semi-negative
    matd_t VV = matd_t::Zero(3, V.cols());
    VV.topRows(2) = V;
    egl::cotmatrix(VV, F, Lap);
    // cout << "Lap\n"
    //      << Lap.toDense() << endl;
    double err = (-Lap - L).norm();
    cout << "err:" << err << endl;
    return 0;
}