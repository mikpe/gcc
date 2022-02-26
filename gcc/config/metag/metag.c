/* Definitions of target machine for GNU compiler.
   Imagination Technologies Meta version.
   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2010,
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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "obstack.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-flags.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "reload.h"
#include "function.h"
#include "expr.h"
#include "optabs.h"
#include "toplev.h"
#include "recog.h"
#include "ggc.h"
#include "except.h"
#include "c-pragma.h"
#include "integrate.h"
#include "cfgloop.h"
#include "timevar.h"
#include "options.h"
#include "cgraph.h"
#include "target.h"
#include "cpplib.h"
#include "tm_p.h"
#include "tm-constrs.h"
#include "target-def.h"
#include "langhooks.h"
#include "version.h"
#include <ctype.h>

/* Is the given character a logical line separator for the assembler?
   Set the default to be the gas line separator and allow the embedded
   backend to override it. */
#ifndef IS_ASM_LOGICAL_LINE_SEPARATOR
#define IS_ASM_LOGICAL_LINE_SEPARATOR(C) ((C) == ';')
#endif

#define IS_PSEUDO_REGNO(REGNO)                                                  \
  ((REGNO) != INVALID_REGNUM && (REGNO) >= FIRST_PSEUDO_REGISTER)

#define IS_HARD_OR_VIRT_REGNO(REGNO)                                            \
  ((REGNO) != INVALID_REGNUM && (REGNO) < FIRST_PSEUDO_REGISTER)

#define NO_FUNCTION_CSE

#define REGNO_BIT(REGNO)        (1U << ((REGNO) >> 1))

#define df_regs_ever_live_p(REGNO)      regs_ever_live[(unsigned)(REGNO)]

static tree metag_handle_model_decl_attribute (tree *, tree, tree, int, bool *);
static int  metag_consumer_stalls_from_load_multi (rtx, rtx);
static bool metag_consumer_is_o2r (rtx, rtx, int, int);
static bool metag_pass_in_reg (CUMULATIVE_ARGS *, enum machine_mode, tree, bool);
static bool metag_same_reg_p (rtx, rtx, bool);
static long metag_const_double_sfmode (rtx);
static unsigned int metag_calculate_ech_ctx (void);
static unsigned int metag_adjust_savesize_ech (unsigned int*, unsigned int*,
                                               unsigned int*);
static void metag_emit_byte_swap16 (rtx, rtx);
static void metag_emit_byte_swap32 (rtx, rtx);
static int  metag_asm_insn_count (rtx);
static bool metag_is_label_within_function (rtx);
static rtx metag_function_return_reg (enum machine_mode mode);

/* METAG specific attribute support.

   model - select code model used to access data

        on VAR_DECL effects code that references symbol (weak effect)

          small:  intended to be addressed using A1GbP+OG(..)        (1 inst)
          large:  intended to be addresses using A1GbP+HI(..)+LO(..) (3 insts)

    on FUNCTION_DECL effects code within function concerned (strong effect)

          small:  access all small data using A1GbP+OG(..)   (no fn overhead)
                  access all larger data using A1GbP+OLA(..)
          large:  access all data using A1GbP+HI(..)+LO(..)  (no fn overhead)

*/

const struct
attribute_spec metag_attribute_table[] =
{
/*{ name,    min_len, max_len, decl_req, type_req, fn_type_req, handler } */
  { "model", 1,       1,       true,     false,    false,       metag_handle_model_decl_attribute },
  { NULL,    0,       0,       false,    false,    false,       NULL }
};

#define DEF_MCC                                                                \
  DEF_MCC_CODES(A,     NV,    "A"),                                            \
  DEF_MCC_CODES(NV,    A,     "NV"),                                           \
  DEF_MCC_CODES(EQ,    NE,    "EQ"),                                           \
  DEF_MCC_CODES(NE,    EQ,    "NE"),                                           \
  DEF_MCC_CODES(LO,    HS,    "LO"),                                           \
  DEF_MCC_CODES(HS,    LO,    "HS"),                                           \
  DEF_MCC_CODES(MI,    PL,    "MI"),                                           \
  DEF_MCC_CODES(PL,    MI,    "PL"),                                           \
  DEF_MCC_CODES(VS,    VC,    "VS"),                                           \
  DEF_MCC_CODES(VC,    VS,    "VC"),                                           \
  DEF_MCC_CODES(HI,    LS,    "HI"),                                           \
  DEF_MCC_CODES(LS,    HI,    "LS"),                                           \
  DEF_MCC_CODES(GE,    LT,    "GE"),                                           \
  DEF_MCC_CODES(LT,    GE,    "LT"),                                           \
  DEF_MCC_CODES(GT,    LE,    "GT"),                                           \
  DEF_MCC_CODES(LE,    GT,    "LE"),                                           \
  DEF_MCC_CODES(CS,    CC,    "CS"),                                           \
  DEF_MCC_CODES(CC,    CS,    "CC"),                                           \
  DEF_MCC_CODES(FEQ,   UNE,   "FEQ"),                                          \
  DEF_MCC_CODES(UNE,   FEQ,   "UNE"),                                          \
  DEF_MCC_CODES(FGT,   NFGT,  "FGT"),                                          \
  DEF_MCC_CODES(FGE,   NFGE,  "FGE"),                                          \
  DEF_MCC_CODES(FLT,   NFLT,  "FLT"),                                          \
  DEF_MCC_CODES(FLE,   NFLE,  "FLE"),                                          \
  DEF_MCC_CODES(U,     NU,    "UVS"),                                          \
  DEF_MCC_CODES(FLEG,  NFLEG, "FVC"),                                          \
  DEF_MCC_CODES(UGT,   NUGT,  "UGT"),                                          \
  DEF_MCC_CODES(UGE,   NUGE,  "UGE"),                                          \
  DEF_MCC_CODES(ULT,   NULT,  "ULT"),                                          \
  DEF_MCC_CODES(ULE,   NULE,  "ULE"),                                          \
  DEF_MCC_CODES(NFGT,  FGT,   "ULE"),                                          \
  DEF_MCC_CODES(NFGE,  FGE,   "ULT"),                                          \
  DEF_MCC_CODES(NFLT,  FLT,   "UGE"),                                          \
  DEF_MCC_CODES(NFLE,  FLE,   "UGT"),                                          \
  DEF_MCC_CODES(NU,    U,     "FVC"),                                          \
  DEF_MCC_CODES(NFLEG, FLEG,  "UVS"),                                          \
  DEF_MCC_CODES(NUGT,  UGT,   "FLE"),                                          \
  DEF_MCC_CODES(NUGE,  UGE,   "FLT"),                                          \
  DEF_MCC_CODES(NULT,  ULT,   "FGE"),                                          \
  DEF_MCC_CODES(NULE,  ULE,   "FGT")

#define MCC_MAX MCC_NULE

#define DEF_MCC_CODES(CODE, INV, ASMCODE) MCC_##CODE

typedef enum /* GCC : META condition codes */
{
  DEF_MCC
} metag_cc;

static metag_cc get_metag_cc (rtx);
static metag_cc get_metag_cc_float (enum rtx_code);
static bool metag_is_cc_quiet (metag_cc);

#undef DEF_MCC_CODES
#define DEF_MCC_CODES(CODE, INV, ASMCODE) MCC_##INV
static metag_cc const metag_inv_cc[MCC_MAX + 1] =
{
  DEF_MCC
};

#undef DEF_MCC_CODES
#define DEF_MCC_CODES(CODE, INV, ASMCODE) ASMCODE

static const char * const metag_cc_names[MCC_MAX + 1] =
{
  DEF_MCC
};

rtx metag_compare_op0;
rtx metag_compare_op1;

enum metag_model metag_model;

/* Which META core we're generating code for.  */
enum metac_target metac_target;

/* Which META core we are scheduling for see (define_attr "metacore" ...) */
enum attr_metacore metacore;

/* What FPU pipeline are we targetting */
int metag_fpu_single = 0;

/* Are we allowed to use any fpu resources */
int metag_fpu_resources = 0;

/* How wide is the widest memory access on this core */
int metag_memory_width = 64;

/* Should MiniM jump tables be emitted short, long or auto detected */
enum metag_jump_table_branch metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_AUTO;

/* -mextensions flags */
bool metag_meta2_bex_enabled = false;

/* Force tbictxsave to be enabled */
int metag_force_tbictxsave = true;

/* A finite state machine takes care of noticing whether or not instructions
   can be conditionally executed, and thus decrease execution time and code
   size by deleting branch instructions.  The fsm is controlled by
   final_prescan_insn, and controls the actions of ASM_OUTPUT_OPCODE.  */

/* For an explanation of these variables, see final_prescan_insn below.  */
static int metag_ccfsm_state;
static metag_cc metag_current_cc;
static GTY(()) rtx metag_target_insn = NULL_RTX;
static int metag_target_label;
static int metag_max_insns_skipped = 10;

#define METAG_DEBUG_CCEXEC      0

#if METAG_DEBUG_CCEXEC
static const char *attr_cond_name    (enum attr_cond);
static const char *attr_ccstate_name (enum attr_ccstate);
static const char *attr_predicale_name (enum attr_predicable);
#endif


bool
metag_datareg_p (unsigned int regno)
{
#if FIRST_DATA_REG == 0
  return regno <= LAST_DATA_REG;
#else
  return FIRST_DATA_REG <= regno && regno <= LAST_DATA_REG;
#endif
}

bool
metag_addrreg_p (unsigned int regno)
{
  return FIRST_ADDR_REG <= regno && regno <= LAST_ADDR_REG;
}

bool
metag_fpcreg_p (unsigned int regno)
{
  return FIRST_FP_REG <= regno && regno <= LAST_FP_REG;
}

bool
metag_fppreg_p (unsigned int regno)
{
  return FIRST_FP_REG <= regno && regno <= LAST_FP_REG && ((regno - FIRST_FP_REG) & 1) == 0;
}

#define METAG_DATAREG_UNIT(REGNO)    ((REGNO) & 1)
#define METAG_DATAREG_REGN(REGNO)    ((REGNO) >> 1)

static long
metag_const_double_sfmode (rtx op)
{
  long            l;
  REAL_VALUE_TYPE rv;

  REAL_VALUE_FROM_CONST_DOUBLE (rv, op);
  REAL_VALUE_TO_TARGET_SINGLE (rv, l);
  return l;
}

void
metag_split_movsi_immediate (rtx operands[])
{
  rtx           target= operands[0];
  rtx           imm   = operands[1];
  HOST_WIDE_INT value = INTVAL (imm);

  if ((value & 0x0000FFFF) == 0)
    emit_move_insn (target, imm);
  else if ((value & 0xFFFF0000) == 0)
    emit_move_insn (target, imm);
  else if ((value & 0xFFFF8000) == 0xFFFF8000)
    emit_move_insn (target, imm);
  else
    {
      emit_move_insn (target, gen_int_mode (value & 0xFFFF0000, SImode));

      emit_insn (gen_addsi3 (target, target, gen_int_mode (value & 0x0000FFFF, SImode)));
    }
}

void
metag_split_movsf_immediate (rtx operands[])
{
  rtx ops[2];

  ops[0] = gen_rtx_REG (SImode, REGNO (operands[0]));
  if (CONST_DOUBLE_P (operands[1]))
    ops[1] = gen_int_mode (metag_const_double_sfmode (operands[1]), SImode);
  else if (CONST_INT_P (operands[1]))
    ops[1] = operands[1];
  else
    gcc_unreachable ();

  metag_split_movsi_immediate (ops);
}

void
metag_split_movdi (rtx operands[])
{
  unsigned int dst_reg = REGNO (operands[0]);
  unsigned int src_reg = REGNO (operands[1]);

  emit_move_insn (gen_rtx_REG (SImode, dst_reg),
                  gen_rtx_REG (SImode, src_reg));
  
  emit_move_insn (gen_rtx_REG (SImode, dst_reg + 1),
                  gen_rtx_REG (SImode, src_reg + 1));
}

void
metag_split_movdi_immediate (rtx operands[])
{
  unsigned int dst_reg = REGNO (operands[0]);
  rtx          imm[2];
  rtx          ops[2];

  split_double (operands[1], &imm[0], &imm[1]);

  ops[0] = gen_rtx_REG (SImode, dst_reg);
  ops[1] = imm[0];
  metag_split_movsi_immediate (ops);

  ops[0] = gen_rtx_REG (SImode, dst_reg + 1);
  ops[1] = imm[1];
  metag_split_movsi_immediate (ops);
}

void
metag_split_movdf (rtx operands[])
{
  unsigned int dst_reg = REGNO (operands[0]);
  unsigned int src_reg = REGNO (operands[1]);

  emit_move_insn (gen_rtx_REG (SImode, dst_reg),
                  gen_rtx_REG (SImode, src_reg));

  emit_move_insn (gen_rtx_REG (SImode, dst_reg + 1),
                  gen_rtx_REG (SImode, src_reg + 1));
}

void
metag_split_movdf_immediate (rtx operands[])
{
  unsigned int dst_reg = REGNO (operands[0]);
  rtx          imm[2];
  rtx          ops[2];

  split_double (operands[1], &imm[0], &imm[1]);

  ops[0] = gen_rtx_REG (SImode, dst_reg);
  ops[1] = imm[0];
  metag_split_movsi_immediate (ops);

  ops[0] = gen_rtx_REG (SImode, dst_reg + 1);
  ops[1] = imm[1];

  metag_split_movsi_immediate (ops);
}

#define metag_non_leaf_function_p()                                                     \
  (reload_completed ? cfun->machine->non_leaf : !leaf_function_p ())

#define TBICTX_XMCC_BIT 0x0020
#define TBICTX_XDX8_BIT 0x0100
#define TBICTX_XAX4_BIT 0x0200
#define TBICTX_XEXT_BIT 0x1000

static unsigned int
metag_calculate_ech_ctx (void)
{
  unsigned int ech_ctx = 0; 

  /* Now emit ECH support */
  if (TARGET_ECH)
    {
      int regno;

      for (regno = FIRST_ECH_DATA_REG ; regno <= LAST_DATA_REG ; regno++)
        if (df_regs_ever_live_p (regno))
          {
            ech_ctx |= (TBICTX_XDX8_BIT << 16);
            break;
          }

      for (regno = FIRST_ECH_ADDR_REG ; regno <= LAST_ADDR_REG ; regno++)
        if (df_regs_ever_live_p (regno))
          {
            ech_ctx |= (TBICTX_XAX4_BIT << 16);
            break;
          }
    }

  return ech_ctx;
}

static unsigned int
metag_adjust_savesize_ech (unsigned int* savesize_gp, unsigned int* extras_gp,
                           unsigned int* FP_SP_offset)
{
  unsigned int ech_ctx = metag_calculate_ech_ctx ();

  if (ech_ctx != 0)
    {
      *extras_gp |= REGNO_BIT (METAG_ECH_REGNUM);
      *savesize_gp += UNITS_PER_WORD * 2;

      if (FP_SP_offset != NULL && METAG_ECH_REGNUM >= MIN_METAG_CSAVE_REGNUM)
        *FP_SP_offset += UNITS_PER_WORD * 2;
    }

  return ech_ctx;
}

/* function prologue */

void
metag_function_prologue (FILE *file ATTRIBUTE_UNUSED, HOST_WIDE_INT size)
{
  unsigned int savesize_gp  = 0;
  unsigned int savesize_eh  = 0;
  unsigned int FP_SP_offset = 0;
  unsigned int pretend_size = ALIGN_ON_STACK_BOUNDARY (current_function_pretend_args_size);
  unsigned int pretend_regs = pretend_size / UNITS_PER_WORD;
  bool         non_leaf     = metag_non_leaf_function_p ();
  unsigned int extras_gp    = 0;
  unsigned int extras_eh    = 0;
  unsigned int ech_ctx      = 0;
  bool         loads_pic_register;

  if (metag_ccfsm_state || metag_target_insn)
    gcc_unreachable ();                                   /* Sanity check */

  /* Track basis for the varargs we save */
  if (cfun->machine->anonymous_args)
    fprintf (file, ASM_COMMENT_START " Store varargs in registers %d (%d)\n",
             current_function_pretend_args_size, pretend_size);
  else if (current_function_pretend_args_size)
    fprintf (file, ASM_COMMENT_START " Store partial args in registers %d (%d)\n",
             current_function_pretend_args_size, pretend_size);

  /* Add in outgoing sizes */
  if (size != 0)
    fprintf (file, ASM_COMMENT_START " Allocate local size %ld\n", (long)size);

  if (non_leaf)
    fprintf (file, ASM_COMMENT_START " Allocate outgoing %d\n",
             current_function_outgoing_args_size);

  size += current_function_outgoing_args_size;

  /* Round size of local stack to preserve 64-bit alignments */
  size = ALIGN_ON_STACK_BOUNDARY (size);

  /* Make pretend regs into the first non-varargs register number */
  pretend_regs += MIN_METAG_PARM_REGNUM;

  {
    unsigned int regno;

    for (regno = MIN_METAG_PARM_REGNUM; regno <= MAX_METAG_CSAVE_REGNUM; regno += 2)
      {
        if (regno < pretend_regs
            || (!call_used_regs[regno]
                && (df_regs_ever_live_p (regno + 0) || df_regs_ever_live_p (regno + 1))))
          {
            extras_gp |= REGNO_BIT (regno);

            /* Push this data register */
            savesize_gp += UNITS_PER_WORD * 2;

            if (regno >= MIN_METAG_CSAVE_REGNUM)
              FP_SP_offset += UNITS_PER_WORD * 2;
          }
      }
  }

  /* Exception handler bits */
  if (current_function_calls_eh_return)
    {
      unsigned int n;

      for (n = 0; n < NUM_EH_RETURN_DATA_REGS; n++)
        {
          unsigned int regno = EH_RETURN_DATA_REGNO (n);

          if (regno != INVALID_REGNUM)
            {
              unsigned int regbit = REGNO_BIT (regno);

              if ((extras_eh & regbit) == 0)
                {
                  extras_eh |= regbit;
                  savesize_eh += UNITS_PER_WORD * 2;
                  FP_SP_offset += UNITS_PER_WORD * 2;
                }
            }
        }
    }

  /* Adjust the saved registers for ECH support */
  ech_ctx = metag_adjust_savesize_ech (&savesize_gp, &extras_gp, &FP_SP_offset);

  /* Recover original pretend regs */
  pretend_regs -= MIN_METAG_PARM_REGNUM;

  /* Can only save frame pointer from D0 temporary */
  if (cfun->machine->frame_pointer_epilogue)
    fputs (ASM_COMMENT_START " frame_pointer_needed to optimize epilogue\n", file);
  else if (frame_pointer_needed)
    fputs (ASM_COMMENT_START " GCC says frame_pointer_needed\n", file);

  if (frame_pointer_needed || non_leaf)
    {
      if (non_leaf)
        {
          extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);
          fputs (ASM_COMMENT_START " Save return address for non_leaf\n", file);
        }
      else
        fputs (ASM_COMMENT_START " Save return address with callers frame\n", file);

      if (frame_pointer_needed)
        extras_gp |= REGNO_BIT (TEMP_D0FRT_REGNUM);

      savesize_gp += UNITS_PER_WORD * 2;
      FP_SP_offset += UNITS_PER_WORD * 2;
    }
  else if (df_regs_ever_live_p (RETURN_POINTER_REGNUM))
    {
      extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);

      /* Push this data register */
      savesize_gp += UNITS_PER_WORD * 2;
      if (RETURN_POINTER_REGNUM >= MIN_METAG_CSAVE_REGNUM)
        FP_SP_offset += UNITS_PER_WORD * 2;
    }

  loads_pic_register = METAG_CURRENT_FUNCTION_LOADS_PIC_REGISTER ();
  if (loads_pic_register)
    FP_SP_offset += UNITS_PER_WORD * 2; /* Save PIC register. */

  /* Sanity checks between initial_elimination and prologue.
     If any of these tests fail then the generated code will be wrong so abort.  */

  gcc_assert (cfun->machine->valid);
  gcc_assert (cfun->machine->savesize_gp           == savesize_gp);
  gcc_assert (cfun->machine->savesize_eh           == savesize_eh);
  gcc_assert (cfun->machine->FP_SP_offset          == FP_SP_offset);
  gcc_assert (cfun->machine->frame_pointer_needed  == frame_pointer_needed);
  gcc_assert (cfun->machine->non_leaf              == non_leaf);
  gcc_assert (cfun->machine->out_local_size        == (unsigned HOST_WIDE_INT)size);
  gcc_assert (cfun->machine->calls_eh_return       == current_function_calls_eh_return);
  gcc_assert (cfun->machine->extras_gp             == extras_gp);
  gcc_assert (cfun->machine->extras_eh             == extras_eh);
  gcc_assert (cfun->machine->uses_pic_offset_table == current_function_uses_pic_offset_table);
  gcc_assert (cfun->machine->loads_pic_register    == loads_pic_register);
  gcc_assert (cfun->machine->ech_ctx_required      == (ech_ctx != 0));
}

void
metag_function_end_prologue (FILE *file)
{
  fputs (ASM_COMMENT_START " End prologue\n", file);
  return;
}

void
metag_function_begin_epilogue (FILE *file)
{
  fputs (ASM_COMMENT_START " Begin epilogue\n", file);
  return;
}

/* function epilogue */

void
metag_function_epilogue (FILE *file ATTRIBUTE_UNUSED, HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  return;
}

/* Is the return sequence just recover return address and return - two insts */
bool
metag_cheap_return (bool cond)
{
  unsigned int regno       = 0;
  unsigned int savesize_gp = 0;
  unsigned int extras_gp   = 0;
  unsigned int ech_ctx     = 0;
  bool         non_leaf    = metag_non_leaf_function_p ();

  if (!METAG_USE_RETURN_INSN (cond))
    return false;

  if (non_leaf)
    savesize_gp += UNITS_PER_WORD * 2; /* Must recover return address so return is not that cheap! */

    
  /* Adjust the saved registers for ECH support */
  ech_ctx = metag_adjust_savesize_ech (&savesize_gp, &extras_gp, NULL);

  for (regno = MIN_METAG_PARM_REGNUM; regno <= MAX_METAG_CSAVE_REGNUM; regno += 2)
    {
      if (!call_used_regs[regno]
          && (df_regs_ever_live_p (regno + 0) || df_regs_ever_live_p (regno + 1)))
        {
          /* Cannot do any pops in a conditional return/call */
          if (cond)
            return false;

          /* Cannot do simple return - too many pops! */
          if (savesize_gp != 0)
            return false;

          /* Have to do at least one pop */
          savesize_gp = UNITS_PER_WORD * 2;
        }
    }

  if (!(frame_pointer_needed || non_leaf) && df_regs_ever_live_p (RETURN_POINTER_REGNUM))
    {
      /* Have to do at least one pop */
      savesize_gp += UNITS_PER_WORD * 2;
    }

  if (current_function_calls_eh_return)
    return false;

  /* Cannot make the pop conditional! */
  if (cond && savesize_gp != 0)
    return false;

  /* Detect those hardware trace scenarios that lead to three extra instructions */
  if (cond && cfun->machine->hwtrace)
    return false;

  /* We can announce our intentions to optimise returns */
  return true;
}

/* All the code we need to perform a function sibcall */
const char *
output_sibcall (rtx operands[], unsigned int op_offset)
{
  char buf[1024];
  rtx  CallAddr;

  if (MEM_P (operands[op_offset]))
    CallAddr = XEXP (operands[op_offset], 0);
  else
    CallAddr = operands[op_offset];

  if (!TARGET_METAC_1_1 && SYMBOL_REF_P (CallAddr))
    {
      if (METAG_FLAG_PIC)
        gcc_unreachable ();
      else /* !METAG_FLAG_PIC */
        {
          /* Calculate return pointer using fast address bank add */
          sprintf (buf, "MOVT\t%s, #HI(%%c%d)",
                   reg_names[A0_2_REG], op_offset);
          output_asm_insn (buf, operands);

          /* The actual call is a branch */
          sprintf (buf, "JUMP\t%s, #LO(%%c%d)",
                   reg_names[A0_2_REG], op_offset);
          output_asm_insn (buf, operands);
        }
    }
  else if (SYMBOL_REF_P (CallAddr))
    {
      if (METAG_FLAG_PIC)
        {
          if (SYMBOL_REF_LOCAL_P (CallAddr))
            {
              /* Local funcs go via relative call */
              if (TARGET_MODEL_SMALL)
                {
                  sprintf (buf, "B\t%%c%d", op_offset);
                  output_asm_insn (buf, operands);
                }
              else
                {
                  sprintf (buf, "B\t%%c%d", op_offset);
                  output_asm_insn (buf, operands);
                }
            }
          else
            {
              /* The actual call is to an external function so goes via the PLT */
              sprintf (buf, "B\t%%c%d@PLT", op_offset);
              output_asm_insn (buf, operands);
            }
        }
      else /* !METAG_FLAG_PIC */
        {
          sprintf (buf, "B\t%%c%d", op_offset);
          output_asm_insn (buf, operands);
        }
    }
  else if (REG_P (CallAddr)
           && REGNO (CallAddr) != RETURN_POINTER_REGNUM)
    {
      sprintf (buf, "MOV%%?\tPC, %s", reg_names[REGNO (CallAddr)]);
      output_asm_insn (buf, operands);
    }
  else
    gcc_unreachable ();

  return "";
}

/* All the code we need to perform a function call */
const char *
output_call (rtx operands[], unsigned int op_offset)
{
  char               buf[1024];
  const char *       prefix = "(*call instance ";
  const char * const no_prefix = "";
  rtx                CallAddr;

  if (MEM_P (operands[op_offset]))
    CallAddr = XEXP (operands[op_offset], 0);
  else
    CallAddr = operands[op_offset];

  if (!TARGET_METAC_1_1 && SYMBOL_REF_P (CallAddr))
    {
      if (METAG_FLAG_PIC)
        gcc_unreachable ();
      else /* !METAG_FLAG_PIC */
        {
          /* Calculate return pointer using fast address bank add */
          sprintf (buf, "MOVT\t%s, #HI(%%c%d)\t%s %s",
                   reg_names[RETURN_POINTER_REGNUM], op_offset, ASM_COMMENT_START, prefix);
          output_asm_insn (buf, operands);
          prefix = no_prefix;

          /* The actual call is a branch */
          sprintf (buf, "CALL\t%s, #LO(%%c%d)\t%s ... OK)",
                   reg_names[RETURN_POINTER_REGNUM], op_offset, ASM_COMMENT_START);
          output_asm_insn (buf, operands);

          /* Some calls need additional padding instructions */
          metag_pad_function_call (CallAddr);
        }
    }
  else if (SYMBOL_REF_P (CallAddr))
    {
      if (METAG_FLAG_PIC)
        {
          if (SYMBOL_REF_LOCAL_P (CallAddr))
            {
              /* Local funcs go via relative CALL */
              if (TARGET_MODEL_SMALL)
                {
                  sprintf (buf, "CALLR\t%s, %%c%d\t%s %s)",
                           reg_names[RETURN_POINTER_REGNUM], op_offset, ASM_COMMENT_START, prefix);
                  output_asm_insn (buf, operands);
                  prefix = no_prefix;
                }
              else
                {
                  sprintf (buf, "CALLR\t%s, %%c%d\t%s %s)",
                           reg_names[RETURN_POINTER_REGNUM], op_offset, ASM_COMMENT_START, prefix);
                  output_asm_insn (buf, operands);
                  prefix = no_prefix;
                }
            }
          else
            {
              /* The actual call is to an external function so goes via the PLT */
              sprintf (buf, "CALLR\t%s, %%c%d@PLT\t%s %s)",
                       reg_names[RETURN_POINTER_REGNUM], op_offset, ASM_COMMENT_START, prefix);
              output_asm_insn (buf, operands);
              prefix = no_prefix;
            }
        }
      else /* !METAG_FLAG_PIC */
        {
          /* The actual call is a branch */
          sprintf (buf, "CALLR\t%s, %%c%d\t%s %s)",
                   reg_names[RETURN_POINTER_REGNUM], op_offset, ASM_COMMENT_START, prefix);
          output_asm_insn (buf, operands);
          prefix = no_prefix;
        }

      /* Some calls need additional padding instructions */
      metag_pad_function_call (CallAddr);
    }
  else if (REG_P (CallAddr)
           && REGNO (CallAddr) != RETURN_POINTER_REGNUM)
    {
      /* Must move the address to call into D1RtP */
      sprintf (buf, "MOV%%?\t%s, %s\t%s %s ...",
               reg_names[RETURN_POINTER_REGNUM],
               reg_names[REGNO (CallAddr)], ASM_COMMENT_START, prefix);
      output_asm_insn (buf, operands);

      /* The actual call is a SWAP */
      sprintf (buf, "SWAP%%?\tPC, %s\t%s ... OK)",
               reg_names[RETURN_POINTER_REGNUM], ASM_COMMENT_START);
      output_asm_insn (buf, operands);
    }
  else
    metag_abort (CallAddr);

  return "";
}

unsigned int
metag_mem_base (rtx op)
{
  rtx ref;

  /* We only match with MEM operands */
  if (!MEM_P (op))
    return INVALID_REGNUM;
        
  /* Get root of address expression */
  ref = XEXP (op, 0);

  if (SYMBOL_REF_P (ref))
    {
      /* Must be a direct access to an atomic symbol */
      return GLOBAL_POINTER_REGNUM;
    }

  if (GET_CODE (ref) == PLUS
      || GET_CODE (ref) == PRE_INC
      || GET_CODE (ref) == PRE_DEC
      || GET_CODE (ref) == POST_INC
      || GET_CODE (ref) == POST_DEC
      || GET_CODE (ref) == PRE_MODIFY
      || GET_CODE (ref) == POST_MODIFY)
    {
      /* De-reference first parameter of address expression */
      ref = XEXP (ref, 0);
    }

  if (SUBREG_P (ref)
      && REG_P (SUBREG_REG (ref)))
    {
      unsigned int regno = REGNO (SUBREG_REG (ref));

      if (IS_HARD_OR_VIRT_REGNO (regno))
        return regno + SUBREG_BYTE (ref);

      return regno;
    }

  if (REG_P (ref))
    return REGNO (ref);

  return INVALID_REGNUM;
}

bool
metag_mem_base_p (rtx op, enum reg_class class)
{
  unsigned int reg_num = metag_mem_base (op);

  /* We only match with MEM operands */
  if (reg_num == INVALID_REGNUM)
    return false;

  if (reg_renumber != NULL && IS_PSEUDO_REGNO (reg_num))
    reg_num = reg_renumber[reg_num];

  if (reg_num == INVALID_REGNUM)
    return false;

  return METAG_REGNO_REG_CLASS (reg_num) == class;
}

int
debug_metag_md (void)
{
  static int counter = 0;

  return counter++;
}

bool
metag_slow_store (rtx mem, rtx reg)
{
  unsigned int regno;

  if (!MEM_P (mem))
    return false;

  if (REG_P (reg))
    regno = REGNO (reg);
  else if (SUBREG_P (reg)
           && REG_P (SUBREG_REG (reg)))
    {
      regno = REGNO (SUBREG_REG (reg));
      if (IS_HARD_OR_VIRT_REGNO (regno))
        regno += SUBREG_BYTE (reg);
    }
  else
    return false;

  if (reg_renumber != NULL && IS_PSEUDO_REGNO (regno))
    regno = reg_renumber[regno];

  if (IS_PSEUDO_REGNO (regno))
    return false;

  if (GET_MODE_SIZE (GET_MODE (mem)) <= UNITS_PER_WORD)
    return metag_mem_base_p (mem, METAG_REGNO_REG_CLASS (regno));
  else
    return (metag_mem_base_p (mem, METAG_REGNO_REG_CLASS (regno))
            || metag_mem_base_p (mem, METAG_REGNO_REG_CLASS (regno + 1)));
}

