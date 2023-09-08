/* Definitions for PDP-10 target machine.
   Copyright (C) 2001, 2002 Free Software Foundation, Inc.
   Contributed by Lars Brinkhoff (lars@nocrew.org), funded by XKL, LLC.
   
For information about the PDP-10 archtecture, see the README file in
this directory.

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

#ifndef __PDP10_H__
#define __PDP10_H__

/* XKL-specific stuff.  */
#define XKL_STUFF 1


/**********************************************************************

	Index

**********************************************************************/

/*	Front Page
	Index
	To-do List
	Controlling the Compilation Driver, gcc
	Run-time Target Specification
	Storage Layout
	Layout of Source Language Data Types
	Register Usage
		Basic Characteristics of Registers
		Order of Allocation of Registers
		How Values Fit in Registers
	Register Classes
	Stack Layout and Calling Conventions
		Basic Stack Layout
		Registers That Address the Stack Frame
		Eliminating Frame Pointer and Arg Pointer
		Passing Function Arguments on the Stack
		Passing Arguments in Registers
		How Scalar Function Values Are Returned
		How Large Values Are Returned
		Function Entry and Exit
		Generating Code for Profiling
		Permitting Tail Calls
	Implementing the Varargs Macros
	Implicit Calls To Library Routines
	Addressing Modes
	Describing Relative Costs of Operations
	Dividing the Output into Sections
	Position Independent Code
	Miscellaneous Parameters
	External variable declarations  */


/**********************************************************************

	To-do List

**********************************************************************/

/* PROMOTE_FUNCTION_RETURN, FUNCTION_VALUE: promote QImode and HImode
   to SImode?

   FUNCTION_PROFILER
   MACHINE_STATE_SAVE
   MACHINE_STATE_RESTORE
   LEGITIMIZE_ADDRESS
   LEGITIMIZE_RELOAD_ADDRESS
   BRANCH_COST
   ADJUST_COST
   
   CALLER_SAVE_PROFITABLE: tune the cost of allocating call-clobbered
   registers for pseudos.  */


/**********************************************************************

	Controlling the Compilation Driver, gcc

**********************************************************************/

/* If defined, a list of pairs of strings, the first of which is a
   potential command line target to the gcc driver program, and the
   second of which is a space-separated list of options with which to
   replace the first option.  */

/* A C string constant that tells the GCC driver program options to
   pass to CPP.  It can also specify how to translate options you give
   to GCC into options for GCC to pass to the CPP.  */
#define CPP_SPEC "%(cpp_cpu_arch)"

#define CPP_CPU_ARCH_SPEC "\
%{march=xkl1:-D__PDP10_XKL1__} \
%{march=xkl2:-D__PDP10_XKL2__} \
%{!march=*: \
 %{mcpu=xkl1:-D__PDP10_XKL1__} \
 %{mcpu=xkl2:-D__PDP10_XKL2__} \
 %{!mcpu*:%(cpp_cpu_arch_default)}} \
%{mregparm=0:-D__REGPARM__=0} \
%{mregparm=1:-D__REGPARM__=1} \
%{mregparm=2:-D__REGPARM__=2} \
%{mregparm=3:-D__REGPARM__=3} \
%{mregparm=4:-D__REGPARM__=4} \
%{mregparm=5:-D__REGPARM__=5} \
%{mregparm=6:-D__REGPARM__=6} \
%{mregparm=7:-D__REGPARM__=7} \
%{!mregparm*:-D__REGPARM__=7} \
%{mdual-stack:-D__DUALSTACK__=1} \
%{mlong-long-71bit:-D__LONG_LONG_71BIT__=1} \
"

/* FIXME: this should depend on TARGET_CPU_DEFAULT.  */
#define CPP_ARCH_DEFAULT_SPEC "-D__PDP10_XKL1__"

/* This macro defines names of additional specifications to put in the
   specs that can be used in various specifications like CC1_SPEC.
   Its definition is an initializer with a subgrouping for each
   command option.

   Each subgrouping contains a string constant, that defines the
   specification name, and a string constant that used by the GNU CC
   driver program.  */
#define EXTRA_SPECS						\
  { "cpp_cpu_arch",		CPP_CPU_ARCH_SPEC },		\
  { "cpp_cpu_arch_default",	CPP_ARCH_DEFAULT_SPEC },

/* Define this macro as a C expression for the initializer of an array
   of strings to tell the driver program which options are defaults
   for this target and thus do not need to be handled specially when
   using MULTILIB_OPTIONS.  */
#define MULTILIB_DEFAULTS { "mregparm=7" }

/* The default value contains %G twice which causes libgcc.a to be included twice */
#define LINK_GCC_C_SEQUENCE_SPEC "%L %G"

/* Default values for target_flags
*/
#ifndef TARGET_DEFAULT
#define TARGET_DEFAULT (MASK_71BIT)
#endif


/**********************************************************************

	Run-time Target Specification

**********************************************************************/

/* Define this string to be a string constant containing '-D' options
   to define the pre-defined macros that identify this machine and
   system.  */
   /*
#ifndef CPP_PREDEFINES
#if XKL_STUFF
#define CPP_PREDEFINES  "-Dpdp10 -DPDP10 -Dtops20 -DTOPS20 -DGNUCC -DGCC -Acpu=pdp10 -Amachine=pdp10"
#else
#define CPP_PREDEFINES  "-Dpdp10 -Dtops20 -Acpu=pdp10 -Amachine=pdp10"
#endif
#endif
*/

#undef TARGET_CPU_CPP_BUILTINS
#define TARGET_CPU_CPP_BUILTINS()     \
  do                                 \
    {                                \
      builtin_define ("_pdp10");     \
      builtin_define ("__pdp10");     \
      builtin_define ("__pdp10__");     \
      builtin_define ("_PDP10");     \
      builtin_define ("__PDP10");     \
      builtin_define ("__PDP10__");     \
      builtin_assert ("cpu=pdp10");     \
      builtin_assert ("machine=pdp10");     \
    }                                \
  while (0)

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()     \
  do                                 \
    {                                \
      builtin_define ("_tops20");     \
      builtin_define ("__tops20");     \
      builtin_define ("__tops20__");     \
      builtin_define ("_TOPS20");     \
      builtin_define ("__TOPS20");     \
      builtin_define ("__TOPS20__");     \
    }                                \
  while (0)

/* Run-time compilation parameters selecting different hardware
   subsets.  */
