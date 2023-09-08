/* Do-nothing debug hooks for GCC.
   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: debug.c 9fdf7f3d9c55 2007/11/30 23:34:04 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "debug.h"

/* The do-nothing debug hooks.  */
const struct gcc_debug_hooks do_nothing_debug_hooks =
{
  debug_nothing_charstar,
  debug_nothing_charstar,
  debug_nothing_int_charstar,
  debug_nothing_int_charstar,
  debug_nothing_int_charstar,
  debug_nothing_int,
  debug_nothing_int_int,	         /* begin_block */
  debug_nothing_int_int,	         /* end_block */
  debug_true_const_tree,	         /* ignore_block */
  debug_nothing_int_charstar,	         /* source_line */
  debug_nothing_int_charstar,	         /* begin_prologue */
  debug_nothing_int_charstar,	         /* end_prologue */
  debug_nothing_int_charstar,	         /* end_epilogue */
  debug_nothing_tree,		         /* begin_function */
  debug_nothing_int,		         /* end_function */
  debug_nothing_tree,		         /* function_decl */
  debug_nothing_tree,		         /* global_decl */
  debug_nothing_tree_int,		 /* type_decl */
  debug_nothing_tree_tree,               /* imported_module_or_decl */
  debug_nothing_tree,		         /* deferred_inline_function */
  debug_nothing_tree,		         /* outlining_inline_function */
  debug_nothing_rtx,		         /* label */
  debug_nothing_int,		         /* handle_pch */
  debug_nothing_rtx,		         /* var_location */
  debug_nothing_void,                    /* switch_text_section */
  0                                      /* start_end_main_source_file */
};

/* This file contains implementations of each debug hook that do
   nothing.  */

void
debug_nothing_void (void)
{
}

void
debug_nothing_tree (tree decl ATTRIBUTE_UNUSED)
{
}

void
debug_nothing_tree_tree (tree t1 ATTRIBUTE_UNUSED,
			 tree t2 ATTRIBUTE_UNUSED)
{
}

bool
debug_true_const_tree (const_tree block ATTRIBUTE_UNUSED)
{
  return true;
}

void
debug_nothing_rtx (rtx insn ATTRIBUTE_UNUSED)
{
}

void
debug_nothing_charstar (const char *main_filename ATTRIBUTE_UNUSED)
{
}

void
debug_nothing_int_charstar (unsigned int line ATTRIBUTE_UNUSED,
			    const char *text ATTRIBUTE_UNUSED)
{
}

void
debug_nothing_int (unsigned int line ATTRIBUTE_UNUSED)
{
}

void
debug_nothing_int_int (unsigned int line ATTRIBUTE_UNUSED,
		       unsigned int n ATTRIBUTE_UNUSED)
{
}

void
debug_nothing_tree_int (tree decl ATTRIBUTE_UNUSED,
			int local ATTRIBUTE_UNUSED)
{
}

/* These probably really belong in their own file, but for convenience put them here for now
    At this point it's unclear to me whether pdp10 adk tops20 debugging is it's own new debug infor
    type or whether it should be a variant of one of the 'standard' types
*/
#ifdef __PDP10_H__
#include "flags.h"
#include "tree.h"
#include "output.h"

extern FILE *asm_out_file;

static void tops20dbgout_init (const char *);
static void tops20dbgout_start_source_file (unsigned int, const char *);
static void tops20dbgout_source_line	(unsigned int, const char *);
static void tops20dbgout_end_prologue	(unsigned int, const char *);

const struct gcc_debug_hooks tops20_debug_hooks =
{
  tops20dbgout_init,				/* init */
  debug_nothing_charstar,		/* finish */
  debug_nothing_int_charstar,		/* define */
  debug_nothing_int_charstar,		/* undef */
  tops20dbgout_start_source_file,	/* start_source_file */
  debug_nothing_int,			/* end_source_file */
  debug_nothing_int_int,			/* begin_block */
  debug_nothing_int_int,			/* end_block */
  debug_true_const_tree,				/* ignore_block */
  tops20dbgout_source_line,		/* source_line */
  debug_nothing_int_charstar,		/* begin_prologue */
  tops20dbgout_end_prologue,	/* end_prologue */
  debug_nothing_int_charstar,		/* end_epilogue */
  debug_nothing_tree,			/* begin_function */
  debug_nothing_int,			/* end_function */
  debug_nothing_tree,			/* function_decl */
  debug_nothing_tree,			/* global_decl */
  debug_nothing_tree_int,		/* type_decl */
  debug_nothing_tree_tree,             /* imported_module_or_decl */
  debug_nothing_tree,			/* deferred_inline_function */
  debug_nothing_tree,			/* outlining_inline_function */
  debug_nothing_rtx,			/* label */
  debug_nothing_int,		        /* handle_pch */
  debug_nothing_rtx,		        /* var_location */
  debug_nothing_void,                     /* switch_text_section */
  0                                      	        /* start_end_main_source_file */
};