rtx
metag_gen_safe_temp (enum machine_mode mode, rtx reg)
{
  unsigned int regno;

  gcc_assert (   A0_SCRATCH != INVALID_REGNUM && fixed_regs[A0_SCRATCH]
              && A1_SCRATCH != INVALID_REGNUM && fixed_regs[A1_SCRATCH]);

  if (REG_P (reg))
    regno = REGNO (reg);
  else if (SUBREG_P (reg)
           && REG_P (SUBREG_REG (reg)))
    {
      regno = REGNO (SUBREG_REG (reg));
      if (IS_HARD_OR_VIRT_REGNO (regno))
        regno += SUBREG_BYTE (reg);
    }
  else
    regno = INVALID_REGNUM;

  if (reg_renumber != NULL && IS_PSEUDO_REGNO (regno))
    regno = reg_renumber[regno];

  gcc_assert (regno != INVALID_REGNUM);

  if (METAG_REGNO_REG_CLASS (regno) == A1_REGS
      && GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
    {
      gcc_assert (regno != A0_SCRATCH);

      /* Not safe to use register in same unit */
      return gen_rtx_REG (mode, A0_SCRATCH);
    }

  /* Provide suitable temp register */
  if (GET_MODE_SIZE (GET_MODE (reg)) > UNITS_PER_WORD)
    {
      gcc_assert (regno != A0_SCRATCH);
      gcc_assert (regno != A1_SCRATCH);

      return gen_rtx_REG (mode, A0_SCRATCH);
    }

  gcc_assert (regno != A1_SCRATCH);

  return gen_rtx_REG (mode, A1_SCRATCH);
}

enum machine_mode
metag_select_cc_mode (enum rtx_code code, rtx x, rtx y)
{
#undef LETS_US_SEE
#ifdef LETS_US_SEE
  fprintf (stderr, "metag_select_cc_mode %s\n", GET_RTX_NAME (code));
  debug_rtx (x);
  debug_rtx (y);
#endif

  /* An operation that sets the condition codes as a side-effect, the
     V flag is not set correctly, so we can only use comparisons where
     this doesn't matter.  (For LT and GE we can use MI and PL instead.  */
  if (GET_MODE (x) == SImode
      && y == const0_rtx
      && (code == EQ || code == NE || code == LT || code == GE)
      && (GET_CODE (x) == PLUS
          || GET_CODE (x) == MINUS
          || GET_CODE (x) == AND
          || GET_CODE (x) == IOR
          || GET_CODE (x) == XOR
          || GET_CODE (x) == NOT
          || GET_CODE (x) == NEG
          || GET_CODE (x) == LSHIFTRT
          || GET_CODE (x) == ASHIFT
          || GET_CODE (x) == ASHIFTRT
          || GET_CODE (x) == SIGN_EXTEND
          || GET_CODE (x) == ZERO_EXTEND
          || GET_CODE (x) == ZERO_EXTRACT))
    {
#ifdef LETS_US_SEE
      fputs (" return NOOV\n", stderr);
#endif
      return CC_NOOVmode;
    }

  /* A special case, Combine likes to generate
     (compare (reg X) (neg (reg Y))
     instead of
     (compare (plus (reg X) (reg y))
              (const_int 0))
  */
  if (GET_MODE (x) == SImode
      && (code == EQ || code == NE || code == LT || code == GE)
      && GET_CODE (y) == NEG)
    {
#ifdef LETS_US_SEE
      fputs (" return NOOV\n", stderr);
#endif
      return CC_NOOVmode;
    }

  /* If we're comparingi EQ/NE against 0 then use Zmode */
  if (GET_MODE (x) == SImode
      && y == const0_rtx
      && (code == EQ || code == NE))
    {
#ifdef LETS_US_SEE
       fputs (" return Z\n", stderr);
#endif
       return CC_Zmode;
    }

  if ((GET_MODE (x) == HImode || GET_MODE (x) == QImode)
      && (code == EQ || code == NE))
    {
      /* Enough to trigger the patterns if HI/QI equality involved */
#ifdef LETS_US_SEE
      fputs (" return Z\n", stderr);
#endif
      return CC_Zmode;
    }

  if (GET_MODE_CLASS (GET_MODE (x)) == MODE_FLOAT)
    {
      if (metag_is_cc_quiet (get_metag_cc_float (code)))
        {
#ifdef LETS_US_SEE
          fputs (" return FP_Q\n", stderr);
#endif
          return CC_FP_Qmode;
        }
      else
        {
#ifdef LETS_US_SEE
          fputs (" return FP\n", stderr);
#endif
          return CC_FPmode;
        }
    }

#ifdef LETS_US_SEE
  fputs (" return CC\n", stderr);
#endif
  return CCmode;
}

bool
gen_metag_compare (enum rtx_code code, rtx operands[], int index)
{
  if (!(code == LTGT || code == UNEQ))
  {
    enum machine_mode mode = SELECT_CC_MODE (code, metag_compare_op0,
                                                   metag_compare_op1);

    rtx cc_reg = gen_rtx_REG (mode, MCC_REGNUM);

    emit_insn (gen_rtx_SET (VOIDmode,
                            cc_reg,
                            gen_rtx_COMPARE (mode, metag_compare_op0, metag_compare_op1)));

    operands[index] = cc_reg;

    return true;
  }

  return false;
}

static metag_cc
get_metag_cc_float (enum rtx_code comp_code)
{
  switch (comp_code)
    {
    case NE:  return MCC_UNE;
    case EQ:  return MCC_FEQ;
    case GE:  return MCC_FGE;
    case GT:  return MCC_FGT;
    case LE:  return MCC_FLE;
    case LT:  return MCC_FLT;
/* LTGT cannot be handled by META */
/* UNEQ cannot be handled by META */
    case UNGE:  return MCC_UGE;
    case UNGT:  return MCC_UGT;
    case UNLE:  return MCC_ULE;
    case UNLT:  return MCC_ULT;
    case UNORDERED: return MCC_U;
    case ORDERED:   return MCC_FLEG;
    default:
                    break;
    }
  gcc_unreachable ();
}

static metag_cc
get_metag_cc (rtx comparison)
{
  enum machine_mode mode = GET_MODE (XEXP (comparison, 0));
  enum rtx_code comp_code = GET_CODE (comparison);

  if (GET_MODE_CLASS (mode) != MODE_CC)
    mode = SELECT_CC_MODE (comp_code, XEXP (comparison, 0),
                           XEXP (comparison, 1));

  switch (mode)
    {
    case CC_NOOVmode:
      switch (comp_code)
        {
        case NE:  return MCC_NE;
        case EQ:  return MCC_EQ;
        case GE:  return MCC_PL;
        case LT:  return MCC_MI;
        case LTU: return MCC_CS;
        case GEU: return MCC_CC;
        default:
          break;
        }
      break;

    case CC_Zmode:
      switch (comp_code)
        {
        case NE: return MCC_NE;
        case EQ: return MCC_EQ;
        default:
          break;
        }
      break;

    case CC_FPmode:
    case CC_FP_Qmode:
      return get_metag_cc_float (comp_code);

    case CCmode:
      switch (comp_code)
        {
        case NE:  return MCC_NE;
        case EQ:  return MCC_EQ;
        case GE:  return MCC_GE;
        case GT:  return MCC_GT;
        case LE:  return MCC_LE;
        case LT:  return MCC_LT;
        case GEU: return MCC_HS;
        case GTU: return MCC_HI;
        case LEU: return MCC_LS;
        case LTU: return MCC_LO;
        default:
         break;
        }
      break;

    default:
     break;
    }

  metag_abort (comparison);
}

static bool
metag_is_cc_quiet (metag_cc metag_comp_code)
{
  switch (metag_comp_code)
    {
    case MCC_FEQ:
    case MCC_UNE:
    case MCC_U:
    case MCC_UGT:
    case MCC_UGE:
    case MCC_ULT:
    case MCC_ULE:
    case MCC_NU:
    case MCC_NUGT:
    case MCC_NUGE:
    case MCC_NULT:
    case MCC_NULE:
      return true;
    case MCC_FGT:
    case MCC_FGE:
    case MCC_FLT:
    case MCC_FLE:
    case MCC_FLEG:
    case MCC_NFGT:
    case MCC_NFGE:
    case MCC_NFLT:
    case MCC_NFLE:
    case MCC_NFLEG:
      return false;
/* LTGT cannot be handled by META */
/* UNEQ cannot be handled by META */
    default:
      gcc_unreachable ();
    }
}

/* Recognise VAR_DECL DECL's which are atomics of size <= 8

   For the metag we only want to mark variables that are atomic and less
   than 64-bits for optimisation as condidates for direct LOAD/STORE using
   A1LbP or A1GbP as a base.

   All other symbols will be accessed indirectly using OGA(), this is true of
   functions, string constants, or constructors.
*/

#define SET_SYMBOL_FLAG_SMALL(SYMBOL)                                                   \
  (SYMBOL_REF_FLAGS (SYMBOL) = (SYMBOL_REF_FLAGS (SYMBOL) & ~METAG_SYMBOL_FLAG_GLOBAL   \
                                                          & ~METAG_SYMBOL_FLAG_LARGE)   \
                               | METAG_SYMBOL_FLAG_SMALL)

#define SET_SYMBOL_FLAG_LARGE(SYMBOL)                                                   \
  (SYMBOL_REF_FLAGS (SYMBOL) = (SYMBOL_REF_FLAGS (SYMBOL) & ~METAG_SYMBOL_FLAG_GLOBAL   \
                                                          & ~METAG_SYMBOL_FLAG_SMALL)   \
                               | METAG_SYMBOL_FLAG_LARGE)

#define SET_SYMBOL_FLAG_GLOBAL(SYMBOL)                                                  \
  (SYMBOL_REF_FLAGS (SYMBOL) = (SYMBOL_REF_FLAGS (SYMBOL) & ~METAG_SYMBOL_FLAG_SMALL    \
                                                          & ~METAG_SYMBOL_FLAG_LARGE)   \
                               | METAG_SYMBOL_FLAG_GLOBAL)

#define SET_SYMBOL_FLAG_BYTE(SYMBOL)                                                    \
  (SYMBOL_REF_FLAGS (SYMBOL) = (SYMBOL_REF_FLAGS (SYMBOL)  & ~METAG_SYMBOL_FLAG_WORD    \
                                                           & ~METAG_SYMBOL_FLAG_DWORD   \
                                                           & ~METAG_SYMBOL_FLAG_LONG)   \
                               |  METAG_SYMBOL_FLAG_BYTE)

#define SET_SYMBOL_FLAG_WORD(SYMBOL)                                                    \
  (SYMBOL_REF_FLAGS (SYMBOL) = (SYMBOL_REF_FLAGS (SYMBOL) & ~METAG_SYMBOL_FLAG_BYTE     \
                                                          & ~METAG_SYMBOL_FLAG_DWORD    \
                                                          & ~METAG_SYMBOL_FLAG_LONG)    \
                               |  METAG_SYMBOL_FLAG_WORD)

#define SET_SYMBOL_FLAG_DWORD(SYMBOL)                                                   \
  (SYMBOL_REF_FLAGS (SYMBOL) = (SYMBOL_REF_FLAGS (SYMBOL) & ~METAG_SYMBOL_FLAG_BYTE     \
                                                          & ~METAG_SYMBOL_FLAG_WORD     \
                                                          & ~METAG_SYMBOL_FLAG_LONG)    \
                               |  METAG_SYMBOL_FLAG_DWORD)

#define SET_SYMBOL_FLAG_LONG(SYMBOL)                                                    \
  (SYMBOL_REF_FLAGS (SYMBOL) = (SYMBOL_REF_FLAGS (SYMBOL) & ~METAG_SYMBOL_FLAG_BYTE     \
                                                          & ~METAG_SYMBOL_FLAG_WORD     \
                                                          & ~METAG_SYMBOL_FLAG_DWORD)   \
                               |  METAG_SYMBOL_FLAG_LONG)

#define SET_SYMBOL_FLAG_UNKN(SYMBOL)                                                    \
  (SYMBOL_REF_FLAGS (SYMBOL) = (SYMBOL_REF_FLAGS (SYMBOL) & ~METAG_SYMBOL_FLAG_BYTE     \
                                                          & ~METAG_SYMBOL_FLAG_WORD     \
                                                          & ~METAG_SYMBOL_FLAG_DWORD    \
                                                          & ~METAG_SYMBOL_FLAG_LONG)

/* This macro definition, if any, is executed immediately after the rtl for
   decl has been created and stored in DECL_RTL (decl). The value of the rtl
   will be a mem whose address is a symbol_ref. 

   The usual thing for this macro to do is to record a flag in the symbol_ref
   (such as SYMBOL_REF_FLAG) or to store a modified name string in the
   symbol_ref (if one bit is not enough information).

   For META we do both, the METAG_SYMBOL_FLAG_DIRECT is set if-

      The decl defines the address of an atomic variable <= 8 bytes in size
          AND either we are targeting SMALL compilation mode
                     AND the decl DOES NOT HAVE __attribute__ ((model(large)))
              or     we are targeting LARGE compilation mode
                     AND the decl HAS __attribute__ ((model(small)))

   The METAG_SYMBOL_FLAG_DIRECT hence indicates if direct access to the data
   symbols value is to be supported by the instruction patterns provided the
   size of the load/store matches the size of atomic value concerned. The access
   causes the symbol is to be generally accessed using either A1GbP+OG(..) or
   A1GbP+OGA(...) if the address or value is to be accessed.

        METAG_SYMBOL_FLAG_SMALL
              If SYMBOL_REF_FLAG_DIRECT set to directly access data using-

                GETx  Reg, [A1GbP+#OG(Symbol)] ; x set to name[2]
                SETx  [A1GbP+#OG(Symbol)], Reg ; x set to name[2]

              else and if address of location required-

                GETD  Reg, [A1GbP+#OGA(Symbol)]

        METAG_SYMBOL_FLAG_LARGE
              Only support access to a data symbol's address via-

                MOV  Reg, A1GbP                ; name[2] will be 'X'
                ADDT Reg, Reg, #HI(OG(Symbol)) ; 32-bit offset
                ADD  Reg, Reg, #LO(OG(Symbol))
        
        METAG_SYMBOL_FLAG_GLOBAL
              Only support access as a absolute global address-

                MOVT Reg, #HI(Symbol)      ; 32-bit absolute address
                ADD  Reg, Reg, #LO(Symbol)

   The following flags:
     METAG_SYMBOL_FLAG_BYTE
     METAG_SYMBOL_FLAG_WORD
     METAG_SYMBOL_FLAG_DWORD
     METAG_SYMBOL_FLAG_LONG
   are used to indicate either the size of a directly accessible data symbol or
   that only the address of the item can be accessed directly because it is
   too large, a small array, a small structure, a small union, or not in the
   optimised data section at all.
*/

void
metag_encode_section_info (tree decl, rtx rtl, int first)
{
  char         SorLorG  = '\0';
  rtx          symbol   = NULL_RTX;
  bool         direct   = false;
  char         size     = 'X';
  default_encode_section_info (decl, rtl, first);

  /* Grab the symbol from the rtl passed in */
  symbol = XEXP (rtl, 0);

  if (METAG_FLAG_PIC)
    {
      /* PIC code only needs to deal with functions and variables */
      if (TREE_CODE (decl) != FUNCTION_DECL
          && TREE_CODE (decl) != VAR_DECL)
        return;

      gcc_assert (SYMBOL_REF_P (symbol));

      direct = SYMBOL_REF_LOCAL_P (symbol);
    }
  else if (metag_bfd_tls_referenced_p (rtl))
    {
      if (TREE_CODE (decl) != VAR_DECL)
        return;
    }
  else
    {
      /* Note: Binutils toolchain DOESN'T support #OG or #OGA addressing
         via A1GbP so we ignore the memory model and always use direct
         32-bit absolute access.  */

      /* Direct 32-bit absolute access */
      SorLorG = 'G';
      size    = 'X';
      direct   = false;

      if (SorLorG == 'S')
        SET_SYMBOL_FLAG_SMALL (symbol);
      else if (SorLorG == 'L')
        SET_SYMBOL_FLAG_LARGE (symbol);
      else if (SorLorG == 'G')
        SET_SYMBOL_FLAG_GLOBAL (symbol);
      else
        gcc_unreachable ();

      if (size == 'B')
        SET_SYMBOL_FLAG_BYTE (symbol);
      else if (size == 'W')
        SET_SYMBOL_FLAG_WORD (symbol);
      else if (size == 'D')
        SET_SYMBOL_FLAG_DWORD (symbol);
      else if (size == 'L')
        SET_SYMBOL_FLAG_LONG (symbol);
      else if (size == 'X')
        SET_SYMBOL_FLAG_LONG (symbol);
      else
        gcc_unreachable ();
    }

  if (direct)
    SYMBOL_REF_FLAGS (symbol) |= METAG_SYMBOL_FLAG_DIRECT;
  else
    SYMBOL_REF_FLAGS (symbol) &= ~METAG_SYMBOL_FLAG_DIRECT;
}

/* With DSP features enabled, the compiler will use the V2SImode
   vectors and with FPU features enabled, the compiler will use
   V2SFmode */

bool
metag_vector_mode_supported_p (enum machine_mode mode)
{
    return (TARGET_DSP && (mode == V2SImode))
           || (TARGET_FPU_SIMD && (mode == V2SFmode));
}

/* Called by OVERRIDE_OPTIONS to initialize various things.  */

void
metag_override_options (void)
{
  static const struct cpu_table {
    const char *const         name;
    const enum processor_type processor;
    const enum attr_metacore  tune;
    const enum metac_target   cpu;
  } cpu_table[] = {
    { "0.1",    PROCESSOR_METAC_0_1, METACORE_METAC_0_1, METAC_0_1_ID },
    { "1.0",    PROCESSOR_METAC_1_0, METACORE_METAC_1_0, METAC_1_0_ID },
    { "1.1",    PROCESSOR_METAC_1_1, METACORE_METAC_1_1, METAC_1_1_ID },
    { "1.2",    PROCESSOR_METAC_1_2, METACORE_METAC_1_2, METAC_1_2_ID },
    { "2.1",    PROCESSOR_METAC_2_1, METACORE_METAC_2_1, METAC_2_1_ID },
    { NULL,     0,                   0,                  0            }
  };

  if (strcmp (metag_model_string, "small") == 0)
    {
      warning (0, "Small memory model not supported, using large model");
      metag_model = METAG_MODEL_LARGE;
    }
  else if (strcmp (metag_model_string, "large") == 0)
    metag_model = METAG_MODEL_LARGE;
  else
    error ("bad value %qs for -mmodel switch", metag_model_string);

  /* If it's not defined or still has the initial value then use METAC_DEFAULT
     to set the target string. The conversion of string into ID can then take
     place as normal. */
  if (strcmp (metag_cpu_string, "") == 0)
    {
      if (TARGET_MTX)
        metag_cpu_string = "1.2";
      else
        metag_cpu_string = METAC_DEFAULT;
    }

  if (metag_cpu_string)
    {
      unsigned int i;
      bool newer_than_default = false;

      for (i = 0; cpu_table[i].name != NULL; i++)
        {
          if (strcmp (metag_cpu_string, cpu_table[i].name) == 0)
            {
              /* This test is present in order to prevent a toolchain built for an old core
                 allowing a newer core to be targetted. The rest of the toolchain may
                 therefore not support the newer core! */
              if (newer_than_default)
                {
                  error ("Bad value %qs for -mmetac switch. Must not be more recent than %qs.", metag_cpu_string, METAC_DEFAULT);
                  break;
                }
              metac_target = cpu_table[i].cpu;
              metacore     = cpu_table[i].tune;
              break;
            }

          if (strcmp (METAC_DEFAULT, cpu_table[i].name) == 0)
            newer_than_default = true;
        }

      if (cpu_table[i].name == NULL)
        error ("Bad value %qs for -mmetac switch", metag_cpu_string);
    }
  
  if (metag_tune_string)
    {
      unsigned int i;

      for (i = 0; cpu_table[i].name != NULL; i++)
        if (strcmp (metag_tune_string, cpu_table[i].name) == 0)
          {
            metacore = cpu_table[i].tune;
            break;
          }

      if (cpu_table[i].name == NULL)
        error ("bad value %qs for -mtune switch", metag_tune_string);
    }

  if (TARGET_MTX)
    {
      if (metac_target != METAC_1_2_ID)
        error ("MTX is based on a Meta 1.2 core, use -mmetac=1.2");
    } 

  if (TARGET_MINIM_CORE)
    metag_max_insns_skipped = 2;

  if (TARGET_FPU_SIMD && (!TARGET_FPU
                          || metag_fpu_single))
    error ("-msimd-float only valid with -mhard-float=D");

  if (!TARGET_METAC_2_1 && TARGET_FPU)
    error ("FPU not available on specified processor: %s", metag_cpu_string);

  if (!TARGET_METAC_2_1 && metag_meta2_bex_enabled)
    error ("The 'bex' extension is not available for the specified meta core");

  metag_override_options_per_os();
  flag_no_function_cse = 1;
}

bool
metag_return_in_memory (tree type)
{
  HOST_WIDE_INT size = int_size_in_bytes (type);

  return (size < 0 || size > UNITS_PER_WORD * 2);
}

void
metag_abort (rtx val)
{
  debug_rtx (val);
  gcc_unreachable ();
}

static void
metag_emit_load_post_inc (rtx dstbase, enum machine_mode mode, rtx dst,
                          HOST_WIDE_INT dstoffset, unsigned int reg)
{
#if 0
  rtx addrm mem, insn;

  addr = gen_rtx_POST_INC (Pmode, dst);
  mem  = adjust_automodify_address_nv (dstbase, mode, addr, dstoffset);
  insn = emit_insn (gen_rtx_SET (VOIDmode,
                                 mem,
                                 gen_rtx_REG (mode, reg)));
  REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (addr, 0), REG_NOTES (insn));
#else
  rtx plus, mem, insn, set1, set2;

  mem  = adjust_automodify_address_nv (dstbase, mode, dst, dstoffset);

  dst  = XEXP (mem, 0);
  plus = gen_rtx_PLUS (SImode, dst,  
                       gen_int_mode (GET_MODE_SIZE (mode), SImode));
  set1 = gen_rtx_SET (VOIDmode, mem, gen_rtx_REG (mode, reg));
  set2 = gen_rtx_SET (VOIDmode, dst, plus);

  insn = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (2));
  XVECEXP (insn, 0, 0) = set1;
  XVECEXP (insn, 0, 1) = set2;
  emit_insn (insn);
#endif
}

rtx
metag_gen_load_multiple (unsigned int      base_regno,
                         unsigned int      count,
                         enum machine_mode mode,
                         rtx               from,
                         bool              write_back,
                         rtx               basemem,
                         HOST_WIDE_INT *   offsetp)
{
  HOST_WIDE_INT offset    = *offsetp;
  unsigned int  i;
  unsigned int  word_size = GET_MODE_SIZE (mode);
  unsigned int  j;
  rtx           result;

  if (count < 2)
    gcc_unreachable ();

  result = gen_rtx_PARALLEL (VOIDmode,
                             rtvec_alloc (count + (write_back ? 1 : 0)));
  if (write_back)
    {
      XVECEXP (result, 0, 0) = gen_rtx_SET (VOIDmode,
                                            from,
                                            plus_constant (from, count * word_size));
      i = 1;
      count++;
    }
  else
    i = 0;

  for (j = 0; i < count; i++, j++)
    {
      rtx addr = plus_constant (from, j * word_size);
      rtx mem  = adjust_automodify_address_nv (basemem, mode, addr, offset);

      XVECEXP (result, 0, i) = gen_rtx_SET (VOIDmode,
                                            gen_rtx_REG (mode, base_regno + (j * 2)),
                                            mem);

      offset += word_size;
    }

  if (write_back)
    *offsetp = offset;

  return result;
}

rtx
metag_gen_store_multiple (unsigned int      base_regno,
                          unsigned int      count,
                          enum machine_mode mode,
                          rtx               to,
                          bool              write_back,
                          rtx               basemem,
                          HOST_WIDE_INT *   offsetp)
{
  HOST_WIDE_INT offset    = *offsetp;
  unsigned int  i;
  unsigned int  word_size = GET_MODE_SIZE (mode);
  unsigned int  j;
  rtx           result;

  if (count < 2)
    gcc_unreachable ();

  result = gen_rtx_PARALLEL (VOIDmode,
                             rtvec_alloc (count + (write_back ? 1 : 0)));

  if (write_back)
    {
      XVECEXP (result, 0, 0) = gen_rtx_SET (VOIDmode,
                                            to,
                                            plus_constant (to, count * word_size));
      i = 1;
      count++;
    }
  else
    i = 0;

  for (j = 0; i < count; i++, j++)
    {
      rtx addr = plus_constant (to, j * word_size);
      rtx mem  = adjust_automodify_address_nv (basemem, mode, addr, offset);

      XVECEXP (result, 0, i) = gen_rtx_SET (VOIDmode,
                                            mem,
                                            gen_rtx_REG (mode, base_regno + (j * 2)));
      offset += word_size;
    }

  if (write_back)
    *offsetp = offset;

  return result;
}

bool
metag_gen_movmemqi (rtx operands[])
{
  HOST_WIDE_INT     xfer_bytes_to_go; /* # bytes to xfer in big chunks */
  HOST_WIDE_INT     last_bytes;       /* # bytes left over */
  HOST_WIDE_INT     srcoffset;
  HOST_WIDE_INT     dstoffset;
  enum machine_mode word_mode;        /* mode in which to do the transfer */
  unsigned int      word_size;        /* units in which to transfer */
  unsigned int      do_in_bytes = 0;
  unsigned int      do_in_max;        /* max # of bytes to do in on go */
  rtx               src;
  rtx               dst;
  rtx               dstbase;
  rtx               srcbase;

  if (   !CONST_INT_P (operands[2]) 
      || !CONST_INT_P (operands[3])
      || INTVAL (operands[2]) > 96
      || INTVAL (operands[2]) < 8
      || INTVAL (operands[3]) & 3)
    return false;

  dstbase = operands[0];
  srcbase = operands[1];

  dst = copy_addr_to_reg (XEXP (dstbase, 0));
  src = copy_addr_to_reg (XEXP (srcbase, 0));
  if ((INTVAL (operands[3]) & 7) == 0
      && INTVAL (operands[2]) >= 16)
    {
      /* Data 64-bit aligned with enough for two transfers */
      xfer_bytes_to_go  = INTVAL (operands[2]) & ~(UNITS_PER_WORD * 2 - 1);
      word_mode         = DImode;
      word_size         = UNITS_PER_WORD * 2;
      last_bytes        = INTVAL (operands[2]) & (UNITS_PER_WORD * 2 - 1);
      do_in_max         = 4 * word_size;

      /* Cannot use 64-bit mode if it's too short! */
      gcc_assert (xfer_bytes_to_go >= 16);
    }
  else
    {
      /* Data not 64-bit aligned, used paired bursts so still 64-bit organised */
      xfer_bytes_to_go  = INTVAL (operands[2]) & ~(UNITS_PER_WORD - 1);
      word_mode         = SImode;
      word_size         = UNITS_PER_WORD;
      last_bytes        = INTVAL (operands[2]) & (UNITS_PER_WORD - 1);
      do_in_max         = 5 * word_size;

      if ((xfer_bytes_to_go + last_bytes) > 64)
        return false;
    }

  gcc_assert (do_in_max > 0);

  dstoffset = srcoffset = 0;

  while (xfer_bytes_to_go > 0)
    {
      unsigned int first_regno;

      /* Burst loop, transfer upto 32 or 20 bytes each */
      do_in_bytes = xfer_bytes_to_go;

      /* On the input side bytes at the end count as an extra word */
      if (last_bytes != 0)
        do_in_bytes += word_size;

      if (do_in_bytes >= do_in_max * 2)
        {
          /* At least two full bursts left */
          do_in_bytes = do_in_max;
        }
      else if (do_in_bytes > do_in_max)
        {
          /* Don't leave a runt at the end */
          do_in_bytes |=  (word_size * 2) - 1;
          do_in_bytes >>= 1;
          do_in_bytes += 1;
        }
#if 0
      first_regno = D0_0_REG;
#else
      first_regno = D0_2_REG;
      if (D0_7_REG - 2 * ((do_in_bytes / word_size)-1) < first_regno)
        first_regno = D0_7_REG - 2 * ((do_in_bytes / word_size)-1);
#endif

      /* Load the data */
      emit_insn (metag_gen_load_multiple (first_regno, do_in_bytes / word_size,
                                          word_mode, src, true,
                                          srcbase, &srcoffset));

      if (xfer_bytes_to_go < (int)do_in_bytes)
        {
          /* Replace the rounded up size with the real extra size */
          do_in_bytes -= word_size;
          xfer_bytes_to_go = 0;
        }
      else
        {
          /* Reduce bytes to go */
          xfer_bytes_to_go -= do_in_bytes;
        }

      /* Store the data */
      if (do_in_bytes >= 2 * word_size)
        emit_insn (metag_gen_store_multiple (first_regno, do_in_bytes / word_size,
                                             word_mode, dst, true,
                                             dstbase, &dstoffset));
      else if (do_in_bytes >= word_size)
        {
          metag_emit_load_post_inc (dstbase, word_mode, dst,
                                    dstoffset, first_regno);
          dstoffset += word_size;
        }
      else
        gcc_unreachable ();
    }

  /* Are we finished? */
  if (last_bytes > 0)
    {
#if 0
      unsigned int last_regno = D0_0_REG + 2 * (do_in_bytes / word_size);
#else
      unsigned int last_regno = D0_2_REG + 2 * (do_in_bytes / word_size);
      if (D0_7_REG - 2 * (do_in_bytes / word_size) < D0_2_REG)
        last_regno -= D0_2_REG - (D0_7_REG - 2 * (do_in_bytes / word_size));
#endif

      /* Here we handle and remaining 4-byte chunks left to xfer */ 
      if ((last_bytes & 4) != 0)
        {
          gcc_assert (word_size == UNITS_PER_WORD * 2);
          /* Store last 32-bit word and switch remainder into SImode */

          /* Generate write for bottom SImode subreg of last DImode register,
             Further manipulation needed on upper part of DImode register */
          metag_emit_load_post_inc (dstbase, SImode, dst,
                                    dstoffset, last_regno);

          dstoffset += 4;

          /* If we have any remaing 2-byte or 1-byte chunks left adjust dst */
          if ((last_bytes & 3) != 0)
            last_regno = last_regno + 1;
        }

      /* Here we handle any remaining 2-byte chunks left to xfer */
      if ((last_bytes & 2) != 0)
        {
          metag_emit_load_post_inc (dstbase, HImode, dst,
                                    dstoffset, last_regno);

          dstoffset += 2;

          /* If we have any remaing 1-byte chunks left adjust dst */
          if ((last_bytes & 1) != 0)
            {
              rtx tmp = gen_rtx_REG (SImode, last_regno);

              emit_insn (gen_lshrsi3 (tmp, tmp, GEN_INT (16)));
            }
        }

      /* Here we handle any remaining 1-byte chunks left to xfer */
      if ((last_bytes & 1) != 0)
        {
          metag_emit_load_post_inc (dstbase, QImode, dst,
                                    dstoffset, last_regno);

          dstoffset += 1;
        }
    }

  return true;
}

