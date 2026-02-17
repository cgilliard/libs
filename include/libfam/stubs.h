#ifndef _STUBS_H
#define _STUBS_H

#endif /* _STUBS_H */

#ifdef STUBS_IMPL
#ifndef STUBS_IMPL_GUARD
#define STUBS_IMPL_GUARD

#include <libfam/debug.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/types.h>
#include <libfam/utils.h>

u128 __udivti3(u128 a, u128 b);
u128 __umodti3(u128 a, u128 b);
void __stack_chk_fail(void);

typedef union {
	u128 all;
	struct {
		u64 low;
		u64 high;
	} s;
} utwords;

static inline u64 udiv128by64to64(u64 u1, u64 u0, u64 v, u64 *r) {
	const unsigned n_udword_bits = sizeof(u64) * 8;
	const u64 b = (1ULL << (n_udword_bits / 2));
	u64 un1, un0;
	u64 vn1, vn0;
	u64 q1, q0;
	u64 un64, un21, un10;
	u64 rhat;
	i32 s;

	s = __builtin_clzll(v);
	if (s > 0) {
		v = v << s;
		un64 = (u1 << s) | (u0 >> (n_udword_bits - s));
		un10 = u0 << s;
	} else {
		un64 = u1;
		un10 = u0;
	}

	vn1 = v >> (n_udword_bits / 2);
	vn0 = v & 0xFFFFFFFF;

	un1 = un10 >> (n_udword_bits / 2);
	un0 = un10 & 0xFFFFFFFF;

	q1 = un64 / vn1;
	rhat = un64 - q1 * vn1;

	while (q1 >= b || q1 * vn0 > b * rhat + un1) {
		q1 = q1 - 1;
		rhat = rhat + vn1;
		if (rhat >= b) break;
	}

	un21 = un64 * b + un1 - q1 * v;

	q0 = un21 / vn1;
	rhat = un21 - q0 * vn1;

	while (q0 >= b || q0 * vn0 > b * rhat + un0) {
		q0 = q0 - 1;
		rhat = rhat + vn1;
		if (rhat >= b) break;
	}

	*r = (un21 * b + un0 - q0 * v) >> s;
	return q1 * b + q0;
}

static u128 __udivmodti4(u128 a, u128 b, u128 *rem) {
	const unsigned n_utword_bits = sizeof(u128) * 8;
	utwords dividend;
	dividend.all = a;
	utwords divisor;
	divisor.all = b;
	utwords quotient;
	utwords remainder;
	if (divisor.all > dividend.all) {
		if (rem) *rem = dividend.all;
		return 0;
	}
	if (divisor.s.high == 0) {
		remainder.s.high = 0;
		if (dividend.s.high < divisor.s.low) {
			quotient.s.low =
			    udiv128by64to64(dividend.s.high, dividend.s.low,
					    divisor.s.low, &remainder.s.low);
			quotient.s.high = 0;
		} else {
			quotient.s.high = dividend.s.high / divisor.s.low;
			dividend.s.high = dividend.s.high % divisor.s.low;
			quotient.s.low =
			    udiv128by64to64(dividend.s.high, dividend.s.low,
					    divisor.s.low, &remainder.s.low);
		}
		if (rem) *rem = remainder.all;
		return quotient.all;
	}
	i32 shift =
	    __builtin_clzll(divisor.s.high) - __builtin_clzll(dividend.s.high);
	divisor.all <<= shift;
	quotient.s.high = 0;
	quotient.s.low = 0;
	for (; shift >= 0; --shift) {
		quotient.s.low <<= 1;
		const i128 s = (i128)(divisor.all - dividend.all - 1) >>
			       (n_utword_bits - 1);
		quotient.s.low |= s & 1;
		dividend.all -= divisor.all & s;
		divisor.all >>= 1;
	}
	if (rem) *rem = dividend.all;
	return quotient.all;
}

PUBLIC void __stack_chk_fail(void) {
	pwrite(2, "Stack check fail!", 17, -1);
	exit_group(-1);
}

PUBLIC u128 __umodti3(u128 a, u128 b) {
	u128 r;
	__udivmodti4(a, b, &r);
	return r;
}

PUBLIC u128 __udivti3(u128 a, u128 b) { return __udivmodti4(a, b, 0); }

/* GCOVR_EXCL_START */
#ifdef TEST
#include <libfam/limits.h>
#include <libfam/test.h>

u128 __umodti3(u128 a, u128 b);
u128 __udivti3(u128 a, u128 b);

