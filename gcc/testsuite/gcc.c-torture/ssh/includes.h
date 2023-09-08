/*	$OpenBSD: includes.h,v 1.15 2001/06/08 15:25:40 markus Exp $	*/

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This file includes most of the needed system headers.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#ifndef INCLUDES_H
#define INCLUDES_H

#include "tops20.h"

#define RCSID(msg) \
static /**/const char *const rcsid[] = { (char *)rcsid, "\100(#)" msg }

/* Formerly in config.h */

#ifdef TOPS_HEADERS
   #undef vax
   #undef i386
   #undef mc68000
   #undef sparc
   
   #define _KCCtype_char6  char
   #define _KCCtype_char7  char
   #define _KCCtype_char8  char
#endif

/* Formerly in defines.h */          
          
/* Necessary headers */

#include "sys/types.h" /* For [u]intxx_t */
#include "sys/socket.h" /* For SHUT_XXXX */
#include "sys/param.h" /* For MAXPATHLEN and roundup() */
#include "netinet/in_systm.h" /* For typedefs */
#include "netinet/in.h" /* For IPv6 macros */
#include "netinet/ip.h" /* For IPTOS macros */
#include "sys/un.h" /* For sockaddr_un */
#ifndef TOPS_HEADERS
# include "sys/bitypes.h" /* For u_intXX_t */
#endif
#include "sys/time.h" /* For timersub */
#include "sys/cdefs.h" /* For __P() */
#include "sys/stat.h" /* For S_* constants and macros */

#include "unistd.h" /* For STDIN_FILENO, etc */
#ifndef TOPS_HEADERS
#include "termios.h" /* Struct winsize */
#endif
#include "fcntl.h" /* For O_NONBLOCK */
#include "openssl/opensslv.h" /* For OPENSSL_VERSION_NUMBER */

/* *-*-nto-qnx needs these headers for strcasecmp and LASTLOG_FILE respectively */
#include "string.h"

#ifdef TOPS_HEADERS
typedef unsigned long uint32_t;
typedef unsigned int int32_t;
typedef UNS8 u_int8_t;
typedef unsigned short u_int16_t;
typedef uint32_t u_int32_t;
typedef int socklen_t;

enum
{
  SHUT_RD = 0,		/* No more receptions.  */
  SHUT_WR,			/* No more transmissions.  */
  SHUT_RDWR			/* No more receptions or transmissions.  */
};
#define SHUT_RD   SHUT_RD
#define SHUT_WR   SHUT_WR
#define SHUT_RDWR SHUT_RDWR

#define O_NONBLOCK      00004 /* Non Blocking Open */

typedef struct	tagSAddr {
	short	sin_family;
	short	sin_port;
	struct {
		long s_addr;
		}	sin_addr;
	long	junk[2];
}	SAddr, *SAddrP;

struct	sockaddr_storage {
	short	ss_family;
	short	ss_port;
	struct {
		long s_addr;
		}	ss_addr;
	long	junk[2];
};

struct addrinfo {
	int		ai_flags;      /* AI_PASSIVE, AI_CANONNAME */
	int		ai_family;     /* PF_xxx */
	int		ai_socktype;   /* SOCK_xxx */
	int		ai_protocol;   /* 0 or IPPROTO_xxx for IPv4 and IPv6 */
	size_t		ai_addrlen;    /* Length of ai_addr */
	char		*ai_canonname; /* Canonical name for hostname */
	struct sockaddr	*ai_addr;      /* Binary address */
	struct addrinfo	*ai_next;      /* Next structure in linked list */
};

/* netdb.h functions which may need to be written from scratch or removed */

/* Translate name of a service location and/or a service name to set of
   socket addresses.  */
extern int getaddrinfo (const char * __name,
			const char * __service,
			const struct addrinfo * __req,
			struct addrinfo ** __pai);

/* Free `addrinfo' structure AI including associated storage.  */
extern VOID freeaddrinfo (struct addrinfo *__ai);

/* Convert error return from getaddrinfo() to a string.  */
extern const char *gai_strerror (int __ecode);

/* Translate a socket address to a location and service name.  */
extern int getnameinfo (const struct sockaddr * __sa,
			socklen_t __salen, char * __host,
			socklen_t __hostlen, char * __serv,
			socklen_t __servlen, unsigned int __flags);

#define NI_NUMERICHOST  0x00000002
#define NI_NUMERICSERV  0x00000008
#define NI_MAXHOST 1025
#define NI_MAXSERV 32

#define AI_PASSIVE	0x0001	/* Socket address is intended for `bind'.  */
#define AI_CANONNAME	0x0002	/* Request for canonical name.  */
#define AI_NUMERICHOST	0x0004	/* Don't use name resolution.  */

#define TCP_NODELAY	0x01	/* don't delay send to coalesce packets */

#define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))
#define __attribute__(x)

#endif

/* Function replacement / compatibility hacks */

/* OPENSSL_free() is Free() in versions before OpenSSL 0.9.6 */
#if !defined(OPENSSL_VERSION_NUMBER) || (OPENSSL_VERSION_NUMBER < 0x0090600f)
# define OPENSSL_free(x) Free(x)
#endif

/* Original includes contents */

//#include "sys/resource.h"

#include "netinet/tcp.h"
#include "arpa/inet.h"

#include "stdio.h"
#include "ctype.h"
#include "errno.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "time.h"

#include "netdb.h"

#include "utime.h"
#include "version.h"
#include "entropy.h"

#endif /* INCLUDES_H */
