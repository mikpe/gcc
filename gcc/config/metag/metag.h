/* Definitions of target machine for GNU compiler.
   Imagination Technologies Meta version.
   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
   2013
   Imagination Technologies Ltd

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef __METAG_H_
#define __METAG_H_

#define USE_EH_FRAME_REGISTRY

/* Which processor to schedule for. The metacore attribute defines a list that
   mirrors this list, so changes to metag.md must be made at the same time.  */

enum processor_type
{
  PROCESSOR_METAC_1_0,
  PROCESSOR_METAC_1_1,
  PROCESSOR_METAC_1_2,
  PROCESSOR_METAC_0_1,
  PROCESSOR_METAC_2_1,
  PROCESSOR_METAC_MAX
};

/* Support for a compile-time default CPU, et cetera.  The rules are:
   --with-cpu is ignored if -mmetac is specified.
   --with-tune is ignored if -mtune is specified. */
#define OPTION_DEFAULT_SPECS                                                    \
  {"cpu",  "%{!mmetac=*:-mmetac=%(VALUE)}" },                                   \
  {"tune", "%{!mtune=*:-mtune=%(VALUE)}" },                                     \
  {"fpu",  "%{mhard-float|mhard-float=*|msoft-float|msimd-float:; :-mhard-float=%(VALUE)}" } \

/* Handle the pragma to alter the default jump_table_branch size.  */
#define REGISTER_TARGET_PRAGMAS()                                               \
  do {                                                                          \
    c_register_pragma (0, "mjump", metag_pragma_jump_table_branch);             \
    c_register_pragma (0, "hwtrace_function", metag_pragma_hwtrace_function);   \
  } while (0)

#define SYMBOL_REF_P(RTX)       (GET_CODE (RTX) == SYMBOL_REF)
#define LABEL_REF_P(RTX)        (GET_CODE (RTX) == LABEL_REF)
#define CONST_DOUBLE_P(RTX)     (GET_CODE (RTX) == CONST_DOUBLE)
#define SUBREG_P(RTX)           (GET_CODE (RTX) == SUBREG)

#define TARGET_USE_JCR_SECTION  0

#define TARGET_LIBGCC_SDATA_SECTION     DATA_SECTION_ASM_OP

#define TARGET_FAST_MATH fast_math_flags_set_p ()

#define DWARF2_UNWIND_INFO              1

#define NUM_EH_RETURN_DATA_REGS                                                 \
  (EH_RETURN_LAST_DATA_REG - EH_RETURN_FIRST_DATA_REG + 1)

#define EH_RETURN_DATA_REGNO(N)                                                 \
  ((N) < NUM_EH_RETURN_DATA_REGS                                                \
   ? EH_RETURN_FIRST_DATA_REG + (N) : INVALID_REGNUM)

#define EH_RETURN_STACKADJ_RTX                                                  \
  gen_rtx_REG (SImode, EH_RETURN_STACKADJ_REG)

#define DWARF_FRAME_REGISTERS           (2 + 8 + 3)

/* An optimisation for reducing the size of an unwind table. Only registers
 * that will be present in a frame are included
 *
 * D1.0 and D1.1 are present as they are the EH_RETURN data registers
 * D0FrT and D1RtP because they store the frame pointer and return address
 * D1Ar1 -> D0Ar6 are the call save registers
 * A1LbP is present as it is the PIC register for META Linux
 * A0StP and A0FrP are obvious!
 *
 * D0.8 is not included as it is not applicable to Linux and the relevant
 * code that refers to this table is not currently used in the embedded
 * toolchain
 */

#define DWARF_REG_TO_UNWIND_COLUMN_TABLE                                        \
static signed char const dwarf_reg_to_unwind_column[FIRST_PSEUDO_REGISTER + 1] =\
{                                                                               \
  /* D0_0/D1_0 ... D0_7/D1_7   */                                               \
  -1, -1,  0,  1, -1, -1, -1, -1,  2,  3,  4,  5,  6,  7,  8,  9,               \
  /* D0_8/D1_8 ... D0_15/D1_15 */                                               \
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,               \
  /* A0_0/A1_0 ..  A0_7/A1_7 */                                                 \
  10, -1, 11, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,               \
  /* FRAME ... TXRPT */                                                         \
  -1, -1, -1, -1, -1, -1, -1, -1, -1,                                           \
  /* FX */                                                                      \
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,               \
  /* TTREC(L) */                                                                \
  -1, -1                                                                        \
}

#define DWARF_REG_TO_UNWIND_COLUMN(REGNO)                                       \
  dwarf_reg_to_unwind_column[REGNO]

/* We're using ELF files */

#define OBJECT_FORMAT_ELF

#ifndef CPP_SUBTARGET_SPEC
#define CPP_SUBTARGET_SPEC ""
#endif

#undef CPP_SPEC
#define CPP_SPEC                                                                \
  "%(cpp_cpu_arch) "                                                            \
  "%(cpp_subtarget) "                                                           \
  "%{pthread: -D_THREAD_SAFE} "

#define CPP_CPU_ARCH_SPEC ""

#ifndef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS
#endif

#ifndef SUBTARGET_CPP_SPEC
#define SUBTARGET_CPP_SPEC      ""
#endif

/* This macro defines names of additional specifications to put in the specs
   that can be used in various specifications like CC1_SPEC.  Its definition
   is an initializer with a subgrouping for each command option.

   Each subgrouping contains a string constant, that defines the
   specification name, and a string constant that used by the GNU CC driver
   program.

   Do not define this macro if it does not need to do anything.  */

#define EXTRA_SPECS                                                             \
  { "metac_default",            METAC_DEFAULT },                                \
  { "cpp_cpu_arch",             CPP_CPU_ARCH_SPEC },                            \
  { "cpp_subtarget",            CPP_SUBTARGET_SPEC },                           \
  { "subtarget_cpp_spec",       SUBTARGET_CPP_SPEC },                           \
  SUBTARGET_EXTRA_SPECS

/* Run-time compilation parameters selecting different hardware subsets.  */

extern int frame_pointer_needed;

/* Define the information needed to expand branch insns. This is stored from
   the previous compare operation - which we do not expand at all! */
extern GTY(()) rtx metag_compare_op0;
extern GTY(()) rtx metag_compare_op1;

/* Macros used in the machine description to test the flags.  */

extern int optimize;

#define TARGET_COND_EXEC_OPTIMIZE (optimize && TARGET_COND_EXEC)
#define TARGET_MINIM_CORE (TARGET_MINIM && TARGET_MINIM_OPTIMISE)

enum metag_jump_table_branch
{
  METAG_MINIM_JUMP_TABLE_BRANCH_AUTO,
  METAG_MINIM_JUMP_TABLE_BRANCH_LONG,
  METAG_MINIM_JUMP_TABLE_BRANCH_SHORT
};

extern int metag_fpu_single;
extern enum metag_jump_table_branch metag_jump_table_branch;
extern int metag_fpu_resources;
extern int metag_force_tbictxsave;

/* Access Models

   The __model__ attribute can be used to select the code model to use when
   accessing particular objects.  */

enum metag_model { METAG_MODEL_SMALL, METAG_MODEL_LARGE };

extern enum metag_model metag_model;
#define TARGET_MODEL_SMALL (metag_model == METAG_MODEL_SMALL)
#define TARGET_MODEL_LARGE (metag_model == METAG_MODEL_LARGE)

/* Target options */
enum metac_target
{
  METAC_1_0_ID,
  METAC_1_1_ID,
  METAC_1_2_ID,
  METAC_0_1_ID,
  METAC_2_1_ID
};

extern enum metac_target metac_target;

#define TARGET_METAC_0_1                                                        \
  (metac_target == METAC_0_1_ID)

#define TARGET_METAC_1_0                                                        \
  (metac_target == METAC_1_0_ID)

#define TARGET_METAC_1_1                                                        \
  (metac_target == METAC_1_1_ID || metac_target == METAC_1_2_ID ||              \
   metac_target == METAC_0_1_ID || metac_target == METAC_2_1_ID)

#define TARGET_METAC_1_2                                                        \
  (metac_target == METAC_1_2_ID || metac_target == METAC_0_1_ID                 \
   metac_target == METAC_2_1_ID)

#define TARGET_METAC_2_1                                                        \
  (metac_target == METAC_2_1_ID)

/* target machine storage layout */

/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields. */
#define BITS_BIG_ENDIAN 0

/* Define this if most significant byte of a word is the lowest numbered.  */
#define BYTES_BIG_ENDIAN 0

/* Define this if most significant word of a multiword is lowest numbered.  */
#define WORDS_BIG_ENDIAN 0

/* number of bits in an addressable storage unit */
#define BITS_PER_UNIT 8

/* Width in bits of a "word", which is the contents of a machine register.
   Note that this is not necessarily the width of data type `int';
   if using 16-bit ints on a metag, this would still be 32.
   But on a machine with 16-bit registers, this would be 16.  */
#define BITS_PER_WORD 32

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD 4

/* Width of a SIMD word, in units (bytes).  */
#define UNITS_PER_SIMD_WORD 8

/* Width in bits of a pointer.
   See also the macro `Pmode' defined below.  */
#define POINTER_SIZE 32

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */
#define PARM_BOUNDARY 32

/* Boundary (in *bits*) on which stack pointer should be aligned.  */
#define STACK_BOUNDARY 64

#define STACK_BOUNDARY_BYTES (STACK_BOUNDARY / BITS_PER_UNIT)

#define ALIGN_ON_STACK_BOUNDARY(X)                                              \
  (((X) + STACK_BOUNDARY_BYTES - 1) & ~(STACK_BOUNDARY_BYTES - 1))

/* Allocation boundary (in *bits*) for the code of a function.  */
#define FUNCTION_BOUNDARY 32

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 32

/* No data type wants to be aligned rounder than this.  */
/* metag_emb_asm_select_section asserts that BIGGEST_ALIGNMENT is 64. This is
   because a specific section is required for any alignment bigger than 4
   bytes. Currently only 8 byte alignment maximum is supported, anything
   greater is ignored and converted to 8 byte alignment.
   1, 2 and 4 byte alignment do not need special sections as the automatic
   alignment handling in the assembler will correctly align such sections
   depending on the data contained within. Only 8 byte must be explicitly
   stated. */
#define BIGGEST_ALIGNMENT 64

/* Every structure's size must be a multiple of this.  */
#define STRUCTURE_SIZE_BOUNDARY 32

