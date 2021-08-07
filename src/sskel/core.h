#pragma once

#include <iostream>
using namespace std;

#include <list>
#include <memory>
#include <queue>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <Eigen/Eigen>

#include "macros.h"
#include "typedefs.h"
#include "ref_count_base.h"

#include "dbl.h"
#include "geom_2.h"
#include "in_place_list.h"
#include "sskel_aux.h"
#include "sskel_cons.h" // construction: trisegment
#include "sskel_pred.h" // predicates: intersection_time
#include "sskel_items.h"
#include "sskel_events.h" // edge, split, vertex
#include "sskel_builder.h"