extern int target_flags;

/* Run-time compilation parameter selecting different instruction set
   tunings.  */
extern int target_tune;

/* Run-time compilation parameter selecting the code model to use.  */
extern int target_model;

/* This series of macros is to allow compiler command arguments to
   enable or disable the use of optional features of the target
   machine.  */

#define CPU_XKL1	1	/* XKL XKL-1.  */
#define CPU_XKL2	2	/* XKL XKL-2.  */

#define MODEL_NONE	0	/* No code model selected.  */
#define MODEL_SMALL	1	/* Everything in one section...  */
#define MODEL_MEDIUM	2	/* Mostly single section...  */
#define MODEL_LARGE	3	/* Everything in multiple sections...  */
#define MODEL_HUGE	4	/* Multi-section functions...  */

#define TARGET_XKL1	(target_tune == CPU_XKL1)
#define TARGET_XKL2	(target_tune == CPU_XKL2)

#define TARGET_CACHE	TARGET_XKLup


/* target models -

   small  == everything goes into a single section.
   medium == the stack is moved into a separate section
   large  == data and rodata are moved into separate sections
   huge   == code (but NOT a single function) may span multiple sections

   NOTE: huge is unimplemented at the present time

   This has effects on both code generation and linking.
   Since code compiled for a given model is 'downward compatible'
   but not 'upward compatible', it makes sense to compile
   and link by default as 'large', at least until we know better
   whether or not the program will fit into a smaller model.

   If the program will fit into a smaller footprint, then only a relink 
   is necessary to accomplish this, although a bit more squeezing
   can be gained if it is recompiled too.

   If code is linked with a model larger than it's original compilation,
   parameters, the linker is smart enough to complain when any address 
   truncation occurs.  */

#define TARGET_SMALL	(target_model == MODEL_SMALL)
#define TARGET_MEDIUM	(target_model == MODEL_MEDIUM)
#define TARGET_LARGE	(target_model == MODEL_LARGE)
#define TARGET_HUGE	(target_model == MODEL_HUGE)

#define TARGET_XKLup	(TARGET_XKL1 || TARGET_XKL2)

#define TUNE_XKL1	(target_tune == CPU_XKL1)
#define TUNE_XKL2	(target_tune == CPU_XKL2)

/* Just in case configure has failed to define anything.  */
#ifndef TARGET_CPU_DEFAULT
#define TARGET_CPU_DEFAULT 0
#endif

#ifndef TARGET_DEFAULT
#define TARGET_DEFAULT  0
#endif

extern const char *pdp10_cpu;
extern const char *pdp10_arch;
extern const char *pdp10_tune;
extern const char *pdp10_text_psect;
extern const char *pdp10_rodata_psect;
extern const char *pdp10_data_psect;
extern const char *pdp10_bss_psect;
extern const char *pdp10_model;

/* This macro is a C statement to print on stderr a string describing
   the particular machine description choice.  */
#ifndef TARGET_VERSION
#define TARGET_VERSION fputs (" (PDP-10)", stderr);
#endif

/* Sometimes certain combinations of command options do not make sense
   on a particular target machine.  You can define a macro
   OVERRIDE_OPTIONS to take account of this.  This macro is executed
   once just after all the command options have been parsed.  */
#define OVERRIDE_OPTIONS pdp10_override_options ()

/* Define this macro if debugging can be performed even without a
   frame pointer.  If this macro is defined, GCC will turn on the
   -fomit-frame-pointer option whenever -O is specified.
#define CAN_DEBUG_WITHOUT_FP */


/**********************************************************************

	Storage Layout

**********************************************************************/

/* Define this macro to have the value 1 if the most significant bit
   in a byte has the lowest number; otherwise define it to be zero.  */
#define BITS_BIG_ENDIAN 1

/* Define this macro to to have the value 1 if most significant byte
   of a word has the lowest number.  */
#define BYTES_BIG_ENDIAN 1

/* Define this macro to have the value 1 if, in a multiword object,
   the most significant word has the lowest number.  This applies to
   both memory locations and registers; GCC fundamentally assumes that
   the order of words in memory is the same as the order in registers.  */
#define WORDS_BIG_ENDIAN  1

/* Define this macro to be the number of bits in an addressable
   storage unit.  */
#define BITS_PER_UNIT 9

/* Number of bits in a word.  */
#define BITS_PER_WORD 36

/* Constant folding doesn't work well enough to use an expression for this */
/* -mtc 4/10/2007 */
/* Number of storage units in a word.  */
#define UNITS_PER_WORD 4
/*#define UNITS_PER_WORD ((unsigned int) (BITS_PER_WORD / BITS_PER_UNIT))*/

/* The horrible truth.  */
#define REAL_BITS_PER_UNIT 36
#define REAL_UNITS_PER_WORD 1

/* Number of bits per memory unit used to access a bitfield.  */
#define BITS_PER_BITFIELD_UNIT 36

/* Width of a pointer, in bits.  You must specify a value no wider
   than the width of Pmode.  If it is not equal to the width of Pmode,
   you must define POINTERS_EXTEND_UNSIGNED.  */
#define POINTER_SIZE 36

/* A macro to update MODE and UNSIGNEDP when an object whose type is
   TYPE and which has the specified mode and signedness is to be
   stored in a register.  This macro is only called when TYPE is a
   scalar type.  */
#if BITS_PER_UNIT == 9
#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)	\
  if (GET_MODE_CLASS (MODE) == MODE_INT		\
      && GET_MODE_SIZE (MODE) < 4)      	\
    {						\
      UNSIGNEDP = 1;				\
      (MODE) = SImode;				\
    }
#else
#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)	\
  if (GET_MODE_CLASS (MODE) == MODE_INT		\
      && GET_MODE_SIZE (MODE) < 1)      	\
    {						\
      UNSIGNEDP = 1;				\
      (MODE) = QImode;				\
    }
#endif

/* Normal alignment required for function parameters on the stack, in
   bits.  */
#define PARM_BOUNDARY 36

/* Normal alignment required for stack variables, in bits.  */
#define STACKVAR_BOUNDARY 36

/* Define this macro if there is a guaranteed alignment for the stack
   pointer on this machine.  */
#define STACK_BOUNDARY 36

/* Alignment required for a function entry point, in bits.  */
#define FUNCTION_BOUNDARY 36

/* Biggest alignment that any data type can require on this machine,
   in bits.  */
#define BIGGEST_ALIGNMENT 36