/* The best alignment to use in cases where we have a choice.  */
#define FASTEST_ALIGNMENT 32

/* Make strings 32-bit aligned so strcpy from constants will be faster.  */
#define CONSTANT_ALIGNMENT(EXP, ALIGN)                                          \
  ((TREE_CODE (EXP) == STRING_CST && (ALIGN) < FASTEST_ALIGNMENT)               \
   ? FASTEST_ALIGNMENT : (ALIGN))

/* Make arrays of chars 32-bit aligned for the same reasons.  */
#define DATA_ALIGNMENT(TYPE, ALIGN)                                             \
  (TREE_CODE (TYPE) == ARRAY_TYPE                                               \
   && TYPE_MODE (TREE_TYPE (TYPE)) == QImode                                    \
   && (ALIGN) < FASTEST_ALIGNMENT ? FASTEST_ALIGNMENT : (ALIGN))

/* Make local arrays of chars 32-bit aligned for the same reasons.  */
#define LOCAL_ALIGNMENT(TYPE, ALIGN)                                            \
  (TREE_CODE (TYPE) == ARRAY_TYPE                                               \
   && TYPE_MODE (TREE_TYPE (TYPE)) == QImode                                    \
   && (ALIGN) < FASTEST_ALIGNMENT ? FASTEST_ALIGNMENT : (ALIGN))
  
/* Define this if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* Define number of bits in most basic integer type.
   (If undefined, default is BITS_PER_WORD).  */
#define INT_TYPE_SIZE 32

/* Integer bit fields have the same size and alignment as actual integers */
#define PCC_BITFIELD_TYPE_MATTERS 1

/* Specify the size_t type.  */
#define SIZE_TYPE "unsigned int"

/* Standard register usage.  */

/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers. */
#define FIRST_PSEUDO_REGISTER 74

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator. */
#define FIXED_REGISTERS                                                         \
{                                                                               \
  /* D0.0/D1.0-D0.7/D1.7 */                                                     \
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,                               \
  /* D0.8/D1.8-D0.15/D1.15 currently reserved */                                \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                               \
  /* A0.0/A1.0-A0.7/A1.7 */                                                     \
  1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,                               \
  /* FRAME, CC, ARGP, RAPF, CPC0, CPC1, PC, TXRPT */                            \
     1,     1,  1,    1,    1,    1,    1,  1,                                  \
  /* FX */                                                                      \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                               \
  /* TTREC(L) */                                                                \
  1, 1                                                                          \
}

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you like.  */
#define CALL_USED_REGISTERS                                                     \
{                                                                               \
  /* D0.0/D1.0-D0.7/D1.7 */                                                     \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,                               \
  /* D0.8-D0.15/D1.8-D1.15 currently reserved */                                \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                               \
  /* A0.0/A1.0-A0.7/A1.7 */                                                     \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                               \
  /* FRAME, CC, ARGP, RAPF, CPC0, CPC1, PC, TXRPT */                            \
     1,     1,  1,    1,    1,    1,    1,  1,                                  \
  /* FX */                                                                      \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                               \
  /* TTREC(L) */                                                                \
  1, 1                                                                          \
}

/* Like `CALL_USED_REGISTERS' except this macro doesn't require that
   the entire set of `FIXED_REGISTERS' be included.
   (`CALL_USED_REGISTERS' must be a superset of `FIXED_REGISTERS').
   This macro is optional.  If not specified, it defaults to the value
   of `CALL_USED_REGISTERS'.  */

#define CALL_REALLY_USED_REGISTERS                                              \
{                                                                               \
  /* D0/D1 */                                                                   \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,                               \
  /* D0/D1 currently reserved */                                                \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                               \
  /* A0/A1 */                                                                   \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                               \
  /* FRAME, CC, ARGP, RAPF, CPC0, CPC1, PC, TXRPT */                            \
     1,     1,  1,    1,    1,    1,    1,  0,                                  \
  /* FX */                                                                      \
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                               \
  /* TTREC(L) */                                                                \
  1, 1                                                                          \
}

/* Make sure everything's fine if we *don't* have a given processor.
   This assumes that putting a register in fixed_regs will keep the
   compilers mitt's completely off it.  We don't bother to zero it out
   of register classes.  */
#define CONDITIONAL_REGISTER_USAGE                                              \
  do {                                                                          \
    static const int bases[]   = {16, 17, 40, 41, 0};                           \
    static const int strides[] = { 2,  2,  2,  2, 0};                           \
    static const int limits[]  = { 8,  8,  4,  4, 0};                           \
    const char *pmetagopt = metag_extreg_string;                                \
    int unit;                                                                   \
    long int fpuregs = strtol (metag_fpureg_string, NULL, 10);                  \
    bool extended_reg_enabled = false;                                          \
                                                                                \
    /* Minimum 3 fpu regs, maximum 16 */                                        \
    if (fpuregs != 0)                                                           \
      {                                                                         \
        /* The compiler may use FX registers so all sections are FPU*/          \
        metag_fpu_resources = 1;                                                \
        fpuregs = (fpuregs > 16) ? 16 : (fpuregs < 3) ? 3 : fpuregs;            \
      }                                                                         \
                                                                                \
    if (strlen (pmetagopt) != 0 && strlen (pmetagopt) != 4)                     \
      error ("-mextreg takes an argument of four digits");                      \
                                                                                \
    if (TARGET_METAC_1_1)                                                       \
      {                                                                         \
        /* Don't need temporary registers in AX unit */                         \
        fixed_regs[A0_3_REG] = 0;                                               \
        fixed_regs[A1_3_REG] = 0;                                               \
        /* Hence compact default/minimum register set to 8844 for v1.1 */       \
        fixed_regs[A0_4_REG] = 1;                                               \
        fixed_regs[A1_4_REG] = 1;                                               \
      }                                                                         \
                                                                                \
    /*  Enabled only the extended regs specified in the extreg string */        \
    for (unit = 0; *pmetagopt != 0 && limits[unit]; unit++)                     \
      {                                                                         \
        int add = (*pmetagopt++  - '0');                                        \
        int reg = bases[unit];                                                  \
                                                                                \
        if (add > limits[unit])                                                 \
          add = limits[unit];                                                   \
                                                                                \
        while (add-- > 0)                                                       \
          {                                                                     \
            if (TARGET_MTX)                                                     \
              error ("MTX does not support extended registers\n");              \
            extended_reg_enabled = true;                                        \
            fixed_regs[reg] = 0;                                                \
            reg += strides[unit];                                               \
          }                                                                     \
      }                                                                         \
                                                                                \
    if (extended_reg_enabled && metag_force_tbictxsave)                         \
      target_flags |= MASK_ECH;                                                 \
                                                                                \
    if (TARGET_ECH)                                                             \
      {                                                                         \
        if (fixed_regs[METAG_ECH_REGNUM] == 0)                                  \
          fixed_regs[METAG_ECH_REGNUM] = 1;                                     \
        else                                                                    \
          error ("-mtbictxsave cannot be used unless D0.8 is enabled via -mextreg\n" \
                 "Either use -mno-tbictxsave or enable D0.8");                  \
      }                                                                         \
                                                                                \
    for ( ; fpuregs > 0 ; fpuregs-- )                                           \
      fixed_regs[FIRST_FP_REG+fpuregs-1] = 0;                                   \
                                                                                \
    if (METAG_FLAG_PIC)                                                         \
      fixed_regs[PIC_OFFSET_TABLE_REGNUM] = 1 ;                                 \
  } while (0)

/* Determine which register classes are very likely used by spill registers.
   local-alloc.c won't allocate pseudos that have these classes as their
   preferred class unless they are "preferred or nothing".  */

#define CLASS_LIKELY_SPILLED_P(CLASS)                                           \
  (reg_class_size[(int) (CLASS)] == 1                                           \
   || (CLASS) == A0_REGS                                                        \
   || (CLASS) == A1_REGS                                                        \
   || (CLASS) == A_REGS                                                         \
   || (CLASS) == Ye_REGS                                                        \
   || (CLASS) == Yf_REGS                                                        \
   || (CLASS) == Yd_REGS                                                        \
   || (CLASS) == Yh_REGS                                                        \
   || (CLASS) == Yl_REGS                                                        \
   || (CLASS) == Ya_REGS                                                        \
   || (CLASS) == Yr_REGS)

/* The order in which registers should be allocated is defined so that the
   result registers are treated as the last scratch registers to be used
   after the argument registers are used in least likely used first order.
   Then we do the call-saved registers and address unit registers in numeric
   order - which is the default anyway. */
#define REG_ALLOC_ORDER                                                         \
{                                                                               \
  /* D0.4 happens to be a completely free scratch register */                   \
  D0_4_REG,                                                                     \
  /* If we have tons of free scratch data registers use them first */           \
  D0_8_REG,  D1_8_REG,  D0_9_REG,  D1_9_REG,  D0_10_REG, D1_10_REG,             \
  D0_11_REG, D1_11_REG, D0_12_REG, D1_12_REG, D0_13_REG, D1_13_REG,             \
  D0_14_REG, D1_14_REG, D0_15_REG, D1_15_REG,                                   \
  /* Then use the args and result registers in least-used first order */        \
  /* The D0.1 and D1.1 are however used for 12bit offsets so are      */        \
  /* towards the end */                                                         \
  D0_2_REG, D1_2_REG, D0_3_REG, D1_3_REG,                                       \
  D0_1_REG, D1_1_REG, D1_0_REG, D0_0_REG,                                       \
  /* Then use the call-saved registers */                                       \
  D0_5_REG, D1_5_REG, D0_6_REG, D1_6_REG, D0_7_REG, D1_7_REG,                   \
  /* Then use the address unit scratch registers */                             \
  A0_2_REG, A1_2_REG, A0_3_REG, A1_3_REG, A0_4_REG, A1_4_REG,                   \
  A0_5_REG, A1_5_REG, A0_6_REG, A1_6_REG, A0_7_REG, A1_7_REG,                   \
  /* FX - disuade use of FCC_REGS class */                                      \
  FX_0_REG , FX_1_REG , FX_2_REG , FX_3_REG,                                    \
  FX_4_REG , FX_5_REG , FX_6_REG , FX_7_REG,                                    \
  FX_8_REG , FX_9_REG , FX_10_REG, FX_11_REG,                                   \
  FX_12_REG, FX_13_REG, FX_14_REG, FX_15_REG,                                   \
  TXRPT_REG, TTREC_REG, TTRECL_REG,                                             \
  /* The remainder can never be allocated by the compiler anyway */             \
  D1_4_REG, A0_0_REG, A1_0_REG, A0_1_REG, A1_1_REG,                             \
  FRAME_REG, CC_REG, ARGP_REG, RAPF_REG, CPC0_REG, CPC1_REG, PC_REG             \
}

