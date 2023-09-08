/*
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
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
RCSID("$OpenBSD: session.c,v 1.108 2001/10/11 13:45:21 markus Exp $");

#include "ssh.h"
#include "ssh1.h"
#include "ssh2.h"
#include "xmalloc.h"
#include "packet.h"
#include "buffer.h"
#include "mpaux.h"
#include "compat.h"
#include "channels.h"
#include "bufaux.h"
#include "auth.h"
#include "auth-options.h"
#include "pathnames.h"
#include "log.h"
#include "servconf.h"
#include "sshlogin.h"
#include "serverloop.h"
#include "canohost.h"
#include "session.h"

#ifdef _AIX
# include <uinfo.h>
#endif

#include "addmore.h"

/* types */

#define TTYSZ 64
typedef struct Session Session;
struct Session {
	int	used;
	int	self;
#ifdef TOPS_PORT
	struct tops_passwd	*pw;
#else
	struct passwd	*pw;
#endif
	Authctxt *authctxt;
	pid_t	pid;
	int	chanid;
};

/* func */

Session *session_new(VOID);
VOID	session_set_fds(Session *, int, int, int);
VOID	do_exec_no_pty(Session *, const char *);
VOID	do_exec(Session *, const char *);

static VOID do_authenticated2(Authctxt *);

static VOID session_close(Session *);

/* import */
extern ServerOptions options;
extern int debug_flag;
extern u_int utmp_len;
extern VOID destroy_sensitive_data(VOID);

/* original command from peer. */
const char *original_command = NULL;

/* data */
#define MAX_SESSIONS 10
Session	sessions[MAX_SESSIONS];

#ifdef HAVE_LOGIN_CAP
static login_cap_t *lc;
#endif

VOID
do_authenticated(Authctxt *authctxt)
{
	do_authenticated2(authctxt);
}



#ifdef TOPS_PORT
/*
static VOID print_bytes( UNS8 *d, int L)
{
	int i;
	for(i=0; i<64 && i<L; i+=8 )
	{
		fprintf(stderr, "%02x %02x %02x %02x %02x %02x %02x %02x \r\n",
			d[i],
			d[i+1],
			d[i+2],
			d[i+3],
			d[i+4],
			d[i+5],
			d[i+6],
			d[i+7] );
	}
}
*/
int telnet_input_filter( struct Channel *c, UNS8 *data, int L)
{
	UNS8 seq[4];
	UNS8 *d = data;
	UNS8 pdata[16*1024];  /* the max size of data */
	UNS8 *pd;
	int i, j;
	debug("channel %d data input %d\r\n", c->self, L );
	/*
	print_bytes( d, L );
	*/
	if ( c->self == 0 )
	{
		i = 0;
		j = 0;
		d = data;
		pd = pdata;
		while ( i< L )
		{
			/*--------------------------------------------------
			* UNS8 opcode[4];
			*--------------------------------------------------*/
			UNS8 v = *d;
			switch ( v ) 
			{
				case 0xff:
				{
					d++; i++;
					v = *d;
					if ( v == 0xff )
					{
						*pd++ = v; j++;
					}
					else if ( v >= 251 )
					{
						/*--------------------------------------------------
						* opcode[0] = *d++; i++;
						*--------------------------------------------------*/
						d++; i++;
						if ( v == 253 ) /* do */
						{
							/* reply 252  WONT */
							debug("do %d\r\n", *d );
							seq[0] = 0xff;
							seq[1] = 252;
							seq[2] = *d;
							buffer_append(&c->output, seq, 3 );
							d++; i++;
						}
						if ( v == 251 ) /* WILL */
						{
							/* reply 254 DONT */
							debug("WILL %d\r\n", *d );
							if (*d == 1) { 
							/* if ECHO reply
							 * do */
							seq[0] = 0xff;
							seq[1] = 253;
							seq[2] = 1;
							} 
							else 
							{	
							seq[0] = 0xff;
							seq[1] = 254;
							seq[2] = *d;
							}
							buffer_append(&c->output, seq, 3 );
							d++; i++;
						}
					}
					else if ( v == 250 ) /* SB */
					{
						while ( *d != 240 ) { d++; i++; }
					}
					else if ( v > 240 ) /* commands */
					{
						d++; i++;
					}
					else
					{
						d++; i++;
					}
				}
				break;

				default:
				{
					*pd++ = *d++;
					i++;
					j++;
				}
			} /* end switch */
		} /* end while */
		debug("channel %d data filtered %d\r\n", c->self, j );
		/*
		print_bytes( pdata, j );
		*/
		buffer_append(&c->input, pdata, j);
		return j;
	}
	else
	{
		debug("channel %d data not-filtered\r\n", c->self );
		buffer_append(&c->input, data, L);
		return L;
	}
}

