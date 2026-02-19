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
#define AESENC_IMPL
#define LIMITS_IMPL
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
#define STORM_IMPL
#define FMT_IMPL
#define DATE_IMPL
#endif

#include <libfam/aesenc.h>
#include <libfam/arena.h>
#include <libfam/colors.h>
#include <libfam/date.h>
#include <libfam/env.h>
#include <libfam/fmt.h>
#include <libfam/format.h>
#include <libfam/storm.h>

i32 cur_tests = 0, cur_benches = 0, exe_test = 0;
bool is_bench = false;

typedef struct {
	void (*test_fn)(void);
	char name[MAX_TEST_NAME + 1];
} TestEntry;

TestEntry tests[MAX_TESTS];
TestEntry benches[MAX_TESTS];

PUBLIC const char *get_active(void) {
	return is_bench ? benches[exe_test].name : tests[exe_test].name;
}

const char *SPACER =(void*)
    "------------------------------------------------------------------"
    "--------------------------";

static int run_tests(char **envp) {
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
		char *filter = getenv("TEST_FILTER");
		i64 timer = 0;
		if (filter && strcmp(filter, "*") &&
		    strcmp(filter, tests[exe_test].name))
			continue;

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
		println("{}Success!{} {} {}tests passed!{} {}[{}µs]{}",
			BOLD_GREEN, RESET, cur_tests, BOLD_BLUE, RESET, GREEN,
			global_timer, RESET);
	else if (global_timer < 1000000)
		println("{}Success!{} {} {}tests passed!{} {}[{:.2}ms]{}",
			BOLD_GREEN, RESET, cur_tests, BOLD_BLUE, RESET, YELLOW,
			(f64)global_timer / 1000.0, RESET);
	else
		println("{}Success!{} {} {}tests passed!{} {}[{:.2}s]{}",
			BOLD_GREEN, RESET, cur_tests, BOLD_BLUE, RESET, RED,
			(f64)global_timer / 1000000.0, RESET);

	return 0;
}

static i32 run_benches(char **envp) {
	i64 global_timer;
	i32 count, test_count;
	Arena *a = NULL;
	test_count = 0;

	arena_init(&a, 1024 * 1024 * 16, 8);
	init_environ(envp, a);

	println("{}Running {} benches{}...", BOLD_BLUE, cur_benches, RESET);
	println("{}", SPACER);

	global_timer = micros();
	for (exe_test = 0; exe_test < cur_benches; exe_test++) {
		char *filter = getenv("TEST_FILTER");
		if (filter && strcmp(filter, "*") &&
		    strcmp(filter, benches[exe_test].name))
			continue;

		print("{}Running bench {} {} ({}{}{}) ", YELLOW, RESET,
		      ++test_count, DIMMED, benches[exe_test].name, RESET);
		benches[exe_test].test_fn();
	}
	global_timer = micros() - global_timer;
	println("{}", SPACER);
	println("{}Success!{} {} {}benches passed!{} {}[{:.2}s]{}", BOLD_GREEN,
		RESET, cur_benches, BOLD_BLUE, RESET, GREEN,
		(f64)global_timer / 1000000.0, RESET);

	return 0;
}

PUBLIC i32 main(i32 argc, char **argv, char **envp) {
	i32 status;
	if (argc >= 2 && !strcmp(argv[1], "bench")) {
		is_bench = true;
		status = run_benches(envp);
	} else {
		is_bench = false;
		status = run_tests(envp);
	}
#ifndef COVERAGE
	exit_group(status);
#endif /* COVERAGE */
}

PUBLIC void add_test_fn(void (*test_fn)(void), const char *name) {
	if (strlen(name) > MAX_TEST_NAME) {
		const char *msg = "Test name too long!";
		pwrite(2, msg, strlen(msg), -1);
		exit_group(-1);
	}
	if (cur_tests >= MAX_TESTS) {
		const char *msg = "Too many tests!";
		pwrite(2, msg, strlen(msg), -1);
		exit_group(-1);
	}

	tests[cur_tests].test_fn = test_fn;
	memset(tests[cur_tests].name, 0, MAX_TEST_NAME);
	strcpy(tests[cur_tests].name, name);
	cur_tests++;
}

PUBLIC void add_bench_fn(void (*bench_fn)(void), const char *name) {
	if (strlen(name) > MAX_TEST_NAME) {
		const char *msg = "Bench name too long!";
		pwrite(2, msg, strlen(msg), -1);
		exit_group(-1);
	}
	if (cur_benches >= MAX_TESTS) {
		const char *msg = "Too many benches!";
		pwrite(2, msg, strlen(msg), -1);
		exit_group(-1);
	}

	benches[cur_benches].test_fn = bench_fn;
	memset(benches[cur_benches].name, 0, MAX_TEST_NAME);
	strcpy(benches[cur_benches].name, name);
	cur_benches++;
}
/* GCOVR_EXCL_STOP */

