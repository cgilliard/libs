#ifndef _COMPRESS_H
#define _COMPRESS_H

#define MAX_COMPRESS_LEN (1 << 18)

long compress_bound(unsigned source_len);
int compress_block(const void *in, unsigned len, void *out, unsigned capacity);
int decompress_block(const void *in, unsigned len, void *out,
		     unsigned capacity);

#ifdef COMPRESS_IMPL
#ifndef COMPRESS_IMPL_GUARD
#define COMPRESS_IMPL_GUARD

#define EFAULT 14
#define EINVAL 22

#define MAX_MATCH_LEN 256
#define MIN_MATCH_LEN 4
#define MAX_CODE_LENGTH 11
#define REPEAT_VALUE_INDEX (MAX_CODE_LENGTH + 1)
#define REPEAT_ZERO_LONG_INDEX (MAX_CODE_LENGTH + 2)
#define REPEAT_ZERO_SHORT_INDEX (MAX_CODE_LENGTH + 3)
#define MAX_BOOK_CODE_LENGTH 7
#define MAX_BOOK_CODES (MAX_CODE_LENGTH + 4)
#define LEN_SHIFT 4
#define DIST_MASK 0xF
#define SYMBOL_TERM 256
#define MATCH_OFFSET (SYMBOL_TERM + 1)
#define MAX_MATCH_CODE 127
#define SYMBOL_COUNT (MATCH_OFFSET + MAX_MATCH_CODE + 1)
#define HASH_CONSTANT 0x9e3779b9U
#define LZ_HASH_ENTRIES (1 << 16)

#define CEIL(x, y) ((x + (y - 1)) & ~(y - 1))
#define SET_HASH(table, in, i) \
	table[((*(const unsigned *)((in) + (i))) * HASH_CONSTANT) >> 16] = i;
#ifdef __TINYC__
#define MATCH_CODE(len, dist) \
	(((31 - clz32((len) - 3)) << LEN_SHIFT) | (31 - clz32(dist)))
#else
#define MATCH_CODE(len, dist)                             \
	(((31 - __builtin_clz((len) - 3)) << LEN_SHIFT) | \
	 (31 - __builtin_clz(dist)))
#endif
#define LEN_EXTRA_BITS(mc) ((mc) >> LEN_SHIFT)
#define DIST_EXTRA_BITS(mc) ((mc) & DIST_MASK)
#define LEN_BASE(mc) ((1 << ((mc) >> LEN_SHIFT)) - 1)
#define DIST_BASE(mc) (1 << ((mc) & DIST_MASK))
#define LEN_EXTRA_BITS_VALUE(code, actual_length) \
	(actual_length - LEN_BASE(code) - 4)
#define DIST_EXTRA_BITS_VALUE(code, actual_distance) \
	((actual_distance) - (1 << ((code) & DIST_MASK)))

#define min(a, b) ((a) - (((a) - (b)) & -((a) > (b))))
#ifdef __TINYC__
#define FLUSH_STREAM(buffer, bits_in_buffer, out_bit_offset, data)            \
	do {                                                                  \
		unsigned long bits_to_write, bits_mask, *data64, existing,    \
		    bit_offset, byte_pos;                                     \
		unsigned char new_bits, mask, current_byte;                   \
		bit_offset = out_bit_offset & 0x7;                            \
		byte_pos = out_bit_offset >> 3;                               \
		out_bit_offset += bits_in_buffer;                             \
		bits_to_write = 8 - bit_offset;                               \
		bits_to_write = min(bits_to_write, bits_in_buffer);           \
		new_bits =                                                    \
		    (unsigned char)(buffer & bitstream_masks[bits_to_write]); \
		new_bits <<= bit_offset;                                      \
		mask = bitstream_partial_masks[bit_offset][bits_to_write];    \
		current_byte = data[byte_pos];                                \
		data[byte_pos] = (current_byte & mask) | new_bits;            \
		buffer >>= bits_to_write;                                     \
		bits_in_buffer -= bits_to_write;                              \
		byte_pos++;                                                   \
		bits_mask = bitstream_masks[bits_in_buffer];                  \
		data64 = (unsigned long *)(data + byte_pos);                  \
		existing = *data64;                                           \
		*data64 = (existing & ~bits_mask) | (buffer & bits_mask);     \
		buffer = bits_in_buffer = 0;                                  \
	} while (0);
