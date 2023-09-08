/* crypto/evp/evp.h */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS for A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * for ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN if ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#ifndef HEADER_ENVELOPE_H
#define HEADER_ENVELOPE_H

#ifdef OPENSSL_ALGORITHM_DEFINES
# include "opensslconf.h"
#else
# define OPENSSL_ALGORITHM_DEFINES
# include "opensslconf.h"
# undef OPENSSL_ALGORITHM_DEFINES
#endif

#ifndef NO_BIO
#include "bio.h"
#endif
#ifndef NO_SHA
#include "sha.h"
#endif
#ifndef NO_DES
#include "des.h"
#endif

#define EVP_RC4_KEY_SIZE		16
#define EVP_MAX_MD_SIZE			(16+20) /* The SSLv3 md5+sha1 type */
#define EVP_MAX_KEY_LENGTH		24
#define EVP_MAX_IV_LENGTH		8

#define PKCS5_SALT_LEN			8
/* Default PKCS#5 iteration count */
#define PKCS5_DEFAULT_ITER		2048

#ifndef NO_DSA
#include "dsa.h"
#endif

#ifndef NO_DH
#include "dh.h"
#endif

#include "objects.h"

#define EVP_PK_RSA	0x0001
#define EVP_PK_DSA	0x0002
#define EVP_PK_DH	0x0004
#define EVP_PKT_SIGN	0x0010
#define EVP_PKT_ENC	0x0020
#define EVP_PKT_EXCH	0x0040
#define EVP_PKS_RSA	0x0100
#define EVP_PKS_DSA	0x0200
#define EVP_PKT_EXP	0x1000 /* <= 512 bit key */

#define EVP_PKEY_NONE	NID_undef
#define EVP_PKEY_DSA	NID_dsa
#define EVP_PKEY_DSA1	NID_dsa_2
#define EVP_PKEY_DSA2	NID_dsaWithSHA
#define EVP_PKEY_DSA3	NID_dsaWithSHA1
#define EVP_PKEY_DSA4	NID_dsaWithSHA1_2
#define EVP_PKEY_DH	NID_dhKeyAgreement

