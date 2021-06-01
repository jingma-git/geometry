#pragma once

namespace egl
{
    typedef Eigen::Vector2d Pnt2;
    typedef Eigen::Vector2d Vec2;

    enum Sign
    {
        NEGATIVE = -1,
        ZERO = 0,
        POSITIVE = 1,

        // Orientation constants:
        RIGHT_TURN = -1,
        LEFT_TURN = 1,

        CLOCKWISE = -1,
        COUNTERCLOCKWISE = 1,

        COLLINEAR = 0,
        COPLANAR = 0,
        DEGENERATE = 0,

        // Oriented_side constants:
        ON_NEGATIVE_SIDE = -1,
        ON_ORIENTED_BOUNDARY = 0,
        ON_POSITIVE_SIDE = 1,

        // Comparison_result constants:
        SMALLER = -1,
        EQUAL = 0,
        LARGER = 1
    };

    typedef Sign Orientation;
    typedef Sign Oriented_side;
    typedef Sign Comparison_result;

    inline Sign opposite(Sign o)
    {
        return static_cast<Sign>(-static_cast<int>(o));
    }
}
