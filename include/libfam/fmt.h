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

#ifndef _FMT_H
#define _FMT_H

/*****************************************************************************
 *
```
sudo ./build install
```
 * test.c
```
#include <libfam/fmt.h>

i32 main(void) {
    println("{} + {} = {}", 1, 1, 2);
    return 0;
}
```
 *
```
cc -DFMT_ALL_IMPL test.c -o test
```
 *
 ****************************************************************************/

#ifdef FMT_ALL_IMPL
#define SYSCALL_IMPL
#define FMT_IMPL
#define STRING_IMPL
#define SYSEXT_IMPL
#define SYNC_IMPL
#define STUBS_IMPL
#define DATE_IMPL
#endif

#include <libfam/sysext.h>
#include <libfam/types.h>

#define MAX_PRECISION 32
#define MAX_WIDTH 4096

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored \
    "-Wincompatible-pointer-types-discards-qualifiers"
#else
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif /* !__clang__ */

/* GCOVR_EXCL_START */
#define FORMAT_ITEM(ign, value)                                                \
	({                                                                     \
		FmtItem _p__ = _Generic((value),                               \
		    char: ((FmtItem){                                          \
			.t = FmtIntType,                                       \
			.data.ivalue =                                         \
			    _Generic((value), char: (value), default: 0)}),    \
		    signed char: ((FmtItem){.t = FmtIntType,                   \
					    .data.ivalue = _Generic((value),   \
					    signed char: (value),              \
					    default: 0)}),                     \
		    short int: ((FmtItem){.t = FmtIntType,                     \
					  .data.ivalue = _Generic((value),     \
					  short int: (value),                  \
					  default: 0)}),                       \
		    int: ((FmtItem){                                           \
			.t = FmtIntType,                                       \
			.data.ivalue =                                         \
			    _Generic((value), int: (value), default: 0)}),     \
		    long: ((FmtItem){                                          \
			.t = FmtIntType,                                       \
			.data.ivalue =                                         \
			    _Generic((value), long: (value), default: 0)}),    \
		    long long: ((FmtItem){.t = FmtIntType,                     \
					  .data.ivalue = _Generic((value),     \
					  long long: (value),                  \
					  default: 0)}),                       \
		    __int128_t: ((FmtItem){.t = FmtIntType,                    \
					   .data.ivalue = _Generic((value),    \
					   __int128_t: (value),                \
					   default: 0)}),                      \
		    unsigned char: ((FmtItem){.t = FmtUIntType,                \
					      .data.uvalue = _Generic((value), \
					      unsigned char: (value),          \
					      default: 0)}),                   \
		    unsigned short int: ((FmtItem){                            \
			.t = FmtUIntType,                                      \
			.data.uvalue = _Generic((value),                       \
			unsigned short int: (value),                           \
			default: 0)}),                                         \
		    unsigned int: ((FmtItem){.t = FmtUIntType,                 \
					     .data.uvalue = _Generic((value),  \
					     unsigned int: (value),            \
					     default: 0)}),                    \
		    unsigned long: ((FmtItem){.t = FmtUIntType,                \
					      .data.uvalue = _Generic((value), \
					      unsigned long: (value),          \
					      default: 0)}),                   \
		    unsigned long long: ((FmtItem){                            \
			.t = FmtUIntType,                                      \
			.data.uvalue = _Generic((value),                       \
			unsigned long long: (value),                           \
			default: 0)}),                                         \
		    __uint128_t: ((FmtItem){.t = FmtUIntType,                  \
					    .data.uvalue = _Generic((value),   \
					    __uint128_t: (value),              \
					    default: 0)}),                     \
		    char *: ((FmtItem){.t = FmtStringType,                     \
				       .data.svalue = _Generic((value),        \
				       char *: (value),                        \
				       default: NULL)}),                       \
		    const char *: ((FmtItem){.t = FmtStringType,               \
					     .data.svalue = _Generic((value),  \
					     const char *: (value),            \
					     default: NULL)}),                 \
		    signed char *: ((FmtItem){.t = FmtStringType,              \
					      .data.svalue = _Generic((value), \
					      char *: (value),                 \
					      default: NULL)}),                \
		    u8 *: ((FmtItem){.t = FmtStringType,                       \
				     .data.svalue = _Generic((value),          \
				     const u8 *: (value),                      \
				     u8 *: (value),                            \
				     default: NULL)}),                         \
		    const u8 *: ((FmtItem){.t = FmtStringType,                 \
					   .data.svalue = _Generic((value),    \
					   const u8 *: (value),                \
					   default: NULL)}),                   \
		    void *: ((FmtItem){.t = FmtUIntType,                       \
				       .data.uvalue = _Generic((value),        \
				       void *: ((u64)value),                   \
				       default: 0)}),                          \
		    double: ((FmtItem){.t = FmtFloatType,                      \
				       .data.fvalue = _Generic((value),        \
				       double: (value),                        \
				       default: 0.0)}),                        \
		    float: ((FmtItem){                                         \
			.t = FmtFloatType,                                     \
			.data.fvalue =                                         \
			    _Generic((value), float: (value), default: 0.0)}), \
		    default: ((FmtItem){                                       \
			.t = FmtStringType,                                    \
			.data.svalue = (char *)"unsupported"}));               \
		_p__;                                                          \
	})