#else
#define FLUSH_STREAM(buffer, bits_in_buffer, out_bit_offset, data)            \
	do {                                                                  \
		unsigned long bits_to_write, bits_mask, *data64, existing,    \
		    bit_offset, byte_pos;                                     \
		unsigned char new_bits, mask, current_byte;                   \
		bit_offset = out_bit_offset & 0x7;                            \
		byte_pos = out_bit_offset >> 3;                               \
		__builtin_prefetch(data + byte_pos, 1, 3);                    \
		out_bit_offset += bits_in_buffer;                             \
		bits_to_write = 8 - bit_offset;                               \
		bits_to_write = min(bits_to_write, bits_in_buffer);           \
		new_bits =                                                    \
		    (unsigned char)(buffer & bitstream_masks[bits_to_write]); \
		new_bits <<= bit_offset;                                      \
		mask = bitstream_partial_masks[bit_offset][bits_to_write];    \
		current_byte = data[byte_pos];                                \
		data[byte_pos] = (current_byte & mask) | new_bits;            \
		buffer >>= bits_to_write;                                     \
		bits_in_buffer -= bits_to_write;                              \
		byte_pos++;                                                   \
		bits_mask = bitstream_masks[bits_in_buffer];                  \
		data64 = (unsigned long *)(data + byte_pos);                  \
		existing = *data64;                                           \
		*data64 = (existing & ~bits_mask) | (buffer & bits_mask);     \
		buffer = bits_in_buffer = 0;                                  \
	} while (0);
#endif

static const unsigned char bitstream_partial_masks[8][9] = {
    {255, 254, 252, 248, 240, 224, 192, 128, 0},
    {255, 253, 249, 241, 225, 193, 129, 1, 1},
    {255, 251, 243, 227, 195, 131, 3, 3, 3},
    {255, 247, 231, 199, 135, 7, 7, 7, 7},
    {255, 239, 207, 143, 15, 15, 15, 15, 15},
    {255, 223, 159, 31, 31, 31, 31, 31, 31},
    {255, 191, 63, 63, 63, 63, 63, 63, 63},
    {255, 127, 127, 127, 127, 127, 127, 127, 127}};

static const unsigned long bitstream_masks[65] = {
    0x0000000000000000UL, 0x0000000000000001UL, 0x0000000000000003UL,
    0x0000000000000007UL, 0x000000000000000FUL, 0x000000000000001FUL,
    0x000000000000003FUL, 0x000000000000007FUL, 0x00000000000000FFUL,
    0x00000000000001FFUL, 0x00000000000003FFUL, 0x00000000000007FFUL,
    0x0000000000000FFFUL, 0x0000000000001FFFUL, 0x0000000000003FFFUL,
    0x0000000000007FFFUL, 0x000000000000FFFFUL, 0x000000000001FFFFUL,
    0x000000000003FFFFUL, 0x000000000007FFFFUL, 0x00000000000FFFFFUL,
    0x00000000001FFFFFUL, 0x00000000003FFFFFUL, 0x00000000007FFFFFUL,
    0x0000000000FFFFFFUL, 0x0000000001FFFFFFUL, 0x0000000003FFFFFFUL,
    0x0000000007FFFFFFUL, 0x000000000FFFFFFFUL, 0x000000001FFFFFFFUL,
    0x000000003FFFFFFFUL, 0x000000007FFFFFFFUL, 0x00000000FFFFFFFFUL,
    0x00000001FFFFFFFFUL, 0x00000003FFFFFFFFUL, 0x00000007FFFFFFFFUL,
    0x0000000FFFFFFFFFUL, 0x0000001FFFFFFFFFUL, 0x0000003FFFFFFFFFUL,
    0x0000007FFFFFFFFFUL, 0x000000FFFFFFFFFFUL, 0x000001FFFFFFFFFFUL,
    0x000003FFFFFFFFFFUL, 0x000007FFFFFFFFFFUL, 0x00000FFFFFFFFFFFUL,
    0x00001FFFFFFFFFFFUL, 0x00003FFFFFFFFFFFUL, 0x00007FFFFFFFFFFFUL,
    0x0000FFFFFFFFFFFFUL, 0x0001FFFFFFFFFFFFUL, 0x0003FFFFFFFFFFFFUL,
    0x0007FFFFFFFFFFFFUL, 0x000FFFFFFFFFFFFFUL, 0x001FFFFFFFFFFFFFUL,
    0x003FFFFFFFFFFFFFUL, 0x007FFFFFFFFFFFFFUL, 0x00FFFFFFFFFFFFFFUL,
    0x01FFFFFFFFFFFFFFUL, 0x03FFFFFFFFFFFFFFUL, 0x07FFFFFFFFFFFFFFUL,
    0x0FFFFFFFFFFFFFFFUL, 0x1FFFFFFFFFFFFFFFUL, 0x3FFFFFFFFFFFFFFFUL,
    0x7FFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL};

