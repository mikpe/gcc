/* crypto/asn1/asn1_mac.h */
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

#ifndef HEADER_ASN1_MAC_H
#define HEADER_ASN1_MAC_H

#include <openssl/asn1.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef ASN1_MAC_ERR_LIB
#define ASN1_MAC_ERR_LIB	ERR_LIB_ASN1
#endif 

#define ASN1_MAC_H_err(f,r,line) \
	ERR_PUT_error(ASN1_MAC_ERR_LIB,(f),(r),ERR_file_name,(line))

#define M_ASN1_D2I_vars(a,type,func) \
	ASN1_CTX c; \
	type ret=NULL; \
	\
	c.pp=pp; \
	c.q= *pp; \
	c.error=ERR_R_NESTED_ASN1_ERROR; \
	if ((a == NULL) || ((*a) == NULL)) \
		{ if ((ret=(type)func()) == NULL) \
			{ c.line=__LINE__; goto err; } } \
	else	ret=(*a);

#define M_ASN1_D2I_Init() \
	c.p= *pp; \
	c.max=(length == 0)?0:(c.p+length);

#define M_ASN1_D2I_Finish_2(a) \
	if (!asn1_Finish(&c)) \
		{ c.line=__LINE__; goto err; } \
	*pp=c.p; \
	if (a != NULL) (*a)=ret; \
	return(ret);

#define M_ASN1_D2I_start_sequence() \
	if (!asn1_GetSequence(&c,&length)) \
		{ c.line=__LINE__; goto err; }


/* Don't use this with d2i_ASN1_BOOLEAN() */
#define M_ASN1_D2I_get(b,func) \
	c.q=c.p; \
	if (func(&(b),&c.p,c.slen) == NULL) \
		{c.line=__LINE__; goto err; } \
	c.slen-=SUBBP(c.p,c.q);





/*************************************************/




int asn1_GetSequence(ASN1_CTX *c, long *length);
#ifdef  __cplusplus
}
#endif

#endif
