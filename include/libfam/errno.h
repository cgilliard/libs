#ifndef _ERRNO_H
#define _ERRNO_H

#include <libfam/types.h>

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

char *strerror(i32 err_code);

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

/* GCOVR_EXCL_START */
#ifdef TEST
#include <libfam/test.h>
Test(strerror) {
	/*
	ASSERT(!strcmp(strerror(SUCCESS), "Success"), "SUCCESS → Success");
	ASSERT(!strcmp(strerror(EPERM), "Operation not permitted"),
	       "EPERM → Operation not permitted");
	       */
	ASSERT(!strcmp(strerror(SUCCESS), "Success"), "SUCCESS → Success");
	ASSERT(!strcmp(strerror(EPERM), "Operation not permitted"),
	       "EPERM → Operation not permitted");
	ASSERT(!strcmp(strerror(ENOENT), "No such file or directory"),
	       "ENOENT → No such file or directory");
	ASSERT(!strcmp(strerror(ESRCH), "No such process"),
	       "ESRCH → No such process");
	ASSERT(!strcmp(strerror(EINTR), "Interrupted system call"),
	       "EINTR → Interrupted system call");
	ASSERT(!strcmp(strerror(EIO), "I/O error"), "EIO → I/O error");
	ASSERT(!strcmp(strerror(ENXIO), "No such device or address"),
	       "ENXIO → No such device or address");
	ASSERT(!strcmp(strerror(E2BIG), "Argument list too long"),
	       "E2BIG → Argument list too long");
	ASSERT(!strcmp(strerror(ENOEXEC), "Exec format error"),
	       "ENOEXEC → Exec format error");
	ASSERT(!strcmp(strerror(EBADF), "Bad file descriptor"),
	       "EBADF → Bad file descriptor");
	ASSERT(!strcmp(strerror(ECHILD), "No child processes"),
	       "ECHILD → No child processes");
	ASSERT(!strcmp(strerror(EAGAIN), "Resource temporarily unavailable"),
	       "EAGAIN → Resource temporarily unavailable");
	ASSERT(!strcmp(strerror(ENOMEM), "Cannot allocate memory"),
	       "ENOMEM → Cannot allocate memory");
	ASSERT(!strcmp(strerror(EACCES), "Permission denied"),
	       "EACCES → Permission denied");
	ASSERT(!strcmp(strerror(EFAULT), "Bad address"),
	       "EFAULT → Bad address");
	ASSERT(!strcmp(strerror(ENOTBLK), "Block device required"),
	       "ENOTBLK → Block device required");
	ASSERT(!strcmp(strerror(EBUSY), "Device or resource busy"),
	       "EBUSY → Device or resource busy");
	ASSERT(!strcmp(strerror(EEXIST), "File exists"),
	       "EEXIST → File exists");
	ASSERT(!strcmp(strerror(EXDEV), "Invalid cross-device link"),
	       "EXDEV → Invalid cross-device link");
	ASSERT(!strcmp(strerror(ENODEV), "No such device"),
	       "ENODEV → No such device");
	ASSERT(!strcmp(strerror(ENOTDIR), "Not a directory"),
	       "ENOTDIR → Not a directory");
	ASSERT(!strcmp(strerror(EISDIR), "Is a directory"),
	       "EISDIR → Is a directory");
	ASSERT(!strcmp(strerror(EINVAL), "Invalid argument"),
	       "EINVAL → Invalid argument");
	ASSERT(!strcmp(strerror(ENFILE), "Too many open files in system"),
	       "ENFILE → Too many open files in system");
	ASSERT(!strcmp(strerror(EMFILE), "Too many open files"),
	       "EMFILE → Too many open files");
	ASSERT(!strcmp(strerror(ENOTTY), "Not a typewriter"),
	       "ENOTTY → Not a typewriter");
	ASSERT(!strcmp(strerror(ETXTBSY), "Text file busy"),
	       "ETXTBSY → Text file busy");
	ASSERT(!strcmp(strerror(EFBIG), "File too large"),
	       "EFBIG → File too large");
	ASSERT(!strcmp(strerror(ENOSPC), "No space left on device"),
	       "ENOSPC → No space left on device");
	ASSERT(!strcmp(strerror(ESPIPE), "Illegal seek"),
	       "ESPIPE → Illegal seek");
	ASSERT(!strcmp(strerror(EROFS), "Read-only file system"),
	       "EROFS → Read-only file system");
	ASSERT(!strcmp(strerror(EMLINK), "Too many links"),
	       "EMLINK → Too many links");
	ASSERT(!strcmp(strerror(EPIPE), "Broken pipe"), "EPIPE → Broken pipe");
	ASSERT(!strcmp(strerror(EDOM), "Math argument out of domain of func"),
	       "EDOM → Math argument out of domain of func");
	ASSERT(!strcmp(strerror(ERANGE), "Math result not representable"),
	       "ERANGE → Math result not representable");
	ASSERT(!strcmp(strerror(EDEADLK), "Resource deadlock would occur"),
	       "EDEADLK → Resource deadlock would occur");
	ASSERT(!strcmp(strerror(ENAMETOOLONG), "File name too long"),
	       "ENAMETOOLONG → File name too long");
	ASSERT(!strcmp(strerror(ENOLCK), "No record locks available"),
	       "ENOLCK → No record locks available");
	ASSERT(!strcmp(strerror(ENOSYS), "Function not implemented"),
	       "ENOSYS → Function not implemented");
	ASSERT(!strcmp(strerror(ENOTEMPTY), "Directory not empty"),
	       "ENOTEMPTY → Directory not empty");
	ASSERT(!strcmp(strerror(ELOOP), "Too many symbolic links encountered"),
	       "ELOOP → Too many symbolic links encountered");

	ASSERT(!strcmp(strerror(ENOTSOCK), "Socket operation on non-socket"),
	       "ENOTSOCK → Socket operation on non-socket");
	ASSERT(!strcmp(strerror(EDESTADDRREQ), "Destination address required"),
	       "EDESTADDRREQ → Destination address required");
	ASSERT(!strcmp(strerror(EMSGSIZE), "Message too long"),
	       "EMSGSIZE → Message too long");
	ASSERT(!strcmp(strerror(EPROTOTYPE), "Protocol wrong type for socket"),
	       "EPROTOTYPE → Protocol wrong type for socket");
	ASSERT(!strcmp(strerror(ENOPROTOOPT), "Protocol not available"),
	       "ENOPROTOOPT → Protocol not available");
	ASSERT(!strcmp(strerror(EPROTONOSUPPORT), "Protocol not supported"),
	       "EPROTONOSUPPORT → Protocol not supported");
	ASSERT(!strcmp(strerror(ESOCKTNOSUPPORT), "Socket type not supported"),
	       "ESOCKTNOSUPPORT → Socket type not supported");
	ASSERT(!strcmp(strerror(ENOTSUP), "Operation not supported"),
	       "ENOTSUP → Operation not supported");
	ASSERT(!strcmp(strerror(EAFNOSUPPORT),
		       "Address family not supported by protocol"),
	       "EAFNOSUPPORT → Address family not supported by protocol");
	ASSERT(!strcmp(strerror(EADDRINUSE), "Address already in use"),
	       "EADDRINUSE → Address already in use");
	ASSERT(
	    !strcmp(strerror(EADDRNOTAVAIL), "Cannot assign requested address"),
	    "EADDRNOTAVAIL → Cannot assign requested address");
	ASSERT(!strcmp(strerror(ENETDOWN), "Network is down"),
	       "ENETDOWN → Network is down");
	ASSERT(!strcmp(strerror(ENETUNREACH), "Network is unreachable"),
	       "ENETUNREACH → Network is unreachable");
	ASSERT(
	    !strcmp(strerror(ECONNABORTED), "Software caused connection abort"),
	    "ECONNABORTED → Software caused connection abort");
	ASSERT(!strcmp(strerror(ECONNRESET), "Connection reset by peer"),
	       "ECONNRESET → Connection reset by peer");
	ASSERT(!strcmp(strerror(ENOBUFS), "No buffer space available"),
	       "ENOBUFS → No buffer space available");
	ASSERT(!strcmp(strerror(EISCONN),
		       "Transport endpoint is already connected"),
	       "EISCONN → Transport endpoint is already connected");
	ASSERT(
	    !strcmp(strerror(ENOTCONN), "Transport endpoint is not connected"),
	    "ENOTCONN → Transport endpoint is not connected");
	ASSERT(!strcmp(strerror(ESHUTDOWN),
		       "Cannot send after transport endpoint shutdown"),
	       "ESHUTDOWN → Cannot send after transport endpoint shutdown");
	ASSERT(!strcmp(strerror(ETIMEDOUT), "Connection timed out"),
	       "ETIMEDOUT → Connection timed out");
	ASSERT(!strcmp(strerror(ECONNREFUSED), "Connection refused"),
	       "ECONNREFUSED → Connection refused");
	ASSERT(!strcmp(strerror(EHOSTDOWN), "Host is down"),
	       "EHOSTDOWN → Host is down");
	ASSERT(!strcmp(strerror(EHOSTUNREACH), "No route to host"),
	       "EHOSTUNREACH → No route to host");
	ASSERT(!strcmp(strerror(EALREADY), "Operation already in progress"),
	       "EALREADY → Operation already in progress");
	ASSERT(!strcmp(strerror(EINPROGRESS), "Operation now in progress"),
	       "EINPROGRESS → Operation now in progress");
	ASSERT(!strcmp(strerror(EOVERFLOW),
		       "Value too large for defined data type"),
	       "EOVERFLOW → Value too large for defined data type");
	ASSERT(!strcmp(strerror(ECANCELED), "Operation Canceled"),
	       "ECANCELED → Operation Canceled");

	ASSERT(
	    !strcmp(strerror(EWOULDBLOCK), "Resource temporarily unavailable"),
	    "EWOULDBLOCK → EAGAIN");
	ASSERT(!strcmp(strerror(EDEADLOCK), "Resource deadlock would occur"),
	       "EDEADLOCK → EDEADLK");

	ASSERT(!strcmp(strerror(EDUPLICATE), "Duplicate entry"),
	       "EDUPLICATE → Duplicate entry");
	ASSERT(!strcmp(strerror(ETODO), "Feature not implemented"),
	       "ETODO → Feature not implemented");

	ASSERT(!strcmp(strerror(-1337), "Unknown error"),
	       "Negative unknown → Unknown error");
	ASSERT(!strcmp(strerror(99999), "Unknown error"),
	       "Large unknown → Unknown error");

	ASSERT(!strcmp(strerror(ENOMSG), "No message of desired type"),
	       "ENOMSG coverage");
	ASSERT(!strcmp(strerror(EIDRM), "Identifier removed"),
	       "EIDRM coverage");
	ASSERT(!strcmp(strerror(ECHRNG), "Channel number out of range"),
	       "ECHRNG coverage");
	ASSERT(!strcmp(strerror(EL2NSYNC), "Level 2 not synchronized"),
	       "EL2NSYNC coverage");
	ASSERT(!strcmp(strerror(EL3HLT), "Level 3 halted"), "EL3HLT coverage");
	ASSERT(!strcmp(strerror(EL3RST), "Level 3 reset"), "EL3RST coverage");
	ASSERT(!strcmp(strerror(ELNRNG), "Link number out of range"),
	       "ELNRNG coverage");
	ASSERT(!strcmp(strerror(EUNATCH), "Protocol driver not attached"),
	       "EUNATCH coverage");
	ASSERT(!strcmp(strerror(ENOCSI), "No CSI structure available"),
	       "ENOCSI coverage");
	ASSERT(!strcmp(strerror(EL2HLT), "Level 2 halted"), "EL2HLT coverage");
	ASSERT(!strcmp(strerror(EBADE), "Invalid exchange"), "EBADE coverage");
	ASSERT(!strcmp(strerror(EBADR), "Invalid request descriptor"),
	       "EBADR coverage");
	ASSERT(!strcmp(strerror(EXFULL), "Exchange full"), "EXFULL coverage");
	ASSERT(!strcmp(strerror(ENOANO), "No anode"), "ENOANO coverage");
	ASSERT(!strcmp(strerror(EBADRQC), "Invalid request code"),
	       "EBADRQC coverage");
	ASSERT(!strcmp(strerror(EBADSLT), "Invalid slot"), "EBADSLT coverage");
	ASSERT(!strcmp(strerror(EBFONT), "Bad font file format"),
	       "EBFONT coverage");
	ASSERT(!strcmp(strerror(ENOSTR), "Device not a stream"),
	       "ENOSTR coverage");
	ASSERT(!strcmp(strerror(ENODATA), "No data available"),
	       "ENODATA coverage");
	ASSERT(!strcmp(strerror(ETIME), "Timer expired"), "ETIME coverage");
	ASSERT(!strcmp(strerror(ENOSR), "Out of streams resources"),
	       "ENOSR coverage");
	ASSERT(!strcmp(strerror(ENONET), "Machine is not on the network"),
	       "ENONET coverage");
	ASSERT(!strcmp(strerror(ENOPKG), "Package not installed"),
	       "ENOPKG coverage");
	ASSERT(!strcmp(strerror(EREMOTE), "Object is remote"),
	       "EREMOTE coverage");
	ASSERT(!strcmp(strerror(ENOLINK), "Link has been severed"),
	       "ENOLINK coverage");
	ASSERT(!strcmp(strerror(EADV), "Advertise error"), "EADV coverage");
	ASSERT(!strcmp(strerror(ESRMNT), "Srmount error"), "ESRMNT coverage");
	ASSERT(!strcmp(strerror(ECOMM), "Communication error on send"),
	       "ECOMM coverage");
	ASSERT(!strcmp(strerror(EPROTO), "Protocol error"), "EPROTO coverage");
	ASSERT(!strcmp(strerror(EMULTIHOP), "Multihop attempted"),
	       "EMULTIHOP coverage");
	ASSERT(!strcmp(strerror(EDOTDOT), "RFS specific error"),
	       "EDOTDOT coverage");
	ASSERT(!strcmp(strerror(EBADMSG), "Not a data message"),
	       "EBADMSG coverage");
	ASSERT(!strcmp(strerror(ENOTUNIQ), "Name not unique on network"),
	       "ENOTUNIQ coverage");
	ASSERT(!strcmp(strerror(EBADFD), "File descriptor in bad state"),
	       "EBADFD coverage");
	ASSERT(!strcmp(strerror(EREMCHG), "Remote address changed"),
	       "EREMCHG coverage");
	ASSERT(!strcmp(strerror(ELIBACC),
		       "Can not access a needed shared library"),
	       "ELIBACC coverage");
	ASSERT(
	    !strcmp(strerror(ELIBBAD), "Accessing a corrupted shared library"),
	    "ELIBBAD coverage");
	ASSERT(!strcmp(strerror(ELIBSCN), ".lib section in a.out corrupted"),
	       "ELIBSCN coverage");
	ASSERT(!strcmp(strerror(ELIBMAX),
		       "Attempting to link in too many shared libraries"),
	       "ELIBMAX coverage");
	ASSERT(!strcmp(strerror(ELIBEXEC),
		       "Cannot exec a shared library directly"),
	       "ELIBEXEC coverage");
	ASSERT(!strcmp(strerror(EILSEQ), "Illegal byte sequence"),
	       "EILSEQ coverage");
	ASSERT(!strcmp(strerror(ERESTART),
		       "Interrupted system call should be restarted"),
	       "ERESTART coverage");
	ASSERT(!strcmp(strerror(ESTRPIPE), "Streams pipe error"),
	       "ESTRPIPE coverage");
	ASSERT(!strcmp(strerror(EUSERS), "Too many users"), "EUSERS coverage");

	ASSERT(!strcmp(strerror(ENETRESET),
		       "Network dropped connection because of reset"),
	       "ENETRESET coverage");
	ASSERT(!strcmp(strerror(ESTALE), "Stale file handle"),
	       "ESTALE coverage");
	ASSERT(!strcmp(strerror(EUCLEAN), "Structure needs cleaning"),
	       "EUCLEAN coverage");
	ASSERT(!strcmp(strerror(ENOTNAM), "Not a XENIX named type file"),
	       "ENOTNAM coverage");
	ASSERT(!strcmp(strerror(ENAVAIL), "No XENIX semaphores available"),
	       "ENAVAIL coverage");
	ASSERT(!strcmp(strerror(EISNAM), "Is a named type file"),
	       "EISNAM coverage");
	ASSERT(!strcmp(strerror(EREMOTEIO), "Remote I/O error"),
	       "EREMOTEIO coverage");
	ASSERT(!strcmp(strerror(EDQUOT), "Quota exceeded"), "EDQUOT coverage");
	ASSERT(!strcmp(strerror(ENOMEDIUM), "No medium found"),
	       "ENOMEDIUM coverage");
	ASSERT(!strcmp(strerror(EMEDIUMTYPE), "Wrong medium type"),
	       "EMEDIUMTYPE coverage");
	ASSERT(!strcmp(strerror(ENOKEY), "Required key not available"),
	       "ENOKEY coverage");
	ASSERT(!strcmp(strerror(EKEYEXPIRED), "Key has expired"),
	       "EKEYEXPIRED coverage");
	ASSERT(!strcmp(strerror(EKEYREVOKED), "Key has been revoked"),
	       "EKEYREVOKED coverage");
	ASSERT(!strcmp(strerror(EKEYREJECTED), "Key was rejected by service"),
	       "EKEYREJECTED coverage");

	ASSERT(!strcmp(strerror(EPFNOSUPPORT), "Protocol family not supported"),
	       "EPFNOSUPPORT coverage");
	ASSERT(!strcmp(strerror(ETOOMANYREFS),
		       "Too many references: cannot splice"),
	       "Too many references: cannot splice");
}
#endif /* TEST */
/* GCOVR_EXCL_STOP */

#endif /* ERRNO_IMPL_GUARD */
#endif /* ERRNO_IMPL */
