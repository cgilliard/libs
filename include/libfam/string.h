#ifndef _STRING_H
#define _STRING_H

#include <libfam/types.h>

#define MAX_U128_STRING_LEN 255
#define MAX_I128_STRING_LEN (MAX_U128_STRING_LEN + 1)
#define MAX_F64_STRING_LEN 64

typedef enum {
	Int128DisplayTypeDecimal,
	Int128DisplayTypeHexUpper,
	Int128DisplayTypeHexLower,
	Int128DisplayTypeBinary,
	Int128DisplayTypeCommas,
} Int128DisplayType;

u64 strlen(const char *msg);
i32 strncmp(const char *x, const char *y, u64 n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dst, const char *src, u64 n);
i32 strcmp(const char *x, const char *y);
char *strstr(const char *s, const char *sub);
char *strchr(const char *s, i32 c);
char *strcat(char *dest, const char *src);
void *memset(void *ptr, i32 x, u64 n);
void *memcpy(void *dst, const void *src, u64 n);
void *memmove(void *dst, const void *src, u64 n);
i32 memcmp(const void *s1, const void *s2, u64 n);
u8 f64_to_string(char buf[MAX_F64_STRING_LEN], f64 v, i32 max_decimals,
		 bool commas);
i32 string_to_u128(const char *buf, u64 len, u128 *result);
u8 i128_to_string(char buf[MAX_I128_STRING_LEN], i128 value,
		  Int128DisplayType t);
u8 u128_to_string(char buf[MAX_U128_STRING_LEN], u128 value,
		  Int128DisplayType t);

__attribute__((unused, noinline)) static void secure_zero(void *ptr, u64 len) {
	volatile u8 *p = (volatile u8 *)ptr;
	while (len--) *p++ = 0;
}

void secure_zero32(u8 buf[32]);

#endif /* _STRING_H */

#ifdef STRING_IMPL
#ifndef STRING_IMPL_GUARD
#define STRING_IMPL_GUARD

#include <libfam/errno.h>
#include <libfam/limits.h>
#include <libfam/utils.h>

PUBLIC u64 strlen(const char *x) {
	const char *y = x;
	while (*x) x++;
	return x - y;
}

PUBLIC i32 strcmp(const char *x, const char *y) {
	while (*x == *y && *x) x++, y++;
	return *x > *y ? 1 : *y > *x ? -1 : 0;
}

PUBLIC char *strcpy(char *dest, const char *src) {
	char *ptr = dest;
	while ((*ptr++ = *src++));
	return dest;
}

PUBLIC char *strncpy(char *dest, const char *src, u64 n) {
	u64 i;
	for (i = 0; i < n && src[i] != '\0'; i++) dest[i] = src[i];
	for (; i < n; i++) dest[i] = '\0';
	return dest;
}

PUBLIC char *strcat(char *dest, const char *src) {
	char *ptr = dest;
	while (*ptr) ptr++;
	while ((*ptr++ = *src++));
	return dest;
}

PUBLIC i32 strncmp(const char *x, const char *y, u64 n) {
	while (n > 0 && *x == *y && *x) x++, y++, n--;
	if (n == 0) return 0;
	return (char)*x - (char)*y;
}

PUBLIC void *memset(void *dest, i32 c, u64 n) {
	u8 *tmp = dest;
	while (n--) *tmp++ = (char)c;
	return dest;
}

PUBLIC void *memcpy(void *dest, const void *src, u64 n) {
	u8 *d = (u8 *)dest;
	const u8 *s = (const void *)src;
	while (n--) *d++ = *s++;
	return dest;
}

PUBLIC i32 memcmp(const void *s1, const void *s2, u64 n) {
	const u8 *p1 = (const void *)s1;
	const u8 *p2 = (const void *)s2;
	while (n--) {
		i32 diff = *p1++ - *p2++;
		if (diff) return diff;
	}
	return 0;
}

PUBLIC void *memmove(void *dest, const void *src, u64 n) {
	u8 *d = (void *)((u8 *)dest + n);
	const u8 *s = (const void *)((const u8 *)src + n);
	while (n--) d--, s--, *d = *s;
	return dest;
}

