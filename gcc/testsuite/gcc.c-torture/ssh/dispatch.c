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
RCSID("$OpenBSD: dispatch.c,v 1.11 2001/06/10 11:29:20 markus Exp $");

#include "ssh1.h"
#include "ssh2.h"
#include "log.h"
#include "dispatch.h"
#include "packet.h"
#include "compat.h"

#define DISPATCH_MIN	0
#define DISPATCH_MAX	255

dispatch_fn *dispatch[DISPATCH_MAX];

VOID
dispatch_protocol_error(int type, int plen, LONG* ctxt)
{
	fatal("dispatch_protocol_error: type %d plen %d", type, plen);
}
VOID
dispatch_init(dispatch_fn *dflt)
{
	int i;
	for (i = 0; i < DISPATCH_MAX; i++)
		dispatch[i] = dflt;
}
VOID
dispatch_set(int type, dispatch_fn *fn)
{
	dispatch[type] = fn;
}
VOID
dispatch_run(int mode, int *done, LONG* ctxt)
{
	for (;;) {
		int plen;
		int type;

		if (mode == DISPATCH_BLOCK) {
			type = packet_read(&plen);
		} else {
			type = packet_read_poll(&plen);
			if (type == SSH_MSG_NONE)
				return;
		}
		if (type > 0 && type < DISPATCH_MAX && dispatch[type] != NULL)
			(*dispatch[type])(type, plen, ctxt);
		else
			packet_disconnect("protocol error: rcvd type %d", type);
		if (done != NULL && *done)
			return;
	}
}