int
makeTelnetConnection();

static VOID
do_exec_no_pty_to_telnet(Session *s, const char *command)
{
	int fd;

	fd = makeTelnetConnection();
	if ( fd < 0 )
	{
		error("telnet connection failed: %.100s", strerror(errno));
		exit(errno);
	}

	debug("telnet connection success: %.100s", strerror(errno));
	s->pid = 0;

	/* Set interactive/non-interactive mode. */
	packet_set_interactive(1);

#if 1
		/* JOHN BORCHEK FIX: behavior on telnet server's close */
		session_set_fds(s, fd, fd, -1);
#else
		/* this is an attempt to have server 
		* send close on telnet connection close 
		* - does not work */
	channel_set_fds(s->chanid,
	    fd, fd, -1,
	    CHAN_EXTENDED_READ,
	    1);
#endif
}
#endif

VOID
do_exec_no_pty(Session *s, const char *command)
{
   do_exec_no_pty_to_telnet(s, command);
}

/*
 * This is called to fork and execute a command.  If another command is
 * to be forced, execute that instead.
 */
VOID
do_exec(Session *s, const char *command)
{
	do_exec_no_pty(s, command);

	original_command = NULL;
}

Session *
session_new(VOID)
{
	int i;
	static int did_init = 0;
	if (!did_init) {
		debug("session_new: init");
		for(i = 0; i < MAX_SESSIONS; i++) {
			sessions[i].used = 0;
		}
		did_init = 1;
	}
	for(i = 0; i < MAX_SESSIONS; i++) {
		Session *s = &sessions[i];
		if (! s->used) {
			memset(s, 0, sizeof(*s));
			s->chanid = -1;
			s->used = 1;
			s->self = i;
			debug("session_new: session %d", i);
			return s;
		}
	}
	return NULL;
}

static VOID
session_dump(VOID)
{
	int i;
	for(i = 0; i < MAX_SESSIONS; i++) {
		Session *s = &sessions[i];
		debug("dump: used %d session %d %p channel %d pid %d",
		    s->used,
		    s->self,
		    s,
		    s->chanid,
		    s->pid);
	}
}

int
session_open(Authctxt *authctxt, int chanid)
{
	Session *s = session_new();
	debug("session_open: channel %d", chanid);
	if (s == NULL) {
		error("no more sessions");
		return 0;
	}
	s->authctxt = authctxt;
	s->pw = authctxt->pw;
	if (s->pw == NULL)
		fatal("no user for session %d", s->self);
	debug("session_open: session %d: link with channel %d", s->self, chanid);
	s->chanid = chanid;
	return 1;
}

static Session *
session_by_channel(int id)
{
	int i;
	for(i = 0; i < MAX_SESSIONS; i++) {
		Session *s = &sessions[i];
		if (s->used && s->chanid == id) {
			debug("session_by_channel: session %d channel %d", i, id);
			return s;
		}
	}
	debug("session_by_channel: unknown channel %d", id);
	session_dump();
	return NULL;
}

static Session *
session_by_pid(pid_t pid)
{
	int i;
	debug("session_by_pid: pid %d", pid);
	for(i = 0; i < MAX_SESSIONS; i++) {
		Session *s = &sessions[i];
		if (s->used && s->pid == pid)
			return s;
	}
	error("session_by_pid: unknown pid %d", pid);
	session_dump();
	return NULL;
}

static int
session_shell_req(Session *s)
{
	packet_done();
	do_exec(s, NULL);
	return 1;
}

