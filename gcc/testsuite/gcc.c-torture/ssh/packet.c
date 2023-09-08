/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This file contains code implementing the packet protocol and communication
 * with the other side.  This same code is used both on client and server side.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 *
 * SSH2 packet format added by Markus Friedl.
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

#include "includes.h"
RCSID("$OpenBSD: packet.c,v 1.72 2001/11/10 13:37:20 markus Exp $");

#include "xmalloc.h"
#include "buffer.h"
#include "packet.h"
#include "bufaux.h"
#include "getput.h"

#include "channels.h"

#include "compat.h"
#include "ssh1.h"
#include "ssh2.h"

#include "cipher.h"
#include "kex.h"
#include "mac.h"
#include "log.h"
#include "canohost.h"
#include "addmore.h"

#ifdef PACKET_DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif

/*
 * This variable contains the file descriptors used for communicating with
 * the other side.  connection_in is used for reading; connection_out for
 * writing.  These can be the same descriptor, in which case it is assumed to
 * be a socket.
 */
static int connection_in = -1;
static int connection_out = -1;

/* Encryption context for receiving data.  This is only used for decryption. */
static CipherContext receive_context;

/* Encryption context for sending data.  This is only used for encryption. */
static CipherContext send_context;

/* Buffer for raw input data from the socket. */
static Buffer input;

/* Buffer for raw output data going to the socket. */
static Buffer output;

/* Buffer for the partial outgoing packet being constructed. */
static Buffer outgoing_packet;

/* Buffer for the incoming packet currently being processed. */
static Buffer incoming_packet;

/* Flag indicating whether this module has been initialized. */
static int initialized = 0;

/* Set to true if the connection is interactive. */
static int interactive_mode = 0;

/* Session key information for Encryption and MAC */
Newkeys *newkeys[MODE_MAX];

/* roundup current message to extra_pad bytes */
static int extra_pad = 0;

/*
 * Sets the descriptors used for communication.  Disables encryption until
 * packet_set_encryption_key is called.
 */
VOID
packet_set_connection(int fd_in, int fd_out)
{
	Cipher *none = cipher_by_name("none");
	if (none == NULL)
		fatal("packet_set_connection: cannot load cipher 'none'");
	connection_in = fd_in;
	connection_out = fd_out;
	cipher_init(&send_context, none, (u_char *) "", 0, NULL, 0);
	cipher_init(&receive_context, none, (u_char *) "", 0, NULL, 0);
	newkeys[MODE_IN] = newkeys[MODE_OUT] = NULL;
	if (!initialized) {
		initialized = 1;
		buffer_init(&input);
		buffer_init(&output);
		buffer_init(&outgoing_packet);
		buffer_init(&incoming_packet);
	}
	/* Kludge: arrange the close function to be called from fatal(). */
	fatal_add_cleanup((VOID (*) (VOIDP)) packet_close, NULL);
}
/* returns 1 if connection is via ipv4 */

int
packet_connection_is_ipv4()
{
  return 1;
}

/* Sets the connection into non-blocking mode. */

VOID
packet_set_nonblocking()
{
	/* Set the socket into non-blocking mode. */
	if (fcntl(connection_in, F_SETFL, O_NONBLOCK) < 0)
		error("fcntl O_NONBLOCK: %.100s", strerror(errno));

	if (connection_out != connection_in) {
		if (fcntl(connection_out, F_SETFL, O_NONBLOCK) < 0)
			error("fcntl O_NONBLOCK: %.100s", strerror(errno));
	}
}

/* Returns the socket used for reading. */

int
packet_get_connection_in()
{
	return connection_in;
}

/* Returns the descriptor used for writing. */

int
packet_get_connection_out()
{
	return connection_out;
}

/* Closes the connection and clears and frees internal data structures. */

VOID
packet_close()
{
	if (!initialized)
		return;
	initialized = 0;
	if (connection_in == connection_out) {
		shutdown(connection_out, SHUT_RDWR);
		closesocket(connection_out);
	} else {
		closesocket(connection_in);
		closesocket(connection_out);
	}
	buffer_free(&input);
	buffer_free(&output);
	buffer_free(&outgoing_packet);
	buffer_free(&incoming_packet);
}

