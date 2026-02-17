#ifndef _ERRNO_H
#define _ERRNO_H

#define SUCCESS 0	/* Success */
#define EPERM 1		/* Operation not permitted */
#define ENOENT 2	/* No such file or directory */
#define ESRCH 3		/* No such process */
#define EINTR 4		/* Interrupted system call */
#define EIO 5		/* I/O error */
#define ENXIO 6		/* No such device or address */
#define E2BIG 7		/* Argument list too long */
#define ENOEXEC 8	/* Exec format error */
#define EBADF 9		/* Bad file descriptor */
#define ECHILD 10	/* No child processes */
#define EAGAIN 11	/* Resource temporarily unavailable */
#define ENOMEM 12	/* Out of memory */
#define EACCES 13	/* Permission denied */
#define EFAULT 14	/* Bad address */
#define ENOTBLK 15	/* Block device required */
#define EBUSY 16	/* Device or resource busy */
#define EEXIST 17	/* File exists */
#define EXDEV 18	/* Invalid cross-device link */
#define ENODEV 19	/* No such device */
#define ENOTDIR 20	/* Not a directory */
#define EISDIR 21	/* Is a directory */
#define EINVAL 22	/* Invalid argument */
#define ENFILE 23	/* File table overflow */
#define EMFILE 24	/* Too many open files */
#define ENOTTY 25	/* Inappropriate ioctl for device */
#define ETXTBSY 26	/* Text file busy */
#define EFBIG 27	/* File too large */
#define ENOSPC 28	/* No space left on device */
#define ESPIPE 29	/* Illegal seek */
#define EROFS 30	/* Read-only file system */
#define EMLINK 31	/* Too many links */
#define EPIPE 32	/* Broken pipe */
#define EDOM 33		/* Numerical argument out of domain */
#define ERANGE 34	/* Result too large */
#define EDEADLK 35	/* Resource deadlock avoided */
#define ENAMETOOLONG 36 /* File name too long */
#define ENOLCK 37	/* No locks available */
#define ENOSYS 38	/* Function not implemented */
#define ENOTEMPTY 39	/* Directory not empty */
#define ELOOP 40	/* Too many levels of symbolic links */
/* 41 was EWOULDBLOCK (historical, now = EAGAIN) */
#define ENOMSG 42 /* No message of desired type */
#define EIDRM 43  /* Identifier removed */
/* 44–59 unused/reserved */
#define ECHRNG 44   /* Channel number out of range */
#define EL2NSYNC 45 /* Level 2 not synchronized */
#define EL3HLT 46   /* Level 3 halted */
#define EL3RST 47   /* Level 3 reset */
#define ELNRNG 48   /* Link number out of range */
#define EUNATCH 49  /* Protocol driver not attached */
#define ENOCSI 50   /* No CSI structure available */
#define EL2HLT 51   /* Level 2 halted */
#define EBADE 52    /* Invalid exchange */
#define EBADR 53    /* Invalid request descriptor */
#define EXFULL 54   /* Exchange full */
#define ENOANO 55   /* No anode */
#define EBADRQC 56  /* Invalid request code */
#define EBADSLT 57  /* Invalid slot */
/* 58–59 unused */
#define EDEADLOCK EDEADLK /* Synonym */
#define EBFONT 59	  /* Bad font file format */

