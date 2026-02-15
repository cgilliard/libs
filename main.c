#define COMPRESS_IMPL
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
	long r, size;
	int fd;
	char *in;
	char out[MAX_COMPRESS_LEN + 3];

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

	exit_group(r);
	return r;
	(void)envp;
	(void)argv;
	(void)argc;
	(void)compress_bound;
	(void)decompress_block;
}
