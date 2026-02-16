#ifndef _SYNC_H
#define _SYNC_H

#include <libfam/atomic.h>
#include <libfam/syscall.h>
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
#include <libfam/mmap.h>

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

i32 sync_init(Sync **s) {
	Sync *sync = NULL;

	sync = mmap(NULL, sizeof(Sync), PROT_READ | PROT_WRITE,
		    MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (sync == MAP_FAILED) return -1;

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

	if (sync->sq_ring == MAP_FAILED) {
		sync->sq_ring = NULL;
		sync_destroy(sync);
		return -1;
	}

	sync->cq_ring = mmap(NULL, sync->cq_ring_size, PROT_READ | PROT_WRITE,
			     MAP_SHARED, sync->ring_fd, IORING_OFF_CQ_RING);

	if (sync->cq_ring == MAP_FAILED) {
		sync->cq_ring = NULL;
		sync_destroy(sync);
		return -1;
	}
	sync->sqes = mmap(NULL, sync->sqes_size, PROT_READ | PROT_WRITE,
			  MAP_SHARED, sync->ring_fd, IORING_OFF_SQES);
	if (sync->sqes == MAP_FAILED) {
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

i32 sync_execute(Sync *sync, const struct io_uring_sqe sqe) {
	i32 ret;
	u32 cq_mask = *sync->cq_mask;
	u32 sq_mask = *sync->sq_mask;
	u32 sq_tail = *sync->sq_tail;
	u32 index = sq_tail & sq_mask;
	u32 cq_head = *sync->cq_head;
	u32 idx, flag = IORING_ENTER_GETEVENTS;
	sync->sq_array[index] = index;
	sync->sqes[index] = sqe;
	atomic_fetch_add32(sync->sq_tail, 1);
	ret = io_uring_enter2(sync->ring_fd, 1, 1, flag, NULL, 0);

	if (ret < 0)
		atomic_fetch_add32(sync->sq_tail, 1);
	else {
		idx = cq_head & cq_mask;
		ret = sync->cqes[idx].res;
		atomic_fetch_add32(sync->cq_head, 1);
	}

	return ret;
}

void sync_destroy(Sync *sync) {
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

#endif /* SYNC_IMPL_GUARD */
#endif /* SYNC_IMPL */
