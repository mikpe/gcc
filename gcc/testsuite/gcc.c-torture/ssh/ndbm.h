/* ndbm.h  -  The include file for ndbm users.  */

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

/* Parameters to dbm_store for simple insertion or replacement. */
#define  DBM_INSERT  0
#define  DBM_REPLACE 1


/* The data and key structure.  This structure is defined for compatibility. */
typedef struct {
	char *dptr;
	int   dsize;
      } datum;


/* The file information header. This is good enough for most applications. */
typedef struct {int dummy[10];} DBM;


/* These are the routines (with some macros defining them!) */

#define		 dbm_error(dbf)  (0)

#define		 dbm_clearerr(dbf)

/* Extracts from remap.h, must match that and gdbm.h */
#define dbm_close dbm_17
#define dbm_delete dbm_18
#define dbm_dirfno dbm_19
#define dbm_fetch dbm_20
#define dbm_firstkey dbm_21
#define dbm_nextkey dbm_22
#define dbm_open dbm_23
#define dbm_pagfno dbm_24
#define dbm_rdonly dbm_25
#define dbm_store dbm_26

/* Function prototypes */
#if defined(__STDC__) || defined(__cplusplus)
# define P_(s) s
#else
# define P_(s) ()
#endif

/* DBMCLOSE.C */
extern void dbm_close P_((DBM *dbf));

/* DBMDELETE.C */
extern int dbm_delete P_((DBM *dbf,datum key));

/* DBMDIRFNO.C */
extern int dbm_dirfno P_((DBM *dbf));

/* DBMFETCH.C */
extern datum dbm_fetch P_((DBM *dbf,datum key));

/* DBMOPEN.C */
extern DBM *dbm_open P_((char *file,int flags,int mode));

/* DBMPAGFNO.C */
extern int dbm_pagfno P_((DBM *dbf));

/* DBMRDONLY.C */
extern int dbm_rdonly P_((DBM *dbf));

/* DBMSEQ.C */
extern datum dbm_firstkey P_((DBM *dbf));
extern datum dbm_nextkey P_((DBM *dbf));

/* DBMSTORE.C */
extern int dbm_store P_((DBM *dbf,datum key,datum content,int flags));

#undef P_
