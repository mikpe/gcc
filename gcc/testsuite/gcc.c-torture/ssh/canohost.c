/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Functions for returning the canonical host name of the remote site.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#include "includes.h"
RCSID("$OpenBSD: canohost.c,v 1.27 2001/06/23 15:12:17 itojun Exp $");

#include "packet.h"
#include "xmalloc.h"
#include "log.h"
#include "canohost.h"

int atoi(const char*);

 /*
 * Returns the remote IP-address of socket as a string.  The returned
 * string must be freed.
 */
static char *
get_socket_address(int socket, int remote, int flags)
{
#if TOPS20
	  return xstrdup("10.1.0.19");
#else
	struct sockaddr_storage addr;
	socklen_t addrlen;
	char ntop[NI_MAXHOST];

	/* Get IP address of client. */
	addrlen = sizeof(addr);
	memset(&addr, 0, sizeof(addr));

	if (remote) {
	  if (getpeername(socket, (struct sockaddr *)&addr, &addrlen)
	      < 0) {
	    debug("get_socket_ipaddr: getpeername failed: %.100s",
		  strerror(errno));
	    return NULL;
	  }
	} else {
	  debug("get_socket_ipaddr: getsockname failed: %.100s",
		strerror(errno));
	  return NULL;
	}

	/* Get the address in ascii. */
	if (getnameinfo((struct sockaddr *)&addr, addrlen, ntop, sizeof(ntop),
	     NULL, 0, flags) != 0) {
		error("get_socket_ipaddr: getnameinfo %d failed", flags);
		return NULL;
	}
	return xstrdup(ntop);
#endif
}

char *
get_peer_ipaddr(int socket)
{
	return get_socket_address(socket, 1, NI_NUMERICHOST);
}

/*
 * Returns the IP-address of the remote host as a string.  The returned
 * string must not be freed.
 */

const char *
get_remote_ipaddr()
{
#if 0
	static char *canonical_host_ip = NULL;

	/* Check whether we have cached the ipaddr. */
	if (canonical_host_ip == NULL) {
	  canonical_host_ip = get_peer_ipaddr(packet_get_connection_in());
	  if (canonical_host_ip == NULL)
	    fatal_cleanup();
	}
#endif
	return xstrdup("UNKNOWN");
}

/* Returns the local/remote port for the socket. */

static int
get_sock_port(int sock, int local)
{
#if TOPS20
  return 1234;
#else
	struct sockaddr_storage from;
	socklen_t fromlen;
	char strport[NI_MAXSERV];

	/* Get IP address of client. */
	fromlen = sizeof(from);
	memset(&from, 0, sizeof(from));
	if (local) {
		if (getsockname(sock, (struct sockaddr *)&from, &fromlen) < 0) {
			error("getsockname failed: %.100s", strerror(errno));
			return 0;
		}
	} else {
		if (getpeername(sock, (struct sockaddr *) & from, &fromlen) < 0) {
			debug("getpeername failed: %.100s", strerror(errno));
			fatal_cleanup();
		}
	}
	/* Return port number. */
	if (getnameinfo((struct sockaddr *)&from, fromlen, NULL, 0,
	     strport, sizeof(strport), NI_NUMERICSERV) != 0)
		fatal("get_sock_port: getnameinfo NI_NUMERICSERV failed");
	return atoi(strport);
#endif
}

/* Returns remote/local port number for the current connection. */

static int
get_port(int local)
{
	/* Get socket and return the port number. */
	return get_sock_port(packet_get_connection_in(), local);
}

int
get_peer_port(int sock)
{
	return get_sock_port(sock, 0);
}

int
get_remote_port()
{
	return get_port(0);
}

