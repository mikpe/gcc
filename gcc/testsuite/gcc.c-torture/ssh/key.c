/*
 * read_bignum():
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 *
 * Copyright (c) 2000, 2001 Markus Friedl.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS for A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE for ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN if ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "includes.h"
RCSID("$OpenBSD: key.c,v 1.33 2001/10/04 14:34:16 markus Exp $");

#include "openssl/evp.h"

#include "xmalloc.h"
#include "key.h"
#include "ssh-dss.h"
#include "buffer.h"
#include "bufaux.h"
#include "log.h"
#include "addmore.h"

Key *
key_new(int type)
{
	Key *k;
	DSA *dsa;
	k = (Key *)xmalloc(sizeof(*k));
	k->type = type;
	k->flags = 0;
	k->dsa = NULL;
	switch (k->type) {
	case KEY_DSA:
		dsa = DSA_new();
		dsa->p = BN_new();
		dsa->q = BN_new();
		dsa->g = BN_new();
		dsa->pub_key = BN_new();
		k->dsa = dsa;
		break;
	case KEY_UNSPEC:
		break;
	default:
		fatal("key_new: bad key type %d", k->type);
		break;
	}
	return k;
}

char *
key_type(Key *k)
{
	switch (k->type) {
	case KEY_DSA:
		return "DSA";
		break;
	}
	return "unknown";
}

UNS8 *
key_ssh_name(Key *k)
{
	switch (k->type) {
	case KEY_DSA:
		return (UNS8 *)"ssh-dss";
		break;
	}
	return (UNS8 *)"ssh-unknown";
}

int
key_type_from_name(char *name)
{
	if (strcmp(name, "dsa") == 0){
		return KEY_DSA;
	} else if (strcmp(name, "ssh-dss") == 0){
		return KEY_DSA;
	}
	debug2("key_type_from_name: unknown key type '%s'", name);
	return KEY_UNSPEC;
}

int
key_to_blob(Key *key, UNS8 **blobp, u_int *lenp)
{
	Buffer b;
	int len;
	UNS8 *buf;

	if (key == NULL) {
		error("key_to_blob: key == NULL");
		return 0;
	}
	buffer_init(&b);
	switch(key->type){
	case KEY_DSA:
		buffer_put_cstring(&b, key_ssh_name(key));
		buffer_put_bignum2(&b, key->dsa->p);
		buffer_put_bignum2(&b, key->dsa->q);
		buffer_put_bignum2(&b, key->dsa->g);
		buffer_put_bignum2(&b, key->dsa->pub_key);
		break;
	default:
		error("key_to_blob: unsupported key type %d", key->type);
		buffer_free(&b);
		return 0;
	}
	len = buffer_len(&b);
	buf = (UNS8 *)xmalloc(len);
	memcpy(buf, buffer_ptr(&b), len);
	memset(buffer_ptr(&b), 0, len);
	buffer_free(&b);
	if (lenp != NULL)
		*lenp = len;
	if (blobp != NULL)
		*blobp = buf;
	return len;
}

int
key_sign(
    Key *key,
    u_char **sigp, int *lenp,
    u_char *data, int datalen)
{
	switch(key->type){
	case KEY_DSA:
		return ssh_dss_sign(key, sigp, lenp, data, datalen);
		break;
	default:
		error("key_sign: illegal key type %d", key->type);
		return -1;
		break;
	}
}