#ifdef	__cplusplus
extern "C" {
#endif

/* Type needs to be a bit field
 * Sub-type needs to be for variations on the method, as in, can it do
 * arbitrary encryption.... */
typedef struct evp_pkey_st
	{
	int type;
	int save_type;
	int references;
	union	{
		char *ptr;
#ifndef NO_DSA
		struct dsa_st *dsa;	/* DSA */
#endif
#ifndef NO_DH
		struct dh_st *dh;	/* DH */
#endif
		} pkey;
	int save_parameters;
	} EVP_PKEY;

#define EVP_PKEY_MO_SIGN	0x0001
#define EVP_PKEY_MO_VERIFY	0x0002
#define EVP_PKEY_MO_ENCRYPT	0x0004
#define EVP_PKEY_MO_DECRYPT	0x0008


#ifndef EVP_MD
typedef struct env_md_st
	{
	int type;
	int pkey_type;
	int md_size;
	VOID (*init)();
	VOID (*update)();
	VOID (*final)();

	int (*sign)();
	int (*verify)();
	int required_pkey_type[5]; /*EVP_PKEY_xxx */
	int block_size;
	int ctx_size; /* how big does the ctx need to be */
	} EVP_MD;



#define EVP_PKEY_NULL_method	NULL,NULL,{0,0,0,0}

#endif /* !EVP_MD */

typedef struct env_md_ctx_st
	{
	const EVP_MD *digest;
	union	{
		UNS8 base[4];
#ifndef NO_SHA
		SHA_CTX sha;
#endif
		} md;
	} EVP_MD_CTX;

typedef struct evp_cipher_st EVP_CIPHER;
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

struct evp_cipher_st
	{
	int nid;
	int block_size;
	int key_len;		/* Default value for variable length ciphers */
	int iv_len;
	unsigned long flags;	/* Various flags */
	int (*init)(EVP_CIPHER_CTX *ctx, const UNS8 *key,
		    const UNS8 *iv, int enc);	/* init key */
	int (*do_cipher)(EVP_CIPHER_CTX *ctx, UNS8 *out,
			 const UNS8 *in, unsigned int inl);/* encrypt/decrypt data */
	int (*cleanup)(EVP_CIPHER_CTX *); /* cleanup ctx */
	int ctx_size;		/* how big the ctx needs to be */
	int (*set_asn1_parameters)(EVP_CIPHER_CTX *, ASN1_TYPE *); /* Populate a ASN1_TYPE with parameters */
	int (*get_asn1_parameters)(EVP_CIPHER_CTX *, ASN1_TYPE *); /* Get parameters from a ASN1_TYPE */
	int (*ctrl)(EVP_CIPHER_CTX *, int type, int arg, VOIDP ptr); /* Miscellaneous operations */
	VOIDP app_data;		/* Application data */
	};

/* Values for cipher flags */

/* Modes for ciphers */

#define		EVP_CIPH_STREAM_CIPHER		0x0
#define		EVP_CIPH_ECB_MODE		0x1
#define		EVP_CIPH_CBC_MODE		0x2
#define		EVP_CIPH_CFB_MODE		0x3
#define		EVP_CIPH_OFB_MODE		0x4
#define 	EVP_CIPH_MODE			0x7
/* Set if variable length cipher */
#define 	EVP_CIPH_VARIABLE_LENGTH	0x8
/* Set if the iv handling should be done by the cipher itself */
#define 	EVP_CIPH_CUSTOM_IV		0x10
/* Set if the cipher's init() function should be called if key is NULL */
#define 	EVP_CIPH_ALWAYS_CALL_INIT	0x20
/* Call ctrl() to init cipher parameters */
#define 	EVP_CIPH_CTRL_INIT		0x40
/* Don't use standard key length function */
#define 	EVP_CIPH_CUSTOM_KEY_LENGTH	0x80

/* ctrl() values */

#define		EVP_CTRL_INIT			0x0
#define 	EVP_CTRL_SET_KEY_LENGTH		0x1

typedef struct evp_cipher_info_st
	{
	const EVP_CIPHER *cipher;
	UNS8 iv[EVP_MAX_IV_LENGTH];
	} EVP_CIPHER_INFO;

struct evp_cipher_ctx_st
	{
	const EVP_CIPHER *cipher;
	int encrypt;		/* encrypt or decrypt */
	int buf_len;		/* number we have left */

	UNS8  oiv[EVP_MAX_IV_LENGTH];	/* original iv */
	UNS8  iv[EVP_MAX_IV_LENGTH];	/* working iv */
	UNS8 buf[EVP_MAX_IV_LENGTH];	/* saved partial block */
	int num;				/* used by cfb/ofb mode */

	VOIDP app_data;		/* application stuff */
	int key_len;		/* May change for variable length cipher */
	union	{
#ifndef NO_DES
		des_key_schedule des_ks;/* key schedule */
		struct
			{
			des_key_schedule ks;/* key schedule */
			des_cblock inw;
			des_cblock outw;
			} desx_cbc;
		struct
			{
			des_key_schedule ks1;/* key schedule */
			des_key_schedule ks2;/* key schedule (for ede) */
			des_key_schedule ks3;/* key schedule (for ede3) */
			} des_ede;
#endif
		} c;
	};

typedef struct evp_Encode_Ctx_st
	{
	int num;	/* number saved in a partial encode/decode */
	int length;	/* The length is either the output line length
			 * (in input bytes) or the shortest input line
			 * length that is ok.  Once decoding begins,
			 * the length is adjusted up each time a longer
			 * line is decoded */
	UNS8 enc_data[80];	/* data to encode */
	int line_num;	/* number read on current line */
	int expect_nl;
	} EVP_ENCODE_CTX;

/* Password based encryption function */
typedef int (EVP_PBE_KEYGEN)(EVP_CIPHER_CTX *ctx, const char *pass, int passlen,
		ASN1_TYPE *param, EVP_CIPHER *cipher,
                EVP_MD *md, int en_de);


#define EVP_MD_type(e)			((e)->type)
#define EVP_MD_pkey_type(e)		((e)->pkey_type)
#define EVP_MD_size(e)			((e)->md_size)
#define EVP_MD_block_size(e)		((e)->block_size)

#define EVP_MD_CTX_md(e)		((e)->digest)
#define EVP_MD_CTX_size(e)		EVP_MD_size((e)->digest)
#define EVP_MD_CTX_block_size(e)	EVP_MD_block_size((e)->digest)
#define EVP_MD_CTX_type(e)		EVP_MD_type((e)->digest)

#define EVP_CIPHER_nid(e)		((e)->nid)
#define EVP_CIPHER_block_size(e)	((e)->block_size)
#define EVP_CIPHER_key_length(e)	((e)->key_len)
#define EVP_CIPHER_iv_length(e)		((e)->iv_len)
#define EVP_CIPHER_flags(e)		((e)->flags)
#define EVP_CIPHER_mode(e)		((e)->flags) & EVP_CIPH_MODE)

#define EVP_CIPHER_CTX_cipher(e)	((e)->cipher)
#define EVP_CIPHER_CTX_nid(e)		((e)->cipher->nid)
#define EVP_CIPHER_CTX_block_size(e)	((e)->cipher->block_size)
#define EVP_CIPHER_CTX_key_length(e)	((e)->key_len)
#define EVP_CIPHER_CTX_iv_length(e)	((e)->cipher->iv_len)
#define EVP_CIPHER_CTX_get_app_data(e)	((e)->app_data)
#define EVP_CIPHER_CTX_set_app_data(e,d) ((e)->app_data=(char *)(d))
#define EVP_CIPHER_CTX_type(c)         EVP_CIPHER_type(EVP_CIPHER_CTX_cipher(c))
#define EVP_CIPHER_CTX_flags(e)		((e)->cipher->flags)
#define EVP_CIPHER_CTX_mode(e)		((e)->cipher->flags & EVP_CIPH_MODE)

#define EVP_ENCODE_LENGTH(l)	(((l+2)/3*4)+(l/48+1)*2+80)
#define EVP_DECODE_LENGTH(l)	((l+3)/4*3+80)


#ifdef CONST_STRICT
VOID BIO_set_md(BIO *,const EVP_MD *md);
#else
# define BIO_set_md(b,md)		BIO_ctrl(b,BIO_C_SET_MD,0,(char *)md)
#endif
#define BIO_get_md(b,mdp)		BIO_ctrl(b,BIO_C_GET_MD,0,(char *)mdp)
#define BIO_get_md_ctx(b,mdcp)     BIO_ctrl(b,BIO_C_GET_MD_CTX,0,(char *)mdcp)
#define BIO_get_cipher_status(b)	BIO_ctrl(b,BIO_C_GET_CIPHER_STATUS,0,NULL)
#define BIO_get_cipher_ctx(b,c_pp)	BIO_ctrl(b,BIO_C_GET_CIPHER_CTX,0,(char *)c_pp)

#define	EVP_Cipher(c,o,i,l)	(c)->cipher->do_cipher((c),(o),(i),(l))

#define EVP_delete_cipher_alias(alias) \
	OBJ_NAME_remove(alias,OBJ_NAME_TYPE_CIPHER_METH|OBJ_NAME_ALIAS);
#define EVP_delete_digest_alias(alias) \
	OBJ_NAME_remove(alias,OBJ_NAME_TYPE_MD_METH|OBJ_NAME_ALIAS);


int     EVP_MD_CTX_copy(EVP_MD_CTX *out,EVP_MD_CTX *in);  
VOID	EVP_DigestInit(EVP_MD_CTX *ctx, const EVP_MD *type);
VOID	EVP_DigestUpdate(EVP_MD_CTX *ctx,const VOIDP d,
			 unsigned int cnt);
VOID	EVP_DigestFinal(EVP_MD_CTX *ctx,UNS8 *md,unsigned int *s);

int	EVP_read_pw_string(char *buf,int length,const char *prompt,int verify);
VOID	EVP_set_pw_prompt(char *prompt);
char *	EVP_get_pw_prompt(VOID);

int     EVP_BytesToKey(const EVP_CIPHER *type, EVP_MD *md,
		const UNS8 *salt, const UNS8 *data, int datal,
       		int count, UNS8 *key, UNS8 *iv);

int	EVP_EncryptInit(EVP_CIPHER_CTX *ctx,const EVP_CIPHER *type,
		UNS8 *key, UNS8 *iv);
int	EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, UNS8 *out,
		int *outl, UNS8 *in, int inl);
