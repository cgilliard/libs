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

#include <libfam/test.h>

/*
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
*/

#include <libfam/arena.h>
#include <libfam/atomic.h>
#include <libfam/colors.h>
#include <libfam/env.h>
#include <libfam/format.h>
#include <libfam/string.h>
#include <libfam/stubsx.h>
#include <libfam/sync.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/test.h>
#include <libfam/types.h>

void __stack_chk_fail(void);
void __stack_chk_fail(void) {}

i32 cur_tests = 0;
i32 exe_test = 0;

TestEntry tests[MAX_TESTS];
TestEntry benches[MAX_TESTS];
TestEntry *active;

const char *SPACER =(void*)
    "------------------------------------------------------------------"
    "--------------------------";

int main(int argc, char **argv, char **envp) {
	i32 count, test_count;
	Arena *a = NULL;
	test_count = 0;

	arena_init(&a, 1024 * 1024 * 16, 8);
	init_environ(envp, a);

	u128 x = 0;
	println("{} {}", x, SPACER);

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

	println("{}", SPACER);
	println("Success!");

	exit_group(0);
	(void)argc;
	(void)argv;
}

PUBLIC void add_test_fn(void (*test_fn)(void), const char *name) {
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

