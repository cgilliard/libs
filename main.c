#define COMPRESS_IMPL
#define COMPRESS_IO_IMPL
#include "include/compress.h"

__asm__(
    ".section .text\n"
    "syscall:\n"
    "push    %rbp\n"
    "mov     %rdi, %rbp\n"
    "mov     %rsi, %rax\n"
    "mov     %rdx, %rdi\n"
    "mov     %rcx, %rsi\n"
    "mov     %r8,  %rdx\n"
    "mov     %r9,  %r10\n"
    "mov     16(%rsp), %r8\n"
    "mov     24(%rsp), %r9\n"
    "syscall\n"
    "mov     %rax, 0(%rbp)\n"
    "pop     %rbp\n"
    "ret\n");

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
    "    call main\n");

void syscall(void *ret, long sysno, long a0, long a1, long a2, long a3, long a4,
	     long a5);
void exit_group(long status);
long write(long fd, const void *buf, long len);
void mmap(void **ret, void *adr, long length, long prot, long flags, long fd,
	  long offset);

void exit_group(long status) { syscall(&status, 231, status, 0, 0, 0, 0, 0); }

long write(long fd, const void *buf, long len) {
	long ret;
	syscall(&ret, 1, fd, (long)buf, len, 0, 0, 0);
	return ret;
}

void mmap(void **ret, void *addr, long length, long prot, long flags, long fd,
	  long offset) {
	syscall(ret, 9, (long)addr, length, prot, flags, fd, offset);
}

static unsigned long strlen(const char *x) {
	const char *y = x;
	while (*x) x++;
	return x - y;
}

void *memmove(void *dest, const void *src, unsigned long n);
void *memmove(void *dest, const void *src, unsigned long n) {
	unsigned char *d = (void *)((unsigned char *)dest + n);
	const unsigned char *s = (const void *)((const unsigned char *)src + n);
	while (n--) d--, s--, *d = *s;
	return dest;
}

static long write_str(long fd, char *msg) {
	long len = strlen(msg);
	return write(fd, msg, len);
}

static long write_num(int fd, long num) {
	unsigned char buf[21];
	unsigned char *p;
	unsigned long len;
	long written;
	int negative = 0;

	if (fd < 0) return -1;
	p = buf + sizeof(buf) - 1;
	*p = '\0';

	if (num < 0) {
		negative = 1;
		if (num == ((long)(-0x7FFFFFFFFFFFFFFF - 1))) {
			const char min_str[] = "-9223372036854775808";
			len = sizeof(min_str) - 1;
			written = write(fd, min_str, len);
			if (written < 0) return -1;
			if ((unsigned long)written != len) return -1;
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
	written = write(fd, p, len);
	if (written < 0) return -1;
	if ((unsigned long)written != len) return -1;
	return 0;
}

static void *fmap_ro(long fd, long size, long offset) {
	void *ret = 0;
	mmap(&ret, 0, size, 1, 1, fd, offset);
	if ((long)ret < 0) ret = 0;
	return ret;
}

static int open(const char *pathname, int flags, int mode) {
	long ret;
	syscall(&ret, 2, (long)pathname, flags, mode, 0, 0, 0);
	return ret;
}

static long lseek(long fd, long offset, long whence) {
	long ret;
	syscall(&ret, 8, fd, offset, whence, 0, 0, 0);
	return ret;
}

int main(int argc, char **argv, char **envp) {
	long r, size, i;
	int fd;
	char *in;
	char out[MAX_COMPRESS_LEN + 3];
	char verify[1024 * 1024];

	if (argc < 2) {
		write_str(2, "Usage command <pathname>\n");
		exit_group(-1);
	}

	fd = open(argv[1], 0, 0);
	if (fd < 0) {
		write_str(2, "could not open file\n");
		exit_group(-1);
	}

	size = lseek(fd, 0, 2);
	in = fmap_ro(fd, size, 0);

	r = compress_block(in, size, out, sizeof(out));
	write_num(2, r);
	write_str(2, "\n");

	r = decompress_block(out, r, verify, sizeof(verify));
	write_num(2, r);
	write_str(2, "\n");

	if (r != size) {
		write_str(2, "size mismatch!\n");
		exit_group(-1);
	}

	for (i = 0; i < r; i++) {
		if (verify[i] != in[i]) {
			write_str(2, "ne!\n");
			exit_group(-1);
		}
	}
	write_str(2, "equal!\n");

	exit_group(r);
	return r;
	(void)envp;
	(void)argv;
	(void)argc;
	(void)compress_bound;
	(void)decompress_block;
	(void)write_num;
}
