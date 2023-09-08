/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This program is the ssh daemon.  It listens for connections from clients,
 * and performs authentication, executes use commands or shell, and forwards
 * information to/from the application to the user client over an encrypted
 * connection.  This can also handle forwarding of X11, TCP/IP, and
 * authentication agent connections.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 *
 * SSH2 implementation:
 *
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
RCSID("$OpenBSD: sshd.c,v 1.209 2001/11/10 13:19:45 markus Exp $");

#include "openssl/dh.h"
#include "openssl/bn.h"
#include "openssl/hmac.h"

#include "ssh.h"
#include "ssh1.h"
#include "ssh2.h"
#include "xmalloc.h"
#include "packet.h"
#include "mpaux.h"
#include "log.h"
#include "servconf.h"
#include "compat.h"
#include "buffer.h"
#include "cipher.h"
#include "kex.h"
#include "key.h"
#include "myproposal.h"
#include "authfile.h"
#include "pathnames.h"
#include "atomicio.h"
#include "canohost.h"
#include "auth.h"
#include "misc.h"
#include "dispatch.h"
#include "channels.h"

#ifndef O_NOCTTY
#define O_NOCTTY	0
#endif

#include "addmore.h"

/* Server configuration options. */
ServerOptions options;

/*
 * Flag indicating whether IPv4 or IPv6.  This can be set on the command line.
 * Default value is AF_UNSPEC means both IPv4 and IPv6.
 */
int IPv4or6 = AF_INET;

/*
 * Debug mode flag.  This can be set on the command line.  If debug
 * mode is enabled, extra debugging output will be sent to the system
 * log, the daemon will not go to background, and will exit after processing
 * the first connection.
 */
int debug_flag = 1;

/*
 * The sockets that the server is listening
 */
int num_listen_socks = 0;

/*
 * the client's version string, passed by sshd2 in compat mode. if != NULL,
 * sshd will skip the version-number exchange
 */
UNS8 *client_version_string = NULL;
UNS8 *server_version_string = NULL;

/* for rekeying XXX fixme */
Kex *xxx_kex;

/*
 * Any really sensitive data in the application is contained in this
 * structure. The idea is that this structure could be locked into memory so
 * that the pages do not get written into swap.  However, there are some
 * problems. The private key contains BIGNUMs, and we do not (in principle)
 * have access to the internals of them, and locking just the structure is
 * not very useful.  Currently, memory locking is not implemented.
 */
struct {
	Key	*server_key;		/* ephemeral server key */
	Key	*ssh1_host_key;		/* ssh1 host key */
	Key	**host_keys;		/* all private host keys */
	int	have_ssh1_key;
	int	have_ssh2_key;
	u_char	ssh1_cookie[SSH_SESSION_KEY_LENGTH];
} sensitive_data;

/* same for ssh2 */
u_char *session_id2 = NULL;
int session_id2_len = 0;
int datafellows = 0;

/* record remote hostname or ip */
u_int utmp_len = MAXHOSTNAMELEN;

/* Prototypes for various functions defined later in this file. */
VOID destroy_sensitive_data(VOID);

static VOID do_ssh2_kex(VOID);

#ifdef TOPS_PORT
static int sscanfHACK(UNS8 *client_version_string, UNS8 *pattern,
	    int *remote_major, int *remote_minor, char *remote_version)
{
	/* ERR_PortProblem( __FILE__, __LINE__, "HACK - assuming remote version 2.0"); */
	*remote_major = 2;
	*remote_minor = 0;
	*remote_version = 0;
	return 3;
}
#endif

char server[256];
char client[256];

