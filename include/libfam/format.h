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

#ifndef _FORMAT_H
#define _FORMAT_H

/*****************************************************************************
 *
```
sudo ./build install
```
 * test.c
```
#include <libfam/format.h>

i32 main(void) {
    println("{} + {} = {}", 1, 1, 2);
    return 0;
}
```
 *
```
cc -DFORMAT_ALL_IMPL test.c -o test
```
 *
 ****************************************************************************/

#include <libfam/string.h>
#include <libfam/sysext.h>
#include <libfam/types.h>
#include <libfam/utils.h>

#define FORMATTER_INIT {0};

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored \
    "-Wincompatible-pointer-types-discards-qualifiers"
#else
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif /* !__clang__ */

#define FORMAT_ITEM(ign, value)                                                \
	({                                                                     \
		Printable _p__ = _Generic((value),                             \
		    char: ((Printable){                                        \
			.t = IntType,                                          \
			.data.ivalue =                                         \
			    _Generic((value), char: (value), default: 0)}),    \
		    signed char: ((Printable){.t = IntType,                    \
					      .data.ivalue = _Generic((value), \
					      signed char: (value),            \
					      default: 0)}),                   \
		    short int: ((Printable){.t = IntType,                      \
					    .data.ivalue = _Generic((value),   \
					    short int: (value),                \
					    default: 0)}),                     \
		    int: ((Printable){                                         \
			.t = IntType,                                          \
			.data.ivalue =                                         \
			    _Generic((value), int: (value), default: 0)}),     \
		    long: ((Printable){                                        \
			.t = IntType,                                          \
			.data.ivalue =                                         \
			    _Generic((value), long: (value), default: 0)}),    \
		    long long: ((Printable){.t = IntType,                      \
					    .data.ivalue = _Generic((value),   \
					    long long: (value),                \
					    default: 0)}),                     \
		    __int128_t: ((Printable){.t = IntType,                     \
					     .data.ivalue = _Generic((value),  \
					     __int128_t: (value),              \
					     default: 0)}),                    \
		    unsigned char: ((Printable){                               \
			.t = UIntType,                                         \
			.data.uvalue = _Generic((value),                       \
			unsigned char: (value),                                \
			default: 0)}),                                         \
		    unsigned short int: ((Printable){                          \
			.t = UIntType,                                         \
			.data.uvalue = _Generic((value),                       \
			unsigned short int: (value),                           \
			default: 0)}),                                         \
		    unsigned int: ((Printable){                                \
			.t = UIntType,                                         \
			.data.uvalue = _Generic((value),                       \
			unsigned int: (value),                                 \
			default: 0)}),                                         \
		    unsigned long: ((Printable){                               \
			.t = UIntType,                                         \
			.data.uvalue = _Generic((value),                       \
			unsigned long: (value),                                \
			default: 0)}),                                         \
		    unsigned long long: ((Printable){                          \
			.t = UIntType,                                         \
			.data.uvalue = _Generic((value),                       \
			unsigned long long: (value),                           \
			default: 0)}),                                         \
		    __uint128_t: ((Printable){.t = UIntType,                   \
					      .data.uvalue = _Generic((value), \
					      __uint128_t: (value),            \
					      default: 0)}),                   \
		    char *: ((Printable){.t = StringType,                      \
					 .data.svalue = _Generic((value),      \
					 char *: (value),                      \
					 default: NULL)}),                     \
		    const char *: ((Printable){                                \
			.t = StringType,                                       \
			.data.svalue = _Generic((value),                       \
			const char *: (value),                                 \
			default: NULL)}),                                      \
		    signed char *: ((Printable){                               \
			.t = StringType,                                       \
			.data.svalue = _Generic((value),                       \
			char *: (value),                                       \
			default: NULL)}),                                      \
		    u8 *: ((Printable){.t = StringType,                        \
				       .data.svalue = _Generic((value),        \
				       const u8 *: (value),                    \
				       u8 *: (value),                          \
				       default: NULL)}),                       \
		    const u8 *: ((Printable){.t = StringType,                  \
					     .data.svalue = _Generic((value),  \
					     const u8 *: (value),              \
					     default: NULL)}),                 \
		    void *: ((Printable){.t = UIntType,                        \
					 .data.uvalue = _Generic((value),      \
					 void *: ((u64)value),                 \
					 default: 0)}),                        \
		    double: ((Printable){.t = FloatType,                       \
					 .data.fvalue = _Generic((value),      \
					 double: (value),                      \
					 default: 0.0)}),                      \
		    float: ((Printable){                                       \
			.t = FloatType,                                        \
			.data.fvalue =                                         \
			    _Generic((value), float: (value), default: 0.0)}), \
		    default: ((Printable){                                     \
			.t = StringType,                                       \
			.data.svalue = (char *)"unsupported"}));               \
		_p__;                                                          \
	})

