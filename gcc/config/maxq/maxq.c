/*  Copyright (C) 2004, 2005 Free Software Foundation, Inc.
    Contributed by: 
	    Prabu Pariasamy (prabup@noida.hcltech.com)
	    Konark Goel (konarkg@noida.hcltech.com)
	    C Jaiprakash (cjaiprakash@noida.hcltech.com).

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "insn-config.h"
#include "rtl.h"
#include "tree.h"
#include "flags.h"
#include "expr.h"
#include "optabs.h"
#include "function.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "basic-block.h"
#include "real.h"
#include "insn-attr.h"
#include "conditions.h"
#include "output.h"
#include "libfuncs.h"
#include "recog.h"
#include "cpplib.h"
#include "toplev.h"
#include "tm_p.h"
#include "reload.h"
#include "target.h"
#include "target-def.h"
#include "langhooks.h"
#include "ggc.h"

/* Variables maxq_compare_op0 and maxq_compare_op1 are used to save the last
   compare operands.
   On MAXQ there is no compare insn. When subtracting operand1 from operand0
   status register (carry, sign and zero bits) will be affectd based on the result.
   If it is signed comparison, first we need to toggle the sign bits of
   the both operands then generate the subtract insn.
   'maxq_signed_cmp' tells whether the last comparison is signed.
   So using these variables branch insn is generated.  */
rtx maxq_compare_op0;
rtx maxq_compare_op1;
int maxq_signed_cmp;

/* Target hooks.  */
static void target_asm_function_begin_epilogue (FILE *);
static void maxq_asm_file_start (void);
static void maxq_setup_incoming_varags (CUMULATIVE_ARGS *,
       enum machine_mode, tree, int *, int);

static bool maxq_must_pass_in_stack (enum machine_mode, tree);
static rtx maxq_struct_value_rtx (tree, int);
static bool maxq_pass_by_reference (CUMULATIVE_ARGS *, enum machine_mode, tree, bool);
static int maxq_arg_partial_bytes (CUMULATIVE_ARGS *, enum machine_mode, tree, bool);

#undef TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES maxq_arg_partial_bytes

#undef TARGET_ASM_FILE_START
#define TARGET_ASM_FILE_START maxq_asm_file_start

#undef TARGET_ASM_FUNCTION_BEGIN_EPILOGUE
#define TARGET_ASM_FUNCTION_BEGIN_EPILOGUE target_asm_function_begin_epilogue
 
#undef TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS maxq_setup_incoming_varags

#undef TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK maxq_must_pass_in_stack

#undef TARGET_STRUCT_VALUE_RTX 
#define TARGET_STRUCT_VALUE_RTX  maxq_struct_value_rtx

#undef TARGET_PASS_BY_REFERENCE 
#define TARGET_PASS_BY_REFERENCE maxq_pass_by_reference

struct gcc_target targetm = TARGET_INITIALIZER;

/* This function outputs assembler code for adjusting the stack pointer.  
  'stream' is a stdio stream to which the assembler code is output.  */
static void
target_asm_function_begin_epilogue (FILE * stream)
{
  fprintf (stream, "\tmove \tDPC, #1 \n");
  fprintf (stream, "\tmove \tNUL, @DP[1]++ \n");
  fprintf (stream, "\tmove \tNUL, @DP[1]++ \n");
}

/* Output the assembler syntax for operand 'op' to assembly language output file
   'file'. Variable  'letter' tells whether the operands is higher or lower byte.  */
void
maxq_print_operand (FILE * file, rtx op, int letter)
{
  enum rtx_code code;
  int regno;

  if (letter == 'L')
    {
      regno = REGNO (op);
      fprintf (file, "%s", reg_names[regno]);
      return;
    }
  if (letter == 'H')
    {
      regno = REGNO (op);
      fprintf (file, "%s", reg_names[regno + 1]);
      return;
    }

  code = GET_CODE (op);
  switch (code)
    {
    case REG:
      regno = REGNO (op);
      fprintf (file, "%s", reg_names[regno]);
      break;
    case CONST_INT:
      fprintf (file, "#%d", (int)INTVAL (op));
      break;
    case LABEL_REF:
      output_addr_const (file, op);
      break;
    case SYMBOL_REF:
    case CONST:
      output_addr_const (file, op);
      break;
    default:
      abort ();
    }
}