#ifdef __clang__
#define FORMAT(f, fmt, ...)                                                    \
	({                                                                     \
		_Pragma("GCC diagnostic push");                                \
		/* clang-format off */                                       \
                _Pragma("GCC diagnostic ignored \"-Wincompatible-pointer-types-discards-qualifiers\""); \
		/* clang-format on */                                          \
		fmt_append(f, fmt __VA_OPT__(, FOR_EACH(FORMAT_ITEM, _, (, ),  \
							__VA_ARGS__)));        \
		_Pragma("GCC diagnostic pop");                                 \
	})
#else
#define FORMAT(f, fmt, ...)                                                   \
	({                                                                    \
		_Pragma("GCC diagnostic push");                               \
		_Pragma("GCC diagnostic ignored \"-Wdiscarded-qualifiers\""); \
		fmt_append(f, fmt __VA_OPT__(, FOR_EACH(FORMAT_ITEM, _, (, ), \
							__VA_ARGS__)));       \
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
		Fmt _f__ = {0};                                             \
		if (FORMAT(&_f__, fmt, __VA_ARGS__) >= 0) {                 \
			if (fmt_append(&_f__, "\n") >= 0) {                 \
				_tmp__ = fmt_to_string(&_f__);              \
				if (_tmp__) write_all(1, _tmp__, _f__.pos); \
			}                                                   \
		}                                                           \
		fmt_clear(&_f__);                                           \
	})

#define print(fmt, ...)                                             \
	({                                                          \
		const char *_tmp__;                                 \
		Fmt _f__ = {0};                                     \
		if (FORMAT(&_f__, fmt, __VA_ARGS__) >= 0) {         \
			_tmp__ = fmt_to_string(&_f__);              \
			if (_tmp__) write_all(1, _tmp__, _f__.pos); \
		}                                                   \
		fmt_clear(&_f__);                                   \
	})

#define panic(fmt, ...)                                                     \
	({                                                                  \
		const char *_tmp__;                                         \
		Fmt _f__ = {0};                                             \
		if (FORMAT(&_f__, fmt, __VA_ARGS__) >= 0) {                 \
			if (fmt_append(&_f__, "\n") >= 0) {                 \
				_tmp__ = fmt_to_string(&_f__);              \
				if (_tmp__) write_all(2, _tmp__, _f__.pos); \
			}                                                   \
		}                                                           \
		fmt_clear(&_f__);                                           \
		exit_group(-1);                                             \
	})

/* GCOVR_EXCL_STOP */

typedef struct {
	char *buf;
	u64 capacity;
	u64 pos;
} Fmt;

typedef enum {
	FmtIntType,
	FmtUIntType,
	FmtStringType,
	FmtFloatType,
} FmtType;

typedef struct {
	FmtType t;
	union {
		i128 ivalue;
		u128 uvalue;
		f64 fvalue;
		char *svalue;
	} data;
} FmtItem;

i32 fmt_append(Fmt *f, const char *fmt, ...);
void fmt_clear(Fmt *f);
const char *fmt_to_string(Fmt *f);

#endif /* _FMT_H */

#ifdef FMT_IMPL
#ifndef FMT_IMPL_GUARD
#define FMT_IMPL_GUARD

#include <libfam/date.h>
#include <libfam/errno.h>
#include <libfam/limits.h>
#include <libfam/string.h>
#include <libfam/syscall.h>
#include <libfam/sysext.h>
#include <libfam/utils.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif /* PAGE_SIZE */
#define PAGE_MASK (~(PAGE_SIZE - 1))

typedef enum {
	FmtAlignLeft,
	FmtAlignRight,
} FmtAlignment;

typedef enum {
	FmtSpecTypeNone,
	FmtSpecTypeBinary,
	FmtSpecTypeHexUpper,
	FmtSpecTypeHexLower,
	FmtSpecTypeCommas,
	FmtSpecTypeChar,
	FmtSpecTypeTime,
} FmtSpecType;

typedef struct {
	bool has_precision;
	u32 precision;
	bool has_alignment;
	FmtAlignment align;
	u32 width;
	FmtSpecType t;
} FmtSpec;

static const char PADDING[4096] = {[0 ... 4095] = ' '};

static i32 fmt_try_resize(Fmt *f, u64 len) {
	u64 needed = 1 + len + f->pos;
	if (needed > f->capacity) {
		u64 to_alloc;
		void *tmp = NULL;
		if (!f->capacity)
			to_alloc = (needed + PAGE_SIZE - 1) & PAGE_MASK;
		else {
			u64 fcap2 = f->capacity * 2;
			to_alloc = fcap2 >= needed
				       ? fcap2
				       : (needed + PAGE_SIZE - 1) & PAGE_MASK;
		}
		tmp = map(to_alloc);
		if (!tmp) return -ENOMEM;
		memcpy(tmp, f->buf, f->pos);
		munmap(f->buf, f->capacity);
		f->capacity = to_alloc;
		f->buf = tmp;
	}
	return 0;
}

static i32 fmt_append_raw(Fmt *f, const char *buf, u64 len) {
	i32 result = fmt_try_resize(f, len);
	if (result < 0) return result;
	memcpy(f->buf + f->pos, buf, len);
	f->pos += len;
	return 0;
}

