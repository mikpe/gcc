/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Functions for manipulating fifo buffers (that can grow if needed).
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#include "includes.h"
RCSID("$OpenBSD: buffer.c,v 1.13 2001/04/12 19:15:24 markus Exp $");

#include "xmalloc.h"
#include "buffer.h"
#include "log.h"

static VOID memmove_tops20( UNS8 *dst, const UNS8 *src, int n );

/* Initializes the buffer structure. */

VOID
buffer_init(Buffer *buffer)
{
	buffer->alloc = 4096;
	buffer->buf = (UNS8 *)xmalloc(buffer->alloc);
	buffer->offset = 0;
	buffer->end = 0;
}

/* Frees any memory used for the buffer. */

VOID
buffer_free(Buffer *buffer)
{
	UNS8 *b = buffer->buf + buffer->alloc;
	while( b-- != buffer->buf ) *b = 0;
	xfree((LONG*)buffer->buf);
}

/*
 * Clears any data from the buffer, making it empty.  This does not actually
 * zero the memory.
 */

VOID
buffer_clear(Buffer *buffer)
{
	buffer->offset = 0;
	buffer->end = 0;
}

/* Appends data to the buffer, expanding it if necessary. */

VOID
buffer_append(Buffer *buffer, const UNS8 *data, u_int len)
{
	UNS8 *cp;
	buffer_append_space(buffer, &cp, len);
	if ( len > 0 )
	{
	UNS8 *b = cp+len;
	UNS8 *bd = (UNS8 *)(data+len);
	while( b-- != cp ) { bd--; *b = *bd; }
	}
}

/*
 * Appends space to the buffer, expanding the buffer if necessary. This does
 * not actually copy the data into the buffer, but instead returns a pointer
 * to the allocated region.
 */

VOID
buffer_append_space(Buffer *buffer, UNS8 **datap, u_int len)
{
	/* If the buffer is empty, start using it from the beginning. */
	if (buffer->offset == buffer->end) {
		buffer->offset = 0;
		buffer->end = 0;
	}
restart:
	/* If there is enough space to store all data, store it now. */
	if (buffer->end + len < buffer->alloc) {
		*datap = buffer->buf + buffer->end;
		buffer->end += len;
		return;
	}
	/*
	 * If the buffer is quite empty, but all data is at the end, move the
	 * data to the beginning and retry.
	 */
	if (buffer->offset > buffer->alloc / 2) {
		memmove_tops20(buffer->buf, buffer->buf + buffer->offset,
			buffer->end - buffer->offset);
		buffer->end -= buffer->offset;
		buffer->offset = 0;
		goto restart;
	}
	/* Increase the size of the buffer and retry. */
	buffer->alloc += len + 32768;
	buffer->buf = (UNS8 *)xrealloc((LONG*)buffer->buf, buffer->alloc);
	goto restart;
}

/* Returns the number of bytes of data in the buffer. */

u_int
buffer_len(Buffer *buffer)
{
	return buffer->end - buffer->offset;
}

/* Gets data from the beginning of the buffer. */

VOID
buffer_get(Buffer *buffer, UNS8 *buf, u_int len)
{
	if (len > buffer->end - buffer->offset)
		fatal("buffer_get: trying to get more bytes %d than in buffer %d",
		    len, buffer->end - buffer->offset);
	{
	UNS8 *b = buffer->buf + (buffer->offset +len);
	UNS8 *bd = buf+len;
	while( bd-- != buf ) { b--; *bd = *b; }
	}
	buffer->offset += len;
}

/* Consumes the given number of bytes from the beginning of the buffer. */

VOID
buffer_consume(Buffer *buffer, u_int bytes)
{
	if (bytes > buffer->end - buffer->offset)
		fatal("buffer_consume: trying to get more bytes than in buffer");
	buffer->offset += bytes;
}

/* Consumes the given number of bytes from the end of the buffer. */

VOID
buffer_consume_end(Buffer *buffer, u_int bytes)
{
	if (bytes > buffer->end - buffer->offset)
		fatal("buffer_consume_end: trying to get more bytes than in buffer");
	buffer->end -= bytes;
}

/* Returns a pointer to the first used byte in the buffer. */

UNS8 *
buffer_ptr(Buffer *buffer)
{
	return buffer->buf + buffer->offset;
}

/* Dumps the contents of the buffer to stderr. */

VOID
buffer_dump(Buffer *buffer)
{
	int i;
	u_char *ucp = (u_char *) buffer->buf;

	for (i = buffer->offset; i < buffer->end; i++) {
		eprintf("%02x", ucp[i]);
		if ((i-buffer->offset)%16==15)
			eprintf("\r\n");
		else if ((i-buffer->offset)%2==1)
			eprintf(" ");
	}
	eprintf("\r\n");
}

VOID memmove_tops20( UNS8 *dst, const UNS8 *src, int n )
{
	UNS8 *b = (UNS8 *)src;
	UNS8 *bd = dst;
	if ( dst > src )
	{
		b += n;
		bd += n;
		while( bd-- != dst ) { b--; *bd = *b; }
	}
	else
	{
		int i = 0;
		while( i++<n ) { *bd = *b; ++b; ++bd; }
	}
}