PUBLIC u8 f64_to_string(char buf[MAX_F64_STRING_LEN], f64 v, i32 max_decimals,
			bool commas) {
	u64 pos = 0;
	i32 is_negative;
	u64 int_part;
	f64 frac_part;
	i32 i;
	u8 temp[MAX_F64_STRING_LEN];

	if (v != v) {
		buf[0] = 'n';
		buf[1] = 'a';
		buf[2] = 'n';
		buf[3] = '\0';
		return 3;
	}

	if (v > 1.7976931348623157e308 || v < -1.7976931348623157e308) {
		if (v < 0) buf[pos++] = '-';
		buf[pos++] = 'i';
		buf[pos++] = 'n';
		buf[pos++] = 'f';
		buf[pos] = '\0';
		return pos;
	}

	is_negative = v < 0;
	if (is_negative) {
		buf[pos++] = '-';
		v = -v;
	}

	if (v == 0.0) {
		buf[pos++] = '0';
		buf[pos] = '\0';
		return pos;
	}

	if (max_decimals < 0) max_decimals = 0;
	if (max_decimals > 17) max_decimals = 17;

	int_part = (u64)v;
	frac_part = v - (f64)int_part;

	if (max_decimals > 0) {
		f64 rounding = 0.5;
		for (i = 0; i < max_decimals; i++) rounding /= 10.0;
		v += rounding;
		int_part = (u64)v;
		frac_part = v - (f64)int_part;
	}

	if (int_part == 0)
		buf[pos++] = '0';
	else {
		i = 0;
		while (int_part > 0) {
			temp[i++] = '0' + (int_part % 10);
			int_part /= 10;
		}
		if (commas) {
			u64 digit_count = i;
			u64 digits_until_comma =
			    digit_count % 3 ? digit_count % 3 : 3;
			i--;
			while (i >= 0) {
				buf[pos++] = temp[i--];
				digits_until_comma--;
				if (digits_until_comma == 0 && i >= 0) {
					buf[pos++] = ',';
					digits_until_comma = 3;
				}
			}
		} else
			while (i > 0) buf[pos++] = temp[--i];
	}

	if (frac_part > 0 && max_decimals > 0) {
		buf[pos++] = '.';
		u64 frac_start = pos;
		i32 digits = 0;
		while (digits < max_decimals) {
			frac_part *= 10;
			i32 digit = (i32)frac_part;
			buf[pos++] = '0' + digit;
			frac_part -= digit;
			digits++;
		}
		while (pos > frac_start && buf[pos - 1] == '0') pos--;
		if (pos == frac_start) pos--;
	}

	buf[pos] = '\0';
	return pos;
}

PUBLIC i32 string_to_u128(const char *buf, u64 len, u128 *result) {
	u64 i = 0;
	u8 c;

	*result = 0;
	if (!buf || !len) return -EINVAL;
	while (i < len && (buf[i] == ' ' || buf[i] == '\t')) i++;
	if (i == len) return -EINVAL;
	while (i < len) {
		c = buf[i];
		if (c < '0' || c > '9') return -EINVAL;
		if (*result > U128_MAX / 10) return -EOVERFLOW;
		*result = *result * 10 + (c - '0');
		i++;
	}
	return 0;
}

PUBLIC u8 i128_to_string(char buf[MAX_I128_STRING_LEN], i128 value,
			 Int128DisplayType t) {
	u8 len;
	u128 abs_v;
	bool is_negative = value < 0;
	if (is_negative) {
		*buf++ = '-';
		abs_v = value == I128_MIN ? (u128)1 << 127 : (u128)(-value);
	} else
		abs_v = (u128)value;
	len = u128_to_string(buf, abs_v, t);
	return is_negative ? len + 1 : len;
}

PUBLIC u8 u128_to_string(char buf[MAX_U128_STRING_LEN], u128 value,
			 Int128DisplayType t) {
	char temp[MAX_U128_STRING_LEN];
	i32 i = 0, j = 0;
	bool hex =
	    t == Int128DisplayTypeHexUpper || t == Int128DisplayTypeHexLower;
	bool commas = t == Int128DisplayTypeCommas;
	u8 mod_val =
	    hex ? 16 : (commas || t == Int128DisplayTypeDecimal ? 10 : 2);
	const char *hex_code = t == Int128DisplayTypeHexUpper
				   ? "0123456789ABCDEF"
				   : "0123456789abcdef";
	if (hex) {
		j = 2;
		buf[0] = '0';
		buf[1] = 'x';
	}
	if (value == 0) {
		buf[j++] = '0';
		buf[j] = '\0';
		return j;
	}
	while (value > 0) {
		temp[i++] = hex_code[(value % mod_val)];
		if (mod_val == 16)
			value >>= 4;
		else if (mod_val == 10)
			value /= 10;
		else if (mod_val == 2)
			value >>= 1;
	}
	if (commas) {
		u64 digit_count = i;
		u64 comma_count = digit_count > 3 ? (digit_count - 1) / 3 : 0;
		u64 total_bytes = digit_count + comma_count;
		j = 0;
		i--;
		u64 digits_until_comma = digit_count % 3 ? digit_count % 3 : 3;
		while (i >= 0) {
			buf[j++] = temp[i--];
			digits_until_comma--;
			if (digits_until_comma == 0 && i >= 0) {
				buf[j++] = ',';
				digits_until_comma = 3;
			}
		}
		buf[j] = '\0';
		return total_bytes;
	} else {
		for (; i > 0; j++) {
			buf[j] = temp[--i];
		}
		buf[j] = '\0';
		return j;
	}
}

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored \
    "-Wincompatible-pointer-types-discards-qualifiers"