#pragma GCC diagnostic pop

/* GCOVR_EXCL_START */

#ifdef __clang__
#define FORMAT(f, fmt, ...)                                                    \
	({                                                                     \
		_Pragma("GCC diagnostic push");                                \
		/* clang-format off */                                       \
                _Pragma("GCC diagnostic ignored \"-Wincompatible-pointer-types-discards-qualifiers\""); \
		/* clang-format on */                                          \
		format_append(                                                 \
		    f, fmt __VA_OPT__(                                         \
			   , FOR_EACH(FORMAT_ITEM, _, (, ), __VA_ARGS__)));    \
		_Pragma("GCC diagnostic pop");                                 \
	})
#else
#define FORMAT(f, fmt, ...)                                                   \
	({                                                                    \
		_Pragma("GCC diagnostic push");                               \
		_Pragma("GCC diagnostic ignored \"-Wdiscarded-qualifiers\""); \
		format_append(                                                \
		    f, fmt __VA_OPT__(                                        \
			   , FOR_EACH(FORMAT_ITEM, _, (, ), __VA_ARGS__)));   \
		_Pragma("GCC diagnostic pop");                                \
	})
#endif

#define write_all(fd, s, len)                                        \
	({                                                           \
		u64 _wlen__ = 0, _to_write__ = len;                  \
		while (_wlen__ < _to_write__) {                      \
			i64 _result__ =                              \
			    pwrite(1, s + _wlen__, _to_write__, -1); \
			if (_result__ < 0) break;                    \
			_wlen__ += _result__;                        \
			_to_write__ -= _result__;                    \
		}                                                    \
	})

#define println(fmt, ...)                                                   \
	({                                                                  \
		const char *_tmp__;                                         \
		Formatter _f__ = {0};                                       \
		if (FORMAT(&_f__, fmt, __VA_ARGS__) >= 0) {                 \
			if (format_append(&_f__, "\n") >= 0) {              \
				_tmp__ = format_to_string(&_f__);           \
				if (_tmp__) write_all(1, _tmp__, _f__.pos); \
			}                                                   \
		}                                                           \
		format_clear(&_f__);                                        \
	})

#define print(fmt, ...)                                             \
	({                                                          \
		const char *_tmp__;                                 \
		Formatter _f__ = {0};                               \
		if (FORMAT(&_f__, fmt, __VA_ARGS__) >= 0) {         \
			_tmp__ = format_to_string(&_f__);           \
			if (_tmp__) write_all(1, _tmp__, _f__.pos); \
		}                                                   \
		format_clear(&_f__);                                \
	})

#define panic(fmt, ...)                                                     \
	({                                                                  \
		const char *_tmp__;                                         \
		Formatter _f__ = {0};                                       \
		if (FORMAT(&_f__, fmt, __VA_ARGS__) >= 0) {                 \
			if (format_append(&_f__, "\n") >= 0) {              \
				_tmp__ = format_to_string(&_f__);           \
				if (_tmp__) write_all(2, _tmp__, _f__.pos); \
			}                                                   \
		}                                                           \
		format_clear(&_f__);                                        \
		exit_group(-1);                                             \
	})

/* GCOVR_EXCL_STOP */

typedef struct {
	char *buf;
	u64 capacity;
	u64 pos;
} Formatter;

typedef enum {
	IntType,
	UIntType,
	StringType,
	FloatType,
	PRINTABLE_SIZE
} PrintableType;

