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

/* This file should be included last.  */
#include "target-def.h"

/* Stack Layout:

   The stack is laid out as follows:

	last parameter word	(higher addresses)
	...
	5th parameter word	<- caller's SP just before the call
	caller's LINK reg	<- callee's AP = SP on entry to callee
	argument register save area for stdarg (NUM_ARGUMENT_REGISTERS words)
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

   The lowest byte in the frame is available as a scratch for generated code.  */

/* A structure to describe the layout.  */
struct cdp1802_stack_layout
{
  /* Size of the topmost three items on the stack.  */
  int locals_size;
  int register_save_size;
  int stdarg_save_size;
  /* Sum of the above items.  */
  int frame_size;
  /* Various offsets.  */
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

  if (cfun->stdarg)
    layout->stdarg_save_size = NUM_ARGUMENT_REGISTERS * UNITS_PER_WORD;
  else
    layout->stdarg_save_size = 0;

  layout->frame_size = (layout->locals_size
			+ layout->register_save_size
			+ layout->stdarg_save_size);

  layout->ap_minus_fp = (layout->register_save_size
			 + layout->stdarg_save_size);
  layout->fp_minus_sp = layout->locals_size + 1;
}

/* Run-time Target Specification.  */

/* Implement TARGET_CPU_CPP_BUILTINS.  */

void
cdp1802_cpu_cpp_builtins (cpp_reader *pfile)
{
  cpp_define (pfile, "__CDP1802__");
}

/* How Values Fit in Registers.  */

/* Worker function for TARGET_HARD_REGNO_MODE_OK.  */