/* Start constructing a packet to send. */
VOID
packet_start(int type)
{
	UNS8 buf[9];
	int len;

	DBG(debug("packet_start[%d]", type));
	len = 6;
	memset(buf, 0, len - 1);
	buf[len - 1] = type;
	buffer_clear(&outgoing_packet);
	buffer_append(&outgoing_packet, buf, len);
}

/* Append payload. */
VOID
packet_put_char(int value)
{
	UNS8 ch[4];
	ch[0] = value;
	buffer_append(&outgoing_packet, &ch[0], 1);
}
VOID
packet_put_int(u_int value)
{
	buffer_put_int(&outgoing_packet, value);
}
VOID
packet_put_string(const UNS8 *buf, u_int len)
{
	buffer_put_string(&outgoing_packet, buf, len);
}
VOID
packet_put_cstring(const UNS8 *str)
{
	buffer_put_cstring(&outgoing_packet, str);
}
VOID
packet_put_raw(const UNS8 *buf, u_int len)
{
	buffer_append(&outgoing_packet, buf, len);
}
VOID
packet_put_bignum2(BIGNUM * value)
{
	buffer_put_bignum2(&outgoing_packet, value);
}

static VOID
set_newkeys(int mode)
{
	Enc *enc;
	Mac *mac;
	Comp *comp;
	CipherContext *cc;

	debug("newkeys: mode %d", mode);

	cc = (mode == MODE_OUT) ? &send_context : &receive_context;
	if (newkeys[mode] != NULL) {
		debug("newkeys: rekeying");
		/* todo: free old keys, reset compression/cipher-ctxt; */
		memset(cc, 0, sizeof(*cc));
		enc  = &newkeys[mode]->enc;
		mac  = &newkeys[mode]->mac;
		comp = &newkeys[mode]->comp;
		memset(mac->key, 0, mac->key_len);
		xfree((LONG*)enc->name);
		xfree((LONG*)enc->iv);
		xfree((LONG*)enc->key);
		xfree((LONG*)mac->name);
		xfree((LONG*)mac->key);
		xfree((LONG*)comp->name);
		xfree((LONG*)newkeys[mode]);
	}
	newkeys[mode] = kex_get_newkeys(mode);
	if (newkeys[mode] == NULL)
		fatal("newkeys: no keys for mode %d", mode);
	enc  = &newkeys[mode]->enc;
	mac  = &newkeys[mode]->mac;
	comp = &newkeys[mode]->comp;
	if (mac->md != NULL)
		mac->enabled = 1;
	DBG(debug("cipher_init_context: %d", mode));
	cipher_init(cc, enc->cipher, enc->key, enc->cipher->key_len,
	    enc->iv, enc->cipher->block_size);
	memset(enc->iv,  0, enc->cipher->block_size);
	memset(enc->key, 0, enc->cipher->key_len);

}

/*
 * Finalize packet in SSH2 format (compress, mac, encrypt, enqueue)
 */