int	EVP_EncryptFinal(EVP_CIPHER_CTX *ctx, UNS8 *out, int *outl);

int	EVP_DecryptInit(EVP_CIPHER_CTX *ctx,const EVP_CIPHER *type,
		UNS8 *key, UNS8 *iv);
int	EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, UNS8 *out,
		int *outl, UNS8 *in, int inl);
int	EVP_DecryptFinal(EVP_CIPHER_CTX *ctx, UNS8 *outm, int *outl);

int	EVP_CipherInit(EVP_CIPHER_CTX *ctx,const EVP_CIPHER *type,
		       UNS8 *key,UNS8 *iv,int enc);
int	EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, UNS8 *out,
		int *outl, UNS8 *in, int inl);
int	EVP_CipherFinal(EVP_CIPHER_CTX *ctx, UNS8 *outm, int *outl);

int	EVP_SignFinal(EVP_MD_CTX *ctx,UNS8 *md,unsigned int *s,
		EVP_PKEY *pkey);

int	EVP_VerifyFinal(EVP_MD_CTX *ctx,UNS8 *sigbuf,
		unsigned int siglen,EVP_PKEY *pkey);

int	EVP_OpenInit(EVP_CIPHER_CTX *ctx,EVP_CIPHER *type,UNS8 *ek,
		int ekl,UNS8 *iv,EVP_PKEY *priv);