#if METAG_DEBUG_CCEXEC
static const char *
attr_cond_name (enum attr_cond attr)
{
  switch (attr)
    {
    case COND_YES:
      return "yes";
    case COND_NO:
      return "no";
    default:
      break;
    }

  return "??";
}

static const char *
attr_ccstate_name (enum attr_ccstate attr)
{
  switch (attr)
    {
    case CCSTATE_XCC:
      return "xcc";
    case CCSTATE_SET:
      return "set";
    case CCSTATE_FASTSET:
      return "fastset";
    case CCSTATE_FASTFASTSET:
      return "fastfastset";
    case CCSTATE_CCX:
      return "ccx";
    case CCSTATE_NCC:
      return "ncc";
    default:
      break;
    }

  return "???";
}

static const char *
attr_predicable_name (enum attr_predicable attr)
{
  switch (attr)
    {
    case PREDICABLE_YES:
      return "yes";
    case PREDICABLE_NO:
      return "no";
    default:
      break;
    }

  return "??";
}
#endif


/* The state of the fsm controlling condition codes are:
   0: normal, do nothing special
   1: make ASM_OUTPUT_OPCODE not output this instruction
   2: make ASM_OUTPUT_OPCODE not output this instruction
   3: make instructions conditional
   4: make instructions conditional

   State transitions (state->state by whom under condition):
   0 -> 1 final_prescan_insn if the `target' is a label
   0 -> 2 final_prescan_insn if the `target' is an unconditional branch
   1 -> 3 ASM_OUTPUT_OPCODE after not having output the conditional branch
   2 -> 4 ASM_OUTPUT_OPCODE after not having output the conditional branch
   3 -> 0 ASM_OUTPUT_INTERNAL_LABEL if the `target' label is reached
          (the target label has CODE_LABEL_NUMBER equal to metag_target_label).
   4 -> 0 final_prescan_insn if the `target' unconditional branch is reached
          (the target insn is metag_target_insn).

   If the jump clobbers the conditions then we use states 2 and 4.

   A similar thing can be done with conditional return insns.

   XXX In case the `target' is an unconditional branch, this conditionalising
   of the instructions always reduces code size, but not always execution
   time.  But then, I want to reduce the code size to somewhere near what
   /bin/cc produces.  */

void
metag_final_prescan_insn (rtx insn)
{
  /* BODY will hold the body of INSN.  */
  rtx body = PATTERN (insn);

  /* This will be 1 if trying to repeat the trick, and things need to be
     reversed if it appears to fail.  */
  int reverse = 0;

  /* START_INSN will hold the insn from where we start looking.  This is the
     first insn after the following code_label if REVERSE is true.  */
  rtx start_insn = insn;

  /* If in state 4, check if the target branch is reached, in order to
     change back to state 0.  */
  if (metag_ccfsm_state == 4)
    {
      if (insn == metag_target_insn)
        {
          metag_target_insn = NULL_RTX;
          metag_ccfsm_state = 0;
        }
      return;
    }

  /* If in state 3, it is possible to repeat the trick, if this insn is an
     unconditional branch to a label, and immediately following this branch
     is the previous target label which is only used once, and the label this
     branch jumps to is not too far off.  */
  if (metag_ccfsm_state == 3)
    {
      if (simplejump_p (insn))
        {
          start_insn = next_nonnote_insn (start_insn);
          if (BARRIER_P (start_insn))
            {
              /* XXX Isn't this always a barrier?  */
              start_insn = next_nonnote_insn (start_insn);
            }

          if (GET_CODE (start_insn) == CODE_LABEL
              && CODE_LABEL_NUMBER (start_insn) == metag_target_label
              && LABEL_NUSES (start_insn) == 1)
            reverse = true;
          else
            return;
        }
      else
        return;
    }

  if (metag_ccfsm_state != 0 && !reverse)
    gcc_unreachable ();

  if (!JUMP_P (insn))
    return;

  /* This jump might be paralleled with a clobber of the condition codes 
     the jump should always come first */
  if (GET_CODE (body) == PARALLEL && XVECLEN (body, 0) > 0)
    body = XVECEXP (body, 0, 0);

  /* If this jump uses the hardware loop counter, leave it alone */
  if (reg_mentioned_p (gen_rtx_REG (VOIDmode, TXRPT_REGNUM), body))
    return;

  /* If this is a conditional return then we don't want to know */
  if (GET_CODE (body) == SET && GET_CODE (SET_DEST (body)) == PC
      && GET_CODE (SET_SRC (body)) == IF_THEN_ELSE
      && (GET_CODE (XEXP (SET_SRC (body), 1)) == RETURN
          || GET_CODE (XEXP (SET_SRC (body), 2)) == RETURN))
    return;

  if (reverse
      || (GET_CODE (body) == SET && GET_CODE (SET_DEST (body)) == PC
          && GET_CODE (SET_SRC (body)) == IF_THEN_ELSE))
    {
      int insns_skipped = 0;
      int fail = 0;
      int quit = false, succeed = false;
      /* Flag which part of the IF_THEN_ELSE is the LABEL_REF.  */
      int then_not_else = true;
      rtx this_insn = start_insn;
      rtx  label = NULL_RTX;

      /* Register the insn jumped to.  */
      if (reverse)
        label = XEXP (SET_SRC (body), 0);
      else if (LABEL_REF_P (XEXP (SET_SRC (body), 1)))
        label = XEXP (XEXP (SET_SRC (body), 1), 0);
      else if (LABEL_REF_P (XEXP (SET_SRC (body), 2)))
        {
          label = XEXP (XEXP (SET_SRC (body), 2), 0);
          then_not_else = false;
        }
      else
        gcc_unreachable ();

#if METAG_DEBUG_CCEXEC
      fprintf (stderr, "CE =====\n"); debug_rtx (insn);
#endif
      /* See how many insns this branch skips, and what kind of insns.  If all
         insns are okay, and the label or unconditional branch to the same
         label is not too far away, succeed.  */
      while (!quit && !succeed && insns_skipped < metag_max_insns_skipped)
        {
          this_insn = next_nonnote_insn (this_insn);
          if (!this_insn)
            break;

          /* Only count recognised instructions others aren't relevant.*/

          if (INSN_P (this_insn) && INSN_CODE (this_insn) >= 0)
            insns_skipped++;

          switch (GET_CODE (this_insn))
            {
            case CODE_LABEL:
              /* Succeed if it is the target label, otherwise fail since
                 control falls in from somewhere else.  */
              if (this_insn == label)
                {
                  if (fail == 0)
                    {
                      metag_ccfsm_state = 1;
                      succeed = true;
#if METAG_DEBUG_CCEXEC
                      fprintf (stderr, "CE found label after %d successes\n", insns_skipped);
#endif
                    }
                  else
                    {
#if METAG_DEBUG_CCEXEC
                      fprintf (stderr, "CE found label after %d failures\n", fail);
#endif
                      quit = true;
                    }
                }
              else
                {
#if METAG_DEBUG_CCEXEC
                  fprintf (stderr, "CE failed CODE_LABEL after %d insns\n", insns_skipped);
#endif
                  fail++;
                }

              break;

            case BARRIER:
              /* Succeed if the following insn is the target label.
                 Otherwise fail.  
                 If return insns are used then the last insn in a function 
                 will be a barrier. */
              this_insn = next_nonnote_insn (this_insn);
              if (this_insn && this_insn == label)
                {
                  if (fail == 0)
                    {
                      metag_ccfsm_state = 1;
                      succeed = true;
#if METAG_DEBUG_CCEXEC
                      fprintf (stderr, "CE found label after %d successes\n", insns_skipped);
#endif
                    }
                  else
                    {
#if METAG_DEBUG_CCEXEC
                      fprintf (stderr, "CE found label after %d failures\n", fail);
#endif
                      quit = true;
                    }
                }
              else
                {
#if METAG_DEBUG_CCEXEC
                  fprintf (stderr, "CE failed BARRIER after %d insns\n", insns_skipped);
#endif
                  fail++;
                }

              break;

            case CALL_INSN:
#if 0
              if (!(GET_CODE (operands[?]) == REG
                    && REGNO (operands[?]) != RETURN_POINTER_REGNUM))
#endif
                {
#if METAG_DEBUG_CCEXEC
                  fprintf (stderr, "CE failed CALL_INSN after %d insns\n", insns_skipped);
                  debug_rtx (this_insn);
#endif
                  fail++;
                }
              break;

            case JUMP_INSN:
              /* If this is an unconditional branch to the same label, succeed.
                 If it is to another label, do nothing.  If it is conditional,
                 fail.  */
              /* XXX Probably, the tests for SET and the PC are unnecessary. */

              {
                rtx scanbody = PATTERN (this_insn);

                if (GET_CODE (scanbody) == SET
                    && GET_CODE (SET_DEST (scanbody)) == PC)
                  {
                    if (LABEL_REF_P (SET_SRC (scanbody))
                        && XEXP (SET_SRC (scanbody), 0) == label && !reverse)
                      {
                        if (fail == 0)
                          {
                            metag_ccfsm_state = 2;
                            succeed = true;
#if METAG_DEBUG_CCEXEC
                            fprintf (stderr, "CE found jump to target label after %d successes\n", insns_skipped);
#endif
                          }
                        else
                          {
#if METAG_DEBUG_CCEXEC
                            fprintf (stderr, "CE found uncond branch after %d failures\n", fail);
#endif
                            quit = true;
                          }
                      }
                    else if (GET_CODE (SET_SRC (scanbody)) == IF_THEN_ELSE)
                      {
#if METAG_DEBUG_CCEXEC
                        fprintf (stderr, "CE failed JUMP_INSN IF_THEN_ELSE after %d insn\n", insns_skipped);
                        debug_rtx (this_insn);
#endif
                        fail++;
                      }
                  }
                else if (GET_CODE (scanbody) == RETURN)
                  {
                    if (!metag_cheap_return (true))
                      {
#if METAG_DEBUG_CCEXEC
                        fprintf (stderr, "CE failed JUMP_INSN RETURN (not cheap) after %d insn\n", insns_skipped);
                        debug_rtx (this_insn);
#endif
                        fail++;
                      }
                  }
                else
                  {
#if METAG_DEBUG_CCEXEC
                    fprintf (stderr, "CE failed JUMP_INSN not recognised after %d insn\n", insns_skipped);
                    debug_rtx (this_insn);
#endif
                    fail++;
                  }
              }

              break;

            case INSN:
              if (INSN_CODE (this_insn) >= 0)
                {
                  enum attr_cond       cond_attr       = get_attr_cond (this_insn);
                  enum attr_predicable predicable_attr = get_attr_predicable (this_insn);
                  enum attr_ccstate    ccstate_attr    = get_attr_ccstate (this_insn);

                  /* A predicated instruction can't be COND_EXEC unless
                     the predication condition matches the condexec
                     condition. At present we ALWAYS reject predicated
                     instructions which is safe but sub-optimal.  */
                  if (predicable_attr == PREDICABLE_YES && GET_CODE (PATTERN (this_insn)) == COND_EXEC)
                    {
#if METAG_DEBUG_CCEXEC
                      fprintf (stderr, "CE failed INSN PREDICABLE=%s and COND_EXEC after %d insns\n",
                               attr_predicable_name (predicable_attr),
                               insns_skipped);
                      debug_rtx (this_insn);
#endif
                      fail++;
                    }

                  /* Only insns which don't modify CC can be cond-exec */
                  if (cond_attr != COND_YES || ccstate_attr != CCSTATE_NCC)
                    {
#if METAG_DEBUG_CCEXEC
                      fprintf (stderr, "CE failed INSN COND=%s CSTATE=%s after %d insns\n",
                               attr_cond_name (cond_attr),
                               attr_ccstate_name (ccstate_attr),
                               insns_skipped);
                      debug_rtx (this_insn);
#endif
                      fail++;
                    }
                }
              else if (asm_noperands (PATTERN (this_insn)) >= 0)
                {
#if METAG_DEBUG_CCEXEC
                  fprintf (stderr, "CE failed ASM_OPERANDS after %d insns\n",
                           insns_skipped);
                  debug_rtx (this_insn);
#endif
                  fail++;
                }
              else if (GET_CODE (PATTERN (this_insn)) != USE)
                {
                  /* Anything else not recognised with the exception of
                     an USE (typically an USE of return register) fails
                     conditional execution. */
#if METAG_DEBUG_CCEXEC
                  fprintf (stderr, "CE failed unrecognised after %d insns\n",
                           insns_skipped);
                  debug_rtx (this_insn);
#endif
                  fail++;
                }

              break;

            default:
              break;
            }
        }

      if (succeed)
        {
          if (metag_ccfsm_state == 1 || reverse)
            metag_target_label = CODE_LABEL_NUMBER (label);
          else if (metag_ccfsm_state == 2)
            {
              while (this_insn && GET_CODE (PATTERN (this_insn)) == USE)
                {
                  this_insn = next_nonnote_insn (this_insn);
                  if (this_insn
                      && (BARRIER_P (this_insn)
                          || GET_CODE (this_insn) == CODE_LABEL))
                    gcc_unreachable ();
                }

              if (!this_insn)
                {
                  /* Oh, dear! we ran off the end.. give up */
                  if (INSN_CODE (insn) >= 0)
                    {
                      recog (PATTERN (insn), insn, NULL);
                      cleanup_subreg_operands (insn);
                    }

                  metag_ccfsm_state = 0;
                  metag_target_insn = NULL_RTX;
                  return;
                }

              metag_target_insn = this_insn;
            }
          else
            gcc_unreachable ();

          /* If REVERSE is true, METAG_CURRENT_CC needs to be inverted
           * from what it was.  */
          if (!reverse)
            metag_current_cc = get_metag_cc (XEXP (SET_SRC (body), 0));

          if (reverse || then_not_else)
            metag_current_cc = metag_inv_cc [metag_current_cc];
        }

      if (INSN_CODE (insn) >= 0)
        {
          /* restore recog_operand (getting the attributes of other insns can
             destroy this array, but final.c assumes that it remains intact
             across this call; since the insn has been recognized already we
             call recog direct). */
          recog (PATTERN (insn), insn, NULL);
          cleanup_subreg_operands (insn);
        }
    }
}

bool
metag_cond_exec_p (void)
{
  return (metag_ccfsm_state == 3 || metag_ccfsm_state == 4);
}

void
metag_print_cc_if_conditional (FILE *stream)
{
  if (metag_cond_exec_p ())
    {
      gcc_assert (current_insn_predicate == NULL_RTX);
      fputs (metag_cc_names[metag_current_cc], stream);
    }
  else if (current_insn_predicate != NULL_RTX)
    fputs (metag_cc_names[get_metag_cc (current_insn_predicate)], stream);

  return;
}

bool
metag_consume_branch (rtx insn ATTRIBUTE_UNUSED)
{
  if (metag_ccfsm_state == 1 || metag_ccfsm_state == 2)
    {
      metag_ccfsm_state += 2;
      return true;
    }

  return false;
}


/* Output to FILE code to call mcount. */
void
metag_function_profiler (FILE *file)
{
  if (1 || !TARGET_METAC_1_1)
    {
      fprintf (file, "\tMOVT\t%s, #HI(_mcount_wrapper)\n",
               reg_names[TEMP_D0FRT_REGNUM]);
      fprintf (file, "\tCALL\t%s, #LO(_mcount_wrapper)\n",
               reg_names[TEMP_D0FRT_REGNUM]);
    }
  else
    fprintf (file, "\tCALLR\t%s, _mcount_wrapper\n",
             reg_names[TEMP_D0FRT_REGNUM]);
}

static bool
metag_same_reg_p (rtx reg1, rtx reg2, bool strict)
{
  unsigned int r1 = REGNO (reg1);
  unsigned int r2 = REGNO (reg2);

  if (IS_PSEUDO_REGNO (r1) && reg_renumber != NULL)
    r1 = reg_renumber[r1];

  if (IS_PSEUDO_REGNO (r2) && reg_renumber != NULL)
    r2 = reg_renumber[r2];

  return strict ? IS_HARD_OR_VIRT_REGNO (r1) && IS_HARD_OR_VIRT_REGNO (r2) && r1 == r2
                : r1 != INVALID_REGNUM       && r1 != INVALID_REGNUM       && r1 == r2;
}

static bool
metag_regs_same_regclass_p (rtx reg1, rtx reg2, bool strict)
{
  unsigned int   r1 = REGNO (reg1);
  unsigned int   r2 = REGNO (reg2);
  enum reg_class class1;
  enum reg_class class2;

  if (IS_PSEUDO_REGNO (r1) && reg_renumber != NULL)
    r1 = reg_renumber[r1];

  if (IS_PSEUDO_REGNO (r2) && reg_renumber != NULL)
    r2 = reg_renumber[r2];

  class1 = METAG_REGNO_REG_CLASS (r1);
  class2 = METAG_REGNO_REG_CLASS (r2);

  return strict ? class1 != NO_REGS && class2 != NO_REGS && class1 == class2
                : class1 != NO_REGS && class2 != NO_REGS && class1 == class2;
}

bool
metag_same_regclass_p (rtx reg1, rtx reg2)
{
  return metag_regs_same_regclass_p (reg1, reg2, true);
}

/* Return true iff the registers are in the same function unit
   (i.e. D0, D1, A0, A1, CTRL).  */

bool
metag_regno_same_unit_p (unsigned int regno1, unsigned int regno2)
{
  enum reg_class class1 = METAG_REGNO_REG_CLASS (regno1);
  enum reg_class class2 = METAG_REGNO_REG_CLASS (regno2);

  return class1 != NO_REGS && class2 != NO_REGS && class1 == class2;
}

/*  (post_modify (REG ...)
                 (plus (REG ...)
                       (REG ...)))
    or

    (post_modify (REG  ...)
                 (plus (REG ...)
                       (CONST_INT ...)))
*/
bool
metag_legitimate_modify_p (rtx addr, enum machine_mode mode, bool strict)
{
  rtx op0 = XEXP (addr, 0);
  rtx op1 = XEXP (addr, 1);
  rtx op2;
  rtx op3;

  if (GET_CODE (op1) != PLUS)
    return false;

  if (!METAG_LEGITIMATE_REG_P (op0, strict))
    return false;

  op2 = XEXP (op1, 0);
  op3 = XEXP (op1, 1);
  if (!METAG_LEGITIMATE_REG_P (op2, strict))
    return false;
 
  if (!metag_same_reg_p (op0, op2, strict))
    return false;

  if (REG_P (op3))
    return METAG_LEGITIMATE_TWIN_P (op2, op3, mode, strict);

  if (CONST_INT_P (op3) && metag_offset6_mode (op3, mode))
    return true;

  return false;
}

long
metag_const_double_to_hp (rtx op, bool *inexact)
{
  REAL_VALUE_TYPE rv;
  long            half = 0;
  bool            dummy_inexact;

  if (!inexact)
    inexact = &dummy_inexact;

  REAL_VALUE_FROM_CONST_DOUBLE (rv, op);

  *inexact = false;

  if (GET_MODE (op) == SFmode)
    {
      long tgsingle;
      bool tgsgn;
      long tgexp;
      long tgman;

      REAL_VALUE_TO_TARGET_SINGLE (rv, tgsingle);

      /* Split the parts */
      tgsgn = ((tgsingle & 0x80000000ul) != 0);
      tgexp = (tgsingle & 0x7F800000) >> 23;
      tgman =  tgsingle & 0x007FFFFF;

      /* If the fractional part would need rounding, raise inexact */
      if (tgman & 0x00001FFF)
        *inexact = true;

      /* Convert to HF (truncate) */

    /* Exp == MAX we must preserve the Inf or NaN */
      if (tgexp == 0xFF)
        half = tgman ? 0x7C01 : 0x7C00;
      /* Exp == 0 is special we must not bias adjust it */
      else if (tgexp == 0x00)
        half = tgman >> (23 - 10);
      else
        {
          tgexp -= 127; /* Remove SF bias */
          tgexp += 15;  /* Add HF bias */
          tgman >>= 23 - 10;

          if (tgexp >= 32)
            {
              *inexact = true;
              half = 0x7C00; /* Overflow to inf */
            }
          else if (tgexp < 0)
            {
              *inexact = true;
              half = 0x0000; /* Underflow to 0 */
            }
          else
            half = (tgexp & 0x01F) << 10
                 | (tgman & 0x3FF);
        }

      /* Copy the sign */
      if (tgsgn)
        half |= 0x8000;
    }
  else if (GET_MODE (op) == DFmode)
    {
      long tgdouble[2];
      bool tgsgn;
      long tgexp;
      long tgman[2];

      REAL_VALUE_TO_TARGET_DOUBLE (rv, tgdouble);

      /* Split the parts */
      tgsgn    = ((tgdouble[1] & 0x80000000ul) != 0);
      tgexp    = (tgdouble[1] & 0x7FF00000) >> (52-32);
      tgman[1] = (tgdouble[1] & 0x000FFFFF);
      tgman[0] =  tgdouble[0];

      /* If the fractional part would need rounding, reject */
      if (tgman[1] & 0x0000FFFF || tgman[0])
        *inexact = true;

      /* Convert to HF (truncate) */

      /* Exp == MAX we must preserve the Inf or NaN */
      if (tgexp == 0x7FF)
        half = tgman[1] || tgman[0] ? 0x7C01 : 0x7C00;
      /* Exp == 0 is special we must not bias adjust it */
      else if (tgexp == 0x00)
        half = tgman[1] >> (52 - 10 - 32);
      else
      {
        tgexp -= 1023; /* Remove SF bias */
        tgexp += 15;  /* Add HF bias */
        tgman[0] = tgman[1] >> (52 - 10 - 32);
        tgman[1] = 0;

        if (tgexp >= 32)
          {
            *inexact = true;
            half = 0x7C00; /* Overflow to inf */
          }
        else if (tgexp < 0)
          {
            *inexact = true;
            half = 0x0000; /* Underflow to 0 */
          }
        else
          half = (tgexp    & 0x01F) << 10
               | (tgman[0] & 0x3FF);
      }

      /* Copy the sign */
      if (tgsgn)
        half |= 0x8000;
    }

  return half;
}

void
metag_print_operand (FILE * file, rtx op, enum rtx_code code)
{
  if (code == '?')
    metag_print_cc_if_conditional (file);
  else if (code == '@')
    fputs (ASM_COMMENT_START, file);
  else if (code == 'z')
    fputs (metag_cc_names[get_metag_cc (op)], file);
  else if (code == 'Z')
    fputs (metag_cc_names[metag_inv_cc [get_metag_cc (op)]], file);
  else if (code == 'h')
    {
      if (GET_CODE (op) == CONST_VECTOR)
        {
          gcc_assert (GET_MODE_INNER (GET_MODE (op)) == SFmode);
          gcc_assert (rtx_equal_p (CONST_VECTOR_ELT (op, 0),
                                   CONST_VECTOR_ELT (op, 1)));
          op = CONST_VECTOR_ELT (op, 0);
        }
      fprintf (file, "0x%04lX", metag_const_double_to_hp (op, NULL));
    }
  else if (REG_P (op))
    {
      if (code == 't')
        fputs (reg_names[REGNO (op) + 1], file);
      else
        fputs (reg_names[REGNO (op)], file);
    }
  else
    {
      if (MEM_P (op))
        {
          if (SYMBOL_REF_P (XEXP (op, 0)))
            {
              /* Abort if we're about to generate #OG addressing. */
              debug_rtx (op);
              gcc_unreachable ();
            }
          else
            output_address (op);
        }
      else
        {
          if (CONST_DOUBLE_P (op) && GET_MODE (op) == SFmode)
            {
              long value = metag_const_double_sfmode (op);

              if (code != 'c')
                fputc ('#', file);
              fprintf (file, "0x%08lx", value);
            }
          else
            {
              if (code != 'c')
                {
                  rtx itemp = op;

                  fputc ('#', file);
                  if (CONST_INT_P (op)
                      && (INTVAL (op) < -32768 || INTVAL (op) > 0x0000FFFF))
                    itemp = GEN_INT (((INTVAL (op)) >> 16) & 0x0000FFFF);

                  output_addr_const (file, itemp);
                }
              else if (CONST_INT_P (op)
                       && (INTVAL (op) < -32768 || INTVAL (op) > 0x0000FFFF))
                fprintf (file, "0x%08lx", (long)INTVAL (op));
              else
                output_addr_const (file, op);
            }
        }
    }
}

static void
metag_output_pic_addr_const (FILE *file, rtx addr)
{
  output_addr_const (file, XVECEXP (addr, 0, 0));
  switch (XINT (addr, 1))
    {
    case UNSPEC_GOT:
      fputs ("@GOT", file);
      break;
    case UNSPEC_GOTOFF:
      fputs ("@GOTOFF", file);
      break;
    case UNSPEC_PLT:
      fputs ("@PLT", file);
      break;
    default:
      metag_abort (addr);
      break;
    }
}

void
metag_print_operand_address (FILE *file, rtx op)
{
  rtx addr = MEM_P (op) ? XEXP (op, 0) : op;
  rtx offset;
  rtx reg;
  int inc;

  switch (GET_CODE (addr))
    {
    case SYMBOL_REF:
      /* Abort if we're about to generate #OG addressing. */
      gcc_unreachable ();
      break;
    case REG:
      fprintf (file, "[%s]", reg_names[REGNO (addr)]);
      break;
    case PRE_INC:
      reg = XEXP (addr, 0);
      inc = GET_MODE_SIZE (GET_MODE (op));
      fprintf (file, "[%s ++#%d]", reg_names[REGNO (reg)], inc);
      break;
    case POST_INC:
      reg = XEXP (addr, 0);
      inc = GET_MODE_SIZE (GET_MODE (op));
      fprintf (file, "[%s+#%d++]", reg_names[REGNO (reg)], inc);
      break;
    case PRE_DEC:
      reg = XEXP (addr, 0);
      inc = GET_MODE_SIZE (GET_MODE (op));
      fprintf (file, "[%s ++#(-%d)]", reg_names[REGNO (reg)], inc);
      break;
    case POST_DEC:
      reg = XEXP (addr, 0);
      inc = GET_MODE_SIZE (GET_MODE (op));
      fprintf (file, "[%s+#(-%d)++]", reg_names[REGNO (reg)], inc);
      break;
    case PRE_MODIFY:
      reg = XEXP (addr, 0);
      if (GET_CODE (XEXP (addr, 1)) != PLUS)
        metag_abort (op);
      else
        {
          rtx op0 = XEXP (XEXP (addr, 1), 0);

          if (!REG_P (op0))
            metag_abort (op);
          else if (REGNO (reg) != REGNO (op0))
            metag_abort (op);
          else
            {
              rtx op1 = XEXP (XEXP (addr, 1), 1);

              if (REG_P (op1))
                fprintf (file, "[%s++%s]",
                         reg_names[REGNO (op0)], reg_names[REGNO (op1)]);
              else if (CONST_INT_P (op1))
                fprintf (file, "[%s++#(%ld)]",
                         reg_names[REGNO (op0)], INTVAL (op1));
              else
                metag_abort (op);
            }
        }
      break;
    case POST_MODIFY:
      reg = XEXP (addr, 0);
      if (GET_CODE (XEXP (addr, 1)) != PLUS)
        metag_abort (op);
      else
        {
          rtx op0 = XEXP (XEXP (addr, 1), 0);

          if (!REG_P (op0))
            metag_abort (op);
          else if (REGNO (reg) != REGNO (op0))
            metag_abort (op);
          else
            {
              rtx op1 = XEXP (XEXP (addr, 1), 1);

              if (REG_P (op1))
                fprintf (file, "[%s+%s++]",
                         reg_names[REGNO (op0)], reg_names[REGNO (op1)]);
              else if (CONST_INT_P (op1))
                fprintf (file, "[%s+#(%ld)++]",
                         reg_names[REGNO (op0)], INTVAL (op1));
              else
                metag_abort (op);
            }
        }
      break;
    case PLUS:
      reg = XEXP (addr, 0);
      if (CONST_INT_P (reg))
        {
          offset = reg;
          reg = XEXP (addr, 1);
        }
      else
        offset = XEXP (addr, 1);

      if (!REG_P (reg))
        metag_abort (addr);

      if (REG_P (offset))
        fprintf (file, "[%s + %s]",
                 reg_names[REGNO (reg)], reg_names[REGNO (offset)]);
      else if (CONST_INT_P (offset))
        fprintf (file, "[%s + #%ld]", reg_names[REGNO (reg)], INTVAL (offset));
      else if (METAG_FLAG_PIC
               && reg == pic_offset_table_rtx
               && GET_CODE (offset) == CONST
               && GET_CODE (XEXP (offset, 0)) == UNSPEC
               && XVECLEN  (XEXP (offset, 0), 0) == 1
               && (XINT (XEXP (offset, 0), 1) == UNSPEC_GOT
                   || XINT (XEXP (offset, 0), 1) == UNSPEC_PLT))
        {
          fprintf (file, "[%s + #(", reg_names[REGNO (reg)]);
          metag_output_pic_addr_const (file, XEXP (offset, 0));
          fputs (")]", file);
        }
      else
        metag_abort (addr);
      break;
    default:
      if (CONSTANT_ADDRESS_P (addr))
        output_addr_const (file, addr);
      else
        metag_abort (addr);
      break;
    }
}

int
metag_arg_partial_bytes (CUMULATIVE_ARGS * cum, enum machine_mode mode, tree type, bool named)
{
  unsigned int acum;
  unsigned int aarg;
  unsigned int nbytes;

  /* variadic arguments a.k.a named are ALWAYS passed on the stack */

  if (!named)
    return 0;

  acum = ROUND_ADVANCE_CUM (cum->narg, mode, type);
  aarg = ROUND_ADVANCE_ARG (mode, type);

  nbytes = ((acum < MAX_METAG_PARM_BYTES && MAX_METAG_PARM_BYTES < (acum + aarg))
           ? (MAX_METAG_PARM_BYTES - acum)
           : 0);

  if (cum->partial == 0)
    {
      if (nbytes > 0)
        {
          int size   = METAG_ARG_SIZE (mode, type);
          int nstack = size - nbytes;

          gcc_assert ((nstack & (STACK_BOUNDARY_BYTES - 1)) == 0);
        }

      cum->partial = nbytes;
    }
  else
    gcc_assert (nbytes == 0);

  return nbytes;
}

bool
metag_pass_by_reference (CUMULATIVE_ARGS * cum ATTRIBUTE_UNUSED,
                         enum machine_mode mode ATTRIBUTE_UNUSED,
                         tree type ATTRIBUTE_UNUSED,
                         bool named ATTRIBUTE_UNUSED)
{
  return false;
}

static bool
metag_pass_in_reg (CUMULATIVE_ARGS * cum,
                   enum machine_mode mode,
                   tree type,
                   bool named ATTRIBUTE_UNUSED)
{
  int rcum;

  if (!named)
    return false;

  if (cum->narg >= MAX_METAG_PARM_BYTES)
    return false;

  rcum  = ROUND_ADVANCE_CUM (cum->narg, mode, type);
  if (rcum >= MAX_METAG_PARM_BYTES)
    return false; 

  return true;
}