/* Specify the registers used for certain standard purposes.
   The values of these macros are register numbers.  */

/* Register used for the program counter  */
#define PC_REGNUM                 PC_REG

/* Logical base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM        ARGP_REG

/* Condition flag register */
#define MCC_REGNUM                CC_REG

/* Extended context support register */
#define METAG_ECH_REGNUM          D0_8_REG

/* Logical base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM      FRAME_REG

/* Real frame pointer register */
#define HARD_FRAME_POINTER_REGNUM A0FrP_REG

/* First and last register that accepts function arguments, D1.3 - D0.1 */
#define MIN_METAG_PARM_REGNUM     D0Ar6_REG    /* Actually contains last arg! */
#define MAX_METAG_PARM_REGNUM     D1Ar1_REG    /* Actually contains first arg! */

/* The number of registers used for parameter passing.  Local to this file.  */
#define MAX_METAG_PARM_REGS  (1 + (MAX_METAG_PARM_REGNUM - MIN_METAG_PARM_REGNUM))
#define MAX_METAG_PARM_BYTES (MAX_METAG_PARM_REGS * UNITS_PER_WORD)

/* D0.4 is used temporarily to save/restore A0FrP */
#define TEMP_D0FRT_REGNUM         D0FrT_REG

/* First and last register that is call-saved, D0.5 - D1.7 */
#define MIN_METAG_CSAVE_REGNUM    D0_5_REG
#define MAX_METAG_CSAVE_REGNUM    D1_7_REG

/* Register to use for call/return addresses D1RtP */
#define RETURN_POINTER_REGNUM     D1RtP_REG

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM      A0StP_REG

/* Register in which static-chain is passed to a function.  */
#define GLOBAL_POINTER_REGNUM     A1GbP_REG
#define STATIC_CHAIN_REGNUM       D0Re0_REG

/* Some temporaries are currently left for internal library/config use */
#define A0_SCRATCH                (!TARGET_METAC_1_1 ? A0_3_REG : INVALID_REGNUM)
#define A1_SCRATCH                (!TARGET_METAC_1_1 ? A1_3_REG : INVALID_REGNUM)

/* Structure value address is passed is 'hidden' parameter */
#define STRUCT_VALUE              0

#define RAPF_REGNUM               RAPF_REG

#define CPC0_REGNUM               CPC0_REG
#define CPC1_REGNUM               CPC1_REG

#define TXRPT_REGNUM              TXRPT_REG
#define TTREC_REGNUM              TTREC_REG

#define DECREMENT_AND_BRANCH_REG(MODE) gen_rtx_REG (MODE, TXRPT_REGNUM)

#define TABLEJUMP_USES_DBRA_REG    0

/* Value should be nonzero if functions must have frame pointers.
   Zero means the frame pointer need not be set up (and parms
   may be accessed via the stack pointer) in functions that seem suitable.
   This is computed in `reload', in reload1.c.  */
#define FRAME_POINTER_REQUIRED                                                  \
  metag_frame_pointer_required ()

#define SETUP_FRAME_ADDRESSES()                                                 \
  metag_setup_frame_addresses ()

/* Definitions for register eliminations.

   This is an array of structures.  Each structure initializes one pair
   of eliminable registers.  The "from" register number is given first,
   followed by "to".  Eliminations of the same "from" register are listed
   in order of preference.

   We have two registers that MUST be eliminated FRAME_POINTER and
   ARG_POINTER. ARG_POINTER is ALWAYS eliminated to either STACK_POINTER_REGNUM
   or HARD_FRAME_POINTER_REGNUM. FRAME_POINTER is ALWAYS eliminated to either
   STACK_POINTER_REGNUM or HARD_FRAME_POINTER_REGNUM.

   STACK_POINTER_REGUM is the preferred elimination. */

#define ELIMINABLE_REGS                                                         \
{                                                                               \
  {ARG_POINTER_REGNUM,   STACK_POINTER_REGNUM},                                 \
  {ARG_POINTER_REGNUM,   HARD_FRAME_POINTER_REGNUM},                            \
  {FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM},                                 \
  {FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM}                             \
}

/* Given FROM and TO register numbers, say whether this elimination is allowed.
   Frame pointer elimination is automatically handled.

   Only eliminate down to the HARD_FRAME_POINTER if it's available. */
#define CAN_ELIMINATE(FROM, TO)                                                 \
  (((TO) == STACK_POINTER_REGNUM && frame_pointer_needed) ? 0 : 1)

/* Define the offset between two registers, one to be eliminated,
   and the other its replacement, at the start of a routine.
 */

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)                            \
  ((OFFSET) = metag_initial_elimination_offset (FROM, TO))

/* Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.
   This is ordinarily the length in words of a value of mode MODE
   but can be less for certain modes in special long registers. */
#define HARD_REGNO_NREGS(REGNO, MODE)                                           \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Value is 1 if hard register REGNO can hold a value of machine-mode MODE. */
#define HARD_REGNO_MODE_OK(REGNO, MODE)                                         \
  metag_hard_regno_mode_ok (REGNO, MODE)

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */
#define MODES_TIEABLE_P(MODE1, MODE2)                                           \
  (GET_MODE_CLASS (MODE1) == GET_MODE_CLASS (MODE2))

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
{
  NO_REGS,
  Wx_REGS,
  WQh_REGS,
  WQl_REGS,
  Ye_REGS,
  Yf_REGS,
  Yd_REGS,
  Yh_REGS,
  Yl_REGS,
  Ya_REGS,
  Yr_REGS,
  D0_REGS,
  D1_REGS,
  D_REGS,
  A0_REGS,
  A1_REGS,
  A_REGS,
  DA_REGS,
  Be_REGS,
  Bf_REGS,
  Bd_REGS,
  Bh_REGS,
  Bl_REGS,
  Ba_REGS,
  Br_REGS,
  nD0_REGS,
  nD1_REGS,
  nA0_REGS,
  nA1_REGS,
  nBU_REGS,
  nYe_REGS,
  nYf_REGS,
  nYd_REGS,
  nYh_REGS,
  nYl_REGS,
  nYa_REGS,
  nYr_REGS,
  GENERAL_REGS,
  FPP_REGS,
  FPC_REGS,
  cD0_REGS,
  cD1_REGS,
  cD_REGS,
  cA0_REGS,
  cA1_REGS,
  cA_REGS,
  cnD0_REGS,
  cnD1_REGS,
  cnA0_REGS,
  cnA1_REGS,
  cDA_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};

#define N_REG_CLASSES ((int) LIM_REG_CLASSES)

/* Give names of register classes as strings for dump file.   */

#define REG_CLASS_NAMES                                                         \
{                                                                               \
  "NO_REGS",                                                                    \
  "Wx_REGS",                                                                    \
  "WQh_REGS",                                                                   \
  "WQl_REGS",                                                                   \
  "Ye_REGS",                                                                    \
  "Yf_REGS",                                                                    \
  "Yd_REGS",                                                                    \
  "Yh_REGS",                                                                    \
  "Yl_REGS",                                                                    \
  "Ya_REGS",                                                                    \
  "Yr_REGS",                                                                    \
  "D0_REGS",                                                                    \
  "D1_REGS",                                                                    \
  "D_REGS",                                                                     \
  "A0_REGS",                                                                    \
  "A1_REGS",                                                                    \
  "A_REGS",                                                                     \
  "DA_REGS",                                                                    \
  "Be_REGS",                                                                    \
  "Bf_REGS",                                                                    \
  "Bd_REGS",                                                                    \
  "Bh_REGS",                                                                    \
  "Bl_REGS",                                                                    \
  "Ba_REGS",                                                                    \
  "Br_REGS",                                                                    \
  "nD0_REGS",                                                                   \
  "nD1_REGS",                                                                   \
  "nA0_REGS",                                                                   \
  "nA1_REGS",                                                                   \
  "nBU_REGS",                                                                   \
  "nYe_REGS",                                                                   \
  "nYf_REGS",                                                                   \
  "nYd_REGS",                                                                   \
  "nYh_REGS",                                                                   \
  "nYl_REGS",                                                                   \
  "nYa_REGS",                                                                   \
  "nYr_REGS",                                                                   \
  "GENERAL_REGS",                                                               \
  "FPP_REGS",                                                                   \
  "FPC_REGS",                                                                   \
  "cD0_REGS",                                                                   \
  "cD1_REGS",                                                                   \
  "cD_REGS",                                                                    \
  "cA0_REGS",                                                                   \
  "cA1_REGS",                                                                   \
  "cA_REGS",                                                                    \
  "cnD0_REGS",                                                                  \
  "cnD1_REGS",                                                                  \
  "cnA0_REGS",                                                                  \
  "cnA1_REGS",                                                                  \
  "cDA_REGS",                                                                   \
  "ALL_REGS"                                                                    \
}

/* Define which registers fit in which classes.
   This is an initializer for a vector of HARD_REG_SET
   of length N_REG_CLASSES.  */