/* A C expression to compute the alignment for a variable in the
   static store.  TYPE is the data type, and ALIGN is the alignment
   that the object would ordinarily have.  The value of this macro is
   used instead of that alignment to align the object.  */
#define DATA_ALIGNMENT(TYPE, ALIGN) \
  (((ALIGN) < BITS_PER_WORD) ? BITS_PER_WORD : (ALIGN))

/* A C expression to compute the alignment given to a constant that is
   being placed in memory.  EXP is the constant and ALIGN is the
   alignment that the object would ordinarily have.  The value of this
   macro is used instead of that alignment to align the object.  */
#define CONSTANT_ALIGNMENT(EXP, ALIGN) \
  (((TREE_CODE ((EXP)) == STRING_CST) && ((ALIGN) < BITS_PER_WORD)) ? BITS_PER_WORD : (ALIGN))

/* A C expression to compute the alignment for a variable in the local
   store.  TYPE is the data type, and ALIGN is the alignment that the
   object would ordinarily have.  The value of this macro is used
   instead of that alignment to align the object.  */
#define LOCAL_ALIGNMENT(TYPE, ALIGN) \
  DATA_ALIGNMENT((TYPE), (ALIGN))

/* A C expression to compute the size of a variable in the local
   store.  TYPE is the data type, and SIZE is the size that the
   object would ordinarily have.  */
#define LOCAL_SIZE(TYPE, SIZE) \
  (((SIZE) < UNITS_PER_WORD) ? UNITS_PER_WORD : (SIZE))

/* A C expression to generate some RTL code to align X, which is RTX
   for the stack pointer.  ALIGN is the alignment in storage units.  */
#define ALIGN_STACK_POINTER(X, ALIGN) \
  pdp10_align_stack_pointer ((X), (ALIGN))

/* Alignment in bits to be given to a structure bit field that follows
   an empty field such as int : 0;  */
#define EMPTY_FIELD_BOUNDARY  36

/* Number of bits which any structure or union's size must be a
   multiple of.  */
#define STRUCTURE_SIZE_BOUNDARY 36

/* Define this macro to be the value 1 if instructions will fail to
   work if given data not on the nominal alignment.  */
#define STRICT_ALIGNMENT 1

/* A code distinguishing the floating point format of the target
   machine.  */
#define TARGET_FLOAT_FORMAT PDP10_FLOAT_FORMAT

/* What is the inherent alignment of a bit offset value
    -mtc 10/15/2007
*/
#define NATURAL_ALIGNMENT(OFFSET) \
	((unsigned) (OFFSET) % BITS_PER_UNIT ? 1 :			\
				(((OFFSET) / BITS_PER_UNIT) & -((OFFSET) / BITS_PER_UNIT)) * BITS_PER_UNIT)




/**********************************************************************

	Layout of Source Language Data Types

**********************************************************************/

/* A C expression for the size in bits of the type @code{int} on the
   target machine.  If you don't define this, the default is one word.  */
#define INT_TYPE_SIZE 36

/* A C expression for the size in bits of the type @code{short} on the
   target machine.  If you don't define this, the default is half a word.  */
#define SHORT_TYPE_SIZE 18

/* A C expression for the size in bits of the type @code{long} on the
   target machine.  If you don't define this, the default is one word.  */
#define LONG_TYPE_SIZE 36

/* A C expression for the size in bits of the type @code{long long} on
   the target machine.  If you don't define this, the default is two
   words.  If you want to support GNU Ada on your machine, the value
   of this macro must be at least 64.  */
#define LONG_LONG_TYPE_SIZE 72

/* A C expression for the size in bits of the type @code{char} on the
   target machine.  If you don't define this, the default is
   BITS_PER_UNIT.  */
#define CHAR_TYPE_SIZE pdp10_char_bytesize
extern int pdp10_char_bytesize;

/* A C expression for the size in bits of the type bool on the target
   machine.  */
#define BOOL_TYPE_SIZE 36

/* A C expression for the size in bits of the type @code{float} on the
   target machine.  If you don't define this, the default is one word.  */
#define FLOAT_TYPE_SIZE 36

/* A C expression for the size in bits of the type @code{double} on
   the target machine.  If you don't define this, the default is two
   words.  */
#define DOUBLE_TYPE_SIZE 72

/* An expression whose value is 1 or 0, according to whether the type
   char should be signed or unsigned by default.  */
#ifndef DEFAULT_SIGNED_CHAR
#define DEFAULT_SIGNED_CHAR 0
#endif


/**********************************************************************

	Register Usage

**********************************************************************/

/*
 * Basic Characteristics of Registers
 */

/* The number of hardware registers known to the compiler. MJD */
#define FIRST_PSEUDO_REGISTER 022

/* An initializer that says which registers are used for fixed
   purposes all throughout the compiled code and are therefore not
   available for general allocation.

   AC0 is reserved as a scratch register.  AC17 is the stack pointer.  */
#define FIXED_REGISTERS		\
{				\
  1, 0, 0, 0, 0, 0, 0, 0,	\
  0, 0, 0, 0, 1, 1, 1, 1,	\
  1, 1				\
}

/* Like FIXED_REGISTERS, but has 1 for each register that is clobbered
   (in general) by function calls as well as for fixed registers.

   The default is to clobber AC1 - AC7, which are used to pass
   arguments, and all frame registers.

   NOTE: This must be a superset of FIXED_REGISTERS  */
#define CALL_USED_REGISTERS	\
{				\
  1, 1, 1, 1, 1, 1, 1, 1,	\
  0, 0, 0, 0, 1, 1, 1, 1,	\
  1, 1				\
}

/* Zero or more C statements that may conditionally modify five
   variables fixed_regs, call_used_regs, global_regs, reg_names, and
   reg_class_contents, to take into account any dependence of these
   register sets on target flags.  */
#define CONDITIONAL_REGISTER_USAGE 

/*
 * Order of Allocation of Registers
 */

/* An initializer for a vector of integers, containing the numbers of
   hard registers in the order in which GCC should prefer to use them
   (from most preferred to last).  */
#define REG_ALLOC_ORDER			\
{					\
     7,  6,  5,  4,  3,  2,  1,  8,	\
     9, 10, 11, 12, 13, 14, 15,  0,	\
    16, 17				\
}

/*
 * How Values Fit in Registers
 */

/* A C expression for the number of consecutive hard registers,
   starting at register number REGNO, required to hold a value of mode
   MOD.  */
