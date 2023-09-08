/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This file contains functions for generic socket connection forwarding.
 * There is also code for initiating connection forwarding for X11 connections,
 * arbitrary tcp/ip connections, and the authentication agent connection.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 * SSH2 support added by Markus Friedl.
 * Copyright (c) 1999, 2000, 2001 Markus Friedl.  All rights reserved.
 * Copyright (c) 1999 Dug Song.  All rights reserved.
 * Copyright (c) 1999 Theo de Raadt.  All rights reserved.
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
RCSID("$OpenBSD: channels.c,v 1.140 2001/10/10 22:18:47 markus Exp $");

#include "ssh.h"
#include "ssh1.h"
#include "ssh2.h"
#include "packet.h"
#include "xmalloc.h"
#include "log.h"
#include "misc.h"
#include "auth.h"
#include "channels.h"
#include "compat.h"
#include "canohost.h"
#include "key.h"
#include "addmore.h"


/* -- channel core */

/*
 * Pointer to an array containing all allocated channels.  The array is
 * dynamically extended as needed.
 */
static Channel **channels = NULL;

/*
 * Size of the channel array.  All slots of the array must always be
 * initialized (at least the type field); unused slots set to NULL
 */
static int channels_alloc = 0;

/*
 * Maximum file descriptor value used in any of the channels.  This is
 * updated in channel_new.
 */
static int channel_max_fd = 0;


/* -- tcp forwarding */

/*
 * Data structure for storing which hosts are permitted for forward requests.
 * The local sides of any remote forwards are stored in this array to prevent
 * a corrupt remote server from accessing arbitrary TCP/IP ports on our local
 * network (which might be behind a firewall).
 */
typedef struct {
	char *host_to_connect;		/* Connect to 'host'. */
	u_short port_to_connect;	/* Connect to 'port'. */
	u_short listen_port;		/* Remote side should listen port number. */
} ForwardPermission;

/* -- agent forwarding */

#define	NUM_SOCKS	10

/* AF_UNSPEC or AF_INET or AF_INET6 */
static int IPv4or6 = AF_UNSPEC;

/* helper */
static VOID port_open_helper(Channel *c, char *rtype);

/* -- channel core */

Channel *
channel_lookup(int id)
{
	Channel *c;

	if (id < 0 || id >= channels_alloc) {
		log("channel_lookup: %d: bad id", id);
		return NULL;
	}
	c = channels[id];
	if (c == NULL) {
		log("channel_lookup: %d: bad id: channel free", id);
		return NULL;
	}
	return c;
}

/*
 * Register filedescriptors for a channel, used when allocating a channel or
 * when the channel consumer/producer is ready, e.g. shell exec'd
 */

static VOID
channel_register_fds(Channel *c, int rfd, int wfd, int efd,
    int extusage, int nonblock)
{
	/* Update the maximum file descriptor value. */
	channel_max_fd = MAX(channel_max_fd, rfd);
	channel_max_fd = MAX(channel_max_fd, wfd);
	channel_max_fd = MAX(channel_max_fd, efd);

	/* XXX set close-on-exec -markus */

	c->rfd = rfd;
	c->wfd = wfd;
	c->sock = (rfd == wfd) ? rfd : -1;
	c->efd = efd;
	c->extended_usage = extusage;

#ifndef TOPS_PORT
	/* XXX ugly hack: nonblock is only set by the server */
	if (nonblock && isatty(c->rfd)) {
		debug("channel %d: rfd %d isatty", c->self, c->rfd);
		c->isatty = 1;
		if (!isatty(c->wfd)) {
			error("channel %d: wfd %d is not a tty?",
			    c->self, c->wfd);
		}
	} else {
		c->isatty = 0;
	}
#endif
	/* enable nonblocking mode */
	if (nonblock) {
		if (rfd != -1)
			set_nonblock(rfd);
		if (wfd != -1)
			set_nonblock(wfd);
		if (efd != -1)
			set_nonblock(efd);
	}
}

/*
 * Allocate a new channel object and set its type and socket. This will cause
 * remote_name to be freed.
 */

#define MAX_CHANNELS 1

Channel *
channel_new(char *ctype, int type, int rfd, int wfd, int efd,
    int window, int maxpack, int extusage, char *remote_name, int nonblock)
{
	int i, found;
	Channel *c;

	/* Do initial allocation if this is the first call. */
	if (channels_alloc == 0) {
		chan_init();
		channels_alloc = MAX_CHANNELS;
		channels = (Channel **)xmalloc(channels_alloc * 
					       sizeof(Channel *));
		for (i = 0; i < channels_alloc; i++)
			channels[i] = NULL;
		fatal_add_cleanup((VOID (*) (VOIDP)) channel_free_all, NULL);
	}
	/* Try to find a free slot where to put the new channel. */
	for (found = -1, i = 0; i < channels_alloc; i++)
		if (channels[i] == NULL) {
			/* Found a free slot. */
			found = i;
			break;
		}
	if (found < 0) {
	  return NULL;
	}
	/* Initialize and return new channel. */
	c = channels[found] = (Channel *)xmalloc(sizeof(Channel));
	memset(c, 0, sizeof(Channel));
	buffer_init(&c->input);
	buffer_init(&c->output);
	buffer_init(&c->extended);
	chan_init_iostates(c);
	channel_register_fds(c, rfd, wfd, efd, extusage, nonblock);
	c->self = found;
	c->type = type;
	c->ctype = ctype;
	c->local_window = window;
	c->local_window_max = window;
	c->local_consumed = 0;
	c->local_maxpacket = maxpack;
	c->remote_id = -1;
	c->remote_name = remote_name;
	c->remote_window = 0;
	c->remote_maxpacket = 0;
	c->cb_fn = NULL;
	c->cb_arg = NULL;
	c->cb_event = 0;
	c->force_drain = 0;
	c->detach_user = NULL;
	c->input_filter = NULL;
	debug("channel %d: new [%s]", found, remote_name);
	return c;
}

