/* gdbm.h  -  The include file for dbm users.  */

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

/* Protection for multiple includes. */
#ifndef _GDBM_H_
#define _GDBM_H_

/* Remap potentially ambiguous symbol names */
#define _gdbm_alloc dbm_1
#define _gdbm_end_update dbm_2
#define _gdbm_fatal dbm_3
#define _gdbm_fetch_val dbm_4
#define _gdbm_file dbm_5
#define _gdbm_findkey dbm_6
#define _gdbm_free dbm_7
#define _gdbm_get_bucket dbm_8
#define _gdbm_hash dbm_9
#define _gdbm_init_cache dbm_10
#define _gdbm_memory dbm_11
#define _gdbm_new_bucket dbm_12
#define _gdbm_put_av_elem dbm_13
#define _gdbm_read_entry dbm_14
#define _gdbm_split_bucket dbm_15
#define _gdbm_write_bucket dbm_16
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
#define dbmclose dbm_27
#define dbminit dbm_28
#define firstkey dbm_29
#define gdbm_close dbm_30
#define gdbm_delete dbm_31
#define gdbm_errlist dbm_32
#define gdbm_errno dbm_33
#define gdbm_exists dbm_34
#define gdbm_fetch dbm_35
#define gdbm_firstkey dbm_36
#define gdbm_nextkey dbm_37
#define gdbm_open dbm_38
#define gdbm_reorganize dbm_39
#define gdbm_setopt dbm_40
#define gdbm_store dbm_41
#define gdbm_strerror dbm_42
#define gdbm_sync dbm_43
#define gdbm_version dbm_44
#define nextkey dbm_45

/* Parameters to gdbm_open for READERS, WRITERS, and WRITERS who
   can create the database. */
#define  GDBM_READER  0		/* A reader. */
#define  GDBM_WRITER  1		/* A writer. */
#define  GDBM_WRCREAT 2		/* A writer.  Create the db if needed. */
#define  GDBM_NEWDB   3		/* A writer.  Always create a new db. */
#define  GDBM_FAST    16	/* Write fast! => No fsyncs. */

/* Parameters to gdbm_store for simple insertion or replacement in the
   case that the key is already in the database. */
#define  GDBM_INSERT  0		/* Never replace old data with new. */
#define  GDBM_REPLACE 1		/* Always replace old data with new. */

/* Parameters to gdbm_setopt, specifing the type of operation to perform. */
#define  GDBM_CACHESIZE 1       /* Set the cache size. */
#define  GDBM_FASTMODE  2       /* Toggle fast mode. */

/* The data and key structure.  This structure is defined for compatibility. */
typedef struct {
	char *dptr;
	int   dsize;
      } datum;


/* The file information header. This is good enough for most applications. */
typedef struct {int dummy[10];} *GDBM_FILE;

/* Determine if the C(++) compiler requires complete function prototype  */
#if  __STDC__ || defined(__cplusplus) || defined(c_plusplus)
#define GDBM_Proto(x) x
#else
#define GDBM_Proto(x) ()
#endif /* NeedFunctionPrototypes */

/* External variable, the gdbm build release string. */
extern char *gdbm_version;	


/* GDBM C++ support */
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* These are the routines! */

extern GDBM_FILE gdbm_open GDBM_Proto((
     char *file,
     int  block_size,
     int  flags,
     int  mode,
     void (*fatal_func)(char *)
));

extern void gdbm_close GDBM_Proto((
     GDBM_FILE dbf
));

extern int gdbm_store GDBM_Proto((
     GDBM_FILE dbf,
     datum key,
     datum content,
     int flags
));

extern datum gdbm_fetch GDBM_Proto((
     GDBM_FILE dbf,
     datum key
));

extern int gdbm_delete GDBM_Proto((
     GDBM_FILE dbf,
     datum key
));

extern datum gdbm_firstkey GDBM_Proto((
     GDBM_FILE dbf
));

extern datum gdbm_nextkey GDBM_Proto((
     GDBM_FILE dbf,
     datum key
));

extern int gdbm_reorganize GDBM_Proto((
     GDBM_FILE dbf
));

extern void gdbm_sync GDBM_Proto((
     GDBM_FILE dbf
));

extern int gdbm_exists GDBM_Proto((
     GDBM_FILE dbf,
     datum key
));

extern int gdbm_setopt GDBM_Proto((
     GDBM_FILE dbf,
     int optflag,
     int *optval,
     int optlen
));

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/* gdbm sends back the following error codes in the variable gdbm_errno. */
typedef enum {	GDBM_NO_ERROR,
		GDBM_MALLOC_ERROR,
		GDBM_BLOCK_SIZE_ERROR,
		GDBM_FILE_OPEN_ERROR,
		GDBM_FILE_WRITE_ERROR,
		GDBM_FILE_SEEK_ERROR,
		GDBM_FILE_READ_ERROR,
		GDBM_BAD_MAGIC_NUMBER,
		GDBM_EMPTY_DATABASE,
		GDBM_CANT_BE_READER,
	        GDBM_CANT_BE_WRITER,
		GDBM_READER_CANT_DELETE,
		GDBM_READER_CANT_STORE,
		GDBM_READER_CANT_REORGANIZE,
		GDBM_UNKNOWN_UPDATE,
		GDBM_ITEM_NOT_FOUND,
		GDBM_REORGANIZE_FAILED,
		GDBM_CANNOT_REPLACE,
		GDBM_ILLEGAL_DATA,
		GDBM_OPT_ALREADY_SET,
		GDBM_OPT_ILLEGAL}
	gdbm_error;
extern gdbm_error gdbm_errno;

/* extra prototypes */

/* GDBM C++ support */
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern const char *gdbm_strerror GDBM_Proto((
     gdbm_error error
));

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
