/* Target Macros for the RCA CDP1802 processor.
   Copyright (C) 2024 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef GCC_CDP1802_H
#define GCC_CDP1802_H

/* Controlling the Compilation Driver.  */

#undef  LINK_SPEC
#define LINK_SPEC "%{h*} %{v:-V} \
		  %{static:-Bstatic} %{shared:-shared} %{symbolic:-Bsymbolic}"

#undef  LIB_SPEC
#define LIB_SPEC "-lc"

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC "crt0.o%s crtbegin.o%s"

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC "crtend.o%s"

/* Run-time Target Specification.  */

#define TARGET_CPU_CPP_BUILTINS() cdp1802_cpu_cpp_builtins (pfile)

/* Storage Layout.  */

#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 1
#define WORDS_BIG_ENDIAN 1
#define UNITS_PER_WORD 2

#define PARM_BOUNDARY 8
#define STACK_BOUNDARY 8
#define FUNCTION_BOUNDARY 8
#define BIGGEST_ALIGNMENT 8
#define EMPTY_FIELD_BOUNDARY 8
#define STRICT_ALIGNMENT 0
#define PCC_BITFIELD_TYPE_MATTERS 0
#define MAX_FIXED_MODE_SIZE 32

/* Layout of Source Language Data Types.  */

#define INT_TYPE_SIZE 16
#define SHORT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64

#define DEFAULT_SIGNED_CHAR 0

#define SIZE_TYPE "unsigned int"
#define PTRDIFF_TYPE "int"

#define WCHAR_TYPE "long unsigned int"
#define WCHAR_TYPE_SIZE 32

/* Register Usage.  */

/* CDP1802 hardware registers.  */
#define CDP1802_R0	0		/* fixed, DMA and PC at boot */
#define CDP1802_R1	1		/* fixed, interrupt handler PC */
#define CDP1802_R2	2		/* fixed, interrupt and NCRT SP */
#define CDP1802_R3	3		/* fixed, NCRT PC */
#define CDP1802_R4	4		/* fixed, NCRT call routine */
#define CDP1802_R5	5		/* fixed, NCRT return routine */
#define CDP1802_R6	6		/* caller-save, static chain, indirect call target */
#define CDP1802_R7	7		/* caller-save, argument, return value */
#define CDP1802_R8	8		/* caller-save, argument, return value */
#define CDP1802_R9	9		/* caller-save, argument, return value */
#define CDP1802_R10	10		/* caller-save, argument, return value */
#define CDP1802_R11	11		/* callee-save */
#define CDP1802_R12	12		/* callee-save */
#define CDP1802_R13	13		/* callee-save */
#define CDP1802_R14	14		/* callee-save */
#define CDP1802_R15	15		/* callee-save, optional frame pointer */
#define CDP1802_D	16		/* fixed, 8-bit accumulator */
#define CDP1802_DF	17		/* caller-save, 1-bit carry/borrow flag */
#define CDP1802_X	18		/* fixed, 4-bit register selector for memory accesses */

/* CDP1802 virtual registers.  */
#define CDP1802_FP	19		/* fixed, virtual frame pointer */
#define CDP1802_AP	20		/* fixed, virtual argument pointer */

/* CDP1802 NCRT (New Call and Return Technique) register aliases.  */
#define CDP1802_SP	CDP1802_R2	/* NCRT and hardware interrupt Stack Pointer */
#define CDP1802_PC	CDP1802_R3	/* NCRT Program Counter */
#define CDP1802_CALL	CDP1802_R4	/* NCRT routine to make recursive calls */
#define CDP1802_RETN	CDP1802_R5	/* NCRT routine to return from recursive calls */

/* GCC register aliases.  */
#define CDP1802_ARG0	CDP1802_R7	/* first argument register */
#define CDP1802_RV	CDP1802_R7	/* first return value register */
#define CDP1802_SC	CDP1802_R6	/* static chain */
#define CDP1802_HFP	CDP1802_R15	/* hard frame pointer */
#define CDP1802_CARRY	CDP1802_DF	/* carry/borrow flag */

/* Basic Characteristics of Registers.  */

#define FIRST_PSEUDO_REGISTER 21

#define FIXED_REGISTERS {		\
  1, 1, 1, 1,	/*  r0  r1  SP  PC */	\
  1, 1, 0, 0,	/*  r4  r5  r6  r7 */	\
  0, 0, 0, 0,	/*  r8  r9 r10 r11 */	\
  0, 0, 0, 0,	/* r12 r13 r14 r15 */	\
  1, 0, 1, 1,	/*   D  DF   X  FP */	\
  1		/*  AP		   */	}

