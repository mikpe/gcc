/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Interface for the packet protocol functions.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

/* RCSID("$OpenBSD: packet.h,v 1.26 2001/11/07 16:03:17 markus Exp $"); */

#ifndef PACKET_H
#define PACKET_H

#include "openssl/bn.h"

VOID     packet_set_connection(int, int);
VOID     packet_set_nonblocking(VOID);
int      packet_get_connection_in(VOID);
int      packet_get_connection_out(VOID);
VOID     packet_close(VOID);
VOID     packet_set_interactive(int);

VOID     packet_start(int);
VOID     packet_put_char(int ch);
VOID     packet_put_int(u_int value);
VOID     packet_put_bignum2(BIGNUM * value);
VOID     packet_put_string(const UNS8 *buf, u_int len);
VOID     packet_put_cstring(const UNS8 *str);
VOID     packet_put_raw(const UNS8 *buf, u_int len);
VOID     packet_send(VOID);

int      packet_read(int *payload_len_ptr);
VOID     packet_read_expect(int *payload_len_ptr, int type);
int      packet_read_poll(int *packet_len_ptr);
VOID     packet_process_incoming(const UNS8 *buf, u_int len);

u_int	 packet_get_char(VOID);
u_int	 packet_get_int(VOID);
VOID     packet_get_bignum2(BIGNUM * value, int *length_ptr);
UNS8	*packet_get_raw(int *length_ptr);
UNS8    *packet_get_string(u_int *length_ptr);
VOID     packet_disconnect(const char *fmt,...) __attribute__((format(printf, 1, 2)));
VOID     packet_send_debug(const char *fmt,...) __attribute__((format(printf, 1, 2)));

VOID     packet_write_poll(VOID);
VOID     packet_write_wait(VOID);
int      packet_have_data_to_write(VOID);
int      packet_not_very_much_data_to_write(VOID);

int	 packet_connection_is_on_socket(VOID);
int	 packet_connection_is_ipv4(VOID);
int	 packet_remaining(VOID);
VOID	 packet_send_ignore(int);

#define packet_integrity_check(payload_len, expected_len, type) \
do { \
	int _p = (payload_len), _e = (expected_len); \
	if (_p != _e) { \
		log("Packet integrity error (%d != %d) at %s:%d", \
		    _p, _e, __FILE__, __LINE__); \
		packet_disconnect("Packet integrity error. (%d)", (type)); \
	} \
} while (0)

#define packet_done() \
do { \
	int _len = packet_remaining(); \
	if (_len > 0) { \
		log("Packet integrity error (%d bytes remaining) at %s:%d", \
		    _len ,__FILE__, __LINE__); \
		packet_disconnect("Packet integrity error."); \
	} \
} while (0)

#endif				/* PACKET_H */
