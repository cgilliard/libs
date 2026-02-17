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

#endif /* DEBUG_IMPL_GUARD */
#endif /* DEBUG_IMPL */
#endif /* TEST */
