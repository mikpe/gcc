/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

/* RCSID("$OpenBSD: bufaux.h,v 1.13 2001/06/26 17:27:22 markus Exp $"); */

#ifndef BUFAUX_H
#define BUFAUX_H

#include "buffer.h"
#include "openssl/bn.h"

int	buffer_get_bignum2(Buffer *, BIGNUM *);
VOID    buffer_put_bignum2(Buffer *, BIGNUM *);

u_int buffer_get_int(Buffer *);
VOID    buffer_put_int(Buffer *, u_int);

int     buffer_get_char(Buffer *);

VOID    buffer_put_char(Buffer *, int);

#ifdef TOPS_PORT
UNS8   *buffer_get_string(Buffer *, u_int *);
#else
char   *buffer_get_string(Buffer *, u_int *);
#endif

/* JOHN BORCHEK FIX: watch out for VOID pointer */
VOID    buffer_put_string(Buffer *, const UNS8 *, u_int);
#ifdef TOPS_PORT
VOID	buffer_put_cstring(Buffer *, const UNS8 *);
#else
VOID	buffer_put_cstring(Buffer *, const char *);
#endif

#endif				/* BUFAUX_H */