#define REG_CLASS_CONTENTS                                                      \
{                                                                               \
  { 0x00000000, 0x00000000, 0x00000000 }, /* NO_REGS  */                        \
  { 0x00000000, 0x00800000, 0x00000300 }, /* Wx_REGS  */                        \
  { 0x00000000, 0x00000050, 0x00000000 }, /* WQh_REGS  */                       \
  { 0x00000000, 0x000000a0, 0x00000000 }, /* WQl_REGS  */                       \
  { 0x00000005, 0x00000000, 0x00000000 }, /* Ye_REGS  */                        \
  { 0x0000000a, 0x00000000, 0x00000000 }, /* Yf_REGS  */                        \
  { 0x0000000f, 0x00000000, 0x00000000 }, /* Yd_REGS  */                        \
  { 0x00000000, 0x00000005, 0x00000000 }, /* Yh_REGS  */                        \
  { 0x00000000, 0x0000000a, 0x00000000 }, /* Yl_REGS  */                        \
  { 0x00000000, 0x0000000f, 0x00000000 }, /* Ya_REGS  */                        \
  { 0x0000000f, 0x0000000f, 0x00000000 }, /* Yr_REGS  */                        \
  { 0x55555555, 0x00000000, 0x00000000 }, /* D0_REGS  */                        \
  { 0xaaaaaaaa, 0x00000000, 0x00000000 }, /* D1_REGS  */                        \
  { 0xffffffff, 0x00000000, 0x00000000 }, /* D_REGS   */                        \
  { 0x00000000, 0x00005555, 0x00000000 }, /* A0_REGS  */                        \
  { 0x00000000, 0x0000aaaa, 0x00000000 }, /* A1_REGS  */                        \
  { 0x00000000, 0x0000ffff, 0x00000000 }, /* A_REGS   */                        \
  { 0xffffffff, 0x0000ffff, 0x00000000 }, /* DA_REGS  */                        \
  { 0x5555ffff, 0x0000ffff, 0x00000000 }, /* Be_REGS  */                        \
  { 0xaaaaffff, 0x0000ffff, 0x00000000 }, /* Bf_REGS  */                        \
  { 0x0000ffff, 0x0000ffff, 0x00000000 }, /* Bd_REGS  */                        \
  { 0x0000ffff, 0x0000ffff, 0x00000000 }, /* Bh_REGS  */                        \
  { 0x0000ffff, 0x0000ffff, 0x00000000 }, /* Bl_REGS  */                        \
  { 0x0000ffff, 0x0000ffff, 0x00000000 }, /* Ba_REGS  */                        \
  { 0x0000ffff, 0x0000ffff, 0x00000000 }, /* Br_REGS  */                        \
  { 0xaaaaaaaa, 0x0000ffff, 0x00000000 }, /* nD0_REGS */                        \
  { 0x55555555, 0x0000ffff, 0x00000000 }, /* nD1_REGS */                        \
  { 0xffffffff, 0x0000aaaa, 0x00000000 }, /* nA0_REGS */                        \
  { 0xffffffff, 0x00005555, 0x00000000 }, /* nA1_REGS */                        \
  { 0xffff0000, 0x00000000, 0x00000000 }, /* nBU_REGS */                        \
  { 0xfffffffa, 0x0000ffff, 0x00000000 }, /* nYe_REGS */                        \
  { 0xfffffff5, 0x0000ffff, 0x00000000 }, /* nYf_REGS */                        \
  { 0xfffffff0, 0x0000ffff, 0x00000000 }, /* nYd_REGS */                        \
  { 0xffffffff, 0x0000fffa, 0x00000000 }, /* nYh_REGS */                        \
  { 0xffffffff, 0x0000fff5, 0x00000000 }, /* nYl_REGS */                        \
  { 0xffffffff, 0x0000fff0, 0x00000000 }, /* nYa_REGS */                        \
  { 0xfffffff0, 0x0000fff0, 0x00000000 }, /* nYr_REGS */                        \
  { 0xffffffff, 0x0000ffff, 0x00000000 }, /* GENERAL_REGS */                    \
  { 0x00000000, 0x55000000, 0x00000055 }, /* FPP_REGS */                        \
  { 0x00000000, 0xff000000, 0x000000ff }, /* FPC_REGS */                        \
  { 0x55555555, 0xff000000, 0x000000ff }, /* cD0_REGS  */                       \
  { 0xaaaaaaaa, 0xff000000, 0x000000ff }, /* cD1_REGS  */                       \
  { 0xffffffff, 0xff000000, 0x000000ff }, /* cD_REGS   */                       \
  { 0x00000000, 0xff005555, 0x000000ff }, /* cA0_REGS  */                       \
  { 0x00000000, 0xff00aaaa, 0x000000ff }, /* cA1_REGS  */                       \
  { 0x00000000, 0xff00ffff, 0x000000ff }, /* cA_REGS   */                       \
  { 0xaaaaaaaa, 0xff00ffff, 0x000000ff }, /* cnD0_REGS */                       \
  { 0x55555555, 0xff00ffff, 0x000000ff }, /* cnD1_REGS */                       \
  { 0xffffffff, 0xff00aaaa, 0x000000ff }, /* cnA0_REGS */                       \
  { 0xffffffff, 0xff005555, 0x000000ff }, /* cnA1_REGS */                       \
  { 0xffffffff, 0xff00ffff, 0x000000ff }, /* cDA_REGS  */                       \
  { 0xffffffff, 0xff80ffff, 0x000003ff }  /* ALL_REGS */                        \
}

/* The same information, inverted:
   Return the class number of the smallest class containing
   reg number REGNO.  This could be a conditional expression
   or could index an array.  */

#define REGNO_REG_CLASS(REGNO)                                                  \
  metag_regno_reg_class_minimal (REGNO)

#define METAG_REGNO_REG_CLASS(REGNO)                                            \
  metag_regno_reg_class_unit (REGNO)

#define METAG_REGNO_SAME_UNIT(REGNUM1, REGNUM2)                                 \
  metag_regno_same_unit_p (REGNUM1, REGNUM2)

/* The class value for index registers, and the one for base regs.  */
#define INDEX_REG_CLASS NO_REGS
#define BASE_REG_CLASS  GENERAL_REGS

#define IN_RANGE_P(VALUE, LOW, HIGH)                                            \
  ((LOW) <= (VALUE) && (VALUE) <= (HIGH))

#define METAG_CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C)  0

/* Define the cost of moving between registers of various classes. */
#define REGISTER_MOVE_COST(MODE, CLASS1, CLASS2)                                \
  ((int)(CLASS1) == (int)(CLASS2) ? 1 : 4 )

/* A C expressions returning the cost of moving data of MODE from a register to
   or from memory. Keep it higher than max register/register costs  */
#define MEMORY_MOVE_COST(MODE, CLASS, IN) 8

/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
   In general this is just CLASS; but on some machines
   in some cases it is preferable to use a more restrictive class. */
#define PREFERRED_RELOAD_CLASS(X, CLASS) (CLASS)

#define SECONDARY_INPUT_RELOAD_CLASS(CLASS, MODE, X)                            \
  metag_secondary_reload_class (CLASS, MODE, X, true)

#define SECONDARY_OUTPUT_RELOAD_CLASS(CLASS, MODE, X)                           \
  metag_secondary_reload_class (CLASS, MODE, X, false)

/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.  */
#define CLASS_MAX_NREGS(CLASS, MODE)                                            \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Stack layout; function entry, exit and calling.  */

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */
/*#define STACK_GROWS_DOWNWARD*/

/* Define this if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
/* #define FRAME_GROWS_DOWNWARD */

#define ARGS_GROW_DOWNWARD

/* We use post increment on metag because of 64-bit vs 32-bit alignments */
#define STACK_PUSH_CODE POST_INC

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */
#define STARTING_FRAME_OFFSET   0

/* This points to the location of dynamically allocated memory on the stack
   immediately after the stack pointer has been adjusted by the amount of
   memory desired.  */
#define STACK_DYNAMIC_OFFSET(FNDECL)                                            \
  (-(current_function_outgoing_args_size + (STACK_POINTER_OFFSET)))

/* If we generate an insn to push BYTES bytes,
   this says how many the stack pointer really advances by.
   (incompatible with ACCUMULATE_OUTGOING_ARGS)
#define PUSH_ROUNDING(BYTES) (((BYTES) + 3) & ~3) */

/* If nonzero, the maximum amount of space required for outgoing arguments will be
 computed and placed into the variable current_function_outgoing_args_size. No space
 will be pushed onto the stack for each call; instead, the function prologue should
 increase the stack frame size by this amount. Setting both PUSH_ARGS and 
 ACCUMULATE_OUTGOING_ARGS is not proper. 
*/
#define ACCUMULATE_OUTGOING_ARGS 1

/* Offset of first parameter from the argument pointer register value.  */
#define FIRST_PARM_OFFSET(FNDECL)                                               \
  metag_first_parm_offset (FNDECL)

/* A C expression whose value is RTL representing the value of the return
   address for the frame COUNT steps up from the current frame.  */

#define RETURN_ADDR_RTX(COUNT, FRAME)                                           \
  metag_return_addr_rtx (COUNT, FRAME)

/* Value is 1 if returning from a function call automatically
   pops the arguments described by the number-of-args field in the call.
   FUNDECL is the declaration node of the function (as a tree),
   FUNTYPE is the data type of the function (as a tree),
   or for a library call it is an identifier node for the subroutine name. */

#define RETURN_POPS_ARGS(FUNDECL, FUNTYPE, SIZE) (0)

/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */

/* On the metag the return value is in D0.0/D1.0 regardless.  */

#define LIBCALL_VALUE(MODE) metag_libcall_value (MODE)

/* 1 if N is a possible register number for a function value.
   On the metag, D1.0/D1.1 is the only register thus used.  */

#define FUNCTION_VALUE_REGNO_P(N) ((N) == 0)

/* A C expression which can inhibit the returning of certain function values
   in registers, based on the type of value. A nonzero value says to return
   the function value in memory, just as large structures are always
   returned. Here type will be a C expression of type tree, representing the
   data type of the value. We target all 64-bit or less structures as return
   in registers. */
#define RETURN_IN_MEMORY(TYPE) metag_return_in_memory (TYPE)

/* Define this macro to be 1 if all structure and union return values must be
   in memory. We want 64-bit structs to return in registers under the control
   of the RETURN_IN_MEMORY macro. */
#define DEFAULT_PCC_STRUCT_RETURN 0

/* 1 if N is a possible register number for function argument passing. */
#define FUNCTION_ARG_REGNO_P(N)                                                 \
  (MIN_METAG_PARM_REGNUM <= (N) && (N) <= MAX_METAG_PARM_REGNUM)

/* Define a data type for recording info about an argument list
   during the scan of that argument list.  This data type should
   hold all necessary information about the function itself
   and about the args processed so far, enough to enable macros
   such as FUNCTION_ARG to determine where the next arg should go.

   On the metag, this is a single integer, which is a number of bytes
   of arguments scanned so far.  */
struct cumulative_args
{
  int narg;
  int partial;
};

#define CUMULATIVE_ARGS struct cumulative_args

/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.  */
#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS)        \
  ((CUM).narg = 0, (CUM).partial = 0)

/* Round VALUE up to next multiple of SIZE (Assumes SIZE is 2^N for some N  */
#define ROUND_TO_SIZE(VALUE, SIZE)                                              \
  (((VALUE) + (SIZE) - 1) & ~((SIZE) - 1))

/* Round VALUE up to a double word boundary */
#define ROUND_TO_DWORD(VALUE)                                                   \
  ROUND_TO_SIZE (VALUE, 2 * UNITS_PER_WORD)