bool
metag_must_pass_in_stack (enum machine_mode mode ATTRIBUTE_UNUSED,
                          tree type ATTRIBUTE_UNUSED)
{
  return false;
}

rtx
metag_function_arg (CUMULATIVE_ARGS * cum, enum machine_mode mode, tree type, bool named)
{
  bool pass_in_reg = metag_pass_in_reg (cum, mode, type, named);
  int  reg;

  if (!pass_in_reg)
    return NULL_RTX;

  reg = CALCULATE_REG (MAX_METAG_PARM_REGNUM, cum->narg, mode, type);
  if (reg < MIN_METAG_PARM_REGNUM)
    reg = MIN_METAG_PARM_REGNUM;

  return gen_rtx_REG (mode, reg);
}

void
metag_function_arg_advance (CUMULATIVE_ARGS * cum, enum machine_mode mode, tree type, bool named ATTRIBUTE_UNUSED)
{
  cum->narg = ROUND_ADVANCE_CUM (cum->narg, mode, type) + ROUND_ADVANCE_ARG (mode, type);
  return;
}

/* Define the offset between two registers, one to be eliminated,
   and the other its replacement, at the start of a routine.

   To kick things off we work out OFFSET as the size of the frame save
   area. Then we need to apply the following-

   ARG_POINTER   = STACK_POINTER      - (PRETEND + SAVE + PIC_SAVE + LOCAL + OUT_GOING)
   ARG_POINTER   = HARD_FRAME_POINTER - (PRETEND                                      )
   FRAME_POINTER = STACK_POINTER      - (        + SAVE + PIC_SAVE + LOCAL + OUT_GOING)
   FRAME_POINTER = HARD_FRAME_POINTER + (        + SAVE + PIC_SAVE                    )
 */

int
metag_initial_elimination_offset (int from, int to)
{
  /* This section of code and output_fn_prologue/epilogue
   * MUST agree on how the stack is going to be layedout.
   * Any discrepancy will result in wrong code being
   * generated.
   */

  HOST_WIDE_INT out_local_size   = get_frame_size ();
  bool          non_leaf         = metag_non_leaf_function_p ();
  unsigned int  savesize_gp      = 0;
  unsigned int  savesize_eh      = 0;
  unsigned int  FP_SP_offset     = 0;
  unsigned int  pic_save_size    = 0;
  unsigned int  pretend_size     = ALIGN_ON_STACK_BOUNDARY (current_function_pretend_args_size);
  unsigned int  extras_gp        = 0;
  unsigned int  extras_eh        = 0;
  unsigned int  ech_ctx          = 0;
  unsigned int  pretend_regs;
  int           delta;
  bool          loads_pic_register;

  if (pretend_size != 0)
    {
      /* Determine # register pairs needed for pretend args. */
      pretend_regs = pretend_size / UNITS_PER_WORD;
    }
  else
    pretend_regs = 0;

  out_local_size = ALIGN_ON_STACK_BOUNDARY (out_local_size + current_function_outgoing_args_size);

  /* Make pretend regs into the first non-varargs register number */
  pretend_regs += MIN_METAG_PARM_REGNUM;

  {
    unsigned int regno;

    for (regno = MIN_METAG_PARM_REGNUM;
         regno <= MAX_METAG_CSAVE_REGNUM;
         regno += 2)
      {
        if (regno < pretend_regs
            || (!call_used_regs[regno]
                && (df_regs_ever_live_p (regno + 0) || df_regs_ever_live_p (regno + 1))))
          {
            extras_gp |= REGNO_BIT (regno);
            savesize_gp += UNITS_PER_WORD * 2;

            if (regno >= MIN_METAG_CSAVE_REGNUM)
              FP_SP_offset += UNITS_PER_WORD * 2;
          }
      }
  }

  /* Adjust the saved registers for ECH support */
  ech_ctx = metag_adjust_savesize_ech (&savesize_gp, &extras_gp, &FP_SP_offset);

  if (current_function_calls_eh_return)
    {
      unsigned int n;

      for (n = 0; n < NUM_EH_RETURN_DATA_REGS; n++)
        {
          unsigned int regno = EH_RETURN_DATA_REGNO (n);

          if (regno != INVALID_REGNUM)
            {
              unsigned int regbit = REGNO_BIT (regno);

              if ((extras_eh & regbit) == 0)
                {
                  extras_eh |= regbit;
                  savesize_eh += UNITS_PER_WORD * 2;
                  FP_SP_offset += UNITS_PER_WORD * 2;
                }
            }
        }
    }

  if (frame_pointer_needed || non_leaf)
    {
      savesize_gp += UNITS_PER_WORD * 2, FP_SP_offset += UNITS_PER_WORD * 2;

      if (non_leaf)
        extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);

      if (frame_pointer_needed)
        extras_gp |= REGNO_BIT (TEMP_D0FRT_REGNUM);
    }
  else if (df_regs_ever_live_p (RETURN_POINTER_REGNUM))
    {
      extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);

      /* Have to do at least one pop */
      savesize_gp += UNITS_PER_WORD * 2;
    }

  loads_pic_register = METAG_CURRENT_FUNCTION_LOADS_PIC_REGISTER ();
  if (loads_pic_register)
    pic_save_size += UNITS_PER_WORD * 2;

  cfun->machine->frame_pointer_needed  = frame_pointer_needed;
  cfun->machine->non_leaf              = non_leaf;
  cfun->machine->savesize_gp           = savesize_gp;
  cfun->machine->savesize_eh           = savesize_eh;
  cfun->machine->FP_SP_offset          = FP_SP_offset + pic_save_size;
  cfun->machine->pic_save_size         = pic_save_size;
  cfun->machine->out_local_size        = out_local_size;
  cfun->machine->calls_eh_return       = current_function_calls_eh_return;
  cfun->machine->extras_gp             = extras_gp;
  cfun->machine->extras_eh             = extras_eh;
  cfun->machine->uses_pic_offset_table = current_function_uses_pic_offset_table;
  cfun->machine->loads_pic_register    = loads_pic_register;
  cfun->machine->ech_ctx_required      = (ech_ctx != 0);
  cfun->machine->arg_adjust_delta      = 0;
  cfun->machine->frame_adjust_delta    = 0;
  cfun->machine->can_use_short_branch  = false;
  cfun->machine->valid                 = true;

  switch (from)
    {
    case ARG_POINTER_REGNUM:
      switch (to)
        {
        case STACK_POINTER_REGNUM:
          delta = -savesize_gp - savesize_eh - pic_save_size - out_local_size;
          if (cfun->machine->anonymous_args)
            delta += ALIGN_ON_STACK_BOUNDARY (cfun->machine->anonymous_args_size);
          cfun->machine->arg_adjust_delta = delta;
          return delta;
        case HARD_FRAME_POINTER_REGNUM:
          delta = -pretend_size;
          if (cfun->machine->anonymous_args)
            delta += ALIGN_ON_STACK_BOUNDARY (cfun->machine->anonymous_args_size);
          cfun->machine->arg_adjust_delta = delta;
          return delta;
        default:
          gcc_unreachable ();
        }
      break;
    case FRAME_POINTER_REGNUM:
      switch (to)
        {
        case STACK_POINTER_REGNUM:
          delta = -out_local_size;
          cfun->machine->frame_adjust_delta = delta;
          return delta;
        case HARD_FRAME_POINTER_REGNUM:
          delta = -pretend_size + savesize_gp + savesize_eh + pic_save_size;
          cfun->machine->frame_adjust_delta = delta;
          return delta;
        default:
          gcc_unreachable ();
          break;
        }
      break;
    default:
      gcc_unreachable ();
      break;
    }

  gcc_unreachable ();
}

typedef struct hwtrace_fn
{
  const char *        name;
  int                 onoff;
  struct hwtrace_fn *next;
} hwtrace_fn;

/* A simple linked list records info about "#pragma hwtrace_function (name, 0|1) */
static hwtrace_fn *hwtrace_function_list = NULL;
/* records default if #pragma hwtrace_function (*, 0|1) */
static int         hwtrace_function_default  = -1; /* < 0 none, 0 off > 1 on */

static bool
hwtrace_function_enabled (tree function_decl)
{
  if (function_decl)
    {
      const char * fnname = IDENTIFIER_POINTER (DECL_NAME (function_decl));
      hwtrace_fn *next   = hwtrace_function_list;

      while (next != NULL)
      {
        if (strcmp (next->name, fnname) == 0)
          return next->onoff;

        next = next->next;
      }

      if (hwtrace_function_default < 0)
        return true;

      if (hwtrace_function_default > 0)
        return true;

      return false;
    }

  return true;
}

static struct machine_function *
metag_init_machine_status (void)
{
  struct machine_function *machine
    = (machine_function *) ggc_alloc_cleared (sizeof (*machine));
  bool enabled = hwtrace_function_enabled (current_function_decl);

  machine->valid = false;
  machine->hwtrace       = TARGET_HWTRACE       && enabled;
  machine->hwtrace_leaf  = TARGET_HWTRACE_LEAF  && enabled;
  machine->hwtrace_retpc = TARGET_HWTRACE_RETPC && enabled;

  machine->cond_return_state = METAG_COND_RETURN_NONE;
  return machine;
}

void
metag_init_expanders (void)
{
  init_machine_status = metag_init_machine_status;
}

bool
metag_legitimate_address_p (rtx addr, enum machine_mode mode, bool strict)
{
  rtx tmp;

  if (METAG_FLAG_PIC && SYMBOLIC_CONST (addr))
    return metag_legitimate_pic_address_disp_p (addr);

  if (SYMBOL_REF_P (addr) && METAG_SYMBOL_FLAG_DIRECT_P (addr))
    return true;

  tmp = addr;

  if (SUBREG_P (tmp)
      && (GET_MODE_SIZE (GET_MODE (tmp))
          < GET_MODE_SIZE (GET_MODE (SUBREG_REG (tmp)))))
    tmp = SUBREG_REG (tmp);

  if (METAG_LEGITIMATE_REG_P (tmp, strict))
    return true;

  if (METAG_LEGITIMATE_PRE_INCDEC_P (addr, mode, strict))
    return true;

  if (METAG_LEGITIMATE_POST_INCDEC_P (addr, mode, strict))
    return true;

  if (METAG_LEGITIMATE_PRE_MODIFY_P (addr, mode, strict))
    return true;

  if (METAG_LEGITIMATE_POST_MODIFY_P (addr, mode, strict))
    return true;

  if (GET_CODE (addr) == PLUS)
    {
      rtx op0 = XEXP (addr, 0);
      rtx op1 = XEXP (addr, 1);

      if (SUBREG_P (op0)
          && (GET_MODE_SIZE (GET_MODE (op0))
              < GET_MODE_SIZE (GET_MODE (SUBREG_REG (op0)))))
        op0 = SUBREG_REG (op0);

      if (METAG_LEGITIMATE_REG_P (op0, strict))
        {
          if (REG_P (op1)
              && METAG_LEGITIMATE_TWIN_P (op0, op1, mode, strict))
            return true;

          if (CONST_INT_P (op1)
              && METAG_LEGITIMATE_OFF_P (op0, op1, mode, strict))
            return true;
        }

      if (METAG_FLAG_PIC && op0 == pic_offset_table_rtx)
        return metag_legitimate_pic_address_disp_p (op1);
    }

  if (0 && GET_CODE (addr) == PLUS)
    {
      rtx op0 = XEXP (addr, 0);
      rtx op1 = XEXP (addr, 1);

      if (GET_CODE (op0) == PLUS
          && GET_CODE (op1) == CONST_INT)
        {
          rtx op3 = XEXP (op0, 0);
          rtx op4 = XEXP (op0, 1);

          if (CONST_INT_P (op4))
            {
              if (SUBREG_P (op3)
                  && (GET_MODE_SIZE (GET_MODE (op3))
                      < GET_MODE_SIZE (GET_MODE (SUBREG_REG (op3)))))
                op3 = SUBREG_REG (op3);

              op4 = GEN_INT (INTVAL (op1) + INTVAL (op4));
              if (METAG_LEGITIMATE_REG_P (op3, strict)
                  && METAG_LEGITIMATE_OFF_P (op3, op4, mode, strict))
                return true;
            }
        }
    }

  return false;
}

bool
metag_legitimate_regno_p (unsigned int regno, bool strict)
{
  if (strict)
    {
      if (IS_PSEUDO_REGNO (regno) && reg_renumber != NULL)
        regno = reg_renumber [regno];

      return regno <= FRAME_POINTER_REGNUM || regno == ARG_POINTER_REGNUM;
    }

  return regno != INVALID_REGNUM;
}

bool
metag_legitimate_reg_p (rtx reg, bool strict)
{
  return REG_P (reg) ? metag_legitimate_regno_p (REGNO (reg), strict)
                     : false;
}

/* Return true iff BASE and OFF are valid for  Reg + Reg addressing.
   If STRICT is true then we need to be strict w.r.t pseduo registers
  */
bool
metag_regs_ok_for_base_offset_p (rtx base_reg, rtx off_reg, bool strict)
{
  if (!METAG_LEGITIMATE_REG_P (base_reg, strict))
    return false;

  if (!METAG_LEGITIMATE_REG_P (off_reg, strict))
    return false;

  return metag_regs_same_regclass_p (base_reg, off_reg, strict);
}

bool
metag_reg_ok_for_base_p (rtx reg ATTRIBUTE_UNUSED, bool strict)
{
  return strict ?    STRICT_REG_OK_FOR_BASE_P (reg)
                : NONSTRICT_REG_OK_FOR_BASE_P (reg);
}

bool
metag_reg_ok_for_offset_p (rtx reg, bool strict)
{
  return strict ?    STRICT_REG_OK_FOR_OFFSET_P (reg)
                : NONSTRICT_REG_OK_FOR_OFFSET_P (reg);
}

bool
metag_reg_ok_for_index_p (rtx reg ATTRIBUTE_UNUSED, bool  strict)
{
  return strict ?    STRICT_REG_OK_FOR_INDEX_P (reg)
                : NONSTRICT_REG_OK_FOR_INDEX_P (reg);
}

bool
metag_legitimate_post_incdec_p (rtx addr, enum machine_mode mode ATTRIBUTE_UNUSED, bool strict)
{
  return (GET_CODE (addr) == POST_INC || GET_CODE (addr) == POST_DEC)
          && METAG_LEGITIMATE_REG_P (XEXP (addr, 0), strict)
          && !METAG_ELIMINABLE_REG_P (XEXP (addr, 0));
}

bool
metag_legitimate_pre_incdec_p (rtx addr, enum machine_mode mode ATTRIBUTE_UNUSED, bool strict)
{
  return (GET_CODE (addr) == PRE_INC || GET_CODE (addr) == PRE_DEC)
          && METAG_LEGITIMATE_REG_P (XEXP (addr, 0), strict)
          && !METAG_ELIMINABLE_REG_P (XEXP (addr, 0));
}

bool
metag_legitimate_off_p (rtx base, rtx off, enum machine_mode mode, bool strict ATTRIBUTE_UNUSED)
{
  if (CONST_INT_P (off))
    {
      HOST_WIDE_INT value    = INTVAL (off);
      unsigned int  modesize = GET_MODE_SIZE (mode);

      if  ((value & (modesize - 1)) == 0)
        {
          unsigned int  regno = REGNO (base);
          HOST_WIDE_INT limit;

          if (reg_renumber != NULL && IS_PSEUDO_REGNO (regno))
             regno = reg_renumber[regno];

          if (metag_regno12bit_p (regno)
              && (!metag_fpu_resources
                  || (GET_MODE_CLASS (mode) != MODE_FLOAT)))
            limit = 2048;
          else if (cfun && regno == ARG_POINTER_REGNUM)
            value += cfun->machine->arg_adjust_delta, limit = 2048;
          else if (cfun && regno == FRAME_POINTER_REGNUM)
            {
              if (reload_in_progress)
                return true;

              value += cfun->machine->frame_adjust_delta, limit = 2048;
            }
          else if (!strict && !reload_in_progress && !reload_completed
                   && (regno == ARG_POINTER_REGNUM
                       || regno == FRAME_POINTER_REGNUM))
            limit = 2048;
          else if (!strict && !reload_in_progress && !reload_completed && IS_PSEUDO_REGNO (regno)
                   && (!metag_fpu_resources
                       || (GET_MODE_CLASS (mode) != MODE_FLOAT)))
            limit = 2048;
          else
            limit = 32;

          limit *= modesize;

          return (-limit <= value && value < limit);
        }
    }

  return false;
}

bool
metag_legitimate_twin_p (rtx base, rtx off, enum machine_mode mode ATTRIBUTE_UNUSED, bool strict)
{
  return METAG_REGS_OK_FOR_BASE_OFFSET_P (base, off, strict);
}

bool
metag_frame_related_rtx (rtx op)
{
  return (REG_P (op)
          && (op == frame_pointer_rtx
              || op == arg_pointer_rtx
              || op == virtual_incoming_args_rtx
              || op == virtual_stack_vars_rtx
              || op == virtual_stack_dynamic_rtx
              || op == virtual_outgoing_args_rtx
              || op == virtual_cfa_rtx
              || REGNO (op) == FRAME_POINTER_REGNUM
              || REGNO (op) == ARG_POINTER_REGNUM));
}


/* Returns 1 if OP contains a symbol reference */

bool
metag_symbolic_reference_mentioned_p (rtx op)
{
  const char *fmt;
  int i;

  if (SYMBOL_REF_P (op) || LABEL_REF_P (op))
    return true;

  fmt = GET_RTX_FORMAT (GET_CODE (op));
  for (i = GET_RTX_LENGTH (GET_CODE (op)) - 1; i >= 0; i--)
    {
      if (fmt[i] == 'E')
        {
          int j;

          for (j = XVECLEN (op, i) - 1; j >= 0; j--)
            if (metag_symbolic_reference_mentioned_p (XVECEXP (op, i, j)))
              return true;
        }
      else if (fmt[i] == 'e' && metag_symbolic_reference_mentioned_p (XEXP (op, i)))
        return true;
    }

  return false;
}

bool
metag_legitimate_pic_address_disp_p (rtx disp)
{
  if (GET_CODE (disp) != CONST)
    return false;

  disp = XEXP (disp, 0);

  if (GET_CODE (disp) == PLUS)
    {
      if (!CONST_INT_P (XEXP (disp, 1)))
        return false;

      disp = XEXP (disp, 0);
    }

  if (GET_CODE (disp) != UNSPEC
      || XVECLEN (disp, 0) != 1)
    return false;

  /* Must be @GOT but not @GOTOFF  */
  if (XINT (disp, 1) != UNSPEC_GOT)
    return false;

  if (!SYMBOL_REF_P (XVECEXP (disp, 0, 0))
      && !LABEL_REF_P (XVECEXP (disp, 0, 0)))
    return false;

  return true;
}

/* Try machine-dependent ways of modifying an illegitimate address
   to be legitimate.  If we find one, return the new, valid address.
   This macro is used in only one place: `memory_address' in explow.c.

   OLDX is the address as it was before break_out_memory_refs was called.
   In some cases it is useful to look at this to decide what needs to be done.

   MODE and WIN are passed so that this macro can use
   GO_IF_LEGITIMATE_ADDRESS.

   It is always safe for this macro to do nothing.  It exists to recognize
   opportunities to optimize the output.
*/

rtx
metag_legitimize_address (rtx x, rtx oldx ATTRIBUTE_UNUSED,
                          enum machine_mode mode ATTRIBUTE_UNUSED)
{

  /* We currently only have support for thread local storage (TLS) 
     under META Linux.  There is no TLS support for the embedded 
     toolchain */

  if (tls_symbolic_operand_p (x))
    return metag_bfd_legitimize_tls_address (x);

  if (METAG_FLAG_PIC)
    return SYMBOLIC_CONST (x) ? metag_legitimize_pic_address (x, 0) : x;

  return x;
}

/* This function has been created by using the output template from the movsi
   insn in metag.md */

void
metag_emit_move_sequence (rtx operands[], 
                          enum machine_mode mode ATTRIBUTE_UNUSED)
{

  if (metag_bfd_tls_referenced_p (operands[1]))
    {
      rtx tmp = operands[1];
      rtx addend = NULL;

      /* All TLS symbols should be wrapped in an UNSPEC prior to reload (the
         check is performed by metag_bfd_tls_referenced_p) if they are still 
         symbols raise an error */
      if (reload_in_progress)
        gcc_unreachable ();
      else
        {
          /* Catch and fix the case where GCC is trying to offset a TLS symbol */
          if (GET_CODE (tmp) == CONST && GET_CODE (XEXP (tmp, 0)) == PLUS)
            {
              addend = XEXP (XEXP (tmp, 0), 1);
              tmp = XEXP (XEXP (tmp, 0), 0);
            }
    
          gcc_assert (GET_CODE (tmp) == SYMBOL_REF);
          gcc_assert (SYMBOL_REF_TLS_MODEL (tmp) != 0);
    
          tmp = metag_bfd_legitimize_tls_address (tmp);
    
          if (addend)
            {
              tmp = gen_rtx_PLUS (mode, tmp, addend);
              tmp = force_operand (tmp, operands[0]);
            }
    
          operands[1] = tmp;

          if (MEM_P (operands[0]))
            {
              /* All except mem = const, mem = mem, or mem = addr can be done quickly */
              operands[1] = force_reg (SImode, operands[1]);
            }
        }
    }
  else if (METAG_FLAG_PIC && SYMBOLIC_CONST (operands[1]))
    {
      if (MEM_P (operands[0]) && SYMBOLIC_CONST (operands[1]))
        operands[1] = force_reg (Pmode, operands[1]);
      else
        {
          rtx temp = reload_in_progress ? operands[0] : gen_reg_rtx (Pmode);

          operands[1] = metag_legitimize_pic_address (operands[1], temp);
        }
    }
  else if (MEM_P (operands[0]))
    {
      /* All except mem = const, mem = mem, or mem = addr can be done quickly */
      operands[1] = force_reg (SImode, operands[1]);
    }

  if (REG_P (operands[0])
      && REGNO (operands[0]) == TXRPT_REGNUM)
    {
      if (CONST_INT_P (operands[1])
          && !(METAG_CONST_OK_FOR_LETTERS_KPIJ (operands[1])))
        {
          operands[1] = force_reg (SImode, operands[1]);
        }
    }
}

/* Return a legitimate reference for ORIG (an address) using the
   register REG.  If REG is 0, a new pseudo is generated.

   There are two types of references that must be handled:

   1. Global data references must load the address from the GOT, via
      the PIC reg.  An insn is emitted to do this load, and the reg is
      returned.

   2. Static data references, constant pool addresses, and code labels
      compute the address as an offset from the GOT, whose base is in
      the PIC reg.  Static data objects have SYMBOL_REF_FLAG set to
      differentiate them from global data objects.  The returned
      address is the PIC reg + an unspec constant.
*/

rtx
metag_legitimize_pic_address (rtx orig, rtx reg)
{
  rtx addr = orig;
  rtx new = orig;
  rtx base;

  if (LABEL_REF_P (addr)
      || (SYMBOL_REF_P (addr)
          && (CONSTANT_POOL_ADDRESS_P (addr)
              || METAG_SYMBOL_FLAG_DIRECT_P (addr)
              || SYMBOL_REF_LOCAL_P (addr))))
    {
      /* This symbol may be referenced via a displacement from the PIC
         base address (@GOTOFF).  */

      /* Only mark this function as needing pic if we are not being called
         as part of a cost-estimation process */
      if (!ir_type ())
        current_function_uses_pic_offset_table = 1;

      new = gen_rtx_UNSPEC (VOIDmode, gen_rtvec (1, addr), UNSPEC_GOTOFF);
      new = gen_rtx_CONST (VOIDmode, new);
      new = gen_rtx_PLUS (Pmode, pic_offset_table_rtx, new);

      if (reg != 0)
        {
          emit_move_insn (reg, new);
          new = reg;
        }
    }
  else if (SYMBOL_REF_P (addr))
    {
      /* This symbol must be referenced via a load from the
         Global Offset Table (@GOT). */

      /* Only mark this function as needing pic if we are not being called
         as part of a cost-estimation process */
      if (!ir_type ())
        current_function_uses_pic_offset_table = 1;

      new = gen_rtx_UNSPEC (VOIDmode, gen_rtvec (1, addr), UNSPEC_GOT);
      new = gen_rtx_CONST (VOIDmode, new);
      new = gen_rtx_PLUS (Pmode, pic_offset_table_rtx, new);
      new = gen_rtx_MEM (Pmode, new);

      if (reg == 0)
        reg = gen_reg_rtx (Pmode);

      emit_move_insn (reg, new);
      new = reg;
    }
  else
    {
      if (GET_CODE (addr) == CONST)
        {
          addr = XEXP (addr, 0);
          if (GET_CODE (addr) == UNSPEC)
            {
              /* Check that the unspec is one of the ones we generate?  */
            }
          else if (GET_CODE (addr) != PLUS)
            abort();
        }

      if (GET_CODE (addr) == PLUS)
        {
          rtx op0 = XEXP (addr, 0), op1 = XEXP (addr, 1);

          /* Check first to see if this is a constant offset from a @GOTOFF
             symbol reference.  */
          if ((LABEL_REF_P (op0)
               || (SYMBOL_REF_P (op0)
                   && (CONSTANT_POOL_ADDRESS_P (op0)
                       || METAG_SYMBOL_FLAG_DIRECT_P (op0))))
              && CONST_INT_P (op1))
            {
              /* Only mark this function as needing pic if we are not being called
                 as part of a cost-estimation process */
              if (!ir_type ())
                current_function_uses_pic_offset_table = 1;

              new = gen_rtx_UNSPEC (VOIDmode, gen_rtvec (1, op0), UNSPEC_GOTOFF);
              new = gen_rtx_PLUS (Pmode, new, op1);
              new = gen_rtx_CONST (VOIDmode, new);
              new = gen_rtx_PLUS (Pmode, pic_offset_table_rtx, new);

              if (reg != 0)
                {
                  emit_move_insn (reg, new);
                  new = reg;
                }
            }
          else
            {
              base = metag_legitimize_pic_address (XEXP (addr, 0), reg);
              new  = metag_legitimize_pic_address (XEXP (addr, 1),
                                                   base == reg ? NULL_RTX : reg);

              if (CONST_INT_P (new))
                new = plus_constant (base, INTVAL (new));
              else
                {
                  if (GET_CODE (new) == PLUS && CONSTANT_P (XEXP (new, 1)))
                    {
                      base = gen_rtx_PLUS (Pmode, base, XEXP (new, 0));
                      new = XEXP (new, 1);
                    }

                  new = gen_rtx_PLUS (Pmode, base, new);
                }
            }
        }
    }

  return new;
}

/* Compute a (partial) cost for rtx X.  Return true if the complete
   cost has been computed, and false if subexpressions should be
   scanned.  In either case, *TOTAL contains the cost result.  */

bool
metag_rtx_costs (rtx x, int code, int outer_code, int *total)
{
  switch (code)
    {
      case CONST_INT:
        if (satisfies_constraint_K (x))
          *total = (outer_code == SET     ? COSTS_N_INSNS (1) :
                    outer_code == PLUS    ? 0 :
                    outer_code == MINUS   ? 0 :
                    outer_code == AND     ? 0 :
                    outer_code == IOR     ? 0 :
                    outer_code == XOR     ? 0 :
                    outer_code == COMPARE ? 0 :
                                            COSTS_N_INSNS (1));
        else if (satisfies_constraint_P (x))
          *total = (outer_code == SET     ? COSTS_N_INSNS (1) :
                    outer_code == PLUS    ? 0 :
                    outer_code == MINUS   ? 0 :
                    outer_code == AND     ? 0 :
                    outer_code == IOR     ? 0 :
                    outer_code == XOR     ? 0 :
                    outer_code == COMPARE ? 0 :
                                            COSTS_N_INSNS (1));
        else if ((INTVAL (x) & 0xffff) == 0xffff
                 && (outer_code == AND || outer_code == IOR || outer_code == XOR))
          *total = COSTS_N_INSNS (1);
        else if ((INTVAL (x) & 0xffff0000) == 0xffff0000
                 && (outer_code == AND || outer_code == IOR || outer_code == XOR))
          *total = COSTS_N_INSNS (1);
        else if (INTVAL (x) >= -32768 && INTVAL (x) <= 65535)
          *total = COSTS_N_INSNS (1);
        else if ((INTVAL (x) & 0xffff) == 0)
          *total = COSTS_N_INSNS (2);
        else 
          *total = COSTS_N_INSNS (5);
        break;
      case CONST:
        if (outer_code == MEM)
          {
            *total = COSTS_N_INSNS (1);
            return true;
          }
        *total = COSTS_N_INSNS (2);
        break;
      case LABEL_REF:
      case SYMBOL_REF:
        *total = COSTS_N_INSNS (6);
        break;
      case CONST_DOUBLE:
        *total = COSTS_N_INSNS (10);
        break;
      case MULT:
        *total = COSTS_N_INSNS (2);
        break;
      case DIV:
      case UDIV:
      case MOD:
      case UMOD:
        *total = COSTS_N_INSNS (50);
        break;
      case PLUS:
      case MINUS:
        if (GET_MODE (x) == SImode)
          {
            rtx          exp  = XEXP (x, 0);
            unsigned int reg0 = REG_P (XEXP (x, 0)) ? REGNO (XEXP (x, 0)) : INVALID_REGNUM;
            unsigned int reg1 = REG_P (XEXP (x, 1)) ? REGNO (XEXP (x, 1)) : INVALID_REGNUM;

            if (reg_renumber != NULL && IS_PSEUDO_REGNO (reg0))
              reg0 = reg_renumber[reg0];

            if (reg_renumber != NULL && IS_PSEUDO_REGNO (reg1))
              reg1 = reg_renumber[reg1];

            if (IS_HARD_OR_VIRT_REGNO (reg0)
                && IS_HARD_OR_VIRT_REGNO (reg1)
                && METAG_REGNO_REG_CLASS (reg0) != METAG_REGNO_REG_CLASS (reg1))
              {
                /* Cannot really add/sub registers in different units */
                *total = COSTS_N_INSNS (50);
              }
            else if (exp != frame_pointer_rtx
                     && exp != stack_pointer_rtx
                     && exp != arg_pointer_rtx)
              *total = COSTS_N_INSNS (1);
            else
              *total = COSTS_N_INSNS (2);
          }
        else
          *total = COSTS_N_INSNS (4);

        break;
      case COMPARE:
        *total = COSTS_N_INSNS (1);
        break;
      case MEM:
        if (outer_code == SIGN_EXTEND)
          *total = COSTS_N_INSNS (2);
        else
          *total = COSTS_N_INSNS (1);
        break;
      case SIGN_EXTEND:
        *total = COSTS_N_INSNS (1);
        break;
      default:
        *total = COSTS_N_INSNS (1);
        break;
    }

  return false;
}