static int
channel_find_maxfd(VOID)
{
	int i, max = 0;
	Channel *c;

	for (i = 0; i < channels_alloc; i++) {
		c = channels[i];
		if (c != NULL) {
			max = MAX(max, c->rfd);
			max = MAX(max, c->wfd);
			max = MAX(max, c->efd);
		}
	}
	return max;
}

int
channel_close_fd(int *fdp)
{
	int ret = 0, fd = *fdp;

	if (fd != -1) {
		ret = closesocket(fd);
		*fdp = -1;
		if (fd == channel_max_fd)
			channel_max_fd = channel_find_maxfd();
	}
	return ret;
}

/* Close all channel fd/socket. */

static VOID
channel_close_fds(Channel *c)
{
	debug3("channel_close_fds: channel %d: r %d w %d e %d",
	    c->self, c->rfd, c->wfd, c->efd);

	channel_close_fd(&c->sock);
	channel_close_fd(&c->rfd);
	channel_close_fd(&c->wfd);
	channel_close_fd(&c->efd);
}

/* Free the channel and close its fd/socket. */

VOID
channel_free(Channel *c)
{
	char *s;
	int i, n;

	for (n = 0, i = 0; i < channels_alloc; i++)
		if (channels[i])
			n++;
	debug("channel_free: channel %d: %s, nchannels %d", c->self,
	    c->remote_name ? c->remote_name : (char *)"???", n);

	s = channel_open_message();
	debug3("channel_free: status: %s", s);
	xfree((LONG*)s);

	if (c->sock != -1)
		shutdown(c->sock, SHUT_RDWR);
	channel_close_fds(c);
	buffer_free(&c->input);
	buffer_free(&c->output);
	buffer_free(&c->extended);
	if (c->remote_name) {
		xfree((LONG*)c->remote_name);
		c->remote_name = NULL;
	}
	channels[c->self] = NULL;
	xfree((LONG*)c);
}

VOID
channel_free_all(VOID)
{
	int i;

	for (i = 0; i < channels_alloc; i++)
		if (channels[i] != NULL)
			channel_free(channels[i]);
}

/*
 * Closes the sockets/fds of all channels.  This is used to close extra file
 * descriptors after a fork.
 */

VOID
channel_close_all()
{
	int i;

	for (i = 0; i < channels_alloc; i++)
		if (channels[i] != NULL)
			channel_close_fds(channels[i]);
}

/* Returns the id of an open channel suitable for keepaliving */

int
channel_find_open()
{
	int i;
	Channel *c;

	for (i = 0; i < channels_alloc; i++) {
		c = channels[i];
		if (c == NULL)
			continue;
		switch (c->type) {
		case SSH_CHANNEL_CLOSED:
		case SSH_CHANNEL_DYNAMIC:
		case SSH_CHANNEL_PORT_LISTENER:
		case SSH_CHANNEL_RPORT_LISTENER:
		case SSH_CHANNEL_OPENING:
		case SSH_CHANNEL_CONNECTING:
		case SSH_CHANNEL_ZOMBIE:
			continue;
		case SSH_CHANNEL_LARVAL:
		case SSH_CHANNEL_AUTH_SOCKET:
		case SSH_CHANNEL_OPEN:
			return i;
		case SSH_CHANNEL_INPUT_DRAINING:
		case SSH_CHANNEL_OUTPUT_DRAINING:
			fatal("cannot happen: OUT_DRAIN");
			return i;
		default:
			fatal("channel_find_open: bad channel type %d", c->type);
			/* NOTREACHED */
		}
	}
	return -1;
}


/*
 * Returns a message describing the currently open forwarded connections,
 * suitable for sending to the client.  The message contains crlf pairs for
 * newlines.
 */

char *
channel_open_message()
{
	Buffer buffer;
	Channel *c;
	char buf[1024], *cp;
	int i;

	buffer_init(&buffer);
#ifdef NOSTUB
	snprintf(buf, sizeof buf, "The following connections are open:\r\n");
#else
	sprintf(buf, "The following connections are open:\r\n");
#endif
	buffer_append(&buffer, buf, strlen(buf));
	for (i = 0; i < channels_alloc; i++) {
		c = channels[i];
		if (c == NULL)
			continue;
		switch (c->type) {
		case SSH_CHANNEL_PORT_LISTENER:
		case SSH_CHANNEL_RPORT_LISTENER:
		case SSH_CHANNEL_CLOSED:
		case SSH_CHANNEL_AUTH_SOCKET:
		case SSH_CHANNEL_ZOMBIE:
			continue;
		case SSH_CHANNEL_LARVAL:
		case SSH_CHANNEL_OPENING:
		case SSH_CHANNEL_CONNECTING:
		case SSH_CHANNEL_DYNAMIC:
		case SSH_CHANNEL_OPEN:
		case SSH_CHANNEL_INPUT_DRAINING:
		case SSH_CHANNEL_OUTPUT_DRAINING:
#ifdef NOSTUB
			snprintf(buf, sizeof buf, "  #%d %.300s (t%d r%d i%d/%d o%d/%d fd %d/%d)\r\n",
			    c->self, c->remote_name,
			    c->type, c->remote_id,
			    c->istate, buffer_len(&c->input),
			    c->ostate, buffer_len(&c->output),
			    c->rfd, c->wfd);
#else
			sprintf(buf, "  #%d %.300s (t%d r%d i%d/%d o%d/%d fd %d/%d)\r\n",
			    c->self, c->remote_name,
			    c->type, c->remote_id,
			    c->istate, buffer_len(&c->input),
			    c->ostate, buffer_len(&c->output),
			    c->rfd, c->wfd);
#endif
			buffer_append(&buffer, buf, strlen(buf));
			continue;
		default:
			fatal("channel_open_message: bad channel type %d", c->type);
			/* NOTREACHED */
		}
	}
	buffer_append(&buffer, "\0", 1);
	cp = xstrdup(buffer_ptr(&buffer));
	buffer_free(&buffer);
	return cp;
}