/* Round VALUE up to a word boundary */
#define ROUND_TO_WORD(VALUE)                                                    \
  ROUND_TO_SIZE (VALUE, UNITS_PER_WORD)

/* Round VALUE up to a word boundary */
#define ROUND_ADVANCE(VALUE)                                                    \
  ROUND_TO_WORD (VALUE)

/* Argument size rounded up to word size.  */
#define METAG_ARG_SIZE(MODE, TYPE)                                              \
  ((MODE) == BLKmode                                                            \
   ? ROUND_TO_WORD (int_size_in_bytes (TYPE))                                   \
   : ROUND_TO_WORD (GET_MODE_SIZE (MODE)))

/* Round arg MODE/TYPE up to the next word boundary.    */
#define ROUND_ADVANCE_ARG(MODE, TYPE)                                           \
  METAG_ARG_SIZE (MODE, TYPE)

/* Round CUM up to the necessary point for argument MODE/TYPE.  */
#define ROUND_ADVANCE_CUM(CUM, MODE, TYPE)                                      \
  (METAG_ARG_SIZE (MODE, TYPE) > UNITS_PER_WORD                                 \
   ? (ROUND_TO_DWORD ((CUM) + METAG_ARG_SIZE (MODE, TYPE))                      \
      - METAG_ARG_SIZE (MODE, TYPE))                                            \
   : (CUM))

/* Offset base register by one for 64-bit atomic values and the whole size of
   struct/union parameters */
#define ROUND_BASEREG_NUM(MODE, TYPE)                                           \
  ((ROUND_ADVANCE_ARG (MODE, TYPE) / UNITS_PER_WORD) - 1)

/* Update the data in CUM to advance over an argument of mode MODE and data
   type TYPE. TYPE is null for libcalls where that information may not be
   available. */
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)                            \
  metag_function_arg_advance (&(CUM), MODE, TYPE, NAMED)

/* Calculate register to place argument.  align correctly for 64-bit data */
#define CALCULATE_REG(BASE_REG, CUM, MODE, TYPE)                                \
  ((BASE_REG) - ((ROUND_ADVANCE_CUM (CUM, MODE, TYPE) / UNITS_PER_WORD)         \
                 + ROUND_BASEREG_NUM (MODE, TYPE)))

/* Define where to put the arguments to a function.
   Value is zero to push the argument on the stack,
   or a hard register in which to store the argument.

   MODE is the argument's machine mode.
   TYPE is the data type of the argument (as a tree).
    This is null for libcalls where that information may
    not be available.
   CUM is a variable of type CUMULATIVE_ARGS which gives info about
    the preceding args and about the function being called.
   NAMED is nonzero if this argument is a named parameter
    (otherwise it is an extra parameter matching an ellipsis).  */

#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED)                                    \
  metag_function_arg (&(CUM), MODE, TYPE, NAMED)

/* Defined if some argument types need more than PARM_BOUNDARY alignment */
#define FUNCTION_ARG_BOUNDARY(MODE, TYPE)                                       \
  metag_function_arg_boundary (MODE, TYPE)

/* Perform any actions needed for a function that is receiving a variable number
 of arguments.  CUM is as above.  MODE and TYPE are the mode and type of the
 current parameter.  PRETEND_SIZE is a variable that should be set to the amount
 of stack that must be pushed by the prologue to pretend that our caller pushed 
 it. 
 
 Normally, this macro will push all remaining incoming registers on the stack
 and set PRETEND_SIZE to the length of the registers pushed.
 
 On Metag, PRETEND_SIZE is set in order to have the prologue push the last
 named argument and all anonymous arguments onto the stack .*/

/* Don't output profile counters. */
#define NO_PROFILE_COUNTERS     1

/* Output assembler code to FILE to increment profiler label # LABELNO
   for profiling a function entry.  */

#define FUNCTION_PROFILER(FILE, LABELNO)                                        \
  do {                                                                          \
    if (!TARGET_HWTRACE)                                                        \
      metag_function_profiler (FILE);                                           \
  } while (0)

/* Output assembler code to FILE to initialize this source file's
   basic block profiling info, if that has not already been done.  */

#define FIXME_FUNCTION_BLOCK_PROFILER(FILE, LABELNO)                            \
  fprintf (FILE, ASM_COMMENT_START " block profile code %u\n", (LABELNO))

/* Output assembler code to FILE to increment the entry-count for
   the BLOCKNO'th basic block in this source file.  */

#define FIXME_BLOCK_PROFILER(FILE, BLOCKNO)                                     \
  fprintf (FILE, ASM_COMMENT_START " profile code %u\n", 4 * (BLOCKNO))

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.  */
#define EXIT_IGNORE_STACK 1

/* Determine if the epilogue should be output as RTL.
   You should override this if you define FUNCTION_EXTRA_EPILOGUE.  */
#define METAG_CHEAP_RETURN      metag_cheap_return

#if 1
#define TRAMPOLINE_SECTION      text_section

/* Output assembler code for a block containing the constant parts
   of a trampoline, leaving space for the variable parts.  */

/* On the metag, the trampoline contains 4 instructions + 2 data words:
     MOV        TEMP_D0FRT_REGNUM, PC
     GETD       STATIC_CHAIN_REGNUM, [TEMP_D0FRT_REGNUM + #(16 - 4)]
     GETD       TEMP_D0FRT_REGNUM, [TEMP_D0FRT_REGNUM + #(20 - 4)]
     MOV        PC, TEMP_D0FRT_REGNUM
     .long      <static-chain>
     .long      <function-address>
 */

/* Define offsets from start of Trampoline for the dynamic static
   chain and function address data words.  */
#define TRAMP_SC_OFFSET    16   /* Static chain offset.  */
#define TRAMP_FN_OFFSET    20   /* Function address offset.  */

#define TRAMPOLINE_TEMPLATE(FILE)                                               \
do {                                                                            \
  const char * const scratch = reg_names[TEMP_D0FRT_REGNUM];                    \
  const char * const chain   = reg_names[STATIC_CHAIN_REGNUM];                  \
                                                                                \
  fprintf (FILE, "\tMOV\t%s, PC\n", scratch);                                   \
  fprintf (FILE, "\tGETD\t%s, [%s + #%d]\n",                                    \
           chain, scratch, TRAMP_SC_OFFSET - 4);                                \
  fprintf (FILE, "\tGETD\t%s, [%s + #%d]\n",                                    \
           scratch, scratch, TRAMP_FN_OFFSET - 4);                              \
  fprintf (FILE, "\tMOV\tPC, %s\n", scratch);                                   \
  fputs (targetm.asm_out.aligned_op.si, FILE);                                  \
  fputs ("\t0\n", FILE);                                                        \
  fputs (targetm.asm_out.aligned_op.si, FILE);                                  \
  fputs ("\t0\n", FILE);                                                        \
} while (0)

/* Length in units of the trampoline for entering a nested function.  */

#define TRAMPOLINE_SIZE (UNITS_PER_WORD * 6)

/* Emit RTL insns to initialize the variable parts of a trampoline.
   FNADDR is an RTX for the address of the function's pure code.
   CXT is an RTX for the static chain value for the function.  */

#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT)                              \
do {                                                                           \
  if (!TARGET_MTX)                                                             \
    {                                                                          \
      if (!TARGET_MINIM)                                                       \
        {                                                                      \
          emit_move_insn (gen_rtx_MEM (SImode,                                 \
                                   plus_constant (TRAMP, TRAMP_SC_OFFSET)),    \
                          CXT);                                                \
          emit_move_insn (gen_rtx_MEM (SImode,                                 \
                                   plus_constant (TRAMP, TRAMP_FN_OFFSET)),    \
                          FNADDR);                                             \
        }                                                                      \
      else                                                                     \
        error ("GNU C nested C function extension not supported for MiniM.\n");\
    }                                                                          \
  else                                                                         \
    error ("GNU C nested C function extension not supported.\n");              \
} while (0)

#endif

  
/* Initialize data used by insn expanders.  This is called from insn_emit,
   once for every function before code is generated.  */

#define INIT_EXPANDERS  metag_init_expanders ()

/* Addressing modes, and classification of registers for them.  */

#define HAVE_PRE_INCREMENT      1
#define HAVE_POST_INCREMENT     1
#define HAVE_PRE_DECREMENT      1
#define HAVE_POST_DECREMENT     1

#define HAVE_PRE_MODIFY_REG     1
#define HAVE_POST_MODIFY_REG    1
#define HAVE_PRE_MODIFY_DISP    1
#define HAVE_POST_MODIFY_DISP   1

/* Macros to check register numbers against specific register classes.  */

/* These assume that REGNO is a hard or pseudo reg number.
   They give nonzero only if REGNO is a hard reg of the suitable class
   or a pseudo reg currently allocated to a suitable hard reg.
   Since they use reg_renumber, they are safe only once reg_renumber
   has been allocated, which happens in local-alloc.c.  */

#define REGNO_OK_FOR_INDEX_P(REGNO)                                             \
   FALSE /* REGNO_OK_FOR_BASE_P (REGNO) */

#define TEST_REGNO(R, OP, VALUE)                                                \
  (((unsigned)(R) OP (VALUE)) || (unsigned)reg_renumber[R] OP (VALUE))

#define REGNO_OK_FOR_BASE_P(REGNO)                                              \
  (TEST_REGNO (REGNO, <=, FRAME_POINTER_REGNUM)                                 \
   || TEST_REGNO (REGNO, ==, ARG_POINTER_REGNUM))

/* Maximum number of registers that can appear in a valid memory address.  */
#define MAX_REGS_PER_ADDRESS 2

/* Recognize any constant value that is a valid address.  */

#define CONSTANT_ADDRESS_P(X)                                                   \
  (LABEL_REF_P (X)                                                              \
   || SYMBOL_REF_P (X)                                                          \
   || CONST_INT_P (X)                                                           \
   || GET_CODE (X) == CONST)

/* Nonzero if the constant value X is a legitimate general operand.
   It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE.  

   TODO: This will need to be changed (see definition in metag-linux.h)
   when implementing TLS for the embedded toolchain.
 */

#define LEGITIMATE_CONSTANT_P(X) 1

