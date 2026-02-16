#ifndef _SYSEXT_H
#define _SYSEXT_H

#include <libfam/types.h>

struct statx;
struct msghdr;
struct sockaddr;

i64 pwrite(i32 fd, const void *buf, u64 len, u64 offset);
i32 nsleep(u64 nsec);
i32 usleep(u64 usec);
i32 statx(const char *pathname, struct statx *st);
i32 unlink(const char *pathname);
i32 waitpid(i32 pid);
i32 fsync(i32 fd);
i32 fdatasync(i32 fd);
i32 open(const char *path, i32 flags, u32 mode);
i64 pread(i32 fd, void *buf, u64 len, u64 offset);
i32 fallocate(i32 fd, u64 new_size);
i32 fstatx(i32 fd, struct statx *st);
i32 socket(i32 domain, i32 type, i32 protocol);
i64 sendmsg(i32 socket, const struct msghdr *message, i32 flags);
i64 recvmsg(i32 socket, struct msghdr *message, i32 flags);
i64 bind(i32 sockfd, const struct sockaddr *addr, u64 addrlen);
void *map(u64 length);
void *fmap(i32 fd, i64 size, i64 offset);
void *smap(u64 length);
i64 micros(void);
i64 write_num(i32 fd, i64 num);
i32 fork(void);
void yield(void);
u64 cycle_counter(void);

#endif /* _SYSEXT_H */

#ifdef SYSEXT_IMPL
#ifndef SYSEXT_IMPL_GUARD
#define SYSEXT_IMPL_GUARD

#include <libfam/atomic.h>
#include <libfam/errno.h>
#include <libfam/file.h>
#include <libfam/iouring.h>
#include <libfam/mmap.h>
#include <libfam/net.h>
#include <libfam/pid.h>
#include <libfam/sync.h>
#include <libfam/time.h>
#include <libfam/utils.h>

Sync *__global_sync = NULL;

static i32 global_sync_init(void) {
	if (__global_sync) return 0;
	return sync_init(&__global_sync);
}