#define HARD_REGNO_NREGS(REGNO, MODE) \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* A C expression that is nonzero if it is permissible to store a
   value of mode MODE in hard register number REGNO (or in several
   registers starting with that one).  */
#define HARD_REGNO_MODE_OK(REGNO, MODE)	1

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */
#define MODES_TIEABLE_P(MODE1, MODE2) 	1


/**********************************************************************

	Register Classes

**********************************************************************/

/* An enumeral type that must be defined with all the register class
   names as enumeral values.  NO_REGS must be first.  ALL_REGS must be
   the last register class, followed by one more enumeral value,
   LIM_REG_CLASSES, which is not a register class but rather tells how
   many classes there are.  One of the classes must be named
   GENERAL_REGS.  There is nothing terribly special about the name,
   but the operand constraint letters 'r' and 'g' specify this class.
   If GENERAL_REGS is the same as ALL_REGS, just define it as a macro
   which expands to ALL_REGS.  */
enum reg_class
{
  NO_REGS,
  INDEX_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};
#define GENERAL_REGS ALL_REGS

/* The number of distinct register classes.  */
#define N_REG_CLASSES (int) LIM_REG_CLASSES

/* An initializer containing the names of the register classes as C
   string constants.  */
#define REG_CLASS_NAMES	\
{			\
  "NO_REGS",		\
  "INDEX_REGS",		\
  "ALL_REGS",		\
}

/* An initializer containing the contents of the register classes, as
   integers which are bit masks.  */
/* In 4.1.1 the constraint check can end up objecting to the use of registers
    17 and 18 unless they're marked as among the GENERAL_REGS which
    for the PDP10 is the same as ALL_REGS.
    It's possible that marking them in this way will prevent eventual elimination of
    them, in which case we'll need to do something else to allow recognition,
    perhaps directly in constrain_operands() or reg_fits_class_p()
    -mtc 4/17/2007
*/
#define REG_CLASS_CONTENTS  		\
{					\
  { 0x0000000 }, /* NO_REGS  */		\
  { 0x000FFFE }, /* INDEX_REGS */	\
  { 0x000FFFF }, /* ALL_REGS */		\
}

/* A C expression whose value is a register class containing the hard
   register REGNO.  In general there is more than one such class;
   choose a class which is minimal, meaning that no smaller class
   contains the register.  */
#define REGNO_REG_CLASS(REGNO) \
  ((REGNO) == 0 ? ALL_REGS : INDEX_REGS)

/* A macro whose definition is the name of the class to which a valid
   base register must belong.  A base register is one used in an
   address which is the register value plus a displacement.  */
#define BASE_REG_CLASS INDEX_REGS

/* A macro whose definition is the name of the class to which a valid
   index register must belong.  An index register is one used in an
   address where its value is either multiplied by a scale factor or
   added to another register.  */
#define INDEX_REG_CLASS NO_REGS

/* A C expression which defines the machine-dependent operand
   constraint letters for register classes.  */
#define REG_CLASS_FROM_LETTER(C)		\
  ((C) == 'x' ? INDEX_REGS :			\
   (C) == 'r' ? ALL_REGS :	\
   NO_REGS)

/* A C expression which is nonzero if register number REGNO is
   suitable for use as a base register in operand addresses.  */
#define REGNO_OK_FOR_BASE_P(REGNO) \
  REGNO_REG_CLASS (REGNO) == INDEX_REGS

/* A C expression which is nonzero if register number REGNO is
   suitable for use as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(REGNO) 0

/* A C expression that places additional restrictions on the register
   class to use when it is necessary to copy value X into a register
   in class CLASS.  */
#define PREFERRED_RELOAD_CLASS(X, CLASS) (CLASS)

/* A C expression for the maximum number of consecutive registers of
   class CLASS needed to hold a value of mode MODE.  */
#define CLASS_MAX_NREGS(CLASS, MODE) \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* A C expression that defines the machine-specific operand constraint
   letters ('I', 'J', 'K', ... 'P') that specify particular ranges of
   integer values.  */
#define CONST_OK_FOR_LETTER_P(VALUE, C) \
  pdp10_const_ok_for_letter_p ((VALUE), (C))

/* A C expression that defines the machine-dependent operand
   constraint letters that specify particular ranges of CONST_DOUBLE
   values ('G' or 'H').  */
#define CONST_DOUBLE_OK_FOR_LETTER_P(X, C) \
  pdp10_const_double_ok_for_letter_p ((X), (C))

/* A C expression that defines the optional machine-dependent
   constraint letters ('Q', 'R', 'S', 'T', 'U') that can be used to
   segregate specific types of operands, usually memory references,
   for the target machine.  */
#define EXTRA_CONSTRAINT(OP, C) pdp10_extra_constraint ((OP), (C))


/**********************************************************************

	Stack Layout and Calling Conventions

**********************************************************************/

/* Define this if addresses of constant functions
   shouldn't be put through pseudo regs where they can be cse'd.
   Desirable on machines where ordinary constants are expensive
   but a CALL with constant address is cheap.  

   Added this because in general its more efficient to just use a literal
   -mtc 11/14/2008
*/
#define NO_FUNCTION_CSE


/*
 * Basic Stack Layout
 */

/* Define this macro if successive arguments to a function occupy
   decreasing addresses on the stack.  */
#define ARGS_GROW_DOWNWARD 1

/* Offset from the frame pointer to the first local variable slot to
   be allocated.

   Find the next slot's offset by adding the length of the first slot
   to the value STARTING_FRAME_OFFSET.  */
#define STARTING_FRAME_OFFSET 0

/* Offset from the stack pointer register to the location above the
   first location at which outgoing arguments are placed.  */
/* This value is in bytes, so should not be 1, it's conceivable it should
   be 4 instead of 0
   -mtc 6/26/2007
*/
#define STACK_POINTER_OFFSET 0

/* Offset from the argument pointer register to the location above the
   first argument's address.  */
/* Old definition seemed plain wrong.  Make it dependent on -mdual-stack
    as that now controls whether a return address exists in the stack between the
    frame pointer and the stack parameters
    -mtc 8/14/2008 */
#define FIRST_PARM_OFFSET(FNDECL) \
 (TARGET_DUALSTACK ? 0 : -1)

/* Offset from the stack pointer register to an item dynamically
   allocated on the stack, e.g., by alloca.  */
#define STACK_DYNAMIC_OFFSET(FUNDECL) 1