VOID
channel_request_start(int id, char *service, int wantconfirm)
{
	Channel *c = channel_lookup(id);
	if (c == NULL) {
		log("channel_request: %d: bad id", id);
		return;
	}
	packet_start(SSH2_MSG_CHANNEL_REQUEST);
	packet_put_int(c->remote_id);
	packet_put_cstring(service);
	packet_put_char(wantconfirm);
}
VOID
channel_register_callback(int id, int mtype, channel_callback_fn *fn, VOIDP arg)
{
	Channel *c = channel_lookup(id);
	if (c == NULL) {
		log("channel_register_callback: %d: bad id", id);
		return;
	}
	c->cb_event = mtype;
	c->cb_fn = fn;
	c->cb_arg = arg;
}
VOID
channel_register_cleanup(int id, channel_callback_fn *fn)
{
	Channel *c = channel_lookup(id);
	if (c == NULL) {
		log("channel_register_cleanup: %d: bad id", id);
		return;
	}
	c->detach_user = fn;
}
VOID
channel_cancel_cleanup(int id)
{
	Channel *c = channel_lookup(id);
	if (c == NULL) {
		log("channel_cancel_cleanup: %d: bad id", id);
		return;
	}
	c->detach_user = NULL;
}

#ifdef TOPS_PORT
extern int telnet_input_filter( struct Channel *c, UNS8 *data, int L); 
#endif

VOID
channel_set_fds(int id, int rfd, int wfd, int efd,
    int extusage, int nonblock)
{
	Channel *c = channel_lookup(id);
	if (c == NULL || c->type != SSH_CHANNEL_LARVAL)
		fatal("channel_activate for non-larval channel %d.", id);
	channel_register_fds(c, rfd, wfd, efd, extusage, nonblock);
	c->type = SSH_CHANNEL_OPEN;
#ifdef TOPS_PORT
	c->input_filter = telnet_input_filter;
#endif
	/* XXX window size? */
	c->local_window = c->local_window_max = c->local_maxpacket * 2;
	packet_start(SSH2_MSG_CHANNEL_WINDOW_ADJUST);
	packet_put_int(c->remote_id);
	packet_put_int(c->local_window);
	packet_send();
}

/*
 * 'channel_pre*' are called just before select() to add any bits relevant to
 * channels in the select bitmasks.
 */
/*
 * 'channel_post*': perform any appropriate operations for channels which
 * have events pending.
 */
typedef VOID chan_fn(Channel *c, fd_set * readset, fd_set * writeset);
chan_fn *channel_pre[SSH_CHANNEL_MAX_TYPE];
chan_fn *channel_post[SSH_CHANNEL_MAX_TYPE];

static VOID
channel_pre_listener(Channel *c, fd_set * readset, fd_set * writeset)
{
	FD_SET(c->sock, readset);
}

static VOID
channel_pre_connecting(Channel *c, fd_set * readset, fd_set * writeset)
{
	debug3("channel %d: waiting for connection", c->self);
	FD_SET(c->sock, writeset);
}

static VOID
channel_pre_open_20(Channel *c, fd_set * readset, fd_set * writeset)
{
	if (c->istate == CHAN_INPUT_OPEN &&
	    c->remote_window > 0 &&
	    buffer_len(&c->input) < c->remote_window)
		FD_SET(c->rfd, readset);
	if (c->ostate == CHAN_OUTPUT_OPEN ||
	    c->ostate == CHAN_OUTPUT_WAIT_DRAIN) {
		if (buffer_len(&c->output) > 0) {
			FD_SET(c->wfd, writeset);
		} else if (c->ostate == CHAN_OUTPUT_WAIT_DRAIN) {
			chan_obuf_empty(c);
		}
	}
	/** XXX check close conditions, too */
	if (c->efd != -1) {
		if (c->extended_usage == CHAN_EXTENDED_WRITE &&
		    buffer_len(&c->extended) > 0)
			FD_SET(c->efd, writeset);
		else if (c->extended_usage == CHAN_EXTENDED_READ &&
		    buffer_len(&c->extended) < c->remote_window)
			FD_SET(c->efd, readset);
	}
}