/* Return nonzero if op is suitable for call address of mode 'mode'.  */
int
call_address_operand (rtx op, enum machine_mode mode)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case SYMBOL_REF:
    case LABEL_REF:
      return 1;
    default:
      break;
    }
  return register_operand (op, mode);
}

/* Return nonzero if op is suitable for direct call address of mode 'mode'.  */
int
maxq_U_constraint (rtx op)
{
  /* Don't allow direct addressing to an arbitrary constant.  */
  return GET_CODE (op) == CONST
    || GET_CODE (op) == SYMBOL_REF || GET_CODE (op) == LABEL_REF;
}

/* Return nonzero if op is register or immediate operand of mode 'mode'.  */
int
reg_or_imm_operand (rtx op, enum machine_mode mode)
{
  if (immediate_operand (op, mode) || register_operand (op, mode))
    return 1;
  return 0;

}

/* Generate the compare insn. 
   'x' and 'y' are two operands of the compare insn.  */ 
void 
maxq_gen_compare_reg (enum rtx_code code ATTRIBUTE_UNUSED, rtx x, rtx y)
{
  if (TARGET_MAXQ20)
      emit_insn (gen_cmphi_1 (x, y));
  else
      emit_insn (gen_cmpqi_1 (x, y));
}

/* This function initializes frame pointer and stack pointer and 
   saves the callee saved registers.  */
void
maxq_expand_prologue (void)
{
  unsigned int regno;
  int size = get_frame_size () + current_function_pretend_args_size + 4;
  rtx insn;
  /* select the register A[15] as active accumulator.  */
  emit_move_insn (gen_rtx_REG (QImode, AP_REGNO), GEN_INT (0x0F));
  global_regs[AP_REGNO] = 1;

  if (size > 0)
    {
      /* adjust the stack pointer based on the frame size.  */
      if (TARGET_MAXQ20)
	{
	  emit_move_insn (gen_rtx_REG (HImode, ACC_REGNO),
			  gen_rtx_REG (HImode, SP_REGNO));

	  insn = emit_insn (gen_subhi_1 (GEN_INT (size)));

	  emit_move_insn (gen_rtx_REG (HImode, SP_REGNO),
			  gen_rtx_REG (HImode, ACC_REGNO));
	  RTX_FRAME_RELATED_P (insn) = 1;
	}
      else
	{

	  /* move GR,DP[1]  */
	  emit_insn (gen_move_gr (gen_rtx_REG (HImode, SP_REGNO)));
	  global_regs[GR_REGNO] = 1;
	  do
	    {
	      emit_insn (gen_move_grl_1 (gen_rtx_REG (QImode, ACC_REGNO)));
	      insn = emit_insn (gen_subqi_1
				(((size >
				   127) ? GEN_INT (127) : GEN_INT (size))));
	      emit_insn (gen_move_grl (gen_rtx_REG (QImode, ACC_REGNO)));
	      global_regs[GRL_REGNO] = 1;
	      emit_insn (gen_move_grh_1 (gen_rtx_REG (QImode, ACC_REGNO)));
	      /* subb #0  */
	      emit_insn (gen_subb_1_qi (GEN_INT (0)));
	      /* move GRH,Acc  */
	      emit_insn (gen_move_grh (gen_rtx_REG (QImode, ACC_REGNO)));
	      global_regs[GRH_REGNO] = 1;
	      size = size - 127;
	    }
	  while (size > 0);
	  insn = emit_insn (gen_move_gr_1 (gen_rtx_REG (HImode, SP_REGNO)));
	  RTX_FRAME_RELATED_P (insn) = 1;
	}
    }
  /* push the frame pointer on to the stack.  */
  emit_insn (gen_pushhi (gen_rtx_REG (HImode, BP_REGNO)));
  size = 2;
  /* adjust the frame pointer so that it points to first local variable.  */
  if (TARGET_MAXQ20)
    {
      emit_move_insn (gen_rtx_REG (HImode, ACC_REGNO),
		      gen_rtx_REG (HImode, SP_REGNO));
      emit_insn (gen_addhi3_1 (GEN_INT (size)));
      insn =
	emit_move_insn (gen_rtx_REG (HImode, BP_REGNO),
			gen_rtx_REG (HImode, ACC_REGNO));
      RTX_FRAME_RELATED_P (insn) = 1;
    }
  else
    {
      emit_insn (gen_move_gr (gen_rtx_REG (HImode, SP_REGNO)));	/*move GR,DP[1] */
      global_regs[GR_REGNO] = 1;

      emit_insn (gen_move_grl_1 (gen_rtx_REG (QImode, ACC_REGNO)));
      insn = emit_insn (gen_addqi3_1 (GEN_INT (size)));
      emit_insn (gen_move_grl (gen_rtx_REG (QImode, ACC_REGNO)));	/*move GRL,Acc */
      global_regs[GRL_REGNO] = 1;
      emit_insn (gen_move_grh_1 (gen_rtx_REG (QImode, ACC_REGNO)));
      /* addc #0  */
      insn = emit_insn (gen_addc_1_qi (GEN_INT (0)));
      /* move GRH, Acc  */
      insn = emit_insn (gen_move_grh (gen_rtx_REG (QImode, ACC_REGNO)));
      global_regs[GRH_REGNO] = 1;
      insn = emit_insn (gen_move_gr_1 (gen_rtx_REG (HImode, BP_REGNO)));
    }
  /*  save the registers which are being used by the current function.  */
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (regs_ever_live[regno] && !call_used_regs[regno])
	{
	  if (!(!TARGET_MAXQ20 && (IS_ACC_REGNO (regno)))
	      && (regno != GRL_REGNO) && (regno != GRH_REGNO))
	    insn = emit_insn (gen_pushhi (gen_rtx_REG (HImode, regno)));
	  else
	    insn = emit_insn (gen_pushqi (gen_rtx_REG (QImode, regno)));

	  RTX_FRAME_RELATED_P (insn) = 1;
	}
    }
  emit_insn (gen_prologue_end ());
}

