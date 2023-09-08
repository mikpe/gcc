/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Server main loop for handling the interactive session.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 * SSH2 support by Markus Friedl.
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
RCSID("$OpenBSD: serverloop.c,v 1.83 2001/11/09 18:59:23 markus Exp $");

#include "xmalloc.h"
#include "packet.h"
#include "buffer.h"
#include "log.h"
#include "servconf.h"
#include "channels.h"
#include "compat.h"
#include "ssh1.h"
#include "ssh2.h"
#include "auth.h"
#include "session.h"
#include "dispatch.h"
#include "auth-options.h"
#include "serverloop.h"
#include "misc.h"
#include "kex.h"

extern ServerOptions options;

/* XXX */
extern Kex *xxx_kex;
static Authctxt *xxx_authctxt;

static int connection_in;	/* Connection to client (input). */
static int connection_out;	/* Connection to client (output). */
static int connection_closed = 0;	/* Connection to client closed. */
static int client_alive_timeouts = 0;

/* prototypes */
static VOID server_init_dispatch(VOID);

static VOID
client_alive_check(VOID)
{
	int id;

	/* timeout, check to see how many we have had */
	if (++client_alive_timeouts > options.client_alive_count_max)
		packet_disconnect("Timeout, your session not responding.");

	id = channel_find_open();
	if (id == -1)
		packet_disconnect("No open channels after timeout!");
	/*
	 * send a bogus channel request with "wantreply",
	 * we should get back a failure
	 */
	channel_request_start(id, "keepalive@openssh.com", 1);
	packet_send();
}

/*
 * Sleep in select() until we can do something.  This will initialize the
 * select masks.  Upon return, the masks will indicate which descriptors
 * have data or can accept data.  Optionally, a maximum time can be specified
 * for the duration of the wait (0 = infinite).
 */
static VOID
wait_until_can_do_something(fd_set **readsetp, fd_set **writesetp, int *maxfdp,
    int *nallocp, int max_time_milliseconds)
{
	struct timeval tv, *tvp;
	int ret;
	int client_alive_scheduled = 0;

	/*
	 * if using client_alive, set the max timeout accordingly, 
	 * and indicate that this particular timeout was for client
	 * alive by setting the client_alive_scheduled flag.
	 *
	 * this could be randomized somewhat to make traffic
	 * analysis more difficult, but we're not doing it yet.  
	 */
	if (max_time_milliseconds == 0 && options.client_alive_interval) {
		client_alive_scheduled = 1;
		max_time_milliseconds = options.client_alive_interval * 1000;
	}

	/* Allocate and update select() masks for channel descriptors. */
	channel_prepare_select(readsetp, writesetp, maxfdp, nallocp, 0);

	FD_SET(connection_in, *readsetp);

	/*
	 * If we have buffered packet data going to the client, mark that
	 * descriptor.
	 */
	if (packet_have_data_to_write())
		FD_SET(connection_out, *writesetp);

	if (max_time_milliseconds == 0)
		tvp = NULL;
	else {
		tv.tv_sec = max_time_milliseconds / 1000;
		tv.tv_usec = 1000 * (max_time_milliseconds % 1000);
		tvp = &tv;
	}
	if (tvp!=NULL)
		debug3("tvp!=NULL mili %d", max_time_milliseconds);

	/* Wait for something to happen, or the timeout to expire. */
	ret = select((*maxfdp)+1, *readsetp, *writesetp, NULL, tvp);

	if (ret == -1) {
		memset(*readsetp, 0, *nallocp);
		memset(*writesetp, 0, *nallocp);
		if (errno != EINTR)
			error("select: %.100s", strerror(errno));
	} else if (ret == 0 && client_alive_scheduled)
		client_alive_check();
}

/*
 * Processes input from the client and the program.  Input data is stored
 * in buffers and processed later.
 */