/* try to decode a socks4 header */
static int
channel_decode_socks4(Channel *c, fd_set * readset, fd_set * writeset)
{
	u_char *p, *host;
	int len, have, i, found;
	char username[256];	
	struct {
		u_int8_t version;
		u_int8_t command;
		u_int16_t dest_port;
		struct in_addr dest_addr;
	} s4_req, s4_rsp;

	debug2("channel %d: decode socks4", c->self);

	have = buffer_len(&c->input);
	len = sizeof(s4_req);
	if (have < len)
		return 0;
	p = buffer_ptr(&c->input);
	for (found = 0, i = len; i < have; i++) {
		if (p[i] == '\0') {
			found = 1;
			break;
		}
		if (i > 1024) {
			/* the peer is probably sending garbage */
			debug("channel %d: decode socks4: too long",
			    c->self);
			return -1;
		}
	}
	if (!found)
		return 0;
	buffer_get(&c->input, (char *)&s4_req.version, 1);
	buffer_get(&c->input, (char *)&s4_req.command, 1);
	buffer_get(&c->input, (char *)&s4_req.dest_port, 2);
	buffer_get(&c->input, (char *)&s4_req.dest_addr, 4);
	have = buffer_len(&c->input);
	p = buffer_ptr(&c->input);
	len = strlen(p);
	debug2("channel %d: decode socks4: user %s/%d", c->self, p, len);
	if (len > have)
		fatal("channel %d: decode socks4: len %d > have %d",
		    c->self, len, have);
	strlcpy(username, p, sizeof(username));
	buffer_consume(&c->input, len);
	buffer_consume(&c->input, 1);		/* trailing '\0' */

	/*host = inet_ntoa(s4_req.dest_addr);*/
	host = "127.0.0.1";
	strlcpy(c->path, host, sizeof(c->path));
	c->host_port = ntohs(s4_req.dest_port);
	
	debug("channel %d: dynamic request: socks4 host %s port %u command %u",
	    c->self, host, c->host_port, s4_req.command);

	if (s4_req.command != 1) {
		debug("channel %d: cannot handle: socks4 cn %d",
		    c->self, s4_req.command);
		return -1;
	}
	s4_rsp.version = 0;			/* vn: 0 for reply */
	s4_rsp.command = 90;			/* cd: req granted */
	s4_rsp.dest_port = 0;			/* ignored */
	s4_rsp.dest_addr.s_addr = INADDR_ANY;	/* ignored */
	buffer_append(&c->output, (char *)&s4_rsp, sizeof(s4_rsp));
	return 1;
}

/* dynamic port forwarding */
static VOID
channel_pre_dynamic(Channel *c, fd_set * readset, fd_set * writeset)
{
	u_char *p;
	int have, ret;

	have = buffer_len(&c->input);
	c->delayed = 0;
	debug2("channel %d: pre_dynamic: have %d", c->self, have);
	/* buffer_dump(&c->input); */
	/* check if the fixed size part of the packet is in buffer. */
	if (have < 4) {
		/* need more */
		FD_SET(c->sock, readset);
		return;
	}
	/* try to guess the protocol */
	p = buffer_ptr(&c->input);
	switch (p[0]) {
	case 0x04:
		ret = channel_decode_socks4(c, readset, writeset);
		break;
	default:
		ret = -1;
		break;
	}
	if (ret < 0) {
		chan_mark_dead(c);
	} else if (ret == 0) {
		debug2("channel %d: pre_dynamic: need more", c->self);
		/* need more */
		FD_SET(c->sock, readset);
	} else {
		/* switch to the next state */
		c->type = SSH_CHANNEL_OPENING;
		port_open_helper(c, "direct-tcpip");
	}
}

static VOID
port_open_helper(Channel *c, char *rtype)
{
	int direct;
	char buf[1024];
	char *remote_ipaddr = get_peer_ipaddr(c->sock);
	u_short remote_port = get_peer_port(c->sock);

	direct = (strcmp(rtype, "direct-tcpip") == 0);

#ifdef NOSTUB
	snprintf(buf, sizeof buf,
	    "%s: listening port %d for %.100s port %d, "
	    "connect from %.200s port %d",
	    rtype, c->listening_port, c->path, c->host_port,
	    remote_ipaddr, remote_port);
#else
	sprintf(buf,
		"%s: listening port %d for %.100s port %d, "
		"connect from %.200s port %d",
		rtype, c->listening_port, c->path, c->host_port,
		remote_ipaddr, remote_port);
#endif
	xfree((LONG*)c->remote_name);
	c->remote_name = xstrdup(buf);

   packet_start(SSH2_MSG_CHANNEL_OPEN);
   packet_put_cstring(rtype);
   packet_put_int(c->self);
   packet_put_int(c->local_window_max);
   packet_put_int(c->local_maxpacket);
   if (direct) {
      /* target host, port */
      packet_put_cstring(c->path);
      packet_put_int(c->host_port);
   } else {
      /* listen address, port */
      packet_put_cstring(c->path);
      packet_put_int(c->listening_port);
   }
   /* originator host and port */
   packet_put_cstring(remote_ipaddr);
   packet_put_int(remote_port);
   packet_send();
	xfree((LONG*)remote_ipaddr);
}

/*
 * This socket is listening for connections to a forwarded TCP/IP port.
 */
