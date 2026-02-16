#ifndef _ATOMIC_H
#define _ATOMIC_H

#include <libfam/types.h>

#ifdef __TINYC__
#define HAS_BUILTINS 0
#else
#if __has_builtin(__builtin_clz)
#define HAS_BUILTINS 1
#else
#define HAS_BUILTINS 0
#endif
#endif /* !__TINYC__ */

static i32 atomic_fetch_add32(u32 *ptr, u32 v) {
#if HAS_BUILTINS == 1
	return __atomic_fetch_add(ptr, v, __ATOMIC_SEQ_CST);
#else
	i32 old;

	__asm__ __volatile__("lock xaddl %0, %1"
			     : "=r"(old), "+m"(*ptr)
			     : "0"((i32)v)
			     : "memory", "cc");

	return old;
#endif
}

#endif /* _ATOMIC_H */
