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

#ifndef _SYSCALL_H
#define _SYSCALL_H

/*****************************************************************************
 *
 * This library is dependent on types.h and utils.h. To use it, ensure that
 * those files along with syscall.h are in your include/libfam directory and
 * you must define SYSCALL_IMPL before your include of the library.
 *
 *
 * test.c
 * ```
 * #define SYSCALL_IMPL
 * #include <libfam/syscall.h>
 *
 * void _start(void) { exit_group(3); }
 * ```
 *
 * ```
 * # cc -nostdlib -Iinclude test.c -o test
 * ```
 *
 ****************************************************************************/

/*****************************************************************************
 * Public Interface
 ****************************************************************************/

#include <libfam/types.h>

struct io_uring_params;
struct timespec;
struct sockaddr;

i32 clock_gettime(i32 clockid, struct timespec *tp);
void *mmap(void *addr, u64 length, i32 prot, i32 flags, i32 fd, i64 offset);
i32 munmap(void *addr, u64 len);
i32 clone(i64 flags, void *sp);
void exit_group(i32 status);
i32 io_uring_setup(u32 entries, struct io_uring_params *params);
i32 io_uring_enter2(u32 fd, u32 to_submit, u32 min_complete, u32 flags,
		    void *arg, u64 sz);
i32 io_uring_register(u32 fd, u32 opcode, void *arg, u32 nr_args);
i32 close(i32 fd);
i32 setsockopt(i32 socket, i32 level, i32 option_name, const void *option_value,
	       u64 option_len);
i32 getsockname(i32 sockfd, struct sockaddr *addr, u64 *addrlen);

#endif /* _SYSCALL_H */

#ifdef SYSCALL_IMPL
#ifndef SYSCALL_IMPL_GUARD
#define SYSCALL_IMPL_GUARD

/*****************************************************************************
 * Implementation
 ****************************************************************************/

#include <libfam/debug.h>
#include <libfam/errno.h>
#include <libfam/mmap.h>
#include <libfam/utils.h>

static inline i64 syscall(i64 sysno, i64 a0, i64 a1, i64 a2, i64 a3, i64 a4,
			  i64 a5) {
	i64 result = 0;
#ifdef TEST
	if (_debug_syscall_return) return _debug_syscall_return;
#endif /* TEST */
#ifdef __aarch64__
	__asm__ volatile(
	    "mov x8, %1\n"
	    "mov x0, %2\n"
	    "mov x1, %3\n"
	    "mov x2, %4\n"
	    "mov x3, %5\n"
	    "mov x4, %6\n"
	    "mov x5, %7\n"
	    "svc #0\n"
	    "mov %0, x0\n"
	    : "=r"(result)
	    : "r"(sysno), "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5)
	    : "x0", "x1", "x2", "x3", "x4", "x5", "x8", "memory");
#elif defined(__x86_64__)
	register i64 _a3 __asm__("r10") = a3;
	register i64 _a4 __asm__("r8") = a4;
	register i64 _a5 __asm__("r9") = a5;
	__asm__ volatile("syscall"
			 : "=a"(result)
			 : "a"(sysno), "D"(a0), "S"(a1), "d"(a2), "r"(_a3),
			   "r"(_a4), "r"(_a5)
			 : "rcx", "r11", "memory");
#endif /* __x86_64__ */
	return result;
}

PUBLIC i32 clock_gettime(i32 clockid, struct timespec *tp) {
#ifdef __aarch64__
	return syscall(113, clockid, (i64)tp, 0, 0, 0, 0);
#elif defined(__x86_64__)
	return syscall(228, clockid, (i64)tp, 0, 0, 0, 0);
#endif
}

PUBLIC void *mmap(void *addr, u64 length, i32 prot, i32 flags, i32 fd,
		  i64 offset) {
#ifdef TEST
	if (_debug_alloc_failure-- == 0) return MAP_FAILED;
#endif /* TEST */

#ifdef __aarch64__
	return (void *)syscall(222, (i64)addr, length, prot, flags, fd, offset);
#elif defined(__x86_64__)
	return (void *)syscall(9, (i64)addr, length, prot, flags, fd, offset);
#endif
}