static VOID
channel_post_port_listener(Channel *c, fd_set * readset, fd_set * writeset)
{
	Channel *nc;
	struct sockaddr addr;
	int newsock, nextstate;
	socklen_t addrlen;
	char *rtype;

	if (FD_ISSET(c->sock, readset)) {
		debug("Connection to port %d forwarding "
		    "to %.100s port %d requested.",
		    c->listening_port, c->path, c->host_port);

		if (c->type == SSH_CHANNEL_RPORT_LISTENER) {
			nextstate = SSH_CHANNEL_OPENING;
			rtype = "forwarded-tcpip";
		} else {
			if (c->host_port == 0) {
				nextstate = SSH_CHANNEL_DYNAMIC;
				rtype = "dynamic-tcpip";
			} else {
				nextstate = SSH_CHANNEL_OPENING;
				rtype = "direct-tcpip";
			}
		}

		addrlen = sizeof(addr);
		newsock = accept(c->sock, &addr, &addrlen);
		if (newsock < 0) {
			error("accept: %.100s", strerror(errno));
			return;
		}
		nc = channel_new(rtype,
		    nextstate, newsock, newsock, -1,
		    c->local_window_max, c->local_maxpacket,
		    0, xstrdup(rtype), 1);
		if (nc == NULL) {
			error("channel_post_port_listener: no new channel:");
			closesocket(newsock);
			return;
		}
		nc->listening_port = c->listening_port;
		nc->host_port = c->host_port;
		strlcpy(nc->path, c->path, sizeof(nc->path));

		if (nextstate == SSH_CHANNEL_DYNAMIC) {
			/*
			 * do not call the channel_post handler until
			 * this flag has been reset by a pre-handler.
			 * otherwise the FD_ISSET calls might overflow
			 */
			nc->delayed = 1;
		} else {
			port_open_helper(nc, rtype);
		}
	}
}

/*
 * This is the authentication agent socket listening for connections from
 * clients.
 */
static VOID
channel_post_auth_listener(Channel *c, fd_set * readset, fd_set * writeset)
{
	Channel *nc;
	char *name;
	int newsock;
	struct sockaddr addr;
	socklen_t addrlen;

	if (FD_ISSET(c->sock, readset)) {
		addrlen = sizeof(addr);
		newsock = accept(c->sock, &addr, &addrlen);
		if (newsock < 0) {
			error("accept from auth socket: %.100s", strerror(errno));
			return;
		}
		name = xstrdup("accepted auth socket");
		nc = channel_new("accepted auth socket",
		    SSH_CHANNEL_OPENING, newsock, newsock, -1,
		    c->local_window_max, c->local_maxpacket,
		    0, name, 1);
		if (nc == NULL) {
			error("channel_post_auth_listener: channel_new failed");
			xfree((LONG*)name);
			closesocket(newsock);
		}
      packet_start(SSH2_MSG_CHANNEL_OPEN);
      packet_put_cstring("auth-agent@openssh.com");
      packet_put_int(nc->self);
      packet_put_int(c->local_window_max);
      packet_put_int(c->local_maxpacket);
		packet_send();
	}
}

static VOID
channel_post_connecting(Channel *c, fd_set * readset, fd_set * writeset)
{
  int err = 0;
  
  if (FD_ISSET(c->sock, writeset)) {
#if TOPS20
#else
    {
      socklen_t sz = sizeof(err);

      if (getsockopt(c->sock, SOL_SOCKET, SO_ERROR, (char *)&err, &sz) < 0) {
	err = errno;
	error("getsockopt SO_ERROR failed");
      }
    }
#endif
    if (err == 0) {
      debug("channel %d: connected", c->self);
      c->type = SSH_CHANNEL_OPEN;
      packet_start(SSH2_MSG_CHANNEL_OPEN_CONFIRMATION);
      packet_put_int(c->remote_id);
      packet_put_int(c->self);
      packet_put_int(c->local_window);
      packet_put_int(c->local_maxpacket);
    } else {
      debug("channel %d: not connected: %s",
	    c->self, strerror(err));
      packet_start(SSH2_MSG_CHANNEL_OPEN_FAILURE);
      packet_put_int(c->remote_id);
      packet_put_int(SSH2_OPEN_CONNECT_FAILED);
      if (!(datafellows & SSH_BUG_OPENFAILURE)) {
	packet_put_cstring(strerror(err));
	packet_put_cstring("");
      }
      chan_mark_dead(c);
    }
    packet_send();
  }
}