/* Ones indicate caller-saved (callee-clobbered) registers.  */
#define CALL_REALLY_USED_REGISTERS {	\
  0, 0, 0, 0,	/*  r0  r1  SP  PC */	\
  0, 0, 1, 1,	/*  r4  r5  r6  r7 */	\
  1, 1, 1, 0,	/*  r8  r9 r10 r11 */	\
  0, 0, 0, 0,	/* r12 r13 r14 r15 */	\
  0, 1, 0, 0,	/*   D  DF   X  FP */	\
  0		/*  AP		   */	}

#define PC_REGNUM CDP1802_PC

/* Order of Allocation of Registers.  */

#define REG_ALLOC_ORDER { 10, 9, 8, 7, 6, 12, 13, 14, 15, 11, 17, 0, 1, 2, 3, 4, 5, 16, 18 }

/* How Values Fit in Registers.  */

#undef AVOID_CCMODE_COPIES

/* Registers Classes.  */

enum reg_class
{
  NO_REGS,
  A_REGS,		/* first argument register */
  B_REGS,		/* second argument register */
  Z_REGS,		/* hidden argument register (static chain, indirect calls) */
  GENERAL_REGS,		/* r0-r15, fp, ap */
  ALL_REGS,
  LIM_REG_CLASSES
};

#define N_REG_CLASSES ((int) LIM_REG_CLASSES)

#define REG_CLASS_NAMES {	\
  "NO_REGS",			\
  "A_REGS",			\
  "B_REGS",			\
  "Z_REGS",			\
  "GENERAL_REGS",		\
  "ALL_REGS"			}

#define REG_CLASS_CONTENTS {			\
  0x00000000,					\
  0x00000080, /* A_REGS: r7 */			\
  0x00000100, /* B_REGS: r8 */			\
  0x00000040, /* Z_REGS: r6 */			\
  0x0018FFFF, /* GENERAL_REGS: r0-r15, FP, AP */\
  (1 << FIRST_PSEUDO_REGISTER) - 1		}

#define REGNO_REG_CLASS(REGNO) cdp1802_regno_reg_class ((REGNO))

#define BASE_REG_CLASS GENERAL_REGS
#define INDEX_REG_CLASS NO_REGS
#define REGNO_OK_FOR_BASE_P(NUM) 1
#define REGNO_OK_FOR_INDEX_P(NUM) 0

/* Stack Layout and Calling Conventions.  */

/* Basic Stack Layout.  */

#define STACK_GROWS_DOWNWARD 1
#define STACK_PUSH_CODE	PRE_DEC /* not exactly, but gcc's POST_DEC is very wrong */
#define FRAME_GROWS_DOWNWARD 1
#undef ARGS_GROW_DOWNWARD
#define STACK_POINTER_OFFSET 1

#define FIRST_PARM_OFFSET(FUNDECL) 0

/* After the prologue, RA is at AP-2.  */
#define RETURN_ADDR_RTX(COUNT, FRAMEADDR) 				\
  ((COUNT) == 0								\
   ? gen_rtx_MEM (Pmode, plus_constant (Pmode, arg_pointer_rtx, -2))	\
   : NULL_RTX)

/* Before the prologue, RA is at SP+1.  */
#define INCOMING_RETURN_ADDR_RTX					\
  gen_rtx_MEM (Pmode, plus_constant (Pmode, stack_pointer_rtx, 1))

#define INCOMING_FRAME_SP_OFFSET 2

/* Exception Handling Support.  */

/* Use the caller-save r7..r10 for eh return and static chain for eh stack adj.  */
#define EH_RETURN_DATA_REGNO(N) ((N) < 4 ? CDP1802_ARG0 + (N) : INVALID_REGNUM)
#define EH_RETURN_STACKADJ_RTX gen_rtx_REG (Pmode, CDP1802_SC)
#define EH_RETURN_HANDLER_RTX INCOMING_RETURN_ADDR_RTX

/* Registers That Address the Stack Frame */

#define STACK_POINTER_REGNUM CDP1802_SP
#define FRAME_POINTER_REGNUM CDP1802_FP
#define HARD_FRAME_POINTER_REGNUM CDP1802_HFP
#define ARG_POINTER_REGNUM CDP1802_AP
#define STATIC_CHAIN_REGNUM CDP1802_SC

/* Eliminating Frame Pointer and Arg Pointer.  */

#define ELIMINABLE_REGS					\
{							\
  {FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM},		\
  {FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM},	\
  {ARG_POINTER_REGNUM,	 STACK_POINTER_REGNUM},		\
  {ARG_POINTER_REGNUM,	 HARD_FRAME_POINTER_REGNUM},	\
}

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  do {									\
    (OFFSET) = cdp1802_initial_elimination_offset ((FROM), (TO));	\
  } while (0)

