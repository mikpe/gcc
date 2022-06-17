/* Definitions of target machine for GNU compiler for the Infineon xc16x cpu.
   Copyright (C)  2006
   Free Software Foundation, Inc.

   Contributed by Shrirang Khisti On behalf of KPIT Cummins Infosystems Ltd.

   This file is part of GCC.

   GGC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA */


/*  Variable target_flags is used to distingusih between different target options specified
   on the command line.  */
#ifndef GCC_XC16X_H
#define GCC_XC16X_H

#define TARGET_VERSION fprintf (stderr, " (Infineon xc16x)");

/* #define STARTFILE_SPEC "crt0.o%S" */
/* Different target related switches supported are
    -mtiny
    -msmall
    -mlarge
    -mintrom
    -mno-dppsave
*/

#define TARGET_DEFAULT (MASK_TINY)	/* Default target is tiny  */

/*/---------------------------------------------------------------------------------------------------*/
/* Target machine storage layout  */
/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields.  */
#define BITS_BIG_ENDIAN  0

/* Define this if most significant byte of a word is the lowest numbered.  */
#define BYTES_BIG_ENDIAN  0

/* Define this if most significant word of a multiword number is numbered.  */
#define WORDS_BIG_ENDIAN 0

/* Number of bits in an addressible storage unit  */
#define BITS_PER_UNIT 8

/* Width in bits of a "word", which is the contents of a machine register.  */
#define BITS_PER_WORD 16

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD 2

#define MIN_UNITS_PER_WORD 2

/* Allocation boundary (in *bits*) for storing pointers in memory.  */
#define POINTER_BOUNDARY (TARGET_LARGE ? 32 : 16)

/* Boundary (in *bits*) on which stack pointer should be aligned.  */
#define STACK_BOUNDARY 16

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */
#define PARM_BOUNDARY 16

/* Allocation boundary (in *bits*) for the code of a function.  */
#define FUNCTION_BOUNDARY 16

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 16

/* Every structure's size must be a multiple of this.  */
#define STRUCTURE_SIZE_BOUNDARY 16

/* A bitfield declared as `int' forces `int' alignment for the struct.  */
#define PCC_BITFIELD_TYPE_MATTERS 1

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 16

/* Define this if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT  1

#define MAX_FIXED_MODE_SIZE	32

/* ------------------------------------------------------------------------------------------------------ */
/* Standard register usage.  */
/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers.  */
#define FIRST_PSEUDO_REGISTER 16

#define FIXED_REGISTERS \
 /* r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15*/  \
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0}

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you like.  */

#define CALL_USED_REGISTERS \
 /* r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15 */  \
     {1,1,0,0,1,1,1,1,1,1,1,1,1,0,0,0}

#define REG_ALLOC_ORDER \
     {2,3,4, 5, 6, 7, 8, 9, 10, 11, 12,13,14, 15, 0, 1 }

/* ------------------------------------------------------------------------------------------------------ */
/* This section is all about how values fit in the registers
  Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.
   This is ordinarily the length in words of a value of mode MODE
   but can be less for certain modes in special long registers.  */

#define HARD_REGNO_NREGS(REGNO, MODE)   \
   ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Here 1 implies that it is permissible to store value of mode mode in
   in hard reg or starting from that  hard reg no  */

#define HARD_REGNO_MODE_OK(REGNO, MODE) \
  (((MODE)== QImode && REGNO < 8) ||((MODE) > QImode ))

/* Currently  this macro is returning 0 but it can be further modified  */
#define MODES_TIEABLE_P(MODE1, MODE2) 0


/* ------------------------------------------------------------------------------------------------------- */
/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM 0

/* Base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM 1

/* Value should be nonzero if functions must have frame pointers.
   Zero means the frame pointer need not be set up (and parms
   may be accessed via the stack pointer) in functions that seem suitable.
   This is computed in `reload', in reload1.c.  */
