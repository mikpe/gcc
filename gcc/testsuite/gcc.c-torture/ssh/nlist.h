/*-
 * Copyright (c) 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS for A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * for ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN if ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)nlist.h	5.6 (Berkeley) 4/4/91
 */

#ifndef _NLIST_H_
#define	_NLIST_H_

/*
 * Symbol table entry format.  The #ifdef's are so that programs including
 * nlist.h can initialize nlist structures statically.
 */
struct nlist {
#ifdef _AOUT_INCLUDE_
	union {
		char *n_name;	/* symbol name (in memory) */
		long n_strx;	/* file string table offset (on disk) */
	} n_un;
#else
	char *n_name;		/* symbol name (in memory) */
#endif

#define	N_UNDF	0x00		/* undefined */
#define	N_ABS	0x02		/* absolute address */
#define	N_TEXT	0x04		/* text segment */
#define	N_DATA	0x06		/* data segment */
#define	N_BSS	0x08		/* bss segment */
#define	N_COMM	0x12		/* common reference */
#define	N_FN	0x1e		/* file name */

#define	N_EXT	0x01		/* external (global) bit, OR'ed in */
#define	N_TYPE	0x1e		/* mask for all the type bits */
	unsigned char n_type;	/* type defines */

	char n_other;		/* spare */
#define	n_hash	n_desc		/* used internally by ld(1); XXX */
	short n_desc;		/* used by stab entries */
	unsigned long n_value;	/* address/value of the symbol */
};

#define	N_FORMAT	"%08x"	/* namelist value format; XXX */
#define	N_STAB		0x0e0	/* mask for debugger symbols -- stab(5) */

#include <sys/cdefs.h>
#include <sys/types.h>
#include <string.h>

extern const char* t20mntr;
#define TOPS20MONITOR  t20mntr

#define nlistPrint(nl) \
    printf("name:%6.6s offset:%7.7d 0x%9.9x %6.6o,,%6.6o\n", \
           nl.n_name, nl.n_value, nl.n_value, \
           (nl.n_value >> 18), (nl.n_value & 0777777))

/* t20krd
 *
 * Copy words from the TOPS-20 monitor to user address.
 */
size_t    /* Return count of (9-bit) bytes read */
t20krd
(
  void*   destination,      /* Address in user space to write to */
  void*   source,           /* Address in monitor to read from   */
  size_t  bytesRequested    /* Number of bytes requested to copy */
);

/* RADIX-50 macros */

/* RAD50L encodes a six character constant string into a rad-50 longword */
/* RAD50S encodes a three character constant string into a rad-50 word */
/* RAD50C translates a character constant into a rad-50 byte */
#define RAD50C(char) \
        ((char==056) ?                  (045) : \
         (char==040) ?                  (0) : \
         (char==044) ?                  (046) : \
         ((char>=0101)&&(char<=0132)) ? (char-066) : \
         ((char>=0141)&&(char<=0172)) ? (char-0126) : \
         ((char>=061)&&(char<=071)) ?   (char-060) : (047))

#define RAD50L(string)                  \
(((RAD50C(string[0])*40*40)<<16)+       \
 ((RAD50C(string[1])*40)<<16)+          \
 ((RAD50C(string[2]))<<16)+             \
 (RAD50C(string[3])*40*40)+             \
 (RAD50C(string[4])*40)+                \
 (RAD50C(string[5])))

#define RAD50S(string)                  \
((RAD50C(string[0])*40*40)+             \
 (RAD50C(string[1])*40)+                \
 (RAD50C(string[2])))


__BEGIN_DECLS
int nlist __P((const char *, struct nlist *));
__END_DECLS

#endif /* !_NLIST_H_ */