static int
channel_handle_rfd(Channel *c, fd_set * readset, fd_set * writeset)
{
	UNS8 buf[16*1024];
	int len;

	if (c->rfd != -1 &&
	    FD_ISSET(c->rfd, readset)) {
		len = recv(c->rfd, buf, sizeof(buf),0);
		if (len < 0 && (errno == EINTR || errno == EAGAIN))
			return 1;
		if (len <= 0) {
			debug("channel %d: read<=0 rfd %d len %d",
			    c->self, c->rfd, len);
			if (c->type != SSH_CHANNEL_OPEN) {
				debug("channel %d: not open", c->self);
				chan_mark_dead(c);
				return -1;
			} else {
				chan_read_failed(c);
			}
			return -1;
		}
		if(c->input_filter != NULL) {
			if (c->input_filter(c, buf, len) < 0) {
				debug("channel %d: filter stops", c->self);
				chan_read_failed(c);
			}
		} else {
			buffer_append(&c->input, buf, len);
		}
	}
	return 1;
}
static int
channel_handle_wfd(Channel *c, fd_set * readset, fd_set * writeset)
{
#ifndef TOPS_PORT
	struct termios tio;
#endif
	UNS8 *data;
	u_int dlen;
	int len;

	/* Send buffered output data to the socket. */
	if (c->wfd != -1 &&
	    FD_ISSET(c->wfd, writeset) &&
	    buffer_len(&c->output) > 0) {
		data = buffer_ptr(&c->output);
		dlen = buffer_len(&c->output);
		len = send(c->wfd, data, dlen,0);
		if (len < 0 && (errno == EINTR || errno == EAGAIN))
			return 1;
		if (len <= 0) {
			if (c->type != SSH_CHANNEL_OPEN) {
				debug("channel %d: not open", c->self);
				chan_mark_dead(c);
				return -1;
			} else {
				chan_write_failed(c);
			}
			return -1;
		}
#ifndef TOPS_PORT
		if (c->isatty && dlen >= 1 && data[0] != '\r') {
			if (tcgetattr(c->wfd, &tio) == 0 &&
			    !(tio.c_lflag & ECHO) && (tio.c_lflag & ICANON)) {
				/*
				 * Simulate echo to reduce the impact of
				 * traffic analysis. We need to match the
				 * size of a SSH2_MSG_CHANNEL_DATA message
				 * (4 byte channel id + data)
				 */
				packet_send_ignore(4 + len);
				packet_send();
			}
		}
#endif
		buffer_consume(&c->output, len);
		if (len > 0) {
			c->local_consumed += len;
		}
	}
	return 1;
}
static int
channel_handle_efd(Channel *c, fd_set * readset, fd_set * writeset)
{
	UNS8 buf[16*1024];
	int len;

/** XXX handle drain efd, too */
	if (c->efd != -1) {
		if (c->extended_usage == CHAN_EXTENDED_WRITE &&
		    FD_ISSET(c->efd, writeset) &&
		    buffer_len(&c->extended) > 0) {
			len = send(c->efd, buffer_ptr(&c->extended),
			    buffer_len(&c->extended),0);
			debug2("channel %d: written %d to efd %d",
			    c->self, len, c->efd);
			if (len < 0 && (errno == EINTR || errno == EAGAIN))
				return 1;
			if (len <= 0) {
				debug2("channel %d: closing write-efd %d",
				    c->self, c->efd);
				channel_close_fd(&c->efd);
			} else {
				buffer_consume(&c->extended, len);
				c->local_consumed += len;
			}
		} else if (c->extended_usage == CHAN_EXTENDED_READ &&
		    FD_ISSET(c->efd, readset)) {
			len = recv(c->efd, buf, sizeof(buf),0);
			debug2("channel %d: read %d from efd %d",
			     c->self, len, c->efd);
			if (len < 0 && (errno == EINTR || errno == EAGAIN))
				return 1;
			if (len <= 0) {
				debug2("channel %d: closing read-efd %d",
				    c->self, c->efd);
				channel_close_fd(&c->efd);
			} else {
				buffer_append(&c->extended, buf, len);
			}
		}
	}
	return 1;
}
static int
channel_check_window(Channel *c)
{
	if (c->type == SSH_CHANNEL_OPEN &&
	    !(c->flags & (CHAN_CLOSE_SENT|CHAN_CLOSE_RCVD)) &&
	    c->local_window < c->local_window_max/2 &&
	    c->local_consumed > 0) {
		packet_start(SSH2_MSG_CHANNEL_WINDOW_ADJUST);
		packet_put_int(c->remote_id);
		packet_put_int(c->local_consumed);
		packet_send();
		debug2("channel %d: window %d sent adjust %d",
		    c->self, c->local_window,
		    c->local_consumed);
		c->local_window += c->local_consumed;
		c->local_consumed = 0;
	}
	return 1;
}

static VOID
channel_post_open_2(Channel *c, fd_set * readset, fd_set * writeset)
{
	if (c->delayed)
		return;
	channel_handle_rfd(c, readset, writeset);
	channel_handle_wfd(c, readset, writeset);
	channel_handle_efd(c, readset, writeset);

	channel_check_window(c);
}

static VOID
channel_handler_init(VOID)
{
	int i;
	for(i = 0; i < SSH_CHANNEL_MAX_TYPE; i++) {
		channel_pre[i] = NULL;
		channel_post[i] = NULL;
	}
	channel_pre[SSH_CHANNEL_OPEN] =			&channel_pre_open_20;
	channel_pre[SSH_CHANNEL_PORT_LISTENER] =	&channel_pre_listener;
	channel_pre[SSH_CHANNEL_RPORT_LISTENER] =	&channel_pre_listener;
	channel_pre[SSH_CHANNEL_AUTH_SOCKET] =		&channel_pre_listener;
	channel_pre[SSH_CHANNEL_CONNECTING] =		&channel_pre_connecting;
	channel_pre[SSH_CHANNEL_DYNAMIC] =		&channel_pre_dynamic;

	channel_post[SSH_CHANNEL_OPEN] =		&channel_post_open_2;
	channel_post[SSH_CHANNEL_PORT_LISTENER] =	&channel_post_port_listener;
	channel_post[SSH_CHANNEL_RPORT_LISTENER] =	&channel_post_port_listener;
	channel_post[SSH_CHANNEL_AUTH_SOCKET] =		&channel_post_auth_listener;
	channel_post[SSH_CHANNEL_CONNECTING] =		&channel_post_connecting;
	channel_post[SSH_CHANNEL_DYNAMIC] =		&channel_post_open_2;
}

/* gc dead channels */
static VOID
channel_garbage_collect(Channel *c)
{
	if (c == NULL)
		return;
	if (c->detach_user != NULL) {
		if (!chan_is_dead(c, 0))
			return;
		debug("channel %d: gc: notify user", c->self);
		c->detach_user(c->self, NULL);
		/* if we still have a callback */
		if (c->detach_user != NULL)
			return;
		debug("channel %d: gc: user detached", c->self);
	}
	if (!chan_is_dead(c, 1))
		return;
	debug("channel %d: garbage collecting", c->self);
	channel_free(c);
}

static VOID
channel_handler(chan_fn *ftab[], fd_set * readset, fd_set * writeset)
{
	static int did_init = 0;
	int i;
	Channel *c;

	if (!did_init) {
		channel_handler_init();
		did_init = 1;
	}
	for (i = 0; i < channels_alloc; i++) {
		c = channels[i];
		if (c == NULL)
			continue;
		if (ftab[c->type] != NULL)
			(*ftab[c->type])(c, readset, writeset);
		channel_garbage_collect(c);
	}
}

