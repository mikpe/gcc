/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Functions to interface with the SSH_AUTHENTICATION_FD socket.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

/* $OpenBSD: auth-options.h,v 1.10 2001/06/26 17:27:22 markus Exp $ */

#ifndef AUTH_OPTIONS_H
#define AUTH_OPTIONS_H

/* Linked list of custom environment strings */
struct envstring {
	struct envstring *next;
	char   *s;
};

#endif