VOID
session_input_channel_req(int id, VOIDP arg)
{
	u_int len;
	int reply;
	int success = 0;
	UNS8 *rtypeu;
	Session *s;
	Channel *c;

	rtypeu = packet_get_string(&len);
	reply = packet_get_char();

	s = session_by_channel(id);
	if (s == NULL)
		fatal("session_input_channel_req: channel %d: no session", id);
	c = channel_lookup(id);
	if (c == NULL)
		fatal("session_input_channel_req: channel %d: bad channel", id);

	debug("session_input_channel_req: session %d channel %d request %s reply %d",
	    s->self, id, rtypeu, reply);

	/*
	 * a session is in LARVAL state until a shell, a command
	 * or a subsystem is executed
	 */
	if (c->type == SSH_CHANNEL_LARVAL) {
		if (strcmp(rtypeu, "shell") == 0) {
			success = session_shell_req(s);
		} 
   }

	if (reply) {
		packet_start(success ?
		    SSH2_MSG_CHANNEL_SUCCESS : SSH2_MSG_CHANNEL_FAILURE);
		packet_put_int(c->remote_id);
		packet_send();
	}
	xfree((LONG*)rtypeu);
}

VOID
session_set_fds(Session *s, int fdin, int fdout, int fderr)
{
	/*
	 * now that have a child and a pipe to the child,
	 * we can activate our channel and register the fd's
	 */
	if (s->chanid < 0)
		fatal("no channel for session %d", s->self);
	channel_set_fds(s->chanid,
	    fdout, fdin, fderr,
	    fderr < 0 ? CHAN_EXTENDED_IGNORE : CHAN_EXTENDED_READ,
	    1);
}

/*
 * Function to perform pty cleanup. Also called if we get aborted abnormally
 * (e.g., due to a dropped connection).
 */

static VOID
session_exit_message(Session *s, int status)
{
	Channel *c;
	if (s == NULL)
		fatal("session_close: no session");
	c = channel_lookup(s->chanid);
	if (c == NULL)
		fatal("session_exit_message: session %d: no channel %d",
		    s->self, s->chanid);
	debug("session_exit_message: session %d channel %d pid %d",
	    s->self, s->chanid, s->pid);

	channel_request_start(s->chanid, "exit-status", 0);
	packet_put_int(0);
	packet_send();

	/* disconnect channel */
	debug("session_exit_message: release channel %d", s->chanid);
	channel_cancel_cleanup(s->chanid);
	/*
	 * emulate a write failure with 'chan_write_failed', nobody will be
	 * interested in data we write.
	 * Note that we must not call 'chan_read_failed', since there could
	 * be some more data waiting in the pipe.
	 */
	if (c->ostate != CHAN_OUTPUT_CLOSED)
		chan_write_failed(c);
	s->chanid = -1;
}

static VOID
session_close(Session *s)
{
	debug("session_close: session %d pid %d", s->self, s->pid);
	s->used = 0;
}

VOID
session_close_by_pid(pid_t pid, int status)
{
	Session *s = session_by_pid(pid);
	if (s == NULL) {
		debug("session_close_by_pid: no session for pid %d", pid);
		return;
	}
	if (s->chanid != -1)
		session_exit_message(s, status);
	session_close(s);
}

/*
 * this is called when a channel dies before
 * the session 'child' itself dies
 */
VOID
session_close_by_channel(int id, VOIDP arg)
{
	Session *s = session_by_channel(id);
	if (s == NULL) {
		debug("session_close_by_channel: no session for id %d", id);
		return;
	}
	debug("session_close_by_channel: channel %d child %d", id, s->pid);
	if (s->pid != 0) {
		debug("session_close_by_channel: channel %d: has child", id);
		/*
		 * delay detach of session, but release pty, since
		 * the fd's to the child are already closed
		 */
		return;
	}
	/* detach by removing callback */
	channel_cancel_cleanup(s->chanid);
	s->chanid = -1;
	session_close(s);
}

VOID
session_destroy_all(VOID)
{
	int i;
	for(i = 0; i < MAX_SESSIONS; i++) {
		Session *s = &sessions[i];
		if (s->used) 
			session_close(s);
	}
}

static VOID
do_authenticated2(Authctxt *authctxt)
{
	server_loop2(authctxt);
}
