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

#ifndef _COLORS_H
#define _COLORS_H

#include <libfam/types.h>

const char *get_dimmed(void);
#define DIMMED get_dimmed()

const char *get_red(void);
#define RED get_red()

const char *get_bold_red(void);
#define BOLD_RED get_bold_red()

const char *get_bright_red(void);
#define BRIGHT_RED get_bright_red()

const char *get_bold_bright_red(void);
#define BOLD_BRIGHT_RED get_bold_bright_red()

const char *get_green(void);
#define GREEN get_green()

const char *get_bold_green(void);
#define BOLD_GREEN get_bold_green()

const char *get_yellow(void);
#define YELLOW get_yellow()

const char *get_cyan(void);
#define CYAN get_cyan()

const char *get_magenta(void);
#define MAGENTA get_magenta()

const char *get_blue(void);
#define BLUE get_blue()

const char *get_bold_blue(void);
#define BOLD_BLUE get_bold_blue()

const char *get_reset(void);
#define RESET get_reset()

const char *get_bright_green(void);
#define BRIGHT_GREEN get_bright_green()

const char *get_bold_bright_green(void);
#define BOLD_BRIGHT_GREEN get_bold_bright_green()

const char *get_bright_yellow(void);
#define BRIGHT_YELLOW get_bright_yellow()

const char *get_bold_bright_yellow(void);
#define BOLD_BRIGHT_YELLOW get_bold_bright_yellow()

const char *get_bright_cyan(void);
#define BRIGHT_CYAN get_bright_cyan()

const char *get_bold_bright_cyan(void);
#define BOLD_BRIGHT_CYAN get_bold_bright_cyan()

const char *get_bright_magenta(void);
#define BRIGHT_MAGENTA get_bright_magenta()

const char *get_bold_bright_magenta(void);
#define BOLD_BRIGHT_MAGENTA get_bold_bright_magenta()

#endif /* _COLORS_H */

#ifdef COLORS_IMPL
#ifndef COLORS_IMPL_GUARD
#define COLORS_IMPL_GUARD

#include <libfam/env.h>
#include <libfam/types.h>
#include <libfam/utils.h>

static i32 no_color(void) { return getenv("NO_COLOR") != NULL; }
PUBLIC const char *get_dimmed(void) { return no_color() ? "" : "\x1b[2m"; }
PUBLIC const char *get_red(void) { return no_color() ? "" : "\x1b[31m"; }
PUBLIC const char *get_bright_red(void) { return no_color() ? "" : "\x1b[91m"; }
PUBLIC const char *get_bold_bright_red(void) {
	return no_color() ? "" : "\x1b[1;91m";
}
PUBLIC const char *get_bold_red(void) { return no_color() ? "" : "\x1b[1;31m"; }
PUBLIC const char *get_green(void) { return no_color() ? "" : "\x1b[32m"; }
PUBLIC const char *get_bold_green(void) {
	return no_color() ? "" : "\x1b[1;32m";
}
PUBLIC const char *get_yellow(void) { return no_color() ? "" : "\x1b[33m"; }
PUBLIC const char *get_cyan(void) { return no_color() ? "" : "\x1b[36m"; }
PUBLIC const char *get_magenta(void) { return no_color() ? "" : "\x1b[35m"; }
PUBLIC const char *get_blue(void) { return no_color() ? "" : "\x1b[34m"; }
PUBLIC const char *get_bold_blue(void) {
	return no_color() ? "" : "\x1b[1;34m";
}
PUBLIC const char *get_reset(void) { return no_color() ? "" : "\x1b[0m"; }
PUBLIC const char *get_bright_green(void) {
	return no_color() ? "" : "\x1b[92m";
}
PUBLIC const char *get_bold_bright_green(void) {
	return no_color() ? "" : "\x1b[1;92m";
}

PUBLIC const char *get_bright_yellow(void) {
	return no_color() ? "" : "\x1b[93m";
}
PUBLIC const char *get_bold_bright_yellow(void) {
	return no_color() ? "" : "\x1b[1;93m";
}

PUBLIC const char *get_bright_cyan(void) {
	return no_color() ? "" : "\x1b[96m";
}
PUBLIC const char *get_bold_bright_cyan(void) {
	return no_color() ? "" : "\x1b[1;96m";
}

PUBLIC const char *get_bright_magenta(void) {
	return no_color() ? "" : "\x1b[95m";
}
PUBLIC const char *get_bold_bright_magenta(void) {
	return no_color() ? "" : "\x1b[1;95m";
}

/* GCOVR_EXCL_START */
#ifdef TEST

#include <libfam/env.h>
#include <libfam/fmt.h>
#include <libfam/string.h>
#include <libfam/test.h>

Test(colors1) {
	Fmt f = {0};
	FORMAT(&f, "{}test{}", RED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[31mtest\x1b[0m"), "red");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BLUE, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[34mtest\x1b[0m"), "blue");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", DIMMED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[2mtest\x1b[0m"), "dimmed");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BRIGHT_RED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[91mtest\x1b[0m"),
	       "bright_red");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_RED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[1;31mtest\x1b[0m"),
	       "bold red");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", GREEN, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[32mtest\x1b[0m"), "green");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", YELLOW, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[33mtest\x1b[0m"), "yellow");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", CYAN, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[36mtest\x1b[0m"), "cyan");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", MAGENTA, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[35mtest\x1b[0m"), "magenta");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_BLUE, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[1;34mtest\x1b[0m"),
	       "bold blue");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_GREEN, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[1;32mtest\x1b[0m"),
	       "bold green");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_BRIGHT_RED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "\x1b[1;91mtest\x1b[0m"),
	       "bold bright red");
	fmt_clear(&f);

	fmt_clear(&f);
}

Test(colors2) {
	Arena *a = NULL;
	Fmt f = {0};

	arena_init(&a, 16834, 8);
	_debug_insert_env("NO_COLOR", "1", a);

	FORMAT(&f, "{}test{}", RED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "red");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BLUE, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "blue");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", DIMMED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "dimmed");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BRIGHT_RED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "bright_red");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_RED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "bold red");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", GREEN, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "green");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", YELLOW, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "yellow");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", CYAN, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "cyan");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", MAGENTA, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "magenta");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_BLUE, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "bold blue");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_GREEN, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "bold green");
	fmt_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_BRIGHT_RED, RESET);
	ASSERT(!strcmp(fmt_to_string(&f), "test"), "bold bright red");
	fmt_clear(&f);

	_debug_remove_env("NO_COLOR", "1");
	arena_destroy(a);
}

#endif /* TEST */
       /* GCOVR_EXCL_STOP */

#endif /* COLORS_IMPL_GUARD */
#endif /* COLORS_IMPL */
