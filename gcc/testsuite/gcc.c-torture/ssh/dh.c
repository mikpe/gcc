/*
 * Copyright (c) 2000 Niels Provos.  All rights reserved.
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
RCSID("$OpenBSD: dh.c,v 1.17 2001/06/23 15:12:18 itojun Exp $");

#include "xmalloc.h"

#include "openssl/bn.h"
#include "openssl/dh.h"
#include "openssl/evp.h"

#include "buffer.h"
#include "cipher.h"
#include "kex.h"
#include "dh.h"
#include "pathnames.h"
#include "log.h"
#include "misc.h"
#include "addmore.h"

/* diffie-hellman-group1-sha1 */

int
dh_pub_is_valid(DH *dh, BIGNUM *dh_pub)
{
	int i;
	int n = BN_num_bits(dh_pub);
	int bits_set = 0;

	if (dh_pub->neg) {
		log("invalid public DH value: negativ");
		return 0;
	}
	for (i = 0; i <= n; i++)
		if (BN_is_bit_set(dh_pub, i))
			bits_set++;
	debug("bits set: %d/%d", bits_set, BN_num_bits(dh->p));

	/* if g==2 and bits_set==1 then computing log_g(dh_pub) is trivial */
	if (bits_set > 1 && (BN_cmp(dh_pub, dh->p) == -1))
		return 1;
	log("invalid public DH value (%d/%d)", bits_set, BN_num_bits(dh->p));
	return 0;
}

VOID
dh_gen_key(DH *dh, int need)
{
	int i, bits_set = 0, tries = 0;

	debug("dh_gen_key: need %d\r\n", need);
	if (dh->p == NULL)
		fatal("dh_gen_key: dh->p == NULL");
	if (2*need >= BN_num_bits(dh->p))
		fatal("dh_gen_key: group too small: %d (2*need %d)",
		    BN_num_bits(dh->p), 2*need);
	do {
		if (dh->priv_key != NULL)
			BN_free(dh->priv_key);
		dh->priv_key = BN_new();
		if (dh->priv_key == NULL)
			fatal("dh_gen_key: BN_new failed");
		/* generate a 2*need bits random private exponent */
		if (!BN_rand(dh->priv_key, 2*need, 0, 0))
			fatal("dh_gen_key: BN_rand failed");
		if (DH_generate_key(dh) == 0)
			fatal("DH_generate_key");
		for (i = 0; i <= BN_num_bits(dh->priv_key); i++)
			if (BN_is_bit_set(dh->priv_key, i))
				bits_set++;
		debug("dh_gen_key: priv key bits set: %d/%d",
		    bits_set, BN_num_bits(dh->priv_key));
		if (tries++ > 10)
			fatal("dh_gen_key: too many bad keys: giving up");
	} while (!dh_pub_is_valid(dh, dh->pub_key));
	/*--------------------------------------------------
	* printf("dh_gen_key: Printing public key\r\n");
	* printbn(dh->pub_key);
	*--------------------------------------------------*/
}

DH *
dh_new_group_asc(const UNS8 *gen, const UNS8 *modulus)
{
	DH *dh;

	dh = DH_new();
	if (dh == NULL)
		fatal("DH_new");

	if (BN_hex2bn(&dh->p, modulus) == 0)
		fatal("BN_hex2bn p");
	if (BN_hex2bn(&dh->g, gen) == 0)
		fatal("BN_hex2bn g");

	return (dh);
}

DH *
dh_new_group1(VOID)
{
#ifdef TOPS_PORT
		/* JOHN BORCHEK FIX: TOPS20 compiler may not take this initializer */
	static UNS8 *gen = "2", *group1 =
	    "FFFFFFFF" "FFFFFFFF" "C90FDAA2" "2168C234" "C4C6628B" "80DC1CD1"
	    "29024E08" "8A67CC74" "020BBEA6" "3B139B22" "514A0879" "8E3404DD"
	    "EF9519B3" "CD3A431B" "302B0A6D" "F25F1437" "4FE1356D" "6D51C245"
	    "E485B576" "625E7EC6" "F44C42E9" "A637ED6B" "0BFF5CB6" "F406B7ED"
	    "EE386BFB" "5A899FA5" "AE9F2411" "7C4B1FE6" "49286651" "ECE65381"
	    "FFFFFFFF" "FFFFFFFF";
#else
	static char *gen = "2", *group1 =
	    "FFFFFFFF" "FFFFFFFF" "C90FDAA2" "2168C234" "C4C6628B" "80DC1CD1"
	    "29024E08" "8A67CC74" "020BBEA6" "3B139B22" "514A0879" "8E3404DD"
	    "EF9519B3" "CD3A431B" "302B0A6D" "F25F1437" "4FE1356D" "6D51C245"
	    "E485B576" "625E7EC6" "F44C42E9" "A637ED6B" "0BFF5CB6" "F406B7ED"
	    "EE386BFB" "5A899FA5" "AE9F2411" "7C4B1FE6" "49286651" "ECE65381"
	    "FFFFFFFF" "FFFFFFFF";

#endif
	return (dh_new_group_asc(gen, group1));
}