static i32 fmt_parse_placeholder(const char **np, FmtSpec *spec) {
	i32 result = 0;

	(*np)++;
	while (**np) {
		if (**np == '}') {
			(*np)++;
			goto cleanup;
		} else if (**np == '.') {
			u64 len = 0;
			u128 v;
			if (spec->has_precision) {
				result = -EPROTO;
				goto cleanup;
			}
			(*np)++;
			while (**np <= '9' && **np >= '0') {
				len++;
				(*np)++;
			}
			if (!len) {
				result = -EPROTO;
				goto cleanup;
			}

			result = string_to_u128(*np - len, len, &v);

			if (result < 0) goto cleanup;
			if (v > MAX_PRECISION) {
				result = -EPROTO;
				goto cleanup;
			}
			spec->precision = v;
			spec->has_precision = true;

			if (**np == '}') {
				(*np)++;
				goto cleanup;
			}
			continue;
		} else if (**np == ':') {
			u128 v;
			u64 len = 0;
			if (spec->has_alignment) {
				result = -EPROTO;
				goto cleanup;
			}
			(*np)++;
			if (**np == '>') {
				spec->align = FmtAlignRight;
				(*np)++;
			} else if (**np == '<') {
				spec->align = FmtAlignLeft;
				(*np)++;
			}
			while (**np <= '9' && **np >= '0') {
				len++;
				(*np)++;
			}
			if (!len) {
				result = -EPROTO;
				goto cleanup;
			}
			result = string_to_u128(*np - len, len, &v);
			if (result < 0) goto cleanup;
			if (v > MAX_WIDTH) {
				result = -EPROTO;
				goto cleanup;
			}
			spec->width = v;
			spec->has_alignment = true;
			if (**np == '}') {
				(*np)++;
				goto cleanup;
			}
			continue;
		} else if (**np == 'b') {
			if (spec->t) {
				result = -EPROTO;
				goto cleanup;
			}
			spec->t = FmtSpecTypeBinary;
		} else if (**np == 'X') {
			if (spec->t) {
				result = -EPROTO;
				goto cleanup;
			}
			spec->t = FmtSpecTypeHexUpper;
		} else if (**np == 'x') {
			if (spec->t) {
				result = -EPROTO;
				goto cleanup;
			}
			spec->t = FmtSpecTypeHexLower;
		} else if (**np == 'n') {
			if (spec->t) {
				result = -EPROTO;
				goto cleanup;
			}
			spec->t = FmtSpecTypeCommas;
		} else if (**np == 'c') {
			if (spec->t) {
				result = -EPROTO;
				goto cleanup;
			}
			spec->t = FmtSpecTypeChar;
		} else if (**np == 't') {
			if (spec->t) {
				result = -EPROTO;
				goto cleanup;
			}
			spec->t = FmtSpecTypeTime;
		} else {
			result = -EPROTO;
			goto cleanup;
		}
		(*np)++;
	}
	result = -EFAULT;
cleanup:
	if (result < 0) {
		while (**np && **np != '}') (*np)++;
		if (**np) (*np)++;
	}
	return result;
}

static i32 fmt_proc_err(Fmt *f) { return fmt_append_raw(f, "<?>", 3); }

static i32 fmt_proc_string_type(Fmt *f, const FmtItem *item,
				const FmtSpec *spec) {
	i32 result = 0;
	u64 len;

	if (spec->has_precision || spec->t) {
		result = fmt_append_raw(f, "<?>", 3);
		goto cleanup;
	}

	len = strlen(item->data.svalue);

	if (spec->has_alignment && spec->align == FmtAlignRight &&
	    len < spec->width) {
		result = fmt_append_raw(f, PADDING, spec->width - len);
		if (result < 0) goto cleanup;
	}

	result = fmt_append_raw(f, item->data.svalue, len);
	if (result < 0) goto cleanup;

	if (spec->has_alignment && spec->align == FmtAlignLeft &&
	    len < spec->width) {
		result = fmt_append_raw(f, PADDING, spec->width - len);
	}

cleanup:
	return result;
}

static i32 fmt_proc_int_type(Fmt *f, const FmtItem *item, const FmtSpec *spec) {
	u8 len;
	i32 result = 0;
	char buf[MAX_I128_STRING_LEN];

	if (spec->has_precision && spec->t != FmtSpecTypeTime) {
		result = fmt_append_raw(f, "<?>", 3);
		goto cleanup;
	}

	if (spec->t == FmtSpecTypeTime) {
		len = date_calc(buf, item->data.ivalue,
				spec->has_precision ? spec->precision : 0);
	} else if (spec->t == FmtSpecTypeHexUpper)
		len = i128_to_string(buf, item->data.ivalue,
				     Int128DisplayTypeHexUpper);
	else if (spec->t == FmtSpecTypeHexLower)
		len = i128_to_string(buf, item->data.ivalue,
				     Int128DisplayTypeHexLower);
	else if (spec->t == FmtSpecTypeBinary)
		len = i128_to_string(buf, item->data.ivalue,
				     Int128DisplayTypeBinary);
	else if (spec->t == FmtSpecTypeCommas)
		len = i128_to_string(buf, item->data.ivalue,
				     Int128DisplayTypeCommas);
	else if (spec->t == FmtSpecTypeChar) {
		len = 1;
		if (item->data.ivalue > I8_MAX || item->data.ivalue < 0)
			buf[0] = '?';
		else
			buf[0] = item->data.ivalue;
	} else
		len = i128_to_string(buf, item->data.ivalue,
				     Int128DisplayTypeDecimal);

	if (spec->has_alignment && spec->align == FmtAlignRight &&
	    len < spec->width) {
		result = fmt_append_raw(f, PADDING, spec->width - len);
		if (result < 0) goto cleanup;
	}

	result = fmt_append_raw(f, buf, len);
	if (result < 0) goto cleanup;

	if (spec->has_alignment && spec->align == FmtAlignLeft &&
	    len < spec->width) {
		result = fmt_append_raw(f, PADDING, spec->width - len);
	}
cleanup:
	return result;
}

