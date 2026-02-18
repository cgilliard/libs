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

#include <libfam/aesenc.h>
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
PUBLIC void storm_next_block(StormContext *ctx_void, u8 buf[32]) {
	StormContextImpl *ctx = (StormContextImpl *)ctx_void;
	__asm__ __volatile__(
	    "vmovdqa     (%[ctx]),      %%ymm0           \n\t"
	    "vpxor       (%[buf]),      %%ymm0, %%ymm0   \n\t"
	    "vaesenc     0x20(%[ctx]),  %%ymm0, %%ymm0   \n\t"
	    "vextracti128 $1,           %%ymm0, %%xmm1   \n\t"
	    "vpxor       %%xmm1,        %%xmm0, %%xmm2   \n\t"
	    "vinserti128 $1,            %%xmm2, %%ymm1, %%ymm1   \n\t"
	    "vmovdqa     %%ymm1,        (%[ctx])         \n\t"
	    "vaesenc     0x40(%[ctx]),  %%ymm0, %%ymm0   \n\t"
	    "vpxor       %%ymm0,        %%ymm1, %%ymm0   \n\t"
	    "vaesenc     0x60(%[ctx]),  %%ymm0, %%ymm0   \n\t"
	    "vaesenc     0x80(%[ctx]),  %%ymm0, %%ymm0   \n\t"
	    "vmovdqa     %%ymm0,        (%[buf])         \n\t"
	    "vzeroupper                                  \n\t"
	    :
	    : [ctx] "r"(ctx), [buf] "r"(buf)
	    : "ymm0", "ymm1", "xmm0", "xmm1", "xmm2", "memory");
}