#define FRAME_POINTER_REQUIRED 0

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM FRAME_POINTER_REGNUM

/* Register in which static-chain is passed to a function.*/
#define STATIC_CHAIN_REGNUM 12

/* ---------------------------------------------------------------------------------------------------- */
/* Define the classes of registers for register constraints in the
   machine description.  Also define ranges of constants.

   One of the classes must always be named ALL_REGS and include all hard regs.
   If there is more than one class, another class must be named NO_REGS
   and contain no registers.

   The name GENERAL_REGS must be the name of a class (or an alias for
   another name such as ALL_REGS).  This is the class of registers
   that is allowed by "g" or "r" in a register constraint.
   Also, registers outside this class are allocated only when
   instructions express preferences for them.

   The classes must be numbered in nondecreasing order; that is,
   a larger-numbered class must never be contained completely
   in a smaller-numbered class.

   For any two classes, it is very desirable that there be another
   class that represents their union.  */

enum reg_class
{ NO_REGS, BYTE_REGS, GENERAL_REGS, ALL_REGS, LIM_REG_CLASSES };

#define N_REG_CLASSES (int) LIM_REG_CLASSES

/* Give names of register classes as strings for dump file.   */

#define REG_CLASS_NAMES \
 {"NO_REGS", "BYTE_REGS","GENERAL_REGS", "ALL_REGS" };

#define REG_CLASS_CONTENTS \
 {                                          \
  0x00000000,     /* NO_REGS    */          \
  0x000000ff,     /* BYTE_REGS  */          \
  0x0000ffff,     /* GENERAL_REGS */        \
  0x0000ffff      /* ALL_REGS   */          \
 }


 /* The same information, inverted:
    Return the class number of the smallest class containing
    reg number REGNO.  This could be a conditional expression
    or could index an array.  */

#define REGNO_REG_CLASS(REGNO) \
  ((REGNO) < 8 ? BYTE_REGS : GENERAL_REGS)

/* The class value for index registers, and the one for base regs.  */
#define INDEX_REG_CLASS  GENERAL_REGS

#define BASE_REG_CLASS   GENERAL_REGS

/* Get reg_class from a letter such as appears in the machine description.  */
#define REG_CLASS_FROM_LETTER(C) \
 (((C) == 'b' || (C) == 'B' ) ? BYTE_REGS :NO_REGS)

/* When defined, the compiler allows registers explicitly used in the
   rtl to be used as spill registers but prevents the compiler from
   extending the lifetime of these registers.  */
#define SMALL_REGISTER_CLASSES 1

/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.  */
#define CLASS_MAX_NREGS(CLASS, MODE)	\
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C)	\
    ((C) == 'G' ? (VALUE) == CONST0_RTX (SFmode)	\
     : 0)
/* Macros to check register numbers against specific register classes.  */

#define REGNO_OK_FOR_INDEX_P(regno) 0

/*Expression to indicate that register is used for base or not  */
#define REGNO_OK_FOR_BASE_P(regno) \
((regno) < FIRST_PSEUDO_REGISTER \
    || (reg_renumber[regno] < FIRST_PSEUDO_REGISTER  && reg_renumber[regno] >= 0))

/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
   In general this is just CLASS; but on some machines
   in some cases it is preferable to use a more restrictive class. */

#define PREFERRED_RELOAD_CLASS(X,CLASS)  \
  (GET_MODE (X) == QImode ? \
     BYTE_REGS : CLASS )

#define PREFERRED_OUTPUT_RELOAD_CLASS PREFERRED_RELOAD_CLASS
/* ----------------------------------------------------------------------------------------------- */
/* Stack layout; function entry, exit and calling.  */

/*   Define this if pushing a word on the stack
     makes the stack pointer a smaller address.  */
#define STACK_GROWS_DOWNWARD

/* Define this if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */

#define FRAME_GROWS_DOWNWARD 1

