/*
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
RCSID("$OpenBSD: auth2.c,v 1.72 2001/11/07 22:41:51 markus Exp $");

#include "openssl/evp.h"

#include "ssh2.h"
#include "xmalloc.h"
#include "packet.h"
#include "buffer.h"
#include "log.h"
#include "servconf.h"
#include "compat.h"
#include "channels.h"
#include "bufaux.h"
#include "auth.h"
#include "session.h"
#include "dispatch.h"
#include "key.h"
#include "cipher.h"
#include "kex.h"
#include "pathnames.h"
#include "auth-options.h"
#include "misc.h"
#include "canohost.h"
#include "match.h"
#include "addmore.h"

/* import */
extern ServerOptions options;
extern u_char *session_id2;
extern int session_id2_len;

static Authctxt	*x_authctxt = NULL;
static int one = 1;

typedef struct Authmethod Authmethod;
struct Authmethod {
	char	*name;
	int	(*userauth)(Authctxt *authctxt);
	int	*enabled;
};

/* protocol */

static VOID input_service_request(int, int, LONG*);
static VOID input_userauth_request(int, int, LONG*);
static VOID protocol_error(int, int, LONG*);

/* helper */
static Authmethod *authmethod_lookup(const char *);
static char *authmethods_get(VOID);
#ifndef TOPS_PORT
static int user_key_allowed(struct passwd *, Key *);
static int hostbased_key_allowed(struct passwd *, const char *, char *, Key *);
#endif

VOID setproctitle( const char * fmt, ... );

/* auth */
static int userauth_none(Authctxt *);
static int userauth_passwd(Authctxt *);
#ifndef TOPS_PORT
static int userauth_pubkey(Authctxt *);
static int userauth_hostbased(Authctxt *);
static int userauth_kbdint(Authctxt *);
#endif

Authmethod authmethods[] = {
	{"none",
		userauth_none,
		&one},
	{"password",
		userauth_passwd,
		&options.password_authentication},
	{NULL, NULL, NULL}
};

/*
 * loop until authctxt->success == TRUE
 */

VOID
do_authentication2()
{
	Authctxt *authctxt = authctxt_new();

	x_authctxt = authctxt;		/*XXX*/

	dispatch_init(&protocol_error);
	dispatch_set(SSH2_MSG_SERVICE_REQUEST, &input_service_request);
	dispatch_run(DISPATCH_BLOCK, &authctxt->success, (LONG*)authctxt);
	do_authenticated(authctxt);
}

static VOID
protocol_error(int type, int plen, LONG* ctxt)
{
	log("auth: protocol error: type %d plen %d", type, plen);
	packet_start(SSH2_MSG_UNIMPLEMENTED);
	packet_put_int(0);
	packet_send();
	packet_write_wait();
}

static VOID
input_service_request(int type, int plen, LONG* ctxt)
{
	Authctxt *authctxt = (Authctxt*)ctxt;
	u_int len;
	int accept = 0;
	char *service = packet_get_string(&len);
	packet_done();

	if (authctxt == NULL)
		fatal("input_service_request: no authctxt");

	/* printf("input_service_request: service '%s'\r\n", service); */

	if (strcmp(service, "ssh-userauth") == 0) {
		if (!authctxt->success) {
			accept = 1;
			/* now we can handle user-auth requests */
			dispatch_set(SSH2_MSG_USERAUTH_REQUEST, &input_userauth_request);
		}
	}
	/* XXX all other service requests are denied */

	if (accept) {
		packet_start(SSH2_MSG_SERVICE_ACCEPT);
		packet_put_cstring(service);
		packet_send();
		packet_write_wait();
	} else {
		debug("bad service request %s", service);
		packet_disconnect("bad service request %s", service);
	}
	xfree((LONG*)service);
}

struct tops_passwd *setup_tops_passwd(Authctxt *authctxt);

