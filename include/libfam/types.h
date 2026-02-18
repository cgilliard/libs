/********************************************************************************
 * MIT License
 *
 * Copyright (c) 2026 Christopher Gilliard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/

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