#define ENOSTR 60	/* Device not a stream */
#define ENODATA 61	/* No data available */
#define ETIME 62	/* Timer expired */
#define ENOSR 63	/* Out of streams resources */
#define ENONET 64	/* Machine is not on the network */
#define ENOPKG 65	/* Package not installed */
#define EREMOTE 66	/* Object is remote */
#define ENOLINK 67	/* Link has been severed */
#define EADV 68		/* Advertise error */
#define ESRMNT 69	/* Srmount error */
#define ECOMM 70	/* Communication error on send */
#define EPROTO 71	/* Protocol error */
#define EMULTIHOP 72	/* Multihop attempted */
#define EDOTDOT 73	/* RFS specific error */
#define EBADMSG 74	/* Bad message */
#define EOVERFLOW 75	/* Value too large for defined data type */
#define ENOTUNIQ 76	/* Name not unique on network */
#define EBADFD 77	/* File descriptor in bad state */
#define EREMCHG 78	/* Remote address changed */
#define ELIBACC 79	/* Can not access a needed shared lib */
#define ELIBBAD 80	/* Accessing a corrupted shared lib */
#define ELIBSCN 81	/* .lib section in a.out corrupted */
#define ELIBMAX 82	/* Attempting to link in too many shared libraries */
#define ELIBEXEC 83	/* Cannot exec a shared library directly */
#define EILSEQ 84	/* Invalid or incomplete multibyte or wide character */
#define ERESTART 85	/* Interrupted system call should be restarted */
#define ESTRPIPE 86	/* Streams pipe error */
#define EUSERS 87	/* Too many users */
#define ENOTSOCK 88	/* Socket operation on non-socket */
#define EDESTADDRREQ 89 /* Destination address required */
#define EMSGSIZE 90	/* Message too long */
#define EPROTOTYPE 91	/* Protocol wrong type for socket */
#define ENOPROTOOPT 92	/* Protocol not available */
#define EPROTONOSUPPORT 93 /* Protocol not supported */
#define ESOCKTNOSUPPORT 94 /* Socket type not supported */
#define ENOTSUP 95	   /* Operation not supported */
#define EPFNOSUPPORT 96	   /* Protocol family not supported */
#define EAFNOSUPPORT 97	   /* Address family not supported by protocol */
#define EADDRINUSE 98	   /* Address already in use */
#define EADDRNOTAVAIL 99   /* Cannot assign requested address */
#define ENETDOWN 100	   /* Network is down */
#define ENETUNREACH 101	   /* Network is unreachable */
#define ENETRESET 102	   /* Network dropped connection on reset */
#define ECONNABORTED 103   /* Software caused connection abort */
#define ECONNRESET 104	   /* Connection reset by peer */
#define ENOBUFS 105	   /* No buffer space available */
#define EISCONN 106	   /* Transport endpoint is already connected */
#define ENOTCONN 107	   /* Transport endpoint is not connected */
#define ESHUTDOWN 108	   /* Cannot send after transport endpoint shutdown */
#define ETOOMANYREFS 109   /* Too many references: cannot splice */
#define ETIMEDOUT 110	   /* Connection timed out */
#define ECONNREFUSED 111   /* Connection refused */
#define EHOSTDOWN 112	   /* Host is down */
#define EHOSTUNREACH 113   /* No route to host */
#define EALREADY 114	   /* Operation already in progress */
#define EINPROGRESS 115	   /* Operation now in progress */
#define ESTALE 116	   /* Stale file handle */
#define EUCLEAN 117	   /* Structure needs cleaning */
#define ENOTNAM 118	   /* Not a XENIX named type file */
#define ENAVAIL 119	   /* No XENIX semaphores available */
#define EISNAM 120	   /* Is a named type file */
#define EREMOTEIO 121	   /* Remote I/O error */
#define EDQUOT 122	   /* Disk quota exceeded */
#define ENOMEDIUM 123	   /* No medium found */
#define EMEDIUMTYPE 124	   /* Wrong medium type */
#define ECANCELED 125	   /* Operation canceled */
#define ENOKEY 126	   /* Required key not available */
#define EKEYEXPIRED 127	   /* Key has expired */
#define EKEYREVOKED 128	   /* Key has been revoked */
#define EKEYREJECTED 129   /* Key was rejected by service */
#define EWOULDBLOCK EAGAIN /* Operation would block */
#define EDEADLOCK EDEADLK  /* Resource deadlock avoided */

/* Non-standard additional codes */
#define EDUPLICATE 1001 /* Duplicate Entry */
#define ETODO 1002	/* TODO */

#endif /* _ERRNO_H */

#ifdef ERRNO_IMPL
#ifndef ERRNO_IMPL_GUARD
#define ERRNO_IMPL_GUARD