static VOID
process_input(fd_set * readset)
{
	int len;
	char buf[16384];

	/* Read and buffer any input data from the client. */
	if (FD_ISSET(connection_in, readset)) {
		len = recv(connection_in, buf, sizeof(buf),0);
		if (len == 0) {
			verbose("Connection closed by remote host.");
			connection_closed = 1;
			return;
		} else if (len < 0) {
			if (errno != EINTR && errno != EAGAIN) {
				verbose("Read error from remote host: %.100s", strerror(errno));
				fatal_cleanup();
			}
		} else {
			/* Buffer any received data. */
			packet_process_incoming(buf, len);
		}
	}
}

/*
 * Sends data from internal buffers to client program stdin.
 */
static VOID
process_output(fd_set * writeset)
{
	/* Send any buffered packet data to the client. */
	if (FD_ISSET(connection_out, writeset))
		packet_write_poll();
}

static VOID
process_buffered_input_packets(VOID)
{
	dispatch_run(DISPATCH_NONBLOCK, NULL, (LONG*)xxx_kex);
}

VOID
server_loop2(Authctxt *authctxt)
{
	fd_set *readset = NULL, *writeset = NULL;
	int rekeying = 0, max_fd, nalloc = 0;

	debug("Entering interactive session for SSH2.");

	connection_in = packet_get_connection_in();
	connection_out = packet_get_connection_out();

	/* printf("Server connection_out = %d, connection_in = %d\r\n", 
			connection_out, connection_in); */

	max_fd = MAX(connection_in, connection_out);
	xxx_authctxt = authctxt;

	server_init_dispatch();

	for (;;) {
		process_buffered_input_packets();

		rekeying = (xxx_kex != NULL && !xxx_kex->done);

		if (!rekeying && packet_not_very_much_data_to_write())
			channel_output_poll();
		wait_until_can_do_something(&readset, &writeset, &max_fd,
		    &nalloc, 0);

		if (!rekeying)
			channel_after_select(readset, writeset);
		process_input(readset);
		if (connection_closed)
			break;
		process_output(writeset);
	}

	if (readset)
		xfree((LONG*)readset);
	if (writeset)
		xfree((LONG*)writeset);

	/* free all channels, no more reads and writes */
	channel_free_all();

	/* free remaining sessions, e.g. remove wtmp entries */
	session_destroy_all();
}

static VOID
server_input_channel_failure(int type, int plen, LONG* ctxt)
{
	debug("Got CHANNEL_FAILURE for keepalive");
	/* 
	 * reset timeout, since we got a sane answer from the client.
	 * even if this was generated by something other than
	 * the bogus CHANNEL_REQUEST we send for keepalives.
	 */
	client_alive_timeouts = 0; 
}

static Channel *
server_request_session(char *ctype)
{
	Channel *c;

	debug("input_session_request");
	packet_done();
	/*
	 * A server session has no fd to read or write until a
	 * CHANNEL_REQUEST for a shell is made, so we set the type to
	 * SSH_CHANNEL_LARVAL.  Additionally, a callback for handling all
	 * CHANNEL_REQUEST messages is registered.
	 */
	c = channel_new(ctype, SSH_CHANNEL_LARVAL,
	    -1, -1, -1, /*window size*/0, CHAN_SES_PACKET_DEFAULT,
	    0, xstrdup("server-session"), 1);
	if (c == NULL) {
		error("server_request_session: channel_new failed");
		return NULL;
	}
	if (session_open(xxx_authctxt, c->self) != 1) {
		debug("session open failed, free channel %d", c->self);
		channel_free(c);
		return NULL;
	}
	channel_register_callback(c->self, SSH2_MSG_CHANNEL_REQUEST,
	    session_input_channel_req, 0);
	channel_register_cleanup(c->self, session_close_by_channel);
	return c;
}