static VOID
packet_send2(VOID)
{
	static u_int32_t sqnr = 0;
	unsigned int type;
   	UNS8 *ucp, *macbuf = NULL;
	unsigned int padlen, pad;
	UNS8 *cp;
	u_int packet_length = 0;
	u_int i, len;
	u_int32_t rand = 0;
	Enc *enc   = NULL;
	Mac *mac   = NULL;
	Comp *comp = NULL;
	int block_size;

	if (newkeys[MODE_OUT] != NULL) {
		enc  = &newkeys[MODE_OUT]->enc;
		mac  = &newkeys[MODE_OUT]->mac;
		comp = &newkeys[MODE_OUT]->comp;
	}
	block_size = enc ? enc->cipher->block_size : 8;

	ucp = (UNS8 *) buffer_ptr(&outgoing_packet);
	type = ucp[5];

#ifdef PACKET_DEBUG
	eprintf("plain:     ");
	buffer_dump(&outgoing_packet);
#endif

	/* sizeof (packet_len + pad_len + payload) */
	len = buffer_len(&outgoing_packet);

	/*
	 * calc size of padding, alloc space, get random data,
	 * minimum padding is 4 bytes
	 */
	padlen = block_size - (UMOD(len,block_size));
	if (padlen < 4)
		padlen += block_size;
	if (extra_pad) {
		/* will wrap if extra_pad+padlen > 255 */
	  extra_pad  = roundup((int)extra_pad, block_size) & 0xFF;
	  pad = extra_pad - (UMOD((len + padlen),extra_pad));
	  debug("packet_send2: adding %d (len %d padlen %d extra_pad %d)",
		pad, len, padlen, extra_pad);
	  padlen += pad;
	  extra_pad = 0;
	}
	buffer_append_space(&outgoing_packet, &cp, padlen);
	if (enc && enc->cipher->number != SSH_CIPHER_NONE) {
		/* random padding */
		for (i = 0; i < padlen; i++) {
			if ((i & 3) == 0)
				rand = arc4random();
			cp[i] = rand & 0xff;
			rand >>= 8;
		}
	} else {
		/* clear padding */
		memset(cp, 0, padlen);
	}
	/* packet_length includes payload, padding and padding length field */
	packet_length = buffer_len(&outgoing_packet) - 4;
	ucp = (UNS8 *)buffer_ptr(&outgoing_packet);
	PUT_32BIT(ucp, packet_length);
	ucp[4] = padlen;
	DBG(debug("send: len %d (includes padlen %d)", packet_length+4, padlen));

	/* compute MAC over sqnr and packet(length fields, payload, padding) */
	if (mac && mac->enabled) {
		macbuf = mac_compute(mac, sqnr,
		    (UNS8 *) buffer_ptr(&outgoing_packet),
		    buffer_len(&outgoing_packet));
		DBG(debug("done calc MAC out #%d", sqnr));
	}
	/* encrypt packet and append to output buffer. */
	buffer_append_space(&output, &cp, buffer_len(&outgoing_packet));
	cipher_encrypt(&send_context, cp, buffer_ptr(&outgoing_packet),
	    buffer_len(&outgoing_packet));
	/* append unencrypted MAC */
	if (mac && mac->enabled)
		buffer_append(&output, (char *)macbuf, mac->mac_len);
#ifdef PACKET_DEBUG
	eprintf("encrypted: ");
	buffer_dump(&output);
#endif
	/* increment sequence number for outgoing packets */
	if (++sqnr == 0)
		log("outgoing sqnr wraps around");
	buffer_clear(&outgoing_packet);

	if (type == SSH2_MSG_NEWKEYS)
		set_newkeys(MODE_OUT);
}

VOID
packet_send()
{
	packet_send2();
	DBG(debug("packet_send done"));
#ifdef TOPS20
	/* hack introduced for tops20 since for now we have
	 * blocking sends and recvs. So everytime we queue
	 * something, send it out - sumit, 2/12
	 */
	packet_write_poll();
#endif
}

/*
 * Waits until a packet has been received, and returns its type.  Note that
 * no other data is processed until this returns, so this function should not
 * be used during the interactive session.
 */

int
packet_read(int *payload_len_ptr)
{
	int type, len;
	fd_set *setp;
	UNS8 buf[8192];
	DBG(debug("packet_read()"));

	setp = (fd_set *)xmalloc(howmany(connection_in+1, NFDBITS) *
	    sizeof(fd_mask));

	/* Since we are blocking, ensure that all written packets have been sent. */
	packet_write_wait();

	/* Stay in the loop until we have received a complete packet. */
	for (;;) {
		/* Try to read a packet from the buffer. */
		type = packet_read_poll(payload_len_ptr);
		/* If we got a packet, return it. */
		if (type != SSH_MSG_NONE) {
			xfree((LONG*)setp);
			return type;
		}
		/*
		 * Otherwise, wait for some data to arrive, add it to the
		 * buffer, and try again.
		 */
		memset(setp, 0, howmany(connection_in + 1, NFDBITS) *
		    sizeof(fd_mask));
		FD_SET(connection_in, setp);

		/* Wait for some data to arrive. */
		while (select(connection_in + 1, setp, NULL, NULL, NULL) < 0 &&
		    (errno == EAGAIN || errno == EINTR))
			;

		/* Read data from the socket. */
		len = recv(connection_in, buf, sizeof(buf),0);
		if (len == 0) {
		  log("Connection closed by %.200s", get_remote_ipaddr());
		  fatal_cleanup();
		}
		if (len < 0)
			fatal("Read from socket failed: %.100s", strerror(errno));
		/* Append it to the buffer. */
		packet_process_incoming(buf, len);
	}
	/* NOTREACHED */
}

