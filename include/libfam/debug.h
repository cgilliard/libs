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

#ifdef TEST
#ifndef _DEBUG_H
#define _DEBUG_H

#include <libfam/rbtree.h>
#include <libfam/types.h>

extern i64 _debug_alloc_failure;
extern RbTree *_debug_env_rbtree;
extern bool _debug_env_insert_fail;
extern bool _debug_no_write;
extern bool _debug_no_exit;
extern bool _debug_proc_format_all;
extern bool _debug_io_uring_setup_fail;
extern bool _debug_io_uring_enter2_fail;
extern i64 _debug_syscall_return;
extern i64 _debug_pwrite_return;

#endif /* _DEBUG_H */

#ifdef DEBUG_IMPL
#ifndef DEBUG_IMPL_GUARD
#define DEBUG_IMPL_GUARD

#include <libfam/limits.h>
#include <libfam/utils.h>

PUBLIC i64 _debug_alloc_failure = I64_MAX;
PUBLIC RbTree *_debug_env_rbtree = NULL;
PUBLIC bool _debug_env_insert_fail = false;
PUBLIC bool _debug_no_write = false;
PUBLIC bool _debug_no_exit = false;
PUBLIC bool _debug_proc_format_all = false;
PUBLIC bool _debug_io_uring_setup_fail = false;
PUBLIC bool _debug_io_uring_enter2_fail = false;
PUBLIC i64 _debug_syscall_return = 0;
PUBLIC i64 _debug_pwrite_return = 0;

#endif /* DEBUG_IMPL_GUARD */
#endif /* DEBUG_IMPL */
#endif /* TEST */
