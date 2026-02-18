/********************************************************************************
 * MIT License
 *
 * Copyright (c) 2026 Christopher Gilliard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/

#ifndef _TEST_H
#define _TEST_H

#include <libfam/colors.h>
#include <libfam/format.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/types.h>
#include <libfam/utils.h>

#define MAX_TESTS 1024
#define MAX_TEST_NAME 128

void add_test_fn(void (*test_fn)(void), const char *name);
void add_bench_fn(void (*bench_fn)(void), const char *name);
static __attribute__((unused)) const char *__assertion_msg =
    "\nAssertion failed in test";
const char *get_active(void);

/* GCOVR_EXCL_START */

#define Test(name)                                                  \
	void __test_##name(void);                                   \
	void __add_test_##name(void);                               \
	void __attribute__((constructor)) __add_test_##name(void) { \
		add_test_fn(__test_##name, #name);                  \
	}                                                           \
	void __test_##name(void)

#define ASSERT_EQ(x, y, ...)                                                   \
	({                                                                     \
		if ((x) != (y)) {                                              \
			Formatter fmt = FORMATTER_INIT;                        \
			__VA_OPT__(FORMAT(&fmt, __VA_ARGS__);)                 \
			panic("{}{}{}: [{}]. '{}'", BOLD_RED, __assertion_msg, \
			      RESET, get_active(), format_to_string(&fmt));    \
		}                                                              \
	})

#define ASSERT(x, ...)                                                         \
	({                                                                     \
		if (!(x)) {                                                    \
			Formatter fmt = FORMATTER_INIT;                        \
			__VA_OPT__(FORMAT(&fmt, __VA_ARGS__);)                 \
			panic("{}{}{}: [{}]. '{}'", BOLD_RED, __assertion_msg, \
			      RESET, get_active(), format_to_string(&fmt));    \
		}                                                              \
	})

/* GCOVR_EXCL_STOP */

#endif /* _TEST_H */