/* Output information marking the beginning of a new compilation unit
*/

void
tops20dbgout_init (const char *main_filename)
{
  switch (debug_info_level)
  	{
  	case DINFO_LEVEL_NONE:
		break;
		
	case DINFO_LEVEL_TERSE:
	case DINFO_LEVEL_NORMAL:
	case DINFO_LEVEL_VERBOSE:
		switch_to_section(text_section);
		fprintf(asm_out_file, "\%\%\%\%module:\n");
		fprintf(asm_out_file, "\tASCIZ\\%s\\\n", main_filename);
		break;
  	}
}

/* Output information marking the beginning of a new source file
*/

static void
tops20dbgout_start_source_file (unsigned int line ATTRIBUTE_UNUSED,  const char *filename)
{
  switch (debug_info_level)
  	{
  	case DINFO_LEVEL_NONE:
		break;
		
	case DINFO_LEVEL_TERSE:
		fprintf(asm_out_file, ";source file %s\n", filename);
		break;
		
	case DINFO_LEVEL_NORMAL:
		switch_to_section(text_section);
		fprintf(asm_out_file, ";source file %s\n", filename);
		fprintf(asm_out_file, "\tCAI\t1,[ASCIZ\\%s\\]\n", filename);
		break;
		
	case DINFO_LEVEL_VERBOSE:
		fprintf(asm_out_file, ";source file %s\n", filename);
		break;
  	}
}

/* Output a label or special code to mark the beginning of a source code line entry
 */

static void
tops20dbgout_source_line (unsigned line, const char *filename)
{
  const char *cp;
  const char *procname = IDENTIFIER_POINTER(DECL_NAME(current_function_decl));
  switch (debug_info_level)
  	{
  	case DINFO_LEVEL_NONE:
		break;
		
	case DINFO_LEVEL_TERSE:
		fprintf(asm_out_file, ";line %d in %s\n", line, filename);
		fprintf(asm_out_file, "\tLINENO\t%o\n",line);
		break;
		
	case DINFO_LEVEL_NORMAL:
		switch_to_section(text_section);
		fprintf(asm_out_file, ";line %d in %s\n", line, filename);
		fprintf(asm_out_file, "\tCAI\t%o\n", line);
		fprintf(asm_out_file, "\tLINENO\t%o\n",line);
		break;

	case DINFO_LEVEL_VERBOSE:
		switch_to_section(text_section);
		fprintf(asm_out_file,".LINE%dFILE", line);
		for (cp=filename; *cp; cp++)
			if (ISUPPER(*cp) || ISLOWER(*cp) || ISDIGIT(*cp) || *cp=='.' || *cp=='%' || *cp=='$')
				fputc(*cp, asm_out_file);
		fprintf(asm_out_file,"FUNC");
		for (cp=procname; *cp; cp++)
			if (ISUPPER(*cp) || ISLOWER(*cp) || ISDIGIT(*cp) || *cp=='.' || *cp=='%' || *cp=='$')
				fputc(*cp, asm_out_file);
		fprintf(asm_out_file, ":\n");
		fprintf(asm_out_file, "\tLINENO\t%o\n",line);
		break;

  	}
}

/* Output a marker (i.e. a label) for the beginning of a function, before
   the prologue.  */

static void
tops20dbgout_end_prologue (unsigned int line ATTRIBUTE_UNUSED, const char *filename ATTRIBUTE_UNUSED)
{
  const char *procname = IDENTIFIER_POINTER(DECL_NAME(current_function_decl));
  switch (debug_info_level)
  	{
  	case DINFO_LEVEL_NONE:
		break;
		
	case DINFO_LEVEL_TERSE:
		fprintf(asm_out_file, ";function begin %s\n", procname);
		break;
		
	case DINFO_LEVEL_NORMAL:
		switch_to_section(text_section);
		fprintf(asm_out_file, ";function begin %s\n", procname);
		fprintf(asm_out_file, "\tCAI\t2,[ASCIZ\\%s\\]\n", procname);
		break;
		
	case DINFO_LEVEL_VERBOSE:
		fprintf(asm_out_file, ";function begin %s\n", procname);
		break;
  	}
}


#endif