/* The macros REG_OK_FOR..._P assume that the arg is a REG rtx
   and check its validity for a certain class.
   We have two alternate definitions for each of them.
   The usual definition accepts all pseudo regs; the other rejects
   them unless they have been allocated suitable hard regs.
   The symbol REG_OK_STRICT causes the latter definition to be used.

   Most source files want to accept pseudo regs in the hope that
   they will get allocated to the class that the insn wants them to be in.
   Source files for reload pass need to be strict.
   After reload, it makes no difference, since pseudo regs have
   been eliminated by then.  */

#ifndef REG_OK_STRICT
#define REG_OK_STRICT_FLAG FALSE
#else
#define REG_OK_STRICT_FLAG TRUE
#endif

/* --------------------------------  BEGIN NONSTRICT ------------------------ */

#define NONSTRICT_REGNO_OK_P(R)                                                 \
  METAG_LEGITIMATE_REGNO_P (R, false)

#define NONSTRICT_REG_OK_P(R)                                                   \
  METAG_LEGITIMATE_REG_P (R, false)

/* Nonzero if X is a hard reg that can be used as an index
   or if it is a pseudo reg.  */
#define NONSTRICT_REG_OK_FOR_INDEX_P(X)                                         \
  FALSE /* NONSTRICT_REGNO_OK_P (REGNO (X)) */

/* Nonzero if X is a hard reg that can be used as a base reg
   or if it is a pseudo reg.  */
#define NONSTRICT_REG_OK_FOR_BASE_P(X)                                          \
  FALSE /* NONSTRICT_REGNO_OK_P (REGNO (X)) */

/* Nonzero if X is a hard reg that can be used as a base reg
   or if it is a pseudo reg.  */
#define NONSTRICT_REG_OK_FOR_OFFSET_P(X)                                        \
  NONSTRICT_REG_OK_P (X)

/* Nonzero if the pair of hard regs are okay to use as base + offset
   or if either is a psuedo reg. */
#define NONSTRICT_REGS_OK_FOR_BASE_OFFSET_P(X, Y)                               \
  METAG_REGS_OK_FOR_BASE_OFFSET_P (X, Y, false)

/* ----------------------------------  END NONSTRICT ------------------------ */

/* -----------------------------------  BEGIN STRICT ------------------------ */

#define STRICT_REGNO_OK_P(R)                                                    \
  METAG_LEGITIMATE_REGNO_P (R, true)

#define STRICT_REG_OK_P(R)                                                      \
  METAG_LEGITIMATE_REG_P (R, false)

/* Nonzero if X is a hard reg that can be used as an index.  */
#define STRICT_REG_OK_FOR_INDEX_P(X)                                            \
  FALSE /* STRICT_REGNO_OK_P (REGNO (X)) */

/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define STRICT_REG_OK_FOR_BASE_P(X)                                             \
  FALSE /* STRICT_REGNO_OK_P (REGNO (X)) */

/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define STRICT_REG_OK_FOR_OFFSET_P(X)                                           \
  STRICT_REG_OK_P (X)

/* Nonzero if the pair of hard regs is okay to use as base + offset */
#define STRICT_REGS_OK_FOR_BASE_OFFSET_P(X, Y)                                  \
  METAG_REGS_OK_FOR_BASE_OFFSET_P (X, Y, true)

/* ------------------------------------ END STRICT ------------------------- */

/* Nonzero if X is a hard reg that can be used as an index
   or if it is a pseudo reg.  */
#define REG_OK_FOR_INDEX_P(X)                                                   \
  METAG_REG_OK_FOR_INDEX_P (X, REG_OK_STRICT_FLAG)

#define METAG_REG_OK_FOR_INDEX_P(X, STRICT)                                     \
  metag_reg_ok_for_index_p (X, STRICT)

/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define REG_OK_FOR_BASE_P(X)                                                    \
  METAG_REG_OK_FOR_BASE_P (X, REG_OK_STRICT_FLAG)

#define METAG_REG_OK_FOR_BASE_P(X, STRICT)                                      \
  metag_reg_ok_for_base_p (X, STRICT)

/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define REG_OK_FOR_OFFSET_P(X)                                                  \
  METAG_REG_OK_FOR_OFFSET_P (X, REG_OK_STRICT_FLAG)

#define METAG_REG_OK_FOR_OFFSET_P(X, STRICT)                                    \
  metag_reg_ok_for_offset_p (X, STRICT)

/* Nonzero if the pair of hard regs is okay to use as base + offset */
#define METAG_REGS_OK_FOR_BASE_OFFSET_P(X, Y, STRICT)                           \
  metag_regs_ok_for_base_offset_p (X, Y, STRICT)

/* GO_IF_LEGITIMATE_ADDRESS recognizes an RTL expression
   that is a valid memory address for an instruction.
   The MODE argument is the machine mode for the MEM expression
   that wants to use this address. */

#define METAG_LEGITIMATE_REGNO_P(REGNUM, STRICT)                                \
  metag_legitimate_regno_p (REGNUM, STRICT)

#define METAG_LEGITIMATE_REG_P(REG, STRICT)                                     \
  metag_legitimate_reg_p (REG, STRICT)

/* PRE_MODIFY */
#define METAG_LEGITIMATE_PRE_MODIFY_P(ADDR, MODE, STRICT)                       \
  (GET_CODE (ADDR) == PRE_MODIFY                                                \
   && metag_legitimate_modify_p (ADDR, MODE, STRICT))

/* POST_MODIFY */
#define METAG_LEGITIMATE_POST_MODIFY_P(ADDR, MODE, STRICT)                      \
  (GET_CODE (ADDR) == POST_MODIFY                                               \
   && metag_legitimate_modify_p (ADDR, MODE, STRICT))

/* PRE_INC/PRE_DEC supportable */
#define METAG_LEGITIMATE_PRE_INCDEC_P(ADDR, MODE, STRICT)                       \
  metag_legitimate_pre_incdec_p (ADDR, MODE, STRICT)

/* POST_INC/POST_DEC supportable */
#define METAG_LEGITIMATE_POST_INCDEC_P(ADDR, MODE, STRICT)                      \
  metag_legitimate_post_incdec_p (ADDR, MODE, STRICT)

/* Two ranges of offset are supported dependant on base & mode */
#define METAG_LEGITIMATE_OFF_P(BASE, OFF, MODE, STRICT)                         \
  metag_legitimate_off_p (BASE, OFF, MODE, STRICT)

/* [ Rb + Ro ] */
#define METAG_LEGITIMATE_TWIN_P(BASE, OFF, MODE, STRICT)                        \
  metag_legitimate_twin_p (BASE, OFF, MODE, STRICT)

#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, LABEL)                                \
  do {                                                                          \
    if (metag_legitimate_address_p (X, MODE, REG_OK_STRICT_FLAG))               \
      goto LABEL;                                                               \
  } while (0)

#define SYMBOLIC_CONST(X)                                                       \
  (SYMBOL_REF_P (X)                                                             \
   || LABEL_REF_P (X)                                                           \
   || (GET_CODE (X) == CONST && metag_symbolic_reference_mentioned_p (X)))

/* Go to LABEL if ADDR (a legitimate address expression)
   has an effect that depends on the machine mode it is used for. */
#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR, LABEL)                               \
 do {                                                                           \
   if (   GET_CODE (ADDR) == PRE_DEC                                            \
       || GET_CODE (ADDR) == POST_DEC                                           \
       || GET_CODE (ADDR) == PRE_INC                                            \
       || GET_CODE (ADDR) == POST_INC                                           \
       || GET_CODE (ADDR) == PRE_MODIFY                                         \
       || GET_CODE (ADDR) == POST_MODIFY)                                       \
     goto LABEL;                                                                \
  } while (0)

/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE SImode

/* Define as C expression which evaluates to nonzero if the tablejump
   instruction expects the table to contain offsets from the address of the
   table.                                                             */
#define CASE_VECTOR_PC_RELATIVE 1

/* Define this as 1 if `char' should by default be signed; else as 0.  */
#define DEFAULT_SIGNED_CHAR 0

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX 4

/* Nonzero if access to memory by bytes is the same speed as words */
#define SLOW_BYTE_ACCESS 1

/* Define this to be nonzero if shift instructions ignore all but the low-order
   few bits. */
#define SHIFT_COUNT_TRUNCATED 1

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* Define if loading in MODE, an integral mode narrower than BITS_PER_WORD
   will either zero-extend or sign-extend.  The value of this macro should
   be the code that says which one of the two operations is implicitly
   done, NIL if none.  */
#define LOAD_EXTEND_OP(MODE) ZERO_EXTEND

/* We assume that the store-condition-codes instructions store 0 for false
   and some other value for true.  This is the value stored for true.  */
/* #define STORE_FLAG_VALUE -1 */

/* Define this macro if it is advisable to hold scalars in registers
   in a wider mode than that declared by the program.  In such cases,
   the value is constrained to be within the bounds of the declared
   type, but kept valid in the wider mode.  The signedness of the
   extension may differ from that of the type. It is faster to
   zero extend chars than to sign extend them on META, with 16 bit
   values it's less obvious */
#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)                                     \
  do {                                                                          \
    if (GET_MODE_CLASS (MODE) == MODE_INT                                       \
        && GET_MODE_SIZE (MODE) < UNITS_PER_WORD)                               \
       (MODE) = SImode;                                                         \
  } while (0)

/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode SImode

/* A function address in a call instruction
   is a byte address (for indexing purposes)
   so give the MEM rtx a byte's mode.  */
#define FUNCTION_MODE QImode

/* Try to generate sequences that don't involve branches; enables cc insts */
#define BRANCH_COST 3

/* Comparison extensions-

   CC_NOOVmode is used when the state of the overflow flag is irrelavent to
   the compare case - e.g. comparison against zero is required.

   CC_Zmode is used as a more refined case of CC_NOOVmode where only the zero
   flag is relevant. For example if a HI or QI source value is being tested
   directly and the condition in EQ or NE in this case it's left to the
   pattern to check for other factors like zero as rhs of comparison.

*/

#define SELECT_CC_MODE(OP, X, Y)                                                \
  metag_select_cc_mode ((OP), (X), (Y))

#define REVERSIBLE_CC_MODE(MODE) (1)

#define REVERSE_CONDITION(CODE, MODE)                                           \
  (((MODE) != CC_FPmode && (MODE) != CC_FP_Qmode) ? reverse_condition (CODE)    \
   : reverse_condition_maybe_unordered (CODE))

/* Output to assembler file text saying following lines
   may contain character constants, extra white space, comments, etc.  */

#define ASM_APP_ON ""

/* Output to assembler file text saying following lines
   no longer contain unusual constructs.  */

#define ASM_APP_OFF ""