/* This function restores stack and frame pointers and callee saved registers.  */
void
maxq_expand_epilogue (void)
{
  int regno;
  rtx insn;
  int size = get_frame_size () + current_function_pretend_args_size + 4;
  /* pop the saved registers.  */
  for (regno = FIRST_PSEUDO_REGISTER - 1; regno >= 0; regno--)
    {
      if (regs_ever_live[regno] && !call_used_regs[regno])
	{
	  if (!(!TARGET_MAXQ20 && (IS_ACC_REGNO (regno)))
	      && (regno != GRL_REGNO) && (regno != GRH_REGNO))
	    insn = emit_insn (gen_pophi (gen_rtx_REG (HImode, regno)));
	  else
	    insn = emit_insn (gen_popqi (gen_rtx_REG (QImode, regno)));

	  RTX_FRAME_RELATED_P (insn) = 1;
	}
    }
  /* Restore the frame pointer.  */
  insn = emit_insn (gen_pophi (gen_rtx_REG (HImode, BP_REGNO)));
  RTX_FRAME_RELATED_P (insn) = 1;

  /* Restore the stack pointer.  */
  if (size > 0)
    {
      rtx r, sp;
      if (TARGET_MAXQ20)
	{
	  r = gen_rtx_REG (HImode, ACC_REGNO);
	  sp = gen_rtx_REG (HImode, SP_REGNO);
	  emit_move_insn (r, sp);
	  insn = emit_insn (gen_addhi3_1 (GEN_INT (size)));
	  emit_move_insn (sp, r);
	  RTX_FRAME_RELATED_P (insn) = 1;
	}
      else
	{
	  /* move GR,DP[1]  */
	  emit_insn (gen_move_gr (gen_rtx_REG (HImode, SP_REGNO)));
	  global_regs[GR_REGNO] = 1;
	  do
	    {
	      emit_insn (gen_move_grl_1 (gen_rtx_REG (QImode, ACC_REGNO)));
	      insn =
		emit_insn (gen_addqi3_1
			   (((size > 127) ? GEN_INT (127) : GEN_INT (size))));
	      /* move GRL,Acc  */

	      emit_insn (gen_move_grl (gen_rtx_REG (QImode, ACC_REGNO)));
	      global_regs[GRL_REGNO] = 1;
	      emit_insn (gen_move_grh_1 (gen_rtx_REG (QImode, ACC_REGNO)));
	      /* addc #0  */
	      insn = emit_insn (gen_addc_1_qi (GEN_INT (0)));
	      /* move GRH,Acc  */
	      insn =
		emit_insn (gen_move_grh (gen_rtx_REG (QImode, ACC_REGNO)));
	      global_regs[GRH_REGNO] = 1;
	      size = size - 127;
	    }
	  while (size > 0);
	  insn = emit_insn (gen_move_gr_1 (gen_rtx_REG (HImode, SP_REGNO)));
	  RTX_FRAME_RELATED_P (insn) = 1;
	}
    }
}

