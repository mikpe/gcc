/* dbm.h  -  The include file for dbm users.  */

/*  This file is part of GDBM, the GNU data base manager, by Philip A. Nelson.
    Copyright (C) 1990, 1991, 1993  Free Software Foundation, Inc.

    GDBM is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.

    GDBM is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS for A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GDBM; see the file COPYING.  If not, write to
    the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

    You may contact the author by:
       e-mail:  phil@cs.wwu.edu
      us-mail:  Philip A. Nelson
                Computer Science Department
                Western Washington University
                Bellingham, WA 98226
       
*************************************************************************/

/* The data and key structure.  This structure is defined for compatibility. */
typedef struct {
	char *dptr;
	int   dsize;
      } datum;

/* Extracts from remap.h, must match that and gdbm.h */
#define dbminit dbm_28
#define dbmclose dbm_27
#define firstkey dbm_29
#define nextkey dbm_45

/* Function prototypes */
#if defined(__STDC__) || defined(__cplusplus)
# define P_(s) s
#else
# define P_(s) ()
#endif

/* dbminit.c */
extern int dbminit P_((char *file));

/* close.c */
extern int dbmclose P_((void));

/* fetch.c */
extern datum fetch P_((datum key));

/* store.c */
extern int store P_((datum key,datum content));

/* delete.c */
extern int delete P_((datum key));

/* seq.c */
extern datum firstkey P_((void));
extern datum nextkey P_((datum key));

#undef P_