PUBLIC void storm_xcrypt_buffer(StormContext *ctx_void, u8 buf[32]) {
	StormContextImpl *ctx = (StormContextImpl *)ctx_void;
	__asm__ __volatile__(
	    "vmovdqa     (%[ctx]),      %%ymm0              \n\t"
	    "vpxor       0xa0(%[ctx]),  %%ymm0, %%ymm0      \n\t"
	    "vaesenc     0x20(%[ctx]),  %%ymm0, %%ymm0      \n\t"
	    "vextracti128 $1,           %%ymm0, %%xmm1      \n\t"
	    "vpxor       %%xmm1,        %%xmm0, %%xmm2      \n\t"
	    "vinserti128 $1,            %%xmm2, %%ymm1, %%ymm1 \n\t"
	    "vmovdqa     %%ymm1,        (%[ctx])            \n\t"
	    "vaesenc     0x40(%[ctx]),  %%ymm0, %%ymm0      \n\t"
	    "vpxor       %%ymm0,        %%ymm1, %%ymm0      \n\t"
	    "vaesenc     0x60(%[ctx]),  %%ymm0, %%ymm0      \n\t"
	    "vaesenc     0x80(%[ctx]),  %%ymm0, %%ymm0      \n\t"
	    "vpxor       (%[buf]),      %%ymm0, %%ymm0      \n\t"
	    "vmovdqa     %%ymm0,        (%[buf])            \n\t"
	    "vmovdqa     0xa0(%[ctx]),  %%ymm0              \n\t"
	    "vpcmpeqd    %%ymm1,        %%ymm1, %%ymm1      \n\t"
	    "vpsubq      %%ymm1,        %%ymm0, %%ymm0      \n\t"
	    "vmovdqa     %%ymm0,        0xa0(%[ctx])        \n\t"
	    "vzeroupper                                     \n\t"
	    :
	    : [ctx] "r"(ctx), [buf] "r"(buf)
	    : "ymm0", "ymm1", "xmm0", "xmm1", "xmm2", "memory");
}
#elif defined(__AES__)
PUBLIC void storm_next_block(StormContext *ctx_void, u8 buf[32]) {
	StormContextImpl *ctx = (StormContextImpl *)ctx_void;
	__asm__ __volatile__(
	    "vmovdqa     (%[ctx]),      %%ymm0           \n\t"
	    "vpxor       (%[buf]),      %%ymm0, %%ymm0   \n\t"
	    "vextracti128 $0x1,         %%ymm0, %%xmm1   \n\t"
	    "vaesenc     0x20(%[ctx]),  %%xmm0, %%xmm0   \n\t"
	    "vaesenc     0x30(%[ctx]),  %%xmm1, %%xmm1   \n\t"
	    "vpxor       %%xmm0,        %%xmm1, %%xmm2   \n\t"
	    "vmovdqa     %%xmm1,        (%[ctx])         \n\t"
	    "vmovdqa     %%xmm2,        0x10(%[ctx])     \n\t"
	    "vaesenc     0x40(%[ctx]),  %%xmm0, %%xmm0   \n\t"
	    "vaesenc     0x50(%[ctx]),  %%xmm1, %%xmm3   \n\t"
	    "vpxor       %%xmm1,        %%xmm0, %%xmm0   \n\t"
	    "vpxor       %%xmm2,        %%xmm3, %%xmm1   \n\t"
	    "vaesenc     0x60(%[ctx]),  %%xmm0, %%xmm0   \n\t"
	    "vaesenc     0x70(%[ctx]),  %%xmm1, %%xmm1   \n\t"
	    "vaesenc     0x80(%[ctx]),  %%xmm0, %%xmm0   \n\t"
	    "vaesenc     0x90(%[ctx]),  %%xmm1, %%xmm1   \n\t"
	    "vmovdqa     %%xmm0,        (%[buf])         \n\t"
	    "vmovdqa     %%xmm1,        0x10(%[buf])     \n\t"
	    "vzeroupper                                  \n\t"

	    :
	    : [ctx] "r"(ctx), [buf] "r"(buf)
	    : "ymm0", "ymm1", "xmm0", "xmm1", "xmm2", "xmm3", "memory");
}
void storm_xcrypt_buffer(StormContext *ctx_void, u8 buf[32]) {
	StormContextImpl *ctx = (StormContextImpl *)ctx_void;
	__asm__ __volatile__(
	    "vmovdqa     (%[ctx]),      %%ymm0           \n\t"
	    "vpxor       0xa0(%[ctx]),  %%ymm0, %%ymm0   \n\t"
	    "vextracti128 $0x1,         %%ymm0, %%xmm1   \n\t"
	    "vaesenc     0x20(%[ctx]),  %%xmm0, %%xmm0   \n\t"
	    "vaesenc     0x30(%[ctx]),  %%xmm1, %%xmm1   \n\t"
	    "vpxor       %%xmm0,        %%xmm1, %%xmm2   \n\t"
	    "vmovdqa     %%xmm1,        (%[ctx])         \n\t"
	    "vmovdqa     %%xmm2,        0x10(%[ctx])     \n\t"
	    "vaesenc     0x40(%[ctx]),  %%xmm0, %%xmm0   \n\t"
	    "vaesenc     0x50(%[ctx]),  %%xmm1, %%xmm3   \n\t"
	    "vpxor       %%xmm1,        %%xmm0, %%xmm0   \n\t"
	    "vpxor       %%xmm2,        %%xmm3, %%xmm1   \n\t"
	    "vaesenc     0x60(%[ctx]),  %%xmm0, %%xmm0   \n\t"
	    "vaesenc     0x70(%[ctx]),  %%xmm1, %%xmm1   \n\t"
	    "vaesenc     0x80(%[ctx]),  %%xmm0, %%xmm0   \n\t"
	    "vaesenc     0x90(%[ctx]),  %%xmm1, %%xmm1   \n\t"
	    "vinserti128 $0x1,          %%xmm1, %%ymm0, %%ymm0   \n\t"
	    "vpxor       (%[buf]),      %%ymm0, %%ymm0   \n\t"
	    "vmovdqa     %%ymm0,        (%[buf])         \n\t"
	    "vmovdqa     0xa0(%[ctx]),  %%ymm0           \n\t"
	    "vpcmpeqd    %%ymm1,        %%ymm1, %%ymm1   \n\t"
	    "vpsubq      %%ymm1,        %%ymm0, %%ymm0   \n\t"
	    "vmovdqa     %%ymm0,        0xa0(%[ctx])     \n\t"
	    "vzeroupper                                  \n\t"

	    :
	    : [ctx] "r"(ctx), [buf] "r"(buf)
	    : "ymm0", "ymm1", "xmm0", "xmm1", "xmm2", "xmm3", "memory");
}
#elif defined(__ARM_NEON) && defined(__ARM_FEATURE_CRYPTO)

