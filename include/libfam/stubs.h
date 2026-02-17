#ifndef _STUBS_H
#define _STUBS_H

#endif /* _STUBS_H */

#ifdef STUBS_IMPL
#ifndef STUBS_IMPL_GUARD
#define STUBS_IMPL_GUARD

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

#ifdef TEST
#include <libfam/test.h>

Test(__umodti3) { ASSERT(1); }
#endif

#endif /* STUBS_IMPL_GUARD */
#endif /* STUBS_IMPL */
