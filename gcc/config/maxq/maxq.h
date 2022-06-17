/* Definitions of target machine for GCC.
   Dallas MAXQ version generating coff
   Copyright (C) 2004, 2005  Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#define SDB_DEBUGGING_INFO 1

#define SDB_DELIM "\n"

#ifndef GCC_MAXQ_H
#define GCC_MAXQ_H
#endif

/* Saved operands from the last compare to use when we generate a branch insn.  */
extern rtx maxq_compare_op0;
extern rtx maxq_compare_op1;

/* This variable tells whether the last comparison is signed.  */
extern int maxq_signed_cmp;


#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crt1.o%s"

#undef LIB_SPEC
#define LIB_SPEC "-lc"

/******************************************************************/
/*                     Run-time Target Specification              */
/******************************************************************/

/* Target CPU builtins.  */
#define TARGET_CPU_CPP_BUILTINS()                       \
  do                                                    \
    {                                                   \
      if (TARGET_MAXQ20)                                \
        {                                               \
          builtin_define ("__MAXQ20__");                \
          builtin_assert ("cpu=maxq20");                \
          builtin_assert ("machine=maxq20");            \
        }                                               \
      else if (TARGET_MAXQ10)                           \
        {                                               \
          builtin_define ("__MAXQ10__");                \
          builtin_assert ("cpu=maxq10");                \
          builtin_assert ("machine=maxq10");            \
        }                                               \
    }                                                   \
  while (0)

/* Run-time compilation parameters selecting different hardware subsets.  */

extern int target_flags;

/* Masks for the -m switches.  */
#define MASK_MAXQ10             0x00000001
#define MASK_MAXQ20             0x00000010

/* Select between the MAXQ10 and MAXQ20 cpus  */
#define TARGET_MAXQ20   (target_flags & MASK_MAXQ20)
#define TARGET_MAXQ10   (target_flags & MASK_MAXQ10)

#ifdef IN_LIBGCC2
#undef TARGET_MAXQ20
#undef TARGET_MAXQ10
/* If compiling libgcc2, make these compile time constants based on what
   flags are we actually compiling with.  */
#ifdef __MAXQ20__
#define TARGET_MAXQ20   1
#define DF SF
#else
#define TARGET_MAXQ20   0
#endif
#ifdef __MAXQ10__
#define TARGET_MAXQ10   1
#define DF SF
#else
#define TARGET_MAXQ10   0
#endif
#endif /* !IN_LIBGCC2 */

/* Macro to define tables used to set the target flags.
   We have two variants of MAXQ.  */
#define TARGET_SWITCHES                                                             \
{                                                                                   \
  {"q10",                MASK_MAXQ10, N_("Generate MAXQ10 code")},                  \
  {"q20",                MASK_MAXQ20, N_("Generate MAXQ20 code")},                  \
  { "",                  TARGET_DEFAULT, NULL}                                      \
}

#ifndef TARGET_CPU_DEFAULT
#define TARGET_CPU_DEFAULT MASK_MAXQ20
#endif

/* Default target_flags if no switches specified.  */
#define TARGET_DEFAULT  (TARGET_CPU_DEFAULT)

#define TARGET_VERSION fprintf (stderr, " (Dallas Maxim MaxQ)");

/* Basic block reordering is only beneficial on targets with cache 
   and/or variable-cycle branches where (cycle count taken != 
   cycle count not taken).                    
   Loop unrolling and inline functions will increase the code size. 
   Code memory size is very small (64Kb) on MAXQ, So we disable 
   the optimizations which will increase the code size.  */
#define OPTIMIZATION_OPTIONS(LEVEL, SIZE)                                \
  do                                                                     \
    {                                                                    \
      flag_reorder_blocks = 0;                                           \
      flag_no_peephole = 1;                                              \
      flag_loop_optimize = 0;						 \
      flag_unswitch_loops = 0;						 \
      flag_peel_loops = 0;						 \
      flag_unroll_loops = 0;                                             \
      flag_unroll_all_loops = 0;                                         \
      flag_inline_functions = 0;                                         \
    }                                                                    \
  while (0)

/**********************************************************************************************/
/*                        Storage Layout.                                                     */
/**********************************************************************************************/