#include <libfam/utils.h>

PUBLIC char *strerror(i32 err_code) {
	switch (err_code) {
		case SUCCESS:
			return "Success";
		case EPERM:
			return "Operation not permitted";
		case ENOENT:
			return "No such file or directory";
		case ESRCH:
			return "No such process";
		case EINTR:
			return "Interrupted system call";
		case EIO:
			return "I/O error";
		case ENXIO:
			return "No such device or address";
		case E2BIG:
			return "Argument list too long";
		case ENOEXEC:
			return "Exec format error";
		case EBADF:
			return "Bad file descriptor";
		case ECHILD:
			return "No child processes";
		case EAGAIN:
			return "Resource temporarily unavailable";
		case ENOMEM:
			return "Cannot allocate memory";
		case EACCES:
			return "Permission denied";
		case EFAULT:
			return "Bad address";
		case ENOTBLK:
			return "Block device required";
		case EBUSY:
			return "Device or resource busy";
		case EEXIST:
			return "File exists";
		case EXDEV:
			return "Invalid cross-device link";
		case ENODEV:
			return "No such device";
		case ENOTDIR:
			return "Not a directory";
		case EISDIR:
			return "Is a directory";
		case EINVAL:
			return "Invalid argument";
		case ENFILE:
			return "Too many open files in system";
		case EMFILE:
			return "Too many open files";
		case ENOTTY:
			return "Not a typewriter";
		case ETXTBSY:
			return "Text file busy";
		case EFBIG:
			return "File too large";
		case ENOSPC:
			return "No space left on device";
		case ESPIPE:
			return "Illegal seek";
		case EROFS:
			return "Read-only file system";
		case EMLINK:
			return "Too many links";
		case EPIPE:
			return "Broken pipe";
		case EDOM:
			return "Math argument out of domain of func";
		case ERANGE:
			return "Math result not representable";
		case EDEADLK:
			return "Resource deadlock would occur";
		case ENAMETOOLONG:
			return "File name too long";
		case ENOLCK:
			return "No record locks available";
		case ENOSYS:
			return "Function not implemented";
		case ENOTEMPTY:
			return "Directory not empty";
		case ELOOP:
			return "Too many symbolic links encountered";
		case ENOMSG:
			return "No message of desired type";
		case EIDRM:
			return "Identifier removed";
		case ECHRNG:
			return "Channel number out of range";
		case EL2NSYNC:
			return "Level 2 not synchronized";
		case EL3HLT:
			return "Level 3 halted";
		case EL3RST:
			return "Level 3 reset";
		case ELNRNG:
			return "Link number out of range";
		case EUNATCH:
			return "Protocol driver not attached";
		case ENOCSI:
			return "No CSI structure available";
		case EL2HLT:
			return "Level 2 halted";
		case EBADE:
			return "Invalid exchange";
		case EBADR:
			return "Invalid request descriptor";
		case EXFULL:
			return "Exchange full";
		case ENOANO:
			return "No anode";
		case EBADRQC:
			return "Invalid request code";
		case EBADSLT:
			return "Invalid slot";
		case EBFONT:
			return "Bad font file format";
		case ENOSTR:
			return "Device not a stream";
		case ENODATA:
			return "No data available";
		case ETIME:
			return "Timer expired";
		case ENOSR:
			return "Out of streams resources";
		case ENONET:
			return "Machine is not on the network";
		case ENOPKG:
			return "Package not installed";
		case EREMOTE:
			return "Object is remote";
		case ENOLINK:
			return "Link has been severed";
		case EADV:
			return "Advertise error";
		case ESRMNT:
			return "Srmount error";
		case ECOMM:
			return "Communication error on send";
		case EPROTO:
			return "Protocol error";
		case EMULTIHOP:
			return "Multihop attempted";
		case EDOTDOT:
			return "RFS specific error";
		case EBADMSG:
			return "Not a data message";
		case EOVERFLOW:
			return "Value too large for defined data type";
		case ENOTUNIQ:
			return "Name not unique on network";
		case EBADFD:
			return "File descriptor in bad state";
		case EREMCHG:
			return "Remote address changed";
		case ELIBACC:
			return "Can not access a needed shared library";
		case ELIBBAD:
			return "Accessing a corrupted shared library";
		case ELIBSCN:
			return ".lib section in a.out corrupted";
		case ELIBMAX:
			return "Attempting to link in too many shared "
			       "libraries";
		case ELIBEXEC:
			return "Cannot exec a shared library directly";
		case EILSEQ:
			return "Illegal byte sequence";
		case ERESTART:
			return "Interrupted system call should be restarted";
		case ESTRPIPE:
			return "Streams pipe error";
		case EUSERS:
			return "Too many users";
		case ENOTSOCK:
			return "Socket operation on non-socket";
		case EDESTADDRREQ:
			return "Destination address required";
		case EMSGSIZE:
			return "Message too long";
		case EPROTOTYPE:
			return "Protocol wrong type for socket";
		case ENOPROTOOPT:
			return "Protocol not available";
		case EPROTONOSUPPORT:
			return "Protocol not supported";
		case ESOCKTNOSUPPORT:
			return "Socket type not supported";
		case ENOTSUP:
			return "Operation not supported";
		case EPFNOSUPPORT:
			return "Protocol family not supported";
		case EAFNOSUPPORT:
			return "Address family not supported by protocol";
		case EADDRINUSE:
			return "Address already in use";
		case EADDRNOTAVAIL:
			return "Cannot assign requested address";
		case ENETDOWN:
			return "Network is down";
		case ENETUNREACH:
			return "Network is unreachable";
		case ENETRESET:
			return "Network dropped connection because of reset";
		case ECONNABORTED:
			return "Software caused connection abort";
		case ECONNRESET:
			return "Connection reset by peer";
		case ENOBUFS:
			return "No buffer space available";
		case EISCONN:
			return "Transport endpoint is already connected";
		case ENOTCONN:
			return "Transport endpoint is not connected";
		case ESHUTDOWN:
			return "Cannot send after transport endpoint shutdown";
		case ETOOMANYREFS:
			return "Too many references: cannot splice";
		case ETIMEDOUT:
			return "Connection timed out";
		case ECONNREFUSED:
			return "Connection refused";
		case EHOSTDOWN:
			return "Host is down";
		case EHOSTUNREACH:
			return "No route to host";
		case EALREADY:
			return "Operation already in progress";
		case EINPROGRESS:
			return "Operation now in progress";
		case ESTALE:
			return "Stale file handle";
		case EUCLEAN:
			return "Structure needs cleaning";
		case ENOTNAM:
			return "Not a XENIX named type file";
		case ENAVAIL:
			return "No XENIX semaphores available";
		case EISNAM:
			return "Is a named type file";
		case EREMOTEIO:
			return "Remote I/O error";
		case EDQUOT:
			return "Quota exceeded";
		case ENOMEDIUM:
			return "No medium found";
		case EMEDIUMTYPE:
			return "Wrong medium type";
		case ECANCELED:
			return "Operation Canceled";
		case ENOKEY:
			return "Required key not available";
		case EKEYEXPIRED:
			return "Key has expired";
		case EKEYREVOKED:
			return "Key has been revoked";
		case EKEYREJECTED:
			return "Key was rejected by service";
		case EDUPLICATE:
			return "Duplicate entry";
		case ETODO:
			return "Feature not implemented";
		default:
			return "Unknown error";
	}
}

#ifdef TEST
#include <libfam/test.h>
Test(strerror) {
	ASSERT(!strcmp(strerror(SUCCESS), "Success"), "SUCCESS → Success");
	ASSERT(!strcmp(strerror(EPERM), "Operation not permitted"),
	       "EPERM → Operation not permitted");
}
#endif /* TEST */

#endif /* ERRNO_IMPL_GUARD */
#endif /* ERRNO_IMPL */
