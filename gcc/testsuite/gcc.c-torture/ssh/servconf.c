/*
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#include "includes.h"
RCSID("$OpenBSD: servconf.c,v 1.91 2001/11/12 18:17:07 markus Exp $");

#include "ssh.h"
#include "log.h"
#include "servconf.h"
#include "xmalloc.h"
#include "compat.h"
#include "pathnames.h"
#include "misc.h"
#include "cipher.h"
#include "kex.h"
#include "mac.h"

static VOID add_listen_addr(ServerOptions *, char *, u_short);
static VOID add_one_listen_addr(ServerOptions *, char *, u_short);

/* AF_UNSPEC or AF_INET or AF_INET6 */
extern int IPv4or6;

/* Initializes the server options to their default values. */

VOID
fill_default_server_options(ServerOptions *options)
{
	memset(options, 0, sizeof(*options));

   options->num_ports = 1;
   options->ports[0] = 5022;
   add_listen_addr(options, NULL, 0);

	options->num_host_key_files = 1;
   options->host_key_files[0] = "key_a";

   options->log_facility = SYSLOG_FACILITY_AUTH;
   options->log_level = SYSLOG_LEVEL_INFO; 
   /* options->log_level = SYSLOG_LEVEL_DEBUG1; */

	/* Standard Options */
	options->login_grace_time = 600;
	options->permit_root_login = PERMIT_YES;
	options->password_authentication = 1;
	options->permit_empty_passwd = 0;
	options->keepalives = 1;
   options->ciphers = NULL;
   options->macs = NULL;
	options->num_subsystems = 0;
	options->max_startups = 10;
	options->max_startups_begin = options->max_startups;
   options->max_startups_rate = 100;		/* 100% */
	options->client_alive_interval = 0;
	options->client_alive_count_max = 3;
}

static VOID
add_listen_addr(ServerOptions *options, char *addr, u_short port)
{
	int i;

	if (port == 0)
		for (i = 0; i < options->num_ports; i++)
			add_one_listen_addr(options, addr, options->ports[i]);
	else
		add_one_listen_addr(options, addr, port);
}

static VOID
add_one_listen_addr(ServerOptions *options, char *addr, u_short port)
{
	struct addrinfo hints, *ai, *aitop;
	char strport[NI_MAXSERV];
	int gaierr;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = IPv4or6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = (addr == NULL) ? AI_PASSIVE : 0;
	sprintf(strport, "%d", port);
	if ((gaierr = getaddrinfo(addr, strport, &hints, &aitop)) != 0)
		fatal("bad addr or host: %s (%s)",
		    addr ? addr : (char*)"<NULL>",
		    gai_strerror(gaierr));
	for (ai = aitop; ai->ai_next; ai = ai->ai_next)
		;
	ai->ai_next = options->listen_addrs;
	options->listen_addrs = aitop;
}