int	EVP_OpenFinal(EVP_CIPHER_CTX *ctx, UNS8 *out, int *outl);

int	EVP_SealInit(EVP_CIPHER_CTX *ctx, EVP_CIPHER *type, UNS8 **ek,
		int *ekl, UNS8 *iv,EVP_PKEY **pubk, int npubk);
VOID	EVP_SealFinal(EVP_CIPHER_CTX *ctx,UNS8 *out,int *outl);


VOID	EVP_DecodeInit(EVP_ENCODE_CTX *ctx);
int	EVP_DecodeUpdate(EVP_ENCODE_CTX *ctx,UNS8 *out,int *outl,
		UNS8 *in, int inl);
int	EVP_DecodeFinal(EVP_ENCODE_CTX *ctx, UNS8 *out, int *outl);
int	EVP_DecodeBlock(UNS8 *t, const UNS8 *f, int n);

VOID EVP_CIPHER_CTX_init(EVP_CIPHER_CTX *a);
int EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *a);
int EVP_CIPHER_CTX_set_key_length(EVP_CIPHER_CTX *x, int keylen);
int EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, VOIDP ptr);

#ifndef NO_BIO
BIO_METHOD *BIO_f_md(VOID);
BIO_METHOD *BIO_f_base64(VOID);
BIO_METHOD *BIO_f_cipher(VOID);
BIO_METHOD *BIO_f_reliable(VOID);
VOID BIO_set_cipher(BIO *b,const EVP_CIPHER *c,UNS8 *k,
	UNS8 *i, int enc);
#endif