static i32 fmt_proc_uint_type(Fmt *f, const FmtItem *item,
			      const FmtSpec *spec) {
	u8 len;
	i32 result = 0;
	char buf[MAX_U128_STRING_LEN];

	if (spec->has_precision) {
		result = fmt_append_raw(f, "<?>", 3);
		goto cleanup;
	}

	if (spec->t == FmtSpecTypeHexUpper)
		len = u128_to_string(buf, item->data.uvalue,
				     Int128DisplayTypeHexUpper);
	else if (spec->t == FmtSpecTypeHexLower)
		len = u128_to_string(buf, item->data.uvalue,
				     Int128DisplayTypeHexLower);
	else if (spec->t == FmtSpecTypeBinary)
		len = u128_to_string(buf, item->data.uvalue,
				     Int128DisplayTypeBinary);
	else if (spec->t == FmtSpecTypeCommas)
		len = u128_to_string(buf, item->data.uvalue,
				     Int128DisplayTypeCommas);
	else if (spec->t == FmtSpecTypeChar) {
		len = 1;
		if (item->data.uvalue > I8_MAX)
			buf[0] = '?';
		else
			buf[0] = item->data.uvalue;
	} else
		len = u128_to_string(buf, item->data.uvalue,
				     Int128DisplayTypeDecimal);

	if (spec->has_alignment && spec->align == FmtAlignRight &&
	    len < spec->width) {
		result = fmt_append_raw(f, PADDING, spec->width - len);
		if (result < 0) goto cleanup;
	}

	result = fmt_append_raw(f, buf, len);
	if (result < 0) goto cleanup;

	if (spec->has_alignment && spec->align == FmtAlignLeft &&
	    len < spec->width) {
		result = fmt_append_raw(f, PADDING, spec->width - len);
	}
cleanup:
	return result;
}

static i32 fmt_proc_float_type(Fmt *f, const FmtItem *item,
			       const FmtSpec *spec) {
	u8 len;
	i32 result = 0;
	u32 prec = spec->has_precision ? spec->precision : 5;
	bool commas = spec->t == FmtSpecTypeCommas;
	char buf[MAX_F64_STRING_LEN];

	len = f64_to_string(buf, item->data.fvalue, prec, commas, false);

	if (spec->has_alignment && spec->align == FmtAlignRight &&
	    len < spec->width) {
		result = fmt_append_raw(f, PADDING, spec->width - len);
		if (result < 0) goto cleanup;
	}

	result = fmt_append_raw(f, buf, len);
	if (result < 0) goto cleanup;

	if (spec->has_alignment && spec->align == FmtAlignLeft &&
	    len < spec->width) {
		result = fmt_append_raw(f, PADDING, spec->width - len);
	}
cleanup:
	return result;
}

static i32 fmt_proc_placeholder(Fmt *f, const char **np,
				__builtin_va_list *args) {
	i32 result;
	FmtItem item;
	FmtSpec spec = {0};

	result = fmt_parse_placeholder(np, &spec);
	if (result != -EFAULT) item = __builtin_va_arg(*args, FmtItem);
	if (result < 0) return fmt_proc_err(f);
	if (item.t == FmtIntType) return fmt_proc_int_type(f, &item, &spec);
	if (item.t == FmtStringType)
		return fmt_proc_string_type(f, &item, &spec);
	if (item.t == FmtUIntType) return fmt_proc_uint_type(f, &item, &spec);
	if (item.t == FmtFloatType) return fmt_proc_float_type(f, &item, &spec);
	return -EPROTO;
}

PUBLIC i32 fmt_append(Fmt *f, const char *p, ...) {
	u64 len;
	i32 result = 0;
	const char *start = p;
	__builtin_va_list args;

	if (!p) return -EINVAL;
	__builtin_va_start(args, p);
	while (*p) {
		if (*p == '{') {
			len = p - start;
			if (len) result = fmt_append_raw(f, start, len);
			if (result < 0) goto cleanup;
			if (*(p + 1) == '{') {
				fmt_append_raw(f, "{", 1);
				p += 2;
				start = p;
			} else {
				result = fmt_proc_placeholder(f, &p, &args);
				if (result < 0) goto cleanup;
				start = p;
			}
		} else
			p++;
	}
	len = p - start;
	if (len) result = fmt_append_raw(f, start, p - start);
cleanup:
	__builtin_va_end(args);
	return result;
}