static VOID
input_userauth_request(int type, int plen, LONG* ctxt)
{
	Authctxt *authctxt = (Authctxt*)ctxt;
	Authmethod *m = NULL;
	UNS8 *user, *service, *method, *style = NULL;
	int authenticated = 0;

	if (authctxt == NULL)
		fatal("input_userauth_request: no authctxt");

	user = packet_get_string(NULL);
	service = packet_get_string(NULL);
	method = packet_get_string(NULL);
	debug("userauth-request for user %s service %s method %s", user, service, method);
	debug("attempt %d failures %d", authctxt->attempt, authctxt->failures);

	if ((style = strchr(user, ':')) != NULL)
		*style++ = 0;

	if (authctxt->attempt++ == 0) {
		/* setup auth context */
		authctxt->valid = 1;
		setup_tops_passwd(authctxt);
		/* if the following call is suppressed the forked child somehow crashes
		* before it can be stopped in the gdb debugger
		* -- if you dont want the debugger - you can take out the call to
		* allowed_user - internally it generates a lot of calls to
		* auth access stuff on Linux - like NIS
		*/

		setproctitle("%s", user );
		authctxt->user = xstrdup(user);
		authctxt->service = xstrdup(service);
		authctxt->style = style ? xstrdup(style) : NULL;
	} else if (strcmp(user, authctxt->user) != 0 ||
	    strcmp(service, authctxt->service) != 0) {
		packet_disconnect("Change of username or service not allowed: "
		    "(%s,%s) -> (%s,%s)",
		    authctxt->user, authctxt->service, user, service);
	}
	/* reset state */
	dispatch_set(SSH2_MSG_USERAUTH_INFO_RESPONSE, &protocol_error);
	authctxt->postponed = 0;

	/* try to authenticate user */
	m = authmethod_lookup(method);
	if (m != NULL) {
		debug2("input_userauth_request: try method %s", method);
		authenticated =	m->userauth(authctxt);
	}
	userauth_finish(authctxt, authenticated, method);

	xfree((LONG*)service);
	xfree((LONG*)user);
	xfree((LONG*)method);
}

VOID
userauth_finish(Authctxt *authctxt, int authenticated, char *method)
{
	char *methods;

	if (!authctxt->valid && authenticated)
		fatal("INTERNAL ERROR: authenticated invalid user %s",
		    authctxt->user);

	/* Log before sending the reply */
	auth_log(authctxt, authenticated, method, " ssh2");

	if (authctxt->postponed)
		return;

	/* XXX todo: check if multiple auth methods are needed */
	if (authenticated == 1) {
		/* turn off userauth */
		dispatch_set(SSH2_MSG_USERAUTH_REQUEST, &protocol_error);
		packet_start(SSH2_MSG_USERAUTH_SUCCESS);
		packet_send();
		packet_write_wait();
		/* now we can break out */
		authctxt->success = 1;
	} else {
		if (authctxt->failures++ > AUTH_FAIL_MAX) {
			packet_disconnect(AUTH_FAIL_MSG, authctxt->user);
		}
		methods = authmethods_get();
		packet_start(SSH2_MSG_USERAUTH_FAILURE);
		packet_put_cstring(methods);
		packet_put_char(0);	/* XXX partial success, unused */
		packet_send();
		packet_write_wait();
		xfree((LONG*)methods);
	}
}

static int
userauth_none(Authctxt *authctxt)
{
	/* disable method "none", only allowed one time */
	Authmethod *m = authmethod_lookup("none");
	if (m != NULL)
		m->enabled = NULL;
	packet_done();

	if (authctxt->valid == 0)
		return(0);

	return auth_password(authctxt, "");
}

static int
userauth_passwd(Authctxt *authctxt)
{
	char *password;
	int authenticated = 0;
	int change;
	u_int len;
	change = packet_get_char();
	if (change)
		log("password change not supported");
	password = packet_get_string(&len);
	packet_done();
	if (authctxt->valid &&
	   auth_password(authctxt, password) == 1)
   authenticated = 1;
	memset(password, 0, len);
	xfree((LONG*)password);
	return authenticated;
}

/* get current user */

#ifdef TOPS_PORT
struct tops_passwd *
#else
struct passwd*
#endif
auth_get_user(VOID)
{
	return (x_authctxt != NULL && x_authctxt->valid) ? x_authctxt->pw : NULL;
}

#define	DELIM	","

static char *
authmethods_get(VOID)
{
	Authmethod *method = NULL;
	u_int size = 0;
	char *list;

	for (method = authmethods; method->name != NULL; method++) {
		if (strcmp(method->name, "none") == 0)
			continue;
		if (method->enabled != NULL && *(method->enabled) != 0) {
			if (size != 0)
				size += strlen(DELIM);
			size += strlen(method->name);
		}
	}
	size++;			/* trailing '\0' */
	list = (char *)xmalloc(size);
	list[0] = '\0';

	for (method = authmethods; method->name != NULL; method++) {
		if (strcmp(method->name, "none") == 0)
			continue;
		if (method->enabled != NULL && *(method->enabled) != 0) {
			if (list[0] != '\0')
				strlcat(list, DELIM, size);
			strlcat(list, method->name, size);
		}
	}
	return list;
}

static Authmethod *
authmethod_lookup(const char *name)
{
	Authmethod *method = NULL;
	if (name != NULL)
		for (method = authmethods; method->name != NULL; method++)
			if (method->enabled != NULL &&
			    *(method->enabled) != 0 &&
			    strcmp(name, method->name) == 0)
				return method;
	debug2("Unrecognized authentication method name: %s",
	       name ? name : (char *)"NULL");
	return NULL;
}