int
metag_sched_adjust_cost (rtx insn, rtx link,
                         rtx dep_insn, int cost)
{
  switch (REG_NOTE_KIND (link))
    {
    case REG_DEP_ANTI:
    case REG_DEP_OUTPUT:
      return 0;
    case REG_DEP_TRUE:
      if (recog_memoized (insn) >= 0 && recog_memoized (dep_insn) >= 0)
        {
          enum attr_type      type_attr      = get_attr_type (dep_insn);
          enum attr_memaccess memaccess_attr = get_attr_memaccess (dep_insn);

          /* Match the twox|threex|fourx|fivex loads */
          if ((type_attr == TYPE_TWOX || type_attr == TYPE_THREEX
               || type_attr == TYPE_FOURX || type_attr == TYPE_FIVEX)
              && memaccess_attr == MEMACCESS_LOAD)
            {
              /* If the dependency is based on either of the last 2 registers
               * loaded, the latency increases.  */
              cost += metag_consumer_stalls_from_load_multi (dep_insn, insn);
            }

          /* If there is an o2rhint then the insn may have an o2r operand which
           * may stall.  */
          switch (get_attr_o2rhint (insn))
            {
            case O2RHINT_NONE:
              break;
            case O2RHINT_OP2OP1:
              /* insn has an o2r operand if units of operands 2 and 1 differ.  */
              if (metag_consumer_is_o2r (dep_insn, insn, 2, 1))
                return cost + 1;

              break;
            case O2RHINT_OP1OP0:
              /* insn has an o2r operand if units of operands 1 and 0 differ.  */
              if (metag_consumer_is_o2r (dep_insn, insn, 1, 0))
                return cost + 1;

              break;
            default:
              /* Bad o2rhint, missing a case for the hint.  */
              gcc_unreachable ();
            }

          break;
        }
       break;
     default:
       break;
    }

  return cost;
}

int
metag_address_cost (rtx x)
{
  switch (GET_CODE (x))
    {
    case REG:
      return 0;
    case LABEL_REF:
    case SYMBOL_REF:
    case CONST:
    case MEM:
      return 10;
    case PRE_INC:
    case POST_INC:
    case PRE_DEC:
    case POST_DEC:
    case PRE_MODIFY:
    case POST_MODIFY:
     return 0;
    case PLUS:
      if (REG_P (XEXP (x, 0))
          && (REG_P (XEXP (x, 1)) || CONST_INT_P (XEXP (x, 1))))
        return 2;
      break;
    default:
      break;
    }

  return 6;
}

#define add_builtin_function(NAME, TYPE, CODE, CLASS, LIBNAME, ATTR)    \
  lang_hooks.builtin_function (NAME, TYPE, CODE, CLASS, LIBNAME, ATTR)

void
metag_init_builtins(void)
{
  tree nothrow = tree_cons (get_identifier ("nothrow"), NULL, NULL);
#if 0
  tree const_throw = tree_cons (get_identifier ("const"), NULL, nothrow);
#endif
  tree endlink = void_list_node;
  tree ftdcache_preload
    = build_function_type (ptr_type_node,
                           tree_cons (NULL_TREE, ptr_type_node, 
                                      endlink));
 
  tree ftdcache_flush
    = build_function_type (void_type_node,
                           tree_cons (NULL_TREE, ptr_type_node,
                                      endlink));
 
  tree ftdcache_refresh
    = build_function_type (ptr_type_node,
                           tree_cons (NULL_TREE, ptr_type_node,
                                      endlink));

  tree ftmeta2_cacherd
    = build_function_type (unsigned_intSI_type_node,
                           tree_cons (NULL_TREE, ptr_type_node,
                                      endlink));

  tree ftmeta2_cacherl
    = build_function_type (unsigned_intDI_type_node,
                           tree_cons (NULL_TREE, ptr_type_node,
                                      endlink));

  tree ftmeta2_cachewd
    = build_function_type (void_type_node,
                           tree_cons (NULL_TREE, ptr_type_node,
                                      tree_cons (NULL_TREE, unsigned_intSI_type_node,
                                                 endlink)));

  tree ftmeta2_cachewl
    = build_function_type (void_type_node,
                           tree_cons (NULL_TREE, ptr_type_node,
                                      tree_cons (NULL_TREE, unsigned_intDI_type_node,
                                                 endlink)));

  tree ftmetag_bswap
    = build_function_type (intSI_type_node,
                           tree_cons (NULL_TREE, intSI_type_node,
                                                 endlink));

  tree ftmetag_bswaps
    = build_function_type (intHI_type_node,
                           tree_cons (NULL_TREE, intHI_type_node,
                                                 endlink));

  tree ftmetag_bswapll
    = build_function_type (intDI_type_node,
                           tree_cons (NULL_TREE, intDI_type_node,
                                                 endlink));

  tree ftmetag_wswap
    = build_function_type (intSI_type_node,
                           tree_cons (NULL_TREE, intSI_type_node,
                                                 endlink));

  tree ftmetag_wswapll
    = build_function_type (intDI_type_node,
                           tree_cons (NULL_TREE, intDI_type_node,
                                                 endlink));

  tree ftmetag_dswapll
    = build_function_type (intDI_type_node,
                           tree_cons (NULL_TREE, intDI_type_node,
                                                 endlink));



  add_builtin_function ("__builtin_dcache_preload", ftdcache_preload,
                        METAG_BUILTIN_DCACHE_PRELOAD,
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_dcache_flush", ftdcache_flush,
                        METAG_BUILTIN_DCACHE_FLUSH,
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_dcache_refresh",
                        ftdcache_refresh,
                        METAG_BUILTIN_DCACHE_REFRESH, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_meta2_cacherd",
                        ftmeta2_cacherd,
                        METAG_BUILTIN_META2_CACHERD, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_meta2_cacherl",
                        ftmeta2_cacherl,
                        METAG_BUILTIN_META2_CACHERL, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_meta2_cachewd",
                        ftmeta2_cachewd,
                        METAG_BUILTIN_META2_CACHEWD, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_meta2_cachewl",
                        ftmeta2_cachewl,
                        METAG_BUILTIN_META2_CACHEWL, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_metag_bswaps",
                        ftmetag_bswaps,
                        METAG_BUILTIN_METAG_BSWAPS, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_metag_bswap",
                        ftmetag_bswap,
                        METAG_BUILTIN_METAG_BSWAP, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_metag_bswapll",
                        ftmetag_bswapll,
                        METAG_BUILTIN_METAG_BSWAPLL, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_metag_wswap",
                        ftmetag_wswap,
                        METAG_BUILTIN_METAG_WSWAP, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_metag_wswapll",
                        ftmetag_wswapll,
                        METAG_BUILTIN_METAG_WSWAPLL, 
                        BUILT_IN_MD,
                        NULL, nothrow);

  add_builtin_function ("__builtin_metag_dswapll",
                        ftmetag_dswapll,
                        METAG_BUILTIN_METAG_DSWAPLL, 
                        BUILT_IN_MD,
                        NULL, nothrow);
  /* Initialise the builtin functions for the operating system gcc 
     is targeting code for */
  metag_init_builtins_per_os ();

}

/* Emit the optimal byte swap sequence for a 16 bit byte swap */

static void
metag_emit_byte_swap16 (rtx out, rtx in)
{
  rtx tmp = gen_reg_rtx (SImode);

  emit_insn (gen_andsi3 (out, in, 
                         gen_int_mode (0x0000FFFF, SImode)));
  emit_insn (gen_lshrsi3 (tmp, out, GEN_INT (BITS_PER_UNIT)));
  emit_insn (gen_ashlsi3 (out, out, GEN_INT (BITS_PER_UNIT * 3)));
  emit_insn (gen_ashrsi3 (out, out, GEN_INT (BITS_PER_UNIT * 2)));
  emit_insn (gen_iorsi3 (out, out, tmp));
}

/* Emit the optimal byte swap sequence for a 32 bit byte swap */

static void
metag_emit_byte_swap32 (rtx out, rtx in)
{
  rtx tmp = gen_reg_rtx (SImode);

  emit_insn (gen_rotsi2_16 (tmp, in));
  emit_insn (gen_lshrsi3 (out, tmp, GEN_INT (8)));
  emit_insn (gen_andsi3 (tmp, tmp, 
                         gen_int_mode (0xFFFF00FF, SImode)));
  emit_insn (gen_andsi3 (out, out, 
                         gen_int_mode (0xFFFF00FF, SImode)));
  emit_insn (gen_ashlsi3 (tmp, tmp, GEN_INT (8)));
  emit_insn (gen_iorsi3 (out, out, tmp));
}

/* Expand an expression EXP that calls a built-in function,
   with result going to TARGET if that's convenient
   (and in mode MODE if that's convenient).
   SUBTARGET may be used as the target for computing one of EXP's operands.
   IGNORE is nonzero if the value is to be ignored.  */

rtx
metag_expand_builtin (tree exp, rtx target ATTRIBUTE_UNUSED,
                      rtx subtarget ATTRIBUTE_UNUSED, enum machine_mode mode ATTRIBUTE_UNUSED,
                      int ignore ATTRIBUTE_UNUSED)
{
  tree fndecl  = TREE_OPERAND (TREE_OPERAND (exp, 0), 0);
  tree arglist = TREE_OPERAND (exp, 1);
  int  fcode   = DECL_FUNCTION_CODE (fndecl);

  switch (fcode)
    {
    case METAG_BUILTIN_DCACHE_PRELOAD: /* void * __builtin_dcache_preload (void *) */
      if (   TARGET_BUILTINS_METAC_1_1
          || TARGET_BUILTINS_METAC_1_2
          || TARGET_BUILTINS_METAC_2_1)
        {
          tree              arg0  = TREE_VALUE (arglist);
          rtx               op0   = expand_normal (arg0);
          enum machine_mode mode0 = insn_data[CODE_FOR_dcache_preload].operand[0].mode;
          rtx  pat;

          if (! (*insn_data[CODE_FOR_dcache_preload].operand[0].predicate)(op0, mode0))
            op0 = copy_to_mode_reg (mode0, op0);

          pat = GEN_FCN (CODE_FOR_dcache_preload)(op0);

          if (!pat)
            return NULL_RTX;
          emit_insn (pat);
          return op0;
        }
      else
        error ("__builtin_dcache_preload not supported for metac %s", metag_cpu_string);
      break;
    case METAG_BUILTIN_DCACHE_FLUSH: /* void __builtin_dcache_flush  (void *) */
      if (   TARGET_BUILTINS_METAC_1_2
          || TARGET_BUILTINS_METAC_2_1)
        {
            tree              arg0  = TREE_VALUE (arglist);
            rtx               op0   = expand_normal (arg0);
            rtx               op1;
            enum machine_mode mode0 = insn_data[CODE_FOR_dcache_flush].operand[0].mode;
            enum machine_mode mode1 = insn_data[CODE_FOR_dcache_flush].operand[1].mode;
            rtx  pat;

            if (! (*insn_data[CODE_FOR_dcache_flush].operand[0].predicate)(op0, mode0))
              op0 = copy_to_mode_reg (mode0, op0);

            op1 = gen_reg_rtx (mode1);

            pat = GEN_FCN (CODE_FOR_dcache_flush)(op0, op1);

            if (!pat)
              return NULL_RTX;

            emit_move_insn (op1, const0_rtx);
            emit_insn (pat);
            return const1_rtx;
        }
      else
        error ("__builtin_dcache_flush not supported for metac %s", metag_cpu_string);
      break;
    case METAG_BUILTIN_DCACHE_REFRESH: /* void * __builtin_dcache_refresh (void *) */
      if (   TARGET_BUILTINS_METAC_1_1
          || TARGET_BUILTINS_METAC_1_2
          || TARGET_BUILTINS_METAC_2_1)
        {
          tree              arg0  = TREE_VALUE (arglist);
          rtx               op0   = expand_normal (arg0);
          rtx               op1;
          enum machine_mode mode0 = insn_data[CODE_FOR_dcache_refresh].operand[0].mode;
          enum machine_mode mode1 = insn_data[CODE_FOR_dcache_refresh].operand[1].mode;
          rtx  pat;

          if (! (*insn_data[CODE_FOR_dcache_refresh].operand[0].predicate)(op0, mode0))
            op0 = copy_to_mode_reg (mode0, op0);

          op1 = gen_reg_rtx (mode1);

          pat = GEN_FCN (CODE_FOR_dcache_refresh)(op0, op1);

          if (!pat)
            return NULL_RTX;

          emit_move_insn (op1, const0_rtx);
          emit_insn (pat);
          return op0;
        }
      else
        error ("__builtin_dcache_refresh not supported for metac %s", metag_cpu_string);
      break;
    case METAG_BUILTIN_META2_CACHERD: /* unsigned long __builtin_meta2_cacherd (void *) */
    case METAG_BUILTIN_META2_CACHERL: /* unsigned long long __builtin_meta2_cacherl (void *) */
      if (TARGET_BUILTINS_METAC_2_1)
        {
          tree              arg0  = TREE_VALUE (arglist);
          rtx               op1   = expand_normal (arg0);
          enum machine_mode tgtmode;
          enum machine_mode mode1;
          enum insn_code    icode = CODE_FOR_meta2_cacherd;
          rtx               pat;

          if (fcode == METAG_BUILTIN_META2_CACHERL)
            icode = CODE_FOR_meta2_cacherl;

          tgtmode = insn_data[icode].operand[0].mode;
          mode1 = insn_data[icode].operand[1].mode;

          if (target == 0 || !insn_data[icode].operand[0].predicate (target, tgtmode))
            target = gen_reg_rtx (tgtmode);

          if (! (*insn_data[icode].operand[1].predicate)(op1, mode1))
            op1 = copy_to_mode_reg (mode1, op1);

          pat = GEN_FCN (icode)(target, op1);

          if (!pat)
            return NULL_RTX;

          emit_insn (pat);
          return target;
        }
      else
        error ("__builtin_meta2_cacher[dl] not supported for metac %s", metag_cpu_string);
      break;
    case METAG_BUILTIN_META2_CACHEWD: /* void __builtin_meta2_cachewd (void *, unsigned lon) */
    case METAG_BUILTIN_META2_CACHEWL: /* void __builtin_meta2_cachewl (void *, unsigned long long) */
      if (TARGET_BUILTINS_METAC_2_1)
        {
          tree              arg0  = TREE_VALUE (arglist);
          tree              arg1  = TREE_VALUE (TREE_CHAIN (arglist));
          rtx               op0   = expand_normal (arg0);
          rtx               op1   = expand_normal (arg1);

          enum machine_mode mode0;
          enum machine_mode mode1;
          enum insn_code    icode = CODE_FOR_meta2_cachewd;
          rtx               pat;

          if (fcode == METAG_BUILTIN_META2_CACHEWL)
            icode = CODE_FOR_meta2_cachewl;

          mode0 = insn_data[icode].operand[0].mode;
          mode1 = insn_data[icode].operand[1].mode;

          if (! (*insn_data[icode].operand[0].predicate)(op0, mode0))
            op0 = copy_to_mode_reg (mode0, op0);

          if (! (*insn_data[icode].operand[1].predicate)(op1, mode1))
            op1 = copy_to_mode_reg (mode1, op1);

          pat = GEN_FCN (icode)(op0, op1);

          if (!pat)
            return NULL_RTX;

          emit_insn (pat);
          return const1_rtx;
        }
      else
        error ("__builtin_meta2_cachew[dl] not supported for metac %s (or extension disabled)", metag_cpu_string);
      break;
    case METAG_BUILTIN_METAG_BSWAPS: /* short __builtin_metag_bswaps (short) */
    case METAG_BUILTIN_METAG_BSWAP: /* int __builtin_metag_bswap (int) */
    case METAG_BUILTIN_METAG_BSWAPLL: /* long long __builtin_metag_bswap (long long) */
      if (!TARGET_BUILTINS_METAC_2_1 && metag_meta2_bex_enabled)
        error ("The 'bex' extension is only available on a META 2.1 core");
      else
        {
          tree              arg0  = TREE_VALUE (arglist);
          rtx               op1   = expand_normal (arg0);

          enum machine_mode mode;

          if (fcode == METAG_BUILTIN_METAG_BSWAPS)
            /* This looks like it should be HImode, but type promotion on the arguments causes 
               shorts to become ints so SImode actually needs to be used */
            mode = SImode;
          else if (fcode == METAG_BUILTIN_METAG_BSWAP)
            mode = SImode;
          else
            mode = DImode;

          if (target == 0 || !metag_register_op (target, mode))
            target = gen_reg_rtx (mode);

          if (!metag_register_op (op1, mode))
            op1 = copy_to_mode_reg (mode, op1);

          if (metag_meta2_bex_enabled)
            {
              if (fcode == METAG_BUILTIN_METAG_BSWAPS)
              {
                emit_insn (gen_metag_bswap (target, op1));
                emit_insn (gen_ashrsi3 (target, target, GEN_INT ((UNITS_PER_WORD / 2) * BITS_PER_UNIT)));
              }
              else if (fcode == METAG_BUILTIN_METAG_BSWAP)
                emit_insn (gen_metag_bswap (target, op1));
              else
                emit_insn (gen_metag_bswapll (target, op1));
            }
          else
            {
              /* Generate the optimal byte swap sequence */
              if (fcode == METAG_BUILTIN_METAG_BSWAPS)
                metag_emit_byte_swap16 (target, op1);
              else if (fcode == METAG_BUILTIN_METAG_BSWAP)
                metag_emit_byte_swap32 (target, op1);
              else
                {
                  rtx target_lo = simplify_gen_subreg (SImode, target, mode, 0);
                  rtx target_hi = simplify_gen_subreg (SImode, target, mode, 4);
                  rtx op1_lo = simplify_gen_subreg (SImode, op1, mode, 0);
                  rtx op1_hi = simplify_gen_subreg (SImode, op1, mode, 4);

                  metag_emit_byte_swap32 (target_lo, op1_hi);
                  metag_emit_byte_swap32 (target_hi, op1_lo);
                }
            }

          return target;
        }
      break;
    case METAG_BUILTIN_METAG_WSWAP: /* int __builtin_metag_wswap (int) */
    case METAG_BUILTIN_METAG_WSWAPLL: /* long long __builtin_metag_wswapll (long long) */
    case METAG_BUILTIN_METAG_DSWAPLL: /* long long __builtin_metag_dswapll (long long) */
        {

          tree              arg0  = TREE_VALUE (arglist);
          rtx               op1   = expand_normal (arg0);

          enum machine_mode mode;

          if (fcode == METAG_BUILTIN_METAG_WSWAP) 
            mode = SImode;
          else
            mode = DImode;

          if (target == 0 || !metag_register_op (target, mode))
            target = gen_reg_rtx (mode);

          if (!metag_register_op (op1, mode))
            op1 = copy_to_mode_reg (mode, op1);

          if (fcode == METAG_BUILTIN_METAG_WSWAP)
            {
              emit_insn (gen_rotsi2_16 (target, op1));
            }
          else if (fcode == METAG_BUILTIN_METAG_WSWAPLL)
            {
              rtx target_lo = simplify_gen_subreg (SImode, target, mode, 0);
              rtx target_hi = simplify_gen_subreg (SImode, target, mode, 4);
              if (TARGET_DSP)
                {
                  emit_insn (gen_parallel_rotsi2_16 (target, op1));
                  emit_insn (gen_swapsi (target_hi, target_lo));
                }
              else
                {
                  rtx op1_lo = simplify_gen_subreg (SImode, op1, mode, 0);
                  rtx op1_hi = simplify_gen_subreg (SImode, op1, mode, 4);
                  emit_insn (gen_rotsi2_16 (target_lo, op1_lo));
                  emit_insn (gen_rotsi2_16 (target_hi, op1_hi));
                  emit_insn (gen_swapsi (target_hi, target_lo));
                }
            }
          else if (fcode == METAG_BUILTIN_METAG_DSWAPLL)
            {
              rtx target_lo = simplify_gen_subreg (SImode, target, mode, 0);
              rtx target_hi = simplify_gen_subreg (SImode, target, mode, 4);
              emit_insn (gen_movdi (target, op1));
              emit_insn (gen_swapsi (target_hi, target_lo));
            }
          return target;
        }
        break;
    default:
      break;
  }

  /* Expand any operating system specific builtin functions */
  return metag_expand_builtin_per_os (exp, target);
}

static tree
metag_handle_model_decl_attribute (tree *node ATTRIBUTE_UNUSED,
                                   tree name ATTRIBUTE_UNUSED,
                                   tree args ATTRIBUTE_UNUSED,
                                   int flags ATTRIBUTE_UNUSED,
                                   bool *no_add_attrs ATTRIBUTE_UNUSED)
{
  tree value = TREE_VALUE (args);

  if (strcmp (IDENTIFIER_POINTER (name), "model") == 0)
    {
      if (TREE_CODE (value) != STRING_CST)
        {
          warning (OPT_Wattributes,
                   "argument of %qs attribute is not a string constant",
                   IDENTIFIER_POINTER (name));
          *no_add_attrs = true;
        }
      else if (   strcmp (TREE_STRING_POINTER (value), "small") != 0
               && strcmp (TREE_STRING_POINTER (value), "large") != 0)
        {
          warning (OPT_Wattributes,
                   "argument %qs of %qs is not \"small\" or \"large\"",
                   TREE_STRING_POINTER (value), IDENTIFIER_POINTER (name));
          *no_add_attrs = true;
        }
    }

  return NULL_TREE;
}

tree
metag_merge_decl_attributes (tree decl1, tree decl2)
{
  return merge_decl_attributes (decl1, decl2);
}

tree
metag_merge_type_attributes (tree type1, tree type2)
{
  return merge_type_attributes (type1, type2);
}

int
metag_comp_type_attributes (tree type1, tree type2)
{
  tree m1 = lookup_attribute ("model", TYPE_ATTRIBUTES (type1));
  tree m2 = lookup_attribute ("model", TYPE_ATTRIBUTES (type2));

  if (m1 != NULL_TREE)
    m1 = TREE_VALUE (m1);

  if (m2 != NULL_TREE)
    m2 = TREE_VALUE (m2);

  if (m1 && m2 && TREE_CODE (m1) == STRING_CST && TREE_CODE (m2) == STRING_CST)
    return strcmp (TREE_STRING_POINTER (m1), TREE_STRING_POINTER (m2)) == 0 ? 1 : 0;

  return 1;
}

int
metag_letter_for_const (rtx value)
{
  if (satisfies_constraint_L (value))
    return 'L';
  else if (satisfies_constraint_P (value))
    return 'P';
  else if (satisfies_constraint_K (value))
    return 'K';
  else if (satisfies_constraint_I (value))
    return 'I';
  else if (satisfies_constraint_J (value))
    return 'J';
  else if (satisfies_constraint_M (value))
    return 'M';
  else if (satisfies_constraint_N (value))
    return 'N';
  else
    return 0;
}

bool
metag_const_ok_for_letters_p (rtx value, const char letters[])
{
  char c;

  while ((c = *letters++) != '\0')
    {
      switch (c)
        {
        case 'L':
          if (satisfies_constraint_L (value))
            return true;
          break;
        case 'P':
          if (satisfies_constraint_P (value))
            return true;
          break;
        case 'K':
          if (satisfies_constraint_K (value))
            return true;
          break;
        case 'I':
          if (satisfies_constraint_I (value))
            return true;
          break;
        case 'J':
          if (satisfies_constraint_J (value))
            return true;
          break;
        case 'M':
          if (satisfies_constraint_M (value))
            return true;
          break;
        case 'N':
          if (satisfies_constraint_N (value))
            return true;
          break;
        case 'O':
          switch (*letters)
            {
            case '0':
              letters++;
              if (satisfies_constraint_O0 (value))
                return true;
              break;
            case '1':
              letters++;
              if (satisfies_constraint_O1 (value))
                return true;
              break;
            case '2':
              letters++;
              if (satisfies_constraint_O2 (value))
                return true;
              break;
            case '3':
              letters++;
              if (satisfies_constraint_O3 (value))
                return true;
              break;
            case '4':
              letters++;
              if (satisfies_constraint_O4 (value))
                return true;
              break;
            case '5':
              gcc_unreachable ();
              break;
            case '6':
              gcc_unreachable ();
              break;
            case '7':
              gcc_unreachable ();
              break;
            case '8':
              letters++;
              if (satisfies_constraint_O8 (value))
                return true;
              break;
            case '9':
              gcc_unreachable ();
              break;
            default:
              break;
            }
          break;
        default:
          gcc_unreachable ();
        }
    }

  return false;
}

void
metag_machine_dependent_reorg (void)
{
  return;
}

void
metag_setup_incoming_varargs (CUMULATIVE_ARGS *cum, enum machine_mode mode ATTRIBUTE_UNUSED,
                              tree type, int *pretend_size,
                              int no_rtl ATTRIBUTE_UNUSED)
{
  int narg = ROUND_ADVANCE_CUM (cum->narg, mode, type);  

  cfun->machine->anonymous_args = true;

  if (narg < MAX_METAG_PARM_BYTES)
    {
      *pretend_size = MAX_METAG_PARM_BYTES - narg;
      cfun->machine->anonymous_args_size = *pretend_size;
    }
}

bool
metag_function_ok_for_sibcall (tree fndecl, tree exp)
{
  return (current_function_outgoing_args_size == 0
          && !current_function_calls_alloca
          && !current_function_stdarg
          && current_function_pretend_args_size == 0
          && (fndecl == NULL_TREE || metag_function_ok_for_sibcall_per_os (fndecl, exp)));
}

#define PARM_BYTE_BOUNDARY (PARM_BOUNDARY / BITS_PER_UNIT)

tree
metag_gimplify_va_arg_expr (tree valist, tree type,
                            tree *pre_p ATTRIBUTE_UNUSED,
                            tree *post_p ATTRIBUTE_UNUSED)
{
  tree          ptr;
  tree          valist_type = TREE_TYPE (valist);
  tree          t;
  int           size;
  bool          indirect = pass_by_reference (NULL, TYPE_MODE (type), type, false);
  HOST_WIDE_INT boundary;

  if (indirect)
    type = build_pointer_type (type);

  ptr = build_pointer_type (type);

  boundary = FUNCTION_ARG_BOUNDARY (TYPE_MODE (type), type);

  if (boundary < (int)TYPE_ALIGN (type))
    {
      type = build_variant_type_copy (type);
      TYPE_ALIGN (type) = boundary;
    }

  size = int_size_in_bytes (type);

  boundary /= BITS_PER_UNIT;
  if (boundary > PARM_BYTE_BOUNDARY && size > 0)
    {
      tree u;

      u = fold_convert (valist_type, size_int (size));
      t = build2 (MINUS_EXPR, valist_type, valist, u);

      u = fold_convert (valist_type, size_int (-boundary));
      t = build2 (BIT_AND_EXPR, valist_type, t, u);
    }
  else if (boundary == PARM_BYTE_BOUNDARY && size > 0)
    {
      tree u;

      u = fold_convert (valist_type, size_int (PARM_BYTE_BOUNDARY));
      t = build2 (MINUS_EXPR, valist_type, valist, u);
    }
  else if (boundary == PARM_BYTE_BOUNDARY && size == 0)
    t = valist;
  else
    gcc_unreachable ();

  t = build2 (MODIFY_EXPR, valist_type, valist, t);

  t = fold_convert (ptr, t);

  if (indirect)
    t = build_va_arg_indirect_ref (t);

  return build_va_arg_indirect_ref (t);
}

#undef PARM_BYTE_BOUNDARY

/* True if MODE is valid for the target.  By "valid", we mean able to
   be manipulated in non-trivial ways.  In particular, this means all
   the arithmetic is supported.

   Currently, TImode is not valid 
   Thus, we return false when PRECISION is 2 * BITS_PER_WORD and
   2 * BITS_PER_WORD isn't equal LONG_LONG_TYPE_SIZE.
*/

bool
metag_scalar_mode_supported_p (enum machine_mode mode)
{
  int precision = GET_MODE_PRECISION (mode);

  switch (GET_MODE_CLASS (mode))
    {
    case MODE_PARTIAL_INT:
    case MODE_INT:
      if (precision == CHAR_TYPE_SIZE)
        return true;
      if (precision == SHORT_TYPE_SIZE)
        return true;
      if (precision == INT_TYPE_SIZE)
        return true;
      if (precision == LONG_TYPE_SIZE)
        return true;
      if (precision == LONG_LONG_TYPE_SIZE)
        return true;
      return false;

    case MODE_FLOAT:
      if (precision == FLOAT_TYPE_SIZE)
        return true;
      if (precision == DOUBLE_TYPE_SIZE)
        return true;
      if (precision == LONG_DOUBLE_TYPE_SIZE)
        return true;
      return false;

    case MODE_DECIMAL_FLOAT:
      return false;

    default:
      gcc_unreachable ();
    }
}

enum reg_class
metag_secondary_reload (bool in_p, rtx x, enum reg_class class,
                        enum machine_mode mode, secondary_reload_info *sri)
{
  return default_secondary_reload (in_p, x, class, mode, sri);
}

