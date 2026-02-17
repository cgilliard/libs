/* GCOVR_EXCL_START */
int main(int argc, char **argv, char **envp);

#ifndef COVERAGE
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
#endif /* __aarch64__ */
#endif /* !COVERAGE */

#include <libfam/test.h>

#ifdef COVERAGE
#define DEBUG_IMPL
#define FORMAT_IMPL
#define ARENA_IMPL
#define ENV_IMPL
#define COLORS_IMPL
#define STUBS_IMPL
#define SYNC_IMPL
#define TYPES_IMPL
#define STRING_IMPL
#define SYSCALL_IMPL
#define SYSEXT_IMPL
#define RBTREE_IMPL
#define ERRNO_IMPL
#include <libfam/colors.h>
#endif

#include <libfam/arena.h>
#include <libfam/env.h>
#include <libfam/format.h>

i32 cur_tests = 0;
i32 exe_test = 0;

typedef struct {
	void (*test_fn)(void);
	char name[MAX_TEST_NAME + 1];
} TestEntry;

TestEntry tests[MAX_TESTS];
TestEntry benches[MAX_TESTS];

PUBLIC const char *get_active(void) { return tests[exe_test].name; }

const char *SPACER =(void*)
    "------------------------------------------------------------------"
    "--------------------------";

int main(int argc, char **argv, char **envp) {
	i64 global_timer;
	i32 count, test_count;
	Arena *a = NULL;
	test_count = 0;

	arena_init(&a, 1024 * 1024 * 16, 8);
	init_environ(envp, a);

	println("{}Running {} tests{}...", BOLD_BLUE, cur_tests, RESET);
	println("{}", SPACER);

	global_timer = micros();
	for (exe_test = 0; exe_test < cur_tests; exe_test++) {
		i64 timer = 0;
		print("{}Running test{} {} ({}{}{})", YELLOW, RESET,
		      ++test_count, DIMMED, tests[exe_test].name, RESET);
		timer = micros();
		tests[exe_test].test_fn();
		timer = micros() - timer;
		if (timer < 1000)
			println(" {}[{}µs]{}", GREEN, timer, RESET);
		else if (timer < 1000000)
			println(" {}[{:.2}ms]{}", YELLOW, (f64)timer / 1000.0,
				RESET);
		else
			println(" {}[{:.2}s]{}", BOLD_RED,
				(f64)timer / 1000000.0, RESET);
	}
	global_timer = micros() - global_timer;
	println("{}", SPACER);
	if (global_timer < 1000)
		println("{}Success!{} {} {}tests passed!{} {}[{}µs]{}", GREEN,
			RESET, cur_tests, BOLD_BLUE, RESET, GREEN, global_timer,
			RESET);
	else if (global_timer < 1000000)
		println("{}Success!{} {} {}tests passed!{} {}[{:.2}ms]{}",
			GREEN, RESET, cur_tests, BOLD_BLUE, RESET, YELLOW,
			(f64)global_timer / 1000.0, RESET);
	else
		println("{}Success!{} {} {}tests passed!{} {}[{:.2}s]{}", GREEN,
			RESET, cur_tests, BOLD_BLUE, RESET, RED,
			(f64)global_timer / 1000000.0, RESET);

#ifndef COVERAGE
	exit_group(0);
#endif /* COVERAGE */
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
/* GCOVR_EXCL_STOP */