/* This target is little endian.  */
#define BITS_BIG_ENDIAN 	0
#define BYTES_BIG_ENDIAN 	0
#define WORDS_BIG_ENDIAN 	0

#define BITS_PER_UNIT           8
#define UNITS_PER_WORD          ( (TARGET_MAXQ20)  ? 2 : 1 )

#define MIN_UNITS_PER_WORD      1

#define MAX_BITS_PER_WORD       16
#define MAX_FIXED_MODE_SIZE     ( (TARGET_MAXQ20)  ? 32 : 16 )

/* Allocation boundary (in bits) for storing arguments in argument list.  */
#define PARM_BOUNDARY 		(TARGET_MAXQ20 ? 16 : 8 )

/* minimum allocation boundary (in bits) for the code of a function.  */
#define FUNCTION_BOUNDARY 		16

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 		16

#define PCC_BITFIELD_TYPE_MATTERS  	 0
/* Biggest alignment that any data type can require on this machine, in bits  */
#define BIGGEST_ALIGNMENT       	16

#define STACK_BOUNDARY  		16

/* Set this zero if the instructions will merely go slower when given unaligned data.  */
#define STRICT_ALIGNMENT 		 0

#define MINIMUM_ATOMIC_ALIGNMENT 	16

/* The best alignment  */
#define FASTEST_ALIGNMENT 		16

#undef POINTER_SIZE
#define POINTER_SIZE 			16

/* Number of bits in the high and low parts of a two stage load of an immediate constant.  */
#define BITS_PER_HIGH   8
#define BITS_PER_LO_SUM 8

/**********************************************************************************************/
/* Layout of Source Language Data Types */
/**********************************************************************************************/

#define SHORT_TYPE_SIZE         (TARGET_MAXQ20 || TARGET_MAXQ10 ? 16 : 8)
#define INT_TYPE_SIZE           16
#define CHAR_TYPE_SIZE          8
#define LONG_TYPE_SIZE          32
#define LONG_LONG_TYPE_SIZE     32
#define FLOAT_TYPE_SIZE         32
#define DOUBLE_TYPE_SIZE        32
#define LONG_DOUBLE_TYPE_SIZE   DOUBLE_TYPE_SIZE
#define SIZE_TYPE               "unsigned int"
#define PTRDIFF_TYPE            "int"
#define DEFAULT_SIGNED_CHAR     1

/**********************************************************************************************/
/* Register Usage */
/**********************************************************************************************/

#define A0_REGNO    0
#define A1_REGNO    1
#define A2_REGNO    2
#define A3_REGNO    3
#define A4_REGNO    4
#define A5_REGNO    5
#define A6_REGNO    6
#define A7_REGNO    7
#define A14_REGNO  14
#define LC0_REGNO  15
#define LC1_REGNO  16
#define GR_REGNO   17
#define ACC_REGNO  18
#define AP_REGNO   19
#define APC_REGNO  20
#define BP_REGNO   21		/* BP */
#define DP_REGNO   22		/* DP[0] */
#define SP_REGNO   23		/* DP[1] */
#define OFF_REGNO  24
#define GRH_REGNO  25
#define GRL_REGNO  26
#define GRXL_REGNO 27
#define PSF_REGNO  28		/*PSF register */
#define DPC_REGNO  29
/* fake registers for comparition  */
#define CMP_REG    30
#define MEM1_REG   31
#define MEM2_REG   32
#define MEM3_REG   33
#define ARG_REG    34
#define FIRST_PSEUDO_REGISTER  35

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.  */
#define FIXED_REGISTERS                                                   \
{                                                                         \
/* A[0] A[1] A[2] A[3] A[4] A[5] A[6] A[7] A[8] A[9] A[10]  */            \
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,                  \
/* A[11] A[12] A[13] A[14] LC[0] LC[1]  GR  ACC  AP  APC */	 	  \
    0,    0,     0,    0,   0,   0,     1,    1,   1,   1,		  \
/* BP DP[0] DP[1]  OFFS  GRH GRL GRXL PSF DPC  */			  \
    1,  1,    1,     1,   1,  1,   1,   1,  1,				  \
/* CMP  MEM1 MEM2 MEM3  ARG */ 		 				  \
    1,   1,   1,   1,   1	         				  \
}

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.  */
#define CALL_USED_REGISTERS                                               \
{                                                                         \
/* A[0] A[1] A[2] A[3] A[4] A[5] A[6] A[7] A[8] A[9] A[10]  */            \
    1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,                  \
/* A[11] A[12] A[13] A[14]  LC[0] LC[1]  GR  ACC   AP  APC */             \
    0,    0,     0,    0,    0,    0,     1,   1,   1,   1,               \
/* BP  DP[0] DP[1]  OFFS   GRH GRL GRXL PSF DPC  */                       \
    1,   1,    1,     1,    1,  1,   1,   1,  1,                          \
/* CMP MEM1 MEM2 MEM3 ARG */  			   		          \
    1,	1,   1,   1,  1      	                                          \
}