/* Passing Function Arguments on the Stack.  */

#define PUSH_ARGS_REVERSED 1
#define PUSH_ROUNDING(BYTES) cdp1802_push_rounding ((BYTES))

/* Passing Arguments in Registers.  */

#define CUMULATIVE_ARGS int

#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS) \
  do { \
    (CUM) = 0; \
  } while (0)

#define NUM_ARGUMENT_REGISTERS  4
#define FIRST_ARGUMENT_REGISTER CDP1802_ARG0
#define RETURN_VALUE_REGNUM CDP1802_RV

#define FUNCTION_ARG_REGNO_P(REGNO)					\
  ((REGNO) >= FIRST_ARGUMENT_REGISTER 					\
   && (REGNO) < FIRST_ARGUMENT_REGISTER + NUM_ARGUMENT_REGISTERS)

/* How Large Values Are Returned.  */

#define DEFAULT_PCC_STRUCT_RETURN 0
#undef PCC_STATIC_STRUCT_RETURN

/* Function Entry and Exit.  */

#define EPILOGUE_USES(REGNO) cdp1802_epilogue_uses_p ((REGNO))

/* Generating Code for Profiling.  */

#define FUNCTION_PROFILER(FILE, LABELNO) cdp1802_function_profiler ((FILE), (LABELNO))
#define NO_PROFILE_COUNTERS 1

/* Support for Nested Functions.  */

#define TRAMPOLINE_SIZE 9
#define TRAMPOLINE_ALIGNMENT 8

/* Implicit Calls to Library Routines.  */

/* Addressing Modes.  */

#define HAVE_POST_DECREMENT 1	/* stxd */
#define HAVE_POST_INCREMENT 1	/* lda, ldxa, str-inc */

#define MAX_REGS_PER_ADDRESS 1

/* Describing Relative Costs of Operations.  */

#define BRANCH_COST(SPEED_P, PREDICTABLE_P) 3

#define SLOW_BYTE_ACCESS 0

#define USE_LOAD_POST_DECREMENT(MODE) 0		/* Post-decrement is only available for stores.  */

#define NO_FUNCTION_CSE 1			/* Indirect calls are expensive.  */

/* Dividing the Output into Sections.  */

#define TEXT_SECTION_ASM_OP	"\t.text"
#define DATA_SECTION_ASM_OP	"\t.data"
#define BSS_SECTION_ASM_OP	"\t.bss"

/* The Overall Framework of an Assembler File.  */

#define ASM_COMMENT_START "#"
#define ASM_APP_ON "#APP\n"
#define ASM_APP_OFF "#NO_APP\n"

/* Output of Data.  */

/* TODO: see if we could use e.g. '|' for this */
#define IS_ASM_LOGICAL_LINE_SEPARATOR(C, STR) 0

/* Output and Generation of Labels.  */

#define GLOBAL_ASM_OP "\t.global\t"

/* Output of Assembler Instructions.  */

#define REGISTER_NAMES {        \
  "0", "1", "2", "3",		\
  "4", "5", "6", "7",		\
  "8", "9", "10", "11",		\
  "12", "13", "14", "15",	\
  "?d", "?cc", "?x", "?fp",	\
  "?ap"				}

#define ASM_OUTPUT_REG_PUSH(STREAM, REGNO) cdp1802_output_reg_push ((STREAM), (REGNO))
#define ASM_OUTPUT_REG_POP(STREAM, REGNO) cdp1802_output_reg_pop ((STREAM), (REGNO))

/* Output of Dispatch Tables.  */

#define ASM_OUTPUT_ADDR_VEC_ELT(STREAM, VALUE) cdp1802_output_addr_vec_elt ((STREAM), (VALUE))

/* Exception Region Output.  */

#define DWARF2_UNWIND_INFO		0	/* TODO: NYI */
#define DWARF_CIE_DATA_ALIGNMENT	(-1)

/* Assembler Commands for Alignment.  */

#define ASM_OUTPUT_ALIGN(STREAM, POWER) cdp1802_output_align ((STREAM), (POWER))

#define ASM_OUTPUT_ALIGN_WITH_NOP(STREAM, POWER) cdp1802_output_align_with_nop ((STREAM), (POWER))

/* Miscellaneous Parameters.  */

#define HAS_LONG_COND_BRANCH true
#define HAS_LONG_UNCOND_BRANCH true
#define CASE_VECTOR_MODE HImode
#undef WORD_REGISTER_OPERATIONS
#define MOVE_MAX 1		/* TODO: 1804 can do 2 */
#define Pmode HImode
#define FUNCTION_MODE QImode

#endif /* GCC_CDP1802_H */
