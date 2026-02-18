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

#ifndef _SYNC_H
#define _SYNC_H

#include <libfam/types.h>

typedef struct Sync Sync;
struct io_uring_sqe;

i32 sync_init(Sync **sync);
i32 sync_execute(Sync *sync, const struct io_uring_sqe event);
void sync_destroy(Sync *sync);

#endif /* _SYNC_H */

#ifdef SYNC_IMPL
#ifndef SYMC_IMPL_GUARD
#define SYMC_IMPL_GUARD

#include <libfam/iouring.h>
#include <libfam/limits.h>
#include <libfam/mmap.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/utils.h>

struct Sync {
	struct io_uring_params params;
	i32 ring_fd;
	u8 *sq_ring;
	u8 *cq_ring;
	struct io_uring_sqe *sqes;
	struct io_uring_cqe *cqes;
	u64 sq_ring_size;
	u64 cq_ring_size;
	u64 sqes_size;
	u32 *sq_tail;
	u32 *sq_array;
	u32 *cq_head;
	u32 *cq_tail;
	u32 *sq_mask;
	u32 *cq_mask;
};

PUBLIC i32 sync_init(Sync **s) {
	Sync *sync = NULL;

	sync = mmap(NULL, sizeof(Sync), PROT_READ | PROT_WRITE,
		    MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if ((u64)sync > U64_MAX - 200) return -1;

	sync->sq_ring = NULL;
	sync->cq_ring = NULL;
	sync->sqes = NULL;
	sync->ring_fd = io_uring_setup(1, &sync->params);
	if (sync->ring_fd < 0) {
		sync_destroy(sync);
		return -1;
	}

	sync->sq_ring_size =
	    sync->params.sq_off.array + sync->params.sq_entries * sizeof(u32);
	sync->cq_ring_size =
	    sync->params.cq_off.cqes +
	    sync->params.cq_entries * sizeof(struct io_uring_cqe);
	sync->sqes_size = sync->params.sq_entries * sizeof(struct io_uring_sqe);

	sync->sq_ring = mmap(NULL, sync->sq_ring_size, PROT_READ | PROT_WRITE,
			     MAP_SHARED, sync->ring_fd, IORING_OFF_SQ_RING);

	if ((u64)sync->sq_ring > U64_MAX - 200) {
		sync->sq_ring = NULL;
		sync_destroy(sync);
		return -1;
	}

	sync->cq_ring = mmap(NULL, sync->cq_ring_size, PROT_READ | PROT_WRITE,
			     MAP_SHARED, sync->ring_fd, IORING_OFF_CQ_RING);

	if ((u64)sync->cq_ring > U64_MAX - 200) {
		sync->cq_ring = NULL;
		sync_destroy(sync);
		return -1;
	}
	sync->sqes = mmap(NULL, sync->sqes_size, PROT_READ | PROT_WRITE,
			  MAP_SHARED, sync->ring_fd, IORING_OFF_SQES);
	if ((u64)sync->sqes > U64_MAX - 200) {
		sync->sqes = NULL;
		sync_destroy(sync);
		return -1;
	}

	sync->sq_tail = (u32 *)(sync->sq_ring + sync->params.sq_off.tail);
	sync->sq_array = (u32 *)(sync->sq_ring + sync->params.sq_off.array);
	sync->cq_head = (u32 *)(sync->cq_ring + sync->params.cq_off.head);
	sync->cq_tail = (u32 *)(sync->cq_ring + sync->params.cq_off.tail);
	sync->sq_mask = (u32 *)(sync->sq_ring + sync->params.sq_off.ring_mask);

	sync->cq_mask = (u32 *)(sync->cq_ring + sync->params.cq_off.ring_mask);
	sync->cqes =
	    (struct io_uring_cqe *)(sync->cq_ring + sync->params.cq_off.cqes);

	*s = sync;
	return 0;
}

PUBLIC i32 sync_execute(Sync *sync, const struct io_uring_sqe sqe) {
	i32 ret;
	u32 cq_mask = *sync->cq_mask;
	u32 sq_mask = *sync->sq_mask;
	u32 sq_tail = *sync->sq_tail;
	u32 index = sq_tail & sq_mask;
	u32 cq_head = *sync->cq_head;
	u32 idx, flag = IORING_ENTER_GETEVENTS;
	sync->sq_array[index] = index;
	sync->sqes[index] = sqe;
	__atomic_fetch_add(sync->sq_tail, 1, __ATOMIC_SEQ_CST);
	ret = io_uring_enter2(sync->ring_fd, 1, 1, flag, NULL, 0);

	if (ret < 0)
		__atomic_fetch_sub(sync->sq_tail, 1, __ATOMIC_SEQ_CST);
	else {
		idx = cq_head & cq_mask;
		ret = sync->cqes[idx].res;
		__atomic_fetch_add(sync->cq_head, 1, __ATOMIC_SEQ_CST);
	}

	return ret;
}

PUBLIC void sync_destroy(Sync *sync) {
	if (sync) {
		if (sync->sq_ring) munmap(sync->sq_ring, sync->sq_ring_size);
		sync->sq_ring = NULL;
		if (sync->cq_ring) munmap(sync->cq_ring, sync->cq_ring_size);
		sync->cq_ring = NULL;
		if (sync->sqes) munmap(sync->sqes, sync->sqes_size);
		sync->sqes = NULL;

		if (sync->ring_fd > 0) close(sync->ring_fd);
		sync->ring_fd = -1;

		munmap(sync, sizeof(Sync));
	}
}

/* GCOVR_EXCL_START */
#ifdef TEST

#include <libfam/debug.h>
#include <libfam/test.h>

Test(syncfail) {
	Sync *s = NULL;
	_debug_io_uring_setup_fail = true;
	ASSERT(sync_init(&s), "setup fail");
	_debug_io_uring_setup_fail = false;
	_debug_alloc_failure = 1;
	ASSERT(sync_init(&s), "alloc 1 fail");
	_debug_alloc_failure = I64_MAX;
	_debug_alloc_failure = 2;
	ASSERT(sync_init(&s), "alloc 2 fail");
	_debug_alloc_failure = I64_MAX;
	_debug_alloc_failure = 3;
	ASSERT(sync_init(&s), "alloc 3 fail");
	_debug_alloc_failure = I64_MAX;

	ASSERT(!sync_init(&s), "init success");
	_debug_io_uring_enter2_fail = true;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_WRITE,
				   .addr = (u64) "abc\n",
				   .fd = 2,
				   .len = 4,
				   .off = -1,
				   .user_data = 1};
	ASSERT(sync_execute(s, sqe) < 0, "exec err");

	_debug_io_uring_enter2_fail = false;
	sync_destroy(s);
	sync_destroy(NULL);
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* SYNC_IMPL_GUARD */
#endif /* SYNC_IMPL */
