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

#include "includes.h"
RCSID("$OpenBSD: nchan.c,v 1.32 2001/10/10 22:18:47 markus Exp $");

#include "ssh1.h"
#include "ssh2.h"
#include "buffer.h"
#include "packet.h"
#include "channels.h"
#include "compat.h"
#include "log.h"

/*
 * SSH Protocol 1.5 aka New Channel Protocol
 * Thanks to Martina, Axel and everyone who left Erlangen, leaving me bored.
 * Written by Markus Friedl in October 1999
 *
 * Protocol versions 1.3 and 1.5 differ in the handshake protocol used for the
 * tear down of channels:
 *
 * 1.3:	strict request-ack-protocol:
 * 	CLOSE	->
 * 		<-  CLOSE_CONFIRM
 *
 * 1.5:	uses variations of:
 * 	IEOF	->
 * 		<-  OCLOSE
 * 		<-  IEOF
 * 	OCLOSE	->
 * 	i.e. both sides have to close the channel
 *
 * 2.0: the EOF messages are optional
 *
 * See the debugging output from 'ssh -v' and 'sshd -d' of
 * ssh-1.2.27 as an example.
 *
 */

/* functions manipulating channel states */
/*
 * EVENTS update channel input/output states execute ACTIONS
 */
/* events concerning the INPUT from socket for channel (istate) */
chan_event_fn *chan_rcvd_oclose			= NULL;
chan_event_fn *chan_read_failed			= NULL;
chan_event_fn *chan_ibuf_empty			= NULL;
/* events concerning the OUTPUT from channel for socket (ostate) */
chan_event_fn *chan_rcvd_ieof			= NULL;
chan_event_fn *chan_write_failed		= NULL;
chan_event_fn *chan_obuf_empty			= NULL;
/*
 * ACTIONS: should never update the channel states
 */
static VOID	chan_send_close2(Channel *);
static VOID	chan_send_eof2(Channel *);

/* helper */
static VOID	chan_shutdown_write(Channel *);
static VOID	chan_shutdown_read(Channel *);

/*
 * SSH1 specific implementation of event functions
 */

static VOID
chan_read_failed_12(Channel *c)
{
	debug("channel %d: read failed", c->self);
	switch (c->istate) {
	case CHAN_INPUT_OPEN:
		debug("channel %d: input open -> drain", c->self);
		chan_shutdown_read(c);
		c->istate = CHAN_INPUT_WAIT_DRAIN;
#if 0
		if (buffer_len(&c->input) == 0) {
			debug("channel %d: input: no drain shortcut", c->self);
			chan_ibuf_empty(c);
		}
#endif
		break;
	default:
		error("channel %d: chan_read_failed for istate %d",
		    c->self, c->istate);
		break;
	}
}

/*
 * the same for SSH2
 */