/* A C expression whose value is RTL representing the value of the
   return address for the frame COUNT steps up from the current frame.  */
#define RETURN_ADDR_RTX(COUNT, FRAME) \
	((FRAME) == frame_pointer_rtx ?				\
		gen_rtx_MEM (Pmode, gen_rtx_PLUS (Pmode, (FRAME), constm2_rtx)) :	\
		gen_rtx_MEM (Pmode, gen_rtx_PLUS (Pmode, (FRAME), constm1_rtx)) )

#if 0
#define INCOMING_RETURN_ADDR_RTX \
  gen_rtx_MEM (Pmode, stack_pointer_rtx)
#endif

#define INCOMING_FRAME_SP_OFFSET -UNITS_PER_WORD

/*
 * Fixed register assignments
 */

/* a reserved scratch register, may be used at any time in asm generation.
   NOTE: unlike all other registers, this register CANNOT be used with 
   indexed addressing modes. */
#define ASM_SCRATCH_REGNUM 000

/* Register number used for passing a function's static chain pointer. */
#define STATIC_CHAIN_REGNUM 014

/* The actual frame pointer */
#define HARD_FRAME_POINTER_REGNUM 015

/* Reserved for position-independent code.  */
/* #define PIC_SOMETHING_REGNUM 016 */

/* The register number of the stack pointer register, which must also
   be a fixed register according to FIXED_REGISTERS.  */
#define STACK_POINTER_REGNUM 017

/* The register number of the frame pointer register, which is used to
   access automatic variables in the stack frame.

   NOTE: This is an eliminable virtual register and should never appear after
   register reassignment.  See ELIMINABLE_REGS */
#define FRAME_POINTER_REGNUM 020

/* The register number of the arg pointer register, which is used to
   access the function's argument list.

   This is an eliminable virtual register and should never appear after 
   register reassignment.  See ELIMINABLE_REGS */
#define ARG_POINTER_REGNUM 021

/*
 * Eliminating Frame Pointer(s) and Arg Pointer
 */

/* A C expression which is nonzero if a function must have and use a
   frame pointer.  */
#define FRAME_POINTER_REQUIRED 0

#define ELIMINABLE_REGS						\
{ { ARG_POINTER_REGNUM,		STACK_POINTER_REGNUM },		\
  { FRAME_POINTER_REGNUM,	STACK_POINTER_REGNUM },		\
  { HARD_FRAME_POINTER_REGNUM,	STACK_POINTER_REGNUM },		\
  { ARG_POINTER_REGNUM,		HARD_FRAME_POINTER_REGNUM },	\
  { FRAME_POINTER_REGNUM,	HARD_FRAME_POINTER_REGNUM } }	\

#define CAN_ELIMINATE(FROM, TO) 1

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET) \
  (OFFSET) = pdp10_initial_elimination_offset ((FROM), (TO))

/*
 * Passing Function Arguments on the Stack
 */

/* The maximum amount of space required for outgoing argsuments will
   be computed and placed into the variable
   current_function_outgoing_args_size.  No space will be pushed onto
   the stack for each call; instead, the function prologue should
   increase the stack frame size by this amount.  */
#define ACCUMULATE_OUTGOING_ARGS 1

/* Define this macro if functions should assume that stack space has
   been allocated for arguments even when their values are passed in
   registers.

   This space can be allocated by the caller, or be a part of the
   machine-dependent stack frame: OUTGOING_REG_PARM_STACK_SPACE says
   which.
#define REG_PARM_STACK_SPACE(FNDECL)
*/

/* A C expression that should indicate the number of bytes of its own
   arguments that a function pops on returning, or 0 if the function
   pops no arguments and the caller must therefore pop them all after
   the function returns.  */
#define RETURN_POPS_ARGS(FUNDECL, FUNTYPE, SIZE) 0

/*
 * Passing Arguments in Registers
 */

/* The default number of registers used to pass arguments.  */
#define PDP10_DEFAULT_REGPARM 7


/*

*/
#define FUNCTION_ARG_SIZE(MODE, TYPE) \
	(((((MODE) == BLKmode) ? int_size_in_bytes(TYPE) : GET_MODE_SIZE(MODE)) \
		+ (UNITS_PER_WORD - 1) ) / UNITS_PER_WORD)

/* A C expression that controls whether a function argument is passed
   in a register, and which register.  */
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
  (((NAMED) && ((CUM) + FUNCTION_ARG_SIZE(MODE, TYPE)<= pdp10_regparm)) ? \
  	gen_rtx_REG ((MODE), 1 + (CUM)) : 0)

/* A C expression for the number of words, at the beginning of an
   argument, must be put in registers.  The value must be zero for
   arguments that are passed entirely in registers or that are
   entirely pushed on the stack.  */
/* We generate bad code for split parameters both at the procedure call and in the
    procedure entry.  Its pure foolishness anyhow, so hopefully not defining this will
    turn it off.
    -mtc 8/25/2006
*/
#if 0
#define FUNCTION_ARG_PARTIAL_NREGS(CUM, MODE, TYPE, NAMED)		\
  ((NAMED)								\
   && (CUM) < pdp10_regparm						\
   && pdp10_regparm < (CUM) + (((MODE) == BLKmode			\
				? int_size_in_bytes (TYPE)		\
				: GET_MODE_SIZE (MODE)) + 3) / 4	\
   ? pdp10_regparm - (CUM) : 0)
#endif

/* A C type for declaring a variable that is used as the first
   argument of FUNCTION_ARG and other related values.

   For the PDP-10, the type is int and holds the number of registers
   used for arguments so far.  */
#define CUMULATIVE_ARGS int

/* A C statement for initializing the variable CUM for the state at
   the beginning of the argument list.  */
#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, INDIRECT, N_NAMED_ARGS) \
  (CUM) = (FNTYPE && RETURN_IN_MEMORY (TREE_TYPE (FNTYPE))) ? 1 : 0

/* A C statement to update the summarizer variable CUM to advance past
   an argument in the argument list.  */
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)	\
  (CUM) += (((NAMED) && ((CUM) + FUNCTION_ARG_SIZE(MODE, TYPE)<= pdp10_regparm)) ? \
	  FUNCTION_ARG_SIZE(MODE, TYPE) : 0)

/* FUNCTION_ARG_PADDING(MODE, TYPE)

   A C expression which determines wheter, and in which direction, to
   pad out an argument with extra space.  */

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which function arguments are sometimes passed.  This
   does not include implicit arguments such as the static chain and
   the structure-value adress.  */
