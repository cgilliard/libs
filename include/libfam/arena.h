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

#ifndef _ARENA_H
#define _ARENA_H

/*****************************************************************************
 *
```
sudo ./build install
```
 * test.c
```
#include <libfam/arena.h>

i32 main(void) {
	char *ptr;
	Arena *a = NULL;
	arena_init(&a, 1024, 8);
	ptr = arena_alloc(a, 128);
	ptr[3] = 37;
	exit_group(ptr[3]);
	return 0;
}
```
 *
 * ```
cc -DARENA_ALL_IMPL test.c -o test
 * ```
 *
 ****************************************************************************/

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

#ifdef ARENA_ALL_IMPL
#define ARENA_IMPL
#define SYSCALL_IMPL
#define SYNC_IMPL
#define SYSEXT_IMPL
#endif

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
	if (a->current > a->end - to_alloc || to_alloc > (u64)a->end)
		return NULL;
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
#include <libfam/debug.h>
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

Test(failures) {
	void *ptr = NULL;
	Arena *a = NULL;
	Arena x = {0};
	i32 result;

	result = arena_init(&a, 0, 8);
	ASSERT_EQ(result, -EINVAL);
	result = arena_init(&a, 1024, 0);
	ASSERT_EQ(result, -EINVAL);
	result = arena_init(&a, 1024, 3);
	ASSERT_EQ(result, -EINVAL);
	result = arena_init(&a, 1024, 1024L * 1024L * 1024L * 8L);
	ASSERT_EQ(result, -EINVAL);
	result = arena_init(&a, U64_MAX, 8);
	ASSERT_EQ(result, -ENOMEM);
	ptr = arena_alloc(NULL, 100);
	ASSERT_EQ(ptr, NULL, "null input");

	result = arena_init(&a, 1024, 8);
	ASSERT_EQ(result, 0, "successful arena_init");
	ptr = arena_alloc(a, 2048);
	ASSERT(!ptr, "too big");
	ptr = arena_alloc(a, U64_MAX);
	ASSERT(!ptr, "overflow");

	arena_destroy(a);
	arena_destroy(NULL);
	arena_destroy(&x);

	a = NULL;
	result = arena_init(&a, 1024 * 1024, 4096);
	ASSERT_EQ(result, 0, "successful arena_init (4096)");
	arena_destroy(a);

	a = NULL;
	_debug_alloc_failure = 0;
	result = arena_init(&a, 1024 * 1024, 4096);
	ASSERT_EQ(result, -ENOMEM, "enomem {}", result);
	_debug_alloc_failure = I64_MAX;
}
#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* ARENA_IMPL_GUARD */
#endif /* ARENA_IMPL */