static bool
cdp1802_hard_regno_mode_ok (unsigned int regno, machine_mode mode)
{
  return regno != CDP1802_CARRY || mode == BImode;
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
    result = -layout.fp_minus_sp;
  else if (from == ARG_POINTER_REGNUM && to == HARD_FRAME_POINTER_REGNUM)
    result = -(layout.ap_minus_fp + 1);
  else if (from == ARG_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    result = -(layout.fp_minus_sp + layout.ap_minus_fp);
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
  if (targetm.calls.must_pass_in_stack (arg)
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

/* Worker function for TARGET_BUILD_BUILTIN_VA_LIST.

   For this chip, va_list is a record containing a counter and a pointer.
   The counter is of type 'int' and indicates how many bytes
   have been used to date.  The pointer indicates the stack position
   for arguments that have not been passed in registers.
   To keep the layout nice, the pointer is first in the structure.  */

static tree
cdp1802_build_builtin_va_list (void)
{
  tree f_1, f_2, record, type_decl;

  record = (*lang_hooks.types.make_type) (RECORD_TYPE);
  type_decl = build_decl (BUILTINS_LOCATION,
			  TYPE_DECL, get_identifier ("__va_list_tag"), record);

  f_1 = build_decl (BUILTINS_LOCATION,
		    FIELD_DECL, get_identifier ("base"),
		      ptr_type_node);
  f_2 = build_decl (BUILTINS_LOCATION,
		    FIELD_DECL, get_identifier ("count"),
		      unsigned_type_node);

  DECL_FIELD_CONTEXT (f_1) = record;
  DECL_FIELD_CONTEXT (f_2) = record;

  TYPE_STUB_DECL (record) = type_decl;
  TYPE_NAME (record) = type_decl;
  TYPE_FIELDS (record) = f_1;
  DECL_CHAIN (f_1) = f_2;

  layout_type (record);

  return record;
}

/* Worker function for TARGET_EXPAND_BUILTIN_VA_START.

   Note: tm.texi doesn't document this hook.  */

static void
cdp1802_expand_builtin_va_start (tree valist, rtx /*nextarg*/)
{
  tree f_base, f_count;
  tree base, count;
  tree t,u;

  f_base = TYPE_FIELDS (va_list_type_node);
  f_count = DECL_CHAIN (f_base);

  base = build3 (COMPONENT_REF, TREE_TYPE (f_base), valist, f_base, NULL_TREE);
  count = build3 (COMPONENT_REF, TREE_TYPE (f_count), valist, f_count,
		  NULL_TREE);

  t = make_tree (TREE_TYPE (base), virtual_incoming_args_rtx);
  u = build_int_cst (NULL_TREE, - INCOMING_FRAME_SP_OFFSET);
  u = fold_convert (TREE_TYPE (count), u);
  t = fold_build_pointer_plus (t, u);
  t = build2 (MODIFY_EXPR, TREE_TYPE (base), base, t);
  TREE_SIDE_EFFECTS (t) = 1;
  expand_expr (t, const0_rtx, VOIDmode, EXPAND_NORMAL);

  t = build2 (MODIFY_EXPR, TREE_TYPE (count), count,
	      build_int_cst (NULL_TREE,
			     crtl->args.info * UNITS_PER_WORD));
  TREE_SIDE_EFFECTS (t) = 1;
  expand_expr (t, const0_rtx, VOIDmode, EXPAND_NORMAL);
}

/* Worker function for TARGET_GIMPLIFY_VA_ARG_EXPR.  */

static tree
cdp1802_gimplify_va_arg_expr (tree valist, tree type, gimple_seq *pre_p,
			      gimple_seq */*post_p*/)
{
  tree f_base, f_count;
  tree base, count;
  tree count_tmp, addr, t;
  tree lab_gotaddr, lab_fromstack;
  int size, size_of_reg_args, must_stack;
  tree size_tree;

  f_base = TYPE_FIELDS (va_list_type_node);
  f_count = DECL_CHAIN (f_base);

  base = build3 (COMPONENT_REF, TREE_TYPE (f_base), valist, f_base, NULL_TREE);
  count = build3 (COMPONENT_REF, TREE_TYPE (f_count), valist, f_count,
		  NULL_TREE);

  must_stack = must_pass_va_arg_in_stack (type);
  size_tree = round_up (size_in_bytes (type), UNITS_PER_WORD);
  gimplify_expr (&size_tree, pre_p, NULL, is_gimple_val, fb_rvalue);

  size_of_reg_args = NUM_ARGUMENT_REGISTERS * UNITS_PER_WORD;

  count_tmp = get_initialized_tmp_var (count, pre_p, NULL);
  lab_gotaddr = create_artificial_label (UNKNOWN_LOCATION);
  lab_fromstack = create_artificial_label (UNKNOWN_LOCATION);
  addr = create_tmp_var (ptr_type_node);

  if (!must_stack)
    {
      tree r;

      t = fold_convert (TREE_TYPE (count), size_tree);
      t = build2 (PLUS_EXPR, TREE_TYPE (count), count_tmp, t);
      r = fold_convert (TREE_TYPE (count), size_int (size_of_reg_args));
      t = build2 (GT_EXPR, boolean_type_node, t, r);
      t = build3 (COND_EXPR, void_type_node, t,
		  build1 (GOTO_EXPR, void_type_node, lab_fromstack),
		  NULL_TREE);
      gimplify_and_add (t, pre_p);

      t = fold_build_pointer_plus (base, count_tmp);
      gimplify_assign (addr, t, pre_p);

      t = build1 (GOTO_EXPR, void_type_node, lab_gotaddr);
      gimplify_and_add (t, pre_p);

      t = build1 (LABEL_EXPR, void_type_node, lab_fromstack);
      gimplify_and_add (t, pre_p);
    }

  /* Arguments larger than a word might need to skip over some
     registers, since arguments are either passed entirely in
     registers or entirely on the stack.  */
  size = PUSH_ROUNDING (int_size_in_bytes (type));
  if (size > 2 || size < 0 || must_stack)
    {
      tree r, u;

      r = size_int (NUM_ARGUMENT_REGISTERS * UNITS_PER_WORD);
      u = build2 (MODIFY_EXPR, TREE_TYPE (count_tmp), count_tmp, r);

      t = fold_convert (TREE_TYPE (count), r);
      t = build2 (GE_EXPR, boolean_type_node, count_tmp, t);
      t = build3 (COND_EXPR, void_type_node, t, NULL_TREE, u);
      gimplify_and_add (t, pre_p);
    }

  t = size_int (NUM_ARGUMENT_REGISTERS * UNITS_PER_WORD
		+ INCOMING_FRAME_SP_OFFSET);
  t = fold_convert (TREE_TYPE (count), t);
  t = build2 (MINUS_EXPR, TREE_TYPE (count), count_tmp, t);
  t = build2 (PLUS_EXPR, TREE_TYPE (count), t,
	      fold_convert (TREE_TYPE (count), size_tree));
  t = fold_convert (TREE_TYPE (t), fold (t));
  t = fold_build1 (NEGATE_EXPR, TREE_TYPE (t), t);
  t = fold_build_pointer_plus (base, t);
  gimplify_assign (addr, t, pre_p);

  t = build1 (LABEL_EXPR, void_type_node, lab_gotaddr);
  gimplify_and_add (t, pre_p);

  t = fold_convert (TREE_TYPE (count), size_tree);
  t = build2 (PLUS_EXPR, TREE_TYPE (count), count_tmp, t);
  gimplify_assign (count, t, pre_p);

  addr = fold_convert (build_pointer_type (type), addr);
  return build_va_arg_indirect_ref (addr);
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
	       "\tphi %u\n",	/* put THIS high byte */
	       this_regno, this_regno,
	       this_regno, this_regno);

      /* TODO: if we want X == 2 (SP) on entry to be in the ABI,
	 we'll have to reset X to 2 here.  */
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
      || GET_CODE (x) == POST_DEC)
    x = XEXP (x, 0);

  if (REG_P (x)
      && REGNO_OK_FOR_BASE_P (REGNO (x))
      && (! strict || REGNO (x) < FIRST_PSEUDO_REGISTER))
    return true;

  return false;
}

/* Representation of condition codes using registers.  */

/* Worker function for TARGET_FIXED_CONDITION_CODE_REGS.  */

static bool
cdp1802_fixed_condition_code_regs (unsigned int *p1, unsigned int *p2)
{
  *p1 = CDP1802_CARRY;
  *p2 = INVALID_REGNUM;
  return true;
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
	    return 5; /* inc r2; glo r1; stxd r2; ghi r1; str r2 */
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
      if (mode == QImode || IN_RANGE(INTVAL (x), 0, 255))
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

/* Macros Controlling Initialization Routines.  */

/* Worker function for TARGET_ASM_CONSTRUCTOR.

   Like default_named_section_asm_out_constructor, but don't make
   the section writable.  */

static void
cdp1802_asm_out_constructor (rtx symbol, int priority)
{
  const char *section = ".ctors";
  char buf[18];

  /* ??? This only works reliably with the GNU linker.  */
  if (priority != DEFAULT_INIT_PRIORITY)
    {
      sprintf (buf, ".ctors.%.5u",
	       /* Invert the numbering so the linker puts us in the proper
		  order; constructors are run from right to left, and the
		  linker sorts in increasing order.  */
	       MAX_INIT_PRIORITY - priority);
      section = buf;
    }

  switch_to_section (get_section (section, 0, NULL));
  assemble_align (POINTER_SIZE);
  assemble_integer (symbol, POINTER_SIZE / BITS_PER_UNIT, POINTER_SIZE, 1);
}

/* Worker function for TARGET_ASM_DESTRUCTOR.

   Like default_named_section_asm_out_destructor, but don't make
   the section writable.  */

static void
cdp1802_asm_out_destructor (rtx symbol, int priority)
{
  const char *section = ".dtors";
  char buf[18];

  /* ??? This only works reliably with the GNU linker.  */
  if (priority != DEFAULT_INIT_PRIORITY)
    {
      sprintf (buf, ".dtors.%.5u",
	       /* Invert the numbering so the linker puts us in the proper
		  order; constructors are run from right to left, and the
		  linker sorts in increasing order.  */
	       MAX_INIT_PRIORITY - priority);
      section = buf;
    }

  switch_to_section (get_section (section, 0, NULL));
  assemble_align (POINTER_SIZE);
  assemble_integer (symbol, POINTER_SIZE / BITS_PER_UNIT, POINTER_SIZE, 1);
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

    case 'M':	/* A memory operand: (reg), (reg--), or (reg++).  */
      if (MEM_P (x))
	{
	  rtx address = XEXP (x, 0);

	  switch (GET_CODE (address))
	    {
	    case POST_DEC:
	    case POST_INC:
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

    case 0:
      if (MEM_P (x))
	{
	  rtx tmp = XEXP (x, 0);

	  if (REG_P (tmp))
	    x = tmp;
	  else if (GET_CODE (tmp) == POST_DEC || GET_CODE (tmp) == POST_INC)
	    {
	      tmp = XEXP (tmp, 0);
	      if (REG_P (tmp))
		x = tmp;
	    }
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
  fprintf(stream, "\tsex 2\n\tglo %s\n\tstxd\n\tghi %s\n\tstxd\n", name, name);
}

/* Implement ASM_OUTPUT_REG_POP.  */

void
cdp1802_output_reg_pop (FILE *stream, int regno)
{
  const char *name = reg_names[regno];
  fprintf(stream, "\tinc 2\n\tlda 2\n\tphi %s\n\tldn 2\n\tplo %s\n", name, name);
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

/* Helper for the prologue/epilogue expanders.  */

static rtx
cdp1802_emit_addhi3_postreload (rtx dest, rtx src0, rtx src1)
{
  rtx set, clobber, insn;

  set = gen_rtx_SET (dest, gen_rtx_PLUS (HImode, src0, src1));
  clobber = gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (BImode, CDP1802_CARRY));
  insn = emit_insn (gen_rtx_PARALLEL (VOIDmode, gen_rtvec (2, set, clobber)));
  return insn;
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

  mem_push_rtx = gen_rtx_POST_DEC (Pmode, stack_pointer_rtx);
  mem_push_rtx = gen_rtx_MEM (HImode, mem_push_rtx);

  cdp1802_compute_stack_layout (&layout);

  if (layout.locals_size >= 32768)
    error ("local variable memory requirements exceed capacity");

  if (flag_stack_usage_info)
    current_function_static_stack_size = layout.frame_size;

  /* Save the argument registers if necessary.  */
  if (layout.stdarg_save_size)
    for (int regno = FIRST_ARGUMENT_REGISTER + NUM_ARGUMENT_REGISTERS - 1;
	 regno >= FIRST_ARGUMENT_REGISTER;
	 --regno)
      cdp1802_emit_pushhi (mem_push_rtx, regno);

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
cdp1802_expand_epilogue (void)
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

  /* Pop the stack for the stdarg save area.  */
  if (layout.stdarg_save_size)
    cdp1802_emit_addhi3_postreload (stack_pointer_rtx, stack_pointer_rtx,
				    GEN_INT (layout.stdarg_save_size));

  /* Return.  */
  gcc_assert (one_below_p);
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

/* Worker function for cbranchhi4 expander.  */

void
cdp1802_expand_cbranchhi4 (enum rtx_code code, rtx op0, rtx op1, rtx loc)
{
  rtx condition_rtx, loc_ref, branch, cy_clobber;
  rtvec vec;
  machine_mode mode;

  mode = GET_MODE (op0);
  gcc_assert (mode == HImode);

  condition_rtx = gen_rtx_fmt_ee (code, mode, op0, op1);
  loc_ref = gen_rtx_LABEL_REF (VOIDmode, loc);
  branch = gen_rtx_SET (pc_rtx,
			gen_rtx_IF_THEN_ELSE (VOIDmode, condition_rtx,
					      loc_ref, pc_rtx));

  cy_clobber = gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (BImode, CDP1802_CARRY));

  vec = gen_rtvec (2, branch, cy_clobber);

  emit_jump_insn (gen_rtx_PARALLEL (VOIDmode, vec));
}

/* Worker function for call expanders.

   To make an indirect call on the CDP1802 we call a runtime library routine
   with the address as a hidden parameter in the static chain register, r11.  */

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
      /* This becomes r11 via the z constraint in the insn template.  */
      temp = gen_reg_rtx (HImode);
      emit_move_insn (temp, dest);
    }
  else
    temp = const0_rtx;

  call = gen_rtx_PARALLEL (VOIDmode, gen_rtvec (2, call,
						gen_rtx_USE (VOIDmode, temp)));
  emit_call_insn (call);
}

#undef TARGET_HARD_REGNO_MODE_OK
#define TARGET_HARD_REGNO_MODE_OK cdp1802_hard_regno_mode_ok
#undef TARGET_MODES_TIEABLE_P
#define TARGET_MODES_TIEABLE_P cdp1802_modes_tieable_p

#undef TARGET_SECONDARY_RELOAD
#define TARGET_SECONDARY_RELOAD cdp1802_secondary_reload

#undef TARGET_CAN_ELIMINATE
#define TARGET_CAN_ELIMINATE cdp1802_can_eliminate

#undef TARGET_PUSH_ARGUMENT
#define TARGET_PUSH_ARGUMENT cdp1802_push_argument

#undef  TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG cdp1802_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE cdp1802_function_arg_advance
#undef  TARGET_BUILD_BUILTIN_VA_LIST
#define TARGET_BUILD_BUILTIN_VA_LIST cdp1802_build_builtin_va_list
#undef  TARGET_EXPAND_BUILTIN_VA_START
#define TARGET_EXPAND_BUILTIN_VA_START cdp1802_expand_builtin_va_start
#undef  TARGET_GIMPLIFY_VA_ARG_EXPR
#define TARGET_GIMPLIFY_VA_ARG_EXPR cdp1802_gimplify_va_arg_expr

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

#undef TARGET_TRAMPOLINE_INIT
#define TARGET_TRAMPOLINE_INIT cdp1802_trampoline_init

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P cdp1802_legitimate_address_p

#undef TARGET_FIXED_CONDITION_CODE_REGS
#define TARGET_FIXED_CONDITION_CODE_REGS cdp1802_fixed_condition_code_regs

#undef  TARGET_MEMORY_MOVE_COST
#define TARGET_MEMORY_MOVE_COST cdp1802_memory_move_cost
#undef  TARGET_RTX_COSTS
#define TARGET_RTX_COSTS cdp1802_rtx_costs

#undef  TARGET_ASM_CONSTRUCTOR
#define TARGET_ASM_CONSTRUCTOR cdp1802_asm_out_constructor
#undef  TARGET_ASM_DESTRUCTOR
#define TARGET_ASM_DESTRUCTOR cdp1802_asm_out_destructor

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

#undef  TARGET_HAVE_SPECULATION_SAFE_VALUE
#define TARGET_HAVE_SPECULATION_SAFE_VALUE speculation_safe_value_not_needed

struct gcc_target targetm = TARGET_INITIALIZER;

/* #include "gt-cdp1802.h" */	/* TODO: where does this come from? */
