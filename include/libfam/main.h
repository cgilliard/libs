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

#ifndef _MAIN_H
#define _MAIN_H

/*****************************************************************************
 *
```
sudo ./build install
```
 * test.c
```
#include <libfam/main.h>

i32 main(void) { return 8; }
```
 *
```
cc -DMAIN_ALL_IMPL -static -nostdlib test.c -o test
$ du -h test
12K	test
$ ldd test
	not a dynamic executable
$ ./test; echo $?
8
$
```
 *
 ****************************************************************************/

#include <libfam/types.h>

#ifdef MAIN_ALL_IMPL
#define MAIN_IMPL
#define SYSCALL_IMPL

#include <libfam/syscall.h>
#endif /* MAIN_ALL_IMPL */

#endif /* _MAIN_H */

#ifdef MAIN_IMPL
#ifndef MAIN_IMPL_GUARD

#ifdef __aarch64__
__asm__(
    ".section .text\n"
    ".global _start\n"
    "_start:\n"
    "    ldr x0, [sp]\n"
    "    add x1, sp, #8\n"
    "    add x3, x0, #1\n"
    "    lsl x3, x3, #3\n"
    "    add x2, x1, x3\n"
    "    mov x4, sp\n"
    "    bic x4, x4, #15\n"
    "    mov sp, x4\n"
    "    bl main\n"
    "    bl exit_group\n");
#elif defined(__x86_64__)
__asm__(
    ".section .text\n"
    ".global _start\n"
    "_start:\n"
    "    movq (%rsp), %rdi\n"
    "    lea 8(%rsp), %rsi\n"
    "    mov %rdi, %rcx\n"
    "    add $1, %rcx\n"
    "    shl $3, %rcx\n"
    "    lea (%rsi, %rcx), %rdx\n"
    "    mov %rsp, %rcx\n"
    "    and $-16, %rsp\n"
    "    call main\n"
    "    mov %rax, %rdi\n"
    "    jmp exit_group\n");
#endif /* __x86_64__ */

#endif /* MAIN_IMPL_GUARD */
#endif /* MAIN_IMPL */
