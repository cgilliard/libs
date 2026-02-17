#ifndef _FORMAT_H
#define _FORMAT_H

#include <libfam/types.h>

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

#endif /* _FORMAT_H */

#ifdef FORMAT_IMPL
#ifndef FORMAT_IMPL_GUARD
#define FORMAT_IMPL_GUARD

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

		__builtin_memcpy(tmp, f->buf, f->pos);
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
	__builtin_memcpy(f->buf + f->pos, value, raw_bytes);
	f->pos += raw_bytes;
	if (spec->align == FormatAlignLeft)
		for (i = raw_bytes; i < aligned_bytes; i++)
			f->buf[f->pos++] = ' ';
	return 0;
}

static i32 format_proc_uint(Formatter *f, const FormatSpec *spec, u128 value) {
	i32 result;
	u8 buf[MAX_U128_STRING_LEN];
	Int128DisplayType idt = format_get_displayType(spec);
	u64 raw_bytes;

	if (spec->t == FormatSpecTypeChar) {
		buf[0] = value <= I8_MAX ? value : '?';
		raw_bytes = 1;
	} else
		raw_bytes = u128_to_string(buf, value, idt);
	if ((result = format_proc_padding(f, spec, buf, raw_bytes)) < 0)
		return result;
	return 0;
}

static i32 format_proc_int(Formatter *f, const FormatSpec *spec, i128 value) {
	i32 result;
	u8 buf[MAX_I128_STRING_LEN];
	Int128DisplayType idt = format_get_displayType(spec);
	u64 raw_bytes;

	if (spec->t == FormatSpecTypeChar) {
		buf[0] = value <= I8_MAX ? value : '?';
		raw_bytes = 1;
	} else
		raw_bytes = i128_to_string(buf, value, idt);
	if ((result = format_proc_padding(f, spec, buf, raw_bytes)) < 0)
		return result;
	return 0;
}

static i32 format_proc_string(Formatter *f, const FormatSpec *spec,
			      const char *value) {
	return format_proc_padding(f, spec, value, __builtin_strlen(value));
}

static i32 format_proc_float(Formatter *f, const FormatSpec *spec, f64 value) {
	i32 result;
	u8 buf[MAX_F64_STRING_LEN];
	u64 raw_bytes;

	raw_bytes =
	    f64_to_string(buf, value, spec->has_precision ? spec->precision : 5,
			  spec->t == FormatSpecTypeCommas);
	if ((result = format_proc_padding(f, spec, buf, raw_bytes)) < 0)
		return result;
	return 0;
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
			__builtin_memcpy(f->buf + f->pos, p, len);
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
			len = __builtin_strlen(p);
			if ((ret = format_try_resize(f, len)) < 0) {
				goto cleanup;
			}
			__builtin_memcpy(f->buf + f->pos, p, len);
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

#ifdef TEST
Test(format1) {
	Formatter f = {0};
	Printable p = {.t = StringType, .data.svalue = "xyz"};
	i32 res = format_append(&f, "abc {} 123", p);
	ASSERT(!res);
	ASSERT(!strcmp(format_to_string(&f), "abc xyz 123"));
	p.t = IntType;
	p.data.ivalue = -123;
	res = format_append(&f, "ghi {} 123", p);
	ASSERT(!res);
	ASSERT(!strcmp(format_to_string(&f), "abc xyz 123ghi -123 123"));
	format_clear(&f);
	ASSERT(!strcmp(format_to_string(&f), ""));
	p.t = FloatType;
	p.data.fvalue = 10.3333333;
	res = format_append(&f, "ok1 {:.3}", p);
	ASSERT(!res);
	ASSERT(!strcmp(format_to_string(&f), "ok1 10.333"));
}
#endif /* TEST */

#endif /* FORMAT_IMPL */
#endif /* FORMAT_IMPL_GUARD */
