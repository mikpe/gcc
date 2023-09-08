/* Definitions for PDP-10 target machine using MACRO assembler.
   Copyright (C) 2001, 2002 Free Software Foundation, Inc.
   Contributed by Lars Brinkhoff (lars@nocrew.org), funded by XKL, LLC.
   
This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef __PDP10_MACRO_H__
#define __PDP10_MACRO_H__

/**********************************************************************

	Defining the Output Assembler Language

**********************************************************************/

/*
 * Output of Assembler Instructions
 */

/* A C initializer containing the assembler's names for the machine
   registers, eash one as a C string constant. MJD */
#ifndef REGISTER_NAMES
#define REGISTER_NAMES				   \
{						   \
   "FL", "T1", "T2", "T3", "T4", "Q1", "Q2", "Q3", \
   "P1", "P2", "P3", "P4", "SC", "FP", "CX", "SP", \
  "<fp>", "<ap>"				   \
}
#endif

/* A C statement to output to the stdio stream STREAM any text
   necessary for declaring the name NAME of an initialized variable
   which is being defined.  This macro must output the label
   definition (perhaps using ASM_OUTPUT_LABEL).  */
#define ASM_DECLARE_OBJECT_NAME(STREAM, NAME, DECL) \
  pdp10_asm_declare_object_name ((STREAM), (NAME), (DECL))

/* A C statement to output to the stdio stream STREAM any text
   necessary for declaring the name of an external symbol named NAME
   which is referenced in this compilation but not defined.  */
#define ASM_OUTPUT_EXTERNAL(STREAM, DECL, NAME)		\
do {							\
  if (strncmp (NAME, "__builtin_", 10) == 0)		\
    break;						\
  pdp10_extern_symbol (NAME, PDP10_SYMBOL_EXTERN);	\
} while (0)

/* A C statement to output on STREAM an assembler pseudo-op to declare
   a library function name external.  */
#define ASM_OUTPUT_EXTERNAL_LIBCALL(STREAM, SYMREF) \
  pdp10_extern_symbol (XSTR (SYMREF, 0), PDP10_SYMBOL_EXTERN);

/* A C statement to output to the stdio stream STREAM a reference in
   assembler syntax to a label named NAME.  */
#define ASM_OUTPUT_LABELREF(STREAM, NAME) \
  pdp10_asm_output_labelref ((STREAM), (NAME))

/* A C statement to output a reference to SYMBOL_REF SYM.  If not
   defined, assemble_output will be used to output the name of the
   symbol.  This macro may be used to modify the way a symbol is
   referenced depending on information encoded by
   TARGET_ENCODE_SECTION_INFO.  */
#define ASM_OUTPUT_SYMBOL_REF(STREAM, SYM) \
  pdp10_asm_output_symbol_ref ((STREAM), (SYM))

/* A C statement to output to the stdio stream STREAM a label whose
   name is made from the string PREFIX and the numer NUM.

   To avoid polluting ddt with all these labels, they are declared
   with a trailing "!"  */

#ifndef ASM_OUTPUT_INTERNAL_LABEL
#define ASM_OUTPUT_INTERNAL_LABEL(STREAM, NAME) \
  do { \
    const char *name = (NAME); \
    pdp10_output_byte((STREAM),0,0); \
    if (name[0] == '*') \
    		{ \
		fputs(&name[1], (STREAM)); \
		fputs(":!\n", (STREAM)); \
    		} \
    else \
		asm_fprintf((STREAM), "%L%s:!\n",(NAME)); \
  } while (0)
#endif

/* Similar, but for an external labels.  Note no trailing "!"  */

#ifndef ASM_OUTPUT_LABEL
#define ASM_OUTPUT_LABEL(STREAM, NAME) \
	do {								\
		pdp10_output_byte((STREAM),0,0); \
		assemble_name ((STREAM), (NAME)); \
		fputs (":\n", (STREAM));				\
	} while (0)
#endif

/* A C statement to store into the string STRING a label whose name is
   made from the string PREFIX and the numer NUM.  */