/*
 * Allocate/update select bitmasks and add any bits relevant to channels in
 * select bitmasks.
 */
VOID
channel_prepare_select(fd_set **readsetp, fd_set **writesetp, int *maxfdp,
    int *nallocp, int rekeying)
{
	int n;
	u_int sz;

	n = MAX(*maxfdp, channel_max_fd);

	sz = howmany(n+1, NFDBITS) * sizeof(fd_mask);
	/* perhaps check sz < nalloc/2 and shrink? */
	if (*readsetp == NULL || sz > *nallocp) {
		*readsetp  = (fd_set *)xrealloc((LONG*)*readsetp,  sz);
		*writesetp = (fd_set *)xrealloc((LONG*)*writesetp, sz);
		*nallocp = sz;
	}
	*maxfdp = n;
	memset(*readsetp, 0, sz);
	memset(*writesetp, 0, sz);

	if (!rekeying)
		channel_handler(channel_pre, *readsetp, *writesetp);
}

/*
 * After select, perform any appropriate operations for channels which have
 * events pending.
 */
VOID
channel_after_select(fd_set * readset, fd_set * writeset)
{
	channel_handler(channel_post, readset, writeset);
}


/* If there is data to send to the connection, enqueue some of it now. */

VOID
channel_output_poll()
{
	int len, i;
	Channel *c;

	for (i = 0; i < channels_alloc; i++) {
		c = channels[i];
		if (c == NULL)
			continue;

		/*
		 * We are only interested in channels that can have buffered
		 * incoming data.
		 */
		if (c->type != SSH_CHANNEL_OPEN)
		   continue;

		if ((c->flags & (CHAN_CLOSE_SENT|CHAN_CLOSE_RCVD))) {
			/* XXX is this true? */
			debug3("channel %d: will not send data after close", c->self);
			continue;
		}

		/* Get the amount of buffered data for this channel. */
		if ((c->istate == CHAN_INPUT_OPEN ||
		    c->istate == CHAN_INPUT_WAIT_DRAIN) &&
		    (len = buffer_len(&c->input)) > 0) {
			/*
			 * Send some data for the other side over the secure
			 * connection.
			 */
         if (len > c->remote_window)
            len = c->remote_window;
         if (len > c->remote_maxpacket)
            len = c->remote_maxpacket;
			if (len > 0) {
				packet_start(SSH2_MSG_CHANNEL_DATA);
				packet_put_int(c->remote_id);
				packet_put_string(buffer_ptr(&c->input), len);
				packet_send();
				buffer_consume(&c->input, len);
				c->remote_window -= len;
			}
		} else if (c->istate == CHAN_INPUT_WAIT_DRAIN) {
			/*
			 * input-buffer is empty and read-socket shutdown:
			 * tell peer, that we will not send more data: send IEOF
			 */
			chan_ibuf_empty(c);
		}
		/* Send extended data, i.e. stderr */
		if (c->remote_window > 0 &&
		    (len = buffer_len(&c->extended)) > 0 &&
		    c->extended_usage == CHAN_EXTENDED_READ) {
			debug2("channel %d: rwin %d elen %d euse %d",
			    c->self, c->remote_window, buffer_len(&c->extended),
			    c->extended_usage);
			if (len > c->remote_window)
				len = c->remote_window;
			if (len > c->remote_maxpacket)
				len = c->remote_maxpacket;
			packet_start(SSH2_MSG_CHANNEL_EXTENDED_DATA);
			packet_put_int(c->remote_id);
			packet_put_int(SSH2_EXTENDED_DATA_STDERR);
			packet_put_string(buffer_ptr(&c->extended), len);
			packet_send();
			buffer_consume(&c->extended, len);
			c->remote_window -= len;
			debug2("channel %d: sent ext data %d", c->self, len);
		}
	}
}


/* -- protocol input */

VOID
channel_input_data(int type, int plen, LONG* ctxt)
{
	int id;
	char *data;
	u_int data_len;
	Channel *c;

	/* Get the channel number and verify it. */
	id = packet_get_int();
	c = channel_lookup(id);
	if (c == NULL)
		packet_disconnect("Received data for nonexistent channel %d.", id);

	/* Ignore any data for non-open channels (might happen on close) */
	if (c->type != SSH_CHANNEL_OPEN)
		return;

	/* same for protocol 1.5 if output end is no longer open */
	if (c->ostate != CHAN_OUTPUT_OPEN)
		return;

	/* Get the data. */
	data = packet_get_string(&data_len);
	packet_done();

   if (data_len > c->local_maxpacket) {
      log("channel %d: rcvd big packet %d, maxpack %d",
          c->self, data_len, c->local_maxpacket);
   }
   if (data_len > c->local_window) {
      log("channel %d: rcvd too much data %d, win %d",
          c->self, data_len, c->local_window);
      xfree((LONG*)data);
      return;
   }
   c->local_window -= data_len;
	buffer_append(&c->output, data, data_len);
	xfree((LONG*)data);
}

