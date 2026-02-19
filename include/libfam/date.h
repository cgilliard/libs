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

#ifndef _DATE_H
#define _DATE_H

#include <libfam/types.h>

#define MAX_SECOND_PRECISION 6
#define MAX_DATE_LEN (MAX_SECOND_PRECISION + 30)

u8 date_calc(char buffer[MAX_DATE_LEN], u64 micros, u8 precision);
void unix_to_tm(u64 timestamp, u32 *year, u32 *month, u32 *day, u32 *hour,
		u32 *min, u32 *sec);

#endif /* _DATE_H */

#ifdef DATE_IMPL
#ifndef DATE_IMPL_GUARD
#define DATE_IMPL_GUARD

#include <libfam/string.h>
#include <libfam/utils.h>

static bool is_leap_year(u32 y) {
	return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static const u64 days_in_month[12] = {31, 28, 31, 30, 31, 30,
				      31, 31, 30, 31, 30, 31};

PUBLIC void unix_to_tm(u64 timestamp, u32 *year, u32 *month, u32 *day,
		       u32 *hour, u32 *min, u32 *sec) {
	u64 days = timestamp / 86400;
	u64 rem = timestamp % 86400;

	*hour = rem / 3600;
	rem %= 3600;
	*min = rem / 60;
	*sec = rem % 60;

	u32 y = 1970;
	u32 d = 0;

	while (days >= 365UL) {
		u32 yd = 365 + is_leap_year(y);
		if (days >= yd) {
			days -= yd;
			y++;
		} else
			break;
	}

	*year = y;

	*month = 1;
	while (days >=
	       days_in_month[*month - 1] + (*month == 2 && is_leap_year(y))) {
		days -= days_in_month[*month - 1] +
			(*month == 2 && is_leap_year(y));
		(*month)++;
	}

	*day = days + 1;
}

PUBLIC u8 date_calc(char buffer[MAX_DATE_LEN], u64 micros, u8 precision) {
	char *b = buffer;
	u8 len;
	u32 year, month, day, hour, min, sec;
	unix_to_tm(micros / 1000000, &year, &month, &day, &hour, &min, &sec);
	len = u128_to_string(b, year, Int128DisplayTypeDecimal);
	b += len;
	*b = '-';
	b++;
	if (month < 10) *b++ = '0';
	len = u128_to_string(b, month, Int128DisplayTypeDecimal);
	b += len;
	*b = '-';
	b++;
	if (day < 10) *b++ = '0';
	len = u128_to_string(b, day, Int128DisplayTypeDecimal);
	b += len;
	*b = ' ';
	b++;
	if (hour < 10) *b++ = '0';
	len = u128_to_string(b, hour, Int128DisplayTypeDecimal);
	b += len;
	*b = ':';
	b++;
	if (min < 10) *b++ = '0';
	len = u128_to_string(b, min, Int128DisplayTypeDecimal);
	b += len;
	*b = ':';
	b++;
	if (sec < 10) *b++ = '0';
	len = u128_to_string(b, sec, Int128DisplayTypeDecimal);
	b += len;
	if (precision) {
		u8 prec = precision > MAX_SECOND_PRECISION
			      ? MAX_SECOND_PRECISION
			      : precision;
		f64 v = (f64)(micros % 1000000);
		v /= 1000000.0;
		len = f64_to_string(b, v, prec, false, true);
		b += len;
		while (len < prec + 1) {
			len++;
			*b++ = len == 1 ? '.' : '0';
		}
	}
	*b = 0;
	return b - buffer;
}

#ifdef TEST

#include <libfam/sysext.h>
#include <libfam/test.h>

Test(date1) {
	char buf[MAX_DATE_LEN] = {0};
	date_calc(buf, 1771487889817395, 1);
	ASSERT(!strcmp(buf, "2026-02-19 07:58:09.8"), "2026-02-19 07:58:09.8");
}
#endif /* TEST */

#endif /* DATE_IMPL_GUARD */
#endif /* DATE_IMPL */