#ifndef ASM_GENERATE_INTERNAL_LABEL
#define ASM_GENERATE_INTERNAL_LABEL(STRING, PREFIX, NUM) \
  sprintf ((STRING), "*%s%s%u", LOCAL_LABEL_PREFIX, (PREFIX), (unsigned)(NUM))
#endif

extern const char    *pdp10_macro_mangle_name PARAMS ((const char *name));

#ifndef ASM_FORMAT_PRIVATE_NAME
#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO)                  \
  do {                                                                  \
    const char *_name = pdp10_macro_mangle_name((NAME));                      \
    char *_temp = (char *) alloca (strlen (_name) + 3);                 \
    sprintf (_temp, "L%s.", _name);                                     \
    (OUTPUT) = (char *) alloca (strlen (_temp) + 11);                   \
    ASM_GENERATE_INTERNAL_LABEL ((OUTPUT), _temp, (LABELNO));           \
  } while (0)
#endif

/* A C statement to output to the stdio stream STREAM assembler code
   which defines the symbol NAME to have the value VALUE.

   PDP-10 assemblers define this to use "=" or "==". */
#define ASM_OUTPUT_DEF(FILE, LABEL, VALUE) \
  macro_output_def ((FILE), (LABEL), (VALUE))

/* A C string constant describing how to begin a comment in the target
   assembler language.  The compiler assumes that the comment will end
   at the end of the line. */
#define ASM_COMMENT_START ";"

/* A C string constant for test to be output before each asm statement
   or group of consecutive ones.

   PDP-10 assemblers should define this to the empty string. */
#define ASM_APP_ON ""

/* A C string constant for test to be output after each asm statement
   or group of consecutive ones.

   PDP-10 assemblers should define this to the empty string. */
#define ASM_APP_OFF ""

/* Define this macro as a C expression which is nonzero if the
   character C is used as a logical line separator by the
   assembler.  */
#define IS_ASM_LOGICAL_LINE_SEPARATOR(C, STR) 0

/**********************************************************************

	Dividing the Output into Sections

**********************************************************************/

#define TARGET_HAS_NAMED_SECTIONS
#define TARGET_ASM_NAMED_SECTION pdp10_asm_named_section

#if 0 /* these are redundant */
/* A C expression whose value is a string containing the assembler
   operation that should precede instructions and read-only data.  */
#ifndef TEXT_SECTION_ASM_OP
extern char *pdp10_text_section_asm_op;
#define TEXT_SECTION_ASM_OP  	pdp10_text_section_asm_op
#endif

/* A C expression whose value is a string containing the assembler
   operation to identify the following data as writable initialized
   data.  */
#ifndef DATA_SECTION_ASM_OP
extern char *pdp10_data_section_asm_op;
#define DATA_SECTION_ASM_OP  	pdp10_data_section_asm_op
#endif

/* A C expression whose value is a string containing the assembler
   operation to identify the following data as readonly initialized
   data.  */
#ifndef READONLY_DATA_SECTION_ASM_OP
extern char *pdp10_rodata_section_asm_op;
#define READONLY_DATA_SECTION_ASM_OP  	pdp10_rodata_section_asm_op
#endif

/* A C expression whose value is a string containing the assembler
   operation to identify the following data as uninitialized data.  */
#ifndef BSS_SECTION_ASM_OP
extern char *pdp10_bss_section_asm_op;
#define BSS_SECTION_ASM_OP   	pdp10_bss_section_asm_op
#endif
#endif

/* This define controls initialization code generated in main.  Defining it should eliminate
     unnecessary code.
     -mtc 2/6/2009
*/
#define HAS_INIT_SECTION 1

/* We ifdef such that these value are not actually used, but its still critical to
    define them because their existence controls gcc code that we otherwise would
    have to ifdef even more
    -mtc 3/6/2007
*/
#define TEXT_SECTION_ASM_OP			"\n\t.PSECT\ttext"
#define DATA_SECTION_ASM_OP			"\n\t.PSECT\tdata"
#define READONLY_DATA_SECTION_ASM_OP	"\n\t.PSECT\trdat"
#define BSS_SECTION_ASM_OP				"\n\t.PSECT\tbss"

