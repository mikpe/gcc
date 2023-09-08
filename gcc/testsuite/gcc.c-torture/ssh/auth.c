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
RCSID("$OpenBSD: auth.c,v 1.29 2001/11/08 20:02:24 markus Exp $");

#include "xmalloc.h"
#include "match.h"
#include "log.h"
#include "servconf.h"
#include "auth.h"
#include "auth-options.h"
#include "canohost.h"
#include "buffer.h"
#include "bufaux.h"

#include "addmore.h"

/* import */
extern ServerOptions options;

Authctxt *
authctxt_new(VOID)
{
	Authctxt *authctxt = (Authctxt *)xmalloc(sizeof(*authctxt));
	memset(authctxt, 0, sizeof(*authctxt));
	return authctxt;
}

VOID
auth_log(Authctxt *authctxt, int authenticated, char *method, char *info)
{
	VOID (*authlog) (const char *fmt,...) = verbose;
	char *authmsg;

	/* Raise logging level */
	if (authenticated == 1 ||
	    !authctxt->valid ||
	    authctxt->failures >= AUTH_FAIL_LOG ||
	    strcmp(method, "password") == 0)
		authlog = debug;

	if (authctxt->postponed)
		authmsg = "Postponed";
	else
		authmsg = authenticated ? "Accepted" : "Failed";

	authlog("%s %s for %s%.100s from %.200s port %d%s",
	    authmsg,
	    method,
	    authctxt->valid ? "" : "illegal user ",
	    authctxt->user,
	    get_remote_ipaddr(),
	    get_remote_port(),
	    info);
}