typedef struct {
	PrintableType t;
	union {
		i128 ivalue;
		u128 uvalue;
		f64 fvalue;
		char *svalue;
	} data;
} Printable;

i32 format_append(Formatter *f, const char *fmt, ...);
void format_clear(Formatter *f);
const char *format_to_string(Formatter *f);

#ifdef FORMAT_ALL_IMPL
#define SYSCALL_IMPL
#define FORMAT_IMPL
#define STRING_IMPL
#define SYSEXT_IMPL
#define SYNC_IMPL
#define STUBS_IMPL
#endif

#endif /* _FORMAT_H */

#ifdef FORMAT_IMPL
#ifndef FORMAT_IMPL_GUARD
#define FORMAT_IMPL_GUARD

#include <libfam/debug.h>
#include <libfam/errno.h>
#include <libfam/limits.h>
#include <libfam/string.h>
#include <libfam/stubs.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/utils.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif /* PAGE_SIZE */
#define PAGE_MASK (~(PAGE_SIZE - 1))

typedef enum {
	FormatAlignLeft,
	FormatAlignRight,
} FormatAlignment;

typedef enum {
	FormatSpecTypeNone,
	FormatSpecTypeBinary,
	FormatSpecTypeHexUpper,
	FormatSpecTypeHexLower,
	FormatSpecTypeCommas,
	FormatSpecTypeChar,
	FormatSpecTypeEscapeBracketRight,
	FormatSpecTypeEscapeBracketLeft,
} FormatSpecType;

typedef struct {
	bool has_precision;
	u32 precision;
	bool has_width;
	u32 width;
	FormatAlignment align;
	FormatSpecType t;
	u32 total_bytes;
} FormatSpec;

static i32 format_parse_spec(const char *p, FormatSpec *spec) {
	FormatSpec ret = {.t = FormatSpecTypeNone, .total_bytes = 2};
	p++;
	while (*p) {
		if (*p == '.') {
			if (ret.has_precision) return -EPROTO;
			ret.has_precision = true;
			p++;
			if (*p > '9' || *p < '0') return -EPROTO;
			ret.precision = *p - '0';
			ret.total_bytes += 2;
			p++;
		} else if (*p == '{') {
			if (ret.has_precision || ret.has_width ||
			    ret.t != FormatSpecTypeNone)
				return -EPROTO;
			ret.t = FormatSpecTypeEscapeBracketLeft;
			break;
		} else if (*p == ':') {
			if (ret.has_width) return -EPROTO;
			ret.has_width = true;
			p++;
			ret.align = FormatAlignRight;
			if (*p == '<') {
				ret.align = FormatAlignLeft;
				p++;
				ret.total_bytes++;
			} else if (*p == '>') {
				ret.align = FormatAlignRight;
				p++;
				ret.total_bytes++;
			}

			ret.width = 0;

			while (*p >= '0' && *p <= '9') {
				ret.width = ret.width * 10 + (*p - '0');
				if (ret.width > 999) return -EPROTO;
				p++;
				ret.total_bytes++;
			}
			ret.total_bytes++;
		} else if (*p == 'c') {
			if (ret.t != FormatSpecTypeNone) return -EPROTO;
			ret.t = FormatSpecTypeChar;
			ret.total_bytes++;
			p++;
		} else if (*p == 'b') {
			if (ret.t != FormatSpecTypeNone) return -EPROTO;
			ret.t = FormatSpecTypeBinary;
			ret.total_bytes++;
			p++;
		} else if (*p == 'X') {
			if (ret.t != FormatSpecTypeNone) return -EPROTO;
			ret.t = FormatSpecTypeHexUpper;
			ret.total_bytes++;
			p++;
		} else if (*p == 'x') {
			if (ret.t != FormatSpecTypeNone) return -EPROTO;
			ret.t = FormatSpecTypeHexLower;
			ret.total_bytes++;
			p++;
		} else if (*p == 'n') {
			if (ret.t != FormatSpecTypeNone) return -EPROTO;
			ret.t = FormatSpecTypeCommas;
			ret.total_bytes++;
			p++;
		} else if (*p == '}') {
			break;
		} else
			return -EPROTO;
	}
	*spec = ret;
	return 0;
}