#define FUNCTION_ARG_REGNO_P(REGNO) \
  ((REGNO) >= 1 && ((unsigned int)(REGNO)) <= pdp10_regparm)

/*
 * How Scalar Function Values Are Returned
 */

/* A C expression to create an RTX representing the place where a
   function returns a value of data type VALTYPE.  */
#define FUNCTION_VALUE(VALTYPE, FUNC) gen_rtx_REG (TYPE_MODE (VALTYPE), 1)

/* A C expression to create an RTX representing the place where a
   library function returns a value of mode MODE.  */
#define LIBCALL_VALUE(MODE) gen_rtx_REG ((MODE), 1)

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which the values of a function may come back.  A
   register whose use for returning values is limited to serving as
   the second of a pair need not be recognized by this macro.  */
#define FUNCTION_VALUE_REGNO_P(REGNO) ((REGNO) == 1)

/*
 * How Large Values Are Returned
 */

/* A C expression which can inhibit the returning of certain function
   values in registers, based on the type of the value.  */
/* limit size to 4*UNITS_PER_WORD not pdp10_retregs*UNITS_PER_WORD
    because any register return has to return as some machine type
    -mtc 6/6/2007
*/
#define RETURN_IN_MEMORY(TYPE)						     \
  ((TYPE_SIZE (TYPE) && !TREE_CONSTANT (TYPE_SIZE (TYPE)))		     \
   || (TYPE_MODE (TYPE) == BLKmode					     \
       ? int_size_in_bytes (TYPE)					     \
       : GET_MODE_SIZE (TYPE_MODE (TYPE))) > 4 * UNITS_PER_WORD)

/* Define this macro to be 1 if all structure and union return values
   must be in memory.  Since this results in slower code, this should
   be defined only if needed for compatibility with other compilers or
   with an ABI.  If you define this macro to be 0, then the
   conventions used for structure and union return values are decided
   by the RETURN_IN_MEMORY macro.  */
#define DEFAULT_PCC_STRUCT_RETURN 0

/* used in pdp10_struct_value_rtx which is referenced by TARGET_STRUCT_VALUE_RTX  */
#define PDP10_STRUCT_VALUE_REGNUM 1

/*
 * Caller-Saves Register Allocation
 */

/* A C expression to determine whether it is worthwhile to consider
   placing a pseudo-register in a call-clobbered hard register and
   saving and restoring it around each function call.  The expression
   should be 1 when this is worth doing, and 0 otherwise.
#define CALLER_SAVE_PROFITABLE(REFS, CALLS) (4 * (CALLS) < (REFS))
*/

/*
 * Function Entry and Exit
 */

/* A C compound statement that outputs the assembler code for a thunk
   function, used to implement C++ virtual function calls with
   multiple inheritance.

   If you do not define this macro, the target-independent code in the
   C++ frontend will generate a less efficient heavyweight thunk that
   calls FUNCTION instead of jumping to it.  The generic approach does
   not support varargs.
#define ASM_OUTPUT_MI_THUNK(FILE, THUNK_FNDECL, DELTA, FUNCTION)
*/

/*
 * Generating Code for Profiling
 */

/* A C statement or compound statement to output to FILE some
   assembler code to call the profiling subroutine mcount.  Before
   calling, the assembler code must load the address of a counter
   variable into a register where mcount expects to find the
   address.
*/
#define FUNCTION_PROFILER(FILE,LABELNO) \
  pdp10_function_profiler ((FILE), (LABELNO))

/* MACHINE_STATE_SAVE
   MACHINE_STATE_RESTORE.  */


/**********************************************************************

	Tail Calls

**********************************************************************/

#define FUNCTION_OK_FOR_SIBCALL(DECL) 1
  /* !(current_function_stdarg || current_function_varargs) */


/**********************************************************************

	Trampolines for Nested Functions

**********************************************************************/

/* A "trampoline" is a small piece of code that is created at run time
when the address of a nested function is taken.  It normally resides
on the stack, in the stack frame of the containing function.  These
macros tell GCC how to generate code to allocate and initialize a
trampoline.

   The instructions in the trampoline must do two things: load a
constant address into the static chain register, and jump to the real
address of the nested function.  On CISC machines such as the m68k,
this requires two instructions, a move immediate and a jump.  Then the
two addresses exist in the trampoline as word-long immediate operands.
On RISC machines, it is often necessary to load each address into a
register in two parts.  Then pieces of each address form separate
immediate operands.

   The code generated to initialize the trampoline must store the
variable parts -- the static chain value and the function address --
into the immediate operands of the instructions.  On a CISC machine,
this is simply a matter of copying each address to a memory reference
at the proper offset from the start of the trampoline.  On a RISC
machine, it may be necessary to take out pieces of the address and
store them separately.  */

/* A C statement to output, on the stream FILE, assembler code for a
   block of data that contains the constant part of a trampoline.

   On the PDP-10 the trampoline looks like this (if 14 is the static
   chain regnum):
	MOVE 14,.+2
	JRST @.+2
	static_chain_value
	funtion_address

   Or, if all code is in one section:
	MOVE 14,.+2
	JRST function_address
	static_chain_value

   Or, if everything is in one section:
	MOVE 14,static_chain_value
	JRST function_address */
/*
  Change to just initialize the trampoline in place.  Also change to always create it as a 4 word sequence
  since we no longer support small or medium models
  -mtc 9/11/2009
#define TRAMPOLINE_TEMPLATE(FILE)					  \
do {									  \
  rtx op;								  \
  op = gen_rtx_REG (SImode, STATIC_CHAIN_REGNUM);			  \
                                                                          \
  if (TARGET_SMALL)							  \
    output_asm_insn ("movei %0,0", &op);				  \
  else									  \
    output_asm_insn ("move %0,.+2", &op);				  \
                                                                          \
  if (TARGET_SMALL || TARGET_MEDIUM)					  \
    output_asm_insn ("jrst 0", &op);					  \
  else                                                                    \
    {                                                                     \
      output_asm_insn ("jrst @.+2", &op);		                  \
      assemble_integer (const0_rtx, UNITS_PER_WORD, BITS_PER_WORD, true); \
    }									  \
                                                                          \
  if (!TARGET_SMALL)							  \
    assemble_integer (const0_rtx, UNITS_PER_WORD, BITS_PER_WORD, true);	  \
} while (0)
*/