static VOID
server_input_channel_open(int type, int plen, LONG* ctxt)
{
	Channel *c = NULL;
	char *ctype;
	u_int len;
	int rchan;
	int rmaxpack;
	int rwindow;

	ctype = packet_get_string(&len);
	rchan = packet_get_int();
	rwindow = packet_get_int();
	rmaxpack = packet_get_int();

	debug("server_input_channel_open: ctype %s rchan %d win %d max %d",
	    ctype, rchan, rwindow, rmaxpack);

	if (strcmp(ctype, "session") == 0) {
		c = server_request_session(ctype);
	} 
	if (c != NULL) {
		debug("server_input_channel_open: confirm %s", ctype);
		c->remote_id = rchan;
		c->remote_window = rwindow;
		c->remote_maxpacket = rmaxpack;
		if (c->type != SSH_CHANNEL_CONNECTING) {
			packet_start(SSH2_MSG_CHANNEL_OPEN_CONFIRMATION);
			packet_put_int(c->remote_id);
			packet_put_int(c->self);
			packet_put_int(c->local_window);
			packet_put_int(c->local_maxpacket);
			packet_send();
		}
	} else {
		debug("server_input_channel_open: failure %s", ctype);
		packet_start(SSH2_MSG_CHANNEL_OPEN_FAILURE);
		packet_put_int(rchan);
		packet_put_int(SSH2_OPEN_ADMINISTRATIVELY_PROHIBITED);
		if (!(datafellows & SSH_BUG_OPENFAILURE)) {
			packet_put_cstring("open failed");
			packet_put_cstring("");
		}
		packet_send();
	}
	xfree((LONG*)ctype);
}

static VOID
server_input_global_request(int type, int plen, LONG* ctxt)
{
	char *rtype;
	int want_reply;
	int success = 0;

	rtype = packet_get_string(NULL);
	want_reply = packet_get_char();
	debug("server_input_global_request: rtype %s want_reply %d", rtype, want_reply);

	/* -R style forwarding */
	if (strcmp(rtype, "tcpip-forward") == 0) {
#ifdef TOPS_PORT
		struct tops_passwd *pw;
#else
		struct passwd *pw;
#endif
		char *listen_address;
		u_short listen_port;

		pw = auth_get_user();
		if (pw == NULL)
			fatal("server_input_global_request: no user");
		listen_address = packet_get_string(NULL); /* XXX currently ignored */
		listen_port = (u_short)packet_get_int();
		debug("server_input_global_request: tcpip-forward listen %s port %d",
		    listen_address, listen_port);

      success = 0;
      packet_send_debug("Server has disabled port forwarding.");

		xfree((LONG*)listen_address);
	}
	if (want_reply) {
		packet_start(success ?
		    SSH2_MSG_REQUEST_SUCCESS : SSH2_MSG_REQUEST_FAILURE);
		packet_send();
		packet_write_wait();
	}
	xfree((LONG*)rtype);
}

static VOID
server_init_dispatch(VOID)
{
	debug("server_init_dispatch_20");
	dispatch_init(&dispatch_protocol_error);
	dispatch_set(SSH2_MSG_CHANNEL_CLOSE, &channel_input_oclose);
	dispatch_set(SSH2_MSG_CHANNEL_DATA, &channel_input_data);
	dispatch_set(SSH2_MSG_CHANNEL_EOF, &channel_input_ieof);
	dispatch_set(SSH2_MSG_CHANNEL_EXTENDED_DATA, &channel_input_extended_data);
	dispatch_set(SSH2_MSG_CHANNEL_OPEN, &server_input_channel_open);
	dispatch_set(SSH2_MSG_CHANNEL_OPEN_CONFIRMATION, &channel_input_open_confirmation);
	dispatch_set(SSH2_MSG_CHANNEL_OPEN_FAILURE, &channel_input_open_failure);
	dispatch_set(SSH2_MSG_CHANNEL_REQUEST, &channel_input_channel_request);
	dispatch_set(SSH2_MSG_CHANNEL_WINDOW_ADJUST, &channel_input_window_adjust);
	dispatch_set(SSH2_MSG_GLOBAL_REQUEST, &server_input_global_request);
	/* client_alive */
	dispatch_set(SSH2_MSG_CHANNEL_FAILURE, &server_input_channel_failure);
	/* rekeying */
	dispatch_set(SSH2_MSG_KEXINIT, &kex_input_kexinit);
}
