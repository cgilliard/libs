#ifndef _STUBS_H
#define _STUBS_H

#endif /* _STUBS_H */

#ifdef STUBS_IMPL
#ifndef STUBS_IMPL_GUARD
#define STUBS_IMPL_GUARD

#include <libfam/atomic.h>
#include <libfam/limits.h>
#include <libfam/types.h>
#include <libfam/utils.h>

i64 __fixunsdfdi(f64 x);
f64 __floatundidf(u64 x);

PUBLIC i64 __fixunsdfdi(f64 x) {
	if (x >= 9223372036854775808.0)
		return (i64)(x - 9223372036854775808.0) + I64_MIN;
	return (i64)x;
}

PUBLIC f64 __floatundidf(u64 x) {
	if (x <= 9007199254740991ULL) return (f64)(i64)x;
	f64 hi = (f64)(u32)(x >> 32);
	f64 lo = (f64)(u32)x;
	return hi * 4294967296.0 + lo;
}

#ifdef __TINYC__

void __atomic_thread_fence(int mem_order) {
	switch (mem_order) {
		case __ATOMIC_SEQ_CST:
		case __ATOMIC_ACQ_REL:
			__asm__ __volatile__("mfence" ::: "memory");
			break;

		case __ATOMIC_ACQUIRE:
		case __ATOMIC_CONSUME:
			__asm__ __volatile__("lfence" ::: "memory");
			break;

		case __ATOMIC_RELEASE:
			__asm__ __volatile__("sfence" ::: "memory");
			break;

		case __ATOMIC_RELAXED:
			break;

		default:
			__asm__ __volatile__("mfence" ::: "memory");
			break;
	}
}
#endif

#ifdef TEST
Test(__fixunsdfdi) { ASSERT(1); }
#endif

#endif /* STUBS_IMPL_GUARD */
#endif /* STUBS_IMPL */