static const char *find_next_placeholder(const char *p, FormatSpec *spec) {
	while (*p) {
		if (*p == '{') {
			if (format_parse_spec(p, spec) < 0) {
				return NULL;
			}
			return p;
		} else if (*p == '}' && p[1] == '}') {
			*spec =
			    (FormatSpec){.t = FormatSpecTypeEscapeBracketRight,
					 .total_bytes = 2};
			return p;
		}
		p++;
	}
	return NULL;
}

static i32 format_try_resize(Formatter *f, u64 len) {
	u64 needed = len + f->pos;
	if (needed > f->capacity) {
		u64 to_alloc = (needed + PAGE_SIZE - 1) & PAGE_MASK;
		void *tmp = map(to_alloc);
		if (!tmp) return -1;

		memcpy(tmp, f->buf, f->pos);
		if (f->capacity) munmap(f->buf, f->capacity);
		f->buf = tmp;
		f->capacity = to_alloc;
	}

	return 0;
}

static Int128DisplayType format_get_displayType(const FormatSpec *spec) {
	if (spec->t == FormatSpecTypeNone)
		return Int128DisplayTypeDecimal;
	else if (spec->t == FormatSpecTypeBinary)
		return Int128DisplayTypeBinary;
	else if (spec->t == FormatSpecTypeHexUpper)
		return Int128DisplayTypeHexUpper;
	else if (spec->t == FormatSpecTypeCommas)
		return Int128DisplayTypeCommas;
	else
		return Int128DisplayTypeHexLower;
}

static i32 format_proc_padding(Formatter *f, const FormatSpec *spec,
			       const char *value, u64 raw_bytes) {
	i32 result;
	u64 aligned_bytes, i;
	aligned_bytes = !spec->has_width	  ? raw_bytes
			: spec->width > raw_bytes ? spec->width
						  : raw_bytes;
	if ((result = format_try_resize(f, aligned_bytes)) < 0) return result;
	if (spec->align == FormatAlignRight)
		for (i = raw_bytes; i < aligned_bytes; i++)
			f->buf[f->pos++] = ' ';
	memcpy(f->buf + f->pos, value, raw_bytes);
	f->pos += raw_bytes;
	if (spec->align == FormatAlignLeft)
		for (i = raw_bytes; i < aligned_bytes; i++)
			f->buf[f->pos++] = ' ';
	return 0;
}

static i32 format_proc_uint(Formatter *f, const FormatSpec *spec, u128 value) {
	char buf[MAX_U128_STRING_LEN];
	Int128DisplayType idt = format_get_displayType(spec);
	u64 raw_bytes;

#ifdef TEST
	if (_debug_proc_format_all) return -1;
#endif /* TEST */

	if (spec->t == FormatSpecTypeChar) {
		buf[0] = value <= I8_MAX ? value : '?';
		raw_bytes = 1;
	} else
		raw_bytes = u128_to_string(buf, value, idt);
	return format_proc_padding(f, spec, buf, raw_bytes);
}

static i32 format_proc_int(Formatter *f, const FormatSpec *spec, i128 value) {
	char buf[MAX_I128_STRING_LEN];
	Int128DisplayType idt = format_get_displayType(spec);
	u64 raw_bytes;

#ifdef TEST
	if (_debug_proc_format_all) return -1;
#endif /* TEST */

	if (spec->t == FormatSpecTypeChar) {
		buf[0] = value <= I8_MAX ? value : '?';
		raw_bytes = 1;
	} else
		raw_bytes = i128_to_string(buf, value, idt);
	return format_proc_padding(f, spec, buf, raw_bytes);
}

static i32 format_proc_string(Formatter *f, const FormatSpec *spec,
			      const char *value) {
	return format_proc_padding(f, spec, value, strlen(value));
}

static i32 format_proc_float(Formatter *f, const FormatSpec *spec, f64 value) {
	char buf[MAX_F64_STRING_LEN];
	u64 raw_bytes;

#ifdef TEST
	if (_debug_proc_format_all) return -1;
#endif /* TEST */

	raw_bytes =
	    f64_to_string(buf, value, spec->has_precision ? spec->precision : 5,
			  spec->t == FormatSpecTypeCommas);
	return format_proc_padding(f, spec, buf, raw_bytes);
}