/*
 * Waits until a packet has been received, verifies that its type matches
 * that given, and gives a fatal error and exits if there is a mismatch.
 */

VOID
packet_read_expect(int *payload_len_ptr, int expected_type)
{
	int type;

	type = packet_read(payload_len_ptr);
	if (type != expected_type)
		packet_disconnect("Protocol error: expected packet type %d, got %d",
		    expected_type, type);
}

/* Checks if a full packet is available in the data received so far via
 * packet_process_incoming.  If so, reads the packet; otherwise returns
 * SSH_MSG_NONE.  This does not wait for data from the connection.
 *
 * SSH_MSG_DISCONNECT is handled specially here.  Also,
 * SSH_MSG_IGNORE messages are skipped by this function and are never returned
 * to higher levels.
 *
 * The returned payload_len does include space consumed by:
 * 	Packet length
 * 	Padding
 * 	Packet type
 * 	Check bytes
 */

static int
packet_read_poll2(int *payload_len_ptr)
{
	static u_int32_t sqnr = 0;
	static u_int packet_length = 0;
	u_int padlen, need;
	UNS8 *macbuf, *ucp;
    unsigned int type;
	UNS8 *cp;
	int maclen, block_size;
	Enc *enc   = NULL;
	Mac *mac   = NULL;
	Comp *comp = NULL;

	if (newkeys[MODE_IN] != NULL) {
		enc  = &newkeys[MODE_IN]->enc;
		mac  = &newkeys[MODE_IN]->mac;
		comp = &newkeys[MODE_IN]->comp;
	}
	maclen = mac && mac->enabled ? mac->mac_len : 0;
	block_size = enc ? enc->cipher->block_size : 8;

	if (packet_length == 0) {
		/*
		 * check if input size is less than the cipher block size,
		 * decrypt first block and extract length of incoming packet
		 */
		if (buffer_len(&input) < block_size)
			return SSH_MSG_NONE;
		buffer_clear(&incoming_packet);
		buffer_append_space(&incoming_packet, &cp, block_size);
		cipher_decrypt(&receive_context, cp, buffer_ptr(&input),
		    block_size);
		ucp = (u_char *) buffer_ptr(&incoming_packet);
		packet_length = GET_32BIT(ucp);
		/*--------------------------------------------------
		* printf("packet_read_poll2: packet length %d\r\n", 
		*        packet_length);
		* printf("packet_read_poll2: printing buffer input\r\n");
		* buffer_dump(&input);
		* printf("packet_read_poll2: printing buffer incoming\r\n");
		* buffer_dump(&incoming_packet);
		*--------------------------------------------------*/
		if (packet_length < 1 + 4 || packet_length > 256 * 1024) {
			buffer_dump(&incoming_packet);
			packet_disconnect("Bad packet length %d.", packet_length);
		}
		DBG(debug("input: packet len %d", packet_length+4));
		buffer_consume(&input, block_size);
	}
	/* we have a partial packet of block_size bytes */
	need = 4 + packet_length - block_size;
	DBG(debug("partial packet %d, need %d, maclen %d", block_size,
	    need, maclen));
	if (UMOD(need,block_size) != 0)
		fatal("padding error: need %d block %d mod %d",
		    need, block_size, UMOD(need,block_size));
	/*
	 * check if the entire packet has been received and
	 * decrypt into incoming_packet
	 */
	if (buffer_len(&input) < need + maclen)
		return SSH_MSG_NONE;
#ifdef PACKET_DEBUG
	eprintf("read_poll enc/full: ");
	buffer_dump(&input);
#endif
	buffer_append_space(&incoming_packet, &cp, need);
	cipher_decrypt(&receive_context, cp, buffer_ptr(&input), need);
	buffer_consume(&input, need);
	/*
	 * compute MAC over sqnr and packet,
	 * increment sequence number for incoming packet
	 */
	if (mac && mac->enabled) {
		macbuf = mac_compute(mac, sqnr,
		    (UNS8 *) buffer_ptr(&incoming_packet),
		    buffer_len(&incoming_packet));
		if (memcmp(macbuf, buffer_ptr(&input), mac->mac_len) != 0)
			packet_disconnect("Corrupted MAC on input.");
		DBG(debug("MAC #%d ok", sqnr));
		buffer_consume(&input, mac->mac_len);
	}
	if (++sqnr == 0)
		log("incoming sqnr wraps around");

	/* get padlen */
	cp = buffer_ptr(&incoming_packet) + 4;
#if 0
	/* a bug ?  padlen is declared u_int */
	padlen = (u_char) *cp;
#else
	padlen = (u_int) *cp;
#endif
	DBG(debug("input: padlen %d", padlen));
	if (padlen < 4)
		packet_disconnect("Corrupted padlen %d on input.", padlen);

	/* skip packet size + padlen, discard padding */
	buffer_consume(&incoming_packet, 4 + 1);
	buffer_consume_end(&incoming_packet, padlen);

	DBG(debug("input: len before de-compress %d", buffer_len(&incoming_packet)));
	/*
	 * get packet type, implies consume.
	 * return length of payload (without type field)
	 */
	type = buffer_get_char(&incoming_packet);
	if (type == SSH2_MSG_NEWKEYS)
		set_newkeys(MODE_IN);
	*payload_len_ptr = buffer_len(&incoming_packet);
#ifdef PACKET_DEBUG
	eprintf("read/plain[%d]:\r\n", type);
	buffer_dump(&incoming_packet);
#endif
	/* reset for next packet */
	packet_length = 0;
	return type;
}

