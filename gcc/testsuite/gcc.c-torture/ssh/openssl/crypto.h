/* crypto/crypto.h */
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

#ifndef HEADER_CRYPTO_H
#define HEADER_CRYPTO_H

#include "stdlib.h"

#ifndef NO_FP_API
#include "stdio.h"
#endif

//#include "openssl/stack.h"
#include "opensslv.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* Backward compatibility to SSLeay */
/* This is more to be used to check the correct DLL is being used
 * in the MS world. */
#define SSLEAY_VERSION_NUMBER	OPENSSL_VERSION_NUMBER
#define SSLEAY_VERSION		0
/* #define SSLEAY_OPTIONS	1 no longer supported */
#define SSLEAY_CFLAGS		2
#define SSLEAY_BUILT_ON		3
#define SSLEAY_PLATFORM		4

/* Some applications as well as some parts of OpenSSL need to allocate
   and deallocate locks in a dynamic fashion.  The following typedef
   makes this possible in a type-safe manner.  */
/* struct CRYPTO_dynlock_value has to be defined by the application. */
typedef struct
	{
	int references;
	struct CRYPTO_dynlock_value *data;
	} CRYPTO_dynlock;


/* The following can be used to detect memory leaks in the SSLeay library.
 * It used, it turns on malloc checking */

#define CRYPTO_MEM_CHECK_OFF	0x0	/* an enume */
#define CRYPTO_MEM_CHECK_ON	0x1	/* a bit */
#define CRYPTO_MEM_CHECK_ENABLE	0x2	/* a bit */
#define CRYPTO_MEM_CHECK_DISABLE 0x3	/* an enume */

/* predec of the BIO type */
typedef struct bio_st BIO_dummy;

typedef struct crypto_ex_data_st
	{
	int dummy; /* gcc is screwing up this data structure :-( */
	} CRYPTO_EX_DATA;

/* Called when a new object is created */
typedef int CRYPTO_EX_new(VOIDP parent, VOIDP ptr, CRYPTO_EX_DATA *ad,
					int idx, long argl, VOIDP argp);
/* Called when an object is free()ed */
typedef VOID CRYPTO_EX_free(VOIDP parent, VOIDP ptr, CRYPTO_EX_DATA *ad,
					int idx, long argl, VOIDP argp);
/* Called when we need to dup an object */
typedef int CRYPTO_EX_dup(CRYPTO_EX_DATA *to, CRYPTO_EX_DATA *from, VOIDP from_d, 
					int idx, long argl, VOIDP argp);

/* This stuff is basically class callback functions
 * The current classes are SSL_CTX, SSL, SSL_SESSION, and a few more */

typedef struct crypto_ex_data_func_st
	{
	long argl;	/* Arbitary long */
	VOIDP argp;	/* Arbitary VOIDP  */
	CRYPTO_EX_new *new_func;
	CRYPTO_EX_free *free_func;
	CRYPTO_EX_dup *dup_func;
	} CRYPTO_EX_DATA_FUNCS;


/* Per class, we have a STACK of CRYPTO_EX_DATA_FUNCS for each CRYPTO_EX_DATA
 * entry.
 */

#define CRYPTO_EX_INDEX_BIO		0
#define CRYPTO_EX_INDEX_SSL		1
#define CRYPTO_EX_INDEX_SSL_CTX		2
#define CRYPTO_EX_INDEX_SSL_SESSION	3
#define CRYPTO_EX_INDEX_X509_STORE	4
#define CRYPTO_EX_INDEX_X509_STORE_CTX	5


/* This is the default callbacks, but we can have others as well:
 * this is needed in Win32 where the application malloc and the
 * library malloc may not be the same.
 */

#define OPENSSL_malloc(num)	CRYPTO_malloc((int)num,__FILE__,__LINE__)
#define OPENSSL_realloc(addr,num) \
	CRYPTO_realloc((LONG*)addr,(int)num,__FILE__,__LINE__)
#define OPENSSL_free(addr)	CRYPTO_free((LONG*)addr)




long *CRYPTO_malloc(int num, const char *file, int line);
VOID  CRYPTO_free(LONG*);
long *CRYPTO_realloc(LONG* addr,int num, const char *file, int line);


/* unsigned long order, char *file, int line, int num_bytes, char *addr */

/* BEGIN ERROR CODES */
/* The following lines are auto generated by the script mkerr.pl. Any changes
 * made after this point may be overwritten when the script is next run.
 */
/* Error codes for the CRYPTO functions. */

/* Function codes. */
#define CRYPTO_F_CRYPTO_GET_NEW_DYNLOCKID		 103
#define CRYPTO_F_CRYPTO_GET_NEW_LOCKID			 101
#define CRYPTO_F_CRYPTO_SET_EX_DATA			 102

/* Reason codes. */
#define CRYPTO_R_NO_DYNLOCK_CREATE_CALLBACK		 100

#ifdef  __cplusplus
}
#endif
#endif