PUBLIC void storm_next_block(StormContext *ctx, u8 buf[32]) {
	__asm__ volatile(
	    "ldp     q0, q3, [%1]             \n"
	    "ldp     q1, q2, [%0]             \n"
	    "eor     v4.16b, v0.16b, v1.16b   \n"
	    "eor     v5.16b, v3.16b, v2.16b   \n"
	    "aese    v1.16b, v0.16b           \n"
	    "aesmc   v1.16b, v1.16b           \n"
	    "aese    v2.16b, v3.16b           \n"
	    "aesmc   v2.16b, v2.16b           \n"
	    "stp     q4, q5, [%1]             \n"
	    "ldp     q0, q3, [%0, #32]        \n"
	    "eor     v4.16b, v1.16b, v0.16b   \n"
	    "eor     v5.16b, v2.16b, v3.16b   \n"
	    "aese    v1.16b, v0.16b           \n"
	    "aesmc   v1.16b, v1.16b           \n"
	    "aese    v2.16b, v3.16b           \n"
	    "aesmc   v2.16b, v2.16b           \n"
	    "eor     v6.16b, v5.16b, v4.16b   \n"
	    "stp     q5, q6, [%0]             \n"
	    "stp     q4, q5, [%1]             \n"
	    "ldp     q0, q3, [%0, #64]        \n"
	    "eor     v0.16b, v1.16b, v0.16b   \n"
	    "eor     v1.16b, v2.16b, v3.16b   \n"
	    "stp     q0, q1, [%1]             \n"
	    "ldp     q2, q3, [%0]             \n"
	    "eor     v4.16b, v2.16b, v0.16b   \n"
	    "eor     v5.16b, v3.16b, v1.16b   \n"
	    "aese    v2.16b, v0.16b           \n"
	    "aesmc   v2.16b, v2.16b           \n"
	    "aese    v3.16b, v1.16b           \n"
	    "aesmc   v3.16b, v3.16b           \n"
	    "stp     q4, q5, [%1]             \n"
	    "ldp     q0, q1, [%0, #96]        \n"
	    "eor     v4.16b, v2.16b, v0.16b   \n"
	    "eor     v5.16b, v3.16b, v1.16b   \n"
	    "aese    v2.16b, v0.16b           \n"
	    "aesmc   v2.16b, v2.16b           \n"
	    "aese    v3.16b, v1.16b           \n"
	    "aesmc   v3.16b, v3.16b           \n"
	    "stp     q4, q5, [%1]             \n"
	    "ldp     q0, q1, [%0, #128]       \n"
	    "eor     v0.16b, v2.16b, v0.16b   \n"
	    "eor     v1.16b, v3.16b, v1.16b   \n"
	    "stp     q0, q1, [%1]             \n"
	    :
	    : "r"(ctx), "r"(buf)
	    : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6");
}

PUBLIC void storm_xcrypt_buffer(StormContext *ctx, u8 buf[32]) {
	__asm__ volatile(
	    "ldp     q2, q3, [%0]             \n"
	    "mov     w8, #0x1                 \n"
	    "ldp     q1, q0, [%0, #160]       \n"
	    "ldr     q16, [%0, #144]          \n"
	    "ldp     q4, q5, [%0, #32]        \n"
	    "aese    v2.16b, v1.16b           \n"
	    "aesmc   v2.16b, v2.16b           \n"
	    "aese    v3.16b, v0.16b           \n"
	    "aesmc   v3.16b, v3.16b           \n"
	    "eor     v6.16b, v2.16b, v4.16b   \n"
	    "aese    v2.16b, v4.16b           \n"
	    "aesmc   v2.16b, v2.16b           \n"
	    "eor     v7.16b, v3.16b, v5.16b   \n"
	    "aese    v3.16b, v5.16b           \n"
	    "aesmc   v3.16b, v3.16b           \n"
	    "ldp     q4, q5, [%0, #64]        \n"
	    "eor     v6.16b, v6.16b, v7.16b   \n"
	    "eor     v2.16b, v2.16b, v4.16b   \n"
	    "eor     v3.16b, v3.16b, v5.16b   \n"
	    "stp     q7, q6, [%0]             \n"
	    "aese    v2.16b, v7.16b           \n"
	    "aesmc   v2.16b, v2.16b           \n"
	    "aese    v3.16b, v6.16b           \n"
	    "aesmc   v3.16b, v3.16b           \n"
	    "ldp     q5, q17, [%1]            \n"
	    "ldp     q7, q4, [%0, #112]       \n"
	    "ldr     q6, [%0, #96]            \n"
	    "aese    v2.16b, v6.16b           \n"
	    "aesmc   v2.16b, v2.16b           \n"
	    "dup     v6.2d, x8                \n"
	    "eor     v4.16b, v4.16b, v5.16b   \n"
	    "aese    v3.16b, v7.16b           \n"
	    "aesmc   v3.16b, v3.16b           \n"
	    "eor     v5.16b, v16.16b, v17.16b \n"
	    "add     v1.2d, v1.2d, v6.2d      \n"
	    "add     v0.2d, v0.2d, v6.2d      \n"
	    "eor     v2.16b, v2.16b, v4.16b   \n"
	    "eor     v3.16b, v3.16b, v5.16b   \n"
	    "stp     q2, q3, [%1]             \n"
	    "stp     q1, q0, [%0, #160]       \n"
	    :
	    : "r"(ctx), "r"(buf)
	    : "memory", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16",
	      "v17", "w8", "x8");
}

#else
PUBLIC void storm_next_block(StormContext *ctx, u8 buf[32]) {
	StormContextImpl *st = (StormContextImpl *)ctx;
	u8 x[32], y[32];

	for (int i = 0; i < 32; i++) x[i] = st->state[i] ^ buf[i];
	aesenc256(x, st->key0);
	memcpy(y, x, 32);
	for (int i = 0; i < 16; ++i) {
		st->state[i] = y[i + 16];
		st->state[i + 16] = y[i] ^ y[i + 16];
	}
	aesenc256(y, st->key1);
	memcpy(buf, y, 32);
	for (int i = 0; i < 32; i++) x[i] = st->state[i] ^ buf[i];
	aesenc256(x, st->key2);
	aesenc256(x, st->key3);
	memcpy(buf, x, 32);
}
PUBLIC void storm_xcrypt_buffer(StormContext *ctx, u8 buf[32]) {
	StormContextImpl *st = (StormContextImpl *)ctx;
	u8 block[32];
	memcpy(block, st->counter, 32);
	storm_next_block(ctx, block);
	for (int i = 0; i < 32; i++) buf[i] ^= block[i];
	u64 *counter = (u64 *)st->counter;
	++counter[0];
	++counter[1];
	++counter[2];
	++counter[3];
}
#endif /* !__VAES__ */

/* GCOVR_EXCL_START */
#ifdef TEST

#include <libfam/storm_vectors.h>
#include <libfam/test.h>

Test(storm_vectors) {
	StormContext ctx;
	for (u32 i = 0; i < sizeof(storm_vectors) / sizeof(storm_vectors[0]);
	     i++) {
		storm_init(&ctx, storm_vectors[i].key);
		for (u32 j = 0; j < sizeof(storm_vectors[i].input) /
					sizeof(storm_vectors[i].input[0]);
		     j++) {
			__attribute__((aligned(32))) u8 tmp[32];
			__builtin_memcpy(tmp, storm_vectors[i].input[j], 32);
			storm_next_block(&ctx, tmp);
			i32 res = memcmp(tmp, storm_vectors[i].expected[j], 32);
			if (res) println("i={},j={}", i, j);
			ASSERT(!res, "vector");
		}
	}
}

Test(storm_cipher) {
	StormContext ctx;
	__attribute__((aligned(32))) const u8 SEED[32] = {1, 2, 3};
	__attribute__((aligned(32))) u8 buffer1[32] = {0};
	__attribute__((aligned(32))) u8 buffer2[32] = {0};
	__attribute__((aligned(32))) u8 buffer3[32] = {0};
	__attribute__((aligned(32))) u8 buffer4[32] = {0};
	__attribute__((aligned(32))) u8 buffer5[32] = {0};

	storm_init(&ctx, SEED);
	memcpy(buffer1, "test1", 5);
	storm_xcrypt_buffer(&ctx, buffer1);
	memcpy(buffer2, "test2", 5);
	storm_xcrypt_buffer(&ctx, buffer2);
	memcpy(buffer3, "blahblah", 8);
	storm_xcrypt_buffer(&ctx, buffer3);
	memcpy(buffer4, "ok", 2);
	storm_xcrypt_buffer(&ctx, buffer4);
	memcpy(buffer5, "x", 1);
	storm_xcrypt_buffer(&ctx, buffer5);

	ASSERT(memcmp(buffer1, "test1", 5), "ne1");
	ASSERT(memcmp(buffer2, "test2", 5), "ne2");
	ASSERT(memcmp(buffer3, "blahblah", 8), "ne3");
	ASSERT(memcmp(buffer4, "ok", 2), "ne4");
	ASSERT(memcmp(buffer5, "x", 1), "ne5");

	StormContext ctx2;
	storm_init(&ctx2, SEED);

	storm_xcrypt_buffer(&ctx2, buffer1);
	ASSERT(!memcmp(buffer1, "test1", 5), "eq1");
	storm_xcrypt_buffer(&ctx2, buffer2);
	ASSERT(!memcmp(buffer2, "test2", 5), "eq2");

	storm_xcrypt_buffer(&ctx2, buffer3);
	ASSERT(!memcmp(buffer3, "blahblah", 8), "eq3");

	storm_xcrypt_buffer(&ctx2, buffer4);
	ASSERT(!memcmp(buffer4, "ok", 2), "eq4");

	storm_xcrypt_buffer(&ctx2, buffer5);
	ASSERT(!memcmp(buffer5, "x", 1), "eq5");
}

#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* STORM_IMPL_GUARD */
#endif /* STORM_IMPL */
