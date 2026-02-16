#ifndef _COLORS_H
#define _COLORS_H

#include <libfam/types.h>

const char *get_dimmed(void);
#define DIMMED get_dimmed()

const char *get_red(void);
#define RED get_red()

const char *get_bright_red(void);
#define BRIGHT_RED get_bright_red()

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
PUBLIC const char *get_green(void) { return no_color() ? "" : "\x1b[32m"; }
PUBLIC const char *get_yellow(void) { return no_color() ? "" : "\x1b[33m"; }
PUBLIC const char *get_cyan(void) { return no_color() ? "" : "\x1b[36m"; }
PUBLIC const char *get_magenta(void) { return no_color() ? "" : "\x1b[35m"; }
PUBLIC const char *get_blue(void) { return no_color() ? "" : "\x1b[34m"; }
PUBLIC const char *get_reset(void) { return no_color() ? "" : "\x1b[0m"; }

#endif /* COLORS_IMPL_GUARD */
#endif /* COLORS_IMPL */