PUBLIC void fmt_clear(Fmt *f) {
	if (f->capacity) munmap(f->buf, f->capacity);
	f->capacity = f->pos = 0;
	f->buf = NULL;
}

PUBLIC const char *fmt_to_string(Fmt *f) {
	f->buf[f->pos] = '\0';
	return f->buf;
}

/* GCOVR_EXCL_START */
#ifdef TEST
#include <libfam/debug.h>
#include <libfam/test.h>

Test(fmt1) {
	Fmt f = {0};
	FmtItem x = {.t = FmtIntType, .data.ivalue = 123};
	FmtItem y = {.t = FmtIntType, .data.ivalue = -33};
	FmtItem z = {.t = FmtIntType, .data.ivalue = 100000};

	ASSERT(!fmt_append(&f, "abc"), "fmt_append 1");
	ASSERT(!strcmp(fmt_to_string(&f), "abc"), "abc");
	ASSERT(!fmt_append(&f, "def"), "fmt_append 2");
	ASSERT(!strcmp(fmt_to_string(&f), "abcdef"), "abcdef");
	ASSERT(!fmt_append(&f, " ok {} ", x), "fmt_append 3");
	ASSERT(!strcmp(fmt_to_string(&f), "abcdef ok 123 "), "abcdef ok 123 ");
	fmt_clear(&f);
	x.data.ivalue = 0xFF;
	ASSERT(!fmt_append(&f, "{X} ", x), "fmt append 4");
	ASSERT(!strcmp(fmt_to_string(&f), "0xFF "), "0xFF ");
	fmt_clear(&f);
	ASSERT(!fmt_append(&f, "{x} ", x), "fmt append 5");
	ASSERT(!strcmp(fmt_to_string(&f), "0xff "), "0xff ");
	fmt_clear(&f);
	ASSERT(!fmt_append(&f, "{b} ", x), "fmt append 6");
	ASSERT(!strcmp(fmt_to_string(&f), "0b11111111 "),
	       "0x1111111111111111 ");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{b:20} ", x), "fmt append 7");
	ASSERT_EQ(strlen(fmt_to_string(&f)), 21,
		  "exp strlen=21(20 byte width + 1 trailing space) actual={}",
		  strlen(fmt_to_string(&f)));
	ASSERT(!strcmp(fmt_to_string(&f), "0b11111111           "),
	       "0b11111111           ");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{b:<20} ", x), "fmt append 8");
	ASSERT_EQ(strlen(fmt_to_string(&f)), 21,
		  "exp strlen=21(20 byte width + 1 trailing space) actual={}",
		  strlen(fmt_to_string(&f)));
	ASSERT(!strcmp(fmt_to_string(&f), "0b11111111           "),
	       "0b11111111           ");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{b:>20} ", x), "fmt append 9");
	ASSERT_EQ(strlen(fmt_to_string(&f)), 21,
		  "2exp strlen=21(20 byte width + 1 trailing space) actual={}",
		  strlen(fmt_to_string(&f)));
	ASSERT(!strcmp(fmt_to_string(&f), "          0b11111111 "),
	       "          0b11111111 ");
	fmt_clear(&f);

	x.data.ivalue = 'a';
	ASSERT(!fmt_append(&f, "{c}", x), "fmt append 10");
	ASSERT(!strcmp(fmt_to_string(&f), "a"), "a");
	fmt_clear(&f);

	x.data.ivalue = 999;
	ASSERT(!fmt_append(&f, "{c}", x), "fmt append 10.5");
	ASSERT(!strcmp(fmt_to_string(&f), "?"), "?");
	fmt_clear(&f);

	x.data.ivalue = -1;
	ASSERT(!fmt_append(&f, "{c}", x), "fmt append 10.5");
	ASSERT(!strcmp(fmt_to_string(&f), "?"), "?");
	fmt_clear(&f);

	x.data.ivalue = -88;
	ASSERT(!fmt_append(&f, " {}", x), "fmt append 11");
	ASSERT(!strcmp(fmt_to_string(&f), " -88"), " -88");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x={},y={},z={}", x, y, z), "fmt append 12");
	ASSERT(!strcmp(fmt_to_string(&f), "x=-88,y=-33,z=100000"),
	       "x=-88,y=-33,z=100000");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x={::4} ", x), "fmt append 13");
	ASSERT(!strcmp(fmt_to_string(&f), "x=<?> "), "x=<?> ");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x={:>4},y={..},a={q},z={}", x, y, z, z),
	       "fmt append 14");
	ASSERT(!strcmp(fmt_to_string(&f), "x= -88,y=<?>,a=<?>,z=100000"),
	       "x= -88,y=<?>,a=<?>,z=100000");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x={:>4},y={.5},a={q},z={}", x, y, z, z),
	       "fmt append 15");
	ASSERT(!strcmp(fmt_to_string(&f), "x= -88,y=<?>,a=<?>,z=100000"),
	       "x= -88,y=<?>,a=<?>,z=100000");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{n:10}", z), "fmt append 16");
	ASSERT(!strcmp(fmt_to_string(&f), "100,000   "), "100,000   ");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{:10n}", z), "fmt append 16");
	ASSERT(!strcmp(fmt_to_string(&f), "100,000   "), "100,000   ");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{:10X}", z), "fmt append 17");
	ASSERT(!strcmp(fmt_to_string(&f), "0x186A0   "), "0x186A0   ");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{x:10}x", z), "fmt append 18");
	ASSERT(!strcmp(fmt_to_string(&f), "x0x186a0   x"), "x0x186a0   x");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{x   ", z), "fmt append 19");
	ASSERT(!strcmp(fmt_to_string(&f), "x<?>"), "x<?>");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{{  ", z), "fmt append 20");
	ASSERT(!strcmp(fmt_to_string(&f), "x{  "), "x{  ");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{{{{  {{ }}", z), "fmt append 21");
	ASSERT(!strcmp(fmt_to_string(&f), "x{{  { }}"), "x{{  { }}");
	fmt_clear(&f);

	x.t = FmtStringType;
	x.data.svalue = "ok123";
	ASSERT(!fmt_append(&f, " {} ", x), "fmt append 22");
	ASSERT(!strcmp(fmt_to_string(&f), " ok123 "), " ok123 ");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{:10}x", x), "fmt append 23");
	ASSERT(!strcmp(fmt_to_string(&f), "xok123     x"), "xok123     x");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{:<10}x", x), "fmt append 23");
	ASSERT(!strcmp(fmt_to_string(&f), "xok123     x"), "xok123     x");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{:>10}x", x), "fmt append 24");
	ASSERT(!strcmp(fmt_to_string(&f), "x     ok123x"), "x     ok123x");
	fmt_clear(&f);

	x.t = FmtUIntType;
	x.data.uvalue = 255;

	ASSERT(!fmt_append(&f, "{x}", x), "fmt append 25");
	ASSERT(!strcmp(fmt_to_string(&f), "0xff"), "0xff");
	fmt_clear(&f);

	x.t = FmtFloatType;
	x.data.fvalue = 7771.234567;
	ASSERT(!fmt_append(&f, "{}", x), "fmt append 26");
	ASSERT(!strcmp(fmt_to_string(&f), "7771.23457"), "7771.23457");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{n}", x), "fmt append 27");
	ASSERT(!strcmp(fmt_to_string(&f), "7,771.23457"), "7,771.23457");
	fmt_clear(&f);

	x.data.fvalue = 9999.33;
	ASSERT(!fmt_append(&f, "{n}", x), "fmt append 28");
	ASSERT(!strcmp(fmt_to_string(&f), "9,999.33"), "9,999.33");
	fmt_clear(&f);

	x.data.fvalue = 9999.3312342;
	ASSERT(!fmt_append(&f, "{n}", x), "fmt append 29");
	ASSERT(!strcmp(fmt_to_string(&f), "9,999.33123"), "9,999.33123");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{.3}", x), "fmt append 29");
	ASSERT(!strcmp(fmt_to_string(&f), "9999.331"), "9999.331");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{n.3}", x), "fmt append 30");
	ASSERT(!strcmp(fmt_to_string(&f), "9,999.331"), "9,999.331");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{.3n}", x), "fmt append 31");
	ASSERT(!strcmp(fmt_to_string(&f), "9,999.331"), "9,999.331");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "{.3qn}", x), "fmt append 32");
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "<?>");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{.3:20n}x", x), "fmt append 33");
	ASSERT_EQ(strlen(fmt_to_string(&f)), 22, "align");
	ASSERT(!strcmp(fmt_to_string(&f), "x9,999.331           x"),
	       "x9,999.331           x");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{.3:>20n}x", x), "fmt append 34");
	ASSERT_EQ(strlen(fmt_to_string(&f)), 22, "align");
	ASSERT(!strcmp(fmt_to_string(&f), "x           9,999.331x"),
	       "x           9,999.331x");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{:>20n.3}x", x), "fmt append 35");
	ASSERT_EQ(strlen(fmt_to_string(&f)), 22, "align");
	ASSERT(!strcmp(fmt_to_string(&f), "x           9,999.331x"),
	       "x           9,999.331x");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "x{n:>20.3}x", x), "fmt append 35");
	ASSERT_EQ(strlen(fmt_to_string(&f)), 22, "align");
	ASSERT(!strcmp(fmt_to_string(&f), "x           9,999.331x"),
	       "x           9,999.331x");
	fmt_clear(&f);

	x.t = FmtIntType;
	x.data.ivalue = 1771492996944296;
	y.t = FmtIntType;
	y.data.ivalue = x.data.ivalue;

	ASSERT(!fmt_append(&f, "[{t.3}] num={},x={}", x, y, z),
	       "fmt append 36");
	ASSERT(
	    !strcmp(fmt_to_string(&f),
		    "[2026-02-19 09:23:16.944] num=1771492996944296,x=100000"),
	    "[2026-02-19 09:23:16.944] num=1771492996944296,x=100000");
	fmt_clear(&f);

	ASSERT(!fmt_append(&f, "[{t}] num={},x={}", x, y, z), "fmt append 36");
	ASSERT(!strcmp(fmt_to_string(&f),
		       "[2026-02-19 09:23:16] num=1771492996944296,x=100000"),
	       "[2026-02-19 09:23:16.944] num=1771492996944296,x=100000");
	fmt_clear(&f);
}

Test(fmt2) {
	Fmt f = {0};
	FmtItem v = FORMAT_ITEM(_, 1.2);

	fmt_append(&f, "{}", v);
	ASSERT(!strcmp(fmt_to_string(&f), "1.2"), "1.2");
	fmt_clear(&f);

	FORMAT(&f, "a={},b={},c={},d={}", 1, -5, "test", 1.2);
	ASSERT(!strcmp(fmt_to_string(&f), "a=1,b=-5,c=test,d=1.2"),
	       "a=1,b=-5,c=test,d=1.2");
	fmt_clear(&f);

	u32 a = 1;
	i32 b = -5;
	const char *c = "test";
	f64 d = 1.2;

	FORMAT(&f, "a={},b={},c={},d={}", a, b, c, d);
	ASSERT(!strcmp(fmt_to_string(&f), "a=1,b=-5,c=test,d=1.2"),
	       "a=1,b=-5,c=test,d=1.2");
	fmt_clear(&f);
}

Test(fmt3) {
	u64 i;
	char buf[PAGE_SIZE * 2];
	char cmp[PAGE_SIZE * 2 + 2] = {0};
	Fmt f = {0};

	memset(buf, 'x', PAGE_SIZE * 2);
	buf[PAGE_SIZE * 2 - 1] = 0;
	fmt_append(&f, "y");
	FORMAT(&f, "{}", buf);

	cmp[0] = 'y';
	memcpy(cmp + 1, buf, PAGE_SIZE * 2);
	ASSERT(!strcmp(cmp, fmt_to_string(&f)), "big buf");

	fmt_clear(&f);
}

Test(fmt4) {
	Fmt f = {0};
	FORMAT(&f, "{.1.2}", 1.1);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "two .");
	fmt_clear(&f);
	FORMAT(&f, "{.40}", 1.1);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "max precision = 32");
	fmt_clear(&f);

	FORMAT(&f, "{:40:50}", 1.1);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "has alignment");
	fmt_clear(&f);

	FORMAT(&f, "{:9999}", 1.1);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "max width = 4096");
	fmt_clear(&f);

	FORMAT(&f, "{bb}", 11);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "already specified");
	fmt_clear(&f);

	FORMAT(&f, "{xx}", 11);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "already specified");
	fmt_clear(&f);

	FORMAT(&f, "{XX}", 11);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "already specified");
	fmt_clear(&f);

	FORMAT(&f, "{cc}", 11);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "already specified");
	fmt_clear(&f);

	FORMAT(&f, "{nn}", 11);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "already specified");
	fmt_clear(&f);

	FORMAT(&f, "{tt}", 11);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "already specified");
	fmt_clear(&f);

	FORMAT(&f, "{.3}", "abc");
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "has precision");
	fmt_clear(&f);

	FORMAT(&f, "{c}", "abc");
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "has type");
	fmt_clear(&f);

	FORMAT(&f, "{.3}", 30U);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "has precision");
	fmt_clear(&f);

	FORMAT(&f, "{X}", 15U);
	ASSERT(!strcmp(fmt_to_string(&f), "0xF"), "hex upper unsigned");
	fmt_clear(&f);

	FORMAT(&f, "{x}", 15U);
	ASSERT(!strcmp(fmt_to_string(&f), "0xf"), "hex lower unsigned");
	fmt_clear(&f);

	FORMAT(&f, "{n}", 1234U);
	ASSERT(!strcmp(fmt_to_string(&f), "1,234"), "nice unsigned");
	fmt_clear(&f);

	FORMAT(&f, "{c}", (u8)'x');
	ASSERT(!strcmp(fmt_to_string(&f), "x"), "char unsigned");
	fmt_clear(&f);

	FORMAT(&f, "{b}", 15U);
	ASSERT(!strcmp(fmt_to_string(&f), "0b1111"), "binary unsigned");
	fmt_clear(&f);

	FORMAT(&f, "{c}", 200U);
	ASSERT(!strcmp(fmt_to_string(&f), "?"), "undisplayable char unsigned");
	fmt_clear(&f);

	FORMAT(&f, "{:10}", 200U);
	ASSERT(!strcmp(fmt_to_string(&f), "200       "), "left align unsigned");
	fmt_clear(&f);

	FORMAT(&f, "{:>10}", 200U);
	ASSERT(!strcmp(fmt_to_string(&f), "       200"),
	       "right align unsigned");
	fmt_clear(&f);
}

