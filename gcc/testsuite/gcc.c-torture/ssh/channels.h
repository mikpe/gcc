/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */
/*
 * Copyright (c) 1999, 2000, 2001 Markus Friedl.  All rights reserved.
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
/* RCSID("$OpenBSD: channels.h,v 1.51 2001/11/07 22:53:21 markus Exp $"); */

#ifndef CHANNEL_H
#define CHANNEL_H

#include "buffer.h"

/* Definitions for channel types. */
#define SSH_CHANNEL_PORT_LISTENER	2	/* Listening on a port. */
#define SSH_CHANNEL_OPENING		3	/* waiting for confirmation */
#define SSH_CHANNEL_OPEN		4	/* normal open two-way channel */
#define SSH_CHANNEL_CLOSED		5	/* waiting for close confirmation */
#define SSH_CHANNEL_AUTH_SOCKET		6	/* authentication socket */
#define SSH_CHANNEL_INPUT_DRAINING	8	/* sending remaining data to conn */
#define SSH_CHANNEL_OUTPUT_DRAINING	9	/* sending remaining data to app */
#define SSH_CHANNEL_LARVAL		10	/* larval session */
#define SSH_CHANNEL_RPORT_LISTENER	11	/* Listening to a R-style port  */
#define SSH_CHANNEL_CONNECTING		12
#define SSH_CHANNEL_DYNAMIC		13
#define SSH_CHANNEL_ZOMBIE		14	/* Almost dead. */
#define SSH_CHANNEL_MAX_TYPE		15

#define SSH_CHANNEL_PATH_LEN		256

struct Channel;
typedef struct Channel Channel;

typedef VOID channel_callback_fn(int, VOIDP);
typedef int channel_filter_fn(struct Channel *, VOIDP, int);

struct Channel {
	int     type;		/* channel type/state */
	int     self;		/* my own channel identifier */
	int     remote_id;	/* channel identifier for remote peer */
	int     istate;		/* input from channel (state of receive half) */
	int     ostate;		/* output to channel  (state of transmit half) */
	int     flags;		/* close sent/rcvd */
	int     rfd;		/* read fd */
	int     wfd;		/* write fd */
	int     efd;		/* extended fd */
	int     sock;		/* sock fd */
#ifndef TOPS_PORT
	int     isatty;	 /*	 rfd is a tty */
#endif
      int     force_drain;	/* force close on iEOF */
	int     delayed;		/* fdset hack */
	Buffer  input;		/* data read from socket, to be sent over
				 * encrypted connection */
	Buffer  output;		/* data received over encrypted connection for
				 * send on socket */
	Buffer  extended;
	char    path[SSH_CHANNEL_PATH_LEN];
		/* path for unix domain sockets, or host name for forwards */
	int     listening_port;	/* port being listened for forwards */
	int     host_port;	/* remote port to connect for forwards */
	char   *remote_name;	/* remote hostname */

	int	remote_window;
	int	remote_maxpacket;
	int	local_window;
	int	local_window_max;
	int	local_consumed;
	int	local_maxpacket;
	int     extended_usage;

	char   *ctype;		/* type */

	/* callback */
	channel_callback_fn	*cb_fn;
	VOIDP	cb_arg;
	int	cb_event;
	channel_callback_fn	*detach_user;

	/* filter */
	channel_filter_fn	*input_filter;
};

#define CHAN_EXTENDED_IGNORE		0
#define CHAN_EXTENDED_READ		1
#define CHAN_EXTENDED_WRITE		2

/* default window/packet sizes */
#define CHAN_SES_WINDOW_DEFAULT	(32*1024)
#define CHAN_SES_PACKET_DEFAULT	(CHAN_SES_WINDOW_DEFAULT/2)

/* possible input states */
#define CHAN_INPUT_OPEN			0x01
#define CHAN_INPUT_WAIT_DRAIN		0x02
#define CHAN_INPUT_WAIT_OCLOSE		0x04
#define CHAN_INPUT_CLOSED		0x08

/* possible output states */
#define CHAN_OUTPUT_OPEN		0x10
#define CHAN_OUTPUT_WAIT_DRAIN		0x20
#define CHAN_OUTPUT_WAIT_IEOF		0x40
#define CHAN_OUTPUT_CLOSED		0x80

#define CHAN_CLOSE_SENT			0x01
#define CHAN_CLOSE_RCVD			0x02

/* channel management */

Channel	*channel_lookup(int);
Channel *channel_new(char *, int, int, int, int, int, int, int, char *, int);
VOID	 channel_set_fds(int, int, int, int, int, int);
VOID	 channel_free(Channel *);
VOID	 channel_free_all(VOID);

VOID	 channel_request_start(int, char *, int);
VOID	 channel_register_callback(int, int mtype, channel_callback_fn *, VOIDP);
VOID	 channel_register_cleanup(int, channel_callback_fn *);
VOID	 channel_cancel_cleanup(int);
int	 channel_close_fd(int *);

/* protocol handler */

VOID	 channel_input_channel_request(int, int, LONG*);
VOID	 channel_input_data(int, int, LONG*);
VOID	 channel_input_extended_data(int, int, LONG*);
VOID	 channel_input_ieof(int, int, LONG*);
VOID	 channel_input_oclose(int, int, LONG*);
VOID	 channel_input_open_confirmation(int, int, LONG*);
VOID	 channel_input_open_failure(int, int, LONG*);
VOID	 channel_input_window_adjust(int, int, LONG*);

/* file descriptor handling (read/write) */

VOID	 channel_prepare_select(fd_set **, fd_set **, int *, int*, int);
VOID     channel_after_select(fd_set *, fd_set *);
VOID     channel_output_poll(VOID);

VOID     channel_close_all(VOID);
char	*channel_open_message(VOID);
int	 channel_find_open(VOID);

/* tcp forwarding */
VOID	 channel_set_af(int af);

/* channel close */

int	 chan_is_dead(Channel *, int);
VOID	 chan_mark_dead(Channel *);
VOID 	 chan_init_iostates(Channel *);
VOID	 chan_init(VOID);

typedef VOID    chan_event_fn(Channel *);

/* for the input state */
extern chan_event_fn	*chan_rcvd_oclose;
extern chan_event_fn	*chan_read_failed;
extern chan_event_fn	*chan_ibuf_empty;

/* for the output state */
extern chan_event_fn	*chan_rcvd_ieof;
extern chan_event_fn	*chan_write_failed;
extern chan_event_fn	*chan_obuf_empty;

#endif
