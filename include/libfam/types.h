#ifndef _TYPES_H
#define _TYPES_H

#ifndef NULL
#define NULL ((void *)0)
#endif /* NULL */

typedef signed char i8;
typedef short int i16;
typedef int i32;
typedef long i64;
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long u64;
typedef __uint128_t u128;
typedef __int128_t i128;
typedef double f64;

#ifndef bool
#define bool u8
#endif

#ifndef false
#define false (bool)0
#endif

#ifndef true
#define true (bool)1
#endif

#endif /* _TYPES_H */

#ifdef TYPES_IMPL
#ifndef TYPES_IMPL_GUARD
#define TYPES_IMPL_GUARD

#include <libfam/utils.h>

STATIC_ASSERT(sizeof(u8) == 1, u8_sizes_match);
STATIC_ASSERT(sizeof(i8) == 1, i8_sizes_match);
STATIC_ASSERT(sizeof(u16) == 2, u16_sizes_match);
STATIC_ASSERT(sizeof(i16) == 2, i16_sizes_match);
STATIC_ASSERT(sizeof(u32) == 4, u32_sizes_match);
STATIC_ASSERT(sizeof(i32) == 4, i32_sizes_match);
STATIC_ASSERT(sizeof(u64) == 8, u64_sizes_match);
STATIC_ASSERT(sizeof(i64) == 8, i64_sizes_match);
STATIC_ASSERT(sizeof(u128) == 16, u128_sizes_match);
STATIC_ASSERT(sizeof(i128) == 16, i128_sizes_match);
STATIC_ASSERT(sizeof(f64) == 8, f64_sizes_match);
STATIC_ASSERT(sizeof(void *) == 8, os_64_bit);
STATIC_ASSERT(__BYTE_ORDER__ == 1234, little_endian);

#endif /* TYPES_IMPL_GUARD */
#endif /* TYPES_IMPL */