Test(stubs) {
	u128 v1 = (u128)111 << 77;
	u128 v2 = (u128)333 << 77;
	ASSERT_EQ(__umodti3(v2, v1), 0, "umod0");

	v1 = 1;
	v2 = (u128)U64_MAX + 1;
	ASSERT_EQ(__umodti3(v2, v1), 0, "umod1");

	ASSERT_EQ(__udivti3(100, 7), 14, "div_small1");
	ASSERT_EQ(__umodti3(100, 7), 2, "mod_small1");

	ASSERT_EQ(__udivti3(123456789ULL, 12345), 10000, "div_small2");
	ASSERT_EQ(__umodti3(123456789ULL, 12345), 6789, "mod_small2");

	ASSERT_EQ(__udivti3(0xFFFFFFFFFFFFFFFFULL, 1), 0xFFFFFFFFFFFFFFFFULL,
		  "div_by_1");
	ASSERT_EQ(__umodti3(0xFFFFFFFFFFFFFFFFULL, 1), 0, "mod_by_1");

	ASSERT_EQ(__udivti3(0, 42), 0, "div_zero");
	ASSERT_EQ(__umodti3(0, 42), 0, "mod_zero");

	u128 max = (u128)~0ULL;
	ASSERT_EQ(__udivti3(max, max), 1, "div_max_max");
	ASSERT_EQ(__umodti3(max, max), 0, "mod_max_max");

	ASSERT_EQ(__udivti3(max, 1), max, "div_max_1");
	ASSERT_EQ(__umodti3(max, 1), 0, "mod_max_1");

	u128 pow2_64 = (u128)1 << 64;
	ASSERT_EQ(__udivti3(pow2_64, (u128)1 << 32), (u128)1 << 32,
		  "div_pow2_1");
	ASSERT_EQ(__umodti3(pow2_64, (u128)1 << 32), 0, "mod_pow2_1");

	ASSERT_EQ(__udivti3(max, (u128)1 << 70), max >> 70, "div_max_pow2");
	ASSERT_EQ(__umodti3(max, (u128)1 << 70), max & (((u128)1 << 70) - 1),
		  "mod_max_pow2");

	u128 a = ((u128)1 << 70) + 0x123456789ABCDEF0ULL;
	u128 b = (u128)0xFEDCBA9876543210ULL;
	u128 expected_q = a / b;
	u128 expected_r = a % b;
	ASSERT_EQ(__udivti3(a, b), expected_q, "div_high_bits");
	ASSERT_EQ(__umodti3(a, b), expected_r, "mod_high_bits");
	u128 big_divisor = ((u128)1 << 64) + 12345;
	u128 multiple = big_divisor * 1000;
	ASSERT_EQ(__udivti3(multiple, big_divisor), 1000,
		  "div_big_divisor_exact");
	ASSERT_EQ(__umodti3(multiple, big_divisor), 0, "mod_big_divisor_exact");

	ASSERT_EQ(__umodti3(1000, 999), 1, "mod_large_remainder");
	ASSERT_EQ(__udivti3(1000, 999), 1, "div_large_remainder");

	ASSERT_EQ(__udivti3(7, 8), 0, "div_small_divisor_larger");
	ASSERT_EQ(__umodti3(7, 8), 7, "mod_small_divisor_larger");
}

Test(stubs2) {
	u128 a = (u128)0xFFFFFFFFFFFFFFFF << 64 | 0xFFFFFFFFFFFFFFFF;
	u128 b = (u128)0x8000000000000000ULL;
	u128 c;
	u128 x = a % b;
	ASSERT_EQ(x, 9223372036854775807, "9223372036854775807");
	a = (u128)0xFFFFFFFFFFFFFFFF << 64 | 0xFFFFFFFFFFFFFFFF;
	b = (u128)0xFFFFFFFFFFFFFFFFULL;
	x = a % b;
	ASSERT(!x, "x=0");
	a = (u128)0x0000000100000000 << 64 | 0xFFFFFFFFFFFFFFFF;
	b = (u128)0x0000000100000001ULL;
	x = a % b;
	ASSERT_EQ(x, 4294967296, "x=4294967296");
	a = ((u128)0xFFFFFFFF00000000ULL << 64) | 0xFFFFFFFFFFFFFFFFULL;
	b = 0xFFFFFFFF80000000ULL;
	x = a % b;
	ASSERT_EQ(x, 13835058055282163711ULL, "x=13835058055282163711");

	a = 12345;
	b = 123;
	c = a / b;
	ASSERT_EQ(c, 100, "100");
	a = ((u128)0x1) << 70;
	b = 1;
	c = a / b;
	ASSERT_EQ(c, a, "c=a");

	a = 1;
	b = ((u128)0x1) << 70;
	c = a / b;
	ASSERT(!c, "c=0");
}

Test(stack_fails) {
	_debug_no_write = true;
	_debug_no_exit = true;
	__stack_chk_fail();
	_debug_no_write = false;
	_debug_no_exit = false;
}

#endif
/* GCOVR_EXCL_STOP */

#endif /* STUBS_IMPL_GUARD */
#endif /* STUBS_IMPL */