enum reg_class
metag_secondary_reload_class (enum reg_class class, enum machine_mode mode, rtx x, bool in_p)
{
  if (in_p)
    {
      if (reg_class_subset_p (FPC_REGS, class))
        {
          switch (GET_CODE (x))
            {
              case SYMBOL_REF:
              case CONST_INT:
              case CONST:
                return GENERAL_REGS;

              case SUBREG:
                if (metag_fpu_resources)
                  {
                    x = SUBREG_REG (x);

                    if (!metag_hard_genreg_op (x, VOIDmode))
                      return D_REGS;
                  }

                break;

              /* We can only reload from memory if it's 32 or 64 bit */
              /* 12bit offsets are not supported for FX registers either */
              case MEM:
                if (mode != SImode && mode != DImode
                    && GET_MODE_CLASS (mode) != MODE_FLOAT)
                  return D_REGS;

                if (GET_CODE (XEXP (x, 0)) == PLUS
                    && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT)
                  {
                    HOST_WIDE_INT offset = INTVAL (XEXP (XEXP (x, 0), 1));

                    if (offset / GET_MODE_SIZE (mode) >= 32
                        || offset / GET_MODE_SIZE (mode) < -32)
                      return D_REGS;
                  }

                if (metag_fpu_resources && SYMBOL_REF_P (XEXP (x, 0)))
                  return D_REGS;

                /* With only single precision hard-float we must not reload
                   direct to FX registers for DFmode values*/
                if (metag_fpu_resources && metag_fpu_single && mode == DFmode)
                  return D_REGS;

                break;

              /* We can reload const_double to FX but only if it's a half precision,
               * otherwise we must use GENERAL_REGS
               */
              case CONST_DOUBLE:
                if (metag_fpu_resources
                    && (GET_MODE_CLASS (mode) != MODE_FLOAT
                        || !metag_fphalf_imm_op (x, mode)
                        || (metag_fpu_single
                            && mode == DFmode)))
                  return D_REGS;

                break;

              /* Theres no connection from A0/A1 to/from FX */
              case REG:
                if (metag_fpu_resources && METAG_ADDR_REG_P (true_regnum (x)))
                  return D_REGS;

                break;

              case PLUS:
                if (metag_fpu_resources && GET_MODE_CLASS (mode) != MODE_FLOAT)
                  return DA_REGS;

                break;

              default:
                break;
            }
        }
      else if (reg_class_subset_p (A0_REGS, class)
               || reg_class_subset_p (A1_REGS, class))
        {
          /* We have some restrictions on copying to A0/A1 with respect
           * to floating point
           */
          switch (GET_CODE (x))
            {
              /* Theres no connection from A0/A1 to/from FX */
              case REG:
                if (metag_fpu_resources && METAG_FPC_REG_P (true_regnum (x)))
                  return D_REGS;

                break;

              default:
                break;
            }
        }
    }
  else
    {
      if (reg_class_subset_p (FPC_REGS, class))
        {
          switch (GET_CODE (x))
            {
              /* Theres no connection from A0/A1 to/from FX */
              case REG:
                if (metag_fpu_resources && METAG_ADDR_REG_P (true_regnum (x)))
                  return D_REGS;

                break;

              case SUBREG:
                if (metag_fpu_resources)
                  {
                    x = SUBREG_REG (x);

                    if (!metag_hard_genreg_op (x, VOIDmode))
                      return D_REGS;
                  }

                break;

              /* We can only reload to memory if it's 32 or 64 bit */
              /* 12bit offsets are not supported for FX registers either */
              case MEM:
                if (mode != SImode && mode != DImode
                    && GET_MODE_CLASS (mode) != MODE_FLOAT)
                  return D_REGS;

                if (GET_CODE (XEXP (x, 0)) == PLUS
                    && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT)
                  {
                    HOST_WIDE_INT offset = INTVAL (XEXP (XEXP (x, 0), 1));

                    if (offset / GET_MODE_SIZE (mode) >= 32
                        || offset / GET_MODE_SIZE (mode) < -32)
                      return D_REGS;
                  }

                if (metag_fpu_resources && SYMBOL_REF_P (XEXP (x, 0)))
                  return D_REGS;

                /* With only single precision hard-float we must not reload
                   direct to FX registers for DFmode values*/
                if (metag_fpu_resources && metag_fpu_single && mode == DFmode)
                  return D_REGS;

                break;

             case PLUS:
                if (metag_fpu_resources && GET_MODE_CLASS (mode) != MODE_FLOAT)
                  return DA_REGS;

                break;

              default:
                break;
            }
        }
      else if (reg_class_subset_p (A0_REGS, class)
               || reg_class_subset_p (A1_REGS, class))
        {
          /* We have some restrictions on copying to A0/A1 with respect
           * to floating point
           */
          switch (GET_CODE (x))
            {
              /* Theres no connection from A0/A1 to/from FX */
              case REG:
                if (metag_fpu_resources && METAG_FPC_REG_P (true_regnum (x)))
                  return D_REGS;

                break;

              default:
                break;
            }
        }

      if (MEM_P (x))
        {
          rtx addr = XEXP (x, 0);

          switch (GET_CODE (addr))
            {
            case PRE_MODIFY:
            case POST_MODIFY:
              addr = XEXP (addr, 0);
              break;
            default:
              break;
          }

        if (GET_CODE (addr) == PLUS)
          {
            rtx op0 = XEXP (addr, 0);
            rtx op1 = XEXP (addr, 1);

            if (REG_P (op0) && REG_P (op1))
              {
                int regno = true_regnum (op0);

                switch (METAG_REGNO_REG_CLASS (regno))
                  {
                  case A0_REGS:
                    if (reg_class_subset_p (A0_REGS, class))
                      {
                        switch (mode)
                          {
                          case QImode:
                          case HImode:
                          case SImode:
                          case SFmode:
                            return nA0_REGS;
                          case V2SFmode:
                            return FPC_REGS;
                          default:
                            break;
                          }

                        return D_REGS;
                      }
                    break;
                  case A1_REGS:
                    if (reg_class_subset_p (A1_REGS, class))
                      {
                        switch (mode)
                          {
                          case QImode:
                          case HImode:
                          case SImode:
                          case SFmode:
                            return nA1_REGS;
                          case V2SFmode:
                            return FPC_REGS;
                          default:
                            break;
                          }

                        return D_REGS;
                      }
                    break;
                  case D0_REGS:
                    if (reg_class_subset_p (D0_REGS, class))
                      {
                        switch (mode)
                          {
                          case QImode:
                          case HImode:
                          case SImode:
                          case SFmode:
                            return nD0_REGS;
                          case V2SFmode:
                            return FPC_REGS;
                          default:
                            break;
                          }

                        return A_REGS;
                      }
                    break;
                  case D1_REGS:
                    if (reg_class_subset_p (D1_REGS, class))
                      {
                        switch (mode)
                          {
                          case QImode:
                          case HImode:
                          case SImode:
                          case SFmode:
                            return nD1_REGS;
                          case V2SFmode:
                            return FPC_REGS;
                          default:
                            break;
                          }

                        return A_REGS;
                      }
                    break;
                  default:
                    break;
                  }
              }
          }
        }
    }

  return NO_REGS;
}

/* Output code to add DELTA to the first argument, and then jump
   to FUNCTION.  Used for C++ multiple inheritance.  */

void
metag_output_mi_thunk (FILE *file, tree thunk ATTRIBUTE_UNUSED,
                       HOST_WIDE_INT delta,
                       HOST_WIDE_INT vcall_offset ATTRIBUTE_UNUSED,
                       tree function)
{
  unsigned int this_regno = MAX_METAG_PARM_REGNUM; /* "this" */

  /* This 1st argument is "this" unless thunk returns the result in memory,
     in which  case there is a hidden 1st argument we contains a pointer to
     where the result should be stored.  */
  if (aggregate_value_p (TREE_TYPE (TREE_TYPE (function)), function))
    this_regno--;

  /* Add delta to "this" */
  if (satisfies_constraint_I (GEN_INT (delta)))
    asm_fprintf (file, "\tADD\t%r, %r, #%wd\n", this_regno, this_regno, delta);
  else if (satisfies_constraint_I (GEN_INT (-delta)))
    asm_fprintf (file, "\tSUB\t%r, %r, #%wd\n", this_regno, this_regno, -delta);
  else if (satisfies_constraint_K (GEN_INT (delta)))
    asm_fprintf (file, "\tADD\t%r, %r, #%wd\n", this_regno, this_regno, delta);
  else if (satisfies_constraint_K (GEN_INT (-delta)))
    asm_fprintf (file, "\tSUB\t%r, %r, #%wd\n", this_regno, this_regno, -delta);
  else if ((delta & 0x0000FFFF) == 0)
    asm_fprintf (file, "\tADDT\t%r, %r, #HI(%wd)\n", this_regno, this_regno, delta);
  else if (((-delta) & 0x0000FFFF) == 0)
    asm_fprintf (file, "\tSUBT\t%r, %r, #HI(%wd)\n", this_regno, this_regno, -delta);
  else if ((delta & 0xFFFF0000) == 0)
    asm_fprintf (file, "\tADD\t%r, %r, #LO(%wd)\n", this_regno, this_regno, delta);
  else if (((-delta) & 0xFFFF0000) == 0)
    asm_fprintf (file, "\tSUB\t%r, %r, #LO(%wd)\n", this_regno, this_regno, -delta);
  else
    {
      asm_fprintf (file, "\tADDT\t%r, %r, #HI(%wd)\n", this_regno, this_regno, delta);
      asm_fprintf (file, "\tADD\t%r, %r, #LO(%wd)\n", this_regno, this_regno, delta);
    }

  /* If needed, add *(*THIS + VCALL_OFFSET) to THIS.  */
  if (vcall_offset != 0)
    {
      unsigned int temp1 = D0_0_REG;
      unsigned int temp2 = D1_0_REG;

      /* Set TEMP1 to *THIS.  */
      asm_fprintf (file, "\tGETD\t%r, [%r]\n", temp1, this_regno);

      /* Load the offset from (TEMP1 + VCALL_OFFSET) in TEMP2 ... */
      if (-128 <= vcall_offset && vcall_offset <= 124)
        asm_fprintf (file, "\tGETD\t%r, [%r+#(%wd)]\n", temp2, temp1, vcall_offset);
      else
        {
          if (satisfies_constraint_I (GEN_INT (vcall_offset)))
            asm_fprintf (file, "\tADD\t%r, %r, #%wd\n", temp1, temp1, vcall_offset);
          else if (satisfies_constraint_I (GEN_INT (-vcall_offset)))
            asm_fprintf (file, "\tSUB\t%r, %r, #%wd\n", temp1, temp1, -vcall_offset);
          else if (satisfies_constraint_K (GEN_INT (vcall_offset)))
            asm_fprintf (file, "\tADD\t%r, %r, #%wd\n", temp1, temp1, vcall_offset);
          else if (satisfies_constraint_K (GEN_INT (-vcall_offset)))
            asm_fprintf (file, "\tSUB\t%r, %r, #%wd\n", temp1, temp1, -vcall_offset);
          else if ((vcall_offset & 0x0000FFFF) == 0)
            asm_fprintf (file, "\tADDT\t%r, %r, #HI(%wd)\n", temp1, temp1, vcall_offset);
          else if (((-vcall_offset) & 0x0000FFFF) == 0)
            asm_fprintf (file, "\tSUBT\t%r, %r, #HI(%wd)\n", temp1, temp1, -vcall_offset);
          else if ((vcall_offset & 0xFFFF0000) == 0)
            asm_fprintf (file, "\tADD\t%r, %r, #LO(%wd)\n", temp1, temp1, vcall_offset);
          else if (((-vcall_offset) & 0xFFFF0000) == 0)
            asm_fprintf (file, "\tSUB\t%r, %r, #LO(%wd)\n", temp1, temp1, -vcall_offset);
          else
            {
              asm_fprintf (file, "\tADDT\t%r, %r, #HI(%wd)\n", temp1, temp1, vcall_offset);
              asm_fprintf (file, "\tADD\t%r, %r, #LO(%wd)\n", temp1, temp1, vcall_offset);
            }

          asm_fprintf (file, "\tGETD\t%r, [%r]\n", temp2, temp1);
        }

      /* ... and add it to THIS.  */
      asm_fprintf (file, "\tADD\t%r, %r, %r\n", this_regno, this_regno, temp2);
    }

  fputs ("\tB\t", file);
  {
    rtx symbol = XEXP (DECL_RTL (function), 0);

    gcc_assert (SYMBOL_REF_P (symbol));

    assemble_name (file, XSTR (symbol, 0));
    if (METAG_FLAG_PIC && !SYMBOL_REF_LOCAL_P (symbol))
      fputs ("@PLT", file);
  }
  fputc ('\n', file);
}

bool
metag_can_output_mi_thunk (tree thunk_decl ATTRIBUTE_UNUSED,
                           HOST_WIDE_INT delta ATTRIBUTE_UNUSED,
                           HOST_WIDE_INT vcall_offset ATTRIBUTE_UNUSED,
                           tree function_decl ATTRIBUTE_UNUSED)
{
  return true;
}

bool
metag_handle_option (size_t code, const char *arg, int value)
{
  switch (code)
    {
    case OPT_mhard_float:
      {
        int length = strlen(arg);

        metag_fpureg_string = "16";
        metag_fpu_resources = 1;

        /* Turn off SIMD float whenever we see a hardfloat option */
        target_flags &= ~MASK_FPU_SIMD;

        if (length == 0)
          metag_fpu_single = 0;
        else if (length == 2 && arg[0] == '=')
          if (arg[1] == 'S')
            metag_fpu_single = 1;
          else if (arg[1] == 'D')
            metag_fpu_single = 0;
          else
            error("-mhard-float takes an optional argument of S or D. E.g. -mhard-float=S");
        /* If set to none then just use soft float */
        else if (length == 5 && strncmp ("none", &arg[1], 4) == 0)
          {
            metag_fpu_resources = 0;
            metag_fpureg_string = "";
            target_flags &= ~MASK_FPU;
          }
        else
          error("-mhard-float takes an optional argument of S or D. E.g. -mhard-float=S");
      }
      break;

    case OPT_msoft_float:
      target_flags &= ~MASK_FPU_SIMD;
      break;

    case OPT_mdsp:
      if (value)
        metag_extreg_string = "8844";
      else
        metag_extreg_string = "0000";
      break;

    case OPT_mwidth_:
      {
        metag_memory_width = strtol (metag_width_string, NULL, 10);
        if (metag_memory_width != 32
            && metag_memory_width != 64)
          error ("Invalid memory width specified. Permitted widths are 32 or 64.");
      }
      break;

    case OPT_mjump_table_branch_:
      {
        if (strncmp (metag_jump_table_string, "short", 5) == 0)
          metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_SHORT;
        else if (strncmp (metag_jump_table_string, "long", 4) == 0)
          metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_LONG;
        else if (strncmp (metag_jump_table_string, "auto", 4) == 0)
          metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_AUTO;
        else
          error ("-mjump-table-branch must be either 'long', 'short' or 'auto'");
      }
      break;

    case OPT_mcond_exec:
      return true;

    case OPT_mextensions_:
      {
        char* extension = (char*)metag_extensions_string;
        int extension_length = 0;
        unsigned int i;
        char* comma_position;

        for (i = 0 ; i < strlen(extension) ; i++)
          extension[i] = tolower (extension[i]);

        while (strlen(extension) != 0)
          {
            comma_position = strchr (metag_extensions_string, ',');

            if (comma_position == NULL)
              extension_length = strlen (extension);
            else
              extension_length = (comma_position-extension);

            if (strncmp (extension, "bex", (extension_length > 3) ? extension_length : 3) == 0)
              metag_meta2_bex_enabled = true;
            else /* Print the rest of the list in the warning */
              warning (0, "Instruction set extension not known in list: %s", extension);

            extension += extension_length;
          }
      }
      break;

    case OPT_mtbictxsave:
        metag_force_tbictxsave = value;
      break;

    case OPT_mhwtrace:
      if (!value)
        {
          target_flags &= ~MASK_HWTRACE_RETPC;
          target_flags &= ~MASK_HWTRACE_LEAF;
        }
      break;

    case OPT_mhwtrace_retpc:
    case OPT_mhwtrace_leaf:
      if (value)
        target_flags |= MASK_HWTRACE;
      break;

    default:
      break;
    }

  return metag_handle_option_per_os (code, arg, value);
}

bool
metag_zeroextract_mask_p (rtx op0, rtx op1)
{
  rtx value = GEN_INT (((1 << INTVAL (op0)) - 1) << INTVAL (op1));

  return satisfies_constraint_I (value) ||
         satisfies_constraint_P (value) ||
         satisfies_constraint_K (value) ||
         satisfies_constraint_J (value) ||
         satisfies_constraint_M (value) ||
         satisfies_constraint_N (value);
}

rtx
metag_return_addr_rtx (int count, rtx frame ATTRIBUTE_UNUSED)
{
  if (count != 0)
    return NULL_RTX;

  return get_hard_reg_initial_val (Pmode, RETURN_POINTER_REGNUM);
}

HOST_WIDE_INT
metag_function_arg_boundary (enum machine_mode mode, tree type)
{
  HOST_WIDE_INT size;

  if (type != NULL_TREE)
    size = int_size_in_bytes (type);
  else
    size = GET_MODE_BITSIZE (mode);

  if (size < 0)
    return BIGGEST_ALIGNMENT;
  else if (size > UNITS_PER_WORD)
    return BIGGEST_ALIGNMENT;
  else
    return PARM_BOUNDARY;
}

int
metag_first_parm_offset (tree fndecl ATTRIBUTE_UNUSED)
{
  if (cfun->machine->anonymous_args)
    return current_function_pretend_args_size
           - ALIGN_ON_STACK_BOUNDARY (current_function_pretend_args_size);

  return 0;
}

/* WORK NEEDED: Check that the condition for consumer is not always or never */

bool
metag_consumer_is_cond_p (rtx producer, rtx consumer)
{
  return (GET_CODE (PATTERN (consumer)) == COND_EXEC)
         && (get_attr_ccstate (producer) == CCSTATE_SET);
}

bool
metag_bypass_before_reload_p (rtx producer ATTRIBUTE_UNUSED, rtx consumer ATTRIBUTE_UNUSED)
{
  return !reload_in_progress && !reload_completed;
}

int
metag_consumer_stalls_from_load_multi (rtx producer, rtx consumer)
{
  int          opno;
  bool         firstoperand = true;
  unsigned int penultimate_reg = INVALID_REGNUM;
  unsigned int last_reg = INVALID_REGNUM;
  int          stalls = 0; /* Zero stalls to begin with */

  /* Extract the insn to iterate over the output operands */
  extract_insn (producer);

  for (opno = 0 ; opno < recog_data.n_operands ; opno++)
    {
      if (recog_data.operand_type[opno] == OP_OUT)
        {
          rtx op;

          /* Ignore the first output as it refers to the offset addition */
          if (firstoperand)
            {
              firstoperand = false;
              continue;
            }

          /* Find the output register number*/
          op = recog_data.operand[opno];
          if (SUBREG_P (op))
            op = SUBREG_REG (op);

          if (REG_P (op))
            {
              unsigned int regno = REGNO (op);

              if (IS_HARD_OR_VIRT_REGNO (regno))
                {
                  /* Save the two highest numbered load destination register numbers.  */
                  if (last_reg == INVALID_REGNUM || regno > last_reg)
                    {
                      penultimate_reg = last_reg;
                      last_reg = regno;
                    }
                  else if (penultimate_reg == INVALID_REGNUM || regno > penultimate_reg)
                    penultimate_reg = regno;
                }
            }
        }
    }

  /* Both penultimate_reg and last_reg should have values now, if not there
   * is a big problem.  */
   gcc_assert (penultimate_reg != INVALID_REGNUM && last_reg != INVALID_REGNUM);
 
   /* Extract the consumer to examine its operands.  */
   extract_insn (consumer);

   /* If the consumer is an MSET it has to be handled differently... */
   if (store_multiop (PATTERN (consumer), VOIDmode))
     {
       unsigned int first_reg  = INVALID_REGNUM;
       unsigned int second_reg = INVALID_REGNUM;

       /* For MSET instructions stalls only occur if:
        * 1) The penultimate load collides with the initial store
        * 2) The last load collides with the initial store
        * 3) The last load collides with the 2nd store
        */
       firstoperand = true;
       for (opno = 0 ; opno < recog_data.n_operands ; opno++)
         {
           if (recog_data.operand_type[opno] == OP_IN)
             {
               rtx op;

               /* Ignore the first input as it refers to the offset
                * addition.  */
               if (firstoperand)
                 {
                   firstoperand = false;
                   continue;
                 }

               /* Find the input register number.  */
               op = recog_data.operand[opno];
               if (SUBREG_P (op))
                 op = SUBREG_REG (op);

               if (REG_P (op))
                 {
                   unsigned int regno = REGNO (op);

                   if (IS_HARD_OR_VIRT_REGNO (regno))
                     {
                       /* Save the two lowest numbered store source register numbers.  */
                       if (first_reg == INVALID_REGNUM || regno < first_reg)
                         {
                           second_reg = first_reg;
                           first_reg = regno;
                         }
                       else if (second_reg == INVALID_REGNUM || regno < second_reg)
                         second_reg = regno;
                     }
                 }
             }
         }

       /* First and second regs should have valid numbers now.  */
       gcc_assert (first_reg != INVALID_REGNUM
                   && second_reg != INVALID_REGNUM);

       if (last_reg == first_reg)
         stalls = 2;
       else if (last_reg == second_reg || penultimate_reg == first_reg)
         stalls = 1;
     }
   else
     {
       /* For anything other than MSET we examine all input operands
        * and return a stall count if any operand collides with the
        * penultimate or last value loaded.  */
       for (opno = 0 ; opno < recog_data.n_operands ; opno++)
         {
           if (recog_data.operand_type[opno] == OP_IN)
             {
               rtx op;

               /* Find the register number.  */
               op = recog_data.operand[opno];
               if (SUBREG_P (op))
                 op = SUBREG_REG (op);

               if (REG_P (op))
                 {
                   unsigned int regno = REGNO (op);

                   if (IS_HARD_OR_VIRT_REGNO (regno))
                     {
                       /* 
                        * The consumer will stall for 2 cycles if it uses the last
                        * register and 1 cycle if it uses the penultimate
                        * register.
                        */
                       if (regno == last_reg)
                         {
                           stalls = 2;
                           /* Maximum stalls, no point checking further.  */
                           break;
                         }
                       else if (regno == penultimate_reg)
                         stalls = 1;
                     }
                 }
             }
         }
     }
   
   return stalls;
 }

/* Detect if any of the output registers of producer are used
 * as an O2R input of consumer
 *
 * op1 identifies the operand that may be o2r
 * op2 identifies the operand to check for a mismatch in units.  */

bool
metag_consumer_is_o2r (rtx producer, rtx consumer, int op1, int op2)
{
  enum reg_class o2rregclass = NO_REGS;
  enum reg_class op2regclass = NO_REGS;
  unsigned int   o2rregno    = INVALID_REGNUM;
  unsigned int   op2regno    = INVALID_REGNUM;
  rtx            op;

  /* The registers aren't known until reload has completed. */

  if (!reload_completed)
    return false;

  extract_insn (consumer);

  if (recog_data.n_operands <= ((op1 > op2) ? op1 : op2)
      || recog_data.operand_type[op1] == OP_OUT)
    {
      /* Bad insn pattern. o2rhint attribute incorrect
       * o2rhint should say which operands must be in different units for
       * insn to have an O2R operand generated. both operands must be
       * IN or INOUT.  */
      gcc_unreachable ();
    }

  /* Find the register class for op2.  */
  op = recog_data.operand[op2];
  if (SUBREG_P (op))
    op = SUBREG_REG (op);

  if (REG_P (op))
    op2regno = REGNO (op);

  /* We should know the registers at this point. */
  gcc_assert (IS_HARD_OR_VIRT_REGNO (op2regno));

  op2regclass = METAG_REGNO_REG_CLASS (op2regno);

  /* Find the register number for op1.  */
  op = recog_data.operand[op1];
  if (SUBREG_P (op))
    op = SUBREG_REG (op);

  if (REG_P (op))
    o2rregno = REGNO (op);

  /* We should know the register at this point. */
  gcc_assert (IS_HARD_OR_VIRT_REGNO (o2rregno));

  o2rregclass = METAG_REGNO_REG_CLASS (o2rregno);

  /* Check if op1 and op2 have different class registers.  */
  if (op2regclass != o2rregclass)
    {
      int opno;

      /* We have an O2R operand in op1, check it against all OUT / INOUT
       * operands of producer.  */
      extract_insn (producer);

      for (opno = 0 ; opno < recog_data.n_operands ; opno++)
        {
          if (recog_data.operand_type[opno] != OP_INOUT)
            {
              op = recog_data.operand[opno];
              if (SUBREG_P (op))
                op = SUBREG_REG (op);

              if (REG_P (op))
                {
                  unsigned int regno = REGNO (op);

                  /* We should know the register at this point. */   
                  gcc_assert (IS_HARD_OR_VIRT_REGNO (regno));

                  if  (regno == o2rregno)
                     return true; /* Producer has output reg that feeds o2r
                                     operand in consumer.  */
                }
             }
         }
     }

   return false;
}

bool
metag_hard_regno_rename_ok_p (rtx insn,
                              unsigned int from,
                              unsigned int to)
{
  /* Insn must have been recognised and rename permitted */
  if (INSN_CODE (insn) >= 0 && get_attr_rename (insn))
    {
      if (cfun->machine->ech_ctx_required)
        /* Where an ECH context has been allocated, allow any
           hard register to be renamed */
        return from <= LAST_ADDR_REG && to <= LAST_ADDR_REG;
      else
        {
          /* When an ECH context has not been allocated, restrict
             rename to the GP register set, regardless of what
             registers are available */
          return from <= LAST_ADDR_REG
                 && ((to < FIRST_ECH_DATA_REG)
                     || (to >= FIRST_ADDR_REG
                         && to < FIRST_ECH_ADDR_REG));
        }
    }
  else
    return false;
}

enum reg_class
metag_regno_reg_class_minimal (unsigned int regno)
{
  if (regno == D0_0_REG || regno == D0_1_REG)
    return Ye_REGS;

  if (regno == D1_0_REG || regno == D1_1_REG)
    return Yf_REGS;

  if (regno == A0_0_REG || regno == A0_1_REG)
    return Yh_REGS;

  if (regno == A1_0_REG || regno == A1_1_REG)
    return Yl_REGS;

  if (regno == A0_2_REG || regno == A0_3_REG)
    return WQh_REGS;

  if (regno == A1_2_REG || regno == A1_3_REG)
    return WQl_REGS;

  if (regno == ARG_POINTER_REGNUM || regno == FRAME_POINTER_REGNUM)
    return Yh_REGS;

  return metag_regno_reg_class_unit (regno);
}

enum reg_class
metag_regno_reg_class_unit (unsigned int regno)
{
  if (regno == HARD_FRAME_POINTER_REGNUM
      || regno == STACK_POINTER_REGNUM
      || regno == ARG_POINTER_REGNUM
      || regno == FRAME_POINTER_REGNUM)
    return A0_REGS;

  if (METAG_DATA_REG_P (regno))
    return (regno & 1) ? D1_REGS : D0_REGS;

  if (METAG_ADDR_REG_P (regno))
    return (regno & 1) ? A1_REGS : A0_REGS;

  if (METAG_FPC_REG_P (regno))
    return FPC_REGS;

  if (regno == CPC0_REG)
    return A0_REGS;

  if (regno == CPC1_REG)
    return A1_REGS;

  if (regno == PC_REG)
    return A_REGS;

  if (regno == TXRPT_REG || regno == TTREC_REG)
    return Wx_REGS;

  return NO_REGS;
}

/* Make the last instruction frame related and note that it performs
   the operation described by FRAME_PATTERN.  */

static void
metag_set_frame_expr (rtx frame_pattern)
{
  rtx insn = get_last_insn ();

  RTX_FRAME_RELATED_P (insn) = 1;
  if (frame_pattern != NULL_RTX)
    REG_NOTES (insn) = alloc_EXPR_LIST (REG_FRAME_RELATED_EXPR,
                                        frame_pattern,
                                        REG_NOTES (insn));
}

/* Make the last instruction frame related.  */

static void
metag_set_frame_related (void)
{
  metag_set_frame_expr (NULL_RTX);
}

static void
metag_push_frameregs (unsigned extras, unsigned int first_reg, unsigned int last_reg)
{
  rtx          basemem   = gen_rtx_MEM (DImode, stack_pointer_rtx);
  unsigned int count     = 0;
  unsigned int RegList[8];
  unsigned int *pRegList = RegList;

  {
    unsigned int regno;

    for (regno = first_reg; regno <= last_reg; regno += 2)
      if ((extras & REGNO_BIT (regno)) != 0)
        {
          gcc_assert (pRegList < &RegList[sizeof(RegList)/sizeof(RegList[0])]);

          *pRegList++ = regno;
        }
  }

  count = pRegList - RegList;
  gcc_assert (0 < count && count <= sizeof(RegList)/sizeof(RegList[0]));

  if (count == 1)
    {
      rtx frame_expr = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (3));
      rtx set;

      emit_insn (gen_rtx_SET (VOIDmode,
                              gen_rtx_MEM (DImode,
                                           gen_rtx_POST_INC (Pmode,
                                                             stack_pointer_rtx)),
                              gen_rtx_REG (DImode, RegList[0])));

      /* Describe the above instruction to the Drwarf2 call frame unwinder. */
      set = gen_rtx_SET (VOIDmode,
                         gen_rtx_MEM (SImode, stack_pointer_rtx),
                         gen_rtx_REG (SImode, RegList[0]));
      XVECEXP (frame_expr, 0, 0) = set;
      RTX_FRAME_RELATED_P (set) = 1;

      set = gen_rtx_SET (VOIDmode,
                         gen_rtx_MEM (SImode,
                                      plus_constant (stack_pointer_rtx,
                                                     UNITS_PER_WORD)),
                         gen_rtx_REG (SImode, RegList[0] + 1));
      XVECEXP (frame_expr, 0, 1) = set;
      RTX_FRAME_RELATED_P (set) = 1;

      set = gen_rtx_SET (VOIDmode,
                         stack_pointer_rtx,
                         plus_constant (stack_pointer_rtx,
                                        UNITS_PER_WORD * 2));
      XVECEXP (frame_expr, 0, 2) = set;
      RTX_FRAME_RELATED_P (set) = 1;

      metag_set_frame_expr (frame_expr);
    }
  else
    {
      rtx result     = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (1 + count));
      rtx frame_expr = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (1 + count * 2));
      rtx set;

      set = gen_rtx_SET (VOIDmode,
                         stack_pointer_rtx,
                         plus_constant (stack_pointer_rtx,
                                        count * UNITS_PER_WORD * 2));
      XVECEXP (result, 0, 0) = set;

      set = copy_rtx (set);
      XVECEXP (frame_expr, 0, count * 2) = set;
      RTX_FRAME_RELATED_P (set) = 1;

      {
        unsigned int offset = 0;
        unsigned int i;

        for (i = 0; i < count; i++)
          {
            rtx addr = plus_constant (stack_pointer_rtx, i * UNITS_PER_WORD * 2);
            rtx mem  = adjust_automodify_address_nv (basemem, DImode, addr, offset);
            rtx set;
            int j;

            set = gen_rtx_SET (VOIDmode, mem, gen_rtx_REG (DImode, RegList[i]));
            XVECEXP (result, 0, 1 + i) = set;

            for (j = 0; j < 2; j++)
              {
                set = gen_rtx_SET (VOIDmode,
                                   gen_rtx_MEM (SImode, 
                                                plus_constant (stack_pointer_rtx,
                                                               offset)),
                                   gen_rtx_REG (SImode, RegList[i] + j));
                XVECEXP (frame_expr, 0, i * 2 + j) = set;
                RTX_FRAME_RELATED_P (set) = 1;

                offset += UNITS_PER_WORD;
              }
          }

        emit_insn (result);

        metag_set_frame_expr (frame_expr);
      }
    }
}

