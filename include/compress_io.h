#ifndef _COMPRESS_IO_H__
#define _COMPRESS_IO_H__

#include <compress.h>

int compress_file(int infd, unsigned long in_offset, int outfd,
		  unsigned long out_offset);
int decompress_file(int infd, unsigned long in_offset, int outfd,
		    unsigned long out_offset);
int compress_stream(int infd, unsigned long in_offset, int outfd,
		    unsigned long out_offset);
int decompress_stream(int infd, unsigned long in_offset, int outfd,
		      unsigned long out_offset);

#endif /* _COMPRESS_IO_H__ */

#ifdef COMPRESS_IO_IMPL
#ifndef COMPRESS_IO_IMPL_GUARD
#define COMPRESS_IO_IMPL_GUARD

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

#define MAX_PROCS 128

typedef struct {
	unsigned long next_chunk;
	unsigned long next_write;
	unsigned long chunks;
	unsigned char procs;
	int infd;
	unsigned long in_offset;
	int outfd;
	unsigned long out_offset;
	unsigned err;
} CompressState;

typedef struct {
	unsigned long next_chunk;
	unsigned long next_write;
	unsigned long chunks;
	unsigned char procs;
	int infd;
	unsigned long in_offset;
	unsigned long in_len;
	int outfd;
	unsigned long out_offset;
	unsigned long *chunk_offsets;
	unsigned long chunk_offset_allocation;
	unsigned err;
} DecompressState;

int compress_file(int infd, unsigned long in_offset, int outfd,
		  unsigned long out_offset) {
	int ret = 0, i, pids[MAX_PROCS];
	CompressState *state = 0;
	struct statx st, outst;

	state = smap(sizeof(CompressState));
	if (!state) return -1;
	if (fstatx(infd, &st) < 0) return -1;
	if (fstatx(outfd, &outst) < 0) return -1;
	if (st.stx_size <= in_offset || !S_ISREG(st.stx_mode) ||
	    !S_ISREG(outst.stx_mode)) {
		ret = -EINVAL;
		goto cleanup;
	}

	state->chunks = ((st.stx_size - in_offset) + MAX_COMPRESS_LEN - 1) /
			MAX_COMPRESS_LEN;
	state->procs = min(get_physical_cores_cpuid(), state->chunks);
	state->procs = min(state->procs, MAX_PROCS);
	state->infd = infd;
	state->in_offset = in_offset;
	state->outfd = outfd;
	state->out_offset = out_offset;

	for (i = 0; i < (state->procs - 1); i++) {
		pids[i] = fork();
		if (pids[i] < 0) {
			ret = -pids[i];
			goto cleanup;
		}
		if (!pids[i]) {
			compress_run_proc(i, state);
			exit_group(0);
		}
	}
	compress_run_proc(i, state);
	for (u32 i = 0; i < state->procs; i++) waitpid(pids[i]);
	if (state->err) {
		ret = -state->err;
	}
cleanup:
	if (state) munmap(state, sizeof(CompressState));
	return ret;
}

#endif /* COMPRESS_IO_IMPL_GUARD */
#endif /* COMPRESS_IO_IMPL */

