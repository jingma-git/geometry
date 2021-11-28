#pragma once
#include <cmath>
#include <complex>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <typeinfo>

// #define DEBUG
#define INFO

#ifdef DEBUG
#define M_DEBUG(msg) std::cout << msg << std::endl
#else
#define M_DEBUG(msg)
#endif

#ifdef INFO
#define M_INFO(msg) std::cout << msg << std::endl
#else
#define M_INFO(msg)
#endif

// Error checking macro. CONDITION should be true if life is good (like in assert(CONDITION))
#ifdef NGC_SAFETY_CHECKS
#define GC_SAFETY_ASSERT(CONDITION, MSG)
#else
#define GC_SAFETY_ASSERT(CONDITION, MSG)                                                          \
    if (!(CONDITION))                                                                             \
    {                                                                                             \
        throw std::runtime_error("GC_SAFETY_ASSERT FAILURE from " + std::string(__FILE__) + ":" + \
                                 std::to_string(__LINE__) + " - " + (MSG));                       \
    }
#endif

namespace geometrycentral
{
    const size_t INVALID_IND = std::numeric_limits<size_t>::max();
    const double PI = 3.1415926535897932384;
}