void
metag_expand_prologue (void)
{
  unsigned int  savesize_gp      = 0;
  unsigned int  savesize_eh      = 0;
  unsigned int  FP_SP_offset     = 0;
  HOST_WIDE_INT size             = get_frame_size ();
  unsigned int  pretend_size     = ALIGN_ON_STACK_BOUNDARY (current_function_pretend_args_size);
  unsigned int  pretend_regs     = pretend_size / UNITS_PER_WORD;
  unsigned int  extras_gp        = 0;
  unsigned int  extras_eh        = 0;
  unsigned int  ech_ctx          = 0;
  bool          non_leaf         = metag_non_leaf_function_p ();
  rtx           use              = NULL_RTX;
  bool          loads_pic_register;

  /* Round size of local stack to preserve 64-bit alignments */
  size = ALIGN_ON_STACK_BOUNDARY (size + current_function_outgoing_args_size);

  /* Make pretend regs into the first non-varargs register number */
  pretend_regs += MIN_METAG_PARM_REGNUM;

  {
    unsigned int regno;

    for (regno = MIN_METAG_PARM_REGNUM; regno <= MAX_METAG_CSAVE_REGNUM; regno += 2)
      {
        if (regno < pretend_regs
            || (!call_used_regs[regno]
                && (df_regs_ever_live_p (regno + 0) || df_regs_ever_live_p (regno + 1))))
          {
            /* Push this data register */
            savesize_gp += UNITS_PER_WORD * 2;
            extras_gp |= REGNO_BIT (regno);

            if (regno >= MIN_METAG_CSAVE_REGNUM)
              FP_SP_offset += UNITS_PER_WORD * 2;
          }
       }
  }

  /* Adjust the saved registers for ECH support */
  ech_ctx = metag_adjust_savesize_ech (&savesize_gp, &extras_gp, &FP_SP_offset);

  /* Recover original pretend regs */
  pretend_regs -= MIN_METAG_PARM_REGNUM;

  if (cfun->tail_call_emit && cfun->machine->hwtrace_leaf)
    {
      /* The entry point of a function that ends in a tail call needs to be
         marked up when tracing all functions */

      emit_insn (gen_ttmov_si (gen_rtx_REG (SImode, TEMP_D0FRT_REGNUM),
                               gen_rtx_REG (SImode, RETURN_POINTER_REGNUM)));
    }
  else if (!frame_pointer_needed && cfun->machine->hwtrace_leaf)
    {
      /* The entry point of all functions need to be marked up when tracing
         all functions regardless of whether the frame pointer is omitted.
         Handle the case where the frame pointer is omitted here. See below
         for other case. */

      emit_insn (gen_ttmov_si (gen_rtx_REG (SImode, TEMP_D0FRT_REGNUM),
                               hard_frame_pointer_rtx));

      /* This is NOT frame related because there is no frame pointer really */
    }

  if (frame_pointer_needed || non_leaf)
    {
      if (non_leaf)
        extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);

      /* Save return address and maybe frame pointer via {D0Frt,D1RtP} pair */
      savesize_gp  += UNITS_PER_WORD * 2;
      FP_SP_offset += UNITS_PER_WORD * 2;

      if (frame_pointer_needed)
        {
          /* Need to spill A0FrP ready for saving and calc new frame */
          if (cfun->machine->hwtrace)
          {
            rtx frame_expr;
            /* Mark up function entry when frame pointer is not omitted, for all
               types of H/W tracing */
            emit_insn (gen_ttmov_si (gen_rtx_REG (SImode, TEMP_D0FRT_REGNUM),
                                     hard_frame_pointer_rtx));
            frame_expr = gen_rtx_SET (VOIDmode, gen_rtx_REG (SImode, TEMP_D0FRT_REGNUM), 
                                      hard_frame_pointer_rtx);
            RTX_FRAME_RELATED_P (frame_expr) = 1;
            metag_set_frame_expr (frame_expr);
          }
          else
            {
              emit_insn (gen_rtx_SET (VOIDmode,
                                      gen_rtx_REG (SImode, TEMP_D0FRT_REGNUM),
                                      hard_frame_pointer_rtx));

              metag_set_frame_related ();
            }

          /* Save return address and frame pointer via (D0FrT,D1RtP) pair */
          extras_gp |= REGNO_BIT (TEMP_D0FRT_REGNUM);

          emit_insn (gen_addsi3 (hard_frame_pointer_rtx,
                                 stack_pointer_rtx,
                                 gen_int_mode (pretend_size, SImode)));
          metag_set_frame_related ();

          use = hard_frame_pointer_rtx;
        }
    }
  else if (df_regs_ever_live_p (RETURN_POINTER_REGNUM))
    {
      extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);

      /* Have to do at least ine pop */
      savesize_gp += UNITS_PER_WORD * 2;

      if (RETURN_POINTER_REGNUM >= MIN_METAG_CSAVE_REGNUM)
        FP_SP_offset += UNITS_PER_WORD * 2;
    }

  if (current_function_calls_eh_return)
    {
      unsigned int n;

      for (n = 0; n < NUM_EH_RETURN_DATA_REGS; n++)
        {
          unsigned int regno = EH_RETURN_DATA_REGNO (n);

          if (regno != INVALID_REGNUM)
            {
              unsigned int regbit = REGNO_BIT (regno);

              if ((extras_eh & regbit) == 0)
                {
                  extras_eh |= regbit;
                  savesize_eh  += UNITS_PER_WORD * 2;
                  FP_SP_offset += UNITS_PER_WORD * 2;
                }
            }
        }
    }

  loads_pic_register = METAG_CURRENT_FUNCTION_LOADS_PIC_REGISTER ();
  if (loads_pic_register)
    FP_SP_offset += UNITS_PER_WORD * 2;/* Save PIC register. */

  /* Sanity checks between initial_elimination and prologue. If these
     tests fail then the generated code will be wrong so abort. */

  gcc_assert (cfun->machine->valid);

  gcc_assert (cfun->machine->savesize_gp           == savesize_gp);
  gcc_assert (cfun->machine->savesize_eh           == savesize_eh);
  gcc_assert (cfun->machine->FP_SP_offset          == FP_SP_offset);
  gcc_assert (cfun->machine->frame_pointer_needed  == frame_pointer_needed);
  gcc_assert (cfun->machine->non_leaf              == non_leaf);
  gcc_assert (cfun->machine->extras_gp             == extras_gp);
  gcc_assert (cfun->machine->extras_eh             == extras_eh);
  gcc_assert (cfun->machine->calls_eh_return       == current_function_calls_eh_return);
  gcc_assert (cfun->machine->uses_pic_offset_table == current_function_uses_pic_offset_table);
  gcc_assert (cfun->machine->loads_pic_register    == loads_pic_register);
  gcc_assert (cfun->machine->ech_ctx_required      == (ech_ctx != 0));

  /* When ECH support is enabled, the register pair including D0.8 may be saved
     therefore include it in the initial push */
  if (extras_gp != 0)
    metag_push_frameregs (extras_gp, MIN_METAG_PARM_REGNUM, TARGET_ECH ?
                                                            METAG_ECH_REGNUM :
                                                            MAX_METAG_CSAVE_REGNUM);

  if (extras_eh != 0)
    metag_push_frameregs (extras_eh, EH_RETURN_FIRST_DATA_REG, EH_RETURN_LAST_DATA_REG);

  if (loads_pic_register)
    {
      rtx frame_expr = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (2));
      rtx set;

      emit_insn (gen_rtx_SET (VOIDmode,
                              gen_rtx_MEM (DImode,
                                           gen_rtx_POST_INC (Pmode,
                                                             stack_pointer_rtx)),
                              gen_rtx_UNSPEC (DImode,
                                              gen_rtvec (2,
                                                         hard_frame_pointer_rtx,
                                                         pic_offset_table_rtx),
                                                         UNSPEC_CONCAT)));

      /* Describe the above instruction to the Dwarf2 call frame unwinder. */

      set = gen_rtx_SET (VOIDmode,
                         gen_rtx_MEM (SImode,
                                      plus_constant (stack_pointer_rtx,
                                                     UNITS_PER_WORD)),
                         pic_offset_table_rtx);
      XVECEXP (frame_expr, 0, 0) = set;
      RTX_FRAME_RELATED_P (set) = 1;

      set = gen_rtx_SET (VOIDmode,
                         stack_pointer_rtx,
                         plus_constant (stack_pointer_rtx,
                                        2 * UNITS_PER_WORD));
      XVECEXP (frame_expr, 0, 1) = set;
      RTX_FRAME_RELATED_P (set) = 1;

      metag_set_frame_expr (frame_expr);
    }

  /* Allocate local stack based storage */
  if (size != 0)
    {
      rtx frame_expr = gen_rtx_SET (VOIDmode,
                                    stack_pointer_rtx,
                                    plus_constant (stack_pointer_rtx,
                                                   size));

      emit_insn (gen_addsi3 (stack_pointer_rtx,
                             stack_pointer_rtx,
                             gen_int_mode (size, SImode)));

      metag_set_frame_expr (frame_expr);
    }

  /* Initialise PIC register. */
  if (loads_pic_register)
    {
      emit_insn (gen_load_pic (pic_offset_table_rtx,
                               gen_rtx_REG (SImode, CPC1_REG)));
      emit_insn (gen_prologue_use (pic_offset_table_rtx));
    }

  if (use != NULL_RTX)
    emit_insn (gen_prologue_use (use));

  if (ech_ctx != 0)
    {
      rtx reg_ech = gen_rtx_REG (SImode, METAG_ECH_REGNUM);

      /* Only store context if we used a register. The ECH register will be
         reset to zero by any system supporting ECH contexts prior to
         restoring the context for a process using ECH. 
         
         We must always store the DX8 part of the context as ECH implicitly
         uses D0.8. */

      ech_ctx |= ((TBICTX_XMCC_BIT | TBICTX_XEXT_BIT | TBICTX_XDX8_BIT) << 16);

      /* GCC does not use DSP RAM so the lower 16 bits will be zero */
      gcc_assert ((ech_ctx & 0xFFFF) == 0);

      /* Guaranteed to have bottom 16bits zero, and hence will be set in a
         single instruction */
      emit_insn (gen_rtx_SET (SImode, reg_ech,
                                      gen_int_mode (ech_ctx, SImode)));
      emit_insn (gen_prologue_use (reg_ech));

    }

  emit_insn (gen_blockage ());
}

void
metag_expand_epilogue (bool epilogue_for_tailcall)
{
  unsigned int  savesize_gp      = 0;
  unsigned int  savesize_eh      = 0;
  unsigned int  picsize          = 0;
  unsigned int  num_saved_regs   = 0;
  unsigned int  extras_gp        = 0;
  unsigned int  extras_eh        = 0;
  unsigned int  ech_ctx          = 0;
  bool          non_leaf         = metag_non_leaf_function_p ();
  int           pretend_size     = ALIGN_ON_STACK_BOUNDARY (current_function_pretend_args_size);
  HOST_WIDE_INT size             = get_frame_size ();
  unsigned int  RegList[8];
  unsigned int *pRegList = RegList;
  bool          loads_pic_register;

  emit_insn (gen_blockage ());

  /* Round size of local stack to preserve 64-bit alignments */
  size = ALIGN_ON_STACK_BOUNDARY (size + current_function_outgoing_args_size);

  /* Calculate number of registers to restore 
     if they contain pretend args save all, else save registers with data in them */
  {
    unsigned int regno;

    for (regno = MIN_METAG_CSAVE_REGNUM; regno <= MAX_METAG_CSAVE_REGNUM; regno += 2)
      {
        if (!call_used_regs[regno]
            && (df_regs_ever_live_p (regno + 0) || df_regs_ever_live_p (regno + 1)))
          {
            /* Need to restore all normal Dx register pairs required */
            savesize_gp += UNITS_PER_WORD * 2;
            extras_gp |= REGNO_BIT (regno);
          }
      }
  }

  /* Adjust the saved registers for ECH support */
  ech_ctx = metag_adjust_savesize_ech (&savesize_gp, &extras_gp, NULL);

  /* No registers so far for MGETL|GETL */

  if (frame_pointer_needed || non_leaf)
    {
      /* Restore frame pointer via D0FRT or restore return address */

      if (non_leaf)
        {
          /* Have to get D1RtP back */
          extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);
        }

      if (frame_pointer_needed)
        {
          /* Must restore callers frame pointer */
          extras_gp |= REGNO_BIT (TEMP_D0FRT_REGNUM);
        }

      savesize_gp += UNITS_PER_WORD * 2;
    }
  else if (df_regs_ever_live_p (RETURN_POINTER_REGNUM))
    {
      extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);

      /* Have to do at least one pop */
      savesize_gp += UNITS_PER_WORD * 2;
    }

  if (current_function_calls_eh_return)
    {
      unsigned int n;

      for (n = 0; n < NUM_EH_RETURN_DATA_REGS; n++)
        {
          unsigned int regno = EH_RETURN_DATA_REGNO (n);

          if (regno != INVALID_REGNUM)
            {
              unsigned int regbit = REGNO_BIT (regno);

              if ((extras_eh & regbit) == 0)
                {
                  extras_eh |= regbit;
                  savesize_eh += UNITS_PER_WORD * 2;
                }
            }
        }
    }

  loads_pic_register = METAG_CURRENT_FUNCTION_LOADS_PIC_REGISTER ();
  if (loads_pic_register)
    picsize += UNITS_PER_WORD * 2;

  if (!frame_pointer_needed && size > 16352)
    {
      HOST_WIDE_INT excess = size & 0xFFFF0000;

      /* LCS is too big to use as direct offset for recovery */
      if (excess != 0)
        {
          emit_insn (gen_addsi3 (stack_pointer_rtx,
                                 stack_pointer_rtx,
                                 gen_int_mode (-excess, SImode)));
          size &= 0xFFFF;
        }

      if (size > 16352)
        {
          emit_insn (gen_addsi3 (stack_pointer_rtx,
                                 stack_pointer_rtx,
                                 gen_int_mode (-size, SImode)));
          size = 0;
        }
    }

  if (loads_pic_register)
    {
      if (frame_pointer_needed)
        emit_insn (gen_rtx_SET (VOIDmode,
                                gen_rtx_UNSPEC (DImode,
                                                gen_rtvec (2,
                                                           hard_frame_pointer_rtx,
                                                           pic_offset_table_rtx),
                                                UNSPEC_CONCAT),
                                gen_rtx_MEM (DImode,
                                             plus_constant (hard_frame_pointer_rtx,
                                                            savesize_gp + savesize_eh))));
      else
        emit_insn (gen_rtx_SET (VOIDmode,
                                gen_rtx_UNSPEC (DImode,
                                                gen_rtvec (2,
                                                           hard_frame_pointer_rtx,
                                                           pic_offset_table_rtx),
                                                UNSPEC_CONCAT),
                                gen_rtx_MEM (DImode,
                                             plus_constant (stack_pointer_rtx,
                                                            -(HOST_WIDE_INT)(size + picsize)))));
    }

  if (extras_eh != 0)
    {
      unsigned int delta = 0;
      unsigned int regno;

      for (regno = 0; regno < 32; regno += 2)
        {
          if ((extras_eh & REGNO_BIT (regno)) != 0)
            {
              if (frame_pointer_needed)
                {
                  emit_insn (gen_rtx_SET (VOIDmode,
                                          gen_rtx_REG (DImode, regno),
                                          gen_rtx_MEM (DImode,
                                                       plus_constant (hard_frame_pointer_rtx,
                                                                      savesize_gp + delta))));
                }
              else
                {
                  HOST_WIDE_INT offset = -(HOST_WIDE_INT)(size + picsize + savesize_eh);

                  emit_insn (gen_rtx_SET (VOIDmode,
                                          gen_rtx_REG (DImode, regno),
                                          gen_rtx_MEM (DImode,
                                                       plus_constant (stack_pointer_rtx,
                                                                      offset + delta))));
                }

              delta += UNITS_PER_WORD * 2;
            }
        }
    }

  if (extras_gp != 0)
    {
      unsigned int regno;

      for (regno = MIN_METAG_PARM_REGNUM; regno < 32; regno += 2)
        {
          if ((extras_gp & REGNO_BIT (regno)) != 0)
            {
              HOST_WIDE_INT offset = -(HOST_WIDE_INT)(size + picsize + savesize_eh + savesize_gp);

              gcc_assert (pRegList < &RegList[sizeof(RegList)/sizeof(RegList[0])]);

              if (!frame_pointer_needed
                  && (savesize_gp != UNITS_PER_WORD * 2 || pretend_size != 0 || (size + picsize) >= 256))
                {
                  offset += num_saved_regs * UNITS_PER_WORD * 2;

                  emit_insn (gen_rtx_SET (VOIDmode,
                                          gen_rtx_REG (DImode, regno),
                                          gen_rtx_MEM (DImode,
                                                       plus_constant (stack_pointer_rtx,
                                                                      offset))));
                  pRegList = RegList;
                }
              else if (!frame_pointer_needed)
                {
                  /* Only one pair and no pretend args, pre-decrement stack! */
                  emit_insn (gen_rtx_SET (VOIDmode,
                                          gen_rtx_REG (DImode, regno),
                                          gen_rtx_MEM (DImode,
                                                       gen_rtx_PRE_MODIFY (Pmode,
                                                                           stack_pointer_rtx,
                                                                           plus_constant (stack_pointer_rtx,
                                                                                          offset)))));
                  pRegList = RegList;

                  /* Stack predecremented above */
                  size = -(HOST_WIDE_INT)(savesize_gp + savesize_eh + picsize);
                }
              else
                *pRegList++ = regno;

              /* Count them */
              num_saved_regs++;
            }
        }
    }

  /* Take account of pretend args stored on the stack by the prologue */
  savesize_gp += pretend_size;

  if (pRegList != RegList)
    {
      rtx          basemem = gen_rtx_MEM (DImode, hard_frame_pointer_rtx);
      unsigned int count   = pRegList - RegList;

      gcc_assert (count > 0);

      /* Use frame pointer to recover caller-saved regs */
      if (count == 1)
        {
          /* Only one pair needed, advance frame pointer */
          emit_insn (gen_rtx_SET (VOIDmode,
                                  gen_rtx_REG (DImode, RegList[0]),
                                  gen_rtx_MEM (DImode,
                                               gen_rtx_POST_INC (Pmode,
                                                                 hard_frame_pointer_rtx))));
        }
      else
        {
          rtx          result = gen_rtx_PARALLEL (VOIDmode, rtvec_alloc (1 + count));
          unsigned int offset = 0;
          unsigned int i;

          XVECEXP (result, 0, 0) = gen_rtx_SET (VOIDmode,
                                                hard_frame_pointer_rtx,
                                                plus_constant (hard_frame_pointer_rtx,
                                                               count * UNITS_PER_WORD * 2));

          for (i = 0; i < count; i++)
            {
              rtx addr  = plus_constant (hard_frame_pointer_rtx, i * UNITS_PER_WORD * 2);
              rtx mem   = adjust_automodify_address_nv (basemem, DImode, addr, offset);

              gcc_assert (i <= count);

              XVECEXP (result, 0, 1 + i) = gen_rtx_SET (VOIDmode,
                                                        gen_rtx_REG (DImode, RegList[i]),
                                                        mem);
              offset += UNITS_PER_WORD * 2;
            }

          emit_insn (result);
        }

      /* Use frame pointer to recover stack pointer */
      emit_insn (gen_addsi3 (stack_pointer_rtx,
                             hard_frame_pointer_rtx,
                             gen_int_mode (-savesize_gp, SImode)));
    }
  else if (frame_pointer_needed)
    {
      /* Use unmolested frame pointer to recover stack pointer */
      emit_insn (gen_addsi3 (stack_pointer_rtx,
                             hard_frame_pointer_rtx,
                             gen_int_mode (-pretend_size, SImode)));
    }
  else
    {
      HOST_WIDE_INT adjust = size + picsize + savesize_eh + savesize_gp;

      /* Calculate return stack adjustment directly including savesize */

      if (adjust != 0)
        emit_insn (gen_addsi3 (stack_pointer_rtx,
                               stack_pointer_rtx,
                               gen_int_mode (-adjust, SImode)));
    }

  /* We should be able to insert code here for EPILOGUE delay-slots if
     frame_pointer_needed. The next instruction will stall waiting
     for return data from the MGETL or GETL done above in this case. */

  if (frame_pointer_needed)
    {
      if (!cfun->machine->hwtrace || cfun->machine->hwtrace_retpc)
        {
          /* Restore callers frame pointer. The HWTRACE code below will take
             care of this when enabled, unless the RETPC method is being used,
             in which case it must be done here as normal. */

          emit_insn (gen_rtx_SET (VOIDmode,
                                  hard_frame_pointer_rtx,
                                  gen_rtx_REG (SImode, TEMP_D0FRT_REGNUM)));
        }
    }
  else if ((!cfun->tail_call_emit || !epilogue_for_tailcall)
           && cfun->machine->hwtrace_leaf && cfun->machine->hwtrace_retpc
           && !non_leaf && df_regs_ever_live_p (TEMP_D0FRT_REGNUM))
    {
      /* When tracing all leaf functions with the RETPC method, and we are
         in a leaf function that has clobbered D0FrT and the frame pointer
         is not needed (and this function doesn't end in a sibcall)...
         Then D0FrT must be recovered from the frame pointer ready to be used
         below */

      emit_insn (gen_rtx_SET (VOIDmode,
                              gen_rtx_REG (SImode, TEMP_D0FRT_REGNUM),
                              hard_frame_pointer_rtx));
    }

  if ((!cfun->tail_call_emit || !epilogue_for_tailcall)
      && cfun->machine->hwtrace_retpc)
    {
      /* The RETPC method of tracing is a MOVL TTREC, x,y. Emit one when
         the function does not end in a sibcall*/

      emit_insn (gen_ttrec (gen_rtx_REG (DImode, TTREC_REGNUM),
                            gen_rtx_REG (DImode, TEMP_D0FRT_REGNUM)));
    }
  else if ((!cfun->tail_call_emit || !epilogue_for_tailcall)
           && cfun->machine->hwtrace
           && (frame_pointer_needed || cfun->machine->hwtrace_leaf))
    {
      /* The standard method of tracing is to mark up the FP restore with a
         TTMOV.
         This is done when tracing leaf functions or the frame pointer is
         needed. In the latter case this replaces the normal frame pointer
         restore from above. */

      emit_insn (gen_ttmov_si (hard_frame_pointer_rtx,
                               gen_rtx_REG (SImode, TEMP_D0FRT_REGNUM)));
    }

  /* Returns to exception handlers require an additional stack adjustment. */
  if (current_function_calls_eh_return)
    emit_insn (gen_subsi3 (stack_pointer_rtx, 
                           stack_pointer_rtx,
                           EH_RETURN_STACKADJ_RTX));

  return;
}

/* Return TRUE if it is possible to return using a simple return
   sequence instruction, possibly conditional.  */

bool
metag_use_return_insn (bool cond)
{
  if (! reload_completed)
    return false;

  if (current_function_profile)
    return false;

  if (current_function_calls_alloca)
    return false;

  if (current_function_calls_eh_return)
    return false;

  if (current_function_outgoing_args_size != 0)
    return false;

  if (current_function_pretend_args_size != 0)
    return false;

  if (cfun->machine->anonymous_args_size != 0)
    return false;

  if (cfun->machine->out_local_size != 0)
    return false;

  if (cfun->machine->savesize_gp != 0)
    return false;

  if (cfun->machine->loads_pic_register != 0)
    return false;

  if (cond && cfun->machine->hwtrace)
    return false;

  return true;
}

/* Verify that any instruction that supports conditional execution
 * includes the '%?' placeholder if that instruction has been 
 * marked as conditional.  */
void
metag_asm_output_opcode (FILE *file ATTRIBUTE_UNUSED, const char * opcode)
{
  if (metag_cond_exec_p () || current_insn_predicate != NULL_RTX)
    {
      if (strchr (opcode, '?') == NULL)
        gcc_unreachable ();
    }
}

const char *
metag_invalid_within_doloop (rtx insn)
{
  if (CALL_P (insn))
    return "Function call in the loop.";

  if (INSN_P (insn) && asm_noperands (PATTERN (insn)) >= 0 &&
      regno_clobbered_p (TXRPT_REGNUM, insn, SImode, 0))
    return "In-line assembler clobbering TXRPT.";

  return NULL;
}

/* Return TRUE iff the frame-pointer is required to efficiently restore
 * the stack frame during the epilogue.  */

static bool
metag_frame_pointer_required_for_epilogue_restore (void)
{
  return cfun->machine->frame_pointer_epilogue
         = metag_non_leaf_function_p ()
           && (ALIGN_ON_STACK_BOUNDARY (get_frame_size ()
                                        + current_function_outgoing_args_size) > 16352);
}

/* Return TRUE if the frame-pointer is required.  */

bool
metag_frame_pointer_required (void)
{
  return (current_function_calls_alloca
          || current_function_has_nonlocal_label
          || profile_flag
          || (cfun->machine->valid && cfun->machine->frame_pointer_epilogue)
          || cfun->machine->accesses_prev_frame
          || metag_frame_pointer_required_for_epilogue_restore ());
}

void
metag_setup_frame_addresses (void)
{
  cfun->machine->accesses_prev_frame = true;
}

void
metag_expand_set_return_address (rtx source)
{
  unsigned int  savesize_gp = 0;
  unsigned int  savesize_eh = 0;
  unsigned int  picsize     = 0;
  unsigned int  extras_gp   = 0;
  unsigned int  extras_eh   = 0;
  unsigned int  ech_ctx     = 0;
  bool          non_leaf    = metag_non_leaf_function_p ();
  HOST_WIDE_INT size        = get_frame_size ();
  bool          loads_pic_register;

  /* Round size of local stack to preserve 64-bit alignments */
  size = ALIGN_ON_STACK_BOUNDARY (size + current_function_outgoing_args_size);

  /* Calculate number of registers to restore 
     if they contain pretend args save all, else save registers with data in them */
  {
    unsigned int regno;

    for (regno = MIN_METAG_CSAVE_REGNUM; regno <= MAX_METAG_CSAVE_REGNUM; regno += 2)
      {
        if (!call_used_regs[regno]
            && (df_regs_ever_live_p (regno + 0) || df_regs_ever_live_p (regno + 1)))
          {
            /* Need to restore all normal Dx register pairs required */
            savesize_gp += UNITS_PER_WORD * 2;
            extras_gp |= REGNO_BIT (regno);
          }
      }
  }

  /* Adjust the saved registers for ECH support */
  ech_ctx = metag_adjust_savesize_ech (&savesize_gp, &extras_gp, NULL);

  if (frame_pointer_needed || non_leaf)
    {
      /* Restore frame pointer via D0FRT or restore return address */

      if (non_leaf)
        extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);

      if (frame_pointer_needed)
        extras_gp |= REGNO_BIT (TEMP_D0FRT_REGNUM);

      savesize_gp += UNITS_PER_WORD * 2;
    }
  else if (df_regs_ever_live_p (RETURN_POINTER_REGNUM))
    {
      extras_gp |= REGNO_BIT (RETURN_POINTER_REGNUM);

      /* Have to do at least one pop */
      savesize_gp += UNITS_PER_WORD * 2;
    }

  if (current_function_calls_eh_return)
    {
      unsigned int n;

      for (n = 0; n < NUM_EH_RETURN_DATA_REGS; n++)
        {
          unsigned int regno = EH_RETURN_DATA_REGNO (n);

          if (regno != INVALID_REGNUM)
            {
              unsigned int regbit = REGNO_BIT (regno);

              if ((extras_eh & regbit) == 0)
                {
                  extras_eh |= regbit;
                  savesize_eh += UNITS_PER_WORD * 2;
                }
            }
        }
    }

  loads_pic_register = METAG_CURRENT_FUNCTION_LOADS_PIC_REGISTER ();
  if (loads_pic_register)
    picsize += UNITS_PER_WORD * 2;

  gcc_assert (cfun->machine->valid);
  gcc_assert (cfun->machine->savesize_gp           == savesize_gp);
  gcc_assert (cfun->machine->savesize_eh           == savesize_eh);
  gcc_assert (cfun->machine->extras_gp             == extras_gp);
  gcc_assert (cfun->machine->extras_eh             == extras_eh);
  gcc_assert (cfun->machine->pic_save_size         == picsize);
  gcc_assert (cfun->machine->uses_pic_offset_table == current_function_uses_pic_offset_table);
  gcc_assert (cfun->machine->loads_pic_register    == loads_pic_register);

  if ((extras_gp & REGNO_BIT (RETURN_POINTER_REGNUM)) != 0)
    {
      rtx base;

      /* The return address is @ hard frame pointer + UNITS_PER_WORD */
      if (frame_pointer_needed)
        base = plus_constant (hard_frame_pointer_rtx, UNITS_PER_WORD);
      else
        base = plus_constant (stack_pointer_rtx,
                              -(HOST_WIDE_INT)(size + picsize + savesize_eh + savesize_gp - UNITS_PER_WORD));

      emit_insn (gen_rtx_SET (VOIDmode,
                              gen_rtx_MEM (SImode, base),
                              source));
    }
  else
    emit_insn (gen_rtx_SET (VOIDmode,
                            gen_rtx_REG (SImode, RETURN_POINTER_REGNUM),
                            source));

  return;
}

bool
metag_doloop_loop_nest_optimized (struct loop * loop, struct doloopnest * nest)
{
  for ( ; nest != NULL ; nest = nest->next)
    if (nest->inner == loop)
      return true;

  return false;
}

bool
metag_doloop_check_any_nest_optimized (struct loop * loop, struct doloopnest * nest)
{
  if (loop->inner == NULL)
    /* Check if nest already has an optimized loop */
    return metag_doloop_loop_nest_optimized (loop, nest);
  else
    {
      struct loop* innerloops = loop->inner;

      for ( ; innerloops != NULL ; innerloops = innerloops->next)
        if (metag_doloop_check_any_nest_optimized (innerloops, nest))
          return true;
    }

  return false;
}

void
metag_doloop_mark_nests_optimized (struct loop * loop, struct doloopnest ** nest_p)
{
  /* Check if the nest already has an optimized loop */
  if (loop->inner == NULL)
    {
      while (*nest_p != NULL)
        /* Find the end of the list of nests */
        nest_p = &((*nest_p)->next);
      
      /* Add the new loop nest (innermost loop) to the list */
      *nest_p = (struct doloopnest *)xmalloc (sizeof (struct doloopnest));
      (*nest_p)->next  = NULL;
      (*nest_p)->inner = loop;
    }
  else
    {
      /* Process all inner siblings if this loop is not innermost */
      struct loop* innerloops = loop->inner;

      for ( ; innerloops != NULL ; innerloops = innerloops->next)
        metag_doloop_mark_nests_optimized (innerloops, nest_p);
    }
}

bool
metag_current_function_loads_pic_register (void)
{
#if 0
  /* We need to perform more analysis and house keeping
     before we can optimize the loading of PIC register
     based on local/global properties of functions.

     At present we only mark functions to load the pic
     register if it uses the PIC register, but this
     doesn't interact correctly with the intended local/
     global optimization.

     Consider

         extern int a;

         static void s(void) { a = 1; }

         void g (void) { s (); } 

     We need g() to load/restore the PIC and s() to
     inherit the PIC from g();

  */
  if (flag_unit_at_a_time)
    {
      struct cgraph_local_info * local_info
        = cgraph_local_info (current_function_decl);

      if (local_info && local_info->local)
        return false;
    }
#endif

  return current_function_uses_pic_offset_table;
}

#define MODE_BASE_REG_CLASS(MODE)       (MODE)

rtx
metag_legitimize_reload_address (rtx x, enum machine_mode mode, 
                                 int opnum, int type, 
                                 int ind_levels ATTRIBUTE_UNUSED)
{
  if (GET_CODE (x) == PLUS
      && REG_P (XEXP (x, 0))
      && IS_HARD_OR_VIRT_REGNO (REGNO (XEXP (x, 0)))
      && METAG_LEGITIMATE_REG_P (XEXP (x, 0), TRUE)
      && CONST_INT_P (XEXP (x, 1)))
    {
      if (!strict_memory_address_p (mode, x))
        {
#if 0
          unsigned int  modesize = GET_MODE_SIZE (mode);
          HOST_WIDE_INT value = INTVAL (XEXP (x, 1));
          HOST_WIDE_INT limit_top;
          HOST_WIDE_INT limit_bot;
          HOST_WIDE_INT delta;

          if (metag_reg12bit_op (XEXP (x, 0), mode))
            limit_top = 2048 * modesize;
          else
            limit_top = 32 * modesize;

          limit_bot = -limit_top;
          limit_top = (limit_top - 1) & ~(modesize - 1);

          if (limit_top <= value && value < limit_top + 255)
            delta = limit_top;
          else if (limit_bot - 255 <= value && value < -limit_bot)
            delta = limit_bot;
          else
            delta = 0;
#endif

          x = gen_rtx_PLUS (GET_MODE (x),
                            gen_rtx_PLUS (GET_MODE (x), XEXP (x, 0),
                                          XEXP (x, 1)),
                            const0_rtx);
          push_reload (XEXP (x, 0), NULL_RTX, &XEXP (x, 0), NULL,
                       MODE_BASE_REG_CLASS (mode), GET_MODE (x),
                       VOIDmode, 0, 0, opnum, type);

          return x;
        }
    }

  return NULL_RTX;
}