#define PUSH_ROUNDING(BYTES) (((BYTES) +1) & ~1)

#define RETURN_POPS_ARGS(FUNDECL, FUNTYPE, SIZE) 0

/* Offset of first parameter from the argument pointer register value.  */
/* Is equal to the size of the saved fp + pc, even if an fp isn't
   saved since the value is used before we know.  */

#define FIRST_PARM_OFFSET(FNDECL) \
 (flag_omit_frame_pointer ? 0:2)

#define STARTING_FRAME_OFFSET 0
/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.  */

/* On the xc16x the return value is in R4 regardless.  */
#define RET_VAL_REG    4

/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.

   On the xc16x the return value is in R4/R5.  */

#define FUNCTION_VALUE(VALTYPE, FUNC) \
  gen_rtx_REG (TYPE_MODE (VALTYPE), RET_VAL_REG)

/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */

/* On the xc16x the return value is in R4/R5.  */

#define LIBCALL_VALUE(MODE) \
  gen_rtx_REG (MODE, RET_VAL_REG)

/* 1 if N is a possible register number for a function value.
     On the C166, R4 is the only register thus used.  */
#define FUNCTION_VALUE_REGNO_P(N) ((N) == RET_VAL_REG)

#define NREGS_FOR_REG_PARM   5	/* Total no of registers for parameter passing  */
#define FIRST_PARM_REG       8	/* First register used for parameter passing  */

/* #define FUNCTION_ARG(CUM,MODE,TYPE,NAMED) \
 (!NAMED ? 0 :	\
	 ((CUM) < (NREGS_FOR_REG_PARM*UNITS_PER_WORD-1)	\
	       &&  ((CUM)+ ((MODE) != BLKmode ?			\
		      GET_MODE_SIZE(MODE):int_size_in_bytes (TYPE))	\
		   <= (NREGS_FOR_REG_PARM*UNITS_PER_WORD))		\
               ?  gen_rtx_REG((MODE), FIRST_PARM_REG+(CUM)/UNITS_PER_WORD) : 0)) */

#define FUNCTION_ARG(CUM,MODE,TYPE,NAMED)  function_arg (CUM, MODE, TYPE, NAMED)
#define FUNCTION_ARG_REGNO_P(N) ((N) >= FIRST_PARM_REG \
                           && (N) <= FIRST_PARM_REG+NREGS_FOR_REG_PARM)

/*---------------------------------------------------------------------------------------
 Following macros are regarding cumulative arguments and its initialization */

#define CUMULATIVE_ARGS int

/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.
   .  */
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME, INDIRECT, N_NAMED_ARGS)	\
 ((CUM) = 0)

/* Update the data in CUM to advance over an argument
   of mode MODE and data type TYPE.
   (TYPE is null for libcalls where that information may not be available.)
   Here if odd bytes are passed as function argument then it is rounded to next higher
   even number and added to value of cum  */

#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)	\
 ((CUM) += ((MODE) != BLKmode				\
	    ? (GET_MODE_SIZE (MODE) + 1) & ~1		\
	    : (int_size_in_bytes (TYPE) + 1) & ~1))

   /* Output assembler code to FILE to increment profiler label # LABELNO
      for profiling a function entry.  */
#define FUNCTION_PROFILER(FILE, LABELNO)

/* --------------------------------------------------------------------------------------- */
/* This part of file have macros related to trampoline  */
 /*
    Ttrampoline code will be

    0xfce6 : opcode for mov r12,#xxxx  Here r12 is static chain register number
    0x00ea : opcode for jmpa cc_UC,fun jump to the function
    This implementation is yet not perfect for Large Memory model
  */

