/* Target Code for the RCA CDP1802 processor.
   Copyright (C) 2024 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>

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

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "gimple.h"
#include "df.h"
#include "memmodel.h"
#include "tm_p.h"
#include "emit-rtl.h"
#include "c-family/c-common.h"
#include "diagnostic-core.h"
#include "output.h"
#include "stor-layout.h"
#include "varasm.h"
#include "calls.h"
#include "explow.h"
#include "expr.h"
#include "langhooks.h"
#include "gimplify.h"
#include "builtins.h"
#include "optabs.h"

/* This file should be included last.  */
#include "target-def.h"

/* Stack Layout:

   The stack is laid out as follows:

	last parameter word	(higher addresses)
	...
	5th parameter word	<- callee's AP = caller's SP just before the call
	caller's return address
	Last callee-save reg
	...
	First callee-save reg	<- callee's FP
	...
	Local variables
	...
	Outgoing parameters	<- callee's SP
				(lower addresses)

   All addressing in the frame is via offsets from SP or FP.

   The push instruction STXD is post-decrement, so the SP register always
   points one byte below the last pushed item.  The hard FP register is
   likewise off-by-one.  The virtual FP and AP registers are not off-by-one.

   The lowest byte in the frame is available as a scratch for generated code.

   The unnamed parameters to a variadic function are passed entirely on the
   stack, see TARGET_STRICT_ARGUMENT_NAMING and TARGET_FUNCTION_ARG.  This
   simplifies both the generated code and the backend.  */

/* A structure to describe the layout.

   Caching this in cfun->machine doesn't work, as the set of registers to
   save changes during the course of compiling a function.  In particular,
   the hard frame pointer is sometimes initially in this set and later not.
   That change is not accompanied by a call to TARGET_COMPUTE_FRAME_LAYOUT
   or a change to frame_pointer_needed.

   Therefore the layout is always recomputed when needed.  */
struct cdp1802_stack_layout
{
  /* Size of the low two items in the fixed-size part of the stack.  */
  int locals_size;
  int register_save_size;
  /* Sum of the above items.  */
  int frame_size;
  /* Offsets for register elimination.  */
  int fp_minus_sp;
  int ap_minus_fp;
};

/* Does REGNO need to be saved?  */

static bool
cdp1802_regno_needs_save_p (unsigned int regno)
{
  return df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno);
}

/* Compute the stack layout.  */

