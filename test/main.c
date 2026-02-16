#include <libfam/atomic.h>
#include <libfam/sync.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/types.h>

void *memmove(void *dest, const void *src, u64 n);
void *memmove(void *dest, const void *src, u64 n) {
	u8 *d = (void *)((u8 *)dest + n);
	const u8 *s = (const void *)((const u8 *)src + n);
	while (n--) d--, s--, *d = *s;
	return dest;
}

void *memset(void *dest, i32 c, u64 n);
void *memset(void *dest, i32 c, u64 n) {
	u8 *tmp = dest;
	while (n--) *tmp++ = (char)c;
	return dest;
}

void _start(void);
void _start(void) {
	i32 y = 0;

	pwrite(2, "abc\n", 4, -1);
x:
	y++;
	if (y == 1000000) goto next;
	goto x;
next:
	exit_group(123);
}

#define SYNC_IMPL
#define SYSCALL_IMPL
#define TYPES_IMPL
#define SYSEXT_IMPL
#include <libfam/sync.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/types.h>