/* A C expression for the size in bytes of the trampoline, as an
   integer. 
#define TRAMPOLINE_SIZE (TARGET_SMALL ? 8 : TARGET_MEDIUM ? 12 : 16) */
#define TRAMPOLINE_SIZE (16)

/* A C statement to initialize the variable parts of a trampoline.  */
#define INITIALIZE_TRAMPOLINE(ADDR, FNADDR, CXT)			 \
do {									 \
  rtx tmp = gen_reg_rtx(SImode);                                         \
  \
  /* MOVE SC, .+2 */ \
  emit_move_insn(tmp, GEN_INT(0200600000000));  \
  emit_insn (gen_HRRI(tmp, plus_constant((ADDR), 2))); \
  emit_move_insn(gen_rtx_MEM(SImode, (ADDR)), tmp);  \
  \
  /* JRST @.+2 */ \
  emit_move_insn(tmp, GEN_INT(0254020000000));  \
  emit_insn (gen_HRRI(tmp, plus_constant((ADDR), 3))); \
  emit_move_insn(gen_rtx_MEM(SImode, plus_constant((ADDR), 1)), tmp);  \
  \
  /* static chain address */ \
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant ((ADDR), 2)),	 \
		    (CXT));						 \
  /* function address */ \
  emit_move_insn(tmp, (FNADDR));             \
  emit_move_insn (gen_rtx_MEM (SImode, plus_constant ((ADDR), 3)), tmp);	 \
  \
} while (0)


/**********************************************************************

	Addressing Modes

**********************************************************************/

/* A C expression that is nonzero if the machine supports pre-increment
   addressing.  */
#define HAVE_PRE_INCREMENT 1

/* A C expression that is nonzero if the machine supports
   post-increment addressing.  */
#define HAVE_POST_INCREMENT TARGET_XKL2

/* A C expression that is 1 if the RTX is a constant which is a valid
   address.  */
#define CONSTANT_ADDRESS_P(X)			\
  (GET_CODE (X) == CONST			\
   || GET_CODE (X) == CONST_INT			\
   || GET_CODE (X) == LABEL_REF			\
   || GET_CODE (X) == SYMBOL_REF)

/* A number, the maximum number of registers that can appear in a
   valid memory address.  Note that it is up to you to specify a value
   equal to the maximum number that GO_IF_LEGITIMATE_ADDRESS would
   ever accept.  */
#define MAX_REGS_PER_ADDRESS 1

/* A C compound statement with a contitional goto LABEL; executed if X
   (an RTX) is a legitimate memory address on the target machine for a
   memory operand of mode MODE.

   This macro must exist in two variants: a strict variant and a
   non-strict one.  The strict variant is used in the reload pass.  It
   must be defined so that any pseudo-register that has not been
   allocated a hard register is considered a memory reference.  In
   contexts where some kind of register is required, a pseudo-register
   with no hard register must be rejected.  */
#ifdef REG_OK_STRICT
#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, LABEL)  	\
{							\
  if (pdp10_legitimate_address_p ((MODE), (X), 1))	\
    goto LABEL;						\
}
#else
#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, LABEL)  	\
{							\
  if (pdp10_legitimate_address_p ((MODE), (X), 0))	\
    goto LABEL;						\
}
#endif

/* A C expression that is nonzero if X (assumed to be a reg RTX) is
   valid for use as a base register of a memory reference in mode
   MODE.  For hard registers, it should always accept those which the
   hardware permits and reject the others.  Whether the macro accepts
   or rejects pseudo-registers must be controlled by REG_OK_STRICT
   as described above.

   On the PDP-10, byte pointers are usable in any register, but word
   pointers can only be used in an index register.  */
#ifdef REG_OK_STRICT
#define REG_MODE_OK_FOR_BASE_P(X, MODE)		\
  (GET_MODE_SIZE (MODE) < UNITS_PER_WORD ||	\
   REGNO_REG_CLASS (REGNO (X)) == INDEX_REGS)
#else
#define REG_MODE_OK_FOR_BASE_P(X, MODE)		\
  (REGNO (X) >= FIRST_PSEUDO_REGISTER ||	\
   GET_MODE_SIZE (MODE) < UNITS_PER_WORD ||	\
   REGNO_REG_CLASS (REGNO (X)) == INDEX_REGS)
#endif

/* A C expression that is nonzero if X (assumed to be a reg RTX) is
   valid for use as an index register.  */
#define REG_OK_FOR_INDEX_P(X) 0

/* A C compound statement that attempts to replace X with a valid
   memory address for an operand of mode MODE.  WIN will be a C
   statement label elsewhere in the code.  */
#define LEGITIMIZE_ADDRESS(X, OLDX, MODE, WIN)			\
do {								\
  rtx _tmpx = pdp10_legitimize_address ((X), (OLDX), (MODE));	\
  if (_tmpx)							\
    {								\
      (X) = _tmpx;						\
      goto WIN;							\
    }								\
} while (0)

/* A C compound statement that attempts to replace X, which is an
   address that needs reloading, with a valid memory address for an
   operand of mode MODE.  WIN will be a C statement label elsewhere in
   the code.  It is not necessary to define this macro, but it might
   be useful for performance reasons
#define LEGITIMIZE_RELOAD_ADDRESS(X, MODE, OPNUM, TYPE, IND_LEVELS, WIN)
*/

/* A C statement or compound statement with a conditional goto LABEL;
   executed if memory address X (an RTX) can have different meanings
   depending on the machine mode of the memory reference it is used
   for or if the address is valid for some modes but not others.  */
#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR,LABEL)	\
{							\
  if (GET_CODE (ADDR) == PRE_INC)			\
    goto LABEL;						\
}

/* A C expression that is nonzero if X is a legitimate constant for an
   immediate operand on the target machine.  You can assume that X
   satisfies CONSTANT_P, so you need not check this.  */
#define LEGITIMATE_CONSTANT_P(X) 1


/**********************************************************************

	Describing Relative Costs of Operations

**********************************************************************/

/* A C expression for the cost of moving data of mode MODE from a
   register in class FROM to a register in class TO.

   On the PDP-10, a register-to-register move is always one, fast
   instruction.  */
#define REGISTER_MOVE_COST(MODE, FROM, TO) 1

/* A C expression for the cost of moving data of mode MODE from a
   register of class CLASS and memory; IN is zero if the value is to
   be written to memory, nonzero if it is to be read in.  This cost is
   relative to REGISTER_MOVE_COST.

   Moves to and from memory are quite expensive.  */
