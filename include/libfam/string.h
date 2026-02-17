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

#endif /* TEST */
#endif /* STRING_IMPL_GUARD */
#endif /* STRING_IMPL */
