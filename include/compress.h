#ifndef _COMPRESS_H
#define _COMPRESS_H

#define MAX_COMPRESS_LEN (1 << 18)

static long compress_bound(unsigned source_len);
static int compress_block(const void *in, unsigned len, void *out,
			  unsigned capacity);
static int decompress_block(const void *in, unsigned len, void *out,
			    unsigned capacity);

#ifdef COMPRESS_IMPL
#ifndef COMPRESS_IMPL_GUARD
#define COMPRESS_IMPL_GUARD

__asm__(
    ".section .text\n"
    "compress_syscall:\n"
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

void *memset(void *dest, int c, unsigned long n);
void *memset(void *dest, int c, unsigned long n) {
	char *tmp = dest;
	while (n--) *tmp++ = (char)c;
	return dest;
}

static void compress_zero_memory(void *ptr, long len_bytes) {
#ifdef __AVX2__
	char *p = (char *)ptr;
	char *end = p + len_bytes;
	__asm__ __volatile__(
	    "vpxor  %%ymm0, %%ymm0, %%ymm0\n\t"
	    "0:\n\t"
	    "vmovdqu %%ymm0, (%0)\n\t"
	    "add    $32, %0\n\t"
	    "cmp    %0, %1\n\t"
	    "jb     0b"
	    : "+r"(p)
	    : "r"(end)
	    : "ymm0", "cc", "memory");
#else
	memset(ptr, 0, len_bytes);
#endif
}

void compress_syscall(void *ret, long sysno, long a0, long a1, long a2, long a3,
		      long a4, long a5);

static long compress_bound(unsigned source_len) { return (long)source_len + 3; }

static int compress_block(const void *in, unsigned len, void *out,
			  unsigned capacity) {
	long ret;
	char arr[1024];

	arr[0] = 123;
	compress_zero_memory(arr, sizeof(arr));
	compress_syscall(&ret, 1, 2, (long)"abc\n", 4, 0, 0, 0);
	return arr[0];
	(void)in;
	(void)len;
	(void)out;
	(void)capacity;
}

static int decompress_block(const void *in, unsigned len, void *out,
			    unsigned capacity) {
	return 0;
	(void)in;
	(void)len;
	(void)out;
	(void)capacity;
}

#endif
#endif
#endif /* _COMPRESS_H */