/* A C initializer containing the assembler's names for the machine registers,
   each one as a C string constant.  */
#define REGISTER_NAMES                                                          \
{                                                                               \
 "A[0]", "A[1]", "A[2]", "A[3]", "A[4]", "A[5]", "A[6]", "A[7]", "A[8]",        \
 "A[9]", "A[10]", "A[11]", "A[12]", "A[13]", "A[14]","LC[0]", "LC[1]","GR",     \
 "ACC", "AP", "APC","BP", "DP[0]", "DP[1]", "OFFS",  "GRH", "GRL",        	\
 "GRXL", "PSF", "DPC", "CMP_REG", "MEM1", "MEM2", "MEM3", "ARG"			\
}

/* On MAXQ10 LC0 and LC1 are 16-bit registers where as other general registers
   are 8-bit width. So LC0 and LC1 are not used in MAXQ10 port.  */
#define CONDITIONAL_REGISTER_USAGE              			     \
{                                               			     \
  if (TARGET_MAXQ10)                          				     \
   {									     \
     CLEAR_HARD_REG_BIT (reg_class_contents[(int)GENERAL_REGS],LC1_REGNO);   \
     CLEAR_HARD_REG_BIT (reg_class_contents[(int)GENERAL_REGS],LC0_REGNO);   \
     fixed_regs[LC0_REGNO] = 1;                                              \
     fixed_regs[LC1_REGNO] = 1;                                              \
     call_used_regs[LC0_REGNO] = 1;                                          \
     call_used_regs[LC1_REGNO] = 1;                                          \
     fixed_regs[GR_REGNO]  = 1;						     \
     fixed_regs[DP_REGNO] = 0;						     \
   }                                            			     \
}


/**********************************************************************************************/
/* Order of register allocation */
/**********************************************************************************************/
#define REG_ALLOC_ORDER                                         \
{  4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3, 12, 13, 14, 16, 15,    \
   17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32,  \
   33, 34                        		         		\
}

/* Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.
   GRL, GRH and OFFS registers are 8-bit registers.  
   On MAXQ10 registers A0 - A14 are 8 bit and others 16 bit registers.  */
#define HARD_REGNO_NREGS(REGNO, MODE)    					\
 ((TARGET_MAXQ20 || REGNO <= A14_REGNO) ? ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1)  	\
	/ UNITS_PER_WORD) : (REGNO == GRL_REGNO || REGNO == GRH_REGNO || REGNO == OFF_REGNO )   \
        ? 1  : ((GET_MODE_SIZE (MODE) + 2 * UNITS_PER_WORD - 1)                                 \
        / (2 * UNITS_PER_WORD)))

/* Return 1 if hard register REGNO can hold a value of machine-mode MODE.  
   Two registers are needed to hold 4 byte value on MAXQ20 so LC1 register
   cannot be used to hold 4 byte value, because LC1 is the last register in
   general register class.
   Similarly 2 registers are needed to hold 2 byte value and 4 register are needed
   to hold 4 byte value on MAXQ10.  */