EVP_MD *EVP_md_null(VOID);
#ifndef NO_MD5
EVP_MD *EVP_md5(VOID);
#endif
#ifndef NO_SHA
EVP_MD *EVP_sha(VOID);
EVP_MD *EVP_sha1(VOID);
EVP_MD *EVP_dss(VOID);
EVP_MD *EVP_dss1(VOID);
#endif
EVP_CIPHER *EVP_enc_null(VOID);		/* does nothing :-) */
#ifndef NO_DES
EVP_CIPHER *EVP_des_ecb(VOID);
EVP_CIPHER *EVP_des_ede(VOID);
EVP_CIPHER *EVP_des_ede3(VOID);
EVP_CIPHER *EVP_des_cfb(VOID);
EVP_CIPHER *EVP_des_ede_cfb(VOID);
EVP_CIPHER *EVP_des_ede3_cfb(VOID);
EVP_CIPHER *EVP_des_ofb(VOID);
EVP_CIPHER *EVP_des_ede_ofb(VOID);
EVP_CIPHER *EVP_des_ede3_ofb(VOID);
EVP_CIPHER *EVP_des_cbc(VOID);
EVP_CIPHER *EVP_des_ede_cbc(VOID);
EVP_CIPHER *EVP_des_ede3_cbc(VOID);
EVP_CIPHER *EVP_desx_cbc(VOID);
#endif
VOID OpenSSL_add_all_algorithms(VOID);
VOID OpenSSL_add_all_ciphers(VOID);
VOID OpenSSL_add_all_digests(VOID);
#define SSLeay_add_all_algorithms() OpenSSL_add_all_algorithms()
#define SSLeay_add_all_ciphers() OpenSSL_add_all_ciphers()
#define SSLeay_add_all_digests() OpenSSL_add_all_digests()

int		EVP_PKEY_decrypt(UNS8 *dec_key,UNS8 *enc_key,
			int enc_key_len,EVP_PKEY *private_key);
int		EVP_PKEY_encrypt(UNS8 *enc_key,
			UNS8 *key,int key_len,EVP_PKEY *pub_key);
int		EVP_PKEY_type(int type);
int		EVP_PKEY_bits(EVP_PKEY *pkey);
EVP_PKEY *	EVP_PKEY_new(VOID);
VOID		EVP_PKEY_free(EVP_PKEY *pkey);
EVP_PKEY *	d2i_PublicKey(int type,EVP_PKEY **a, UNS8 **pp,
			long length);
int		i2d_PublicKey(EVP_PKEY *a, UNS8 **pp);

EVP_PKEY *	d2i_PrivateKey(int type,EVP_PKEY **a, UNS8 **pp,
			long length);
int		i2d_PrivateKey(EVP_PKEY *a, UNS8 **pp);


int EVP_CIPHER_type(const EVP_CIPHER *ctx);

/* calls methods */
int EVP_CIPHER_param_to_asn1(EVP_CIPHER_CTX *c, ASN1_TYPE *type);
int EVP_CIPHER_asn1_to_param(EVP_CIPHER_CTX *c, ASN1_TYPE *type);

/* These are used by EVP_CIPHER methods */
int EVP_CIPHER_set_asn1_iv(EVP_CIPHER_CTX *c,ASN1_TYPE *type);
int EVP_CIPHER_get_asn1_iv(EVP_CIPHER_CTX *c,ASN1_TYPE *type);

/* PKCS5 password based encryption */
int PKCS5_PBE_keyivgen(EVP_CIPHER_CTX *ctx, const char *pass, int passlen,
			 ASN1_TYPE *param, EVP_CIPHER *cipher, EVP_MD *md,
			 int en_de);
int PKCS5_PBKDF2_HMAC_SHA1(const char *pass, int passlen,
			   UNS8 *salt, int saltlen, int iter,
			   int keylen, UNS8 *out);
int PKCS5_v2_PBE_keyivgen(EVP_CIPHER_CTX *ctx, const char *pass, int passlen,
			 ASN1_TYPE *param, EVP_CIPHER *cipher, EVP_MD *md,
			 int en_de);

VOID PKCS5_PBE_add(VOID);

int EVP_PBE_alg_add(int nid, EVP_CIPHER *cipher, EVP_MD *md,
		    EVP_PBE_KEYGEN *keygen);
VOID EVP_PBE_cleanup(VOID);

/* BEGIN ERROR CODES */
/* The following lines are auto generated by the script mkerr.pl. Any changes
 * made after this point may be overwritten when the script is next run.
 */
VOID ERR_load_EVP_strings(VOID);

/* Error codes for the EVP functions. */