#define TRAMPOLINE_SIZE  8
#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT)			\
do									    \
    {									    \
	                                           \
	 rtx temp=gen_rtx_CONST_INT(HImode,-794);								    \
	  emit_move_insn (gen_rtx_MEM (HImode, (TRAMP)), temp); \
	  emit_move_insn (gen_rtx_MEM (Pmode, plus_constant ((TRAMP), 2)),  \
			  (CXT));					    \
	  emit_move_insn (gen_rtx_MEM (Pmode, plus_constant ((TRAMP), 4)),  \
			  GEN_INT (0x00ea));				    \
	  emit_move_insn (gen_rtx_MEM (Pmode, plus_constant ((TRAMP), 6)),  \
			  (FNADDR));					    \
									    \
											\
    }									    \
  while (0)
/* ---------------------------------------------------------------------------------------- */
/* Definitions for register eliminations.

   This is an array of structures.  Each structure initializes one pair
   of eliminable registers.  The "from" register number is given first,
   followed by "to".  Eliminations of the same "from" register are listed
   in order of preference.

   We have three registers that can be eliminated on the xc16x.
   First, the frame pointer register can often be eliminated in favor
   of the stack pointer register.  Secondly, the argument pointer
   register  */

#if 0
#define ELIMINABLE_REGS				\
{{ ARG_POINTER_REGNUM, STACK_POINTER_REGNUM},	\
 { ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM},   \
 { FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM}}

/* Given FROM and TO register numbers, say whether this elimination is allowed.
   Frame pointer elimination is automatically handled.
   We would like to
   convert ap into sp, not fp.
   All other eliminations are valid.  */
#define CAN_ELIMINATE(FROM, TO)					\
 ( (TO) == STACK_POINTER_REGNUM	\
  ? ! frame_pointer_needed					\
  : 1)

/* Here diff is total frame size + total no of reg saved*2  */
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
{									\
						\
      int regno;							\
      int offset = 0;							\
      for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)		\
	if ((regs_ever_live[regno] && ! call_used_regs[regno]))		\
	  offset += 2;							\
									\
      (OFFSET) = offset + get_frame_size () ;			\
									\
    }

#endif
#define INITIAL_FRAME_POINTER_OFFSET(DEPTH)			\
{ int regno;							\
  int offset = 0  ;     \
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)	\
    if (regs_ever_live[regno] && ! call_used_regs[regno])	\
      offset += 2;						\
  (DEPTH) = (offset + get_frame_size () ) ;			\
}

/* ------------------------------------------------------------------------------------------------- */
/* Following part contains macros related to addressing modes and address checking */

/* Addressing modes,  */

#define HAVE_POST_INCREMENT 1
#define HAVE_PRE_DECREMENT 1
#define HAVE_PRE_MODIFY_REG  0
#define HAVE_PRE_MODIFY_DISP  0

/* Maximum number of registers that can appear in a valid memory address.  */
#define MAX_REGS_PER_ADDRESS 1

/* 1 if X is an rtx for a constant that is a valid address.  */
/*  for the C166 allow only offsettables with positiv offset  */
#define CONSTANT_ADDRESS_P(X)   \
 ( GET_CODE (X) == LABEL_REF || GET_CODE (X) == SYMBOL_REF		\
   || GET_CODE (X) == CONST_DOUBLE					\
   || GET_CODE (X) == CONST_INT						\
   || (GET_CODE(X) == CONST &&  GET_CODE (XEXP ((X),0)) == PLUS 	\
          && GET_CODE (XEXP (XEXP ((X),0), 0)) == SYMBOL_REF 		\
          && GET_CODE (XEXP (XEXP ((X),0), 1)) == CONST_INT		\
          && (INTVAL (XEXP (XEXP ((X),0),1)) >= 0 			\
             ||  INTVAL (XEXP (XEXP ((X),0),1)) <= 32768) ))

/* Following section checks validity of register X as an index register and base register  */
#ifndef REG_OK_STRICT

/* Nonzero if X is a hard reg that can be used as an index
   or if it is a pseudo reg.  */
#define REG_OK_FOR_INDEX_P(X) 1