PUBLIC i32 munmap(void *addr, u64 len) {
#ifdef __aarch64__
	return syscall(215, (i64)addr, len, 0, 0, 0, 0);
#elif defined(__x86_64__)
	return syscall(11, (i64)addr, len, 0, 0, 0, 0);
#endif
}

PUBLIC i32 clone(i64 flags, void *sp) {
#ifdef __aarch64__
	return syscall(220, flags, (i64)sp, 0, 0, 0, 0);
#elif defined(__x86_64__)
	return syscall(56, flags, (i64)sp, 0, 0, 0, 0);
#endif
}

/* GCOVR_EXCL_START */
PUBLIC void exit_group(i32 status) {
#ifdef TEST
	if (_debug_no_exit) return;
#endif /* TEST */

#ifdef __aarch64__
	syscall(94, status, 0, 0, 0, 0, 0);
#elif defined(__x86_64__)
	syscall(231, status, 0, 0, 0, 0, 0);
#endif
}
/* GCOVR_EXCL_STOP */

PUBLIC i32 io_uring_setup(u32 entries, struct io_uring_params *params) {
#ifdef TEST
	if (_debug_io_uring_setup_fail) return -EINVAL;
#endif /* TEST */
	return syscall(425, entries, (i64)params, 0, 0, 0, 0);
}

PUBLIC i32 io_uring_enter2(u32 fd, u32 to_submit, u32 min_complete, u32 flags,
			   void *arg, u64 sz) {
#ifdef TEST
	if (_debug_io_uring_enter2_fail) return -EINVAL;
#endif /* TEST */

	return syscall(426, fd, to_submit, min_complete, flags, (i64)arg, sz);
}

PUBLIC i32 io_uring_register(u32 fd, u32 opcode, void *arg, u32 nr_args) {
	return syscall(427, fd, opcode, (i64)arg, nr_args, 0, 0);
}

PUBLIC i32 close(i32 fd) {
#ifdef __aarch64__
	return syscall(57, fd, 0, 0, 0, 0, 0);
#elif defined(__x86_64__)
	return syscall(3, fd, 0, 0, 0, 0, 0);
#endif
}

PUBLIC i32 setsockopt(i32 socket, i32 level, i32 option_name,
		      const void *option_value, u64 option_len) {
#ifdef __aarch64__
	return syscall(208, (i64)socket, (i64)level, (i64)option_name,
		       (i64)option_value, option_len, 0);
#elif defined(__x86_64__)
	return syscall(54, (i64)socket, (i64)level, (i64)option_name,
		       (i64)option_value, option_len, 0);
#endif
}

PUBLIC i32 getsockname(i32 sockfd, struct sockaddr *addr, u64 *addrlen) {
#ifdef __aarch64__
	return syscall(204, (i64)sockfd, (i64)addr, (i64)addrlen, 0, 0, 0);
#elif defined(__x86_64__)
	return syscall(51, (i64)sockfd, (i64)addr, (i64)addrlen, 0, 0, 0);
#endif
}

/*****************************************************************************
 * Tests
 ****************************************************************************/
/* GCOVR_EXCL_START */
#ifdef TEST
#include <libfam/format.h>
#include <libfam/limits.h>
#include <libfam/mmap.h>
#include <libfam/test.h>
#include <libfam/time.h>

Test(mmap) {
	void *v = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
		       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT(v);
	ASSERT((u64)v > 0 && (u64)v < U64_MAX - 1000);
	munmap(v, 4096);

	v = mmap(NULL, 4096, U32_MAX, U32_MAX, -1, 0);
	ASSERT_EQ(1 + (U64_MAX - (u64)v), EINVAL, "einval");
}

Test(clone) {
	i32 pid = clone(0, NULL);
	if (!pid) exit_group(0);
	ASSERT(pid);
}

Test(gettime) {
	struct timespec ts = {0};
	ASSERT_EQ(clock_gettime(CLOCK_REALTIME, &ts), 0, "gettime");
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* SYSCALL_IMPL_GUARD */
#endif /* SYSCALL_IMPL */