#define METAG_SYMBOL_FLAG_DIRECT (SYMBOL_FLAG_MACH_DEP << 0)
#define METAG_SYMBOL_FLAG_DIRECT_P(SYMBOL)                                      \
  ((SYMBOL_REF_FLAGS (SYMBOL) & METAG_SYMBOL_FLAG_DIRECT) != 0)

#define METAG_SYMBOL_FLAG_SMALL  (SYMBOL_FLAG_MACH_DEP << 1)
#define METAG_SYMBOL_FLAG_SMALL_P(SYMBOL)                                       \
  ((SYMBOL_REF_FLAGS (SYMBOL) & METAG_SYMBOL_FLAG_SMALL) != 0)

#define METAG_SYMBOL_FLAG_LARGE  (SYMBOL_FLAG_MACH_DEP << 2)
#define METAG_SYMBOL_FLAG_LARGE_P(SYMBOL)                                       \
  ((SYMBOL_REF_FLAGS (SYMBOL) & METAG_SYMBOL_FLAG_LARGE) != 0)

#define METAG_SYMBOL_FLAG_GLOBAL (SYMBOL_FLAG_MACH_DEP << 3)
#define METAG_SYMBOL_FLAG_GLOBAL_P(SYMBOL)                                      \
  ((SYMBOL_REF_FLAGS (SYMBOL) & METAG_SYMBOL_FLAG_GLOBAL) != 0)

#define METAG_SYMBOL_FLAG_BYTE  (SYMBOL_FLAG_MACH_DEP << 4)
#define METAG_SYMBOL_FLAG_BYTE_P(SYMBOL)                                        \
  ((SYMBOL_REF_FLAGS (SYMBOL) & METAG_SYMBOL_FLAG_BYTE) != 0)

#define METAG_SYMBOL_FLAG_WORD  (SYMBOL_FLAG_MACH_DEP << 5)
#define METAG_SYMBOL_FLAG_WORD_P(SYMBOL)                                        \
  ((SYMBOL_REF_FLAGS (SYMBOL) & METAG_SYMBOL_FLAG_WORD) != 0)

#define METAG_SYMBOL_FLAG_DWORD (SYMBOL_FLAG_MACH_DEP << 6)
#define METAG_SYMBOL_FLAG_DWORD_P(SYMBOL)                                       \
  ((SYMBOL_REF_FLAGS (SYMBOL) & METAG_SYMBOL_FLAG_DWORD) != 0)

#define METAG_SYMBOL_FLAG_LONG  (SYMBOL_FLAG_MACH_DEP << 7)
#define METAG_SYMBOL_FLAG_LONG_P(SYMBOL)                                        \
  ((SYMBOL_REF_FLAGS (SYMBOL) & METAG_SYMBOL_FLAG_LONG) != 0)

#define OVERRIDE_OPTIONS                                                        \
  metag_override_options ()

/* How to refer to registers in assembler output.
   This sequence is indexed by compiler's hard-register-number (see above).  */

#define REGISTER_NAMES                                                          \
{                                                                               \
  /* D0/D1 */                                                                   \
  "D0Re0", "D1Re0", "D0Ar6", "D1Ar5", "D0Ar4", "D1Ar3", "D0Ar2", "D1Ar1",       \
  "D0.4",  "D1RtP", "D0.5",  "D1.5",  "D0.6",  "D1.6",  "D0.7",  "D1.7",        \
  /* D0/D1 reserved */                                                          \
  "D0.8",  "D1.8",  "D0.9",  "D1.9",  "D0.10", "D1.10", "D0.11", "D1.11",       \
  "D0.12", "D1.12", "D0.13", "D1.13", "D0.14", "D1.14", "D0.15", "D1.15",       \
  /* A0/A1 */                                                                   \
  "A0StP", "A1GbP", "A0FrP", "A1LbP", "A0.2",  "A1.2",  "A0.3",  "A1.3",        \
  "A0.4",  "A1.4",  "A0.5",  "A1.5",  "A0.6",  "A1.6",  "A0.7",  "A1.7",        \
  /* Fakes may be seen in RTL */                                                \
  "FRAMEP","MCC",   "ARGP",  "RAPF",  "CPC0",  "CPC1",  "PC",    "TXRPT",       \
  /* FX */                                                                      \
  "FX.0",  "FX.1",  "FX.2" , "FX.3" , "FX.4" , "FX.5" , "FX.6" , "FX.7" ,       \
  "FX.8",  "FX.9",  "FX.10", "FX.11", "FX.12", "FX.13", "FX.14", "FX.15",       \
  /* TTREC(L) */                                                                \
  "TTREC", "TTRECL"                                                             \
}

#define ADDITIONAL_REGISTER_NAMES                                               \
{                                                                               \
  {"D0.0",  D0_0_REG},                                                          \
  {"D1.0",  D1_0_REG},                                                          \
  {"D0.1",  D0_1_REG},                                                          \
  {"D1.1",  D1_1_REG},                                                          \
  {"D0.2",  D0_2_REG},                                                          \
  {"D1.2",  D1_2_REG},                                                          \
  {"D0.3",  D0_3_REG},                                                          \
  {"D1.3",  D1_3_REG},                                                          \
  {"D1.4",  D1_4_REG},                                                          \
  {"A0.0",  A0_0_REG},                                                          \
  {"A1.0",  A1_0_REG},                                                          \
  {"A0.1",  A0_1_REG},                                                          \
  {"A1.1",  A1_1_REG},                                                          \
  {"D0FrT", D0_4_REG},                                                          \
  {"cc",    CC_REG}                                                             \
}

#define ASM_IDENTIFY_CPU        ".cpu"

#define META_IDENTIFY_CPU(FILE)                                                 \
  fprintf (FILE, "!\t%s\tmeta%s\n", ASM_IDENTIFY_CPU, metag_cpu_string)

/* A FILE comment and register declaration section should always
    begin the output.  */

/* Use direct B xx jump tables in code */
#define JUMP_TABLES_IN_TEXT_SECTION 1

/* How to renumber registers for dbx and gdb. */

#define DBX_REGISTER_NUMBER(REGNO) (REGNO)

#undef  DWARF2_DENUGGING_INFO
#define DWARF2_DEBUGGING_INFO   1
#define CAN_DEBUG_WITHOUT_FP

#define SUPPORTS_INIT_PRIORITY  0

/* The prefix to add to internally generated labels. */
#undef LOCAL_LABEL_PREFIX
#define LOCAL_LABEL_PREFIX      "$"

#undef IMMEDIATE_PREFIX
#define IMMEDIATE_PREFIX        "#"

/* This is how to output an insn to push a register on the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_PUSH(FILE, REGNO)                                        \
  fprintf (FILE, "\tSETD\t[%s+#4++], %s\n",                                     \
           reg_names[STACK_POINTER_REGNUM], reg_names[REGNO])

/* This is how to output an insn to pop a register from the stack.
   It need not be very fast code.  */
#define ASM_OUTPUT_REG_POP(FILE, REGNO)                                         \
  fprintf (FILE, "\tGETD\t%s, [%s+#(-4)++]\n", reg_names[REGNO],                \
           reg_names[STACK_POINTER_REGNUM])

/* This is how to output an element of a case-vector that is absolute.  */
#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)                                    \
  asm_fprintf (FILE, "\t%s\t%L%d\t" ASM_COMMENT_START                           \
                     " (abs table not expected)\n",                             \
               targetm.asm_out.aligned_op.si,                                   \
               VALUE)


/* This is how to output an element of a case-vector that is relative.  */
/* This is the first implementation of MiniM short branches. There is no check
   to ensure jump targets are in range of a short encoding yet. Short is used
   if requested otherwise long is used */
#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL)                        \
  asm_fprintf (FILE, "\t%sB\t%LL%d\t\t" ASM_COMMENT_START                       \
                     " (table %LL%d OK)\n",                                     \
               (!TARGET_MINIM) ? "" :                                           \
                                 ((metag_jump_table_branch == METAG_MINIM_JUMP_TABLE_BRANCH_AUTO \
                                   && cfun->machine->can_use_short_branch)      \
                                  || metag_jump_table_branch == METAG_MINIM_JUMP_TABLE_BRANCH_SHORT) \
                                 ? "XS\t" : "XL\t",                             \
               VALUE, REL)

#define PRINT_OPERAND(FILE, X, CODE)                                            \
  metag_print_operand (FILE, X, CODE)

#ifdef ENABLE_ASSERT_CHECKING
#define ASM_OUTPUT_OPCODE(FILE, OPCODE)                                         \
  metag_asm_output_opcode (FILE, OPCODE)
#endif /* ENABLE_ASSERT_CHECKING */

#define PRINT_OPERAND_ADDRESS(FILE, ADDR)                                       \
  metag_print_operand_address (FILE, ADDR)

#define OUTPUT_ADDR_CONST_EXTRA(FILE, X, FAIL)                                  \
  do {                                                                          \
    if (!metag_output_addr_const_extra (FILE, X))                               \
      goto FAIL;                                                                \
  } while (0)

/* On the META we want to pass partial stack/reg arguments such that
 * the first N bytes of the argument are passed on the stack and the
 * remaining M bytes of the argument are passed in registers.
 *
 * This allows the function prologue to just push the partial registers
 * onto the stack, which can be combined with the normal stack frame
 * setup.
 *
 * This macro is used in the machine independent files expr.c and function.c
 * to enable some META specific code for handling partial args. 
 */
#define METAG_PARTIAL_ARGS      1

/* Only perform branch elimination (by making instructions conditional) if
   we're optimising.  Otherwise it's of no use anyway.  */
#define FINAL_PRESCAN_INSN(INSN, OPVEC, NOPERANDS)                              \
  do {                                                                          \
    if (TARGET_COND_EXEC_OPTIMIZE)                                              \
    {                                                                           \
      int saved_alternative = which_alternative;                                \
                                                                                \
      metag_final_prescan_insn (INSN);                                          \
      which_alternative = saved_alternative;                                    \
    }                                                                           \
  } while (0)

#define PRINT_OPERAND_PUNCT_VALID_P(CODE) ((CODE) == '?' || (CODE) == '@')