PUBLIC i32 format_append(Formatter *f, const char *p, ...) {
	i32 ret = 0;
	__builtin_va_list args;
	FormatSpec spec;
	u64 len;
	Printable next;

	__builtin_va_start(args, p);
	while (*p != '\0') {
		const char *np = find_next_placeholder(p, &spec);
		const char *pp1 = p + 1;
		if (np && *pp1) {
			len = np - p;
			if ((ret = format_try_resize(f, len)) < 0) {
				goto cleanup;
			}
			memcpy(f->buf + f->pos, p, len);
			f->pos += len;
			if (spec.t == FormatSpecTypeEscapeBracketRight) {
				if ((ret = format_proc_string(f, &spec, "}")) <
				    0) {
					goto cleanup;
				}

				p = np + spec.total_bytes;
				continue;
			} else if (spec.t == FormatSpecTypeEscapeBracketLeft) {
				if ((ret = format_proc_string(f, &spec, "{")) <
				    0) {
					goto cleanup;
				}
				p = np + spec.total_bytes;
				continue;
			}
			next = __builtin_va_arg(args, Printable);
			if (next.t == UIntType) {
				u128 v = next.data.uvalue;
				if ((ret = format_proc_uint(f, &spec, v)) < 0) {
					goto cleanup;
				}
			} else if (next.t == IntType) {
				i128 v = next.data.ivalue;
				if ((ret = format_proc_int(f, &spec, v)) < 0) {
					goto cleanup;
				}
			} else if (next.t == StringType) {
				const char *s = next.data.svalue;
				if ((ret = format_proc_string(f, &spec, s)) <
				    0) {
					goto cleanup;
				}
			} else if (next.t == FloatType) {
				f64 v = next.data.fvalue;
				if ((ret = format_proc_float(f, &spec, v)) <
				    0) {
					goto cleanup;
				}
			} else {
				ret = -EINVAL;
				goto cleanup;
			}
			p = np + spec.total_bytes;
		} else {
			len = strlen(p);
			if ((ret = format_try_resize(f, len)) < 0) {
				goto cleanup;
			}
			memcpy(f->buf + f->pos, p, len);
			f->pos += len;
			break;
		}
	}

cleanup:
	__builtin_va_end(args);
	return ret;
}

PUBLIC void format_clear(Formatter *f) {
	if (f->capacity) munmap(f->buf, f->capacity);
	f->capacity = f->pos = 0;
	f->buf = NULL;
}

PUBLIC const char *format_to_string(Formatter *f) {
	if (format_try_resize(f, 1) < 0) return "";
	f->buf[f->pos] = '\0';
	return f->buf;
}

/* GCOVR_EXCL_START */
#ifdef TEST
#include <libfam/test.h>

Test(format1) {
	Formatter f = FORMATTER_INIT;
	FORMAT(&f, "{}", 1);
	ASSERT(!strcmp("1", format_to_string(&f)), "1");
	format_clear(&f);
	FORMAT(&f, "{}", -1);
	ASSERT(!strcmp("-1", format_to_string(&f)), "-1");
	format_clear(&f);
	FORMAT(&f, "x={x}", 0xFE);
	ASSERT(!strcmp("x=0xfe", format_to_string(&f)), "x=0xfe");
	format_clear(&f);
	FORMAT(&f, "x={X},...", 255);
	ASSERT(!strcmp("x=0xFF,...", format_to_string(&f)), "x=0xFF,...");
	format_clear(&f);
	FORMAT(&f, "a={},b={},c={},d={x}", "test", 1.23456, 9999, 253);
	ASSERT(!strcmp("a=test,b=1.23456,c=9999,d=0xfd", format_to_string(&f)),
	       "multi");
	format_clear(&f);
	FORMAT(&f, "a={c},b={b} {nothing", (u8)'a', 3);
	ASSERT(!strcmp("a=a,b=11 {nothing", format_to_string(&f)),
	       "char and bin");
	format_clear(&f);
	u64 x = 101;
	FORMAT(&f, "{}", x);
	ASSERT(!strcmp("101", format_to_string(&f)), "101");
	format_clear(&f);
	FORMAT(&f, "{n}", 1001);
	ASSERT(!strcmp("1,001", format_to_string(&f)), "101 commas");
	format_clear(&f);
	FORMAT(&f, "x=${n.2}", 1234567.930432);
	ASSERT(!strcmp("x=$1,234,567.93", format_to_string(&f)),
	       "dollar format");
	format_clear(&f);
}

