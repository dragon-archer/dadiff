#ifndef _DADIFF_CONFIG_H_
#define _DADIFF_CONFIG_H_

#include "pch.h"

#ifdef _MSC_VER
	#define DA_MSVC _MSC_VER
#else // TODO: Add more test
	#define DA_GCC __GNUC__
#endif

#ifdef __BYTE_ORDER__
	#define DA_BIG_ENDIAN	 (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
	#define DA_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
	#define DA_PDP_ENDIAN	 (__BYTE_ORDER__ == __ORDER_PDP_ENDIAN__)
#else // TODO: Use other method to detect endian instead of assuming little endian
	#define DA_BIG_ENDIAN	 0
	#define DA_LITTLE_ENDIAN 1
	#define DA_PDP_ENDIAN	 0
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

#define DADIFF_VERSION_MAJOR 0
#define DADIFF_VERSION_MINOR 1
#define DADIFF_VERSION		 (DADIFF_VERSION_MAJOR * 10 + DADIFF_VERSION_MINOR)

/// DADIFF_MAGIC: ASCII of "DADF" in little endian
#if DA_LITTLE_ENDIAN
	#define DADIFF_MAGIC 0x46444144
#elif DA_BIG_ENDIAN
	#define DADIFF_MAGIC 0x44414446
#elif DA_PDP_ENDIAN
	#define DADIFF_MAGIC 0x41444644
#endif

#define DADIFF_ESCAPE_CHAR 0x80

#endif // _DADIFF_CONFIG_H_
