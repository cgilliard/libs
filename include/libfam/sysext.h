#ifndef _SYSEXT_H
#define _SYSEXT_H

#include <libfam/types.h>

i64 pwrite(i32 fd, const void *buf, u64 len, u64 offset);

#endif /* _SYSEXT_H */

#ifdef SYSEXT_IMPL
#ifndef SYSEXT_IMPL_GUARD
#define SYSEXT_IMPL_GUARD

#include <libfam/iouring.h>
#include <libfam/sync.h>

Sync *__global_sync = NULL;

static i32 global_sync_init(void) {
	if (__global_sync) return 0;
	return sync_init(&__global_sync);
}

i64 pwrite(i32 fd, const void *buf, u64 len, u64 offset) {
	i64 result;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_WRITE,
				   .addr = (u64)buf,
				   .fd = fd,
				   .len = len,
				   .off = offset,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

#endif /* SYSEXT_IMPL_GUARD */
#endif /* SYSEXT_IMPL */