static VOID
sshd_exchange_identification(int sock_in, int sock_out)
{
	int i, mismatch;
	int remote_major, remote_minor;
	char *s;
	UNS8 buf[256];		/* Must not be larger than remote_version. */
	char remote_version[256];	/* Must be at least as big as buf. */

	strcpy(server, ("SSH-2.0-" SSH_VERSION "\n"));
	server_version_string = server;

	if (client_version_string == NULL) {
		/* Send our protocol version identification. */
		if (atomicio(send, sock_out, server_version_string, strlen(server_version_string))
		    != strlen(server_version_string)) {
			log("Could not write ident string to %s", get_remote_ipaddr());
			fatal_cleanup();
		}

		/* Read other side's version identification. */
		memset(buf, 0, sizeof(buf));
		for (i = 0; i < sizeof(buf) - 1; i++) {
			if (atomicio(recv, sock_in, &buf[i], 1) != 1) {
				log("Did not receive identification string from %s",
				    get_remote_ipaddr());
				fatal_cleanup();
			}
			if (buf[i] == '\r') {
				buf[i] = 0;
				/* Kludge for F-Secure Macintosh < 1.0.2 */
				if (i == 12 &&
				    strncmp(buf, "SSH-1.5-W1.0", 12) == 0)
					break;
				continue;
			}
			if (buf[i] == '\n') {
				buf[i] = 0;
				break;
			}
		}
		buf[sizeof(buf) - 1] = 0;
		strcpy(client, buf);
		client_version_string = client;
	}

	/*
	 * Check that the versions match.  In future this might accept
	 * several versions and set appropriate flags to handle them.
	 */
	if (sscanfHACK(client_version_string, "SSH-%d.%d-%[^\n]\n",
	    &remote_major, &remote_minor, remote_version) != 3) {
		s = "Protocol mismatch.\r\n";
		(VOID) atomicio(send, sock_out, s, strlen(s));
		close(sock_in);
		close(sock_out);
		log("Bad protocol version identification '%.100s' from %s",
		    client_version_string, get_remote_ipaddr());
		fatal_cleanup();
	}
	debug("Client protocol version %d.%d; client software version %.100s",
	      remote_major, remote_minor, remote_version);

	if (datafellows & SSH_BUG_SCANNER) {
		log("scanned from %s with %s.  Don't panic.",
		    get_remote_ipaddr(), client_version_string);
		fatal_cleanup();
	}

	mismatch = (remote_major != 2);

	chop(server_version_string);
	debug("Local version string %.200s", server_version_string);

	if (mismatch) {
		s = "Protocol major versions differ.\r\n";
		(VOID) atomicio(send, sock_out, s, strlen(s));
		close(sock_in);
		close(sock_out);
		log("Protocol major versions differ for %s: %.200s vs. %.200s",
		    get_remote_ipaddr(),
		    server_version_string, client_version_string);
		fatal_cleanup();
	}
}

#if 0
/* Destroy the host and server keys.  They will no longer be needed. */
VOID
destroy_sensitive_data(VOID)
{
	int i;

	if (sensitive_data.server_key) {
		key_free(sensitive_data.server_key);
		sensitive_data.server_key = NULL;
	}
	for(i = 0; i < options.num_host_key_files; i++) {
		if (sensitive_data.host_keys[i]) {
			key_free(sensitive_data.host_keys[i]);
			sensitive_data.host_keys[i] = NULL;
		}
	}
	sensitive_data.ssh1_host_key = NULL;
	memset(sensitive_data.ssh1_cookie, 0, SSH_SESSION_KEY_LENGTH);
}
#endif

static char *
list_hostkey_types(VOID)
{
	static char buf[1024];
	int i;
	buf[0] = '\0';
	for(i = 0; i < options.num_host_key_files; i++) {
		Key *key = sensitive_data.host_keys[i];
		if (key == NULL)
			continue;
		switch(key->type) {
		case KEY_DSA:
			strlcat(buf, key_ssh_name(key), sizeof buf);
			strlcat(buf, ",", sizeof buf);
			break;
		}
	}
	i = strlen(buf);
	if (i > 0 && buf[i-1] == ',')
		buf[i-1] = '\0';
	debug("list_hostkey_types: %s", buf);
	return buf;
}

static Key *
get_hostkey_by_type(int type)
{
	int i;
	for(i = 0; i < options.num_host_key_files; i++) {
		Key *key = sensitive_data.host_keys[i];
		if (key != NULL && key->type == type)
			return key;
	}
	return NULL;
}

struct sockaddr_in addr;
fd_set fdset;

/*
 * Main program for the daemon.
 */