#define HARD_REGNO_MODE_OK(REGNO, MODE)   						\
 ((TARGET_MAXQ20) 									\
 ? ((MODE == QImode || MODE == HImode)? 1 : ( (MODE == SFmode || MODE == SImode)	\
	&& (REGNO < BP_REGNO)) ? 1 : (REGNO < LC1_REGNO)				\
	? 1 : 0)									\
 : (MODE == QImode)                                                                     \
   ? 1 : (MODE == HImode && ( (REGNO <= A14_REGNO - 1) \
       || REGNO == BP_REGNO || REGNO == LC0_REGNO || REGNO == DPC_REGNO                 \
       || REGNO == DP_REGNO || REGNO == SP_REGNO                                        \
   || REGNO == GR_REGNO )) ? 1		\
   : ((MODE == SFmode || MODE == SImode || MODE == DFmode) && ((REGNO <= A14_REGNO-3)))	\
   ? 1 : 0)

/* Return nonzero if a value of mode mode1 is accessible in mode mode2 without copying.  
 On MAXQ all registers are equivalent  */
#define MODES_TIEABLE_P(MODE1, MODE2)	1

#define IS_MAXQ10_DEST(X)                       		\
 (IS_ACC_REGS(X) || (GET_CODE(X) == REG && REGNO(X) == ACC_REGNO) || GET_CODE(X) == MEM)

#define IS_ACC_REGS(X)	(GET_CODE(X) == REG && (REGNO(X) <= A14_REGNO))

#define IS_ACC_REGNO(X)                         		\
 ((X) <= A14_REGNO)

#define CHECK_SYMBOL_REF(X)                     		\
 (GET_CODE(X) == SYMBOL_REF || GET_CODE(X) == CONST)

/* Register classes.  */
enum reg_class
{
  NO_REGS,
  BASE_REGS,               /* Base registers.  */
  LOOP_REGS,               /* LC0 and LC1 registers.  */
  GENERAL_REGS,            /* General purpose registers A[0] - A[14], LC0, LC1..  */
  GR_REGS,
  ACTIVE_ACC_REG,
  ACC_POINTER_REG,
  ACC_CONTROL_REG,
  DP0_REG,
  STACK_POINTER_REG,        /* Software stack pointer DP[1].  */
  DATAPOINTERCONTROL_REG,
  CONTROL_REGS,             /* Hardware control registers APC ( Accmulator Pointer Control
                              Registers), DPC (Data Pointer Control Registers).. */
  COMPARE_REGS, 
  ALL_REGS,
  LIM_REG_CLASSES
};
#define N_REG_CLASSES (int) LIM_REG_CLASSES

/* Give names of register classes as strings for dump file.  */
#define REG_CLASS_NAMES 	\
{				\
 "NO_REGS",			\
 "BASE_REGS",			\
 "LOOP_REGS",                   \
 "GENERAL_REGS",		\
 "GR_REGS",			\
 "ACTIVE_ACC_REG",		\
 "ACC_POINTER_REG",		\
 "ACC_CONTROL_REG",             \
 "DP0_REG",			\
 "STACK_POINTER_REG",           \
 "DATAPOINTERCONTROL_REG",	\
 "CONTROL_REGS",		\
 "COMPARE_REGS",		\
 "ALL_REGS"			\
}

/* Define which registers fit in which classes.
   This is an initializer for a vector of HARD_REG_SET
   of length N_REG_CLASSES.  */
#define REG_CLASS_CONTENTS 						\
{                                                       		\
 {0x00000000,   0x00000000},   /* No registers  */      		\
 {0x00207FFF,   0x00000004},   /* BASE_REGS A0 - A14,BP / BP,DP0 */	\
 {0x00018000,   0x00000000},   /* LOOP_REGS: LC0, LC1    */             \
 {0x0001FFFF,   0x00000000},   /* GENERAL_REGS: A[0]-A14 / A0-A14,GRL,GRH  */   \
 {0x00020000,   0x00000000},   /* GR_REG		*/		\
 {0x00040000,   0x00000000},   /* ACTIVE_ACC_REG: ACC 	 */		\
 {0x00080000,   0x00000000},   /* ACC_POINTER_REG: AP     */   		\
 {0x00100000,   0x00000000},   /* ACC_CONTROL_REG: APC	 */		\
 {0x00400000,   0x00000000},   /* DP0_REG: DP[0] */			\
 {0x00800000,   0x00000000},   /* STACK_POINTER_REG: DP[1] */		\
 {0x20000000,   0x00000000},   /* DATAPOINTERCONTROL_REG DPC  */	\
 {0x00DE0000,   0x00000000},   /* CONTROL_REGS		   */		\
 {0xFFFFFFFF,   0x00000007}    /* All registers */      		\
}

