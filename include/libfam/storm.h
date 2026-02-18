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

#ifndef _STORM_H
#define _STORM_H

#include <libfam/types.h>

#define STORM_CONTEXT_SIZE 192

typedef struct {
	__attribute__((aligned(32))) u8 _data[STORM_CONTEXT_SIZE];
} StormContext;

void storm_init(StormContext *ctx, const u8 key[32]);
void storm_next_block(StormContext *ctx, u8 buf[32]);
void storm_xcrypt_buffer(StormContext *s, u8 buf[32]);

#endif /* _STORM_H */

#ifdef STORM_IMPL
#ifndef STORM_IMPL_GUARD
#define STORM_IMPL_GUARD

#include <libfam/string.h>
#include <libfam/utils.h>

const __attribute__((aligned(32))) u64 STORM_NUMS[] = {
    0x2d358dccaa6c78a5, 0x8bb84b93962eacc9, 0x4b33a62ed433d4a3,
    0x4d5a2da51de1aa47, 0xa0e44dd4f590aa88, 0x8e13094e6a479dbd,
    0xdd15030f1fa20605, 0x4d24ccf1bfb9486d, 0x8885ab75ca0bcadc,
    0x4fd858ead44132fd, 0x9f6a611cc14e0d51, 0x7a9ef3ef6c6c7e3d,
    0xf621a0e1d2218530, 0x2475b7c896728f7d, 0x4d009aa897b8d30d,
    0xe14eddb3549b0b7d, 0x66936782b8765b24, 0x93a27794ab59c77d,
    0x1f220bea8dd8cbe9, 0xe589cee443ffb77d};

static const u8 *STORM_KEY_MIX = (const void *)STORM_NUMS;

typedef struct {
	__attribute__((aligned(32))) u8 state[32];
	__attribute__((aligned(32))) u8 key0[32];
	__attribute__((aligned(32))) u8 key1[32];
	__attribute__((aligned(32))) u8 key2[32];
	__attribute__((aligned(32))) u8 key3[32];
	__attribute__((aligned(32))) u8 counter[32];
} StormContextImpl;

STATIC_ASSERT(sizeof(StormContextImpl) == sizeof(StormContext),
	      storm_context_size);

PUBLIC void storm_init(StormContext *ctx, const u8 key[32]) {
	StormContextImpl *st = (StormContextImpl *)ctx;
	for (int i = 0; i < 32; ++i) {
		st->state[i] = key[i] ^ STORM_KEY_MIX[i];
		st->key0[i] = key[i] ^ STORM_KEY_MIX[32 + i];
		st->key1[i] = key[i] ^ STORM_KEY_MIX[64 + i];
		st->key2[i] = key[i] ^ STORM_KEY_MIX[96 + i];
		st->key3[i] = key[i] ^ STORM_KEY_MIX[128 + i];
	}
	memset(st->counter, 0, 32);
}

#ifdef __VAES__
static inline void aesenc256avx(void *data, const void *key) {
	__asm__ __volatile__(
	    "vmovdqu     (%[data]),  %%ymm0           \n\t"
	    "vmovdqu     (%[key]),   %%xmm1           \n\t"
	    "vmovdqu     16(%[key]), %%xmm2           \n\t"
	    "vinserti128 $1, %%xmm2, %%ymm1, %%ymm1   \n\t"
	    "vaesenc     %%ymm1,     %%ymm0, %%ymm0   \n\t"
	    "vmovdqu     %%ymm0,     (%[data])        \n\t"

	    :
	    : [data] "r"(data), [key] "r"(key)
	    : "xmm0", "xmm1", "xmm2", "ymm0", "ymm1", "memory");
}
#endif /* __VAES__ */

#endif /* STORM_IMPL_GUARD */
#endif /* STORM_IMPL */