#ifdef __x86_64__
__asm__(
    ".section .text\n"
    "compress_syscall:\n"
    "push    %rbp\n"
    "mov     %rdi, %rbp\n"
    "mov     %rsi, %rax\n"
    "mov     %rdx, %rdi\n"
    "mov     %rcx, %rsi\n"
    "mov     %r8,  %rdx\n"
    "mov     %r9,  %r10\n"
    "mov     16(%rsp), %r8\n"
    "mov     24(%rsp), %r9\n"
    "syscall\n"
    "mov     %rax, 0(%rbp)\n"
    "pop     %rbp\n"
    "ret\n");

void compress_syscall(void *ret, long sysno, long a0, long a1, long a2, long a3,
		      long a4, long a5);
#endif /* __x86_64__ */

static long compress_write_syscall(int fd, const void *buf, unsigned long len) {
	long ret;
#ifdef __x86_64__
	compress_syscall(&ret, 1, fd, (long)buf, len, 0, 0, 0);
#else
	ret = 0;
#endif /* !__x86_64__ */
	return ret;
}

static unsigned long compress_strlen(const char *x) {
	const char *y = x;
	while (*x) x++;
	return x - y;
}

static void *compress_memcpy(void *dest, const void *src, unsigned long n) {
	char *d = dest;
	const char *s = src;
	while (n--) *d++ = *s++;
	return dest;
}

static long write_str(long fd, char *msg) {
	long len = compress_strlen(msg);
	return compress_write_syscall(fd, msg, len);
}

static long write_num(int fd, long num) {
	unsigned char buf[21];
	unsigned char *p;
	unsigned long len;
	long written;
	if (fd < 0) return -1;

	p = buf + sizeof(buf) - 1;
	*p = '\0';

	if (num == 0)
		*--p = '0';
	else
		while (num > 0) {
			*--p = '0' + (num % 10);
			num /= 10;
		}

	len = buf + sizeof(buf) - 1 - p;
	written = compress_write_syscall(fd, p, len);
	if (written < 0) return -1;
	if ((unsigned long)written != len) return -1;
	return 0;
}

#ifdef __TINYC__
static int clz32(unsigned int x) {
	int r;
	__asm__("bsrl %1, %0" : "=r"(r) : "r"(x) : "cc");
	return 31 - r;
}
static void *compress_memset(void *dest, int c, unsigned long n) {
	char *tmp = dest;
	while (n--) *tmp++ = (char)c;
	return dest;
}
#endif /* __TINYC__ */

static void compress_zero_memory(void *ptr, long len_bytes) {
#ifdef __AVX2__
	char *p = (char *)ptr;
	char *end = p + len_bytes;
	__asm__ __volatile__(
	    "vpxor  %%ymm0, %%ymm0, %%ymm0\n\t"
	    "0:\n\t"
	    "vmovdqu %%ymm0, (%0)\n\t"
	    "add    $32, %0\n\t"
	    "cmp    %0, %1\n\t"
	    "jb     0b"
	    : "+r"(p)
	    : "r"(end)
	    : "ymm0", "cc", "memory");
#else
	compress_memset(ptr, 0, len_bytes);
#endif
}

unsigned compress_find_matches(const void *__restrict in, unsigned len,
			       unsigned short *__restrict match_array,
			       unsigned *__restrict frequencies, void *out);