int
main(int ac, char **av)
{
	int sock_in = 0, sock_out = 0, i;
	const char *remote_ip;
	int remote_port;
	int listen_sock, maxfd;
	Key *key;

	init_rng();

	/* Fill in default values for options. */
	fill_default_server_options(&options);

	channel_set_af(IPv4or6);

	log_init(options.log_level == -1 ? SYSLOG_LEVEL_INFO : options.log_level,
	         options.log_facility == -1 ? SYSLOG_FACILITY_AUTH : options.log_facility);

	seed_rng();

	debug("sshd version %.100s", SSH_VERSION);

	/* load private host keys */
	sensitive_data.host_keys = (Key**)xmalloc(options.num_host_key_files*sizeof(Key*));
	for(i = 0; i < options.num_host_key_files; i++)
		sensitive_data.host_keys[i] = NULL;
	sensitive_data.server_key = NULL;
	sensitive_data.ssh1_host_key = NULL;
	sensitive_data.have_ssh1_key = 0;
	sensitive_data.have_ssh2_key = 0;

	for(i = 0; i < options.num_host_key_files; i++) {
		key = key_load_private(options.host_key_files[i]);
		sensitive_data.host_keys[i] = key;
		if (key == NULL) {
			error("Could not load host key: %s",
			    options.host_key_files[i]);
			sensitive_data.host_keys[i] = NULL;
			continue;
		}
		switch(key->type){
		case KEY_DSA:
			sensitive_data.have_ssh2_key = 1;
			break;
		}
		debug("private host key: #%d type %d %s", i, key->type,
		    key_type(key));
	}
	if (!sensitive_data.have_ssh2_key) {
		log("sshd: unable to load protocol version 2 host key");
      exit(1);
	}

	/* Initialize the random number generator. */
	arc4random_stir();

	/* Start listening for a socket */

	/* Create socket for listening. */
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);

#ifndef TOPS20      
	if (fcntl(listen_sock, F_SETFL, O_NONBLOCK) < 0) {
	  error("listen_sock O_NONBLOCK: %s", strerror(errno));
	  close(listen_sock);
	  return -1;
	}
	/*
	 * Set socket options.  We try to make the port
	 * reusable and have it close as fast as possible
	 * without waiting in unnecessary wait states on
	 * close.
	 */
	{
	  int on = 1;
	  struct linger linger;

	  setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
		     (VOIDP) &on, sizeof(on));
	  linger.l_onoff = 1;
	  linger.l_linger = 5;
	  setsockopt(listen_sock, SOL_SOCKET, SO_LINGER,
		     (VOIDP) &linger, sizeof(linger));
	}
#endif
	/* Bind the socket to the desired port. */

	addr.sin_family = AF_INET; 
	addr.sin_port = 0xA0A;
 
	if (bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
	  error("Bind to listen port.");
	  close(listen_sock);
	  return -1;
	}
	num_listen_socks++;
	
	/* Start listening on the port.
	log("Server listening."); */
	if (listen(listen_sock, 5) < 0)
	  fatal("listen: %.100s", strerror(errno));

   if (!num_listen_socks)
      fatal("Cannot bind any address.");

   /* JOHN BORCHEK REMEMBER: alarm handling for authentication timeout */
   /* JOHN BORCHEK REMEMBER: signal handling for program termination, 
                             child termination, ignoring broken pipes, etc. */

   /* setup fd set for listen */
   maxfd = 0;
   /* We don't do any select stuff for TOPS20 since the listen has
      taken care of things...so skip all this completely.
   */
#ifdef TOPS20
   sock_in = sock_out = listen_sock;
