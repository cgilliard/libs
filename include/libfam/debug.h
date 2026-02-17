#ifndef _DEBUG_H
#define _DEBUG_H

#include <libfam/types.h>

extern i64 _debug_alloc_failure;

#endif /* _DEBUG_H */

#ifdef DEBUG_IMPL
#ifndef DEBUG_IMPL_GUARD
#define DEBUG_IMPL_GUARD

#include <libfam/limits.h>
#include <libfam/utils.h>

PUBLIC i64 _debug_alloc_failure = I64_MAX;

#endif /* DEBUG_IMPL_GUARD */
#endif /* DEBUG_IMPL */