/* Nonzero if X is a hard reg that can be used as a base reg
   or if it is a pseudo reg.   */
#define REG_OK_FOR_BASE_P(X)   1
#else

/* Nonzero if X is a hard reg that can be used as an index.  */
#define REG_OK_FOR_INDEX_P(X) REGNO_OK_FOR_INDEX_P(REGNO(X))

/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define REG_OK_FOR_BASE_P(X)  REGNO_OK_FOR_BASE_P (REGNO (X))
#endif

/*
   It isalways safe for this macro to do nothing.  It exists to recognize
   opportunities to optimize the output.  */

#define LEGITIMIZE_ADDRESS(X,OLDX,MODE,WIN) {}

/* Go to LABEL if ADDR (a legitimate address expression)
   has an effect that depends on the machine mode it is used for.
   On the VAX, the predecrement and postincrement address depend thus
   (the amount of decrement or increment being the length of the operand)  */
#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR,LABEL)	\
 { if (GET_CODE (ADDR) == PRE_DEC) 	\
     goto LABEL; }

#define RTX_OK_FOR_BASE_P(X)				\
   ((REG_P (X) && REG_OK_FOR_BASE_P (X))			\
    || (GET_CODE (X) == SUBREG && REG_P (SUBREG_REG (X))	\
        && REG_OK_FOR_BASE_P (SUBREG_REG (X))))

#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, ADDR)		\
   if (RTX_OK_FOR_BASE_P (X)) goto ADDR;			\
   if (CONSTANT_ADDRESS_P (X)) goto ADDR;		\
   if (GET_CODE (X) == PLUS				\
       && GET_CODE (XEXP (X, 1))==CONST_INT		\
      && RTX_OK_FOR_BASE_P (XEXP (X, 0))) goto ADDR;

 /* ------------------------------------------------------------------------------------------- */
 /* Specify the machine mode that this machine uses
    for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE Pmode

/* #define CASE_VECTOR_PC_RELATIVE 1  */
  /* Specify the tree operation to be used to convert reals to integers.
     #define IMPLICIT_FIX_EXPR FIX_ROUND_EXPR  */

 /* This is the kind of divide that is easiest to do in the general case.
    #define EASY_DIV_EXPR TRUNC_DIV_EXPR  */

 /* Define this as 1 if `char' should by default be signed; else as 0.  */
#define DEFAULT_SIGNED_CHAR 0

 /* Max number of bytes we can move from memory to memory
    in one reasonably fast instruction.  */
#define MOVE_MAX 2

/* Following section describes macros related to  c data type size supported on xc16x  */

#define SHORT_TYPE_SIZE  16
#define INT_TYPE_SIZE    16
#define LONG_TYPE_SIZE   32
#define FLOAT_TYPE_SIZE  32
#define DOUBLE_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64
#define MOVE_RATIO  (TARGET_LARGE ? 12 : 6)

#define SIZE_TYPE (TARGET_LARGE ? "long unsigned int" : "unsigned int")

#define Pmode   \
  (TARGET_LARGE ? PSImode : HImode)

#define POINTER_SIZE							\
  (TARGET_LARGE ? 32 : 16)

#define PTRDIFF_TYPE						\
  (TARGET_LARGE ? "long int" : "int")

/* This flag, if defined, says the same insns that convert to a signed fixnum
   also convert validly to an unsigned one.  */
#define FIXUNS_TRUNC_LIKE_FIX_TRUNC

/* Nonzero if access to memory by bytes is slow and undesirable.  */
#define SLOW_BYTE_ACCESS 0

/* A function address in a call instruction
   is a word address (for indexing purposes)
   so give the MEM rtx a word mode.  */

#define FUNCTION_MODE HImode

