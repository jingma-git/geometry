// Copyright (c) 2010 GeometryFactory (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.3/Intersections_3/include/CGAL/Intersections_3/Triangle_3_Triangle_3.h $
// $Id: Triangle_3_Triangle_3.h 52164b1 2019-10-19T15:34:59+02:00 Sébastien Loriot
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Sebastien Loriot
//

#ifndef CGAL_INTERSECTIONS_3_TRIANGLE_3_TRIANGLE_3_H
#define CGAL_INTERSECTIONS_3_TRIANGLE_3_TRIANGLE_3_H

#include <CGAL/Triangle_3.h>

#include <CGAL/Intersections_3/internal/Triangle_3_Triangle_3_intersection.h>
#include <CGAL/Intersections_3/internal/Triangle_3_Triangle_3_do_intersect.h>

namespace CGAL {
CGAL_INTERSECTION_FUNCTION_SELF(Triangle_3, 3)
CGAL_DO_INTERSECT_FUNCTION_SELF(Triangle_3, 3)
}

#endif // CGAL_INTERSECTIONS_3_TRIANGLE_3_TRIANGLE_3_H