Test(format2) {
	Formatter f = FORMATTER_INIT;
	FORMAT(&f, "'{:5x}'", 10);
	ASSERT(!strcmp("'  0xa'", format_to_string(&f)), "alignment hex");
	format_clear(&f);
	FORMAT(&f, "'{{' {}", 10);
	ASSERT(!strcmp("'{' 10", format_to_string(&f)), "esc bracket left");
	format_clear(&f);
	FORMAT(&f, "'}}' {n}", 1000);
	ASSERT(!strcmp("'}' 1,000", format_to_string(&f)),
	       "esc bracket right and commas");
	format_clear(&f);
	FORMAT(&f, "{nn}", 10);
	ASSERT(!strcmp("{nn}", format_to_string(&f)), "formatting error");
	format_clear(&f);
	FORMAT(&f, "'{:<20}'", 10);
	ASSERT(!strcmp("'10                  '", format_to_string(&f)),
	       "formatting error");
	format_clear(&f);
	FORMAT(&f, "'{:>20}'", 10);
	ASSERT(!strcmp("'                  10'", format_to_string(&f)),
	       "formatting error");
	format_clear(&f);
	FORMAT(&f, "{n{}", 10);
	ASSERT(!strcmp("{n{}", format_to_string(&f)), "formatting error - int");
	format_clear(&f);
	i8 x = 'v';
	FORMAT(&f, "{c}", x);
	ASSERT(!strcmp("v", format_to_string(&f)), "i8 as char");
	format_clear(&f);
	FORMAT(&f, "{z}", "abc");
	ASSERT(!strcmp("{z}", format_to_string(&f)),
	       "formatting error - string");
	format_clear(&f);
	Printable p = {.t = 100, .data.ivalue = 100};
	format_append(&f, "{}", p);
	ASSERT(!strcmp("", format_to_string(&f)),
	       "formatting error - invalid type");
	format_clear(&f);
	_debug_alloc_failure = 0;
	FORMAT(&f, "{}", "abc");
	_debug_alloc_failure = I64_MAX;
	ASSERT(!strcmp(format_to_string(&f), ""), "alloc failure1");
	format_clear(&f);
	_debug_alloc_failure = 0;
	FORMAT(&f, "{{");
	_debug_alloc_failure = I64_MAX;
	ASSERT(!strcmp(format_to_string(&f), ""), "alloc failure2");
	format_clear(&f);
	_debug_alloc_failure = 0;
	FORMAT(&f, "}}");
	_debug_alloc_failure = I64_MAX;

	ASSERT(!strcmp(format_to_string(&f), ""), "alloc failure3");
	format_clear(&f);
}

Test(format_errs) {
	Formatter f1 = FORMATTER_INIT;
	Formatter f2 = FORMATTER_INIT;
	Formatter f3 = FORMATTER_INIT;
	Formatter f4 = FORMATTER_INIT;
	Formatter f5 = FORMATTER_INIT;

	_debug_alloc_failure = 0;
	FORMAT(&f1, "   ");
	_debug_alloc_failure = I64_MAX;
	ASSERT(!strcmp(format_to_string(&f1), ""), "alloc failure1");

	_debug_alloc_failure = 0;
	FORMAT(&f2, " {}    ", 1);
	_debug_alloc_failure = I64_MAX;
	ASSERT(!strcmp(format_to_string(&f2), ""), "alloc failure2");

	_debug_proc_format_all = true;
	FORMAT(&f3, " {}    ", 1.1);
	_debug_proc_format_all = false;
	ASSERT(!strcmp(format_to_string(&f3), " "), "float");

	_debug_proc_format_all = true;
	FORMAT(&f4, " {}    ", 1);
	_debug_proc_format_all = false;
	ASSERT(!strcmp(format_to_string(&f4), " "), "int");

	_debug_proc_format_all = true;
	FORMAT(&f5, " {}    ", 1U);
	_debug_proc_format_all = false;
	ASSERT(!strcmp(format_to_string(&f5), " "), "uint");

	format_clear(&f1);
	format_clear(&f2);
	format_clear(&f3);
	format_clear(&f4);
	format_clear(&f5);

	_debug_no_exit = true;
	_debug_no_write = true;
	panic("1");
	_debug_no_write = false;
	_debug_no_exit = false;

	FORMAT(&f1, "{");
	ASSERT(!strcmp(format_to_string(&f1), "{"), "{");
	FORMAT(&f1, "{.X.}");
}

