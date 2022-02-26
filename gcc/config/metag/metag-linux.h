/* Definitions of target machine for GNU compiler.
   Imagination Technologies Meta version.
   Copyright (C) 2008
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

#include "metag.h"

/* OG and OGA addressing are not available */
#define OG_ENABLED false

#define METAG_FLAG_PIC  flag_pic

/* The prefix to add to user-visible assembler symbols. */
#undef USER_LABEL_PREFIX
#define USER_LABEL_PREFIX       "_"

#ifdef MINIM_DEFAULT
#define DEFAULT_MINIM_ASM_SPEC "%{!mno-minim:-minim} "
#else
#define DEFAULT_MINIM_ASM_SPEC
#endif

#define ASM_SPEC                                                               \
  "%:meta_reduce_options(%{mmetac=*&mhard-float*&msoft-float&msimd-float}) "   \
  "%{mdsp:%{mmetac=1.2:-mdsp=metac12}%{mmetac=2.1:-mdsp=metac21}} "            \
  "%{mmetac=0.1:-mcpu=metac01} "                                               \
  "%{mmetac=1.0:-mcpu=metac10} "                                               \
  "%{mmetac=1.1:-mcpu=metac11} "                                               \
  "%{mmetac=1.2:-mcpu=metac12} "                                               \
  "%{mmetac=2.1:-mcpu=metac21} "                                               \
  "%{mhard-float*:%{mmetac=2.1:-mfpu=metac21}%{!mmetac=2.1:%eThe floating point unit is only available on a META 2.1}} " \
  "%{mminim:-minim} "                                                          \
  DEFAULT_MINIM_ASM_SPEC

/* This is how to output an assembler line for a numeric constant byte.  */

#define ASM_OUTPUT_BYTE(FILE, VALUE)                                            \
  fprintf (FILE, "\t.byte\t0x%x\n", (int) ((VALUE) & 0xff))

/* There has been some confusion over the meaning of the .align directive
   in gas over the last few years. However there is now a new directive
   called .balign that is explicitly defined as taking an absolute alignment
   value rather than a log2 value. We will now use this by default if
   available and leave the old implementation as is (even though it is wrong) */
#ifdef HAVE_GAS_BALIGN_AND_P2ALIGN

#undef ASM_OUTPUT_ALIGN
#define ASM_OUTPUT_ALIGN(FILE, LOG)                                             \
  do {                                                                          \
    if ((LOG)!=0)                                                               \
      fprintf ((FILE), "\t.balign %d\n", 1<<(LOG));                             \
  } while (0)

#else

#undef ASM_OUTPUT_ALIGN
#define ASM_OUTPUT_ALIGN(FILE, LOG)                                             \
  do {                                                                          \
    if ((LOG) != 0)                                                             \
      fprintf (FILE, "%s%u\n", ALIGN_ASM_OP, LOG);                              \
  } while (0)

#endif

/* This says how to output an assembler line
   to define a global common symbol.  */

#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)                            \
  do {                                                                          \
    fputs ("\t.comm\t", FILE);                                                  \
    assemble_name (FILE, (NAME));                                               \
    fprintf (FILE, ",%u\n", (SIZE));                                            \
  } while (0)

/* This says how to output an assembler line
   to define a local common symbol.  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE,ROUNDED)                              \
  do {                                                                          \
    fputs ("\t.lcomm\t", FILE);                                                 \
    assemble_name (FILE, (NAME));                                               \
    fprintf (FILE, ",%u\n", (SIZE));                                            \
  } while (0)

#define TEXT_SECTION_ASM_OP     ".text"

#define DATA_SECTION_ASM_OP     ".data"

#define DATA_SECTION            DATA_SECTION_ASM_OP

/* Try machine-dependent ways of modifying an illegitimate address
   to be legitimate.  If we find one, return the new, valid address.
   This macro is used in only one place: `memory_address' in explow.c.

   OLDX is the address as it was before break_out_memory_refs was called.
   In some cases it is useful to look at this to decide what needs to be done.

   MODE and WIN are passed so that this macro can use
   GO_IF_LEGITIMATE_ADDRESS.

   It is always safe for this macro to do nothing.  It exists to recognize
   opportunities to optimize the output. */

#define LEGITIMIZE_ADDRESS(X, OLDX, MODE, WIN)                                  \
  do {                                                                          \
   (X) = metag_legitimize_address (X, OLDX, MODE);                              \
   if (memory_address_p (MODE, X))                                              \
     goto WIN;                                                                  \
  } while (0)

#undef ASM_GENERATE_INTERNAL_LABEL
#define ASM_GENERATE_INTERNAL_LABEL(LABEL, PREFIX, NUM)                         \
  sprintf (LABEL, "*%s%s%u", LOCAL_LABEL_PREFIX, PREFIX, (unsigned)(NUM))

/* Register to hold the addressing base for position independent
   code access to data items.  */
#define PIC_OFFSET_TABLE_REGNUM   PIC_REG

/* Nonzero if the constant value X is a legitimate general operand
   when generating PIC code.  It is given that flag_pic is on and
   that X satisfies CONSTANT_P or is a CONST_DOUBLE.  */

#define LEGITIMATE_PIC_OPERAND_P(X)                                             \
  (! SYMBOLIC_CONST (X) || metag_legitimate_pic_address_disp_p (X))

/* The `FINALIZE_PIC' macro serves as a hook to emit these special
   codes once the function is being compiled into assembly code, but
   not before.  (It is not done before, because in the case of
   compiling an inline function, it would lead to multiple PIC
   prologues being included in functions which used inline functions
   and were compiled to assembly language.)  */

#define FIXME_FINALIZE_PIC                                                      \
  do                                                                            \
  {                                                                             \
    extern int current_function_uses_pic_offset_table;                          \
                                                                                \
    current_function_uses_pic_offset_table                                      \
      |= profile_flag | profile_block_flag;                                     \
  }                                                                             \
  while (0)

#define ASSEMBLE_END_FUNCTION(DECL,FNNAME)                                      \
  do {                                                                          \
    metag_emit_cond_return_stub_if_reqd ();                                     \
  } while (0)

#ifdef  HAVE_AS_TLS 
#undef  TARGET_HAVE_TLS
#define TARGET_HAVE_TLS HAVE_AS_TLS
#endif

/* Nonzero if the constant value X is a legitimate general operand.
   It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE. 

   This is true apart from cases where the symbol represents a TLS
   symbol.  */

#undef  LEGITIMATE_CONSTANT_P
#define LEGITIMATE_CONSTANT_P(X)                                               \
  (!tls_symbolic_operand_p (X))