static void
cdp1802_compute_stack_layout (struct cdp1802_stack_layout *layout)
{
  layout->locals_size = get_frame_size ();

  layout->register_save_size = 0;
  for (int regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (cdp1802_regno_needs_save_p (regno))
      layout->register_save_size += UNITS_PER_WORD;

  layout->frame_size = layout->locals_size + layout->register_save_size;
  layout->ap_minus_fp = layout->register_save_size + 2;
  layout->fp_minus_sp = layout->locals_size + 1;
}

/* Run-time Target Specification.  */

/* Implement TARGET_CPU_CPP_BUILTINS.  */

void
cdp1802_cpu_cpp_builtins (cpp_reader *pfile)
{
  cpp_define (pfile, "__CDP1802__");
  if (TARGET_1804)
    cpp_define (pfile, "__CDP1804__");
}

/* Storage Layout.  */

/* Worker function for TARGET_OVERRIDE_OPTIONS_AFTER_CHANGE.  */

static void
cdp1802_override_options_after_change (void)
{
  /* When STACK_PUSH_CODE is PRE_DEC we need -fno-combine-stack-adjustments
     to avoid breaking lots of tests.  With POST_DEC we needed -fno-dse.  */
  flag_combine_stack_adjustments = 0;
}

/* How Values Fit in Registers.  */

/* Worker function for TARGET_HARD_REGNO_MODE_OK.  */

static bool
cdp1802_hard_regno_mode_ok (unsigned int regno, machine_mode mode)
{
  return mode == BImode || GET_MODE_SIZE (mode) <= 8;
}

/* Worker function for TARGET_MODES_TIEABLE_P.  */

static bool
cdp1802_modes_tieable_p (machine_mode mode1, machine_mode mode2)
{
  return mode1 != BImode && mode2 != BImode;
}

/* Registers Classes.  */

/* Implement REGNO_REG_CLASS.  */

enum reg_class
cdp1802_regno_reg_class (int regno)
{
  return
    (  regno == CDP1802_ARG0+0		? A_REGS
     : regno == CDP1802_ARG0+1		? B_REGS
     : regno == CDP1802_SC		? Z_REGS
     : regno < FIRST_PSEUDO_REGISTER	? GENERAL_REGS
     : ALL_REGS);
}

/* Worker function for TARGET_SECONDARY_RELOAD.  */

static reg_class_t
cdp1802_secondary_reload (bool /*in_p*/, rtx x, reg_class_t rclass,
			    machine_mode /*reload_mode*/, secondary_reload_info */*sri*/)
{
  if ((MEM_P (x)
       || ((GET_CODE (x) == SUBREG || REG_P (x))
	   && (true_regnum (x) == -1
	       || true_regnum (x) >= FIRST_PSEUDO_REGISTER)))
      && ! reg_class_subset_p (rclass, GENERAL_REGS))
    return GENERAL_REGS;

  return NO_REGS;
}

/* Eliminating Frame Pointer and Arg Pointer.  */

/* Worker function for TARGET_FRAME_POINTER_REQUIRED.

   Without this hook the gcc.dg/torture/stackalign/vararg-1.c test
   case is miscompiled due to an incorrect elimination offset from
   HARD_FRAME_POINTER_REGNUM to ARG_POINTER_REGNUM.  */

static bool
cdp1802_frame_pointer_required (void)
{
  return cfun->calls_alloca;
}

/* Worker function for TARGET_CAN_ELIMINATE.  */

static bool
cdp1802_can_eliminate (const int from, const int to)
{
  return (from == ARG_POINTER_REGNUM && to == STACK_POINTER_REGNUM
	  ? ! frame_pointer_needed
	  : true);
}

/* Implement INITIAL_ELIMINATION_OFFSET.  */

int
cdp1802_initial_elimination_offset (int from, int to)
{
  struct cdp1802_stack_layout layout;
  int result;

  cdp1802_compute_stack_layout (&layout);

  if (from == FRAME_POINTER_REGNUM && to == HARD_FRAME_POINTER_REGNUM)
    result = 1;
  else if (from == FRAME_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    result = layout.fp_minus_sp;
  else if (from == ARG_POINTER_REGNUM && to == HARD_FRAME_POINTER_REGNUM)
    result = layout.ap_minus_fp + 1;
  else if (from == ARG_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    result = layout.fp_minus_sp + layout.ap_minus_fp; /* fp_minus_sp includes the +1 */
  else
    gcc_unreachable ();

  return result;
}

/* Passing Function Arguments on the Stack.  */

/* Worker function for TARGET_PUSH_ARGUMENT.  */

static bool
cdp1802_push_argument (unsigned int /*npush*/)
{
  return true;
}

/* Implement PUSH_ROUNDING.  */

poly_int64
cdp1802_push_rounding (poly_int64 bytes)
{
  return bytes;
}

/* Passing Arguments in Registers.  */

#define CDP1802_WORD_SIZE(TYPE, MODE)				\
  ((((TYPE) ? int_size_in_bytes (TYPE) : GET_MODE_SIZE (MODE))	\
    + 1) 							\
   / 2)

/* Worker function for TARGET_FUNCTION_ARG.  */

static rtx
cdp1802_function_arg (cumulative_args_t ca, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (ca);

  if (arg.end_marker_p ())
    return const0_rtx;

  if (!arg.named
      || targetm.calls.must_pass_in_stack (arg)
      || (*cum + CDP1802_WORD_SIZE (arg.type, arg.mode)
	  > NUM_ARGUMENT_REGISTERS))
    return NULL_RTX;

  return gen_rtx_REG (arg.mode, *cum + FIRST_ARGUMENT_REGISTER);
}

/* Worker function for TARGET_FUNCTION_ARG_ADVANCE.  */

static void
cdp1802_function_arg_advance (cumulative_args_t ca, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (ca);

  /* If an argument would otherwise be passed partially in registers,
     and partially on the stack, the whole of it is passed on the
     stack.  */
  if (*cum < NUM_ARGUMENT_REGISTERS
      && (*cum + CDP1802_WORD_SIZE (arg.type, arg.mode)
	  > NUM_ARGUMENT_REGISTERS))
    *cum = NUM_ARGUMENT_REGISTERS;

  *cum += CDP1802_WORD_SIZE (arg.type, arg.mode);
}

/* How Scalar Function Values Are Returned.  */

/* Worker function for TARGET_FUNCTION_VALUE.  */

static rtx
cdp1802_function_value (const_tree valtype, const_tree /*func*/, bool /*outgoing*/)
{
  machine_mode mode;

  mode = TYPE_MODE (valtype);
  return gen_rtx_REG (mode, RETURN_VALUE_REGNUM);
}

/* Worker function for TARGET_LIBCALL_VALUE.  */

static rtx
cdp1802_libcall_value (machine_mode mode, const_rtx /*fun*/)
{
  return gen_rtx_REG (mode, RETURN_VALUE_REGNUM);
}

/* Worker function for TARGET_FUNCTION_VALUE_REGNO_P.  */

static bool
cdp1802_function_value_regno_p (const unsigned int regno)
{
  return regno == RETURN_VALUE_REGNUM;
}

/* How Large Values Are Returned.  */

/* Worker function for TARGET_RETURN_IN_MEMORY.  */

static bool
cdp1802_return_in_memory (const_tree type, const_tree /*fntype*/)
{
  const HOST_WIDE_INT size = int_size_in_bytes (type);

  return size == -1 || size > UNITS_PER_WORD * NUM_ARGUMENT_REGISTERS;
}

/* Function Entry and Exit.  */

/* Implement EPILOGUE_USES.  */

bool
cdp1802_epilogue_uses_p (int /*regno*/)
{
  return false;
}

/* Worker function for TARGET_ASM_OUTPUT_MI_THUNK.  */

static void
cdp1802_asm_output_mi_thunk (FILE *file,
			     tree thunk_fndecl,
			     HOST_WIDE_INT delta,
			     HOST_WIDE_INT vcall_offset,
			     tree function)
{
  const char *fnname = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (thunk_fndecl));
  int this_regno;

  assemble_start_function (thunk_fndecl, fnname);

  /* Find out which register contains THIS.  */
  this_regno =
    (aggregate_value_p (TREE_TYPE (TREE_TYPE (function)), function)
     ? FIRST_ARGUMENT_REGISTER + 1
     : FIRST_ARGUMENT_REGISTER);

  /* Add DELTA to THIS.  */
  /* TODO: if (abs (delta) < 8) generate inc or dec sequence instead */
  if (delta != 0)
    fprintf (file,
	     "\tglo %u\n"
	     "\tadi 0x%02x\n"
	     "\tplo %u\n"
	     "\tghi %u\n"
	     "\tadci 0x%02x\n"
	     "\tphi %u\n",
	     this_regno, (unsigned int) delta & 0xff, this_regno,
	     this_regno, (unsigned int) delta >> 8, this_regno);

  /* If needed, add *(*THIS + VCALL_OFFSET) to THIS.  */
  if (vcall_offset != 0)
    {
      /* We use the caller-save r15 as TEMP.  */

      /* temp = *this + vcall_offset */
      /* TODO: if (abs (vcall_offset) < 8) generate load-then-inc-or-dec-sequence instead */
      fprintf (file,
	       "\tinc %u\n"	/* point THIS to low byte */
	       "\tldn %u\n"	/* load *THIS low byte */
	       "\tadi 0x%02x\n"	/* add VCALL_OFFSET low byte */
	       "\tplo 15\n"	/* put TEMP low byte */
	       "\tdec %u"	/* point THIS to high byte */
	       "\tldn %u\n"	/* load *THIS high byte */
	       "\tadci 0x%02x\n"/* add VCALL_OFFSET high byte and carry */
	       "\tphi 15\n",	/* put TEMP high byte */
	       this_regno, this_regno, (unsigned int) vcall_offset & 0xff,
	       this_regno, this_regno, ((unsigned int) vcall_offset >> 8) & 0xff);

      /* this = this + *temp */
      fprintf (file,
	       "\tsex 15\n"	/* ADD and ADC use R[X] to point to memory */
	       "\tinc 15\n"	/* point TEMP to low byte */
	       "\tglo %u\n"	/* get THIS low byte */
	       "\tadd\n"	/* add *TEMP low byte */
	       "\tplo %u\n"	/* put THIS low byte */
	       "\tdec 15\n"	/* point TEMP to high byte */
	       "\tghi %u\n"	/* get THIS high byte */
	       "\tadc\n"	/* add *TEMP high byte and carry */
	       "\tphi %u\n"	/* put THIS high byte */
	       "\tsex 2\n",	/* restore invariant that X=2 on entry */
	       this_regno, this_regno,
	       this_regno, this_regno);
    }

  /* Jump to FUNCTION.  */
  fputs ("\tlbr ", file);
  assemble_name (file, XSTR (XEXP (DECL_RTL (function), 0), 0));
  putc ('\n', file);

  assemble_end_function (thunk_fndecl, fnname);
}

/* Generating Code for Profiling.  */

/* Implement FUNCTION_PROFILER.  */

void
cdp1802_function_profiler (FILE *file, int /*labelno*/)
{
  fprintf (file, "\tsep 4\n\t.hword _mcount\n");
}

/* Support for Nested Functions.  */

/* Worker function for TARGET_TRAMPOLINE_INIT.  */

static void
cdp1802_trampoline_init (rtx m_tramp, tree fndecl, rtx static_chain)
{
  rtx addr = XEXP (m_tramp, 0);
  rtx fnaddr = XEXP (DECL_RTL (fndecl), 0);

  emit_library_call (gen_rtx_SYMBOL_REF (Pmode, "__trampoline_setup"),
		     LCT_NORMAL, VOIDmode,
		     addr, Pmode,
		     fnaddr, Pmode,
		     static_chain, Pmode);
}

/* Addressing Modes.  */

/* Worker function for TARGET_LEGITIMATE_ADDRESS_P.

   A non-branch memory address is always a single register on the CDP1802.
   Some instructions use post-increment or post-decrement addressing, so
   we accept those too.  */

static bool
cdp1802_legitimate_address_p (machine_mode /*mode*/, rtx x,
			      bool strict, code_helper = ERROR_MARK)
{
  if (GET_CODE (x) == POST_INC
      || GET_CODE (x) == PRE_DEC
      || GET_CODE (x) == POST_DEC)
    x = XEXP (x, 0);

  if (REG_P (x)
      && REGNO_OK_FOR_BASE_P (REGNO (x))
      && (! strict || REGNO (x) < FIRST_PSEUDO_REGISTER))
    return true;

  return false;
}

/* Describing Relative Costs of Operations.  */

/* Worker function for TARGET_MEMORY_MOVE_COST.  */

static int
cdp1802_memory_move_cost (machine_mode mode, reg_class_t rclass, bool in)
{
  switch (rclass)
    {
    case A_REGS:
    case B_REGS:
    case Z_REGS:
    case GENERAL_REGS:
      if (mode == QImode)
	{
	  if (in)
	    return 2; /* ldn r1; plo r2 */
	  else
	    return 2; /* glo r1; str r2 */
	}
      else
	{
	  if (in)
	    return 5; /* lda r1; phi r2; ldn r1; plo r2; dec r1 */
	  else
	    return 6; /* ghi r1; str r2; glo r1; inc r2; str r2; dec r2 */
	}
    }

  /* We should never load or store D, DF, or X.  */
  return 100;
}

/* Worker function for TARGET_RTX_COSTS.  */

/* TODO: needs CDP1802 overhaul */
static bool
cdp1802_rtx_costs (rtx x, machine_mode mode, int /*outer_code*/,
		   int /*opno*/, int *total, bool speed_p)
{
  rtx_code code = GET_CODE (x);

  switch (code)
    {
    case CONST_INT:
      if (mode == QImode || IN_RANGE (INTVAL (x), 0, 255))
	*total = COSTS_N_INSNS (1);
      else
	*total = COSTS_N_INSNS (2);
      return true;

    case CONST_DOUBLE:
    case CONST:
    case SYMBOL_REF:
    case LABEL_REF:
      *total = COSTS_N_INSNS (2);
      return true;

    case PLUS:
    case MINUS:
	{
	  if (CONST_INT_P (XEXP (x, 1)))
	    {
	      if (IN_RANGE (INTVAL (XEXP (x, 1)), 0, 16))
		*total = COSTS_N_INSNS (1);
	      else
		*total = COSTS_N_INSNS (2);
	    }
	  else
	    {
	      *total = COSTS_N_INSNS (1);
	      *total += rtx_cost (XEXP (x, 1), mode, code, 0, speed_p);
	    }
	  *total += rtx_cost (XEXP (x, 0), mode, code, 0, speed_p);
	  return true;
	}
      return false;

    case ASHIFT:
    case ASHIFTRT:
    case LSHIFTRT:
      if (REG_P (XEXP (x, 0))
	  && CONST_INT_P (XEXP (x, 1)))
	{
	  if (mode == HImode)
	    {
	      /* asr/shl/shr.  */
	      *total = COSTS_N_INSNS (1);
	      return true;
	    }
	  else if (mode == QImode)
	    {
	      /* (shl+shr)+shr.  */
	      *total = COSTS_N_INSNS (3);
	      return true;
	    }
	}
      return false;

    case ZERO_EXTEND:
      if (mode == HImode)
	{
	  if (GET_MODE (XEXP (x, 0)) == QImode)
	    /* shl+shr.  */
	    *total = COSTS_N_INSNS (2);
	}
      return false;

    case SIGN_EXTEND:
      if (mode == HImode)
	{
	  if (GET_MODE (XEXP (x, 0)) == QImode)
	    /* cbw.  */
	    *total = COSTS_N_INSNS (1);
	}
      return false;

    case SET:
      if (REG_P (XEXP (x, 0)))
	{
	  if (!REG_P (XEXP (x, 1)))
	    *total = rtx_cost (XEXP (x, 1), mode, SET, 1, speed_p);
	  return true;
	}
      return false;

    default:
      return false;
    }
}

/* Output of Assembler Instructions.  */

static const char *
cdp1802_comparison_code_string (rtx x)
{
  switch (GET_CODE (x))
    {
    case EQ:	return "eq";
    case NE:	return "ne";
    case LT:	return "lt";
    case LTU:	return "ltu";
    case LE:	return "le";
    case LEU:	return "leu";
    case GE:	return "ge";
    case GEU:	return "geu";
    case GT:	return "gt";
    case GTU:	return "gtu";
    default:	gcc_unreachable ();
    }
}

/* Worker function for TARGET_PRINT_OPERAND.

   Note: tm.texi only mentions the old macro, not this hook.  */

static void
cdp1802_print_operand (FILE *stream, rtx x, int code)
{
  switch (code)
    {
    case 'C':	/* Code of a comparison_operator.  */
      fprintf (stream, "%s", cdp1802_comparison_code_string (x));
      return;

    case 'A':	/* High byte of fourth word of a constant.  */
      if (CONST_INT_P (x))
	{
	  unsigned int byte = ((INTVAL (x)) >> 56) & 0xff;

	  fprintf (stream, "0x%02x", byte);
	  return;
	}
      break;

    case 'B':	/* Low byte of fourth word of a constant.  */
      if (CONST_INT_P (x))
	{
	  unsigned int byte = ((INTVAL (x)) >> 48) & 0xff;

	  fprintf (stream, "0x%02x", byte);
	  return;
	}
      break;

    case 'D':	/* High byte of third word of a constant.  */
      if (CONST_INT_P (x))
	{
	  unsigned int byte = ((INTVAL (x)) >> 40) & 0xff;

	  fprintf (stream, "0x%02x", byte);
	  return;
	}
      break;

    case 'E':	/* Low byte of third word of a constant.  */
      if (CONST_INT_P (x))
	{
	  unsigned int byte = ((INTVAL (x)) >> 32) & 0xff;

	  fprintf (stream, "0x%02x", byte);
	  return;
	}
      break;

    case 'F':	/* High byte of second word of a constant.  */
      if (CONST_INT_P (x))
	{
	  unsigned int byte = ((INTVAL (x)) >> 24) & 0xff;

	  fprintf (stream, "0x%02x", byte);
	  return;
	}
      break;

    case 'G':	/* Low byte of second word of a constant.  */
      if (CONST_INT_P (x))
	{
	  unsigned int byte = ((INTVAL (x)) >> 16) & 0xff;

	  fprintf (stream, "0x%02x", byte);
	  return;
	}
      break;

    case 'H':	/* High byte of constant or symbol.  */
      if (CONST_INT_P (x))
	{
	  unsigned int byte = ((INTVAL (x)) >> 8) & 0xff;

	  fprintf (stream, "0x%02x", byte);
	  return;
	}
      else if (CONSTANT_ADDRESS_P (x))
	{
	  fprintf (stream, "%%hi(");
	  output_addr_const (stream, x);
	  fprintf (stream, ")");
	  return;
	}
      break;

    case 'L':	/* Low byte of constant or symbol.  */
      if (CONST_INT_P (x))
	{
	  unsigned int byte = (INTVAL (x)) & 0xff;

	  fprintf (stream, "0x%02x", byte);
	  return;
	}
      else if (CONSTANT_ADDRESS_P (x))
	{
	  fprintf (stream, "%%lo(");
	  output_addr_const (stream, x);
	  fprintf (stream, ")");
	  return;
	}
      break;

    case 'M':	/* A memory operand: (reg), (--reg), (reg--), or (reg++).  */
      if (MEM_P (x))
	{
	  rtx address = XEXP (x, 0);

	  switch (GET_CODE (address))
	    {
	    case POST_DEC:
	    case POST_INC:
	    case PRE_DEC:
	      address = XEXP (address, 0);
	      break;
	    default:
	      break;
	    }

	  gcc_assert (REG_P (address));
	  fputs (reg_names[REGNO (address)], stream);
	  return;
	}
      break;

    case 'N':
      /* The next register in a SI or SF mode register pair, or the second
	 register in a DI or DF mode register quad.  */
      if (REG_P (x) &&
	  (GET_MODE (x) == SImode
	   || GET_MODE (x) == SFmode
	   || GET_MODE (x) == DImode
	   || GET_MODE (x) == DFmode))
	{
	  fputs (reg_names [REGNO (x) + 1], stream);
	  return;
	}
      break;

    case 'P':
      /* The third register in a DI or DF mode register quad.  */
      if (REG_P (x) && (GET_MODE (x) == DImode || GET_MODE (x) == DFmode))
	{
	  fputs (reg_names [REGNO (x) + 2], stream);
	  return;
	}
      break;

    case 'Q':
      /* The fourth register in a DI-mode register quad.  */
      if (REG_P (x) && (GET_MODE (x) == DImode || GET_MODE (x) == DFmode))
	{
	  fputs (reg_names [REGNO (x) + 3], stream);
	  return;
	}
      break;

    case 0:
      if (MEM_P (x))
	{
	  rtx tmp = XEXP (x, 0);

	  if (REG_P (tmp))
	    x = tmp;
	  /* Auto inc/dec addressing modes are only allowed with %M.  */
	}

      if (REG_P (x))
	{
	  fputs (reg_names[REGNO (x)], stream);
	  return;
	}
      else if (CONSTANT_ADDRESS_P (x) || LABEL_P (x))
	{
	  output_addr_const (stream, x);
	  return;
	}
    }

  fprintf (stderr, "@ cdp1802_print_operand: code %d\n", code);
  print_rtl (stderr, x);
  fprintf (stderr, "\n");

  gcc_unreachable ();
}

/* Worker function for TARGET_PRINT_OPERAND_ADDRESS.

   Note: tm.texi only mentions the old macro, not this hook.  */

static void
cdp1802_print_operand_address (FILE *file, machine_mode /*mode*/, rtx address)
{
  output_addr_const (file, address);
}

/* Implement ASM_OUTPUT_REG_PUSH.  */

void
cdp1802_output_reg_push (FILE *stream, int regno)
{
  const char *name = reg_names[regno];
  fprintf (stream, "\tglo %s\n\tstxd\n\tghi %s\n\tstxd\n", name, name);
}

/* Implement ASM_OUTPUT_REG_POP.  */

void
cdp1802_output_reg_pop (FILE *stream, int regno)
{
  const char *name = reg_names[regno];
  fprintf (stream, "\tinc 2\n\tlda 2\n\tphi %s\n\tldn 2\n\tplo %s\n", name, name);
}

/* Output of Dispatch Tables.  */

/* Implement ASM_OUTPUT_ADDR_VEC_ELT.  */

void
cdp1802_output_addr_vec_elt (FILE *stream, int value)
{
  fprintf (stream, "\t.hword .L%d\n", value);
}

/* Assembler Commands for Alignment.  */

/* Implement ASM_OUTPUT_ALIGN.  */

void
cdp1802_output_align (FILE *stream, int power)
{
  fprintf (stream, "\t.p2align %d\n", power);
}

/* Implement ASM_OUTPUT_ALIGN_WITH_NOP.  */

void
cdp1802_output_align_with_nop (FILE *stream, int power)
{
  fprintf (stream, "\t.p2align %d,0xC4\n", power);
}

/* Macros for DWARF Output.  */

/* Worker function for TARGET_DEBUG_UNWIND_INFO.  */

static enum unwind_info_type
cdp1802_debug_unwind_info (void)
{
  return UI_NONE; /* TODO: return UI_DWARF2 once that works */
}

/* Worker function for TARGET_OPTION_OVERRIDE.  */

static void
cdp1802_option_override (void)
{
  if (cdp1802_arch_option)
    {
      if (strcmp (cdp1802_arch_option, "1804") == 0)
	target_flags |= MASK_1804;
      else if (strcmp(cdp1802_arch_option, "1802") == 0)
	target_flags &= ~MASK_1804;
      else
	error ("unknown CDP1802 family member in %<-march=%>: %s", cdp1802_arch_option);
    }

  cdp1802_override_options_after_change ();
}

/* Worker function for the addhi3, addptrhi3, and subhi3 insns.

   AMOUNT should be a CONST_INT in range -7 to 7, inclusive.

   Expand an ADD or SUB with 0-7 to a sequence of INC or DEC instructions.

   An ADD (SUB) with a negative amount is treated as a SUB (ADD) with the negated amount.  */

const char *
cdp1802_template_incdec (bool add_p, rtx amount)
{
  static const char incs[7 * 8 + 1 + 1] =
    "inc %0\n\tinc %0\n\tinc %0\n\tinc %0\n\tinc %0\n\tinc %0\n\tinc %0\n\t;";
  static const char decs[7 * 8 + 1 + 1] =
    "dec %0\n\tdec %0\n\tdec %0\n\tdec %0\n\tdec %0\n\tdec %0\n\tdec %0\n\t;";
  int aval;

  gcc_assert (CONST_INT_P (amount));
  aval = INTVAL (amount);

  if (aval < 0)
    {
      aval = -aval;
      add_p = !add_p;
    }

  gcc_assert (IN_RANGE (aval, 0, 7));

  return (add_p ? incs : decs) + (7 * 8) - (aval * 8);
}

/* Worker function for movdf_internal insn.

   Adjust the copying order to avoid clobbering the value being copied.  */

const char *
cdp1802_template_movdf_r_r (rtx dst, rtx src)
{
  gcc_assert (REG_P (dst) && REG_P (src));

  return
    (REGNO (dst) < REGNO (src))
    ? ("ghi %1\n\tphi %0\n\t"
       "glo %1\n\tplo %0\n\t"
       "ghi %N1\n\tphi %N0\n\t"
       "glo %N1\n\tplo %N0\n\t"
       "ghi %P1\n\tphi %P0\n\t"
       "glo %P1\n\tplo %P0\n\t"
       "ghi %Q1\n\tphi %Q0\n\t"
       "glo %Q1\n\tplo %Q0")
    : ("ghi %Q1\n\tphi %Q0\n\t"
       "glo %Q1\n\tplo %Q0\n\t"
       "ghi %P1\n\tphi %P0\n\t"
       "glo %P1\n\tplo %P0\n\t"
       "ghi %N1\n\tphi %N0\n\t"
       "glo %N1\n\tplo %N0\n\t"
       "ghi %1\n\tphi %0\n\t"
       "glo %1\n\tplo %0");
}

/* Worker function for movsf_internal insn.

   Adjust the copying order to avoid clobbering the value being copied.  */

const char *
cdp1802_template_movsf_r_r (rtx dst, rtx src)
{
  gcc_assert (REG_P (dst) && REG_P (src));

  return
    (REGNO (dst) < REGNO (src))
    ? ("ghi %1\n\tphi %0\n\t"
       "glo %1\n\tplo %0\n\t"
       "ghi %N1\n\tphi %N0\n\t"
       "glo %N1\n\tplo %N0")
    : ("ghi %N1\n\tphi %N0\n\t"
       "glo %N1\n\tplo %N0\n\t"
       "ghi %1\n\tphi %0\n\t"
       "glo %1\n\tplo %0");
}

/* Helper for the prologue/epilogue expanders.  */

static rtx
cdp1802_emit_addhi3_postreload (rtx dest, rtx src0, rtx src1)
{
  return emit_insn (gen_rtx_SET (dest, gen_rtx_PLUS (HImode, src0, src1)));
}

/* Helper for the prologue expander.  */

static void
cdp1802_emit_pushhi (rtx mem_push_rtx, int regno)
{
  rtx reg = gen_rtx_REG (HImode, regno);

  rtx insn = emit_move_insn (mem_push_rtx, reg);
  RTX_FRAME_RELATED_P (insn) = 1;

  rtx dwarf = gen_rtx_SEQUENCE (VOIDmode, rtvec_alloc (2));
  XVECEXP (dwarf, 0, 0) = gen_rtx_SET (gen_rtx_MEM (Pmode, stack_pointer_rtx),
				       reg);
  XVECEXP (dwarf, 0, 1) = gen_rtx_SET (stack_pointer_rtx,
				       plus_constant (Pmode,
						      stack_pointer_rtx,
						      -GET_MODE_SIZE (Pmode)));
  add_reg_note (insn, REG_FRAME_RELATED_EXPR, dwarf);
  RTX_FRAME_RELATED_P (XVECEXP (dwarf, 0, 0)) = 1;
  RTX_FRAME_RELATED_P (XVECEXP (dwarf, 0, 1)) = 1;
}

/* Worker function for prologue expander.  */

void
cdp1802_expand_prologue (void)
{
  struct cdp1802_stack_layout layout;
  rtx mem_push_rtx;

  mem_push_rtx = gen_rtx_PRE_DEC (Pmode, stack_pointer_rtx);
  mem_push_rtx = gen_rtx_MEM (HImode, mem_push_rtx);

  cdp1802_compute_stack_layout (&layout);

  if (layout.locals_size >= 32768)
    error ("local variable memory requirements exceed capacity");

  if (flag_stack_usage_info)
    current_function_static_stack_size = layout.frame_size;

  /* Push each of the registers to save.  */
  for (int regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (cdp1802_regno_needs_save_p (regno))
      cdp1802_emit_pushhi (mem_push_rtx, regno);

  /* Set up the frame pointer, if required.  */
  if (frame_pointer_needed)
    {
      rtx insn = emit_move_insn (hard_frame_pointer_rtx, stack_pointer_rtx);
      RTX_FRAME_RELATED_P (insn) = 1;
    }

  /* Allocate space for local variables.  */
  if (layout.locals_size)
    {
      rtx insn = cdp1802_emit_addhi3_postreload (stack_pointer_rtx, stack_pointer_rtx,
						 GEN_INT (-layout.locals_size));
      RTX_FRAME_RELATED_P (insn) = 1;
    }
}

/* Worker function for epilogue expander.  */

void
cdp1802_expand_epilogue (bool sibcall_p)
{
  struct cdp1802_stack_layout layout;
  rtx mem_pop_rtx;
  bool one_below_p;

  mem_pop_rtx = gen_rtx_POST_INC (Pmode, stack_pointer_rtx);
  mem_pop_rtx = gen_rtx_MEM (HImode, mem_pop_rtx);

  cdp1802_compute_stack_layout (&layout);

  /* Track if SP is one below lowest stack item or not.  */
  one_below_p = true;

  /* Pop the stack for the locals.  */
  if (layout.locals_size)
    {
      if (frame_pointer_needed)
	emit_move_insn (stack_pointer_rtx, hard_frame_pointer_rtx);
      else if (layout.register_save_size)
	{
	  /* Fold the INC in the register pop block into this ADD.  */
	  cdp1802_emit_addhi3_postreload (stack_pointer_rtx, stack_pointer_rtx,
					  GEN_INT (layout.locals_size + 1));
	  one_below_p = false;
	}
      else
	cdp1802_emit_addhi3_postreload (stack_pointer_rtx, stack_pointer_rtx,
					GEN_INT (layout.locals_size));
    }

  /* Restore any call-saved registers.  */
  if (layout.register_save_size)
    {
      if (one_below_p)
	{
	  cdp1802_emit_addhi3_postreload (stack_pointer_rtx, stack_pointer_rtx,
					  GEN_INT (1));
	  one_below_p = false;
	}

      /* POP is POST-increment, so SP must not be one-below here.  */
      gcc_assert (!one_below_p);

      for (int regno = FIRST_PSEUDO_REGISTER - 1; regno >= 0; regno--)
	if (cdp1802_regno_needs_save_p (regno))
	  emit_move_insn (gen_rtx_REG (HImode, regno), mem_pop_rtx);

      /* TODO: the last pop above should be a half-pop that leaves SP
	 one below, that would allow removing the decrement below */

      cdp1802_emit_addhi3_postreload (stack_pointer_rtx, stack_pointer_rtx,
				      GEN_INT (-1));

      one_below_p = true;
    }

  /* Return.  */
  gcc_assert (one_below_p);
  if (!sibcall_p)
    emit_jump_insn (gen_return_internal ());
}

/* Predicate for return expander.  */

bool
cdp1802_direct_return_p (void)
{
  struct cdp1802_stack_layout layout;

  if (! reload_completed)
    return false;
  cdp1802_compute_stack_layout (&layout);
  return layout.frame_size == 0;
}

/* Worker function for save_stack_nonlocal expander.  */

void
cdp1802_expand_save_stack_nonlocal (rtx op0, rtx op1)
{
  rtx save_sp = adjust_address (op0, Pmode, 0);

  emit_move_insn (save_sp, op1);
}

/* Worker function for restore_stack_nonlocal expander.  */

void
cdp1802_expand_restore_stack_nonlocal (rtx op0, rtx op1)
{
  rtx save_sp = adjust_address (op1, Pmode, 0);

  emit_move_insn (op0, save_sp);
}

/* Worker function for nonlocal_goto expander.  */

void
cdp1802_expand_nonlocal_goto (rtx /*op0*/, rtx op1, rtx op2, rtx op3)
{
  rtx r_label = op1;
  rtx r_sp = adjust_address (op2, Pmode, 0);
  rtx r_fp = op3;

  emit_clobber (gen_rtx_MEM (BLKmode, gen_rtx_SCRATCH (VOIDmode)));
  emit_clobber (gen_rtx_MEM (BLKmode, hard_frame_pointer_rtx));

  r_label = copy_to_reg (r_label);

  r_fp = copy_to_reg (r_fp);
  emit_stack_restore (SAVE_NONLOCAL, r_sp);

  emit_insn (gen_blockage ());
  emit_clobber (hard_frame_pointer_rtx);
  emit_clobber (frame_pointer_rtx);
  emit_move_insn (hard_frame_pointer_rtx, r_fp);

  emit_use (hard_frame_pointer_rtx);
  emit_use (stack_pointer_rtx);

  emit_indirect_jump (r_label);
}

/* Worker function for cbranchhi4 expander.  */

void
cdp1802_expand_cbranchhi4 (enum rtx_code code, rtx op0, rtx op1, rtx loc)
{
  rtx condition_rtx, loc_ref, branch;
  machine_mode mode;

  mode = GET_MODE (op0);
  gcc_assert (mode == HImode);

  condition_rtx = gen_rtx_fmt_ee (code, mode, op0, op1);
  loc_ref = gen_rtx_LABEL_REF (VOIDmode, loc);
  branch = gen_rtx_SET (pc_rtx,
			gen_rtx_IF_THEN_ELSE (VOIDmode, condition_rtx,
					      loc_ref, pc_rtx));

  emit_jump_insn (branch);
}

/* Worker function for call expanders.

   To make an indirect call on the CDP1802 we call a runtime library routine
   with the address as a hidden parameter in the static chain register.  */

void
cdp1802_expand_call (rtx retval, rtx dest, rtx counter)
{
  rtx call, temp;
  machine_mode mode;

  gcc_assert (MEM_P (dest));
  dest = XEXP (dest, 0);

  if (! CONSTANT_P (dest) && ! REG_P (dest))
    dest = force_reg (Pmode, dest);

  mode = (retval == NULL_RTX) ? VOIDmode : GET_MODE (retval);

  call = gen_rtx_CALL (mode, gen_rtx_MEM (FUNCTION_MODE, dest), counter);
  if (retval)
    call = gen_rtx_SET (retval, call);

  if (! CONSTANT_P (dest))
    {
      /* This becomes the static chain register via the z constraint in the insn template.  */
      temp = gen_reg_rtx (HImode);
      emit_move_insn (temp, dest);
    }
  else
    temp = const0_rtx;

  call = gen_rtx_PARALLEL (VOIDmode, gen_rtvec (2, call,
						gen_rtx_USE (VOIDmode, temp)));
  emit_call_insn (call);
}

/* Worker function for ashlhi3 expander.

   When function parameter setup (calls.cc:load_register_parameters) processes
   a BLKmode argument that fits in registers but isn't an even multiple of the
   word size (e.g. 3 bytes on the CDP1802), it will generate a left-shift to
   move the last fragment into the last argument register.  If the shift becomes
   a libcall then it clobbers other argument registers already set up.

   We must therefore recognize applicable constant left-shifts and emit direct
   code for them.  Alternatively have arithmetic libcalls use different argument
   registers not otherwise used by gcc, but the CDP1802 is too register-starved
   for that.  Or do like AVR and set the word size to 8.

   Returns TRUE if successful, FALSE if the expander should FAIL and cause a
   libcall to be generated.  */

bool
cdp1802_expand_ashlhi3 (rtx dst, rtx arg, rtx amount)
{
  if (CONST_INT_P (amount)
      && INTVAL (amount) == 8)
    {
      emit_insn (gen_ashlhi3_by_8 (dst, arg));
      return true;
    }

  return false;
}

#undef  TARGET_OVERRIDE_OPTIONS_AFTER_CHANGE
#define TARGET_OVERRIDE_OPTIONS_AFTER_CHANGE cdp1802_override_options_after_change

#undef TARGET_HARD_REGNO_MODE_OK
#define TARGET_HARD_REGNO_MODE_OK cdp1802_hard_regno_mode_ok
#undef TARGET_MODES_TIEABLE_P
#define TARGET_MODES_TIEABLE_P cdp1802_modes_tieable_p

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD cdp1802_secondary_reload

#undef  TARGET_FRAME_POINTER_REQUIRED
#define TARGET_FRAME_POINTER_REQUIRED cdp1802_frame_pointer_required
#undef TARGET_CAN_ELIMINATE
#define TARGET_CAN_ELIMINATE cdp1802_can_eliminate

#undef TARGET_PUSH_ARGUMENT
#define TARGET_PUSH_ARGUMENT cdp1802_push_argument

#undef  TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG cdp1802_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE cdp1802_function_arg_advance

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE cdp1802_function_value
#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE cdp1802_libcall_value
#undef TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P cdp1802_function_value_regno_p

#undef  TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY cdp1802_return_in_memory

#undef  TARGET_ASM_OUTPUT_MI_THUNK
#define TARGET_ASM_OUTPUT_MI_THUNK cdp1802_asm_output_mi_thunk
#undef  TARGET_ASM_CAN_OUTPUT_MI_THUNK
#define TARGET_ASM_CAN_OUTPUT_MI_THUNK hook_bool_const_tree_hwi_hwi_const_tree_true

#undef TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL hook_bool_tree_tree_true

#undef TARGET_STRICT_ARGUMENT_NAMING
#define TARGET_STRICT_ARGUMENT_NAMING hook_bool_CUMULATIVE_ARGS_true

#undef TARGET_TRAMPOLINE_INIT
#define TARGET_TRAMPOLINE_INIT cdp1802_trampoline_init

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P cdp1802_legitimate_address_p

#undef  TARGET_MEMORY_MOVE_COST
#define TARGET_MEMORY_MOVE_COST cdp1802_memory_move_cost
#undef  TARGET_RTX_COSTS
#define TARGET_RTX_COSTS cdp1802_rtx_costs

#undef  TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND cdp1802_print_operand
#undef  TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS cdp1802_print_operand_address

#undef TARGET_DEBUG_UNWIND_INFO
#define TARGET_DEBUG_UNWIND_INFO cdp1802_debug_unwind_info

#undef  TARGET_ASM_ALIGNED_HI_OP
#define TARGET_ASM_ALIGNED_HI_OP "\t.hword\t"
#undef  TARGET_ASM_ALIGNED_SI_OP
#define TARGET_ASM_ALIGNED_SI_OP "\t.word\t"

#undef  TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE cdp1802_option_override

#undef  TARGET_HAVE_SPECULATION_SAFE_VALUE
#define TARGET_HAVE_SPECULATION_SAFE_VALUE speculation_safe_value_not_needed

struct gcc_target targetm = TARGET_INITIALIZER;

/* #include "gt-cdp1802.h" */	/* not needed as we have no struct machine_function */
