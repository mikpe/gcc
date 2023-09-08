/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This file contains functions for reading and writing identity files, and
 * for reading the passphrase from the user.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 *
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
RCSID("$OpenBSD: authfile.c,v 1.39 2001/10/07 10:29:52 markus Exp $");

#include "openssl/err.h"
#include "openssl/evp.h"
#include "openssl/pem.h"

#include "cipher.h"
#include "xmalloc.h"
#include "buffer.h"
#include "bufaux.h"
#include "key.h"
#include "ssh.h"
#include "log.h"
#include "authfile.h"
#include "addmore.h"

/* Version identification string for SSH v1 identity files. */
const char authfile_id_string[] =
    "SSH static KEY FILE FORMAT 1.1\r\n";

/*
 * Loads the public part of the ssh v1 key file.  Returns NULL if an error was
 * encountered (the file does not exist or is not readable), and the key
 * otherwise.
 */

static Key *
key_load_private_pem(int fd, int type)
{
  EVP_PKEY *pk = NULL;
  Key *prv = NULL;
  char *name = "<no key>";
  
  pk = PEM_read_PrivateKey(fd);
  if (pk == NULL) {
    debug("PEM_read_PrivateKey failed");
    (VOID)ERR_get_error();
  } else if (pk->type == EVP_PKEY_DSA &&
	     (type == KEY_UNSPEC||type==KEY_DSA)) {
    prv = key_new(KEY_UNSPEC);
    pk->references++;
    prv->dsa = pk->pkey.dsa;
    prv->type = KEY_DSA;
    name = "dsa w/o comment";
#ifdef DEBUG_PK
    DSA_print_fp(stderr, prv->dsa, 8);
#endif
  } else {
    error("PEM_read_PrivateKey: mismatch or "
	  "unknown EVP_PKEY save_type %d", pk->save_type);
  }
  close(fd);
  if (pk != NULL)
    {
      pk->references--;
      if (pk->references == 0)
	{
	  DSA_free(pk->pkey.dsa);
	  OPENSSL_free(pk);
	}
    }
  
  /*--------------------------------------------------
   *   if (prv) 
   *     printf("key_load_prv_pem: type=%d pk_type=%d prv_type = %d\r\n", type,
   * 	   pk->type, prv->type);
   *   else
   *     printf("key_load_prv_pem: type=%d pk_type=%d no prv\r\n", type, pk->type);
   *--------------------------------------------------*/
  
  debug("read PEM private key done: type %s",
	prv ? key_type(prv) : (char *)"<unknown>");
  return prv;
}

static int
key_perm_ok(int fd, const char *filename)
{
  return 1;
}


Key *
key_load_private(const char *filename)
{
	int fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return NULL;
	if (!key_perm_ok(fd, filename)) {
		error("bad permissions: ignore key: %s", filename);
		close(fd);
		return NULL;
	}
   /* closes fd */
   return key_load_private_pem(fd, KEY_UNSPEC);
}

