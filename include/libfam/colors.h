#ifndef _COLORS_H
#define _COLORS_H

#include <libfam/types.h>

const char *get_dimmed(void);
#define DIMMED get_dimmed()

const char *get_red(void);
#define RED get_red()

const char *get_bright_red(void);
#define BRIGHT_RED get_bright_red()

const char *get_bold_red(void);
#define BOLD_RED get_bold_red()

const char *get_green(void);
#define GREEN get_green()

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
PUBLIC const char *get_bold_red(void) { return no_color() ? "" : "\x1b[1;31m"; }
PUBLIC const char *get_green(void) { return no_color() ? "" : "\x1b[32m"; }
PUBLIC const char *get_yellow(void) { return no_color() ? "" : "\x1b[33m"; }
PUBLIC const char *get_cyan(void) { return no_color() ? "" : "\x1b[36m"; }
PUBLIC const char *get_magenta(void) { return no_color() ? "" : "\x1b[35m"; }
PUBLIC const char *get_blue(void) { return no_color() ? "" : "\x1b[34m"; }
PUBLIC const char *get_bold_blue(void) {
	return no_color() ? "" : "\x1b[1;34m";
}
PUBLIC const char *get_reset(void) { return no_color() ? "" : "\x1b[0m"; }

/* GCOVR_EXCL_START */
#ifdef TEST

#include <libfam/env.h>
#include <libfam/format.h>
#include <libfam/test.h>

Test(colors1) {
	Formatter f = {0};
	FORMAT(&f, "{}test{}", RED, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[31mtest\x1b[0m"), "red");
	format_clear(&f);
	FORMAT(&f, "{}test{}", BLUE, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[34mtest\x1b[0m"), "blue");
	format_clear(&f);
	FORMAT(&f, "{}test{}", DIMMED, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[2mtest\x1b[0m"), "dimmed");
	format_clear(&f);
	FORMAT(&f, "{}test{}", BRIGHT_RED, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[91mtest\x1b[0m"),
	       "bright_red");
	format_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_RED, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[1;31mtest\x1b[0m"),
	       "bold red");
	format_clear(&f);
	FORMAT(&f, "{}test{}", GREEN, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[32mtest\x1b[0m"), "green");
	format_clear(&f);
	FORMAT(&f, "{}test{}", YELLOW, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[33mtest\x1b[0m"), "yellow");
	format_clear(&f);
	FORMAT(&f, "{}test{}", CYAN, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[36mtest\x1b[0m"), "cyan");
	format_clear(&f);
	FORMAT(&f, "{}test{}", MAGENTA, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[35mtest\x1b[0m"), "magenta");
	format_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_BLUE, RESET);
	ASSERT(!strcmp(format_to_string(&f), "\x1b[1;34mtest\x1b[0m"),
	       "bold blue");
	format_clear(&f);
}

Test(colors2) {
	Arena *a = NULL;
	Formatter f = {0};

	arena_init(&a, 16834, 8);
	_debug_insert_env("NO_COLOR", "1", a);

	FORMAT(&f, "{}test{}", RED, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "red");
	format_clear(&f);
	FORMAT(&f, "{}test{}", BLUE, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "blue");
	format_clear(&f);
	FORMAT(&f, "{}test{}", DIMMED, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "dimmed");
	format_clear(&f);
	FORMAT(&f, "{}test{}", BRIGHT_RED, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "bright_red");
	format_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_RED, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "bold red");
	format_clear(&f);
	FORMAT(&f, "{}test{}", GREEN, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "green");
	format_clear(&f);
	FORMAT(&f, "{}test{}", YELLOW, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "yellow");
	format_clear(&f);
	FORMAT(&f, "{}test{}", CYAN, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "cyan");
	format_clear(&f);
	FORMAT(&f, "{}test{}", MAGENTA, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "magenta");
	format_clear(&f);
	FORMAT(&f, "{}test{}", BOLD_BLUE, RESET);
	ASSERT(!strcmp(format_to_string(&f), "test"), "bold blue");
	format_clear(&f);

	_debug_remove_env("NO_COLOR", "1");
	arena_destroy(a);
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* COLORS_IMPL_GUARD */
#endif /* COLORS_IMPL */
