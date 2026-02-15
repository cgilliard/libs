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

void exit_group(long status) { syscall(&status, 231, status, 0, 0, 0, 0, 0); }

long write(long fd, const void *buf, long len) {
	long ret;
	syscall(&ret, 1, fd, (long)buf, len, 0, 0, 0);
	return ret;
}

int main(int argc, char **argv, char **envp) {
	long r;
	char out[1024];

	write(2, "hi\n", 3);
	r = compress_block("ok", 2, out, 1024);

	exit_group(r);
	return r;
	(void)envp;
	(void)argv;
	(void)argc;
	(void)compress_bound;
	(void)decompress_block;
}