Test(fmt5) {
	Fmt f = {0};
	FmtItem x = {.t = 100, .data.ivalue = 123};

	ASSERT_EQ(fmt_append(&f, "{}", x), -EPROTO, "illegal type");
	fmt_clear(&f);
}

Test(fmt6) {
	Fmt f = {0};
	FORMAT(&f, "{");
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "unexpected end");
	fmt_clear(&f);
}

Test(fmt7) {
	u64 i;
	i64 result;
	char buf[PAGE_SIZE];
	char cmp[PAGE_SIZE + 2] = {0};
	Fmt f = {0};

	memset(buf, 'x', PAGE_SIZE);
	buf[PAGE_SIZE - 1] = 0;
	fmt_append(&f, "y");
	FORMAT(&f, "{}", buf);

	cmp[0] = 'y';
	memcpy(cmp + 1, buf, PAGE_SIZE);
	ASSERT(!strcmp(cmp, fmt_to_string(&f)), "big buf");

	fmt_clear(&f);

	_debug_alloc_failure = 0;
	result = FORMAT(&f, "abc");
	ASSERT_EQ(result, -ENOMEM, "enomem");
	_debug_alloc_failure = I64_MAX;
	fmt_clear(&f);

	FORMAT(&f, "{.340282366920938463463374607431768211456000}", 1.1);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "too much precision");
	fmt_clear(&f);

	FORMAT(&f, "{:340282366920938463463374607431768211456000}", 1.1);
	ASSERT(!strcmp(fmt_to_string(&f), "<?>"), "too much width");
	fmt_clear(&f);
}