/* Return the class number of the smallest class containing
   reg number REGNO. */
#define REGNO_REG_CLASS(REGNO) 							\
 ((REGNO <= A14_REGNO && TARGET_MAXQ20)? BASE_REGS 	                	\
    : (REGNO <= A14_REGNO) ? GENERAL_REGS     					\
    : (REGNO == LC0_REGNO || REGNO == LC1_REGNO) ? LOOP_REGS      		\
    : (REGNO == BP_REGNO || REGNO == ARG_REG                                    \
           ||(TARGET_MAXQ10 && REGNO == DP_REGNO)) ? BASE_REGS                  \
    : (REGNO == LC0_REGNO && !TARGET_MAXQ20) ? BASE_REGS			\
    : (REGNO == ACC_REGNO) ? ACTIVE_ACC_REG 		                	\
    : (REGNO == AP_REGNO) ? ACC_POINTER_REG					\
    : (REGNO == APC_REGNO) ? ACC_CONTROL_REG 					\
    : (REGNO == DP_REGNO) ? DP0_REG 						\
    : (REGNO == SP_REGNO) ? STACK_POINTER_REG 					\
    : (REGNO == DPC_REGNO) ? DATAPOINTERCONTROL_REG				\
    : (REGNO == GR_REGNO) ? GR_REGS						\
    : ((REGNO == DPC_REGNO) || (REGNO == AP_REGNO) || (REGNO == APC_REGNO) 	\
        || (REGNO == DP_REGNO) 							\
        || (REGNO == ACC_REGNO) || (REGNO == GR_REGNO)) ? CONTROL_REGS  	\
    : NO_REGS)

/* A[0] - A[14], BP, and ARG are Base Registers.  */
#define BASE_REG_CLASS   BASE_REGS

/* There is no index register on MAXQ.  */
#define INDEX_REG_CLASS NO_REGS

/* Machine-dependent operand constraint letters for register classes.
   The register letter `r', corresponding to class `GENERAL_REGS',
   will not be passed to this macro; We do not need to handle it.  */
#define REG_CLASS_FROM_LETTER(C) 	\
 ( (C == 'b') ? BASE_REGS		\
 : (C == 'l') ? LOOP_REGS		\
 : (C == 'a') ? ACTIVE_ACC_REG		\
 : (C == 'c') ? ACC_CONTROL_REG		\
 : (C == 'k') ? DP0_REG			\
 : (C == 'q') ? STACK_POINTER_REG	\
 : (C == 'd') ? ACC_POINTER_REG		\
 : (C == 'h') ? GR_REGS			\
 : (C == 'f') ? DATAPOINTERCONTROL_REG 	\
 : (C == 't') ? CONTROL_REGS		\
 :  NO_REGS )

/* A C expression which is nonzero if register number NUM is suitable for use
   as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(NUM)  0

/* General register class contains all general purpose registers.
   So we do not have any other more restrictive class.  */
#define PREFERRED_RELOAD_CLASS(X,CLASS)	(CLASS)

/* A C expression for the maximum number of consecutive registers of
   class CLASS needed to hold a value of mode MODE.
   This is closely related to the macro `HARD_REGNO_NREGS'.  
   On MAXQ10 GENERAL_REGS contains 8-bit registers and others contain
   16-bit registers.  */
#define CLASS_MAX_NREGS(CLASS, MODE) 						\
  ((TARGET_MAXQ20 || CLASS == GENERAL_REGS ) 					\
  ? ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)		\
  : ((GET_MODE_SIZE (MODE) + 2 * UNITS_PER_WORD - 1) /(2 * UNITS_PER_WORD)))

#define SMALL_REGISTER_CLASSES 0

#define CONST_DOUBLE_OK_FOR_LETTER_P(OP, C) 0
#define CONST_OK_FOR_LETTER_P(VAL, C)  1