/* Function codes. */
#define EVP_F_D2I_PKEY					 100
#define EVP_F_EVP_CIPHERINIT				 123
#define EVP_F_EVP_CIPHER_CTX_CTRL			 124
#define EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH		 122
#define EVP_F_EVP_DECRYPTFINAL				 101
#define EVP_F_EVP_MD_CTX_COPY				 110
#define EVP_F_EVP_OPENINIT				 102
#define EVP_F_EVP_PBE_ALG_ADD				 115
#define EVP_F_EVP_PBE_CIPHERINIT			 116
#define EVP_F_EVP_PKCS82PKEY				 111
#define EVP_F_EVP_PKCS8_SET_BROKEN			 112
#define EVP_F_EVP_PKEY2PKCS8				 113
#define EVP_F_EVP_PKEY_COPY_PARAMETERS			 103
#define EVP_F_EVP_PKEY_DECRYPT				 104
#define EVP_F_EVP_PKEY_ENCRYPT				 105
#define EVP_F_EVP_PKEY_GET1_DH				 119
#define EVP_F_EVP_PKEY_GET1_DSA				 120
#define EVP_F_EVP_PKEY_GET1_RSA				 121
#define EVP_F_EVP_PKEY_NEW				 106
#define EVP_F_EVP_SIGNFINAL				 107
#define EVP_F_EVP_VERIFYFINAL				 108
#define EVP_F_PKCS5_PBE_KEYIVGEN			 117
#define EVP_F_PKCS5_V2_PBE_KEYIVGEN			 118
#define EVP_F_RC2_MAGIC_TO_METH				 109
#define EVP_F_RC5_CTRL					 125

/* Reason codes. */
#define EVP_R_BAD_DECRYPT				 100
#define EVP_R_BN_DECODE_ERROR				 112
#define EVP_R_BN_PUBKEY_ERROR				 113
#define EVP_R_CIPHER_PARAMETER_ERROR			 122
#define EVP_R_CTRL_NOT_IMPLEMENTED			 132
#define EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED		 133
#define EVP_R_DECODE_ERROR				 114
#define EVP_R_DIFFERENT_KEY_TYPES			 101
#define EVP_R_ENCODE_ERROR				 115
#define EVP_R_EVP_PBE_CIPHERINIT_ERROR			 119
#define EVP_R_EXPECTING_AN_RSA_KEY			 127
#define EVP_R_EXPECTING_A_DH_KEY			 128
#define EVP_R_EXPECTING_A_DSA_KEY			 129
#define EVP_R_INITIALIZATION_ERROR			 134
#define EVP_R_INPUT_NOT_INITIALIZED			 111
#define EVP_R_INVALID_KEY_LENGTH			 130
#define EVP_R_IV_TOO_LARGE				 102
#define EVP_R_KEYGEN_FAILURE				 120
#define EVP_R_MISSING_PARAMETERS			 103
#define EVP_R_NO_CIPHER_SET				 131
#define EVP_R_NO_DSA_PARAMETERS				 116
#define EVP_R_NO_SIGN_FUNCTION_CONFIGURED		 104
#define EVP_R_NO_VERIFY_FUNCTION_CONFIGURED		 105
#define EVP_R_PKCS8_UNKNOWN_BROKEN_TYPE			 117
#define EVP_R_PUBLIC_KEY_NOT_RSA			 106
#define EVP_R_UNKNOWN_PBE_ALGORITHM			 121
#define EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS		 135
#define EVP_R_UNSUPPORTED_CIPHER			 107
#define EVP_R_UNSUPPORTED_KEYLENGTH			 123
#define EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION	 124
#define EVP_R_UNSUPPORTED_KEY_SIZE			 108
#define EVP_R_UNSUPPORTED_PRF				 125
#define EVP_R_UNSUPPORTED_PRIVATE_KEY_ALGORITHM		 118
#define EVP_R_UNSUPPORTED_SALT_TYPE			 126
#define EVP_R_WRONG_FINAL_BLOCK_LENGTH			 109
#define EVP_R_WRONG_PUBLIC_KEY_TYPE			 110

#ifdef  __cplusplus
}
#endif
#endif