unsigned compress_find_matches(const void *in_v, unsigned len,
			       unsigned short *__restrict match_array,
			       unsigned *__restrict frequencies,
			       void *__restrict out_v) {
	const unsigned char *in;
	unsigned long buffer, bits_in_buffer;
	unsigned char *data, *out;
	unsigned i, max, maitt, out_bit_offset;
	unsigned short table[LZ_HASH_ENTRIES];

	in = in_v;
	out = out_v;
	data = out + sizeof(unsigned);
	compress_zero_memory(frequencies, CEIL(SYMBOL_COUNT, 8));
	compress_zero_memory(table, sizeof(table));
	buffer = bits_in_buffer = 0;
	out_bit_offset = i = maitt = 0;
	max = len >= 32 + MAX_MATCH_LEN ? len - (32 + MAX_MATCH_LEN) : 0;

	while (i < max) {
		unsigned key, mpos;
		unsigned short entry, dist, mlen;

		key = *(const unsigned *)(in + i);
		entry = (key * HASH_CONSTANT) >> 16;
		dist = (unsigned short)i - table[entry];
		table[entry] = i;
		mlen = 0;
		mpos = i - dist;

		if (dist) {
#ifdef __AVX2__
			unsigned mask;
			do {
				const void *ptr1, *ptr2;
				ptr1 = in + mpos + mlen;
				ptr2 = in + i + mlen;

				__asm__ volatile(
				    "vmovdqu (%[ptr1]), %%ymm0\n\t"
				    "vmovdqu (%[ptr2]), %%ymm1\n\t"
				    "vpcmpeqb %%ymm1, %%ymm0, %%ymm0\n\t"
				    "vpmovmskb %%ymm0, %[mask]\n\t"
				    : [mask] "=r"(mask)
				    : [ptr1] "r"(ptr1), [ptr2] "r"(ptr2)
				    : "ymm0", "ymm1", "cc", "memory");

				mlen += (mask != 0xFFFFFFFF) *
					    __builtin_ctz(~mask) +
					(mask == 0xFFFFFFFF) * 32;
			} while (mask == 0xFFFFFFFF && mlen < MAX_MATCH_LEN);
#else
			while (mlen < MAX_MATCH_LEN &&
			       in[i + mlen] == in[mpos + mlen])
				mlen++;
#endif /* !__AVX2__ */
		}
		if (mlen >= MIN_MATCH_LEN) {
			unsigned char mc, extra_sum;
			unsigned short match_symbol;
			unsigned long combined_extra;
			mc = MATCH_CODE(mlen, dist);
			extra_sum = LEN_EXTRA_BITS(mc) + DIST_EXTRA_BITS(mc);
			combined_extra =
			    ((unsigned)DIST_EXTRA_BITS_VALUE(mc, dist)
			     << LEN_EXTRA_BITS(mc)) |
			    LEN_EXTRA_BITS_VALUE(mc, mlen);

			if (__builtin_expect(bits_in_buffer + extra_sum > 64,
					     0))
				FLUSH_STREAM(buffer, bits_in_buffer,
					     out_bit_offset, data);

			buffer |= combined_extra << bits_in_buffer;
			bits_in_buffer += extra_sum;

			match_symbol = (unsigned short)mc + MATCH_OFFSET;
			frequencies[match_symbol]++;
			match_array[maitt++] = match_symbol;
			SET_HASH(table, in, i + 1);
			SET_HASH(table, in, i + 2);
			SET_HASH(table, in, i + 3);
			i += mlen;
		}
		frequencies[in[i]]++;
		match_array[maitt++] = in[i++];
	}
	while (i < len) {
		frequencies[in[i]]++;
		match_array[maitt++] = in[i++];
	}

	FLUSH_STREAM(buffer, bits_in_buffer, out_bit_offset, data);
	compress_memcpy(out, &out_bit_offset, sizeof(unsigned));

	match_array[maitt] = SYMBOL_TERM;
	frequencies[SYMBOL_TERM]++;
	return out_bit_offset;
}

long compress_bound(unsigned source_len) { return (long)source_len + 3; }

int compress_block(const void *in, unsigned len, void *out, unsigned capacity) {
	unsigned short match_array[MAX_COMPRESS_LEN + 2];
	unsigned frequencies[CEIL(SYMBOL_COUNT, 8)];

	if (!in || !out) return -EFAULT;
	if (capacity < compress_bound(len) || len > MAX_COMPRESS_LEN)
		return -EINVAL;

	compress_find_matches(in, len, match_array, frequencies, out);
	{
		long i;
		for (i = 0; i < SYMBOL_COUNT; i++) {
			if (frequencies[i]) {
				write_str(2, "frequency[");
				write_num(2, i);
				write_str(2, "]=");
				write_num(2, frequencies[i]);
				write_str(2, "\n");
			}
		}
	}
	return 0;
	(void)capacity;
}

int decompress_block(const void *in, unsigned len, void *out,
		     unsigned capacity) {
	return 0;
	(void)in;
	(void)len;
	(void)out;
	(void)capacity;
}

#endif
#endif
#endif /* _COMPRESS_H */