/* save anonymous register arguments into the stack so that all the arguments 
   appear to have been passed consecutively on the stack.
   Variable cum->args tells the no of registers used so far.
   The arguments mode and type describe the last named argument.
   its machine mode and its data type as a tree node.
   No. of bytes pushed on the stack is assigned to the variable pointed by pretend_size.
   Do not save any register when no_rtl is 1.  */
static void 
maxq_setup_incoming_varags (CUMULATIVE_ARGS *cum,
       enum machine_mode mode, tree type ATTRIBUTE_UNUSED, int *pretend_size, int no_rtl)
{                                                                                       
   HOST_WIDE_INT i, off;   
   rtx mem;                                                                             
   *pretend_size = 0 ;                                                                   
   if (TARGET_MAXQ20 && !no_rtl)                                                        
   {                                                              
     HOST_WIDE_INT set = get_varargs_alias_set ();             
     i = cum->args + ARG_REGNO(mode);
     for(off = 0; i < A4_REGNO; i++, off+=2)                                              
     {                                                                                  
       mem = gen_rtx_MEM (HImode, plus_constant (virtual_incoming_args_rtx, off));   
       set_mem_alias_set (mem, set);                                                    
       emit_move_insn (mem, gen_rtx_REG (HImode, i));                                   
     }                                                                                  
    *pretend_size = off;                                                               
   }                                                                                    
   if (!TARGET_MAXQ20 && !no_rtl)                                                       
   {                                                                                    
     HOST_WIDE_INT set = get_varargs_alias_set ();                                      
     i = cum->args + ARG_REGNO(mode);                                                  
     for(off = 0; i < A4_REGNO ; i++, off++)                                               
     {                                                                                  
       mem = gen_rtx_MEM (QImode, plus_constant (virtual_incoming_args_rtx, off));
       set_mem_alias_set (mem, set);                                                    
       emit_move_insn (mem, gen_rtx_REG (QImode, i));                                   
     }                                                                                  
    *pretend_size = off;                                                               
   }                                                                                    
}

/* Return true if we should not pass type solely in registers.
   Structures and unions types are passed in stack.  */
static bool
maxq_must_pass_in_stack (enum machine_mode mode, tree type)
{
  return    (((mode) == BLKmode)                                         
         || ((type) != 0                                             
         && (TREE_CODE (TYPE_SIZE (type)) != INTEGER_CST        
             || TREE_CODE (type) == RECORD_TYPE                 
             || TREE_CODE (type) == UNION_TYPE                  
             || TREE_CODE (type) == QUAL_UNION_TYPE             
             || TREE_ADDRESSABLE (type))));
}

/* Return true if an argument at the position indicated by cum of mode 'mode' 
   and type 'type' should be passed by reference.
   Structures and unions are passed by reference.  */
static bool 
maxq_pass_by_reference (CUMULATIVE_ARGS *cum ATTRIBUTE_UNUSED, enum machine_mode mode,
                        tree type, bool named ATTRIBUTE_UNUSED)
{
  return maxq_must_pass_in_stack (mode, type);
}

/* Return the location of the structure value address.
   A[4] register is used to hold the structure value address.  */ 
static rtx 
maxq_struct_value_rtx (tree fndecl ATTRIBUTE_UNUSED, int incoming ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (Pmode, A4_REGNO);
}

/* Return the number of words, at the beginning of an argument, that must be put in registers.
   The value must be zero for arguments that are passed entirely in registers 
   or that are entirely pushed on the stack.  */
static int
maxq_arg_partial_bytes (CUMULATIVE_ARGS *cum, enum machine_mode mode,
				  tree type ATTRIBUTE_UNUSED, bool named ATTRIBUTE_UNUSED)
{
  if (cum->args < A4_REGNO && (cum->args + ARG_REGNO(mode)) > 4)
    {
      if (TARGET_MAXQ20)
        return (A4_REGNO - cum->args) * 2;
      else
        return (A4_REGNO - cum->args);
    }
  return 0;
}

/* This function outputs the source file name and version no to 
   the assembly language output file.  */
static void
maxq_asm_file_start (void)
{
  int maxqversion = 0;
  output_file_directive (asm_out_file, main_input_filename);
  fprintf (asm_out_file, "\t.version\t%d\n", maxqversion);
}