bool
metag_offset6_mode (rtx op, enum machine_mode mode)
{
  gcc_assert (CONST_INT_P (op));

  switch (mode)
    {
    case DImode:
      return metag_offset6_di (op, mode);

    case SImode:
      return metag_offset6_si (op, mode);

    case HImode:
      return metag_offset6_hi (op, mode);

    case QImode:
      return metag_offset6_qi (op, mode);

    case DFmode:
      return metag_offset6_df (op, mode);

    case SFmode:
      return metag_offset6_sf (op, mode);

    case V2SImode:
      return metag_offset6_v2si (op, mode);

    case V2SFmode:
      return metag_offset6_v2sf (op, mode);

    default:
      gcc_unreachable ();
    }

  return false;
}

bool
metag_offset12_mode (rtx op, enum machine_mode mode)
{
  gcc_assert (CONST_INT_P (op));

  switch (mode)
    {
    case DImode:
      return metag_offset12_di (op, mode);

    case SImode:
      return metag_offset12_si (op, mode);

    case HImode:
      return metag_offset12_hi (op, mode);

    case QImode:
      return metag_offset12_qi (op, mode);

    case DFmode:
      return metag_offset12_df (op, mode);

    case SFmode:
      return metag_offset12_sf (op, mode);

    case V2SImode:
      return metag_offset12_v2si (op, mode);

    case V2SFmode:
      return metag_offset12_v2sf (op, mode);

    default:
      gcc_unreachable ();
    }

  return false;
}

bool
metag_regno12bit_p (unsigned int regno)
{
  return regno == D0_0_REG || regno == D0_1_REG
      || regno == D1_0_REG || regno == D1_1_REG
      || regno == A0_0_REG || regno == A0_1_REG
      || regno == A1_0_REG || regno == A1_1_REG;
}

bool
metag_split_early (void)
{
  return reload_completed;
}

bool
metag_split_hi_lo_sum_early (void)
{
  return reload_completed;
}

void
metag_internal_label (FILE *file, const char *prefix, unsigned long labelno)
{
  if (metag_ccfsm_state == 3
      && (unsigned)metag_target_label == labelno
      && strcmp (prefix, "L") == 0)
    {
      metag_ccfsm_state = 0;
      metag_target_insn = NULL;
    }

  default_internal_label (file, prefix, labelno);
  return;
}

bool
metag_hard_regno_mode_ok (unsigned int regno, enum machine_mode mode)
{
  if (GET_MODE_CLASS (mode) == MODE_CC)
    return regno == MCC_REGNUM;
  else if (regno == MCC_REGNUM)
    return false;

  if (LAST_ADDR_REG < regno && regno < FIRST_FP_REG)
    return mode == SImode;

  if (FIRST_FP_REG <= regno && regno <= LAST_FP_REG)
    {
      /* FP regs can hold anything that fits in a single reg or a pair */
      if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
        return true;
      else
        return ((regno - FIRST_FP_REG) & 1) == 0 && ((regno + HARD_REGNO_NREGS (regno, mode) - 1) <= LAST_FP_REG);
    }

  if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
    return true;
  else
    {
      unsigned int last_permitted_reg = LAST_ADDR_REG;

      /* V2SI can only use data registers */
      if (mode == V2SImode || mode == V2SFmode)
        last_permitted_reg = LAST_DATA_REG;

      return ((regno <= last_permitted_reg && (regno & 1) == 0)) && ((regno + HARD_REGNO_NREGS (regno, mode) - 1) <= last_permitted_reg);
    }

  return false;
}

bool
metag_dsp_ri16_operands (rtx operands[])
{
  int dunit0, dunit2;
  int regno0, regno2;

  /* Reject if not all registers and also in DATA register class. */
  if (!(REG_P (operands[0  ])
        && METAG_DATA_REG_P (REGNO (operands[0  ]))
        && REG_P (operands[0+2])
        && METAG_DATA_REG_P (REGNO (operands[0+2]))
        && GET_CODE (operands[1  ]) == CONST_INT
        && metag_16bit_op (operands[1], GET_MODE (operands[1]))))
    return false;

  dunit0 = METAG_DATAREG_UNIT (REGNO (operands[0  ]));
  dunit2 = METAG_DATAREG_UNIT (REGNO (operands[0+2]));

  regno0 = METAG_DATAREG_REGN (REGNO (operands[0  ]));
  regno2 = METAG_DATAREG_REGN (REGNO (operands[0+2]));

  /* Accept if register units and regno's match.  */
  return (dunit0 != dunit2 && regno0 == regno2);

}

bool
metag_dsp_rri5_operands (rtx operands[])
{
  int dunit0, dunit1, dunit3, dunit4;
  int regno0, regno1, regno3, regno4;

  /* Reject if not all registers and also in DATA register class. */
  if (!(REG_P (operands[0  ])
        && METAG_DATA_REG_P (REGNO (operands[0  ]))
        && REG_P (operands[0+1])
        && METAG_DATA_REG_P (REGNO (operands[0+1]))
        && REG_P (operands[3+0])
        && METAG_DATA_REG_P (REGNO (operands[3+0]))
        && REG_P (operands[3+1])
        && METAG_DATA_REG_P (REGNO (operands[3+1]))
        && GET_CODE (operands[2  ]) == CONST_INT
        && metag_5bit_op (operands[2], GET_MODE (operands[2]))))
    return false;

  dunit0 = METAG_DATAREG_UNIT (REGNO (operands[0  ]));
  dunit1 = METAG_DATAREG_UNIT (REGNO (operands[0+1]));
  dunit3 = METAG_DATAREG_UNIT (REGNO (operands[3  ]));
  dunit4 = METAG_DATAREG_UNIT (REGNO (operands[3+1]));

  regno0 = METAG_DATAREG_REGN (REGNO (operands[0  ]));
  regno1 = METAG_DATAREG_REGN (REGNO (operands[0+1]));
  regno3 = METAG_DATAREG_REGN (REGNO (operands[3  ]));
  regno4 = METAG_DATAREG_REGN (REGNO (operands[3+1]));

  /* Reject if register units don't match. */
  if (dunit0 != dunit1
      || dunit3 != dunit4
      || dunit0 == dunit3)
    return false;

  /* Reject is dest registers don't match. */
  if (regno0 != regno3)
    return false;

  /* Accept if src register match. */
  return (regno1 == regno4);
}

bool
metag_dsp_rrr_operands (rtx operands[], bool commutable)
{
  int dunit0, dunit1, dunit2, dunit3, dunit4, dunit5;
  int regno0, regno1, regno2, regno3, regno4, regno5;

  /* Reject if not operands  DATA registers. */
  if (!(REG_P (operands[0  ])
        && METAG_DATA_REG_P (REGNO (operands[0  ]))
        && REG_P (operands[0+3])
        && METAG_DATA_REG_P (REGNO (operands[0+3]))
        && REG_P (operands[1  ])
        && METAG_DATA_REG_P (REGNO (operands[1  ]))
        && REG_P (operands[1+3])
        && METAG_DATA_REG_P (REGNO (operands[1+3]))
        && REG_P (operands[2])
        && METAG_DATA_REG_P (REGNO (operands[2  ]))
        && REG_P (operands[2+3])
        && METAG_DATA_REG_P (REGNO (operands[2+3]))))
    return false;

  dunit0 = METAG_DATAREG_UNIT (REGNO (operands[0  ]));
  dunit1 = METAG_DATAREG_UNIT (REGNO (operands[1  ]));
  dunit2 = METAG_DATAREG_UNIT (REGNO (operands[2  ]));

  dunit3 = METAG_DATAREG_UNIT (REGNO (operands[0+3]));
  dunit4 = METAG_DATAREG_UNIT (REGNO (operands[1+3]));
  dunit5 = METAG_DATAREG_UNIT (REGNO (operands[2+3]));

  regno0 = METAG_DATAREG_REGN (REGNO (operands[0  ]));
  regno1 = METAG_DATAREG_REGN (REGNO (operands[1  ]));
  regno2 = METAG_DATAREG_REGN (REGNO (operands[2  ]));

  regno3 = METAG_DATAREG_REGN (REGNO (operands[0+3]));
  regno4 = METAG_DATAREG_REGN (REGNO (operands[1+3]));
  regno5 = METAG_DATAREG_REGN (REGNO (operands[2+3]));

  /* Reject if register units don't match. */
  if (dunit0 != dunit1
      || dunit1 != dunit2
      || dunit3 != dunit4
      || dunit4 != dunit5
      || dunit0 == dunit3)
    return false;

  /* Reject is dest registers don't match. */
  if (regno0 != regno3)
    return false;

  /* Accept if src register match. */
  return ((regno1 == regno4 && regno2 == regno5)
          || (commutable
              && (regno1 == regno5 && regno2 == regno4)));
}

bool
metag_dsp_rrr_mov_operands (rtx operands[], bool commutable)
{
  int dunit1, dunit2, dunit4, dunit5, dunit6, dunit7;
  int regno1, regno2, regno4, regno5, regno6, regno7;

  /* Reject if not operands  DATA registers. */
  if (!(REG_P (operands[0  ])
        && REG_P (operands[0+3])
        && REG_P (operands[1  ])
        && METAG_DATA_REG_P (REGNO (operands[1  ]))
        && REG_P (operands[1+3])
        && METAG_DATA_REG_P (REGNO (operands[1+3]))
        && REG_P (operands[2])
        && METAG_DATA_REG_P (REGNO (operands[2  ]))
        && REG_P (operands[2+3])
        && METAG_DATA_REG_P (REGNO (operands[2+3]))
        && REG_P (operands[0+6])
        && METAG_DATA_REG_P (REGNO (operands[0+6]))
        && REG_P (operands[1+6])
        && METAG_DATA_REG_P (REGNO (operands[1+6]))))
    return false;

  dunit1 = METAG_DATAREG_UNIT (REGNO (operands[1  ]));
  dunit2 = METAG_DATAREG_UNIT (REGNO (operands[2  ]));
  dunit4 = METAG_DATAREG_UNIT (REGNO (operands[1+3]));
  dunit5 = METAG_DATAREG_UNIT (REGNO (operands[2+3]));
  dunit6 = METAG_DATAREG_UNIT (REGNO (operands[6  ]));
  dunit7 = METAG_DATAREG_UNIT (REGNO (operands[7  ]));

  regno1 = METAG_DATAREG_REGN (REGNO (operands[1  ]));
  regno2 = METAG_DATAREG_REGN (REGNO (operands[2  ]));
  regno4 = METAG_DATAREG_REGN (REGNO (operands[1+3]));
  regno5 = METAG_DATAREG_REGN (REGNO (operands[2+3]));
  regno6 = METAG_DATAREG_REGN (REGNO (operands[0+6]));
  regno7 = METAG_DATAREG_REGN (REGNO (operands[1+6]));

  /* Reject if register units don't match */
  if (   dunit1 != dunit2
      || dunit1 != dunit6
      || dunit4 != dunit5
      || dunit4 != dunit7
      || dunit6 == dunit7)
    return false;

    /* Reject is dest registers don't match. */
  if (regno6 != regno7)
    return false;

  /* Accept if src register's match. */
  return ((regno1 == regno4 && regno2 == regno5)
          || (commutable
              && (regno1 == regno5 && regno2 == regno4)));
}

bool
metag_dsp_rr_operands (rtx operands[])
{
  int dunit0, dunit1, dunit2, dunit3;
  int regno0, regno1, regno2, regno3;

  /* Reject if not all are registers and also in DATA register class. */
  if (!(REG_P (operands[0  ])
        && METAG_DATA_REG_P (REGNO (operands[0  ]))
        && REG_P (operands[0+2])
        && METAG_DATA_REG_P (REGNO (operands[0+2]))
        && REG_P (operands[1  ])
        && METAG_DATA_REG_P (REGNO (operands[1  ]))
        && REG_P (operands[1+2])
        && METAG_DATA_REG_P (REGNO (operands[1+2]))))
    return false;

  dunit0 = METAG_DATAREG_UNIT (REGNO (operands[0  ]));
  dunit1 = METAG_DATAREG_UNIT (REGNO (operands[1  ]));
  dunit2 = METAG_DATAREG_UNIT (REGNO (operands[0+2]));
  dunit3 = METAG_DATAREG_UNIT (REGNO (operands[1+2]));

  regno0 = METAG_DATAREG_REGN (REGNO (operands[0  ]));
  regno1 = METAG_DATAREG_REGN (REGNO (operands[1  ]));
  regno2 = METAG_DATAREG_REGN (REGNO (operands[0+2]));
  regno3 = METAG_DATAREG_REGN (REGNO (operands[1+2]));

  /* Accept if register units and regno's match.  */
  return (   dunit0 == dunit1
          && dunit2 == dunit3
          && dunit0 != dunit2
          && regno0 == regno2
          && regno1 == regno3);
}

bool
metag_dsp_rr_rr_mov_operands (rtx operands[])
{
  int dunit1, dunit3, dunit4, dunit5;
  int regno1, regno3, regno4, regno5;

  /* Reject if not operands  DATA registers. */
  if (!(REG_P (operands[0  ])
        && REG_P (operands[0+2])
        && REG_P (operands[1  ])
        && METAG_DATA_REG_P (REGNO (operands[1  ]))
        && REG_P (operands[1+2])
        && METAG_DATA_REG_P (REGNO (operands[1+2]))
        && REG_P (operands[0+4])
        && METAG_DATA_REG_P (REGNO (operands[0+4]))
        && REG_P (operands[1+4])
        && METAG_DATA_REG_P (REGNO (operands[1+4]))))
    return false;

  dunit1 = METAG_DATAREG_UNIT (REGNO (operands[1  ]));
  dunit3 = METAG_DATAREG_UNIT (REGNO (operands[1+2]));
  dunit4 = METAG_DATAREG_UNIT (REGNO (operands[4  ]));
  dunit5 = METAG_DATAREG_UNIT (REGNO (operands[5  ]));

  regno1 = METAG_DATAREG_REGN (REGNO (operands[1  ]));
  regno3 = METAG_DATAREG_REGN (REGNO (operands[1+2]));
  regno4 = METAG_DATAREG_REGN (REGNO (operands[0+4]));
  regno5 = METAG_DATAREG_REGN (REGNO (operands[1+4]));

  /* Accept if register units and regno's match.  */
  return (   dunit4 == dunit1
          && dunit5 == dunit3
          && dunit4 != dunit5
          && regno4 == regno5
          && regno5 == regno3
          && regno4 == regno5);
}

/* This function sets operands 2,3 to support both flag setting and non flag
   setting dsp peephole 2 transformations */
void 
metag_dsp_peephole2_rr_convert (rtx operands[])
{
  int adjust = 0;

  if (REGNO (operands[0]) > REGNO (operands[0+2]))
    adjust = 2;

  operands[2] = gen_rtx_REG (V2SImode, REGNO (operands[0 + adjust]));
  operands[3] = gen_rtx_REG (V2SImode, REGNO (operands[1 + adjust]));
}

void 
metag_dsp_peephole2_rr_mov_convert (rtx operands[])
{
  int adjustin = 0;
  int adjustout = 0;

  if (REGNO (operands[1]) > REGNO (operands[1+2]))
    {
      adjustin = 2;
      adjustout = 1;
    }
  operands[4] = gen_rtx_REG (V2SImode, REGNO (operands[4 + adjustout]));
  operands[1] = gen_rtx_REG (V2SImode, REGNO (operands[1 + adjustin]));
}

/* This function sets operands 3,4,5 to support both flag setting and non flag
   setting dsp peephole 2 transformations */
void 
metag_dsp_peephole2_rrr_convert (rtx operands[])
{
  int adjust = 0;

  if (REGNO (operands[0]) > REGNO (operands[0+3]))
    adjust = 3;

  operands[3] = gen_rtx_REG (V2SImode, REGNO (operands[0 + adjust]));
  operands[4] = gen_rtx_REG (V2SImode, REGNO (operands[1 + adjust]));
  operands[5] = gen_rtx_REG (V2SImode, REGNO (operands[2 + adjust]));
}

void 
metag_dsp_peephole2_rrr_mov_convert (rtx operands[])
{
  int adjustin  = 0;
  int adjustout = 0;

  /* Operands 0 and 3 are not related in this case */
  if (REGNO (operands[1]) > REGNO (operands[1+3]))
    {
      adjustin = 3;
      adjustout = 1;
    }
  operands[6] = gen_rtx_REG (V2SImode, REGNO (operands[6 + adjustout]));
  operands[1] = gen_rtx_REG (V2SImode, REGNO (operands[1 + adjustin]));
  operands[2] = gen_rtx_REG (V2SImode, REGNO (operands[2 + adjustin]));
}

/* This function sets operands 2 and 3 to support both flag setting and non flag
   setting dsp peephole 2 transformations */
void 
metag_dsp_peephole2_ri16_convert (rtx operands[])
{
  int adjust = 0;

  if (REGNO (operands[0]) > REGNO (operands[0+2]))
    adjust = 2;

  operands[2] = gen_rtx_REG (V2SImode, REGNO (operands[0 + adjust]));
  operands[3] = gen_rtx_CONST_VECTOR (V2SImode,
                                      gen_rtvec (2, operands[1], operands[1]));
}

/* This function sets operands 3,4,5 to support both flag setting and non flag
   setting dsp peephole 2 transformations */
void 
metag_dsp_peephole2_rri5_convert (rtx operands[])
{
  int adjust = 0;

  if (REGNO (operands[0]) > REGNO (operands[0+3]))
    adjust = 3;

  operands[3] = gen_rtx_REG (V2SImode, REGNO (operands[0 + adjust]));
  operands[4] = gen_rtx_REG (V2SImode, REGNO (operands[1 + adjust]));
  operands[5] = gen_rtx_CONST_VECTOR (V2SImode,
                                      gen_rtvec (2, operands[2], operands[2]));
}

bool
metag_move_valid_p (rtx to, rtx from)
{
  gcc_assert (reload_completed);

  if (REG_P (from))
    {
      if (METAG_FPC_REG_P (REGNO (from)) && METAG_ADDR_REG_P (REGNO (to)))
        return false;
      else if (METAG_ADDR_REG_P (REGNO (from)) && METAG_FPC_REG_P (REGNO (to)))
        return false;
    }
  else if (CONST_INT_P (from))
    {
      if (METAG_FPC_REG_P (REGNO (to)))
        return false;
    }

  return true;
}

#define builtin_define(TXT) cpp_define (pfile, TXT)
#define builtin_assert(TXT) cpp_assert (pfile, TXT)

void
metag_cpu_cpp_builtins (cpp_reader *pfile)
{
  switch (metac_target)
    {
    case METAC_0_1_ID:
      builtin_define ("METAC_0_1");
      break;
    case METAC_1_0_ID:
      builtin_define ("METAC_1_0");
      break;
    case METAC_1_1_ID:
      builtin_define ("METAC_1_1");
      break;
    case METAC_1_2_ID:
      builtin_define ("METAC_1_2");
      break;
    case METAC_2_1_ID:
      builtin_define ("METAC_2_1");
      break;
    }

  switch (metacore)
    {
    case METACORE_METAC_0_1:
      builtin_define ("METAC_TUNE_0_1");
      break;
    case METACORE_METAC_1_0:
      builtin_define ("METAC_TUNE_1_0");
      break;
    case METACORE_METAC_1_1:
      builtin_define ("METAC_TUNE_1_1");
      break;
    case METACORE_METAC_1_2:
      builtin_define ("METAC_TUNE_1_2");
      break;
    case METACORE_METAC_2_1:
      builtin_define ("METAC_TUNE_2_1");
      break;
    }

  if (TARGET_FPU)
    {
      builtin_define ("METAC_FPU_FLOAT");

      if (TARGET_FPU_SIMD)
        builtin_define ("METAC_FPU_LFLOAT");

      if (!metag_fpu_single)
        builtin_define ("METAC_FPU_DOUBLE");
    }

  if (strcmp (metag_charset_string, "basic") == 0)
    builtin_define ("strcmp=strcmpbcs");

  if (TARGET_MINIM)
    builtin_define ("METAC_MINIM_ENC");

  if (metag_memory_width == 32)
    builtin_define ("METAC_MEMW_32");
  else if (metag_memory_width == 64)
    builtin_define ("METAC_MEMW_64");

  builtin_define ("__metag__");
  builtin_define ("__METAG__");
  builtin_define ("__METAG");

  builtin_define ("METAG");

  builtin_assert ("cpu=metag");
  builtin_assert ("machine=metag");
}

void
metag_expand_didf2 (rtx out, rtx in)
{
    rtx dscr          = gen_reg_rtx (DImode);
    rtx dscrhi        = gen_rtx_SUBREG (SImode, dscr, 4);
    rtx fscr2         = gen_reg_rtx (DFmode);
    rtx fscr2hi_as_si = gen_rtx_SUBREG (SImode, fscr2, 4);
    rtx fscr2lo_as_si = gen_rtx_SUBREG (SImode, fscr2, 0);
    rtx operands[1];

    /* Test to see if rs is in the difficult range (> 2^63) */
    emit_move_insn (dscr, in);
    metag_compare_op0 = gen_rtx_AND (SImode, dscrhi, 
                                     gen_int_mode (0x80000000, SImode));
    metag_compare_op1 = const0_rtx;
    gen_metag_compare (NE, operands, 0);

    /* Drop the 2^63 component */
    emit_insn (gen_andsi3 (dscrhi, dscrhi, 
                           gen_int_mode (0x7fffffff, SImode)));

    /* Convert to double */
    emit_insn (gen_floatdidf2 (out, dscr));

    /* Prepare 2^63 in double precision */
    emit_move_insn (fscr2hi_as_si, 
                    gen_int_mode (0x43e00000, SImode));
    emit_move_insn (fscr2lo_as_si, const0_rtx);

    /* Add on the missing 2^63 if requried */
    emit_insn (gen_rtx_SET (VOIDmode, out,
                       gen_rtx_IF_THEN_ELSE (DFmode,
                                             gen_rtx_NE (VOIDmode, operands[0],
                                                         const0_rtx),
                                             gen_rtx_PLUS (DFmode, out, fscr2),
                                             out)));


}

#define PRAGMA_JUMP_TABLE_BRANCH_WARNING()                                                        \
  do {                                                                                            \
    warning (OPT_Wpragmas, "Incorrect syntax for '#pragma mjump-table-branch=small|large|auto'"); \
    return;                                                                                       \
  } while (0)

void
metag_pragma_jump_table_branch (struct cpp_reader* pFile ATTRIBUTE_UNUSED)
{
  tree x;
  const char * option = NULL;

  if (pragma_lex (&x) != CPP_MINUS)
    PRAGMA_JUMP_TABLE_BRANCH_WARNING ();

  if (pragma_lex (&x) != CPP_NAME || strncmp (IDENTIFIER_POINTER (x), "table", 5) != 0)
    PRAGMA_JUMP_TABLE_BRANCH_WARNING ();

  if (pragma_lex (&x) != CPP_MINUS)
    PRAGMA_JUMP_TABLE_BRANCH_WARNING ();

  if (pragma_lex (&x) != CPP_NAME || strncmp (IDENTIFIER_POINTER (x), "branch", 6) != 0)
    PRAGMA_JUMP_TABLE_BRANCH_WARNING ();

  if (pragma_lex (&x) != CPP_EQ)
    PRAGMA_JUMP_TABLE_BRANCH_WARNING ();

  if (pragma_lex (&x) != CPP_NAME)
    PRAGMA_JUMP_TABLE_BRANCH_WARNING ();

  option = IDENTIFIER_POINTER (x);

  if (pragma_lex (&x) != CPP_EOF)
    PRAGMA_JUMP_TABLE_BRANCH_WARNING ();

  if (strncmp (option, "short", 5) == 0)
    metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_SHORT;
  else if (strncmp (option, "long", 4) == 0)
    metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_LONG;
  else if (strncmp (option, "auto", 4) == 0)
    metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_AUTO;
  else if (strncmp (option, "default", 7) == 0)
    {
      if (strncmp (metag_jump_table_string, "short", 5) == 0)
        metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_SHORT;
      else if (strncmp (metag_jump_table_string, "long", 4) == 0)
        metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_LONG;
      else if (strncmp (metag_jump_table_string, "auto", 4) == 0)
        metag_jump_table_branch = METAG_MINIM_JUMP_TABLE_BRANCH_AUTO;
    }
  else
    PRAGMA_JUMP_TABLE_BRANCH_WARNING ();
}

#define PRAGMA_HWTRACE_SYNTAX_ERROR()                                                           \
  do {                                                                                          \
    warning (OPT_Wpragmas, "Incorrect syntax for '#pragma hwtrace_function (func|*,0|1)");      \
    return;                                                                                     \
  } while (0)


void
metag_pragma_hwtrace_function (struct cpp_reader* pFile ATTRIBUTE_UNUSED)
{
  tree x;
  const char * name = NULL;
  int onoff;

  if (pragma_lex (&x) != CPP_OPEN_PAREN)
    PRAGMA_HWTRACE_SYNTAX_ERROR ();
  
  switch (pragma_lex (&x))
    {
    case CPP_MULT:
      name = "*";
      break;
    case CPP_NAME:
      name = IDENTIFIER_POINTER (x);
      break;
    default:
      PRAGMA_HWTRACE_SYNTAX_ERROR ();
    }

  if (pragma_lex (&x) != CPP_COMMA)
    PRAGMA_HWTRACE_SYNTAX_ERROR ();

  if (pragma_lex (&x) != CPP_NUMBER)
    PRAGMA_HWTRACE_SYNTAX_ERROR ();

  if (TREE_CODE (x) != INTEGER_CST)
    PRAGMA_HWTRACE_SYNTAX_ERROR ();

  onoff = TREE_INT_CST_LOW (x);

  if (pragma_lex (&x) != CPP_CLOSE_PAREN)
    PRAGMA_HWTRACE_SYNTAX_ERROR ();

  if (pragma_lex (&x) != CPP_EOF)
    PRAGMA_HWTRACE_SYNTAX_ERROR ();

  if (strcmp (name, "*") == 0)
    hwtrace_function_default = onoff ? 1 : 0;
  else
    {
      hwtrace_fn * new_fn = xmalloc (sizeof (hwtrace_fn));

      new_fn->name = xstrdup (name);
      new_fn->onoff = onoff ? 1 : 0;
      new_fn->next = hwtrace_function_list;
  
      hwtrace_function_list = new_fn;
    }
}

/* Determine if 'label_ref' refers to a label in the current function */
void 
metag_can_use_short_branch (void)
{
  /* Once we have determined that we can use a short branch there is no need
     to check again. We re-do the check if only long branches are allowed
     even though the decision will not change */
  if (!cfun->machine->can_use_short_branch)
    {
      /* Do the analysis */
      rtx insn = next_active_insn (get_insns());
      int count = 0;

      while (insn)
        {
          rtx body = PATTERN (insn);
          /* Inline assembler... Take a best guess at the instruction count
             then multiply by 8 to assume all insns need long encodings */
          if (GET_CODE (body) == ASM_INPUT || asm_noperands (body) >= 0)
            count += (metag_asm_insn_count (body) << 3);
          else if (GET_CODE (body) == ADDR_DIFF_VEC)
            {
              int i;
              /* Add in the branches in jump tables */
              for (i = 0 ; i < XVECLEN (body, 1) ; i++)
                {
                  /* If a label is within the function it 'can' be short
                     encoded therefore it takes up 4 bytes of PC address
                     space. If a label is not within the function then
                     branch tables must be long encoded */
                  if (metag_is_label_within_function (XVECEXP (body, 1, i)))
                    count += 4;
                  else
                    {
                      count += 2049;
                      break;
                    }
                }
            }
          else if (GET_CODE (body) != UNSPEC_VOLATILE
                   || XINT (body, 1) != VUNSPEC_BLOCKAGE)
            /* *2 for each instruction to make them 'long' */
            count += (get_attr_length (insn) << 1);

          insn = next_active_insn (insn);

          if (count > 2048)
            break;
        }

      /* 2048 is the number of bytes in PC address space that a short
         branch can jump forward or backwards to. The 'count' variable
         conservatively counts the number of bytes in the function
         assuming all instructions will be double their stated size,
         (double being a long MiniM encoding) */

      cfun->machine->can_use_short_branch = (count <= 2048);
    }
}

static bool
metag_is_label_within_function (rtx label_ref)
{
  rtx insn = get_insns();

  while ((insn = next_label (insn)) != 0)
    if (CODE_LABEL_NUMBER (insn) == CODE_LABEL_NUMBER (XEXP (label_ref, 0)))
      return true;

  return false;
}

static int
metag_asm_insn_count (rtx body)
{
  const char *template;
  int count = 1;

  if (GET_CODE (body) == ASM_INPUT)
    template = XSTR (body, 0);
  else
    template = decode_asm_operands (body, NULL, NULL, NULL, NULL);

  for (; *template; template++)
    if (IS_ASM_LOGICAL_LINE_SEPARATOR (*template) || *template == '\n')
      count++;

  return count;
}

/* Generate a conditional branch instruction, inserting the label number for
   the return stub */
char *
metag_gen_cond_return_branch (const char * pattern)
{
  int length = strlen(pattern) + 21;
  char * buf = xmalloc (length);

  snprintf (buf, length, pattern, cfun->funcdef_no);

  if (cfun->machine->cond_return_state == METAG_COND_RETURN_NONE)
    cfun->machine->cond_return_state = METAG_COND_RETURN_REQD;

  return buf;
}

/* Generate the return stub instruction inserting the label number for the
   return stub */
char *
metag_gen_cond_return_stub (void)
{
  static const char * pattern = "$LX%d:\n\tMOV\tPC, D1RtP";
  int length = strlen(pattern) + 21;
  char * buf = xmalloc (length);

  snprintf (buf, length, pattern, cfun->funcdef_no);

  cfun->machine->cond_return_state = METAG_COND_RETURN_DONE;

  return buf;
}

/* Create a stub at the end of a function if one is required and not already
   emitted. */
void
metag_emit_cond_return_stub_if_reqd (void)
{
  if (cfun->machine->cond_return_state == METAG_COND_RETURN_REQD)
    {
      char * stub = metag_gen_cond_return_stub ();

      fputs (stub, asm_out_file);
      fputc ('\n', asm_out_file);
      cfun->machine->cond_return_state = METAG_COND_RETURN_DONE;

      free (stub);
    }
}

bool
metag_output_addr_const_extra (FILE * stream, rtx x)
{
  if (GET_CODE (x) == CONST_VECTOR && GET_MODE (x) == V2SImode)
    {
      /* WORK NEEDED: Assert more rigourously that the values are identical */
      gcc_assert (INTVAL (CONST_VECTOR_ELT (x, 0)) == INTVAL (CONST_VECTOR_ELT (x, 1)));
      output_addr_const (stream, CONST_VECTOR_ELT (x, 0));
      return true;
    }
  return false;
}

/* Produces a rtx representing the return register (D0Re0) using the correct mode. */
rtx
metag_function_return_reg (enum machine_mode mode)
{
  if (GET_MODE_CLASS (mode) == MODE_INT && GET_MODE_SIZE (mode) < UNITS_PER_WORD)
    return gen_rtx_REG (SImode, D0Re0_REG);
  else
    return gen_rtx_REG (mode, D0Re0_REG);
}

rtx
metag_libcall_value (enum machine_mode mode)
{
  return metag_function_return_reg (mode);
}

rtx
metag_function_value (tree ret_type, tree fn_decl_or_type ATTRIBUTE_UNUSED, bool outgoing ATTRIBUTE_UNUSED)
{
  return metag_function_return_reg (TYPE_MODE (ret_type));
}

bool
metag_have_tls(void)
{
  return targetm.have_tls;
}

#include "gt-metag.h"
