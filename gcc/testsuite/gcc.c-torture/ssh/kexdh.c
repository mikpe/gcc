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
RCSID("$OpenBSD: kexdh.c,v 1.7 2001/09/17 19:27:15 stevesk Exp $");

#include "openssl/crypto.h"
#include "openssl/bn.h"

#include "xmalloc.h"
#include "buffer.h"
#include "bufaux.h"
#include "key.h"
#include "kex.h"
#include "log.h"
#include "packet.h"
#include "dh.h"
#include "ssh2.h"

void printbn(BIGNUM *bn);

static UNS8 *
kex_dh_hash(
    UNS8 *client_version_string,
    UNS8 *server_version_string,
    UNS8 *ckexinit, int ckexinitlen,
    UNS8 *skexinit, int skexinitlen,
    UNS8 *serverhostkeyblob, int sbloblen,
    BIGNUM *client_dh_pub,
    BIGNUM *server_dh_pub,
    BIGNUM *shared_secret)
{
	Buffer b;
	static UNS8 digest[SHA_DIGEST_LENGTH];
   SHA_CTX sha1_ctx;

	buffer_init(&b);
	buffer_put_cstring(&b, client_version_string);
	buffer_put_cstring(&b, server_version_string);

	/* kexinit messages: fake header: len+SSH2_MSG_KEXINIT */
	buffer_put_int(&b, ckexinitlen+1);
	buffer_put_char(&b, SSH2_MSG_KEXINIT);
	buffer_append(&b, ckexinit, ckexinitlen);
	buffer_put_int(&b, skexinitlen+1);
	buffer_put_char(&b, SSH2_MSG_KEXINIT);
	buffer_append(&b, skexinit, skexinitlen);

	buffer_put_string(&b, serverhostkeyblob, sbloblen);
	buffer_put_bignum2(&b, client_dh_pub);
	buffer_put_bignum2(&b, server_dh_pub);
	buffer_put_bignum2(&b, shared_secret);

#ifdef DEBUG_KEX
	printf("buffer dump in kex_dh_hash\r\n");
	buffer_dump(&b);
#endif
   SHA1_Init(&sha1_ctx);
   SHA1_Update(&sha1_ctx, buffer_ptr(&b), buffer_len(&b));
/*--------------------------------------------------
*    printf("kexdh_server: printing sha1_ctx\r\n");
*    printf("h0=0x%x h1=0x%x h2=0x%x h3=0x%x h4=0x%x\r\n",
* 		   sha1_ctx.h0,sha1_ctx.h1,sha1_ctx.h2,sha1_ctx.h3,
* 		   sha1_ctx.h4);
*    printf("Nl=0x%x Nh=0x%x, num=%d skipping data\r\n", sha1_ctx.Nl, sha1_ctx.Nh,
* 		   sha1_ctx.num);
* 	for(i=0;i<sha1_ctx.num>>2;i++) {
* 		printf("0x%x ", sha1_ctx.data[i]);
* 	}
* 	printf("\r\n");
*--------------------------------------------------*/

   SHA1_Final(digest, &sha1_ctx);

	buffer_free(&b);

#ifdef DEBUG_KEX
	dump_digest("hash", digest, SHA_DIGEST_LENGTH);
#endif
	return digest;
}

/* kexdh_client removed */

/* server */
/* this is called on TOPS20 */

