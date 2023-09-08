/*
 * Copyright (c) 2001 Markus Friedl.  All rights reserved.
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
RCSID("$OpenBSD: mac.c,v 1.2 2001/04/05 10:42:51 markus Exp $");

#include "openssl/hmac.h"

#include "xmalloc.h"
#include "getput.h"
#include "log.h"
#include "cipher.h"
#include "kex.h"
#include "mac.h"

struct {
	char		*name;
	int		truncatebits;	/* truncate digest if != 0 */
} macs[] = {
	{ "hmac-sha1",	   0 },
	{ NULL,				0 }
};

#if 0
struct {
	char		*name;
	EVP_MD *	(*mdfunc)(VOID);
	int		truncatebits;	/* truncate digest if != 0 */
} macs[] = {
	{ "hmac-sha1",			EVP_sha1, 0, },
	{ NULL,				NULL, 0 }
};
#endif

static EVP_MD sha1_md=
	{
	NID_sha1,
	NID_sha1WithRSAEncryption,
	SHA_DIGEST_LENGTH,
	SHA1_Init,
	SHA1_Update,
	SHA1_Final,
	EVP_PKEY_NULL_method,
	SHA_CBLOCK,
	sizeof(EVP_MD *)+sizeof(SHA_CTX),
	};

int
mac_init(Mac *mac, char *name)
{
	int i;
	for (i = 0; macs[i].name; i++) {
		if (strcmp(name, macs[i].name) == 0) {
			if (mac != NULL) {
				mac->md = &sha1_md;
				mac->key_len = mac->mac_len = mac->md->md_size;
				if (macs[i].truncatebits != 0)
					mac->mac_len = macs[i].truncatebits/8;
			}
			debug2("mac_init: found %s", name);
			return (0);
		}
	}
	debug2("mac_init: unknown %s", name);
	return (-1);
}

UNS8 *
mac_compute(Mac *mac, u_int32_t seqno, UNS8 *data, int datalen)
{
	HMAC_CTX c;
	static UNS8 m[EVP_MAX_MD_SIZE];
	UNS8 b[4];

	if (mac->key == NULL)
		fatal("mac_compute: no key");
	if (mac->mac_len > sizeof(m))
		fatal("mac_compute: mac too long");
	HMAC_Init(&c, mac->key, mac->key_len, mac->md);
	PUT_32BIT(b, seqno);
	HMAC_Update(&c, b, sizeof(b));
	HMAC_Update(&c, data, datalen);
	HMAC_Final(&c, m);
	HMAC_cleanup(&c);
	return (m);
}

