#include "core.h"
#include <iostream>
using namespace egl;

void test_colinear()
{
    Pnt2 p(0, 0), q(0, 2), r(0, 2);
    assert(collinear(p, q, r) == true);

    Segment_2 e0(Pnt2(0, 0), Pnt2(1, 0));
    Segment_2 e1(Pnt2(0, 1), Pnt2(1, 1));
    Segment_2 e2(Pnt2(2, 0), Pnt2(4, 0));
    assert(are_edges_collinear(e0, e1) == false);
    assert(are_edges_collinear(e0, e2) == true);
    assert(are_edges_collinear(e0, e0.opposite()) == true);
    assert(are_edges_orderly_collinear(e0, e0.opposite()) == false);
}

void test_parallel()
{
    Segment_2 e0(Pnt2(0, 0), Pnt2(1, 0));
    Segment_2 e1(Pnt2(0, 1), Pnt2(1, 1));
    assert(are_edges_parallel(e0, e1) == true);
    assert(are_edges_parallel(e0, e0.opposite()) == true);
}

void test_construct_line()
{
    Segment_2 e0(Pnt2(0, 0), Pnt2(1, 0));
    double a, b, c;
    line_from_point(e0.source().x(), e0.source().y(),
                    e0.target().x(), e0.target().y(),
                    a, b, c);

    Segment_2 e1(Pnt2(0, 1), Pnt2(1, 2));
    line_from_point(e1.source().x(), e1.source().y(),
                    e1.target().x(), e1.target().y(),
                    a, b, c);
    assert(is_equal(a, -1.0) && is_equal(b, 1.0) && is_equal(c, -1.0));
}

void test_line_point_facing()
{
    Segment_2 e0(Pnt2(0, 0), Pnt2(1, 0));
    Segment_2 e1(Pnt2(1, 0), Pnt2(1, 1));
    Segment_2 e2(Pnt2(1, 1), Pnt2(0, 1));
    Segment_2 e3(Pnt2(1, 2), Pnt2(0, 2));
    Segment_2 e4(Pnt2(1, 0), Pnt2(0, 0));
    Segment_2 e5(Pnt2(-1, 1), Pnt2(-1, 0));
    Segment_2 e6(Pnt2(0, 0), Pnt2(1, 1));
    assert(is_edge_facing_point(Pnt2(0, 1), e0) == true);
    assert(is_edge_facing_point(Pnt2(0, 1), e1) == true);
    assert(is_edge_facing_point(Pnt2(0, 1), e2) == false); // point pass line
    assert(is_edge_facing_point(Pnt2(0, 1), e3) == true);
    assert(is_edge_facing_point(Pnt2(0, 1), e4) == false); // point pass line
    assert(is_edge_facing_point(Pnt2(0, 1), e5) == true);
    assert(is_edge_facing_point(Pnt2(0, 1), e6) == true);
    assert(is_edge_facing_point(Pnt2(0, 1), e6.opposite()) == false);
}

void test_triseg_isect()
{
    // split
    Segment_2 e0(Pnt2(10, 20), Pnt2(0, 10));
    Segment_2 e1(Pnt2(0, 10), Pnt2(-10, 20));
    Segment_2 e2(Pnt2(-10, -10), Pnt2(10, -10));
    Trisegment_2_ptr tri(new Trisegment_2(e0, e1, e2, TRISEGMENT_COLLINEARITY_NONE));

    boost::optional<double> t = compute_offset_lines_isec_timeC2(tri);
    if (t)
    {
        std::cout << *t << std::endl; //ToDO: why 8.28427
    }

    boost::optional<Pnt2> p = construct_offset_lines_isecC2(tri);
    if (p)
    {
        std::cout << "isect point=" << (*p).transpose() << std::endl;
    }

    // collinear
    Segment_2 e00(Pnt2(10, 10), Pnt2(0, 10));
    Segment_2 e11(Pnt2(0, 10), Pnt2(-10, 10));
    Segment_2 e22(Pnt2(-10, -10), Pnt2(10, -10));
    Trisegment_2_ptr tri1(new Trisegment_2(e00, e11, e22, TRISEGMENT_COLLINEARITY_01));

    boost::optional<double> t1 = compute_offset_lines_isec_timeC2(tri1);
    if (t1)
    {
        std::cout << *t1 << std::endl; //ToDO: why 8.28427
    }

    boost::optional<Pnt2> p1 = construct_offset_lines_isecC2(tri1);
    if (p1)
    {
        std::cout << "isect point1=" << (*p1).transpose() << std::endl;
    }
}

struct item : public In_place_list_base<item>
{
    int key;
    item() {}
    item(const item &i) : In_place_list_base<item>(i), key(i.key) {}
    item(int i) : key(i) {}
    bool operator==(const item &i) const { return key == i.key; }
    bool operator!=(const item &i) const { return !(*this == i); }
    bool operator==(int i) const { return key == i; }
    bool operator!=(int i) const { return !(*this == i); }
    bool operator<(const item &i) const { return key < i.key; }
};

void test_inplace_list()
{
    typedef In_place_list<item, false> List;
    List l;
    item *p = new item(0);
    l.push_back(*p);
    l.push_back(*new item(1));
    l.push_front(*new item(2));
    l.push_front(*new item(3));
    l.push_front(*new item(4));
    std::cout << "size=" << l.size() << std::endl;
    for (List::iterator it = l.begin(); it != l.end(); ++it)
    {
        std::cout << it->key << " ";
    }
    std::cout << std::endl;
    l.erase(l.begin());
    std::cout << "after erase" << std::endl;
    std::cout << "size=" << l.size() << std::endl;
    for (List::iterator it = l.begin(); it != l.end(); ++it)
    {
        std::cout << it->key << " ";
    }
    std::cout << std::endl;
}

struct TestSmartPtr : Ref_counted_base
{
};
void test_smart_ptr()
{
    TestSmartPtr *vp = new TestSmartPtr;
    // boost::intrusive_ptr<TestSmartPtr> vptr(vp);
    // boost::intrusive_ptr<TestSmartPtr> vptr1(vp);
    // boost::intrusive_ptr<TestSmartPtr> vptr2 = vptr1;
    // std::cout << vptr->RefCount() << std::endl;
    // std::cout << vptr1->RefCount() << std::endl;

    // pitfalls of shared_ptr, if use two shared_ptrs manage the same raw_ptr
    // the resources will be released twice
    boost::shared_ptr<TestSmartPtr> vptr(vp);
    boost::shared_ptr<TestSmartPtr> vptr1(vp);
    std::cout << vptr.use_count() << std::endl;
    std::cout << vptr1.use_count() << std::endl;
}

void test_sskel()
{
    typedef Pnt2 Point;
    std::vector<Pnt2> poly;
    poly.push_back(Point(10, -10));
    poly.push_back(Point(10, 50));
    poly.push_back(Point(0, 40));
    poly.push_back(Point(-10, 50));
    poly.push_back(Point(-10, 10));
    poly.push_back(Point(-50, 10));
    poly.push_back(Point(-50, -10));

    Straight_skeleton_builder_2 sskel_builder;
    sskel_builder.enter_contour(poly);
    sskel_builder.Run();
}

int main()
{
    // test_colinear();
    // test_parallel();
    // test_construct_line();
    // test_line_point_facing();
    // test_triseg_isect();
    // test_inplace_list();
    // test_smart_ptr();
    test_sskel();
    return 0;
}