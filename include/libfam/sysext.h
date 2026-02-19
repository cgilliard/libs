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

#include <libfam/debug.h>
#include <libfam/errno.h>
#include <libfam/file.h>
#include <libfam/iouring.h>
#include <libfam/limits.h>
#include <libfam/mmap.h>
#include <libfam/net.h>
#include <libfam/pid.h>
#include <libfam/sync.h>
#include <libfam/syscall.h>
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
#ifdef TEST
	if (_debug_pwrite_return) return _debug_pwrite_return;
	if (_debug_no_write) return len;
#endif /* TEST */
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
	if ((u64)v > U64_MAX - 2000) return NULL;
	return v;
}
PUBLIC void *fmap(i32 fd, i64 size, i64 offset) {
	void *v =
	    mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
	if ((u64)v > U64_MAX - 2000) return NULL;
	return v;
}

PUBLIC void *smap(u64 length) {
	void *v = mmap(NULL, length, PROT_READ | PROT_WRITE,
		       MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if ((u64)v > U64_MAX - 2000) return NULL;
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
			if ((u64)written != len) return -EIO;
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
	/* GCOVR_EXCL_START */
	if (!ret) __global_sync = NULL;
	/* GCOVR_EXCL_STOP */
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

/* GCOVR_EXCL_START */
#ifdef TEST
#include <libfam/iov.h>
#include <libfam/string.h>
#include <libfam/test.h>

Test(map) {
	void *x = map(100);
	ASSERT(x);
	munmap(x, 100);
	x = map(-1L);
	ASSERT(!x);
}
Test(open) {
	i32 fd = open("/tmp/open_test1.dat", O_RDWR | O_CREAT, 0600);
	ASSERT(fd > 0, "fd");
	i32 res = io_uring_register(fd, IORING_REGISTER_FILES, NULL, 0);
	ASSERT(res < 0, "not a ring fd");

	struct statx st = {0};
	ASSERT(!statx("/tmp/open_test1.dat", &st), "statx");
	ASSERT_EQ(st.stx_size, 0, "size=0");
	ASSERT(!fallocate(fd, 100), "fallocate");
	ASSERT(!fstatx(fd, &st), "fstatx");
	ASSERT_EQ(st.stx_size, 100, "size=100");
	u8 buf[100] = {0}, verify[100] = {0}, *verify2;
	i32 i;
	for (i = 0; i < 100; i++) buf[i] = 3;
	pwrite(fd, buf, 100, 0);
	pread(fd, verify, 100, 0);
	verify2 = fmap(fd, 100, 0);
	ASSERT(!memcmp(buf, verify, 100), "verify");
	ASSERT(!memcmp(buf, verify2, 100), "verify2");
	fsync(fd);
	fdatasync(fd);

	munmap(verify2, 100);
	close(fd);
	unlink("/tmp/open_test1.dat");
	fd = open("/tmp/open_test1.dat", O_RDONLY, 0);
	ASSERT(fd < 0, "doesn't exist");
}

Test(socket) {
	struct sockaddr_in src_addr = {0};
	struct sockaddr_in addr = {.sin_family = AF_INET,
				   .sin_port = htons(0),
				   .sin_addr = {htonl(INADDR_ANY)}};
	struct sockaddr_in dest_addr = {.sin_family = AF_INET,
					.sin_addr = {htonl(0x7f000001U)}};
	struct iovec msgvec[1] = {
	    {.iov_base = "Hello1", .iov_len = 6},
	};
	struct msghdr msg = {.msg_name = &dest_addr,
			     .msg_namelen = sizeof(dest_addr),
			     .msg_iov = msgvec,
			     .msg_iovlen = 1};
	u8 msg_buf[32] = {0};
	struct iovec msgoutvec[1] = {
	    {.iov_base = msg_buf, .iov_len = 32},
	};
	struct msghdr msgout = {.msg_name = &src_addr,
				.msg_namelen = sizeof(src_addr),
				.msg_iov = msgoutvec,
				.msg_iovlen = 1};

	u64 addrlen = sizeof(addr);
	u64 one = 1;
	i32 res;
	i32 sfd = socket(AF_INET, SOCK_DGRAM, 0);
	ASSERT(sfd > 0, "server socket");
	i32 cfd = socket(AF_INET, SOCK_DGRAM, 0);
	ASSERT(cfd > 0, "client socket");

	res = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	ASSERT(!res, "setsockopt");
	res = bind(sfd, (struct sockaddr *)&addr, addrlen);
	ASSERT(!res, "bind");
	res = getsockname(sfd, (void *)&addr, &addrlen);
	ASSERT(!res, "getsockname");
	ASSERT(addr.sin_port > 0, "port");

	dest_addr.sin_port = addr.sin_port;

	res = sendmsg(cfd, &msg, 0);
	ASSERT_EQ(res, 6, "sendmsg");

	res = recvmsg(sfd, &msgout, 0);
	ASSERT_EQ(res, 6, "rcvmsg");
	ASSERT(!memcmp(msg_buf, "Hello1", 6), "equal msg");
	close(cfd);
	close(sfd);
}

Test(misc_sysext) {
	ASSERT_EQ(nsleep(1), -ETIME, "nsleep");
	ASSERT_EQ(usleep(1), -ETIME, "usleep");
	u8 buf[] = "abcdef", *ptr;
	ptr = smap(6);
	ASSERT(ptr);
	memcpy(ptr, buf, 6);
	ASSERT(!memcmp(ptr, buf, 6), "memcmp");
	u64 v = cycle_counter();
	yield();
	v = cycle_counter() - v;
	ASSERT(v, "cycle_counter");
}

Test(fork_wait) {
	i32 pid = fork();
	if (!pid) exit_group(0);
	ASSERT(!waitpid(pid), "waitpid");
}

Test(write_num) {
#define PATH "/tmp/write_num.dat"
	char buf[1024] = {0};
	char expected[] = "-92233720368547758080-123123";
	i32 fd = open(PATH, O_RDWR | O_CREAT, 0600);
	write_num(fd, (i64)(-0x7FFFFFFFFFFFFFFF - 1));
	write_num(fd, 0);
	write_num(fd, -123);
	write_num(fd, 123);
	pread(fd, buf, strlen(expected), 0);
	ASSERT(!memcmp(buf, expected, strlen(expected)), "expected");
	close(fd);
	unlink(PATH);
#undef PATH
}

Test(sysext_failures) {
	char buf[2];
	__global_sync = NULL;
	_debug_alloc_failure = 0;
	ASSERT(pwrite(2, "1\n", 2, -1) < 0);
	_debug_alloc_failure = 0;
	ASSERT(nsleep(1) < 0);
	_debug_alloc_failure = 0;
	unlink(NULL);
	_debug_alloc_failure = 0;
	statx(NULL, NULL);
	_debug_alloc_failure = 0;
	waitpid(0);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	ASSERT(usleep(1) < 0);
	_debug_alloc_failure = 0;
	ASSERT(pread(-1, buf, 2, 0) < 0);
	ASSERT(usleep(U64_MAX) < 0);
	_debug_alloc_failure = 0;
	fsync(0);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	fdatasync(0);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	open("", 0, 0);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	fallocate(0, 0);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	fstatx(0, NULL);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	socket(0, 0, 0);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	sendmsg(0, NULL, 0);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	recvmsg(0, NULL, 0);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	bind(0, NULL, 0);
	ASSERT(!__global_sync, "gs");
	_debug_alloc_failure = 0;
	ASSERT(!fmap(0, 100, 0));
	_debug_alloc_failure = 0;
	ASSERT(!smap(100));
	_debug_alloc_failure = I64_MAX;

	ASSERT(write_num(-1, 0) < 0);
	i64 res;

	_debug_syscall_return = -1;
	res = micros();
	_debug_syscall_return = 0;
	ASSERT(res < 0);

	_debug_pwrite_return = 1;
	res = write_num(2, (i64)(-0x7FFFFFFFFFFFFFFF - 1));
	_debug_pwrite_return = 0;
	ASSERT_EQ(res, -EIO, "case 1 expected {}, found {}", -EIO, res);

	_debug_pwrite_return = -47;
	res = write_num(2, (i64)(-0x5FFFFFFFFFFFFFFF - 1));
	_debug_pwrite_return = 0;
	ASSERT_EQ(res, -47, "-47");

	_debug_pwrite_return = -47;
	res = write_num(2, (i64)(-0x7FFFFFFFFFFFFFFF - 1));
	_debug_pwrite_return = 0;
	ASSERT_EQ(res, -47, "-47");

	_debug_pwrite_return = 1;
	res = write_num(2, (i64)(-0x5FFFFFFFFFFFFFFF - 1));
	_debug_pwrite_return = 0;
	ASSERT_EQ(res, -EIO, "case 2 expected {}, found {}", -EIO, res);
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */
#endif /* SYSEXT_IMPL_GUARD */
#endif /* SYSEXT_IMPL */

