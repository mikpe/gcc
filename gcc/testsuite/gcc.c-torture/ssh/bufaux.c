/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Auxiliary functions for storing and retrieving various data types to/from
 * Buffers.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 *
 * SSH2 packet format added by Markus Friedl
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
RCSID("$OpenBSD: bufaux.c,v 1.17 2001/01/21 19:05:45 markus Exp $");

#include "openssl/bn.h"
#include "bufaux.h"
#include "xmalloc.h"
#include "getput.h"
#include "log.h"

/*
 * Stores an BIGNUM in the buffer in SSH2 format.
 */
VOID
buffer_put_bignum2(Buffer *buffer, BIGNUM *value)
{
#ifdef TOPS_PORT
	UNS8 *uc;
	UNS8 *buf; 
#else
	u_char *uc;
	u_char *buf; 
#endif
	int oi;
	int hasnohigh;
	int i, carry;
	int bytes; 

	bytes = BN_num_bytes(value) + 1;
#ifdef TOPS_PORT
	buf = (UNS8 *)xmalloc(bytes);
#else
        buf = (u_char *)xmalloc(bytes);
#endif
	buf[0] = '\0';
	hasnohigh = 0;

	/* Get the value of in binary */
	oi = BN_bn2bin(value, buf+1);
	if (oi != bytes-1)
		fatal("buffer_put_bignum: BN_bn2bin() failed: oi %d != bin_size %d",
		      oi, bytes);
	if ((buf[1] & 0x80)==0) hasnohigh++;

	if (value->neg) {
		/**XXX should be two's-complement */
		log("negativ!");
		uc = buf;
		for(i = bytes-1, carry = 0x100; i>=0; i--) {
			uc[i] ^= 0xff;
			if(carry) {
				carry=uc[i];
				carry++;
				uc[i]=(UNS8)carry;
				carry &= 0x100;
				/* carry = !++uc[i]; */
			}
		}
	}
   /* JOHN BORCHEK FIX: questionable pointer math, may not port? */
	buffer_put_string(buffer, buf+hasnohigh, bytes-hasnohigh);
	memset(buf, 0, bytes);
	xfree((LONG*)buf);
}

int
buffer_get_bignum2(Buffer *buffer, BIGNUM *value)
{
	/**XXX should be two's-complement */
	int len;
#ifdef TOPS_PORT
	UNS8 *bin = (UNS8 *)buffer_get_string(buffer, (u_int *)&len);
#else
	u_char *bin = (u_char *)buffer_get_string(buffer, (u_int *)&len);
#endif
	BN_bin2bn(bin, len, value);
	xfree((LONG*)bin);
	return len;
}

/*
 * Returns an integer from the buffer (4 bytes, msb first).
 */
u_int
buffer_get_int(Buffer *buffer)
{
#ifdef TOPS_PORT
	UNS8 buf[4];
	buffer_get(buffer,  buf, 4);
#else
	u_char buf[4];
	buffer_get(buffer, (char *) buf, 4);
#endif
	return GET_32BIT(buf);
}

/*
 * Stores an integer in the buffer in 4 bytes, msb first.
 */
VOID
buffer_put_int(Buffer *buffer, u_int value)
{
#ifdef TOPS_PORT
	UNS8 buf[4];
#else
	char buf[4];
#endif
	PUT_32BIT(buf, value);
	buffer_append(buffer, buf, 4);
}

/*
 * Returns an arbitrary binary string from the buffer.  The string cannot
 * be longer than 256k.  The returned value points to memory allocated
 * with xmalloc; it is the responsibility of the calling function to free
 * the data.  If length_ptr is non-NULL, the length of the returned data
 * will be stored there.  A null character will be automatically appended
 * to the returned string, and is not counted in length.
 */
#ifdef TOPS_PORT
UNS8 *
#else
char *
#endif
buffer_get_string(Buffer *buffer, u_int *length_ptr)
{
	u_int len;
#ifdef TOPS_PORT
	UNS8 *value;
#else
	char *value;
#endif
	/* Get the length. */
	len = buffer_get_int(buffer);
	if (len > 256 * 1024)
		fatal("Received packet with bad string length %d", len);
	/* Allocate space for the string.  Add one byte for a null character. */
	value = (char *)xmalloc(len + 1);
	/* Get the string. */
	buffer_get(buffer, value, len);
	/* Append a null character to make processing easier. */
	value[len] = 0;
	/* Optionally return the length of the string. */
	if (length_ptr)
		*length_ptr = len;
	return value;
}

/*
 * Stores and arbitrary binary string in the buffer.
 */
VOID
buffer_put_string(Buffer *buffer, const UNS8 *buf, u_int len)
{
	buffer_put_int(buffer, len);
	buffer_append(buffer, buf, len);
}
#ifdef TOPS_PORT
VOID	buffer_put_cstring(Buffer *buffer, const UNS8 *s)
{
	int L = strlen(s);
	buffer_put_string(buffer, s, L);
}
#else
VOID
buffer_put_cstring(Buffer *buffer, const char *s)
{
	buffer_put_string(buffer, s, strlen(s));
}
#endif

/*
 * Returns a character from the buffer (0 - 255).
 */
int
buffer_get_char(Buffer *buffer)
{
	UNS8 ch[4];
	buffer_get(buffer, &ch[0], 1);
	return (u_char) ch[0];
}

/*
 * Stores a character in the buffer.
 */
VOID
buffer_put_char(Buffer *buffer, int value)
{
	UNS8 ch[4];
	ch[0] = value;
	buffer_append(buffer, &ch[0], 1);
}
