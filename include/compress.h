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

#ifndef EFAULT
#define EFAULT 14
#endif
#ifndef EINVAL
#define EINVAL 22
#endif
#ifndef EPROTO
#define EPROTO 71
#endif
#ifndef EOVERFLOW
#define EOVERFLOW 75
#endif

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
#define MATCH_CODE(len, dist)                              \
	(((31 - compress_clz32((len) - 3)) << LEN_SHIFT) | \
	 (31 - compress_clz32(dist)))
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
#define WRITE(buffer, bits_in_buffer, out_bit_offset, data, value, len)      \
	do {                                                                 \
		if ((bits_in_buffer) + (len) > 64)                           \
			FLUSH_STREAM(buffer, bits_in_buffer, out_bit_offset, \
				     data);                                  \
		buffer |= ((unsigned long)(value) << (bits_in_buffer));      \
		(bits_in_buffer) += (len);                                   \
	} while (0);
#define TRY_LOAD(buffer, bits_in_buffer, in_bit_offset, data, capacity)        \
	{                                                                      \
		unsigned long new_bits, high;                                  \
		unsigned long bit_offset = in_bit_offset;                      \
		unsigned long bits_to_load = 64 - bits_in_buffer;              \
		unsigned long end_byte = (bit_offset + bits_to_load + 7) >> 3; \
		unsigned long byte_pos = bit_offset >> 3;                      \
		unsigned char bit_remainder = bit_offset & 0x7;                \
		unsigned long bytes_needed = end_byte - byte_pos;              \
		if (__builtin_expect(end_byte > capacity, 0))                  \
			return -EOVERFLOW;                                     \
		new_bits = *(const unsigned long *)(data + byte_pos);          \
		high =                                                         \
		    bytes_needed == 9 ? (unsigned long)data[byte_pos + 8] : 0; \
		new_bits = (new_bits >> bit_remainder) |                       \
			   (high << (64 - bit_remainder));                     \
		new_bits &= bitstream_masks[bits_to_load];                     \
		buffer |= (new_bits << bits_in_buffer);                        \
		in_bit_offset += bits_to_load;                                 \
		bits_in_buffer += bits_to_load;                                \
	}

#define TRY_READ(res, buffer, bits_in_buffer, in_bit_offset, data, capacity,  \
		 num_bits)                                                    \
	do {                                                                  \
		if ((bits_in_buffer) < (num_bits)) {                          \
			TRY_LOAD(buffer, bits_in_buffer, in_bit_offset, data, \
				 capacity);                                   \
		}                                                             \
		*(res) = buffer & (bitstream_masks[num_bits]);                \
		buffer = buffer >> num_bits;                                  \
		bits_in_buffer -= num_bits;                                   \
	} while (0);

#define PEEK_READER(buffer, num_bits) (buffer & (bitstream_masks[num_bits]))
#define ADVANCE_READER(buffer, bits_in_buffer, num_bits) \
	do {                                             \
		buffer = buffer >> (num_bits);           \
		bits_in_buffer -= (num_bits);            \
	} while (0);

typedef struct {
	unsigned short code;
	unsigned short length;
} CodeLength;