static VOID
chan_rcvd_oclose2(Channel *c)
{
	debug("channel %d: rcvd close", c->self);
	if (c->flags & CHAN_CLOSE_RCVD)
		error("channel %d: protocol error: close rcvd twice", c->self);
	c->flags |= CHAN_CLOSE_RCVD;
	if (c->type == SSH_CHANNEL_LARVAL) {
		/* tear down larval channels immediately */
		c->ostate = CHAN_OUTPUT_CLOSED;
		c->istate = CHAN_INPUT_CLOSED;
		return;
	}
	switch (c->ostate) {
	case CHAN_OUTPUT_OPEN:
		/*
		 * wait until a data from the channel is consumed if a CLOSE
		 * is received
		 */
		debug("channel %d: output open -> drain", c->self);
		c->ostate = CHAN_OUTPUT_WAIT_DRAIN;
		break;
	}
	switch (c->istate) {
	case CHAN_INPUT_OPEN:
		debug("channel %d: input open -> closed", c->self);
		chan_shutdown_read(c);
		break;
	case CHAN_INPUT_WAIT_DRAIN:
		debug("channel %d: input drain -> closed", c->self);
		chan_send_eof2(c);
		break;
	}
	c->istate = CHAN_INPUT_CLOSED;
}
static VOID
chan_ibuf_empty2(Channel *c)
{
	debug("channel %d: ibuf empty", c->self);
	if (buffer_len(&c->input)) {
		error("channel %d: chan_ibuf_empty for non empty buffer",
		     c->self);
		return;
	}
	switch (c->istate) {
	case CHAN_INPUT_WAIT_DRAIN:
		debug("channel %d: input drain -> closed", c->self);
		if (!(c->flags & CHAN_CLOSE_SENT)) {
			/* chan_send_eof2(c); */
			/* changing this to send close instead..sumit */
			chan_send_close2(c);
		}
		c->istate = CHAN_INPUT_CLOSED;
		break;
	default:
		error("channel %d: chan_ibuf_empty for istate %d",
		     c->self, c->istate);
		break;
	}
}
static VOID
chan_rcvd_ieof2(Channel *c)
{
	debug("channel %d: rcvd eof", c->self);
	if (c->ostate == CHAN_OUTPUT_OPEN) {
		debug("channel %d: output open -> drain", c->self);
		c->ostate = CHAN_OUTPUT_WAIT_DRAIN;
	}
}
static VOID
chan_write_failed2(Channel *c)
{
	debug("channel %d: write failed", c->self);
	switch (c->ostate) {
	case CHAN_OUTPUT_OPEN:
		debug("channel %d: output open -> closed", c->self);
		chan_shutdown_write(c); /* ?? */
		c->ostate = CHAN_OUTPUT_CLOSED;
		break;
	case CHAN_OUTPUT_WAIT_DRAIN:
		debug("channel %d: output drain -> closed", c->self);
		chan_shutdown_write(c);
		c->ostate = CHAN_OUTPUT_CLOSED;
		break;
	default:
		error("channel %d: chan_write_failed for ostate %d",
		    c->self, c->ostate);
		break;
	}
}
static VOID
chan_obuf_empty2(Channel *c)
{
	debug("channel %d: obuf empty", c->self);
	if (buffer_len(&c->output)) {
		error("channel %d: chan_obuf_empty for non empty buffer",
		    c->self);
		return;
	}
	switch (c->ostate) {
	case CHAN_OUTPUT_WAIT_DRAIN:
		debug("channel %d: output drain -> closed", c->self);
		chan_shutdown_write(c);
		c->ostate = CHAN_OUTPUT_CLOSED;
		break;
	default:
		error("channel %d: chan_obuf_empty for ostate %d",
		    c->self, c->ostate);
		break;
	}
}
static VOID
chan_send_eof2(Channel *c)
{
	debug("channel %d: send eof", c->self);
	switch (c->istate) {
	case CHAN_INPUT_WAIT_DRAIN:
		packet_start(SSH2_MSG_CHANNEL_EOF);
		packet_put_int(c->remote_id);
		packet_send();
		break;
	default:
		error("channel %d: cannot send eof for istate %d",
		    c->self, c->istate);
		break;
	}
}
static VOID
chan_send_close2(Channel *c)
{
	debug("channel %d: send close", c->self);
	/* removed this condition..so that when the
	 * local telnet closes, we send close to the
	 * remote client. We don't want to send eof etc..
	 * - sumit, 2/13
	 */

	/* if (c->ostate != CHAN_OUTPUT_CLOSED ||
	    c->istate != CHAN_INPUT_CLOSED) {
		error("channel %d: cannot send close for istate/ostate %d/%d",
		    c->self, c->istate, c->ostate);
	} else */ if (c->flags & CHAN_CLOSE_SENT) {
		error("channel %d: already sent close", c->self);
	} else {
		packet_start(SSH2_MSG_CHANNEL_CLOSE);
		packet_put_int(c->remote_id);
		packet_send();
		c->flags |= CHAN_CLOSE_SENT;
	}
}

/* shared */