#define TARGET_ASM_INIT_SECTIONS pdp10_init_sections


/* Define this macro if references to a symbol must be treated
   differently depending on something about the variable or function
   named by the symbol.  */
#define TARGET_ENCODE_SECTION_INFO pdp10_encode_section_info


/*
 * Output of Data
 */

/* C statements for outputting to the stdio stream STREAM an assembler
   instruction to assemble a floating-point constant.  */

#ifndef ASM_OUTPUT_DOUBLE
#define ASM_OUTPUT_DOUBLE(STREAM, VALUE)		\
do {							\
  unsigned HOST_WIDE_INT high, low;			\
  long l[3];						\
  REAL_VALUE_TO_TARGET_DOUBLE ((VALUE), l);		\
  high = (((((HOST_WIDE_INT)l[0]) & 0xffffffffUL) << 4)	\
	  + (HOST_WIDE_INT)((l[1] >> 28) & 0xfUL));	\
  low = ((((HOST_WIDE_INT)l[1] << 8) & 0xfffffffUL)	\
	 + (HOST_WIDE_INT)((l[2] >> 24) & 0xffUL));	\
  fprintf ((STREAM), "\tEXP %llo,%llo\n", high, low);	\
} while (0)
#endif

#ifndef ASM_OUTPUT_FLOAT
#define ASM_OUTPUT_FLOAT(STREAM, VALUE)		\
do {						\
  char str[31];					\
  real_to_decimal (str, &(VALUE), 30, 9, 1);    \
  fprintf ((STREAM), "\t%s\n", str);		\
} while (0);
#endif

/* C statements for outputting to the stdio stream STREAM an assembler
   instruction to assemble an integer constant.  */

/*  #define ASM_OUTPUT_DOUBLE_INT(STREAM, EXP)	\ */
/*  do {						\ */
/*    pdp10_output_byte ((STREAM), 0, 0);		\ */
/*    fputc ('\t', STREAM);				\ */
/*    pdp10_print_operand (STREAM, EXP, 'D');	\ */
/*    fputc ('\n', STREAM);				\ */
/*  } while (0) */

/*  #define ASM_OUTPUT_INT(STREAM, EXP)		\ */
/*    pdp10_output_byte (STREAM, 0, 0);		\ */
/*    fputc ('\t', STREAM);				\ */
/*    pdp10_print_operand (STREAM, EXP, 0);		\ */
/*    fputc ('\n', STREAM); */

/*  #define ASM_OUTPUT_SHORT(STREAM, EXP) \ */
/*    pdp10_output_byte (STREAM, EXP, 18) */

/*  #define ASM_OUTPUT_CHAR(STREAM, EXP) \ */
/*    pdp10_output_byte (STREAM, EXP, 9) */

/* A C statement to output to the stdio stream STREAM an assembler
   instruction to assemble a single byte containing the number
   VALUE.  */

/*  #define ASM_OUTPUT_BYTE(STREAM, VALUE) \ */
/*    pdp10_output_byte (STREAM, GEN_INT (VALUE), 9) */

/*
 * Output of Dispatch Tables
 */

/* TODO: the following use of GIW is suspect, since when TARGET_EXTENDED is not
 * true, GIW won't get defined
 * also should remove pdp10_giw, since all it controls is generation of the ONE
 * reference, which it makes more sense to generate solely on TARGET_EXTENDED
 * But there's the additional issue of whether TARGET_EXTENDED should simply be
 * removed and assumed
 */
 
/* use of BSS(n) has been changed to BLOCK n
    -mtc 8/26/2008
*/

#define ASM_OUTPUT_ADDR_VEC_ELT(STREAM, VALUE)				 \
 do {									 \
   fprintf ((STREAM), "\t%s%sL%d)\n",					 \
	    TARGET_EXTENDED ? "GAW(" : "GIW(1,0,", LOCAL_LABEL_PREFIX, (VALUE));\
   if (TARGET_EXTENDED) pdp10_giw++;  				         \
 } while (0)

