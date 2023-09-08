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
 *
 *
 * Copyright (c) 1999 Niels Provos.  All rights reserved.
 * Copyright (c) 1999, 2000 Markus Friedl.  All rights reserved.
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
RCSID("$OpenBSD: cipher.c,v 1.47 2001/08/23 11:31:59 markus Exp $");

#include "xmalloc.h"
#include "log.h"
#include "cipher.h"

/* no encryption */
static VOID
none_setkey(CipherContext *cc, const UNS8 *key, u_int keylen)
{
}
static VOID
none_setiv(CipherContext *cc, const UNS8 *iv, u_int ivlen)
{
}
static VOID
none_crypt(CipherContext *cc, UNS8 *dest, const UNS8 *src, u_int len)
{
	memcpy(dest, src, len);
}

/* 3DES */
static VOID
des3_setkey(CipherContext *cc, const UNS8 *key, u_int keylen)
{
	des_set_key((const_des_cblock*)  key,     cc->u.des3.key1);
	des_set_key((const_des_cblock*) (key+8),  cc->u.des3.key2);
	des_set_key((const_des_cblock*) (key+16), cc->u.des3.key3);
}
static VOID
des3_setiv(CipherContext *cc, const UNS8 *iv, u_int ivlen)
{
	memset(cc->u.des3.iv1, 0, sizeof(cc->u.des3.iv1));
	memset(cc->u.des3.iv2, 0, sizeof(cc->u.des3.iv2));
	memset(cc->u.des3.iv3, 0, sizeof(cc->u.des3.iv3));
	if (iv == NULL)
		return;
	memcpy(cc->u.des3.iv3, (char *)iv, 8);
}
static VOID
des3_cbc_encrypt(CipherContext *cc, UNS8 *dest, const UNS8 *src, u_int len)
{
	des_ede3_cbc_encrypt(src, dest, len,
	    cc->u.des3.key1, cc->u.des3.key2, cc->u.des3.key3,
	    &cc->u.des3.iv3, DES_ENCRYPT);
}
static VOID
des3_cbc_decrypt(CipherContext *cc, UNS8 *dest, const UNS8 *src, u_int len)
{
	des_ede3_cbc_encrypt(src, dest, len,
	    cc->u.des3.key1, cc->u.des3.key2, cc->u.des3.key3,
	    &cc->u.des3.iv3, DES_DECRYPT);
}

Cipher ciphers[] = {
	{ "none",
		SSH_CIPHER_NONE, 8, 0,
		none_setkey, none_setiv,
		none_crypt, none_crypt },
	{ "3des-cbc",
		SSH_CIPHER_SSH2, 8, 24,
		des3_setkey, des3_setiv,
		des3_cbc_encrypt, des3_cbc_decrypt },
	{ NULL, SSH_CIPHER_ILLEGAL, 0, 0, NULL, NULL, NULL, NULL }
};

/*--*/

Cipher *
cipher_by_name(const char *name)
{
	Cipher *c;
	for (c = ciphers; c->name != NULL; c++)
		if (strcasecmp(c->name, name) == 0)
			return c;
	return NULL;
}

VOID
cipher_init(CipherContext *cc, Cipher *cipher,
    const UNS8 *key, u_int keylen, const UNS8 *iv, u_int ivlen)
{
	if (keylen < cipher->key_len)
		fatal("cipher_init: key length %d is insufficient for %s.",
		    keylen, cipher->name);
	if (iv != NULL && ivlen < cipher->block_size)
		fatal("cipher_init: iv length %d is insufficient for %s.",
		    ivlen, cipher->name);
	cc->cipher = cipher;
	cipher->setkey(cc, key, keylen);
	cipher->setiv(cc, iv, ivlen);
}

VOID
cipher_encrypt(CipherContext *cc, UNS8 *dest, const UNS8 *src, u_int len)
{
  if (UMOD(len,cc->cipher->block_size))
    fatal("cipher_encrypt: bad plaintext length %d", len);
  cc->cipher->encrypt(cc, dest, src, len);
}

VOID
cipher_decrypt(CipherContext *cc, UNS8 *dest, const UNS8 *src, u_int len)
{
  if (UMOD(len,cc->cipher->block_size))
    fatal("cipher_decrypt: bad ciphertext length %d", len);
  cc->cipher->decrypt(cc, dest, src, len);
}
