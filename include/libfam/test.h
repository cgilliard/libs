#ifndef _TEST_H
#define _TEST_H

#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/types.h>

#define MAX_TESTS 1024
#define MAX_TEST_NAME 128

void add_test_fn(void (*test_fn)(void), const char *name);
void add_bench_fn(void (*bench_fn)(void), const char *name);
static __attribute__((unused)) const char *__assertion_msg =
    "\nassertion failed in test";

extern i32 exe_test;
typedef struct {
	void (*test_fn)(void);
	char name[MAX_TEST_NAME + 1];
} TestEntry;
extern TestEntry *active;

#define Test(name)                                                         \
	void __test_##name(void);                                          \
	static void __attribute__((constructor)) __add_test_##name(void) { \
		add_test_fn(__test_##name, #name);                         \
	}                                                                  \
	void __test_##name(void)

#define ASSERT(x, ...)                               \
	({                                           \
		if (!(x)) {                          \
			pwrite(2, "fail!\n", 6, -1); \
			exit_group(-1);              \
		}                                    \
	})

#endif /* _TEST_H */