enum metag_builtins
{
   METAG_BUILTIN_DCACHE_PRELOAD,
   METAG_BUILTIN_DCACHE_FLUSH,
   METAG_BUILTIN_DCACHE_REFRESH,
   METAG_BUILTIN_META2_CACHERD,
   METAG_BUILTIN_META2_CACHERL,
   METAG_BUILTIN_META2_CACHEWD,
   METAG_BUILTIN_META2_CACHEWL,
   METAG_BUILTIN_METAG_BSWAPS,
   METAG_BUILTIN_METAG_BSWAP,
   METAG_BUILTIN_METAG_BSWAPLL,
   METAG_BUILTIN_METAG_WSWAP,
   METAG_BUILTIN_METAG_WSWAPLL,
   METAG_BUILTIN_METAG_DSWAPLL,
   METAG_BUILTIN_THREAD_POINTER
};

#define TARGET_BUILTINS_METAC_1_0                                               \
  (metac_target == METAC_1_0_ID)

#define TARGET_BUILTINS_METAC_1_1                                               \
  (metac_target == METAC_1_1_ID)

#define TARGET_BUILTINS_METAC_1_2                                               \
  (metac_target == METAC_1_2_ID || metac_target == METAC_0_1_ID)

#define TARGET_BUILTINS_METAC_2_1                                               \
  (metac_target == METAC_2_1_ID)

#define METAG_CURRENT_FUNCTION_LOADS_PIC_REGISTER()                             \
  metag_current_function_loads_pic_register ()

#define ASSEMBLE_SYMBOL_REF(STREAM, X)                                          \
  assemble_name (STREAM, XSTR (X, 0))

/* The architecture define.  */
extern char metag_arch_name[];

/* Target CPU builtins.  */
#define TARGET_CPU_CPP_BUILTINS()                                               \
  metag_cpu_cpp_builtins (pfile)

#define METAG_CONST_OK_FOR_LETTERS_KPIJ(VALUE)                                  \
  metag_const_ok_for_letters_p (VALUE, "KIPJ")

#define METAG_CONST_OK_FOR_LETTERS_KPIJO3(VALUE)                                \
  metag_const_ok_for_letters_p (VALUE, "KIPJO3")

#define METAG_LETTER_FOR_CONST(VALUE)                                           \
  metag_letter_for_const (VALUE)

#define METAG_DATA_REG_P(REGNUM)                                                \
  metag_datareg_p (REGNUM)

#define METAG_ADDR_REG_P(REGNUM)                                                \
  metag_addrreg_p (REGNUM)

#define METAG_FPC_REG_P(REGNUM)                                                 \
  metag_fpcreg_p (REGNUM)

#define METAG_FPP_REG_P(REGNUM)                                                 \
  metag_fppreg_p (REGNUM)

/* Track the status of condition returns. 'REQD' means that a stub is
   required at the end of the function. 'DONE' means the stub has been emitted
   and should not be emitted again */
enum metag_cond_return_state
{
  METAG_COND_RETURN_NONE,
  METAG_COND_RETURN_REQD,
  METAG_COND_RETURN_DONE
};

typedef struct machine_function GTY(())
{
  int           valid;
  int           can_use_short_branch;
  int           pretend_regs;
  int           anonymous_args;
  int           anonymous_args_size;
  int           uses_pic_offset_table;
  int           loads_pic_register;
  unsigned int  savesize_gp;
  unsigned int  savesize_eh;
  unsigned int  FP_SP_offset;
  unsigned int  pic_save_size;
  unsigned int  out_local_size;
  int           ech_ctx_required;
  int           frame_pointer_needed;
  int           non_leaf;
  int           frame_pointer_epilogue;
  int           accesses_prev_frame;
  unsigned int  extras_gp;
  unsigned int  extras_eh;
  unsigned int  calls_eh_return;
  int           arg_adjust_delta;
  int           frame_adjust_delta;
  int           hwtrace;
  int           hwtrace_leaf;
  int           hwtrace_retpc;
  enum metag_cond_return_state cond_return_state;
} machine_function;

#define INCOMING_RETURN_ADDR_RTX                                                \
  gen_rtx_REG (SImode, RETURN_POINTER_REGNUM)

#define DWARF_FRAME_RETURN_COLUMN                                               \
  DWARF_FRAME_REGNUM (RETURN_POINTER_REGNUM)

#define ASM_FPRINTF_EXTENSIONS(FILE, ARGS, P)                                   \
  case '@':                                                                     \
    fputs (ASM_COMMENT_START, FILE);                                            \
    break;                                                                      \
                                                                                \
  case 'r':                                                                     \
    fputs (reg_names [va_arg (ARGS, unsigned int)], file);                      \
    break;

#define HARD_REGNO_RENAME_OK_FOR_INSN(INSN, FROM, TO)                           \
  metag_hard_regno_rename_ok_p (INSN, FROM, TO)

#define EPILOGUE_USES(REGNO)                                                    \
  ((REGNO) == D1RtP_REG || (REGNO) == A0FrP_REG                                 \
   || (TARGET_ECH && (REGNO) == METAG_ECH_REGNUM))

#define METAG_USE_RETURN_INSN(ISCOND)                                           \
  metag_use_return_insn (ISCOND)

/* A stucture to support counting the number of doloop optimised loops per nest */
struct doloopnest
{
  struct loop* inner;
  struct doloopnest* next;
};

#define DOLOOP_OPTIMIZE_INIT()                                                  \
   struct doloopnest * nests = NULL

#define DOLOOP_OPTIMIZE_LOOP(LOOP)                                              \
  do {                                                                          \
    /* Determine if any inner loop nests are already optimized */               \
    if (!metag_doloop_check_any_nest_optimized (LOOP, nests))                   \
      /* If we doloop optimize this loop, mark all inner loops as optimized */  \
      if (doloop_optimize (LOOP))                                               \
        metag_doloop_mark_nests_optimized (LOOP, &nests);                       \
  } while (0)


#define DOLOOP_OPTIMIZE_FINI()                                                  \
  do {                                                                          \
    /* Free the doloop nest counters */                                         \
    while (nests != NULL)                                                       \
      {                                                                         \
        struct doloopnest * next = nests->next;                                 \
                                                                                \
        free (nests);                                                           \
        nests = next;                                                           \
      }                                                                         \
  } while (0)

/* Try a machine-dependent way of reloading an illegitimate address
   operand.  If we find one, push the reload and jump to WIN.  This
   macro is used in only one place: `find_reloads_address' in reload.c.

   For the META, we wish to handle large displacements off a base
   register by splitting the addend across a MOV and the mem insn.
   This can cut the number of reloads needed.  */
#define LEGITIMIZE_RELOAD_ADDRESS(X, MODE, OPNUM, TYPE, IND, WIN)               \
  do {                                                                          \
    rtx new ## X = metag_legitimize_reload_address (X, MODE, OPNUM, TYPE, IND); \
                                                                                \
    if (new ## X)                                                               \
      {                                                                         \
        (X) = new ## X;                                                         \
        goto WIN;                                                               \
      }                                                                         \
  } while (0)

#define METAG_ELIMINABLE_REG_P(X)                                               \
  (REGNO (X) == FRAME_POINTER_REGNUM || REGNO (X) == ARG_POINTER_REGNUM)


#define SPLIT_EARLY                                                             \
  metag_split_early ()

#define SPLIT_HI_LO_SUM_EARLY                                                   \
  metag_split_hi_lo_sum_early ()

#define CALLER_SAVE_INSN_CODE(CODE)     (-1)

/* In driver-metag.c.  */
extern const char *metag_reduce_options (int argc, const char **argv);
extern const char *metag_emb_asm_preprocessor (int argc, const char **argv);
extern const char *metag_emb_onlylast (int argc, const char **argv);
extern const char *metag_emb_change_suffix (int argc, const char **argv);

#define EXTRA_SPEC_FUNCTIONS                                                    \
  { "meta_preprocessor", metag_emb_asm_preprocessor },                          \
  { "meta_reduce_options", metag_reduce_options },                              \
  { "meta_onlylast", metag_emb_onlylast },                                      \
  { "meta_change_suffix", metag_emb_change_suffix },


/* Do not re-invent the wheel!
   Instead of writing our very own option file expander, get gcc to do it
   convert the -mcpu-config=file option to be @file and expand */

#define GCC_DRIVER_HOST_INITIALIZATION                                          \
  do {                                                                          \
    int i;                                                                      \
    char * inject_options = getenv ("METAG_COMPILER_OPTIONS");                  \
                                                                                \
    /* Allow option injection from the environment primarily to reduce the      \
       build system logic required when building GCC with its library code and  \
       data in sections with non-standard names. This feature may find uses     \
       outside of this initial purpose but it's general use is discouraged. */  \
                                                                                \
    if (inject_options != NULL)                                                 \
      {                                                                         \
        int env_argc = 0;                                                       \
        int new_argc = 0;                                                       \
        char** new_argv = NULL;                                                 \
        char** env_argv = buildargv (inject_options);                           \
                                                                                \
        if (env_argv == NULL)                                                   \
          {                                                                     \
            fputs ("\nout of memory\n", stderr);                                \
            xexit (1);                                                          \
          }                                                                     \
                                                                                \
        /* Count the number of arguments.  */                                   \
        while (env_argv[env_argc] && *env_argv[env_argc])                       \
          ++env_argc;                                                           \
                                                                                \
        /* Make space for the new arguments */                                  \
        new_argc = argc + env_argc;                                             \
        new_argv = (char **)xmalloc (sizeof (char*) * (new_argc + 1));           \
                                                                                \
        /* Fill in the new argv */                                              \
        new_argv[0] = argv[0];                                                  \
        memcpy (new_argv + 1, env_argv, sizeof (char *) * env_argc);            \
        memcpy (new_argv + env_argc + 1, argv + 1,                              \
                sizeof (char *) * (argc - 1));                                  \
        new_argv[new_argc] = NULL;                                              \
                                                                                \
        /* Swap in the new argv */                                              \
        argv = new_argv;                                                        \
        argc = new_argc;                                                        \
                                                                                \
        /* Free the env_argv */                                                 \
        free (env_argv);                                                        \
      }                                                                         \
                                                                                \
    for (i = 1 ; i < argc ; i++)                                                \
      {                                                                         \
        if (strncmp ("-mcpu-config=", argv[i], 13) == 0)                        \
          {                                                                     \
            argv[i] = argv[i]+12;                                               \
            *argv[i] = '@';                                                     \
          }                                                                     \
      }                                                                         \
                                                                                \
    expandargv (&argc, &argv);                                                  \
                                                                                \
    prune_options (&argc, &argv);                                               \
  } while (0)

#endif /* __METAG_H */

#define METAG_HAVE_TLS metag_have_tls ()