static VOID
kexdh_server(Kex *kex)
{
	BIGNUM *shared_secret = NULL, *dh_client_pub = NULL;
	DH *dh;
	Key *server_host_key;
	UNS8 *kbuf, *hash, *signature = NULL, *server_host_key_blob = NULL;
	u_int sbloblen, klen, kout;
	int dlen, slen, plen;

	/* generate server DH public key */
	dh = dh_new_group1();
	dh_gen_key(dh, kex->we_need * 8);
	
	debug("expecting SSH2_MSG_KEXDH_INIT");

	packet_read_expect(&plen, SSH2_MSG_KEXDH_INIT);

	if (kex->load_host_key == NULL)
		fatal("Cannot load hostkey");
	server_host_key = kex->load_host_key(kex->hostkey_type);
	if (server_host_key == NULL)
		fatal("Unsupported hostkey type %d", kex->hostkey_type);

	/* key, cert */
	dh_client_pub = BN_new();
	if (dh_client_pub == NULL)
		fatal("dh_client_pub == NULL");
	packet_get_bignum2(dh_client_pub, &dlen);

	if (!dh_pub_is_valid(dh, dh_client_pub))
		packet_disconnect("bad client public DH value");

/*	printf("Printing client public key\r\n");*/
/*	printbn(dh_client_pub);*/

	klen = DH_size(dh);
	kbuf = (UNS8 *)xmalloc(klen);
	kout = DH_compute_key(kbuf, dh_client_pub, dh);

/*	printf("kexdh_server: printing kbuf \r\n");*/
/*	for(i=0;i<klen;i++)*/
/*		printf("0x%x ",kbuf[i]);*/
/*	printf("\r\n");*/

	shared_secret = BN_new();
	BN_bin2bn(kbuf, kout, shared_secret);
	memset(kbuf, 0, klen);
	xfree((LONG*)kbuf);

	/*--------------------------------------------------
	* printf("kexdh_server: printing shared_secret\r\n");
	* printbn(shared_secret);
	*--------------------------------------------------*/

	/*--------------------------------------------------
	* printf("kexdh_server: printing server_host_key\r\n\r\n");
	* printf("kexdh_server: server_host_key name %s length %d\r\n",
	* 		key_ssh_name(server_host_key), strlen(key_ssh_name(server_host_key)));
	* printf("kexdh_server: server_host_key->dsa->p\r\n");
	* printbn(server_host_key->dsa->p);
	* printf("\r\n\r\nkexdh_server: server_host_key->dsa->q\r\n");
	* printbn(server_host_key->dsa->q);
	* printf("\r\n\r\nkexdh_server: server_host_key->dsa->g\r\n");
	* printbn(server_host_key->dsa->g);
	* printf("\r\n\r\nkexdh_server: server_host_key->dsa->pub_key\r\n");
	* printbn(server_host_key->dsa->pub_key);
	* printf("\r\n\r\n");
	*--------------------------------------------------*/

	key_to_blob(server_host_key, &server_host_key_blob, &sbloblen);

	/*--------------------------------------------------
	* printf("kexdh_server: client_version_string=%s\r\n",
	* 		kex->client_version_string);
	* printf("kexdh_server: server_version_string=%s\r\n",
	* 		kex->server_version_string);
	* printf("kexdh_server: kex->peer length %d\r\n",
	* 		buffer_len(&kex->peer));
	* for(i=0;i<buffer_len(&kex->peer);i++) {
	* 	printf("0x%x ", *(buffer_ptr(&kex->peer)+i));
	* }
	* printf("\r\nkexdh_server: kex->my length %d\r\n",
	* 		buffer_len(&kex->my));
	* for(i=0;i<buffer_len(&kex->my);i++) {
	* 	printf("0x%x ", *(buffer_ptr(&kex->my)+i));
	* }
	*--------------------------------------------------*/

	/* calc H */
	hash = kex_dh_hash(
	    kex->client_version_string,
	    kex->server_version_string,
	    buffer_ptr(&kex->peer), buffer_len(&kex->peer),
	    buffer_ptr(&kex->my), buffer_len(&kex->my),
	    server_host_key_blob, sbloblen,
	    dh_client_pub,
	    dh->pub_key,
	    shared_secret
	);
	BN_free(dh_client_pub);

	/* save session id := H */
	/* XXX hashlen depends on KEX */
	if (kex->session_id == NULL) {
		kex->session_id_len = 20;
		kex->session_id = (UNS8 *)xmalloc(kex->session_id_len);
		memcpy(kex->session_id, hash, kex->session_id_len);
	}

	/* sign H */
	/* XXX hashlen depends on KEX */
	key_sign(server_host_key, &signature, &slen, hash, 20);
	/*--------------------------------------------------
	* printf("kexdh_server: signature is \r\n");
	* for(i=0;i<slen;i++)
	* 	printf("0x%x ", signature[i]);
	* printf("\r\n");
	*--------------------------------------------------*/

	/* destroy_sensitive_data(); */

	/* send server hostkey, DH pubkey 'f' and singed H */
	packet_start(SSH2_MSG_KEXDH_REPLY);
	packet_put_string(server_host_key_blob, sbloblen);
	packet_put_bignum2(dh->pub_key);	/* f */
	packet_put_string(signature, slen);
	packet_send();

	xfree((LONG*)signature);
	xfree((LONG*)server_host_key_blob);
	/* have keys, free DH */
	DH_free(dh);

	kex_derive_keys(kex, hash, shared_secret);
	BN_clear_free(shared_secret);
	kex_finish(kex);
}

VOID
kexdh(Kex *kex)
{
	kexdh_server(kex);
}