VOID
chan_mark_dead(Channel *c)
{
	c->type = SSH_CHANNEL_ZOMBIE;
}

int
chan_is_dead(Channel *c, int send)
{
	if (c->type == SSH_CHANNEL_ZOMBIE) {
		debug("channel %d: zombie", c->self);
		return 1;
	}
	if (c->istate != CHAN_INPUT_CLOSED || c->ostate != CHAN_OUTPUT_CLOSED)
		return 0;
	/*
	 * we have to delay the close message if the efd (for stderr) is
	 * still active
	 */
	if (((c->extended_usage != CHAN_EXTENDED_IGNORE) &&
	    buffer_len(&c->extended) > 0)
#if 0
	    || ((c->extended_usage == CHAN_EXTENDED_READ) &&
	    c->efd != -1)
#endif
	    ) {
		debug2("channel %d: active efd: %d len %d type %s",
		    c->self, c->efd, buffer_len(&c->extended),
		    c->extended_usage==CHAN_EXTENDED_READ ?
		       "read": "write");
	} else {
		if (!(c->flags & CHAN_CLOSE_SENT)) {
			if (send) {
				chan_send_close2(c);
			} else {
				/* channel would be dead if we sent a close */
				if (c->flags & CHAN_CLOSE_RCVD) {
					debug("channel %d: almost dead",
					    c->self);
					return 1;
				}
			}
		}
		if ((c->flags & CHAN_CLOSE_SENT) &&
		    (c->flags & CHAN_CLOSE_RCVD)) {
			debug("channel %d: is dead", c->self);
			return 1;
		}
	}
	return 0;
}

VOID
chan_init_iostates(Channel *c)
{
	c->ostate = CHAN_OUTPUT_OPEN;
	c->istate = CHAN_INPUT_OPEN;
	c->flags = 0;
}

/* init */
VOID
chan_init(VOID)
{
   chan_rcvd_oclose		= chan_rcvd_oclose2;
   chan_read_failed		= chan_read_failed_12;
   chan_ibuf_empty			= chan_ibuf_empty2;

   chan_rcvd_ieof			= chan_rcvd_ieof2;
   chan_write_failed		= chan_write_failed2;
   chan_obuf_empty			= chan_obuf_empty2;
}

/* helper */
static VOID
chan_shutdown_write(Channel *c)
{
	buffer_consume(&c->output, buffer_len(&c->output));
	if (c->type == SSH_CHANNEL_LARVAL)
		return;
	/* shutdown failure is allowed if write failed already */
	debug("channel %d: close_write", c->self);
	if (c->sock != -1) {
		if (shutdown(c->sock, SHUT_WR) < 0)
			debug("channel %d: chan_shutdown_write: "
			    "shutdown() failed for fd%d: %.100s",
			    c->self, c->sock, strerror(errno));
	} else {
		if (channel_close_fd(&c->wfd) < 0)
			log("channel %d: chan_shutdown_write: "
			    "close() failed for fd%d: %.100s",
			    c->self, c->wfd, strerror(errno));
	}
}
static VOID
chan_shutdown_read(Channel *c)
{
	if (c->type == SSH_CHANNEL_LARVAL)
		return;
	debug("channel %d: close_read", c->self);
	if (c->sock != -1) {
		/*
		 * shutdown(sock, SHUT_READ) may return ENOTCONN if the
		 * write side has been closed already. (bug on Linux)
		 * HP-UX may return ENOTCONN also.
		 */
		if (shutdown(c->sock, SHUT_RD) < 0
		    && errno != ENOTCONN)
			error("channel %d: chan_shutdown_read: "
			    "shutdown() failed for fd%d [i%d o%d]: %.100s",
			    c->self, c->sock, c->istate, c->ostate,
			    strerror(errno));
	} else {
		if (channel_close_fd(&c->rfd) < 0)
			log("channel %d: chan_shutdown_read: "
			    "close() failed for fd%d: %.100s",
			    c->self, c->rfd, strerror(errno));
	}
}