/* Offset from the frame pointer to the first local variable slot to
   be allocated.  
   We reserve first 4 bytes for temporary usage.  */
#define STARTING_FRAME_OFFSET  4
#define STACK_ALIGNMENT_NEEDED 0

/* Offset from the argument pointer register to the first argument's address.  */
#define FIRST_PARM_OFFSET(FNDECL) 0

/* The register number of the stack pointer register.  */
#define STACK_POINTER_REGNUM  SP_REGNO

/* The register number of the frame pointer register, which is used to access
   automatic variables in the stack frame.  */
#define FRAME_POINTER_REGNUM  BP_REGNO

/* The register number of the arg pointer register, which is used to access the
   function's argument list.  */
#define ARG_POINTER_REGNUM    ARG_REG

/* ARG_REG is not a hard register. It should be eliminated with frame pointer.  */
#define ELIMINABLE_REGS  \
{{ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM}}

#define CAN_ELIMINATE(FROM, TO) 1

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET) \
 ((OFFSET) = get_frame_size () + 4) 

/* A[3] is used for passing a function's static chain pointer.  */
#define STATIC_CHAIN_REGNUM   A3_REGNO

/* Frame pointer should be used.  */
#define FRAME_POINTER_REQUIRED  1

/* push insns will be used to pass outgoing arguments  */
#define PUSH_ARGS 1
#define PUSH_ARGS_REVERSED 1

/* push insn pushes exactly 'BYTES' no of bytes.  */
#define PUSH_ROUNDING(BYTES) (BYTES)

/* Functions pop no arguments and callers pop them all after the function returns.  */
#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE) (0)

/* The variable args holds the number of registers used
   for arguments so far.  */
typedef struct maxq_args
{
  int args;
} CUMULATIVE_ARGS;

/* Registers A[0], A[1], A[2], and A[3] can be used to pass an argument  */
#define FUNCTION_ARG_REGNO_P(NUM) (NUM <= A3_REGNO)

/* Initialize the variable CUM.  */
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,INDIRECT,NAMED) 	\
do {									\
 (CUM).args = A0_REGNO;							\
} while(0)

/* Return no of registers required to pass an argument of mode MODE */
#define ARG_REGNO(MODE)	\
 ((TARGET_MAXQ20) ?							\
 ((MODE==QImode || MODE == HImode) ? 1 : ( MODE == DFmode) ? 2 : 2)	\
 : ((MODE == QImode) ? 2 : (MODE == HImode) ? 2 : 4))

/* A C expression that controls whether a function argument is passed in a
   register, and which register.  
   Only registers A[0]-A[3] is used to pass an argument.  */
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
 (((CUM).args  < A4_REGNO) ? gen_rtx_REG (MODE, (CUM).args) : 0)

/* Update the variable CUM.  */
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED) 	\
{							\
	(CUM).args += ARG_REGNO(MODE);			\
}

#define STACK_GROWS_DOWNWARD

/* A[0] is used to hold the return value of a
   function of data type VALTYPE. */
#define FUNCTION_VALUE(VALTYPE, FUNC) \
        gen_rtx_REG( TYPE_MODE(VALTYPE), A0_REGNO)

/* A[0] is used to hold the return value of a library
   function of mode 'MODE'. */
#define LIBCALL_VALUE(MODE) \
        gen_rtx_REG (MODE, A0_REGNO)

/* In register A[0] the values of called function may come back.  */
#define FUNCTION_VALUE_REGNO_P(NUM) ((NUM) == A0_REGNO )

#define FUNCTION_PROFILER(FILE, LABELNO)

/* SYMBOL_REF and CONST_INT are valid addresses.  */
#define CONSTANT_ADDRESS_P(X) \
 ((GET_CODE (X) == SYMBOL_REF) || (GET_CODE (X) == CONST_INT))

/* Only one can appear in a valid memory address.  */
#define MAX_REGS_PER_ADDRESS    1

/* Registers A[0]-A[14], BP, and ARG can be valid base registers.  */
#define REGNO_OK_FOR_BASE_P(NUM)                                \
 ((NUM) <= A14_REGNO || (NUM) == BP_REGNO || (NUM) == ARG_REG) 

