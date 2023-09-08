/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Simple pattern matching, with '*' and '?' as wildcards.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */
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
RCSID("$OpenBSD: match.c,v 1.14 2001/06/27 04:48:53 markus Exp $");

#include "match.h"
#include "xmalloc.h"

#if 0
/*
 * Returns true if the given string matches the pattern (which may contain ?
 * and * as wildcards), and zero if it does not match.
 */

int
match_pattern(const char *s, const char *pattern)
{
	for (;;) {
		/* If at end of pattern, accept if also at end of string. */
		if (!*pattern)
			return !*s;

		if (*pattern == '*') {
			/* Skip the asterisk. */
			pattern++;

			/* If at end of pattern, accept immediately. */
			if (!*pattern)
				return 1;

			/* If next character in pattern is known, optimize. */
			if (*pattern != '?' && *pattern != '*') {
				/*
				 * Look instances of the next character in
				 * pattern, and try to match starting from
				 * those.
				 */
				for (; *s; s++)
					if (*s == *pattern &&
					    match_pattern(s + 1, pattern + 1))
						return 1;
				/* Failed. */
				return 0;
			}
			/*
			 * Move ahead one character at a time and try to
			 * match at each position.
			 */
			for (; *s; s++)
				if (match_pattern(s, pattern))
					return 1;
			/* Failed. */
			return 0;
		}
		/*
		 * There must be at least one more character in the string.
		 * If we are at the end, fail.
		 */
		if (!*s)
			return 0;

		/* Check if the next character of the string is acceptable. */
		if (*pattern != '?' && *pattern != *s)
			return 0;

		/* Move to the next character, both in string and in pattern. */
		s++;
		pattern++;
	}
	/* NOTREACHED */
}
#endif

/*
 * Returns first item from client-list that is also supported by server-list,
 * caller must xfree() returned string.
 */
#define	MAX_PROP	20
#define	SEP	","
char *
match_list(const char *client, const char *server, u_int *next)
{
	char *sproposals[MAX_PROP];
	char *c, *s, *p, *ret, *cp, *sp;
	int i, j, nproposals;

	c = cp = xstrdup(client);
	s = sp = xstrdup(server);

	for ((p = strtok(sp, SEP)), i=0; p && *p != '\0';
	     (p = strtok(NULL, SEP)), i++) {
		if (i < MAX_PROP)
			sproposals[i] = p;
		else
			break;
	}
	nproposals = i;

	for ((p = strtok(cp, SEP)), i=0; p && *p != '\0';
	     (p = strtok(NULL, SEP)), i++) {
		for (j = 0; j < nproposals; j++) {
			if (strcmp(p, sproposals[j]) == 0) {
				ret = xstrdup(p);
#if 0
				if (next != NULL)
					*next = (cp == NULL) ?
					    strlen(c) : cp - c;
#endif
				xfree((LONG*)c);
				xfree((LONG*)s);
				return ret;
			}
		}
	}
	if (next != NULL)
		*next = strlen(c);
	xfree((LONG*)c);
	xfree((LONG*)s);
	return NULL;
}