typedef struct HuffmanNode {
	unsigned short symbol;
	unsigned long freq;
	struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct {
	HuffmanNode *nodes[SYMBOL_COUNT * 2 + 1];
	unsigned long size;
} HuffmanMinHeap;

typedef struct {
	unsigned short symbol;
	unsigned char length;
} HuffmanLookup;

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

__inline static unsigned long compress_strlen(const char *x) {
	const char *y = x;
	while (*x) x++;
	return x - y;
}

__inline static void *compress_memcpy(void *dest, const void *src,
				      unsigned long n) {
	char *d = dest;
	const char *s = src;
	while (n--) *d++ = *s++;
	return dest;
}

__inline static void *compress_memcpy_movsb(void *dest, const void *src,
					    unsigned long n) {
#ifdef __x86_64__
	__asm__ __volatile__("rep movsb"
			     : "+D"(dest), "+S"(src), "+c"(n)
			     :
			     : "memory");
#else
	compress_memcpy(dest, src, n);
#endif
	return dest;
}

#ifdef __TINYC__
__inline static int compress_clz32(unsigned int x) {
	int r;
	__asm__("bsrl %1, %0" : "=r"(r) : "r"(x) : "cc");
	return 31 - r;
}
__inline static void *compress_memset(void *dest, int c, unsigned long n) {
	char *tmp = dest;
	while (n--) *tmp++ = (char)c;
	return dest;
}
#endif /* __TINYC__ */

__inline static void compress_zero_memory(void *ptr, long len_bytes) {
/*
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
*/
#ifdef __AVX2__
	char *p = (char *)ptr;
	char *end = p + len_bytes;

	__asm__ __volatile__(
	    "vpxor  %%ymm0, %%ymm0, %%ymm0\n\t"
	    "jmp    2f\n\t"
	    "1:\n\t"
	    "vmovdqu %%ymm0, (%0)\n\t"
	    "add    $32, %0\n\t"
	    "2:\n\t"
	    "cmp    %0, %1\n\t"
	    "ja     1b"
	    : "+r"(p)
	    : "r"(end)
	    : "ymm0", "cc", "memory");
#else
	compress_memset(ptr, 0, len_bytes);
#endif
}

__inline static unsigned compress_find_matches(
    const void *in_v, unsigned len, unsigned short *__restrict match_array,
    unsigned *__restrict frequencies, void *__restrict out_v) {
	const unsigned char *in;
	unsigned long buffer, bits_in_buffer;
	unsigned char *data, *out;
	unsigned i, max, maitt, out_bit_offset;
	unsigned short table[LZ_HASH_ENTRIES];

	in = in_v;
	out = out_v;
	data = out + sizeof(unsigned);
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

__inline static void compress_init_node(HuffmanNode *node,
					unsigned short symbol,
					unsigned long freq) {
	node->symbol = symbol;
	node->freq = freq;
	node->left = node->right = 0;
}

__inline static void compress_swap_nodes(HuffmanNode **a, HuffmanNode **b) {
	HuffmanNode *temp = *a;
	*a = *b;
	*b = temp;
}

__inline static void compress_heapify(HuffmanMinHeap *heap, unsigned long idx) {
	unsigned long smallest = idx;
	unsigned long left = 2 * idx + 1;
	unsigned long right = 2 * idx + 2;

	if (left < heap->size &&
	    heap->nodes[left]->freq < heap->nodes[smallest]->freq)
		smallest = left;
	if (right < heap->size &&
	    heap->nodes[right]->freq < heap->nodes[smallest]->freq)
		smallest = right;

	if (smallest != idx) {
		compress_swap_nodes(&heap->nodes[idx], &heap->nodes[smallest]);
		compress_heapify(heap, smallest);
	}
}

__inline static void compress_insert_heap(HuffmanMinHeap *heap,
					  HuffmanNode *node) {
	unsigned long i = ++heap->size - 1;
	heap->nodes[i] = node;

	while (i && heap->nodes[(i - 1) / 2]->freq > heap->nodes[i]->freq) {
		compress_swap_nodes(&heap->nodes[i], &heap->nodes[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

__inline static HuffmanNode *compress_extract_min(HuffmanMinHeap *heap) {
	HuffmanNode *min;
	if (heap->size == 0) return 0;

	min = heap->nodes[0];
	heap->nodes[0] = heap->nodes[heap->size - 1];
	heap->size--;
	compress_heapify(heap, 0);

	return min;
}

__inline static void compress_compute_lengths(HuffmanNode *node,
					      unsigned char length,
					      CodeLength *code_lengths) {
	if (!node) return;
	if (!node->left && !node->right)
		code_lengths[node->symbol].length = length;
	compress_compute_lengths(node->left, length + 1, code_lengths);
	compress_compute_lengths(node->right, length + 1, code_lengths);
}

__inline static void compress_build_tree(const unsigned *frequencies,
					 CodeLength *code_lengths,
					 unsigned short count,
					 HuffmanMinHeap *heap,
					 HuffmanNode *nodes) {
	int i;
	unsigned short node_counter = 0;

	heap->size = 0;

	for (i = 0; i < count; i++) {
		if (frequencies[i]) {
			HuffmanNode *next = &nodes[node_counter++];
			compress_init_node(next, i, frequencies[i]);
			compress_insert_heap(heap, next);
		}
	}

	if (heap->size == 1) {
		HuffmanNode *node = compress_extract_min(heap);
		code_lengths[node->symbol].length = 1;
	} else {
		while (heap->size > 1) {
			HuffmanNode *left = compress_extract_min(heap);
			HuffmanNode *right = compress_extract_min(heap);
			HuffmanNode *parent = &nodes[node_counter++];
			compress_init_node(parent, 0xFFFF,
					   left->freq + right->freq);
			parent->left = left;
			parent->right = right;
			compress_insert_heap(heap, parent);
		}
	}
}

__inline static void compress_limit_lengths(const unsigned *frequencies,
					    CodeLength *code_lengths,
					    unsigned short count,
					    unsigned char max_length) {
	int i;
	unsigned excess = 0;
	unsigned needed = 0;
	unsigned long sum;

	for (i = 0; i < count; i++) {
		if (code_lengths[i].length > max_length) {
			excess += (code_lengths[i].length - max_length) *
				  frequencies[i];
			code_lengths[i].length = max_length;
			needed += frequencies[i];
		}
	}

	while (excess > 0 && needed > 0) {
		for (i = 0; i < count && excess > 0; i++) {
			if (code_lengths[i].length > 0 &&
			    code_lengths[i].length < max_length &&
			    frequencies[i] > 0) {
				unsigned delta = (excess < frequencies[i])
						     ? excess
						     : frequencies[i];
				code_lengths[i].length++;
				excess -= delta;
				needed -= delta;
			}
		}
	}

	sum = 0;
	for (i = 0; i < count; i++)
		if (code_lengths[i].length > 0)
			sum += 1UL << (max_length - code_lengths[i].length);

	while (sum > (1UL << max_length)) {
		unsigned char old_len;
		unsigned short best = 0;
		unsigned min_freq = 0xFFFFFFFF;
		for (i = 0; i < count; ++i) {
			if (code_lengths[i].length > 1 &&
			    code_lengths[i].length < max_length &&
			    frequencies[i] < min_freq) {
				min_freq = frequencies[i];
				best = i;
			}
		}
		if (min_freq == 0xFFFFFFFF) break;

		old_len = code_lengths[best].length;
		code_lengths[best].length++;

		sum -= (1UL << (max_length - old_len));
		sum += (1UL << (max_length - code_lengths[best].length));
	}
}

__inline static void compress_calculate_lengths(const unsigned *frequencies,
						CodeLength *code_lengths,
						unsigned short count,
						int max_length) {
	HuffmanMinHeap heap;
	HuffmanNode nodes[SYMBOL_COUNT * 2 + 1];
	HuffmanNode *root;
	compress_build_tree(frequencies, code_lengths, count, &heap, nodes);
	if ((root = compress_extract_min(&heap)) != 0) {
		compress_compute_lengths(root, 0, code_lengths);
		compress_limit_lengths(frequencies, code_lengths, count,
				       max_length);
	}
}

__inline static void compress_calculate_codes(CodeLength *code_lengths,
					      unsigned short count) {
	unsigned i, j, code = 0;
	unsigned length_count[CEIL(MAX_CODE_LENGTH + 1, 8)] /*= {0}*/;
	unsigned length_start[CEIL(MAX_CODE_LENGTH + 1, 8)] /*= {0}*/;
	unsigned length_pos[CEIL(MAX_CODE_LENGTH + 1, 8)] /*= {0}*/;

	compress_zero_memory(length_count, sizeof(length_count));
	compress_zero_memory(length_start, sizeof(length_start));
	compress_zero_memory(length_pos, sizeof(length_pos));

	for (i = 0; i < count; i++) length_count[code_lengths[i].length]++;
	for (i = 1; i <= MAX_CODE_LENGTH; i++) {
		code <<= 1;
		length_start[i] = code;
		code += length_count[i];
	}

	for (i = 0; i < count; i++) {
		if (code_lengths[i].length != 0) {
			unsigned short reversed, temp;
			unsigned char len = code_lengths[i].length;

			code_lengths[i].code =
			    length_start[len] + length_pos[len]++;
			code_lengths[i].code &= (1U << len) - 1;

			reversed = 0;
			temp = code_lengths[i].code;
			for (j = 0; j < len; j++) {
				reversed = (reversed << 1) | (temp & 1);
				temp >>= 1;
			}
			code_lengths[i].code = reversed;
		}
	}
}

__inline static void compress_build_code_book(
    const CodeLength code_lengths[CEIL(SYMBOL_COUNT, 8)],
    CodeLength book[CEIL(MAX_BOOK_CODES, 8)],
    unsigned frequencies[CEIL(MAX_BOOK_CODES, 8)]) {
	unsigned short i;
	unsigned char last_length = 0;
	for (i = 0; i < SYMBOL_COUNT; i++) {
		if (code_lengths[i].length) {
			if (last_length == code_lengths[i].length && i > 0) {
				unsigned short repeat = 1;
				while (i + repeat < SYMBOL_COUNT &&
				       code_lengths[i + repeat].length ==
					   last_length &&
				       repeat < 6) {
					repeat++;
				}
				if (repeat >= 3) {
					frequencies[REPEAT_VALUE_INDEX]++;
					i += repeat - 1;
					last_length = 0;
					continue;
				}
			}
			last_length = code_lengths[i].length;
			frequencies[code_lengths[i].length]++;
		} else {
			unsigned short run = i + 1;
			while (run < SYMBOL_COUNT &&
			       code_lengths[run].length == 0)
				run++;
			run -= i;

			if (run >= 11) {
				run = run > 138 ? 127 : run - 11;
				i += run + 10;
				frequencies[REPEAT_ZERO_LONG_INDEX]++;
			} else if (run >= 3) {
				run = run - 3;
				i += run + 2;
				frequencies[REPEAT_ZERO_SHORT_INDEX]++;
			} else
				frequencies[0]++;
			last_length = 0;
		}
	}
	compress_calculate_lengths(frequencies, book, MAX_BOOK_CODES,
				   MAX_BOOK_CODE_LENGTH);
	compress_calculate_codes(book, MAX_BOOK_CODES);
}

__inline static int compress_calculate_block_type(
    const unsigned frequencies[SYMBOL_COUNT],
    const unsigned book_frequencies[MAX_BOOK_CODES],
    const CodeLength code_lengths[SYMBOL_COUNT],
    const CodeLength book[MAX_BOOK_CODES], unsigned len) {
	unsigned sum = 0, i;
	for (i = 0; i < SYMBOL_COUNT; i++) {
		sum += frequencies[i] * code_lengths[i].length;
		if (i >= MATCH_OFFSET)
			sum += frequencies[i] *
			       (DIST_EXTRA_BITS(i - MATCH_OFFSET) +
				LEN_EXTRA_BITS(i - MATCH_OFFSET));
	}
	sum += MAX_BOOK_CODES * 3;
	for (i = 0; i < MAX_BOOK_CODES; i++) {
		sum += book_frequencies[i] * book[i].length;
		if (i == REPEAT_VALUE_INDEX) sum += book_frequencies[i] * 2;
		if (i == REPEAT_ZERO_LONG_INDEX) sum += book_frequencies[i] * 7;
		if (i == REPEAT_ZERO_SHORT_INDEX)
			sum += book_frequencies[i] * 3;
	}
	sum += 7 + 64 + 64;
	sum >>= 3;
	return sum > len;
}

__inline static int compress_write_raw(const unsigned char *in, unsigned len,
				       unsigned char *out) {
	unsigned value;
	if (!len) {
		out[0] = 0x0;
		out[1] = 0x0;
		out[2] = 0x80;
		return 3;
	}
	value = len | 0x00800000;
	compress_memcpy(out, &value, 3);
	compress_memcpy_movsb(out + 3, in, len);
	return len + 3;
}

__inline static int compress_write(
    const CodeLength code_lengths[SYMBOL_COUNT],
    const CodeLength book[MAX_BOOK_CODES],
    const unsigned short match_array[MAX_COMPRESS_LEN + 2], unsigned char *out,
    unsigned out_bit_offset) {
	unsigned i;
	unsigned long buffer = 0, bits_in_buffer = 0;
	unsigned char last_length = 0;
	unsigned char *data = out + sizeof(unsigned);
	for (i = 0; i < MAX_BOOK_CODES; i++) {
		WRITE(buffer, bits_in_buffer, out_bit_offset, data,
		      book[i].length, 3);
	}
	for (i = 0; i < SYMBOL_COUNT; i++) {
		if (code_lengths[i].length) {
			if (last_length == code_lengths[i].length) {
				unsigned short repeat = 1;
				while (i + repeat < SYMBOL_COUNT &&
				       code_lengths[i + repeat].length ==
					   last_length &&
				       repeat < 6) {
					repeat++;
				}
				if (repeat >= 3) {
					WRITE(buffer, bits_in_buffer,
					      out_bit_offset, data,
					      book[REPEAT_VALUE_INDEX].code,
					      book[REPEAT_VALUE_INDEX].length);
					WRITE(buffer, bits_in_buffer,
					      out_bit_offset, data, repeat - 3,
					      2);
					i += repeat - 1;
					last_length = 0;
					continue;
				}
			}

			WRITE(buffer, bits_in_buffer, out_bit_offset, data,
			      book[code_lengths[i].length].code,
			      book[code_lengths[i].length].length);
			last_length = code_lengths[i].length;
		} else {
			unsigned short run = i + 1;
			while (run < SYMBOL_COUNT &&
			       code_lengths[run].length == 0)
				run++;
			run -= i;
			if (run >= 11) {
				run = run > 138 ? 127 : run - 11;
				WRITE(buffer, bits_in_buffer, out_bit_offset,
				      data, book[REPEAT_ZERO_LONG_INDEX].code,
				      book[REPEAT_ZERO_LONG_INDEX].length);
				WRITE(buffer, bits_in_buffer, out_bit_offset,
				      data, run, 7);
				i += run + 10;
			} else if (run >= 3) {
				run = run - 3;
				WRITE(buffer, bits_in_buffer, out_bit_offset,
				      data, book[REPEAT_ZERO_SHORT_INDEX].code,
				      book[REPEAT_ZERO_SHORT_INDEX].length);
				WRITE(buffer, bits_in_buffer, out_bit_offset,
				      data, run, 3);
				i += run + 2;
			} else
				WRITE(buffer, bits_in_buffer, out_bit_offset,
				      data, book[0].code, book[0].length);

			last_length = 0;
		}
	}

	i = 0;
	while (match_array[i] != SYMBOL_TERM) {
		unsigned short symbol = match_array[i++];
		unsigned short code = code_lengths[symbol].code;
		unsigned char length = code_lengths[symbol].length;
		WRITE(buffer, bits_in_buffer, out_bit_offset, data, code,
		      length);
	}
	WRITE(buffer, bits_in_buffer, out_bit_offset, data,
	      code_lengths[SYMBOL_TERM].code, code_lengths[SYMBOL_TERM].length);
	WRITE(buffer, bits_in_buffer, out_bit_offset, data, 0, 64);
	WRITE(buffer, bits_in_buffer, out_bit_offset, data, 0, 64);
	FLUSH_STREAM(buffer, bits_in_buffer, out_bit_offset, data);

	return (out_bit_offset + 7) / 8;
}

__inline static int compress_read_raw(const unsigned char *in, unsigned len,
				      unsigned char *out, unsigned capacity) {
	unsigned block_len;
	unsigned char bytes[4];

	if (len < 3) return -EOVERFLOW;

	if (len == 3) {
		if (in[0] != 0x0 || in[1] != 0 || in[2] != 0x80)
			return -EOVERFLOW;
		return 0;
	}
	bytes[0] = in[0];
	bytes[1] = in[1];
	bytes[2] = in[2] & ~0x80;
	bytes[3] = 0;
	block_len = (*(unsigned *)bytes);
	if (block_len > capacity) return -EOVERFLOW;
	if (len < block_len + 3) return -EOVERFLOW;

	compress_memcpy_movsb(out, in + 3, block_len);
	return block_len;
}

__inline static void compress_build_lookup_table(const CodeLength *code_lengths,
						 unsigned short count,
						 HuffmanLookup *lookup_table,
						 unsigned char max_length) {
	int i, j;
	for (i = 0; i < count; i++) {
		if (code_lengths[i].length) {
			int index = code_lengths[i].code &
				    ((1U << code_lengths[i].length) - 1);
			int fill_depth =
			    1U << (max_length - code_lengths[i].length);
			for (j = 0; j < fill_depth; j++) {
				lookup_table[index |
					     (j << code_lengths[i].length)]
				    .length = code_lengths[i].length;
				lookup_table[index |
					     (j << code_lengths[i].length)]
				    .symbol = i;
			}
		}
	}
}

__inline static int compress_read_block(const unsigned char *in, unsigned len,
					unsigned char *out, unsigned capacity) {
	CodeLength code_lengths[CEIL(SYMBOL_COUNT, 8)];
	unsigned i, j;
	unsigned in_bit_offset;
	unsigned long buffer = 0;
	unsigned bits_in_buffer = 0;
	CodeLength book_code_lengths[CEIL(MAX_BOOK_CODES, 8)];
	unsigned short last_length = 0;
	HuffmanLookup book_lookup_table[(1U << MAX_BOOK_CODE_LENGTH)];
	HuffmanLookup lookup_table[(1U << MAX_CODE_LENGTH)];
	unsigned itt = 0;
	unsigned extra_bits_offset = 32, extra_bits_bits_in_buffer = 0;
	unsigned long extra_bits_buffer = 0;

	if (len < sizeof(unsigned)) return -EOVERFLOW;
	compress_memcpy(&in_bit_offset, in, sizeof(unsigned));

	compress_zero_memory(code_lengths, sizeof(code_lengths));
	compress_zero_memory(book_code_lengths, sizeof(book_code_lengths));
	compress_zero_memory(book_lookup_table, sizeof(book_lookup_table));
	compress_zero_memory(lookup_table, sizeof(lookup_table));

	in_bit_offset += 32;
	for (i = 0; i < MAX_BOOK_CODES; i++) {
		TRY_READ(&book_code_lengths[i].length, buffer, bits_in_buffer,
			 in_bit_offset, in, len, 3);
	}

	compress_calculate_codes(book_code_lengths, MAX_BOOK_CODES);
	compress_build_lookup_table(book_code_lengths, MAX_BOOK_CODES,
				    book_lookup_table, MAX_BOOK_CODE_LENGTH);

	i = 0;
	while (i < SYMBOL_COUNT) {
		unsigned char bits;
		unsigned short code;
		HuffmanLookup entry;
		if (bits_in_buffer < MAX_BOOK_CODE_LENGTH) {
			TRY_LOAD(buffer, bits_in_buffer, in_bit_offset, in,
				 len);
		}
		bits = PEEK_READER(buffer, MAX_BOOK_CODE_LENGTH);
		entry = book_lookup_table[bits];
		code = entry.symbol;
		ADVANCE_READER(buffer, bits_in_buffer, entry.length);
		if (code < REPEAT_VALUE_INDEX) {
			code_lengths[i++].length = code;
			last_length = code;
		} else if (code == REPEAT_VALUE_INDEX) {
			unsigned char repeat;
			if (i == 0 || last_length == 0) return -EPROTO;
			TRY_READ(&repeat, buffer, bits_in_buffer, in_bit_offset,
				 in, len, 2);
			repeat += 3;
			if (i + repeat > SYMBOL_COUNT) return -EPROTO;
			for (j = 0; j < repeat; j++) {
				code_lengths[i++].length = last_length;
			}
		} else if (code == REPEAT_ZERO_LONG_INDEX) {
			unsigned char zeros;
			TRY_READ(&zeros, buffer, bits_in_buffer, in_bit_offset,
				 in, len, 7);
			zeros += 11;
			if (i + zeros > SYMBOL_COUNT) return -EPROTO;
			for (j = 0; j < zeros; j++)
				code_lengths[i++].length = 0;
		} else if (code == REPEAT_ZERO_SHORT_INDEX) {
			unsigned char zeros;
			TRY_READ(&zeros, buffer, bits_in_buffer, in_bit_offset,
				 in, len, 3);
			zeros += 3;
			if (i + zeros > SYMBOL_COUNT) return -EPROTO;
			for (j = 0; j < zeros; j++)
				code_lengths[i++].length = 0;
		}
	}

	compress_calculate_codes(code_lengths, SYMBOL_COUNT);

	compress_build_lookup_table(code_lengths, SYMBOL_COUNT, lookup_table,
				    MAX_CODE_LENGTH);

	while (1) {
		unsigned short bits;
		HuffmanLookup entry;
		unsigned short symbol;
		if (__builtin_expect(bits_in_buffer < MAX_CODE_LENGTH, 0))
			TRY_LOAD(buffer, bits_in_buffer, in_bit_offset, in,
				 len);
		bits = PEEK_READER(buffer, MAX_CODE_LENGTH);
		entry = lookup_table[bits];
		symbol = entry.symbol;
		ADVANCE_READER(buffer, bits_in_buffer, entry.length);
		if (__builtin_expect(symbol < SYMBOL_TERM, 0)) {
			if (itt >= capacity) return -EOVERFLOW;
			out[itt++] = symbol;
		} else if (__builtin_expect(symbol == SYMBOL_TERM, 0)) {
			break;
		} else {
			unsigned char *out_dst, *out_src;
			unsigned char mc = entry.symbol - MATCH_OFFSET;
			unsigned char deb = DIST_EXTRA_BITS(mc);
			unsigned char leb = LEN_EXTRA_BITS(mc);
			unsigned dist = DIST_BASE(mc);
			unsigned short mlen = LEN_BASE(mc) + 4;
			if (__builtin_expect(extra_bits_bits_in_buffer < 22,
					     0)) {
				TRY_LOAD(extra_bits_buffer,
					 extra_bits_bits_in_buffer,
					 extra_bits_offset, in, len);
			}
			mlen += PEEK_READER(extra_bits_buffer, leb);
			ADVANCE_READER(extra_bits_buffer,
				       extra_bits_bits_in_buffer, leb);
			dist += PEEK_READER(extra_bits_buffer, deb);
			ADVANCE_READER(extra_bits_buffer,
				       extra_bits_bits_in_buffer, deb);
			if (__builtin_expect(
				mlen + 32 + itt > capacity || dist > itt, 0))
				return -EOVERFLOW;

			out_dst = out + itt;
			out_src = out + itt - dist;
			itt += mlen;
#ifdef __AVX2__
			if (__builtin_expect(out_src + 32 <= out_dst, 1)) {
				unsigned long chunks = (mlen + 31) >> 5;
				while (chunks--) {
					__asm__ __volatile__(
					    "vmovdqu  (%0),     %%ymm0\n\t"
					    "vmovdqu  %%ymm0,   (%1)\n\t"
					    "add      $32,      %0\n\t"
					    "add      $32,      %1\n\t"
					    : "+r"(out_src), "+r"(out_dst)
					    :
					    : "ymm0", "memory", "cc");
				}
			} else if (__builtin_expect(out_src + 16 <= out_dst,
						    1)) {
				unsigned long chunks = (mlen + 15) >> 4;
				while (chunks--) {
					__asm__ __volatile__(
					    "movdqu   (%0),     %%xmm0\n\t"
					    "movdqu   %%xmm0,   (%1)\n\t"
					    "add      $16,      %0\n\t"
					    "add      $16,      %1\n\t"
					    : "+r"(out_src), "+r"(out_dst)
					    :
					    : "xmm0", "memory", "cc");
				}
			} else if (__builtin_expect(out_src + 8 <= out_dst,
						    1)) {
				unsigned long chunks = (mlen + 7) >> 3;
				while (chunks--) {
					*((unsigned long *)out_dst) =
					    *((unsigned long *)out_src);
					out_src += 8;
					out_dst += 8;
				}
			} else {
				while (mlen--) *out_dst++ = *out_src++;
			}
#else
			while (mlen--) *out_dst++ = *out_src++;
#endif
		}
	}

	return itt;
}

long compress_bound(unsigned source_len) { return (long)source_len + 3; }

int compress_block(const void *in, unsigned len, void *out, unsigned capacity) {
	unsigned short match_array[MAX_COMPRESS_LEN + 2];
	unsigned frequencies[CEIL(SYMBOL_COUNT, 8)];
	CodeLength code_lengths[CEIL(SYMBOL_COUNT, 8)];
	unsigned book_frequencies[CEIL(MAX_BOOK_CODES, 8)];
	CodeLength book[CEIL(MAX_BOOK_CODES, 8)];
	unsigned out_bit_offset;

	if (!in || !out) return -EFAULT;
	if (capacity < compress_bound(len) || len > MAX_COMPRESS_LEN)
		return -EINVAL;

	compress_zero_memory(frequencies, sizeof(frequencies));
	compress_zero_memory(code_lengths, sizeof(code_lengths));
	compress_zero_memory(book_frequencies, sizeof(book_frequencies));
	compress_zero_memory(book, sizeof(book));

	out_bit_offset =
	    compress_find_matches(in, len, match_array, frequencies, out);
	compress_calculate_lengths(frequencies, code_lengths, SYMBOL_COUNT,
				   MAX_CODE_LENGTH);
	compress_calculate_codes(code_lengths, SYMBOL_COUNT);

	compress_build_code_book(code_lengths, book, book_frequencies);

	if (compress_calculate_block_type(frequencies, book_frequencies,
					  code_lengths, book, len))
		return compress_write_raw(in, len, out);
	else
		return compress_write(code_lengths, book, match_array, out,
				      out_bit_offset);
}

int decompress_block(const void *in, unsigned len, void *out,
		     unsigned capacity) {
	if (in == 0 || out == 0) return -EINVAL;
	if (len < 3) return -EOVERFLOW;
	if ((((const unsigned char *)in)[2] & 0x80) != 0)
		return compress_read_raw(in, len, out, capacity);
	else
		return compress_read_block(in, len, out, capacity);
}

#endif /* COMPRESS_IMPL_GUARD */
#endif /* COMPRESS_IMPL */
#endif /* _COMPRESS_H__ */
