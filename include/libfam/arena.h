#ifndef _ARENA_H
#define _ARENA_H

#include <libfam/types.h>

typedef struct {
	u8 *start;
	u8 *current;
	u8 *end;
	u64 align;
} Arena;

i32 arena_init(Arena **a, u64 size, u64 alignment);
void *arena_alloc(Arena *a, u64 size);
void arena_destroy(Arena *a);

#endif /* _ARENA_H */

#ifdef ARENA_IMPL
#ifndef ARENA_IMPL_GUARD
#define ARENA_IMPL_GUARD

#include <libfam/errno.h>
#include <libfam/limits.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/utils.h>

PUBLIC i32 arena_init(Arena **a, u64 size, u64 align) {
	Arena *ret;
	if (size == 0 || align == 0 || (align & (align - 1)) != 0)
		return -EINVAL;
	if (align > U32_MAX) return -EINVAL;
	if (size + sizeof(Arena) < size) return -ENOMEM;
	ret = map(size + sizeof(Arena));
	if (!ret) return -ENOMEM;
	ret->start = ((u8 *)(ret)) + sizeof(Arena);
	ret->end = ret->start + size;
	ret->current =
	    ((u64)ret->start % align) == 0
		? ret->start
		: (u8 *)((u64)(ret->start + (align - 1)) & ~(align - 1));
	ret->align = align;
	*a = ret;
	return 0;
}

PUBLIC void *arena_alloc(Arena *__restrict a, u64 size) {
	void *ret;
	u64 to_alloc;
	if (!a) return NULL;
	to_alloc = (size + (a->align - 1)) & ~(a->align - 1);
	if (to_alloc < size) return NULL;
	if (a->current > a->end - to_alloc) return NULL;
	ret = a->current;
	a->current += to_alloc;
	return ret;
}

PUBLIC void arena_destroy(Arena *a) {
	if (a && a->start) {
		u64 size = a->end - a->start;
		a->current = a->end = a->start = NULL;
		a->align = 0;
		munmap(a, size + sizeof(Arena));
	}
}

/* GCOVR_EXCL_START */
#ifdef TEST
#include <libfam/test.h>

Test(alloc) {
	Arena *a = NULL;
	u64 align = 8;
	u64 result = arena_init(&a, 1024, align);
	void *ptr1 = arena_alloc(a, 6);
	void *ptr2 = arena_alloc(a, 5);
	void *ptr3 = arena_alloc(a, 3);
	void *ptr4 = arena_alloc(a, 100);
	void *ptr5 = arena_alloc(a, 8);
	void *ptr6 = arena_alloc(a, 8);

	ASSERT(ptr1);
	ASSERT(ptr2);
	ASSERT(ptr3);
	ASSERT(ptr4);
	ASSERT(ptr5);
	ASSERT(ptr6);
	ASSERT(((u64)ptr1 & (align - 1)) == 0);
	ASSERT(((u64)ptr2 & (align - 1)) == 0);
	ASSERT(((u64)ptr3 & (align - 1)) == 0);
	ASSERT(((u64)ptr4 & (align - 1)) == 0);
	ASSERT(((u64)ptr5 & (align - 1)) == 0);
	ASSERT(((u64)ptr6 & (align - 1)) == 0);
	ASSERT((u64)ptr2 == (u64)ptr1 + 8);
	ASSERT((u64)ptr3 == (u64)ptr2 + 8);
	ASSERT((u64)ptr4 == (u64)ptr3 + 8);
	ASSERT((u64)ptr5 == (u64)ptr4 + 104);
	ASSERT((u64)ptr6 == (u64)ptr5 + 8);

	arena_destroy(a);
}
#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* ARENA_IMPL_GUARD */
#endif /* ARENA_IMPL */