/* A C expression that is nonzero if X (assumed to be a `reg' RTX) is valid for
   use as a base register.  */
#ifndef REG_OK_STRICT

/* A C expression which is nonzero if register number NUM is suitable for use
   as a base register in operand addresses.  */
#define REG_OK_FOR_BASE_P(X) \
  (REGNO_OK_FOR_BASE_P (REGNO (X)) || (REGNO (X) >= FIRST_PSEUDO_REGISTER))
#else
/* A C expression which is nonzero if register number NUM is suitable for use
   as a base register in operand addresses.  */
#define REG_OK_FOR_BASE_P(X) REGNO_OK_FOR_BASE_P (REGNO (X))

#endif

/* A C expression that is nonzero if X (assumed to be a `reg' RTX) is valid for
   use as an index register.  */
#define REG_OK_FOR_INDEX_P(X) 0

/* Valid base registers  */
#define LEGITIMATE_ADDRESS_REG(X)                                      \
  (GET_CODE (X) == REG && (REG_OK_FOR_BASE_P (X) 			\
	|| REGNO(X) == BP_REGNO || REGNO(X) == ARG_REG))

/* Valid displacement for registers BP and ARG.  */
#define LEGITIMATE_DISP_BP(Y)						\
  (GET_CODE(Y) == CONST_INT && INTVAL(Y) >= 0				\
  && INTVAL(Y) <= 255 )

/* Valid displacement for other registers.  */
#define LEGITIMATE_DISP_GEN(Y)                                          \
  ( GET_CODE(Y) == CONST_INT )

/* Valid reg+disp addressing modes.  */
#define LEGITIMATE_ADDRESS_PLUS(X,Y)	                                \
  (((REGNO(X) == BP_REGNO ||(REGNO(X) == ARG_REG))                      \
       && LEGITIMATE_DISP_BP(Y))			                \
    || (REGNO(X) != BP_REGNO && REGNO(X) != ARG_REG                     \
       && LEGITIMATE_DISP_GEN(Y)))

/* Legitimate addressing modes.  */
#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, LABEL)                        \
{									\
if ( LEGITIMATE_ADDRESS_REG (X))                              		\
   goto LABEL;                                                        	\
else if ( (MODE == QImode || MODE == HImode) 				\
 	&& GET_CODE(X) == PLUS && LEGITIMATE_ADDRESS_REG (XEXP (X, 0))	\
	  && LEGITIMATE_ADDRESS_PLUS (XEXP(X,0), XEXP (X,1)) )		\
	  goto LABEL;							\
}

#define LEGITIMIZE_ADDRESS(X,OLDX,MODE,WIN)
#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR, LABEL)

/* Legitimate constants for an
   immediate operand on the target machine.  */
#define LEGITIMATE_CONSTANT_P(X)           				\
  ((GET_CODE (X) == CONST_DOUBLE )         				\
  || (GET_CODE (X) == CONST_INT )  					\
  || (GET_CODE (X) == SYMBOL_REF)          				\
  || (GET_CODE (X) == CONST))

#define EXTRA_CONSTRAINT(OP, C)                                         \
        ( ((C) == 'U') ? (maxq_U_constraint (OP))                       \
        : 0 )

/* Given a comparison code (EQ, NE, etc.) and the first operand of a COMPARE,
   return the mode to be used for the comparison.  */
#define SELECT_CC_MODE(OP,X,Y) HImode

/* Accessing less than a word of memory
   is faster than accessing a word of memory on MAXQ.  */
#define SLOW_BYTE_ACCESS 0

/* It is better to call a constant function
   address than to call an address kept in a register.
   Indirect function calls are more expensive than direct function calls, so
   don't cse function addresses.  */
#define NO_FUNCTION_CSE

/* Define the codes that are matched by predicates in maxq.c.  */
#define PREDICATE_CODES						\
  {"reg_or_imm_operand", {REG, SUBREG, CONST}},				\
  {"call_address_operand", {REG, SYMBOL_REF, LABEL_REF, CONST}},

/* A C expression whose value is a string containing the assembler operation
   that should precede instructions and read-only data.  */
#define TEXT_SECTION_ASM_OP "\t.text"