/* Completely reworded this macro and made it a procedure call
    -mtc 5/5/2009
*/
#define ASM_OUTPUT_SKIP(STREAM, NBYTES)		pdp10_output_skip ((STREAM), (NBYTES))

/* A C statement to output to the stdio stream STREAM an assembler
   instruction to assemble a string constant.  */
#define ASM_OUTPUT_ASCII(STREAM, PTR, LEN) \
  macro_output_ascii (STREAM, PTR, LEN)

#define TARGET_ASM_GLOBALIZE_LABEL macro_globalize_label

enum
{
  PDP10_SYMBOL_EXTERN = 0x01,
  PDP10_SYMBOL_USED = 0x02,
  PDP10_SYMBOL_DEFINED = 0x04
};

/*
 * Output of Uninitialized Variables
 */

/*  The following four macros are partially a reversion to revision 2760 and partially a
      defactoring of the different macros.  ASM_OUTPUT_COMMON is the original macro
      which is probably no longer invoked anywhere.  ASM_OUTPUT_ALIGNED_BSS
      would probably be better done by calling asm_output_aligned_bss and modifying that
      routine.  Quite likely the different macros require different minor differences from each
      other.  The change I've made at this time is to not make symbols output with
      ASM_OUTPUT_ALIGNED_DECL_LOCAL global by using INTERN on them.  The
      other macros probably should determine whether INTERN is needed by checking
      TREE_PUBLIC(DECL) on them or something else.
      -mtc 11/15/2006
*/

/* A C statement to output to the stdio stream STREAM the assembler
   definition of a common-label named NAME whose size is SIZE bytes.
   The variable ROUNDED is the size rounded up to whatever alignment
   the caller wants.  */
#ifndef ASM_OUTPUT_COMMON
#define ASM_OUTPUT_COMMON(STREAM, NAME, SIZE, ROUNDED)	\
do {							\
  switch_to_section(bss_section);					\
  pdp10_output_byte (STREAM, 0, 0);			\
  fputs ("\n\tINTERN\t", STREAM);	       		\
  assemble_name (STREAM, (NAME));			\
  fputc ('\n', STREAM);					\
  assemble_name (STREAM, (NAME));			\
  fputs (":\n\tBLOCK\t", STREAM);  			\
  fprintf (STREAM, "%o\n", ((ROUNDED) + 3) / 4);	\
  pdp10_extern_symbol ((NAME), PDP10_SYMBOL_DEFINED);	\
} while (0)
#endif

#define MAX_OFILE_ALIGNMENT 36*512  /* maximum objectfile alignment is one page */

/* A C statement to output to the stdio stream STREAM the assembler
   definition of uninitialized global DECL named NAME whose size is
   SIZE bytes.  The variable ROUNDED is the size rounded up to
   whatever alignment the caller wants.  */

#ifndef ASM_OUTPUT_ALIGNED_BSS
#define ASM_OUTPUT_ALIGNED_BSS(STREAM, DECL, NAME, SIZE, ALIGN)      \
do {							        \
  int power = floor_log2 ((ALIGN) / BITS_PER_UNIT);             \
  int rounded = (((SIZE) * (BITS_PER_UNIT)) + (ALIGN) - 1) / (ALIGN) * (ALIGN) / (BITS_PER_UNIT);     \
  switch_to_section(bss_section);					\
  pdp10_output_byte (STREAM, 0, 0);			        \
  ASM_OUTPUT_ALIGN((STREAM), power);                            \
  fputs ("\n\tINTERN\t", STREAM);	        		\
  assemble_name (STREAM, (NAME));		           	\
  fputc ('\n', STREAM);					        \
  assemble_name (STREAM, (NAME));			        \
  fputs (":\n\tBLOCK\t", STREAM);  			        \
  fprintf (STREAM, "%o\n", (rounded + UNITS_PER_WORD - 1) / UNITS_PER_WORD);	\
  pdp10_extern_symbol ((NAME), PDP10_SYMBOL_DEFINED);	        \
} while (0)
#endif
     
/* A C statement to output to the stdio stream STREAM the assembler
   definition of a local-common-label named NAME whose size is SIZE
   bytes.  The variable ROUNDED is the size rounded up to whatever
   alignment the caller wants.  */