Test(fmt8) {
	Fmt f = {0};
	FORMAT(&f, "{:>4}", "abc");
	ASSERT(!strcmp(fmt_to_string(&f), " abc"));
	fmt_clear(&f);

	FORMAT(&f, "{:<4}", "abc");
	ASSERT(!strcmp(fmt_to_string(&f), "abc "));
	fmt_clear(&f);

	FORMAT(&f, "{:>4}", "abcdef");
	ASSERT(!strcmp(fmt_to_string(&f), "abcdef"));
	fmt_clear(&f);

	FORMAT(&f, "{:<4}", "abcdef");
	ASSERT(!strcmp(fmt_to_string(&f), "abcdef"));
	fmt_clear(&f);

	FORMAT(&f, "{:>4}", 123);
	ASSERT(!strcmp(fmt_to_string(&f), " 123"));
	fmt_clear(&f);

	FORMAT(&f, "{:<4}", 123);
	ASSERT(!strcmp(fmt_to_string(&f), "123 "));
	fmt_clear(&f);

	FORMAT(&f, "{:>4}", 123456);
	ASSERT(!strcmp(fmt_to_string(&f), "123456"));
	fmt_clear(&f);

	FORMAT(&f, "{:<4}", 123456);
	ASSERT(!strcmp(fmt_to_string(&f), "123456"));
	fmt_clear(&f);

	FORMAT(&f, "{:>4}", 123U);
	ASSERT(!strcmp(fmt_to_string(&f), " 123"));
	fmt_clear(&f);

	FORMAT(&f, "{:<4}", 123U);
	ASSERT(!strcmp(fmt_to_string(&f), "123 "));
	fmt_clear(&f);

	FORMAT(&f, "{:>4}", 123456U);
	ASSERT(!strcmp(fmt_to_string(&f), "123456"));
	fmt_clear(&f);

	FORMAT(&f, "{:<4}", 123456U);
	ASSERT(!strcmp(fmt_to_string(&f), "123456"));
	fmt_clear(&f);

	FORMAT(&f, "{.1:>4}", 1.2);
	ASSERT(!strcmp(fmt_to_string(&f), " 1.2"));
	fmt_clear(&f);

	FORMAT(&f, "{.1:<4}", 1.2);
	ASSERT(!strcmp(fmt_to_string(&f), "1.2 "));
	fmt_clear(&f);

	FORMAT(&f, "{.4:>4}", 1.2345);
	ASSERT(!strcmp(fmt_to_string(&f), "1.2345"));
	fmt_clear(&f);

	FORMAT(&f, "{.4:<4}", 1.2345);
	ASSERT(!strcmp(fmt_to_string(&f), "1.2345"));
	fmt_clear(&f);

	ASSERT_EQ(FORMAT(&f, NULL), -EINVAL, "NULL input");
}

