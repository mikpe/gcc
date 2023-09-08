/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Definitions for server configuration data and for the functions reading it.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

/* RCSID("$OpenBSD: servconf.h,v 1.49 2001/08/17 18:59:47 stevesk Exp $"); */

#ifndef SERVCONF_H
#define SERVCONF_H

#define MAX_PORTS		256	/* Max # ports. */

#define MAX_ALLOW_USERS		256	/* Max # users on allow list. */
#define MAX_DENY_USERS		256	/* Max # users on deny list. */
#define MAX_ALLOW_GROUPS	256	/* Max # groups on allow list. */
#define MAX_DENY_GROUPS		256	/* Max # groups on deny list. */
#define MAX_SUBSYSTEMS		256	/* Max # subsystems. */
#define MAX_HOSTKEYS		256	/* Max # hostkeys. */

/* permit_root_login */
#define	PERMIT_NOT_SET		-1
#define	PERMIT_NO		0
#define	PERMIT_FORCED_ONLY	1
#define	PERMIT_NO_PASSWD	2
#define	PERMIT_YES		3


typedef struct {
	u_int num_ports;
	u_short ports[MAX_PORTS];	/* Port number to listen on. */
	struct addrinfo *listen_addrs;	/* Addresses on which the server listens. */
	char   *host_key_files[MAX_HOSTKEYS];	/* Files containing host keys. */
	int     num_host_key_files;     /* Number of files for host keys. */
	int     login_grace_time;	/* Disconnect if no auth in this time
					 * (sec). */
	int     permit_root_login;	/* PERMIT_*, see above */
	int     keepalives;	/* If true, set SO_KEEPALIVE. */
	char   *ciphers;	/* Supported SSH2 ciphers. */
	char   *macs;		/* Supported SSH2 macs. */
	SyslogFacility log_facility;	/* Facility for system logging. */
	LogLevel log_level;	/* Level for system logging. */
	int     password_authentication;	/* If true, permit password
						 * authentication. */
	int     permit_empty_passwd;	/* If false, do not permit empty
					 * passwords. */
	int     use_login;	/* If true, login(1) is used */

	u_int num_subsystems;
	char   *subsystem_name[MAX_SUBSYSTEMS];
	char   *subsystem_command[MAX_SUBSYSTEMS];

	int	max_startups_begin;
	int	max_startups_rate;
	int	max_startups;
	int	client_alive_interval;	/*
					 * poke the client this often to 
					 * see if it's still there 
					 */
	int	client_alive_count_max;	/*
					 * If the client is unresponsive
					 * for this many intervals above,
					 * disconnect the session 
					 */

   int dummy;
   char* dummy2;
   int dummy3;
   int dummy4;

}       ServerOptions;

VOID	 fill_default_server_options(ServerOptions *);

#endif				/* SERVCONF_H */