PUBLIC i64 pwrite(i32 fd, const void *buf, u64 len, u64 offset) {
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

PUBLIC i32 nsleep(u64 nanos) {
	i32 result;
	struct timespec ts = {.tv_nsec = nanos};
	struct io_uring_sqe sqe = {.opcode = IORING_OP_TIMEOUT,
				   .addr = (u64)&ts,
				   .len = 1,
				   .user_data = 1};

	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 usleep(u64 micros) {
	i32 result;
	if (micros * 1000 < micros) return -EOVERFLOW;
	struct timespec ts = {.tv_nsec = micros * 1000};
	struct io_uring_sqe sqe = {.opcode = IORING_OP_TIMEOUT,
				   .addr = (u64)&ts,
				   .len = 1,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 unlink(const char *pathname) {
	i32 result;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_UNLINKAT,
				   .fd = -100,
				   .addr = (u64)pathname,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 statx(const char *pathname, struct statx *st) {
	i32 result;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_STATX,
				   .fd = -100,
				   .addr = (u64)pathname,
				   .len = 0x07ffU,
				   .off = (u64)st,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 waitpid(i32 pid) {
	i32 result;
	u8 buf[1024] = {0};
	struct io_uring_sqe sqe = {.opcode = IORING_OP_WAITID,
				   .addr2 = (u64)buf,
				   .len = P_PID,
				   .fd = pid,
				   .file_index = WEXITED,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 fsync(i32 fd) {
	i32 result;
	struct io_uring_sqe sqe = {
	    .opcode = IORING_OP_FSYNC, .fd = fd, .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 fdatasync(i32 fd) {
	i32 result;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_FSYNC,
				   .fd = fd,
				   .user_data = 1,
				   .fsync_flags = IORING_FSYNC_DATASYNC};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 open(const char *path, i32 flags, u32 mode) {
	i32 result;
	struct open_how how = {.flags = flags, .mode = mode};
	struct io_uring_sqe sqe = {.opcode = IORING_OP_OPENAT2,
				   .addr = (u64)path,
				   .fd = AT_FDCWD,
				   .len = sizeof(struct open_how),
				   .off = (u64)&how,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i64 pread(i32 fd, void *buf, u64 len, u64 offset) {
	i32 result;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_READ,
				   .addr = (u64)buf,
				   .fd = fd,
				   .len = len,
				   .off = offset,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 fallocate(i32 fd, u64 new_size) {
	i32 result;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_FALLOCATE,
				   .fd = fd,
				   .addr = new_size,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 fstatx(i32 fd, struct statx *st) {
	i32 result;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_STATX,
				   .fd = fd,
				   .addr = (u64) "",
				   .len = STATX_BASIC_STATS,
				   .off = (u64)st,
				   .rw_flags = AT_EMPTY_PATH,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i32 socket(i32 domain, i32 type, i32 protocol) {
	i32 result;
	struct io_uring_sqe sqe = {
	    .opcode = IORING_OP_SOCKET,
	    .fd = domain,
	    .off = type,
	    .len = protocol,
	    .user_data = 1,
	};

	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i64 sendmsg(i32 socket, const struct msghdr *message, i32 flags) {
	i32 result;
	struct io_uring_sqe sqe = {
	    .opcode = IORING_OP_SENDMSG,
	    .fd = socket,
	    .addr = (u64)message,
	    .msg_flags = flags,
	    .user_data = 1,
	};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i64 recvmsg(i32 socket, struct msghdr *message, i32 flags) {
	i32 result;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_RECVMSG,
				   .fd = socket,
				   .addr = (u64)message,
				   .msg_flags = flags,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC i64 bind(i32 sockfd, const struct sockaddr *addr, u64 addrlen) {
	i32 result;
	struct io_uring_sqe sqe = {.opcode = IORING_OP_BIND,
				   .fd = sockfd,
				   .addr = (u64)addr,
				   .addr2 = addrlen,
				   .user_data = 1};
	if ((result = global_sync_init()) < 0) return result;
	return sync_execute(__global_sync, sqe);
}

PUBLIC void *map(u64 length) {
	void *v = mmap(NULL, length, PROT_READ | PROT_WRITE,
		       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (v == MAP_FAILED) return NULL;
	return v;
}
PUBLIC void *fmap(i32 fd, i64 size, i64 offset) {
	void *v =
	    mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
	if (v == MAP_FAILED) return NULL;
	return v;
}

PUBLIC void *smap(u64 length) {
	void *v = mmap(NULL, length, PROT_READ | PROT_WRITE,
		       MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (v == MAP_FAILED) return NULL;
	return v;
}

PUBLIC i64 micros(void) {
	i32 result;
	struct timespec ts;
	if ((result = clock_gettime(CLOCK_REALTIME, &ts)) < 0) return result;
	return (i64)ts.tv_sec * 1000000L + (i64)(ts.tv_nsec / 1000);
}

PUBLIC i64 write_num(i32 fd, i64 num) {
	u8 buf[21];
	u8 *p;
	u64 len;
	i64 written;
	i32 negative = 0;

	if (fd < 0) return fd;
	p = buf + sizeof(buf) - 1;
	*p = '\0';

	if (num < 0) {
		negative = 1;
		if (num == ((i64)(-0x7FFFFFFFFFFFFFFF - 1))) {
			u8 min_str[] = "-9223372036854775808";
			len = sizeof(min_str) - 1;
			written = pwrite(fd, min_str, len, -1);
			if (written < 0) return written;
			if ((u64)written != len) return written;
			return 0;
		}
		num = -num;
	}

	if (num == 0)
		*--p = '0';
	else
		while (num > 0) {
			*--p = '0' + (num % 10);
			num /= 10;
		}

	if (negative) *--p = '-';
	len = (buf + sizeof(buf) - 1) - p;
	written = pwrite(fd, p, len, -1);
	if (written < 0) return written;
	if ((u64)written != len) return -EIO;
	return 0;
}

PUBLIC i32 fork(void) {
	i32 ret = clone(17, 0);
	if (!ret) __global_sync = NULL;
	return ret;
}

PUBLIC void yield(void) {
#if defined(__x86_64__)
	__asm__ __volatile__("pause" ::: "memory");
#elif defined(__aarch64__)
	__asm__ __volatile__("yield" ::: "memory");
#endif
}

PUBLIC u64 cycle_counter(void) {
#if defined(__x86_64__)
	u32 lo, hi;
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return ((u64)hi << 32) | lo;
#elif defined(__aarch64__)
	u64 cnt;
	__asm__ __volatile__("isb" : : : "memory");
	__asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(cnt));
	return cnt;
#else
#error "Unsupported architecture"
#endif
}

#endif /* SYSEXT_IMPL_GUARD */
#endif /* SYSEXT_IMPL */