int
packet_read_poll(int *payload_len_ptr)
{
	int reason;
	u_char type;
	char *msg;

	for (;;) {
      type = packet_read_poll2(payload_len_ptr);
      if (type)
         DBG(debug("received packet type %d", type));
      switch(type) {
      case SSH2_MSG_IGNORE:
         break;
      case SSH2_MSG_DEBUG:
         packet_get_char();
         msg = packet_get_string(NULL);
         debug("Remote: %.900s", msg);
         xfree((LONG*)msg);
         msg = packet_get_string(NULL);
         xfree((LONG*)msg);
         break;
      case SSH2_MSG_DISCONNECT:
         reason = packet_get_int();
         msg = packet_get_string(NULL);
         log("Received disconnect from %s: %d: %.400s", get_remote_ipaddr(),
            reason, msg);
         xfree((LONG*)msg);
         fatal_cleanup();
         break;
      default:
         return type;
         break;
      }
	}
}

/*
 * Buffers the given amount of input characters.  This is intended to be used
 * together with packet_read_poll.
 */

VOID
packet_process_incoming(const UNS8 *buf, u_int len)
{
	buffer_append(&input, buf, len);
}

/* Returns a character from the packet. */

u_int
packet_get_char()
{
	UNS8 ch[4];
	buffer_get(&incoming_packet, &ch[0], 1);
	return (UNS8)ch[0];
}

/* Returns an integer from the packet data. */

u_int
packet_get_int()
{
	return buffer_get_int(&incoming_packet);
}

/*
 * Returns an arbitrary precision integer from the packet data.  The integer
 * must have been initialized before this call.
 */

VOID
packet_get_bignum2(BIGNUM * value, int *length_ptr)
{
	*length_ptr = buffer_get_bignum2(&incoming_packet, value);
}

UNS8 *
packet_get_raw(int *length_ptr)
{
	int bytes = buffer_len(&incoming_packet);
	if (length_ptr != NULL)
		*length_ptr = bytes;
	return buffer_ptr(&incoming_packet);
}

int
packet_remaining(VOID)
{
	return buffer_len(&incoming_packet);
}

/*
 * Returns a string from the packet data.  The string is allocated using
 * xmalloc; it is the responsibility of the calling program to free it when
 * no longer needed.  The length_ptr argument may be NULL, or point to an
 * integer into which the length of the string is stored.
 */

UNS8 *
packet_get_string(u_int *length_ptr)
{
	return buffer_get_string(&incoming_packet, length_ptr);
}

/*
 * Sends a diagnostic message from the server to the client.  This message
 * can be sent at any time (but not while constructing another message). The
 * message is printed immediately, but only if the client is being executed
 * in verbose mode.  These messages are primarily intended to ease debugging
 * authentication problems.   The length of the formatted message must not
 * exceed 1024 bytes.  This will automatically call packet_write_wait.
 */