Test(otherparsing) {
	FormatSpec spec = {.has_precision = true, .precision = 4};
	ASSERT_EQ(format_parse_spec("{.1:2.1}", &spec), -EPROTO,
		  "already has formatting");
	spec.has_precision = false;
	ASSERT_EQ(format_parse_spec("{.", &spec), -EPROTO, "early term");

	ASSERT_EQ(format_parse_spec("{:1.2:1}", &spec), -EPROTO,
		  "already has align");

	ASSERT_EQ(format_parse_spec("{.1{", &spec), -EPROTO,
		  "shouldn't be here");

	ASSERT_EQ(format_parse_spec("{:1000", &spec), -EPROTO, "out of bounds");

	ASSERT_EQ(format_parse_spec("{.1cc}", &spec), -EPROTO,
		  "shouldn't be here");

	ASSERT_EQ(format_parse_spec("{.1bb}", &spec), -EPROTO,
		  "shouldn't be here");

	ASSERT_EQ(format_parse_spec("{.1xx}", &spec), -EPROTO,
		  "shouldn't be here");

	ASSERT_EQ(format_parse_spec("{.1XX}", &spec), -EPROTO,
		  "shouldn't be here");

	ASSERT_EQ(format_parse_spec("{.1nn}", &spec), -EPROTO,
		  "shouldn't be here");

	Formatter f = {0};
	FORMAT(&f, " }} ok");
	ASSERT(!strcmp(format_to_string(&f), " } ok"), "single bracket");
	format_clear(&f);
	FORMAT(&f, " } ok");
	ASSERT(!strcmp(format_to_string(&f), " } ok"), "single bracket");
	format_clear(&f);

	char buf[20007] = {0};
	u64 i;

	for (i = 0; i < 20006; i++) buf[i] = '1';
	ASSERT_EQ(f.pos, 0);
	format_append(&f, buf);
	ASSERT_EQ(f.pos, 20006);
	format_append(&f, buf);
	ASSERT_EQ(f.pos, 40012);

	format_clear(&f);

	spec.t = FormatSpecTypeChar;
	format_proc_uint(&f, &spec, 240);
	format_proc_int(&f, &spec, 239);
	format_clear(&f);
}

Test(has_precision1) {
	/*
		static i32 format_parse_spec(const char *p, FormatSpec *spec) {
		FormatSpec ret = {.t = FormatSpecTypeNone, .total_bytes = 2};
		p++;
		while (*p) {
			if (*p == '.') {
				if (ret.has_precision) return -EPROTO;
				ret.has_precision = true;
				p++;
				if (*p > '9' || *p < '0') return -EPROTO;
				ret.precision = *p - '0';
				ret.total_bytes += 2;
				p++;
			} else if (*p == '{') {
				if (ret.has_precision || ret.has_width ||
				    ret.t != FormatSpecTypeNone)
					return -EPROTO;
				ret.t = FormatSpecTypeEscapeBracketLeft;
				break;
			} else if (*p == ':') {
				*/
	FormatSpec spec;
	ASSERT_EQ(format_parse_spec("{.1{", &spec), -EPROTO, "has_prec");
	ASSERT_EQ(format_parse_spec("{:1{", &spec), -EPROTO, "has_width");
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* FORMAT_IMPL */
#endif /* FORMAT_IMPL_GUARD */