/* A C expression whose value is a string containing the assembler operation to
   identify the following data as writable initialized data.  */
#define DATA_SECTION_ASM_OP "\t.data"

#define READONLY_DATA_SECTION_ASM_OP "\t.sect\t.rodata"

/* A C string constant describing how to begin a comment in the target
   assembler language.  The compiler assumes that the comment will end at the
   end of the line.  */
#define ASM_COMMENT_START ";"

/* A C string constant for text to be output before each `asm' statement or
   group of consecutive ones.  */
#define ASM_APP_ON ""

/* A C string constant for text to be output after each `asm' statement or
   group of consecutive ones.  */
#define ASM_APP_OFF ""

/* Assembler derivatives for different data types.  */
#define ASM_OUTPUT_INT(STREAM,VALUE)				\
 ( fprintf ((STREAM), "\t.int"),				\
   output_addr_const ((STREAM), (VALUE)),			\
   fputs ("\n", (STREAM)))

#define ASM_OUTPUT_BYTE (STREAM,VALUE)  			\
 ( fprintf ((STREAM), "\t.byte "),                       	\
   output_addr_const ((STREAM), (VALUE)),                 	\
   fputs ("\n", (STREAM)))

#undef ASM_OUTPUT_LOCAL
#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED)     \
( fputs ("\t.lcomm ", (FILE)),                          \
  assemble_name ((FILE), (NAME)),                       \
  fprintf ((FILE), ",%d\n", (SIZE)))

#undef ASM_OUTPUT_COMMON
#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)    \
( fputs ("\t.comm ", (FILE)),                   \
  assemble_name ((FILE), (NAME)),               \
  fprintf ((FILE), ",%d\n", (SIZE)))

/* A C statement to store into the string STRING a label whose name is made
   from the string PREFIX and the number NUM.  */
#undef ASM_GENERATE_INTERNAL_LABEL
#define ASM_GENERATE_INTERNAL_LABEL(BUFFER, PREFIX, NUM) \
    sprintf (BUFFER, "*%s%ld", PREFIX, (long)(NUM))

/* In maxq assembly 'dot' is used for bit access, so it should not
   present in the label.  */
#define NO_DOT_IN_LABEL
#define NO_DOLLAR_IN_LABEL

/* Globalizing directive for a label.  */
#define GLOBAL_ASM_OP "\t.global\t"

#define INIT_SECTION_ASM_OP  "\t.sect\t\".init\""

#define OBJECT_FORMAT_COFF

#define USER_LABEL_PREFIX "_"

/* A C compound statement to output to stdio stream STREAM the assembler syntax
   for an instruction operand X.  X is an RTL expression.  */
#define PRINT_OPERAND(FILE, X, CODE) maxq_print_operand(FILE, X, CODE)

/* A C compound statement to output to stdio stream STREAM the assembler syntax
   for an instruction operand that is a memory reference whose address is X.  X
   is an RTL expression.  */
#define PRINT_OPERAND_ADDRESS(FILE, X)

/* This is how to output an element of a case-vector that is absolute.  */
#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)  \
  fprintf (FILE, "\t.int L%d\n", VALUE)

/* A C statement to output to the stdio stream STREAM an assembler command to
   advance the location counter to a multiple of 2 to the POWER bytes.  */
#define ASM_OUTPUT_ALIGN(FILE, LOG)     \
  if ((LOG) != 0)                       \
    fprintf (FILE, "\t.align\t%d\n", (LOG))

/* A C statement to output to the stdio stream stream an assembler instruction 
   to advance the location counter by nbytes bytes  */
#undef  ASM_OUTPUT_SKIP
#define ASM_OUTPUT_SKIP(FILE, SIZE) \
  fprintf (FILE, "\t.zero\t%u\n", (SIZE))

/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode HImode

/* An alias for the machine mode used for memory references to functions being
   called, in call RTL expressions.  */
#define FUNCTION_MODE HImode

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX 2

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE Pmode
#define CASE_USE_BIT_TESTS (0)

/* We do not support nested functions.  */
#define TRAMPOLINE_SIZE (1)

#define TRAMPOLINE_TEMPLATE(FILE)

#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT)
