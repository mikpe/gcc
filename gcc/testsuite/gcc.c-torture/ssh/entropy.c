/*
 * Copyright (c) 2000 Damien Miller.  All rights reserved.
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

#include "openssl/rand.h"
#include "openssl/sha.h"
#include "openssl/crypto.h"

#include "ssh.h"
#include "misc.h"
#include "xmalloc.h"
#include "atomicio.h"
#include "pathnames.h"
#include "log.h"

RCSID("$Id: entropy.c 562abd07bbb7 2008/03/04 03:29:39 Martin Chaney <chaney@xkl.com> $");

#ifdef TOPS20
int
get_random_bytes(UNS8 *buf, int len)
{
   /* JOHN BORCHEK FIX: This needs to fill-in buffer buf with random bytes of 
                        length len */
   return(0);
}

VOID
seed_rng(VOID)
{
   /* JOHN BORCHEK FIX: What needs to be done to seed the random number 
                        generator */
}

VOID
init_rng(VOID) 
{
   /* JOHN BORCHEK FIX: What needs to be done to initialize the random number 
                        generator */
}
#else

/* Location of random number pool  */
#define RANDOM_POOL "/dev/urandom"
/* Collect entropy from /dev/urandom or pipe */
static int
get_random_bytes(UNS8 *buf, int len)
{
	int random_pool;

	random_pool = open(RANDOM_POOL, O_RDONLY);
	if (random_pool == -1) {
		error("Couldn't open random pool \"%s\": %s",
			RANDOM_POOL, strerror(errno));
		return(0);
	}

	if (atomicio(read, random_pool, buf, len) != len) {
		error("Couldn't read from random pool \"%s\": %s",
			RANDOM_POOL, strerror(errno));
		close(random_pool);
		return(0);
	}

	close(random_pool);

	return(1);
}

/*
 * Seed OpenSSL's random number pool from Kernel random number generator
 * or PRNGD/EGD
 */
VOID
seed_rng(VOID)
{
	UNS8 buf[32];

	debug("Seeding random number generator");

	if (!get_random_bytes(buf, sizeof(buf))) {
		if (!RAND_status())
			fatal("Entropy collection failed and entropy exhausted");
	} else {
		RAND_add(buf, sizeof(buf), sizeof(buf));
	}

	memset(buf, '\0', sizeof(buf));
}

VOID
init_rng(VOID) 
{
}
#endif /* TOPS20 */

