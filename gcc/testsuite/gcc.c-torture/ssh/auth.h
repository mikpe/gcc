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
 *
 * $OpenBSD: auth.h,v 1.22 2001/06/26 17:27:22 markus Exp $
 */
#ifndef AUTH_H
#define AUTH_H

#include "key.h"

typedef struct Authctxt Authctxt;
typedef struct KbdintDevice KbdintDevice;

#ifdef TOPS_PORT
struct tops_passwd {
  UNS8 *pw_name;	/* Username.  */
  UNS8 *pw_passwd;	/* Password.  */
};

#define USE_GETPWNAM 1
#undef USE_GETPWNAM
#endif

struct Authctxt {
	int		 success;
	int		 postponed;
	int		 valid;
	int		 attempt;
	int		 failures;
	UNS8		*user;
	UNS8		*service;
#ifdef TOPS_PORT
	struct tops_passwd	*pw;
#else
	struct passwd	*pw;
#endif
	char		*style;
	char		*kbdintctxt;
#ifdef KRB5
	krb5_context	 krb5_ctx;
	krb5_auth_context krb5_auth_ctx;
	krb5_ccache	 krb5_fwd_ccache;
	krb5_principal	 krb5_user;
	char		*krb5_ticket_file;
#endif
};

/*
 * Keyboard interactive device:
 * init_ctx	returns: non NULL upon success 
 * query	returns: 0 - success, otherwise failure 
 * respond	returns: 0 - success, 1 - need further interaction,
 *		otherwise - failure
 */
struct KbdintDevice
{
	const char *name;
	char*	(*init_ctx)	__P((Authctxt*));
	int	(*query)	__P((char *ctx, char **name, char **infotxt,
				u_int *numprompts, char ***prompts,
				u_int **echo_on));
	int	(*respond)	__P((char *ctx, u_int numresp, char **responses));
	VOID	(*free_ctx)	__P((char *ctx));
};

int     auth_rhosts(struct tops_passwd *, const char *);
int	auth_rhosts2(struct tops_passwd *, const char *, const char *, const char *);

int      auth_password(Authctxt *, const char *);

#ifdef KRB5
int	auth_krb5(Authctxt *authctxt, krb5_data *auth, char **client);
int	auth_krb5_tgt(Authctxt *authctxt, krb5_data *tgt);
int	auth_krb5_password(Authctxt *authctxt, const char *password);
VOID	krb5_cleanup_proc(char *authctxt);
#endif /* KRB5 */




VOID	do_authentication(VOID);
VOID	do_authentication2(VOID);

Authctxt *authctxt_new(VOID);
VOID	auth_log(Authctxt *, int, char *, char *);
VOID	userauth_finish(Authctxt *, int, char *);

int	auth2_challenge(Authctxt *, char *);

char	*get_challenge(Authctxt *);
int	verify_response(Authctxt *, const char *);

#ifdef TOPS_PORT
struct tops_passwd * auth_get_user(VOID);
#else
struct passwd * auth_get_user(VOID);
#endif

#define AUTH_FAIL_MAX 6
#define AUTH_FAIL_LOG (AUTH_FAIL_MAX/2)
#define AUTH_FAIL_MSG "Too many authentication failures for %.100s"

#endif
