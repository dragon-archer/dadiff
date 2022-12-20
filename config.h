#ifndef _DADIFF_CONFIG_H_
#define _DADIFF_CONFIG_H_

#include "pch.h"

#ifdef _MSC_VER
	#define DA_MSVC _MSC_VER
#else // TODO: Add more test
	#define DA_GCC __GNUC__
#endif

#define DADIFF_BEGIN \
	namespace da {   \
		namespace diff {
#define DADIFF_END \
	}              \
	}
#define DADIFF ::da::diff

#define DA_STRINGIFY_(x) #x
#define DA_STRINGIFY(x)	 DA_STRINGIFY_(x)

#define DA_FAIL(msg)                                          \
	do {                                                      \
		std::cerr << __FILE__ ":" DA_STRINGIFY(__LINE__) ": " \
				  << msg;                                     \
		return {};                                            \
	} while(false)

#define DA_VERIFY(expr)                               \
	do {                                              \
		if(!bool(expr)) {                             \
			DA_FAIL("DA_VERIFY(" #expr ") failed\n"); \
		}                                             \
	} while(false)

/// DA_UNREACHABLE
#if DA_GCC
	#define DA_UNREACHABLE() __builtin_unreachable()
#elif DA_MSVC
	#define DA_UNREACHABLE() __assume(0)
#endif

#endif // _DADIFF_CONFIG_H_