Test(fmt9) {
	Fmt f = {0};
	_debug_alloc_failure = 0;
	ASSERT_EQ(FORMAT(&f, "{:>4}", "abc"), -ENOMEM, "fail raw append");
	_debug_alloc_failure = 0;
	ASSERT_EQ(FORMAT(&f, "{:<4}", "abc"), -ENOMEM, "fail raw append");

	_debug_alloc_failure = 0;
	ASSERT_EQ(FORMAT(&f, "{:>4}", 123), -ENOMEM, "fail raw append");
	_debug_alloc_failure = 0;
	ASSERT_EQ(FORMAT(&f, "{:<4}", 123), -ENOMEM, "fail raw append");

	_debug_alloc_failure = 0;
	ASSERT_EQ(FORMAT(&f, "{:>4}", 123U), -ENOMEM, "fail raw append");
	_debug_alloc_failure = 0;
	ASSERT_EQ(FORMAT(&f, "{:<4}", 123U), -ENOMEM, "fail raw append");

	_debug_alloc_failure = 0;
	ASSERT_EQ(FORMAT(&f, "{:>4}", 1.2), -ENOMEM, "fail raw append");
	_debug_alloc_failure = 0;
	ASSERT_EQ(FORMAT(&f, "{:<4}", 1.2), -ENOMEM, "fail raw append");

	_debug_alloc_failure = 0;
	ASSERT_EQ(FORMAT(&f, " {}", 1), -ENOMEM, "fail raw append");

	_debug_alloc_failure = I64_MAX;
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* FMT_IMPL_GUARD */
#endif /* FMT_IMPL */