VOID
channel_input_extended_data(int type, int plen, LONG* ctxt)
{
	int id;
	int tcode;
	char *data;
	u_int data_len;
	Channel *c;

	/* Get the channel number and verify it. */
	id = packet_get_int();
	c = channel_lookup(id);

	if (c == NULL)
		packet_disconnect("Received extended_data for bad channel %d.", id);
	if (c->type != SSH_CHANNEL_OPEN) {
		log("channel %d: ext data for non open", id);
		return;
	}
	tcode = packet_get_int();
	if (c->efd < 0 ||
	    c->extended_usage != CHAN_EXTENDED_WRITE ||
	    tcode != SSH2_EXTENDED_DATA_STDERR) {
		log("channel %d: bad ext data", c->self);
		return;
	}
	data = packet_get_string(&data_len);
	packet_done();
	if (data_len > c->local_window) {
		log("channel %d: rcvd too much extended_data %d, win %d",
		    c->self, data_len, c->local_window);
		xfree((LONG*)data);
		return;
	}
	debug2("channel %d: rcvd ext data %d", c->self, data_len);
	c->local_window -= data_len;
	buffer_append(&c->extended, data, data_len);
	xfree((LONG*)data);
}

VOID
channel_input_ieof(int type, int plen, LONG* ctxt)
{
	int id;
	Channel *c;

	packet_integrity_check(plen, 4, type);

	id = packet_get_int();
	c = channel_lookup(id);
	if (c == NULL)
		packet_disconnect("Received ieof for nonexistent channel %d.", id);
	chan_rcvd_ieof(c);

	/* XXX force input close */
	if (c->force_drain) {
		debug("channel %d: FORCE input drain", c->self);
		c->istate = CHAN_INPUT_WAIT_DRAIN;
	}

}

/* proto version 1.5 overloads CLOSE_CONFIRMATION with OCLOSE */
VOID
channel_input_oclose(int type, int plen, LONG* ctxt)
{
	int id = packet_get_int();
	Channel *c = channel_lookup(id);
	packet_integrity_check(plen, 4, type);
	if (c == NULL)
		packet_disconnect("Received oclose for nonexistent channel %d.", id);
	chan_rcvd_oclose(c);
}

VOID
channel_input_open_confirmation(int type, int plen, LONG* ctxt)
{
	int id, remote_id;
	Channel *c;

	id = packet_get_int();
	c = channel_lookup(id);

	if (c==NULL || c->type != SSH_CHANNEL_OPENING)
		packet_disconnect("Received open confirmation for "
		    "non-opening channel %d.", id);
	remote_id = packet_get_int();
	/* Record the remote channel number and mark that the channel is now open. */
	c->remote_id = remote_id;
	c->type = SSH_CHANNEL_OPEN;

   c->remote_window = packet_get_int();
   c->remote_maxpacket = packet_get_int();
   packet_done();
   if (c->cb_fn != NULL && c->cb_event == type) {
      debug2("callback start");
      c->cb_fn(c->self, c->cb_arg);
      debug2("callback done");
   }
   debug("channel %d: open confirm rwindow %d rmax %d", c->self,
       c->remote_window, c->remote_maxpacket);
}

static char *
reason2txt(int reason)
{
	switch(reason) {
	case SSH2_OPEN_ADMINISTRATIVELY_PROHIBITED:
		return "administratively prohibited";
	case SSH2_OPEN_CONNECT_FAILED:
		return "connect failed";
	case SSH2_OPEN_UNKNOWN_CHANNEL_TYPE:
		return "unknown channel type";
	case SSH2_OPEN_RESOURCE_SHORTAGE:
		return "resource shortage";
	}
	return "unknown reason";
}

VOID
channel_input_open_failure(int type, int plen, LONG* ctxt)
{
	int id, reason;
	char *msg = NULL, *lang = NULL;
	Channel *c;

	id = packet_get_int();
	c = channel_lookup(id);

	if (c==NULL || c->type != SSH_CHANNEL_OPENING)
		packet_disconnect("Received open failure for "
		    "non-opening channel %d.", id);
   reason = packet_get_int();
   if (!(datafellows & SSH_BUG_OPENFAILURE)) {
      msg  = packet_get_string(NULL);
      lang = packet_get_string(NULL);
   }
   packet_done();
   log("channel %d: open failed: %s%s%s", id,
       reason2txt(reason), msg ? (char *)": (char *)": (char *)"",
       msg ? msg : (char *)"");
   if (msg != NULL)
      xfree((LONG*)msg);
   if (lang != NULL)
      xfree((LONG*)lang);
	/* Free the channel.  This will also close the socket. */
	channel_free(c);
}

VOID
channel_input_channel_request(int type, int plen, LONG* ctxt)
{
	int id;
	Channel *c;

	id = packet_get_int();
	c = channel_lookup(id);

	if (c == NULL ||
	    (c->type != SSH_CHANNEL_OPEN && c->type != SSH_CHANNEL_LARVAL))
		packet_disconnect("Received request for "
		    "non-open channel %d.", id);
	if (c->cb_fn != NULL && c->cb_event == type) {
		debug2("callback start");
		c->cb_fn(c->self, c->cb_arg);
		debug2("callback done");
	} else {
		char *service = packet_get_string(NULL);
		debug("channel %d: rcvd request for %s", c->self, service);
		debug("cb_fn %p cb_event %d", c->cb_fn , c->cb_event);
		xfree((LONG*)service);
	}
}

VOID
channel_input_window_adjust(int type, int plen, LONG* ctxt)
{
	Channel *c;
	int id, adjust;

	/* Get the channel number and verify it. */
	id = packet_get_int();
	c = channel_lookup(id);

	if (c == NULL || c->type != SSH_CHANNEL_OPEN) {
		log("Received window adjust for "
		    "non-open channel %d.", id);
		return;
	}
	adjust = packet_get_int();
	packet_done();
	debug2("channel %d: rcvd adjust %d", id, adjust);
	c->remote_window += adjust;
}

/* -- tcp forwarding */

VOID
channel_set_af(int af)
{
	IPv4or6 = af;
}

