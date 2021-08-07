#pragma once

#ifdef WITH_INLINE
#define M_INLINE inline
#else
#define M_INLINE
#endif

#ifndef M_DEBUG
#define M_DEBUG std::cout << __FILE__ << " " << __LINE__ << " "
#endif