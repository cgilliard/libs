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
 * # gcc -nostdlib -Iinclude test.c -o test
 * ```
 *
 ****************************************************************************/

/*****************************************************************************
 * Public Interface
 ****************************************************************************/

#include <libfam/types.h>

struct io_uring_params;
struct timespec;

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

#endif /* _SYSCALL_H */

#ifdef SYSCALL_IMPL
#ifndef SYSCALL_IMPL_GUARD
#define SYSCALL_IMPL_GUARD

/*****************************************************************************
 * Implementation
 ****************************************************************************/

#include <libfam/utils.h>

#ifdef __x86_64__
__asm__(
    ".section .text\n"
    "syscall:\n"
    "push    %rbp\n"
    "mov     %rdi, %rbp\n"
    "mov     %rsi, %rax\n"
    "mov     %rdx, %rdi\n"
    "mov     %rcx, %rsi\n"
    "mov     %r8,  %rdx\n"
    "mov     %r9,  %r10\n"
    "mov     16(%rsp), %r8\n"
    "mov     24(%rsp), %r9\n"
    "syscall\n"
    "mov     %rax, 0(%rbp)\n"
    "pop     %rbp\n"
    "ret\n");
#elif defined(__aarch64__)
__asm__(
    ".section .text\n"
    "syscall:\n"
    "stp     x29, x30, [sp, #-16]!\n"
    "mov     x29, x0\n"
    "mov     x8, x1\n"
    "mov     x0, x2\n"
    "mov     x1, x3\n"
    "mov     x2, x4\n"
    "mov     x3, x5\n"
    "mov     x4, x6\n"
    "mov     x5, x7\n"
    "svc     #0\n"
    "str     x0, [x29]\n"
    "ldp     x29, x30, [sp], #16\n"
    "ret\n");
#else
#error "Unsupported platform"
#endif

void syscall(void *ret, i64 sysno, i64 a0, i64 a1, i64 a2, i64 a3, i64 a4,
	     i64 a5);

PUBLIC i32 clock_gettime(i32 clockid, struct timespec *tp) {
	i64 result;
#ifdef __aarch64__
	syscall(&result, 113, clockid, (i64)tp, 0, 0, 0, 0);
#elif defined(__x86_64__)
	syscall(&result, 231, clockid, (i64)tp, 0, 0, 0, 0);
#endif
	return result;
}

PUBLIC void *mmap(void *addr, u64 length, i32 prot, i32 flags, i32 fd,
		  i64 offset) {
	void *result;
#ifdef __aarch64__
	syscall(&result, 222, (i64)addr, length, prot, flags, fd, offset);
#elif defined(__x86_64__)
	syscall(&result, 9, (i64)addr, length, prot, flags, fd, offset);
#endif
	return result;
}

PUBLIC i32 munmap(void *addr, u64 len) {
	i64 result;
#ifdef __aarch64__
	syscall(&result, 215, (i64)addr, len, 0, 0, 0, 0);
#elif defined(__x86_64__)
	syscall(&result, 11, (i64)addr, len, 0, 0, 0, 0);
#endif
	return result;
}

PUBLIC i32 clone(i64 flags, void *sp) {
	i64 result;
#ifdef __aarch64__
	syscall(&result, 220, flags, (i64)sp, 0, 0, 0, 0);
#elif defined(__x86_64__)
	syscall(&result, 56, flags, (i64)sp, 0, 0, 0, 0);
#endif
	return result;
}

PUBLIC void exit_group(i32 status) {
#ifdef __aarch64__
	syscall(&status, 94, status, 0, 0, 0, 0, 0);
#elif defined(__x86_64__)
	syscall(&status, 231, status, 0, 0, 0, 0, 0);
#endif
}

PUBLIC i32 io_uring_setup(u32 entries, struct io_uring_params *params) {
	i64 result;
	syscall(&result, 425, entries, (i64)params, 0, 0, 0, 0);
	return result;
}

PUBLIC i32 io_uring_enter2(u32 fd, u32 to_submit, u32 min_complete, u32 flags,
			   void *arg, u64 sz) {
	i64 result;
	syscall(&result, 426, fd, to_submit, min_complete, flags, (i64)arg, sz);
	return result;
}

PUBLIC i32 io_uring_register(u32 fd, u32 opcode, void *arg, u32 nr_args) {
	i64 result;
	syscall(&result, 427, fd, opcode, (i64)arg, nr_args, 0, 0);
	return result;
}

PUBLIC i32 close(i32 fd) {
	i64 result;
#ifdef __aarch64__
	syscall(&result, 57, fd, 0, 0, 0, 0, 0);
#elif defined(__x86_64__)
	syscall(&result, 3, fd, 0, 0, 0, 0, 0);
#endif
	return result;
}

/*****************************************************************************
 * Tests
 ****************************************************************************/
#ifdef TEST

#include <libfam/mmap.h>

Test(mmap) {
	void *v = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
		       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT(v);
	ASSERT(v != MAP_FAILED);
	munmap(v, 4096);
}

Test(clone) {
	i32 pid = clone(0, NULL);
	if (!pid) exit_group(0);
	ASSERT(pid);
}

#endif /* TEST */

#endif /* SYSCALL_IMPL_GUARD */
#endif /* SYSCALL_IMPL */