/* Check a `double' value for validity for a particular machine mode.  */
#define CHECK_FLOAT_VALUE(mode, d, overflow) \
  if ((mode) == SFmode) \
    { \
      if ((d) > 1.701411733192643e+38) \
	{ error ("magnitude of constant too large for `float'"); \
	  (d) = 1.701411733192643e+38; } \
      else if ((d) < -1.701411733192643e+38) \
	{ error ("magnitude of constant too large for `float'"); \
	  (d) = -1.701411733192643e+38; } \
      else if (((d) > 0) && ((d) < 2.9387358770557188e-39)) \
	{ warning ("`float' constant truncated to zero"); \
	  (d) = 0.0; } \
      else if (((d) < 0) && ((d) > -2.9387358770557188e-39)) \
	{ warning ("`float' constant truncated to zero"); \
	  (d) = 0.0; } \
    }

/* ------------------------------------------------------------------------------------------------- */
/* Following part of the file contains macros related to assembler output formatting. */

/* Control the assembler format that we output.  */

/* Output to assembler file text saying following lines
   may contain character constants, extra white space, comments, etc.  */

#define ASM_APP_ON "#APP\n"

/* Output to assembler file text saying following lines
   no longer contain unusual constructs.  */
#define ASM_APP_OFF "#NO_APP\n"

#define FILE_ASM_OP "\t.file\n"

/* Write the sections names  */
#define TEXT_SECTION_ASM_OP "\t.section .text"
#define DATA_SECTION_ASM_OP "\t.section .data"
#define BSS_SECTION_ASM_OP "\t.section .bss"
#define READONLY_DATA_SECTION_ASM_OP "\n\t.section .rodata\n"
/* #define READONLY_DATA_SECTION() READONLY_DATA_SECTION_ASM_OP  */

/* How to put comment in assembly code  */
#define ASM_COMMENT_START  ";"

#define ASM_GENERATE_INTERNAL_LABEL(LABEL,PREFIX,NUM)	\
  sprintf (LABEL, "*.%s%d", PREFIX, (unsigned)(NUM))

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE) \
  fprintf (FILE, "%s.L%d\n", ASM_WORD_OP, VALUE)

/* This is how to output an element of a case-vector that is relative.  */

/*#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL) \
  fprintf (FILE, "%s.L%d-.L%d\n", ASM_WORD_OP, VALUE, REL)*/

/* This is how to output an insn to push a register on the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_PUSH(FILE,REGNO)  \
  fprintf (FILE, "\tmov [-r%d],%s\n", STACK_POINTER_REGNUM, reg_names[REGNO])

/* This is how to output an insn to pop a register from the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_POP(FILE,REGNO)  \
  fprintf (FILE, "\tmov %s,[r%d+]\n", reg_names[REGNO], STACK_POINTER_REGNUM)

/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2*LOG bytes.  */

#define ASM_OUTPUT_ALIGN(FILE, LOG)		\
  if ((LOG) != 0)				\
    fprintf (FILE, "\t.align %d\n", (LOG))

#define ASM_OUTPUT_SKIP(FILE, SIZE) \
  fprintf (FILE, "\t.space %d\n", (int)(SIZE))

/* This says how to output an assembler line
   to define a global common symbol.  */

#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)	\
( fputs ("\t.comm ", (FILE)),				\
  assemble_name ((FILE), (NAME)),			\
  fprintf ((FILE), ",%lu\n", (unsigned long)(SIZE)))

/* This says how to output a n assembler line
    to define a local symbol  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED)	\
( fputs ("\t.lcomm ", (FILE)),				\
  assemble_name ((FILE), (NAME)),			\
  fprintf ((FILE), ",%d\n", (int)(SIZE)))

/* Following macro will call function "print_operand" in xc16x.c file which takes
   total responsibility of printing particular operand  */

#define PRINT_OPERAND(FILE, X, CODE) print_operand (FILE, X, CODE)

/* Print a memory operand whose address is X, on file FILE.
   This uses a function in xc16x.c.  */