#define MEMORY_MOVE_COST(MODE, CLASS, IN) \
  (1 + GET_MODE_SIZE (MODE) / UNITS_PER_WORD)

/* A C expression for the cost of a branch instruction.  A value of 1
   is the default; other values are interpreted relative to that.  */
#define BRANCH_COST 1

/* Define this macro as a C expression which is nonzero if accessing
   less than a word of memory is no faster than accessing a word of
   memory.  */
#define SLOW_BYTE_ACCESS 1

/* A C expression used to determine whether a load preincrement is a
   good thing to use for a given mode.  */
#define USE_LOAD_POST_INCREMENT(MODE) GET_MODE_SIZE (MODE) < 4

/* A C expression used to determine whether a store preincrement is a
   good thing to use for a given mode.  */
#define USE_STORE_POST_INCREMENT(MODE) GET_MODE_SIZE (MODE) < 4

/* A C expression used to determine whether a load preincrement is a
   good thing to use for a given mode.  */
#define USE_LOAD_PRE_INCREMENT(MODE) GET_MODE_SIZE (MODE) < 4

/* A C expression used to determine whether a store preincrement is a
   good thing to use for a given mode.  */
#define USE_STORE_PRE_INCREMENT(MODE) GET_MODE_SIZE (MODE) < 4


/**********************************************************************

	Position Independent Code

**********************************************************************/

/* This section describes macros that help implement generation of
position independent code.  Simply defining these macros is not enough
to generate valid PIC; you must also add support to the macros
GO_IF_LEGITIMATE_ADDRESS and PRINT_OPERAND_ADDRESS well as
LEGITIMIZE_ADDRESS.  You must modify the definition of movsi to do
something appropriate when the source operand contains a symbolic
address.  You may also need to alter the handling of switch statements
so that they use relative addresses.  */

/* The register number of the register used to address a table of
   static data addresses in memory.  In some cases this register is
   defined by a processor's ABI.
#define PIC_OFFSET_TABLE_REGNUM 015
*/

/* FINALIZE_PIC */

/* LEGITIMATE_PIC_OPERAND */


/**********************************************************************

	Miscellaneous Parameters

**********************************************************************/

/* An alias for a machine mode name.  This is the machine mode that
   elements of a jump-table should have.  */
#define CASE_VECTOR_MODE SImode

/* Define this macro if operations between registers with integral
   mode smaller than a word are alway performed on the entire
   register.  */
#define WORD_REGISTER_OPERATIONS

/* Define this macro to be a C expression indicating when insns that
   read memory in MODE, an integral mode narrower than a word, set the
   bits outside of MODE to be either the sign-extension or the
   zero-extension of the data read.

   PDP-10 byte instructions zero-extend the registers.  */
#define LOAD_EXTEND_OP(MODE) ZERO_EXTEND

/* The maximum number of bytes that a single instruction can move
   quickly between memory and registers or between two memory
   locations.  */
#define MOVE_MAX (2*UNITS_PER_WORD)

/* The maximum number of bytes that a single instruction can move
   quickly between memory and registers or between two memory
   locations.  If this is undefined, the default is MOVE_MAX.
   Otherwise, it is the constant value that is the largest value that
   MOVE_MAX can have at run-time.
#define MAX_MOVE_MAX 8
*/

/* A C expression which is nonzero if on this machine it is safe to
   "convert" an integer of INPREC bits to one of OUTPREC bits (where
   OUTPREC is smaller than INPREC) by merely operating on it as if it
   had only OUTPREC bits.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* A C expression describing the value returned by a comparison
   operator with an integral mode and stored by a store-flag
   instruction when the condition is true.  */
#define STORE_FLAG_VALUE 1

/* An alias for the machine mode for pointers.  */
#define Pmode SImode

/* An alias for the machine mode used for memory references to
   functions being called.  */
#define FUNCTION_MODE SImode

#define REGISTER_TARGET_PRAGMAS() \
  pdp10_register_target_pragmas()

/* Define this macro to control use of the character '$' in identifier
   names.  0 means '$' is not allowed by default; 1 means it is
   allowed.  */
#ifndef DOLLARS_IN_IDENTIFIERS
#define DOLLARS_IN_IDENTIFIERS 0
#endif

/* Define this macro as a C string constant for the linker argument to
   link in the system math library, or "" if the target machine does
   not have a separate math library.  */
#define MATH_LIBRARY ""

#define NAME__MAIN "__main"

enum pdp10_builtins
{
  PDP10_BUILTIN_FFO,
  PDP10_BUILTIN_FSC,
  PDP10_BUILTIN_DFSC
};

#define EXPAND_BUILTIN_VA_START(VALIST, NEXTARG) \
  pdp10_expand_builtin_va_start ((VALIST), (NEXTARG))


/**********************************************************************

	External Variable Declarations

**********************************************************************/

extern rtx pdp10_compare_op0, pdp10_compare_op1;

extern unsigned int pdp10_regparm;
extern unsigned int pdp10_retregs;


/**********************************************************************

	Miscellaneous

**********************************************************************/
#define MD_ARRAY_OFFSET(EXP, INDEX, UNIT_SIZE, OFFSET, BIT_OFFSET) \
  pdp10_md_array_offset ((EXP), (INDEX), (UNIT_SIZE), &(OFFSET), &(BIT_OFFSET))

#define MD_EXPAND_BINOP(MODE,OPTAB,OP0,OP1,TARGET,UNSIGNEDP,METHODS,EXP) \
  pdp10_expand_binop (MODE, OPTAB, OP0, OP1, TARGET, UNSIGNEDP, METHODS, EXP)

#define gen_rtx_ADJBP(MODE, OP0, OP1) \
  gen_rtx_UNSPEC ((MODE), gen_rtvec (2, (OP0), (OP1)), UNSPEC_ADJBP)

/*	for now include ptr_mode in the list of modes recognized as pointers
	eventually should remove, but start out permissive to reduce chance of
	breaking stuff
	-mtc 7/14/2006
*/
#define PTR_MODE_P(x) \
	((x) == Q6Pmode \
	|| (x) == Q7Pmode \
	|| (x) == Q8Pmode \
	|| (x) == Q9Pmode \
	|| (x) == HPmode \
	|| (x) == WPmode \
	|| (x) == ptr_mode)
	
#endif /* __PDP10_H__ */
