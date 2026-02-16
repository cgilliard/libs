#ifndef _TIME_H
#define _TIME_H

#include <libfam/types.h>

struct timespec {
	u64 tv_sec;
	u64 tv_nsec;
};

struct timeval {
	u64 tv_sec;
	u64 tv_usec;
};

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID 3
#define CLOCK_MONOTONIC_RAW 4
#define CLOCK_REALTIME_COARSE 5
#define CLOCK_MONOTONIC_COARSE 6
#define CLOCK_BOOTTIME 7
#define CLOCK_REALTIME_ALARM 8
#define CLOCK_BOOTTIME_ALARM 9

#endif /* _TIME_H */