#ifndef ASM_OUTPUT_ALIGNED_DECL_LOCAL
#define ASM_OUTPUT_ALIGNED_DECL_LOCAL(STREAM, DECL, NAME, SIZE, ALIGN)	\
do {							        \
  int power = floor_log2 ((ALIGN) / BITS_PER_UNIT);             \
  int rounded = (((SIZE) * (BITS_PER_UNIT)) + (ALIGN) - 1) / (ALIGN) * (ALIGN) / (BITS_PER_UNIT);     \
  switch_to_section(bss_section);					\
  pdp10_output_byte (STREAM, 0, 0);			        \
  ASM_OUTPUT_ALIGN((STREAM), power);                            \
  assemble_name (STREAM, (NAME));			        \
  fputs (":\n\tBLOCK\t", STREAM);  			        \
  fprintf (STREAM, "%o\n", (rounded + UNITS_PER_WORD - 1) / UNITS_PER_WORD);	\
  pdp10_extern_symbol ((NAME), PDP10_SYMBOL_DEFINED);	        \
} while (0)
#endif

/* A C statement to output to the stdio stream STREAM the assembler
   definition of a common-label named NAME whose size is SIZE
   bytes.  The variable ROUNDED is the size rounded up to whatever
   alignment the caller wants.  */
#ifndef ASM_OUTPUT_ALIGNED_DECL_COMMON
#define ASM_OUTPUT_ALIGNED_DECL_COMMON(STREAM, DECL, NAME, SIZE, ALIGN)	\
do {							        \
  int power = floor_log2 ((ALIGN) / BITS_PER_UNIT);             \
  int rounded = (((SIZE) * (BITS_PER_UNIT)) + (ALIGN) - 1) / (ALIGN) * (ALIGN) / (BITS_PER_UNIT);     \
  switch_to_section(bss_section);					\
  pdp10_output_byte (STREAM, 0, 0);			        \
  ASM_OUTPUT_ALIGN((STREAM), power);                            \
  fputs ("\n\tINTERN\t", STREAM);	        		\
  assemble_name (STREAM, (NAME));		           	\
  fputc ('\n', STREAM);					        \
  assemble_name (STREAM, (NAME));			        \
  fputs (":\n\tBLOCK\t", STREAM);  			        \
  fprintf (STREAM, "%o\n", (rounded + UNITS_PER_WORD - 1) / UNITS_PER_WORD);	\
  pdp10_extern_symbol ((NAME), PDP10_SYMBOL_DEFINED);	        \
} while (0)
#endif

/* A C statement to output to the stdio stream STREAM any text
   necessary for declaring the name NAME of a function which is being
   defined.  This macro is responsible for outputting the label
   definition (perhaps using ASM_OUTPUT_LABEL).  The argument DECL is
   the FUNCTION_DECL tree node representing the function.

   If this macro is not defined, then the function name is defined in
   the usual manner as a label (by means of ASM_OUTPUT_LABEL).  */
#ifndef ASM_DECLARE_FUNCTION_NAME
#define ASM_DECLARE_FUNCTION_NAME(STREAM, NAME, DECL)		\
do {								\
  if (! DECL_WEAK (DECL))					\
    SYMBOL_REF_INTERNAL (XEXP (DECL_RTL (DECL), 0)) = 1;	\
  ASM_OUTPUT_LABEL ((STREAM), (NAME));				\
  pdp10_extern_symbol ((NAME), PDP10_SYMBOL_DEFINED);		\
} while (0)
#endif

/* SYMBOL_REF_FLAG is used to indicate that the symbol is referring to
   a function known to be defined in the current translation unit.  */
#define SYMBOL_REF_INTERNAL SYMBOL_REF_FLAG

#define ASM_OUTPUT_WEAK_ALIAS(STREAM, NAME, VALUE) \
  ASM_OUTPUT_DEF (STREAM, NAME, VALUE)

#define pdp10_print_tab(STREAM) \
  fputc (TARGET_ASM_TAB ? '\t' : ' ', (STREAM));

