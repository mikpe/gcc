/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#include "includes.h"
RCSID("$OpenBSD: auth-options.c,v 1.20 2001/08/30 20:36:34 stevesk Exp $");

#include "packet.h"
#include "xmalloc.h"
#include "match.h"
#include "log.h"
#include "canohost.h"
#include "channels.h"
#include "auth-options.h"
#include "servconf.h"
#include "misc.h"

extern ServerOptions options;

