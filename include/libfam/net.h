#ifndef _NET_H
#define _NET_H

#include <libfam/types.h>

#define AF_INET 2
#define SOCK_STREAM 1
#define SOL_SOCKET 1
#define SOCK_DGRAM 2
#define SO_REUSEADDR 2
#define INADDR_ANY 0x00000000U

struct sockaddr {
	u16 sa_family;
	char sa_data[14];
};

struct in_addr {
	u32 s_addr;
};

struct sockaddr_in {
	u16 sin_family;
	u16 sin_port;
	struct in_addr sin_addr;
	u8 sin_zero[8];
};

static inline u16 htons(u16 hostshort) {
	return (hostshort << 8) | (hostshort >> 8);
}

static inline u32 htonl(u32 hostlong) {
	return ((hostlong & 0xFF000000U) >> 24) |
	       ((hostlong & 0x00FF0000U) >> 8) |
	       ((hostlong & 0x0000FF00U) << 8) |
	       ((hostlong & 0x000000FFU) << 24);
}

struct msghdr {
	void *msg_name;
	u64 msg_namelen;
	struct iovec *msg_iov;
	u64 msg_iovlen;
	void *msg_control;
	u64 msg_controllen;
	i32 msg_flags;
};

#define ntohs(x) htons(x)

#endif /* _NET_H */
