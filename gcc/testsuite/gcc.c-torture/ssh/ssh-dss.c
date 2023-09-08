/*
 * Copyright (c) 2000 Markus Friedl.  All rights reserved.
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
RCSID("$OpenBSD: ssh-dss.c,v 1.9 2001/11/07 22:10:28 markus Exp $");

#include "openssl/bn.h"
#include "openssl/evp.h"

#include "xmalloc.h"
#include "buffer.h"
#include "bufaux.h"
#include "compat.h"
#include "log.h"
#include "key.h"
#include "ssh-dss.h"

#define INTBLOB_LEN	20
#define SIGBLOB_LEN	(2*INTBLOB_LEN)

int
ssh_dss_sign(
    Key *key,
    u_char **sigp, int *lenp,
    u_char *data, int datalen)
{
	DSA_SIG *sig;
   SHA_CTX sha1_ctx;
	u_char *digest, *ret, sigblob[SIGBLOB_LEN];
	u_int rlen, slen, len, dlen;
	Buffer b;

	if (key == NULL || key->type != KEY_DSA || key->dsa == NULL) {
		error("ssh_dss_sign: no DSA key");
		return -1;
	}
	dlen = SHA_DIGEST_LENGTH;
	digest = (u_char *)xmalloc(dlen);
	SHA1_Init(&sha1_ctx);
	SHA1_Update(&sha1_ctx, data, datalen);
	SHA1_Final(digest, &sha1_ctx);

	sig = DSA_do_sign(digest, dlen, key->dsa);

	memset(digest, 0, dlen);
	xfree((LONG*)digest);
	if (sig == NULL) {
		error("ssh_dss_sign: sign failed");
		return -1;
	}

	rlen = BN_num_bytes(sig->r);
	slen = BN_num_bytes(sig->s);
	if (rlen > INTBLOB_LEN || slen > INTBLOB_LEN) {
		error("bad sig size %d %d", rlen, slen);
		DSA_SIG_free(sig);
		return -1;
	}
	memset(sigblob, 0, SIGBLOB_LEN);
	BN_bn2bin(sig->r, sigblob+ SIGBLOB_LEN - INTBLOB_LEN - rlen);
	BN_bn2bin(sig->s, sigblob+ SIGBLOB_LEN - slen);
	DSA_SIG_free(sig);

	if (datafellows & SSH_BUG_SIGBLOB) {
		ret = (u_char *)xmalloc(SIGBLOB_LEN);
		memcpy(ret, sigblob, SIGBLOB_LEN);
		if (lenp != NULL)
			*lenp = SIGBLOB_LEN;
		if (sigp != NULL)
			*sigp = ret;
	} else {
		/* ietf-drafts */
		buffer_init(&b);
      /* JOHN BORCHEK FIX: what is the type of a string under TOPS? */
		buffer_put_cstring(&b, "ssh-dss");
		buffer_put_string(&b, sigblob, SIGBLOB_LEN);
		len = buffer_len(&b);
		ret = (u_char *)xmalloc(len);
		memcpy(ret, buffer_ptr(&b), len);
		buffer_free(&b);
		if (lenp != NULL)
			*lenp = len;
		if (sigp != NULL)
			*sigp = ret;
	}
	return 0;
}
