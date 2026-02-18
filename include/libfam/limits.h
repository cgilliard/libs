/********************************************************************************
 * MIT License
 *
 * Copyright (c) 2025-2026 Christopher Gilliard
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

#ifndef _LIMITS_H
#define _LIMITS_H

#ifndef U8_MIN
#define U8_MIN ((u8)0x0)
#endif

#ifndef U16_MIN
#define U16_MIN ((u16)0x0)
#endif

#ifndef U32_MIN
#define U32_MIN ((u32)0x0)
#endif

#ifndef U64_MIN
#define U64_MIN ((u64)0x0)
#endif

#ifndef U128_MIN
#define U128_MIN ((u128)0x0)
#endif

#ifndef U8_MAX
#define U8_MAX ((u8)0xFF)
#endif

#ifndef U16_MAX
#define U16_MAX ((u16)0xFFFF)
#endif

#ifndef U32_MAX
#define U32_MAX ((u32)0xFFFFFFFF)
#endif

#ifndef U64_MAX
#define U64_MAX ((u64)0xFFFFFFFFFFFFFFFF)
#endif

#ifndef U128_MAX
#define U128_MAX (((u128)0xFFFFFFFFFFFFFFFFUL << 64) | 0xFFFFFFFFFFFFFFFFUL)
#endif

#ifndef I8_MIN
#define I8_MIN ((i8)(-0x7F - 1))
#endif

#ifndef I16_MIN
#define I16_MIN ((i16)(-0x7FFF - 1))
#endif

#ifndef I32_MIN
#define I32_MIN ((i32)(-0x7FFFFFFF - 1))
#endif

#ifndef I64_MIN
#define I64_MIN ((i64)(-0x7FFFFFFFFFFFFFFF - 1))
#endif

#ifndef I128_MIN
#define I128_MIN \
	((i128)(((u128)0x8000000000000000UL << 64) | 0x0000000000000000UL))
#endif

#ifndef I8_MAX
#define I8_MAX ((i8)0x7F)
#endif

#ifndef I16_MAX
#define I16_MAX ((i16)0x7FFF)
#endif

#ifndef I32_MAX
#define I32_MAX ((i32)0x7FFFFFFF)
#endif

#ifndef I64_MAX
#define I64_MAX ((i64)0x7FFFFFFFFFFFFFFF)
#endif

#ifndef I128_MAX
#define I128_MAX \
	((i128)(((i128)0x7FFFFFFFFFFFFFFFUL << 64) | 0xFFFFFFFFFFFFFFFFUL))
#endif

#endif /* _LIMITS_H */

#ifdef TEST
#include <libfam/test.h>

Test(limits) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Woverflow"
#pragma GCC diagnostic ignored "-Winteger-overflow"

	ASSERT(U8_MIN == 0, "U8_MIN should be 0");
	ASSERT(U16_MIN == 0, "U16_MIN should be 0");
	ASSERT(U32_MIN == 0, "U32_MIN should be 0");
	ASSERT(U64_MIN == 0, "U64_MIN should be 0");
	ASSERT(U128_MIN == 0, "U128_MIN should be 0");

	ASSERT(U8_MAX == 0xFF, "U8_MAX should be 255");
	ASSERT(U16_MAX == 0xFFFF, "U16_MAX should be 65535");
	ASSERT(U32_MAX == 0xFFFFFFFF, "U32_MAX should be 4294967295");
	ASSERT(U64_MAX == 0xFFFFFFFFFFFFFFFF,
	       "U64_MAX should be 18446744073709551615");
	u128 u128_max_expected =
	    ((u128)0xFFFFFFFFFFFFFFFFUL << 64) | 0xFFFFFFFFFFFFFFFFUL;
	ASSERT(U128_MAX == u128_max_expected, "U128_MAX incorrect");

	ASSERT(I8_MAX == 0x7F, "I8_MAX should be 127");
	ASSERT(I16_MAX == 0x7FFF, "I16_MAX should be 32767");
	ASSERT(I32_MAX == 0x7FFFFFFF, "I32_MAX should be 2147483647");
	ASSERT(I64_MAX == 0x7FFFFFFFFFFFFFFF,
	       "I64_MAX should be 9223372036854775807");

	i128 i128_max_expected =
	    ((i128)(((i128)0x7FFFFFFFFFFFFFFFUL << 64) | 0xFFFFFFFFFFFFFFFFUL));
	ASSERT(I128_MAX == i128_max_expected, "I128_MAX incorrect");

	ASSERT(I8_MIN == -128, "I8_MIN should be -128");
	ASSERT(I16_MIN == -32768, "I16_MIN should be -32768");
	ASSERT(I32_MIN == -2147483648, "I32_MIN should be -2147483648");
	ASSERT(I64_MIN == I64_MAX + 1,
	       "I64_MIN should be -9223372036854775808");
	i128 i128_min_expected =
	    ((i128)(((u128)0x8000000000000000UL << 64) | 0x0000000000000000UL));
	ASSERT(I128_MIN == i128_min_expected, "I128_MIN");

	ASSERT_EQ((u8)(U8_MAX + 1), U8_MIN, "overflow U8_MAX");
	ASSERT_EQ((u8)(U8_MIN - 1), U8_MAX, "underflow U8_MIN");
	ASSERT_EQ((u16)(U16_MAX + 1), U16_MIN, "overflow U16_MAX");
	ASSERT_EQ((u16)(U16_MIN - 1), U16_MAX, "underflow U16_MIN");
	ASSERT_EQ(U32_MAX + 1, U32_MIN, "overflow U32_MAX");
	ASSERT_EQ(U32_MIN - 1, U32_MAX, "underflow U32_MIN");
	ASSERT_EQ(U64_MAX + 1, U64_MIN, "overflow U64_MAX");
	ASSERT_EQ(U64_MIN - 1, U64_MAX, "underflow U64_MIN");
	ASSERT_EQ(U128_MAX + 1, U128_MIN, "overflow U128_MAX");
	ASSERT_EQ(U128_MIN - 1, U128_MAX, "underflow U128_MIN");
	ASSERT_EQ((i8)(I8_MAX + 1), I8_MIN, "overflow I8_MAX");
	ASSERT_EQ((i8)(I8_MIN - 1), I8_MAX, "underflow I8_MIN");
	ASSERT_EQ((i16)(I16_MAX + 1), I16_MIN, "overflow I16_MAX");
	ASSERT_EQ((i16)(I16_MIN - 1), I16_MAX, "underflow I16_MIN");
	ASSERT_EQ(I32_MAX + 1, I32_MIN, "overflow I32_MAX");
	ASSERT_EQ(I32_MIN - 1, I32_MAX, "underflow I32_MIN");
	ASSERT_EQ(I64_MAX + 1, I64_MIN, "overflow I64_MAX");
	ASSERT_EQ(I64_MIN - 1, I64_MAX, "underflow I64_MIN");
	ASSERT_EQ((i128)(I128_MAX + 1), I128_MIN, "overflow I128_MAX");
	ASSERT_EQ((i128)(I128_MIN - 1), I128_MAX, "underflow I128_MIN");
#pragma GCC diagnostic pop
}

#endif /* TEST */