#else
   maxfd = listen_sock;

   /*
    * Stay listening for connections until the system crashes or
    * the daemon is killed.
    */

   for (;;)
     {

       int ret;
       int newsock;
       pid_t pid;
       socklen_t fromlen;
       struct sockaddr_storage from;

       FD_SET(listen_sock, &fdset);

       /* Wait in select until there is a connection. */
       ret = select(maxfd+1, &fdset, NULL, NULL, NULL);
       if (ret < 0 && errno != EINTR)
         error("select: %.100s", strerror(errno));
       if (ret < 0)
         continue;

       if (FD_ISSET(listen_sock, &fdset))
	 {

         fromlen = sizeof(from);
         newsock = accept(listen_sock, (struct sockaddr *)&from, &fromlen);
         if (newsock < 0) {
	   if (errno != EINTR && errno != EWOULDBLOCK)
	     error("accept: %.100s", strerror(errno));
	   continue;
         }
         if (fcntl(newsock, F_SETFL, 0) < 0) {
	   error("newsock del O_NONBLOCK: %s", strerror(errno));
	   continue;
         }
	 
         /* JOHN BORCHEK REMEMBER: pipe used as signaling mechanism by forked
	    children to indicate that they their 
	    connection has been authenticated */
	 
         /*
          * Got connection.  Fork a child to handle it, unless
          * we are in debugging mode.
          */
         if (debug_flag) {
	   /*
	    * In debugging mode.  Close the listening
	    * socket, and start processing the
	    * connection without forking.
	    */
	   debug("Server will not fork when running in debugging mode.");
	   sock_in = newsock;
	   sock_out = newsock;
	   pid = 1;
	   break;
         } 
#if 0 /* JOHN BORCHEK FIX */
         else {
	   /*
	    * Normal production daemon.  Fork, and have
	    * the child process the connection. The
	    * parent continues listening.
	    */
	   if ((pid = fork()) == 0) {
	     /*
	      * Child.  Close the listening and max_startup
	      * sockets.  Start using the accepted socket.
	      * Reinitialize logging (since our pid has
	      * changed).  We break out of the loop to handle
	      * the connection.
	      */
	     sock_in = newsock;
	     sock_out = newsock;
	     log_init(options.log_level, options.log_facility);
	     break;
	   }
         }
#endif /* 0 */

         /* Parent.  Stay in the loop. */
         if (pid < 0)
	   error("fork: %.100s", strerror(errno));
         else
	   debug("Forked child %d.", pid);
	 
         arc4random_stir();

         /* Close the new socket (the child is now taking care of it). */
	 
	 /* We do not need to close the new socket..since we don't fork */
	 /* --sumit */
         /* close(newsock); */
       }
       /* child process check (or debug mode) */
       if (num_listen_socks < 0)
         break;
     }
   
   /* This is the child processing a new connection. */
   
   /*
    * Set socket options for the connection.  We want the socket to
    * close as fast as possible without waiting for anything.  If the
    * connection is not a socket, these will do nothing.
    */
   {
     struct linger linger;
     /*
     int on = 1;
     setsockopt(sock_in, SOL_SOCKET, SO_REUSEADDR, (VOIDP)&on, sizeof(on));
     */
     linger.l_onoff = 1;
     linger.l_linger = 5;
     setsockopt(sock_in, SOL_SOCKET, SO_LINGER, (VOIDP) &linger, sizeof(linger));
     
     /* Set keepalives if requested.
     if (options.keepalives)
       {
	 int j;
	 char *s;

	 if ((j = setsockopt(sock_in, SOL_SOCKET, SO_KEEPALIVE, (VOIDP)&on,
			     sizeof(on))) < 0)
	   {
	     s = strerror(errno);
	     error("setsockopt SO_KEEPALIVE: %s", s);
	   }
       }
     */
   }
#endif /* TOPS20 */
   /*
    * Register our connection.  This turns encryption off because we do
    * not have a key.
    */
   packet_set_connection(sock_in, sock_out);
   
   remote_port = get_remote_port();
   remote_ip = get_remote_ipaddr();

   /* Log the connection. */
   verbose("Connection from %.500s port %d", remote_ip, remote_port);

   sshd_exchange_identification(sock_in, sock_out);
	
   packet_set_nonblocking();

   /* perform the key exchange */
   /* authenticate user and start session */
   do_ssh2_kex();
   do_authentication2();
   /* The connection has been terminated. */
   verbose("Closing connection to %.100s", remote_ip);
   
   packet_close();
   exit(0);
   return(0);
}


/*
 * SSH2 key exchange: diffie-hellman-group1-sha1
 */
static VOID
do_ssh2_kex(VOID)
{
	Kex *kex;

	if (options.ciphers != NULL) {
		myproposal[PROPOSAL_ENC_ALGS_CTOS] =
		myproposal[PROPOSAL_ENC_ALGS_STOC] = options.ciphers;
	}
	myproposal[PROPOSAL_ENC_ALGS_CTOS] = myproposal[PROPOSAL_ENC_ALGS_CTOS];
	myproposal[PROPOSAL_ENC_ALGS_STOC] = myproposal[PROPOSAL_ENC_ALGS_STOC];

	if (options.macs != NULL) {
		myproposal[PROPOSAL_MAC_ALGS_CTOS] =
		myproposal[PROPOSAL_MAC_ALGS_STOC] = options.macs;
	}
	myproposal[PROPOSAL_SERVER_HOST_KEY_ALGS] = list_hostkey_types();

	/* start key exchange */
	kex = kex_setup(myproposal);
	kex->server = 1;
	kex->client_version_string=client_version_string;
	kex->server_version_string=server_version_string;
	kex->load_host_key=&get_hostkey_by_type;

	xxx_kex = kex;

	dispatch_run(DISPATCH_BLOCK, &kex->done, (LONG*)kex);

	session_id2 = kex->session_id;
	session_id2_len = kex->session_id_len;

#ifdef DEBUG_KEXDH
	/* send 1st encrypted/maced/compressed message */
	packet_start(SSH2_MSG_IGNORE);
	packet_put_cstring("markus");
	packet_send();
	packet_write_wait();
#endif
	debug("KEX done");
}