/* Define this macro if you are using an unusual assembler that
   requires different names for the machine instructions.

   The definition is a C statement or statements which output an
   assembler instruction opcode to the stdio stream STREAM.  PTR is a
   variable of type char * which points to the opcode name.  The
   definition should output the opcode name to STREAM, performing any
   translation you require, and increment the variable PTR to point at
   the end of the opcode.

   On the PDP-10, this is used to optionally change lower-case
   characters to upper-case, and a space into a tab character.  */
#define ASM_OUTPUT_OPCODE(STREAM, PTR)					\
  if (TARGET_ASM_UPPER | TARGET_ASM_TAB | TARGET_ASM_INDENT_SKIPPED)	\
    {									\
      (PTR) = pdp10_print_insn ((STREAM), (PTR));			\
      if (*(PTR) == ' ')						\
	{								\
	  pdp10_print_tab (STREAM);					\
	  (PTR)++;							\
	}								\
    }

/* flags which control the function of PRINT_OPERAND and PRINT_OPERAND_ADDRESS */
/* order is important here, must be numerically sequential!! dont change without changing pdp10.c!! */

enum PRINT_OP_CLASS {
  PRINT_OP_IMMEDIATE = 0,
  PRINT_OP_LITERAL   = 1,
  PRINT_OP_OPERAND   = 2,
  PRINT_OP_CLASSMAX  = 3
};

enum PRINT_OP_TYPE {
  PRINT_OP_DIRECT   = 0,
  PRINT_OP_POINTER  = 1,
  PRINT_OP_INDIRECT = 2,
  PRINT_OP_TYPEMAX  = 3
};

/* A C compound statement to output to the stdio stream STREAM the
   assembler syntax for an instruction operand X.  */
#define PRINT_OPERAND(STREAM, X, CODE) \
  pdp10_print_operand ((STREAM), (X), (CODE), PRINT_OP_OPERAND, PRINT_OP_DIRECT)

/* A C expression which evaluates to true if CODE is a valid
   punctuation character for use in the PRINT_OPERAND macro.

   %_ is used to output a space if -mindent-skipped is enabled.
   %@ is used to output "0," if -mac0 is enabled.
   %; is used to start a comment.  */
#define PRINT_OPERAND_PUNCT_VALID_P(CODE) \
  ((CODE) == '_' || (CODE) == '@' || (CODE) == ';')

/* A C compound statement to output to the stdio stream STREAM the
   assembler syntax for an instruction operand that is a memory
   reference whose address is X.  */
#define PRINT_OPERAND_ADDRESS(STREAM,X) \
  pdp10_print_operand ((STREAM), (X), 'a', PRINT_OP_OPERAND, PRINT_OP_POINTER)

/* C string expressions to be used for the %R, %L, %U, and %I options
   of asm_printf.  */

/* WARNING: %R has been usurped by print_operand to mean reversed
   conditions.  Some other letter should have been used for this
   purpose!!!.  TODO: fix it!!. */

/* #define REGISTER_PREFIX "" */
#ifndef LOCAL_LABEL_PREFIX
#define LOCAL_LABEL_PREFIX "."
#endif

/* we EXPLICITLY decorate user defined symbols to prevent them from colliding
   with built-in assembler syntax.  We don't decorate compiler generated
   symbols, since we can ensure that they will never so collide.

   for example, picture what happens with the innocent declarations

   int end;    --> END :        (abuses the END directive) 
   int _psect; --> .PSECT :     (abuses the .PSECT directive)
   int t1;     --> T1 :         (abuses a register definition)

   and yes, this DID actually happen, and it DOES generate errors.  */
 
#ifndef USER_LABEL_PREFIX
#define USER_LABEL_PREFIX "%"
#endif

/* #define IMMEDIATE_PREFIX "" */

/* A C expression to output to STREAM some assembler code which will
   push hard register number REGNO onto the stack.  */
#define ASM_OUTPUT_REG_PUSH(STREAM, REGNO) \
  fprintf ((STREAM), "\tpush %s,%s\n", reg_names[STACK_POINTER_REGNUM], reg_names[(REGNO)])

