int main(int argc, char **argv, char **envp);
#ifdef __x86_64__
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
#elif defined(__aarch64__)
__asm__(
    ".section .text\n"
    ".global _start\n"
    "_start:\n"
    "    ldr x0, [sp]\n"
    "    add x1, sp, #8\n"
    "    add x3, x0, #1\n"
    "    lsl x3, x3, #3\n"
    "    add x2, x1, x3\n"
    "    mov x4, sp\n"
    "    bic x4, x4, #15\n"
    "    mov sp, x4\n"
    "    bl main\n");
#endif /* !__aarch64__ */

#define TEST

#include <libfam/test.h>

#define ARENA_IMPL
#define FORMAT_IMPL
#define COLORS_IMPL
#define RBTREE_IMPL
#define ENV_IMPL
#define SYSCALL_IMPL
#define TYPES_IMPL
#define SYSEXT_IMPL
#define STRING_IMPL
#define STUBS_IMPL
#define SYNC_IMPL

#include <libfam/arena.h>
#include <libfam/atomic.h>
#include <libfam/colors.h>
#include <libfam/env.h>
#include <libfam/format.h>
#include <libfam/string.h>
#include <libfam/stubs.h>
#include <libfam/sync.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/test.h>
#include <libfam/types.h>

i32 cur_tests = 0;
i32 exe_test = 0;

TestEntry tests[MAX_TESTS];
TestEntry benches[MAX_TESTS];
TestEntry *active;

extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);

const char *SPACER =(void*)
    "------------------------------------------------------------------"
    "--------------------------\n";

int main(int argc, char **argv, char **envp) {
	i32 count, i, test_count;
	Arena *a = NULL;
	test_count = 0;
	count = __init_array_end - __init_array_start;
	for (i = 0; i < count; i++) __init_array_start[i]();

	arena_init(&a, 1024 * 1024 * 16, 8);
	init_environ(envp, a);

	pwrite(2, SPACER, __builtin_strlen(SPACER), -1);

	for (exe_test = 0; exe_test < cur_tests; exe_test++) {
		const char *msg = "Running test ";
		pwrite(2, msg, __builtin_strlen(msg), -1);
		write_num(2, ++test_count);
		pwrite(2, " (", 2, -1);
		pwrite(2, tests[exe_test].name,
		       __builtin_strlen(tests[exe_test].name), -1);
		pwrite(2, ")\n", 2, -1);
		tests[exe_test].test_fn();
	}

	pwrite(2, SPACER, __builtin_strlen(SPACER), -1);
	pwrite(2, "Success!\n", 9, -1);

	exit_group(0);
	(void)argc;
	(void)argv;
	(void)envp;
}

void add_test_fn(void (*test_fn)(void), const char *name) {
	if (__builtin_strlen((const void *)name) > MAX_TEST_NAME) {
		const char *msg = "Test name too long!";
		pwrite(2, msg, __builtin_strlen(msg), -1);
		exit_group(-1);
	}
	if (cur_tests >= MAX_TESTS) {
		const char *msg = "Too many tests!";
		pwrite(2, msg, __builtin_strlen(msg), -1);
		exit_group(-1);
	}

	tests[cur_tests].test_fn = test_fn;
	__builtin_memset(tests[cur_tests].name, 0, MAX_TEST_NAME);
	__builtin_strcpy((void *)tests[cur_tests].name, (const void *)name);
	cur_tests++;
}