#else
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif
PUBLIC char *strstr(const char *s, const char *sub) {
	for (; *s; s++) {
		const char *tmps = s, *tmpsub = sub;
		while (*tmps == *tmpsub && *tmps) tmps++, tmpsub++;
		if (*tmpsub == '\0') return (const char *)s;
	}
	return NULL;
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored \
    "-Wincompatible-pointer-types-discards-qualifiers"
#else
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif
PUBLIC char *strchr(const char *s, i32 c) {
	do
		if (*s == c) return (const char *)s;
	while (*s++);
	return !c ? (const char *)s : NULL;
}
#pragma GCC diagnostic pop

PUBLIC void secure_zero32(u8 buf[32]) {
#ifdef __AVX2__
	__asm__ __volatile__(
	    "vpxor   %%ymm0, %%ymm0, %%ymm0     \n\t"
	    "vmovdqa %%ymm0, %0                 \n\t"
	    :
	    : "m"(*(buf))
	    : "ymm0", "memory");

	__asm__ __volatile__("" ::: "memory");
#else
	secure_zero(buf, 32);
#endif /* !__AVX2__ */
}

/* GCOVR_EXCL_START */
#ifdef TEST
#include <libfam/test.h>

Test(strlen) {
	u64 len = strlen("abc");
	ASSERT(len == 3);
	len = strlen("");
	ASSERT(len == 0);
	u64 x = 0;
	ASSERT(!x);
}

Test(to_string) {
	char buf[MAX_I128_STRING_LEN];
	i128 x = -123;
	u8 res = i128_to_string(buf, x, Int128DisplayTypeHexLower);
	ASSERT(res == 5);
	ASSERT(!strcmp(buf, "-0x7b"));
	res = i128_to_string(buf, 1234, Int128DisplayTypeDecimal);
	ASSERT(res == 4);
	ASSERT(!strcmp(buf, "1234"));
}

Test(strcmp) {
	ASSERT(strcmp("abc", "def"), "abc!=def");
	ASSERT(!strcmp("abc", "abc"), "abc=abc");
}

Test(strncpy) {
	char x[1024] = {0};
	char *in1 = "abc\0";
	strncpy(x, in1, 4);
	ASSERT_EQ(x[0], 'a', "a");
	ASSERT_EQ(x[1], 'b', "b");
	ASSERT_EQ(x[2], 'c', "c");
	ASSERT_EQ(x[3], 0, "\0");
}

Test(f64_to_string) {
	char buf[64] = {0};
	u64 len;

	len = f64_to_string(buf, 0.3, 1, false);
	ASSERT_EQ(len, 3, "len=3");
	ASSERT(!strcmp(buf, "0.3"), "0.3");

	len = f64_to_string(buf, 0.0 / 0.0, 6, false);
	ASSERT(!strcmp(buf, "nan"), "nan");
	ASSERT_EQ(len, 3, "nan_len");

	len = f64_to_string(buf, 1.0 / 0.0, 6, false);
	ASSERT(!strcmp(buf, "inf"), "inf");
	ASSERT_EQ(len, 3, "inf_len");

	len = f64_to_string(buf, -1.0 / 0.0, 6, false);
	ASSERT(!strcmp(buf, "-inf"), "neg_inf");
	ASSERT_EQ(len, 4, "neg_inf_len");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Woverflow"
#pragma GCC diagnostic ignored "-Wliteral-range"
	len = f64_to_string(buf, 1.8e308, 6, false);
	ASSERT(!strcmp(buf, "inf"), "overflow_inf");
	ASSERT_EQ(len, 3, "overflow_inf_len");
#pragma GCC diagnostic pop

	len = f64_to_string(buf, 0.0, 6, false);
	ASSERT(!strcmp(buf, "0"), "zero");
	ASSERT_EQ(len, 1, "zero_len");
	len = f64_to_string(buf, -0.0, 6, false);
	ASSERT(!strcmp(buf, "0"), "neg_zero");
	ASSERT_EQ(len, 1, "neg_zero_len");

	len = f64_to_string(buf, 123.0, 0, false);
	ASSERT(!strcmp(buf, "123"), "int_pos");
	ASSERT_EQ(len, 3, "int_pos_len");

	len = f64_to_string(buf, -123.0, 0, false);
	ASSERT(!strcmp(buf, "-123"), "int_neg");

	ASSERT_EQ(len, 4, "int_neg_len");

	len = f64_to_string(buf, 123.456789, 6, false);
	ASSERT(!strcmp(buf, "123.456789"), "frac");
	ASSERT_EQ(len, 10, "frac_len");

	len = f64_to_string(buf, -123.456789, 6, false);
	ASSERT(!strcmp(buf, "-123.456789"), "neg_frac");
	ASSERT_EQ(len, 11, "neg_frac_len");

	len = f64_to_string(buf, 0.9999995, 6, false);
	ASSERT(!strcmp(buf, "1"), "round_up");
	ASSERT_EQ(len, 1, "round_up_len");

	len = f64_to_string(buf, 123.4000, 6, false);
	ASSERT(!strcmp(buf, "123.4"), "trim_zeros");
	ASSERT_EQ(len, 5, "trim_zeros_len");

	len = f64_to_string(buf, 123.0000001, 6, false);
	ASSERT(!strcmp(buf, "123"), "remove_decimal");
	ASSERT_EQ(len, 3, "remove_decimal_len");

	len = f64_to_string(buf, 123.456789123456789, 18, false);
	buf[len] = 0;
	ASSERT(!strcmp(buf, "123.45678912345678668"), "max_decimals");
	ASSERT_EQ(len, 21, "max_decimals_len");

	len = f64_to_string(buf, 123.456, -1, false);
	ASSERT(!strcmp(buf, "123"), "neg_decimals");
	ASSERT_EQ(len, 3, "neg_decimals_len");

	len = f64_to_string(buf, 9993234.334, 2, true);
	ASSERT(!strcmp(buf, "9,993,234.33"), "commas");
	ASSERT_EQ(strlen("9,993,234.33"), len, "commas len");
}

/*
Test(memmove) {
	const char *test = "test";
	const char *test2 = "aaaaa";
	char out[1024] = {0};

	ASSERT(memcmp(out, test, 4), "memcmp ne");
	memcpy(out, test, 4);
	ASSERT(!memcmp(out, test, 4), "memcmp eq");
	memmove(out, test2, 5);
	ASSERT(!memcmp(out, test2, 5), "memcmp eq");
	memcpy(out + 5, "bbbbbbbb", 8);
	memmove(out + 5, out, 8);
	ASSERT(!memcmp(out, "aaa", 3), "memmove cmp");
}
*/

Test(secure_zero) {
	__attribute__((aligned(32))) u8 buf[32] = {1, 2, 3, 4};
	ASSERT(memcmp(buf, (u8[32]){0}, 32), "not zero");
	secure_zero32(buf);
	ASSERT(!memcmp(buf, (u8[32]){0}, 32), "not zero");
}

Test(string_u128) {
	u128 i;
	u128 v1 = 1234;
	i128 v2 = -5678;
	char buf[MAX_I128_STRING_LEN];
	ASSERT(u128_to_string(buf, v1, Int128DisplayTypeDecimal) > 0,
	       "u128_to_string");
	ASSERT(!strcmp(buf, "1234"), "1234");

	ASSERT(i128_to_string(buf, v2, Int128DisplayTypeDecimal) > 0,
	       "i128_to_string");
	ASSERT(!strcmp(buf, "-5678"), "-5678");

	for (i = 0; i < 100000 * 10000; i += 10000) {
		u128 v = i;
		u128 vout;
		u128_to_string(buf, v, Int128DisplayTypeDecimal);
		string_to_u128(buf, strlen(buf), &vout);
		ASSERT_EQ(v, vout, "v=vout");
	}

	ASSERT_EQ(i128_to_string(buf, 0x123, Int128DisplayTypeHexUpper), 5,
		  "len=5");
	ASSERT(!strcmp(buf, "0x123"), "string 0x123");

	ASSERT_EQ(i128_to_string(buf, 0xF, Int128DisplayTypeBinary), 4,
		  "binary 0xF");
	ASSERT(!strcmp(buf, "1111"), "string 1111");

	ASSERT(u128_to_string(buf, 9993, Int128DisplayTypeCommas) > 0,
	       "commas");
	ASSERT(!strcmp(buf, "9,993"), "comma verify");
}

Test(memset) {
	i32 i;
	u8 x[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	memset(x, 0, sizeof(x));
	for (i = 0; i < 10; i++) ASSERT_EQ(x[i], 0, "memset[{}]", x[i]);
}

Test(strstr) {
	const char *s = "abcdefghi";
	ASSERT_EQ(strstr(s, "def"), s + 3, "strstr1");
	ASSERT_EQ(strstr(s, "x"), NULL, "no match");
	ASSERT_EQ(strstr(s, "abcdefghixyz"), NULL, "no match 2");
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* STRING_IMPL_GUARD */
#endif /* STRING_IMPL */