#define PRINT_OPERAND_ADDRESS(FILE, ADDR) print_operand_address (FILE, ADDR)

/* The prefix to add to user-visible assembler symbols.  */

#define USER_LABEL_PREFIX "_"

/* Globalizing directive for a label.  */
#define GLOBAL_ASM_OP "\t.global "

/* The assembler op to get a word, 2 bytes for the H8/300, 4 for H8/300H.  */
#define ASM_WORD_OP "\t.word\t"

/* This macro is to generate function name  */

#define ASM_DECLARE_FUNCTION_NAME(FILE, NAME, DECL) \
   ASM_OUTPUT_LABEL (FILE, NAME)

#define REGISTER_NAMES \
 {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \
 "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" }

#define REGISTER_TARGET_PRAGMAS() \
 do                \
  {					\
	  c_register_pragma (0, "disable_intr", xc16x_disable_intr);	\
      c_register_pragma (0, "interrupt_handler", xc16x_interrupt_handler);	\
      c_register_pragma (0, "interrupt_noreturn", xc16x_interrupt_noreturn);	\
  } \
   while(0)

#define NOTICE_UPDATE_CC(EXP, INSN) {CC_STATUS_INIT;}

#define CC_STATUS_INIT

    /* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
       is done just by pretending it is already truncated.  */

#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 0

#define TARGET_CPU_CPP_BUILTINS()			\
  do							\
    { \
    if(TARGET_LARGE)\
    {\
    builtin_define ("__xc16xL__");			\
		  builtin_assert ("cpu=xc16x");			\
		  builtin_assert ("machine=xc16x");		\
    }\
    else if(TARGET_SMALL) \
    { \
	builtin_define ("__xc16xS__");			\
			  builtin_assert ("cpu=xc16x");			\
			  builtin_assert ("machine=xc16x");		\
    }\
    else\
    {\
	builtin_define ("__xc16x__");			\
			  builtin_assert ("cpu=xc16x");			\
			  builtin_assert ("machine=xc16x");		\
	}\
	}						\
	while(0)

/* Nonzero if the constant value X is a legitimate general operand.
   It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE.  */
#define LEGITIMATE_CONSTANT_P(X) (1)
#define CONST_OK_FOR_LETTER_P(value,c) 0

#define ASM_OUTPUT_LABELREF(FILE,NAME)	\
  fprintf (FILE, "_%s", NAME)

#define  POINTERS_EXTEND_UNSIGNED 1

#define LINK_SPEC "%{mlarge: -m elf32xc16xl} %{msmall: -m elf32xc16xs}"
#define EXTRA_CONSTRAINT(OP, C)			\
  (   (C) == 'S' ? OK_FOR_S (OP) :			\
      (C) == 'R' ? OK_FOR_R (OP) :	\
      (C) == 'T' ? OK_FOR_R (OP) :	\
        0)

#define OK_FOR_S(op) \
 (GET_CODE (op) == MEM && ((GET_CODE (XEXP (op, 0))==SYMBOL_REF)||(GET_CODE(XEXP(op,0)))==CONST	)		\
    ? 1 \
    : 0)

#define OK_FOR_T(op) \
  (GET_CODE(op)==MEM && (GET_CODE (XEXP (op, 0))==PLUS))\
     ? 0 \
     : 1)

#define OK_FOR_R(op) \
  (GET_CODE (op)==MEM &&(GET_CODE(XEXP(op,0))==REG||GET_CODE(XEXP(op,0))==SYMBOL_REF) \
  ? 1 \
  :0)

#define OBJECT_FORMAT_ELF 1
#define DBX_DEBUGGING_INFO

/*  For DWARF2 debugging information */
#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG
#define TARGET_ASM_NAMED_SECTION  default_elf_asm_named_section
#define DWARF2_DEBUGGING_INFO 1
#define DWARF2_ASM_LINE_DEBUG_INFO 1

#endif