/* A C expression to output to STREAM some assembler code which will
   pop hard register number REGNO off of the stack.  */
#define ASM_OUTPUT_REG_POP(STREAM, REGNO) \
  fprintf ((STREAM), "\tpop %s,%s\n", reg_names[STACK_POINTER_REGNUM], reg_names[(REGNO)])

/*
 * Assembler Commands for Alignment
 */

/* A C statement to output to the stdio stream STREAM an assembler
   instruction to advance the location counter by NBYTES bytes.  Those
   bytes should be zero when loaded.  */
#ifndef ASM_OUTPUT_SKIP
#define ASM_OUTPUT_SKIP(STREAM, NBYTES) \
   fprintf ((STREAM), "\tREPEAT %o,<EXP 0>\t; skip zeroed words\n", ((NBYTES) + 3) / 4 )
#endif

/* A C statement to output to the stdio stream STREAM an assembler
   command to advance the location counter to a multiple of 2 to the
   POWER bytes.  */
#ifndef ASM_OUTPUT_ALIGN
#define ASM_OUTPUT_ALIGN(STREAM, POWER) \
  if ((POWER)-2 > 0) fprintf ((STREAM),"\tALIGN(%o)\t; Align to 2**%d words\n", (POWER)-2, (POWER)-2)
#endif

/**********************************************************************

	Controlling Debugging Information Format

**********************************************************************/

/*
 * Macros Affecting All Debugging Formats
 */

#define TOPS20_DEBUGGING_INFO 1

/* DBX register number for a given compiler register number.
#define DBX_REGISTER_NUMBER(REGNO) (REGNO)
*/

/*
 * Specific Options for DBX Output
 */

/* Define this macro if GCC should produce debugging output for DBX in
   response to the -g option.
*/
#define DBX_DEBUGGING_INFO 1

/* Define this macro to control wether GCC should by default generate
   GDB's extended version of DBX debugging information.
#define DEFAULT_GDB_EXTENSIONS 1
*/

#ifdef XCOFF_DEBUGGING_INFO
# define ASM_STABS_OP ";.stabx\t"
#else
# define ASM_STABS_OP ";.stabs\t"
#endif
#define ASM_STABN_OP ";.stabn\t"
#define ASM_STABD_OP ";.stabd\t"

/*
 * Macros for SDB and DWARF Output
 */

/* Define this macro if GCC should produce dwarf version 2 debuggging
   output in response to the -g option.
#define DWARF2_DEBUGGING_INFO 1
*/

/* Define this macro to a nonzero value if GCC should always output
   Dwarf 2 frame information.
#define DWARF2_FRAME_INFO 1
*/

#define DEBUG_INFO_SECTION	".dbinf"
#define DEBUG_ABBREV_SECTION	".dbabb"
#define DEBUG_ARANGES_SECTION	".dbarn"
#define DEBUG_MACINFO_SECTION	".dbmac"
#define DEBUG_LINE_SECTION	".dblin"
#define DEBUG_LOC_SECTION	".dbloc"
#define DEBUG_PUBNAMES_SECTION	".dbpub"
#define DEBUG_STR_SECTION	".dbstr"
#define DEBUG_RANGES_SECTION	".dbrng"
#define DEBUG_FRAME_SECTION	".dbfrm"

#define TEXT_SECTION_LABEL		".Ltxt"
#define DEBUG_LINE_SECTION_LABEL	".Ldbl"
#define DEBUG_INFO_SECTION_LABEL	".Ldbi"
#define DEBUG_ABBREV_SECTION_LABEL	".Ldba"
#define DEBUG_LOC_SECTION_LABEL		".Ldbl"
#define DEBUG_RANGES_SECTION_LABEL	".Ldbr"
#define DEBUG_MACINFO_SECTION_LABEL     ".Ldbm"
#define TEXT_END_LABEL			".Letx"
#define DATA_END_LABEL			".Ledt"
#define BSS_END_LABEL			".Lebs"
#define BLOCK_BEGIN_LABEL		".BB"
#define BLOCK_END_LABEL			".BE"

#undef  PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE DBX_DEBUG

#endif /* __PDP10_MACRO_H__ */