VOID
packet_send_debug(const char *fmt,...)
{
#ifdef TOPS_PORT
	packet_start(SSH2_MSG_DEBUG);
	packet_put_char(0);	/* bool: always display */
	packet_put_cstring((UNS8 *)"debug message");
	packet_put_cstring((UNS8 *)"");
#else
	char buf[1024];
	va_list args;

	if (datafellows & SSH_BUG_DEBUG)
		return;

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

   packet_start(SSH2_MSG_DEBUG);
   packet_put_char(0);	/* bool: always display */
   packet_put_cstring(buf);
   packet_put_cstring("");
#endif
	packet_send();
	packet_write_wait();
}

/*
 * Logs the error plus constructs and sends a disconnect packet, closes the
 * connection, and exits.  This function never returns. The error message
 * should not contain a newline.  The length of the formatted message must
 * not exceed 1024 bytes.
 */

VOID
packet_disconnect(const char *fmt,...)
{
	UNS8 *message = "Must disconnect";
	char buf[1024];
	static int disconnecting = 0;
	if (disconnecting)	/* Guard against recursive invocations. */
		fatal("packet_disconnect called recursively.");
	disconnecting = 1;

	/*
	 * Format the message.  Note that the caller must make sure the
	 * message is of limited size.
	 */
	buf[0]='\0';
#ifndef TOPS_PORT
	{
	  va_list args;
	  va_start(args, fmt);
	  vsnprintf(buf, sizeof(buf), fmt, args);
	  va_end(args);
	}
#endif
	/* Send the disconnect message to the other side, and wait for it to get sent. */
   packet_start(SSH2_MSG_DISCONNECT);
   packet_put_int(SSH2_DISCONNECT_PROTOCOL_ERROR);
   packet_put_cstring(message);
   packet_put_cstring((UNS8 *)"");
	packet_send();
	packet_write_wait();

	/* Stop listening for connections. */
	channel_close_all();

	/* Close the connection. */
	packet_close();

	/* Display the error locally and exit. */
	log("Disconnecting: %.100s", buf);
	fatal_cleanup();
}

/* Checks if there is any buffered output, and tries to write some of the output. */

VOID
packet_write_poll()
{
	int len = buffer_len(&output);
	if (len > 0) {
		len = send(connection_out, buffer_ptr(&output), len,0);
		if (len <= 0) {
			if (errno == EAGAIN)
				return;
			else
				fatal("Write failed: %.100s", strerror(errno));
		}
		buffer_consume(&output, len);
	}
}

/*
 * Calls packet_write_poll repeatedly until all pending output data has been
 * written.
 */

VOID
packet_write_wait()
{
	fd_set *setp;

	setp = (fd_set *)xmalloc(howmany(connection_out + 1, NFDBITS) *
	    sizeof(fd_mask));
	packet_write_poll();
	while (packet_have_data_to_write()) {
		memset(setp, 0, howmany(connection_out + 1, NFDBITS) *
		    sizeof(fd_mask));
		FD_SET(connection_out, setp);
		while (select(connection_out + 1, NULL, setp, NULL, NULL) < 0 &&
		    (errno == EAGAIN || errno == EINTR))
			;
		packet_write_poll();
	}
	xfree((LONG*)setp);
}

/* Returns true if there is buffered data to write to the connection. */

int
packet_have_data_to_write()
{
	return buffer_len(&output) != 0;
}

/* Returns true if there is not too much data to write to the connection. */

int
packet_not_very_much_data_to_write()
{
	if (interactive_mode)
		return buffer_len(&output) < 16384;
	else
		return buffer_len(&output) < 128 * 1024;
}

/* Informs that the current session is interactive.  Sets IP flags for that. */

VOID
packet_set_interactive(int interactive)
{
  /* Record that we are in interactive mode. */
  interactive_mode = interactive;
}

/*
 * 9.2.  Ignored Data Message
 *
 *   byte      SSH_MSG_IGNORE
 *   string    data
 *
 * All implementations MUST understand (and ignore) this message at any
 * time (after receiving the protocol version). No implementation is
 * required to send them. This message can be used as an additional
 * protection measure against advanced traffic analysis techniques.
 */
VOID
packet_send_ignore(int nbytes)
{
	u_int32_t rand = 0;
	int i;

	packet_start(SSH2_MSG_IGNORE);
	packet_put_int(nbytes);
	for(i = 0; i < nbytes; i++) {
		if ((i & 3) == 0)
			rand = arc4random();
		packet_put_char(rand & 0xff);
		rand >>= 8;
	}
}
