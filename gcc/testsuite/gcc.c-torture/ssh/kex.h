/*	$OpenBSD: kex.h,v 1.26 2001/06/26 17:27:23 markus Exp $	*/

/*
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
#ifndef KEX_H
#define KEX_H

#include "openssl/evp.h"
#include "buffer.h"
#include "cipher.h"
#include "key.h"

#define	KEX_DH1		"diffie-hellman-group1-sha1"
#define	KEX_DHGEX	"diffie-hellman-group-exchange-sha1"

enum kex_init_proposals {
	PROPOSAL_KEX_ALGS,
	PROPOSAL_SERVER_HOST_KEY_ALGS,
	PROPOSAL_ENC_ALGS_CTOS,
	PROPOSAL_ENC_ALGS_STOC,
	PROPOSAL_MAC_ALGS_CTOS,
	PROPOSAL_MAC_ALGS_STOC,
	PROPOSAL_COMP_ALGS_CTOS,
	PROPOSAL_COMP_ALGS_STOC,
	PROPOSAL_LANG_CTOS,
	PROPOSAL_LANG_STOC,
	PROPOSAL_MAX
};

enum kex_modes {
	MODE_IN,
	MODE_OUT,
	MODE_MAX
};

enum kex_exchange {
	DH_GRP1_SHA1,
	DH_GEX_SHA1
};

#define KEX_INIT_SENT	0x0001

typedef struct Kex Kex;
typedef struct Mac Mac;
typedef struct Comp Comp;
typedef struct Enc Enc;
typedef struct Newkeys Newkeys;

struct Enc {
	char	*name;
	Cipher	*cipher;
	int	enabled;
	u_char	*key;
	u_char	*iv;
};
struct Mac {
	char	*name;
	int	enabled;
	EVP_MD	*md;
	int	mac_len;
	u_char	*key;
	int	key_len;
};
struct Comp {
	int	type;
	int	enabled;
	char	*name;
};
struct Newkeys {
	Enc	enc;
	Mac	mac;
	Comp	comp;
};
struct Kex {
	UNS8	*session_id;
	int	session_id_len;
	Newkeys	*newkeys[MODE_MAX];
	int	we_need;
	int	server;
	char	*name;
	int	hostkey_type;
	int	kex_type;
	Buffer	my;
	Buffer	peer;
	int	done;
	int	flags;
	UNS8	*client_version_string;
	UNS8	*server_version_string;
	int	(*verify_host_key)(Key *);
	Key	*(*load_host_key)(int);
};

Kex	*kex_setup(char *[PROPOSAL_MAX]);
VOID	 kex_finish(Kex *);

VOID	 kex_send_kexinit(Kex *);
VOID	 kex_input_kexinit(int, int, LONG*);
VOID	 kex_derive_keys(Kex *, UNS8 *, BIGNUM *);

VOID	 kexdh(Kex *);

Newkeys *kex_get_newkeys(int);

#if defined(DEBUG_KEX) || defined(DEBUG_KEXDH)
VOID	dump_digest(char *, u_char *, int);
#endif

#endif
