/*Subroutines used for code generation on the Infineon xc16x cpu.
   Copyright (C)  2006
   Free Software Foundation, Inc.

   Contributed by Shrirang Khisti on behalf of KPIT Cummins Infosystems Ltd.

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
   the Free Software Foundation, 51 Franklin Street,Fifth Floor,
   Boston, MA 02110-1301, USA */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "recog.h"
#include "expr.h"
#include "function.h"
#include "optabs.h"
#include "toplev.h"
#include "c-pragma.h"
#include "tm_p.h"
#include "ggc.h"
#include "target.h"
#include "target-def.h"
static void xc16x_output_function_prologue (FILE * file, HOST_WIDE_INT size);
static void xc16x_output_function_epilogue (FILE * file, HOST_WIDE_INT size);
static const char *byte_reg (rtx x, int a);
static tree xc16x_handle_fndecl_attribute (tree * node, tree name,
					   tree args ATTRIBUTE_UNUSED,
					   int flags ATTRIBUTE_UNUSED,
					   bool * no_add_attrs);
static tree xc16x_handle_fndecl1_attribute (tree * node, tree name,
					    tree args ATTRIBUTE_UNUSED,
					    int flags ATTRIBUTE_UNUSED,
					    bool * no_add_attrs);
static int xc16x_interrupt_function_p (tree func);
static int xc16x_disable_interrupt_function_p (tree func);
static int xc16x_interrupt_noreturn_function_p (tree func);
static int xc16x_using_regno_function_p (tree func);
static void xc16x_insert_attributes (tree node, tree * attributes);
static int xc16x_far_data (tree decl);
int single_one_operand (rtx operand, enum machine_mode mode ATTRIBUTE_UNUSED);
int single_zero_operand (rtx operand,
			 enum machine_mode mode ATTRIBUTE_UNUSED);
void xc16x_ext_libcall (rtx symref);


/* Flags regarding pragmas supported specific to xc16x  */
static int pragma_interrupt_noreturn;
static int pragma_interrupt_handler;
static int pragma_disable_intr;


int fun_arg;
/* Word register names  */
static const char *const names_big[] =
  { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11",
  "r12", "r13", "r14", "r15"
};

/* Byte register names  r0 -r7 are byte addressable  */
static const char *
byte_reg (rtx x, int b)
{
  static const char *const names_small[] = {
    "rl0", "rh0", "rl1", "rh1", "rl2", "rh2", "rl3", "rh3",
    "rl4", "rh4", "rl5", "rh5", "rl6", "rh6", "rl7", "rh7"
  };
  return names_small[REGNO (x) * 2 + b];
}

/* Output bset instruction instead of and instruction  */
const char *
op_or_insn (rtx * operands)
{
  if (single_one_operand (operands[2], QImode))
    {
      switch (INTVAL (operands[2]))
	{
	case 1:
	  return "bset %T0.0";
	  break;
	case 2:
	  return "bset %T0.1";
	  break;
	case 4:
	  return "bset %T0.2";
	  break;
	case 8:
	  return "bset %T0.3";
	  break;
	case 16:
	  return "bset %T0.4";
	  break;
	case 32:
	  return "bset %T0.5";
	  break;
	case 64:
	  return "bset %T0.6";
	  break;
	case 128:
	  return "bset %T0.7";
	  break;
	}
    }
  else
    return "orb %X0,%X2";
}

/* Output bclr instruction instead of and instruction  */
const char *
op_and_insn (rtx * operands)
{
  if (single_zero_operand (operands[2], QImode))
    {
      switch (INTVAL (operands[2]))
	{
	case -2:
	  return "bclr %T0.0";
	  break;
	case -3:
	  return "bclr %T0.1";
	  break;
	case -5:
	  return "bclr %T0.2";
	  break;
	case -9:
	  return "bclr %T0.3";
	  break;
	case -17:
	  return "bclr %T0.4";
	  break;
	case -33:
	  return "bclr %T0.5";
	  break;
	case -65:
	  return "bclr %T0.6";
	  break;
	case 127:
	  return "bclr %T0.7";
	  break;
	}
    }
  else
    return "andb %X0,%X2";

}

/*  Following function checks weather operand in mode mode has
    only one bit = 1 in it or not  */
int
single_one_operand (rtx operand, enum machine_mode mode ATTRIBUTE_UNUSED)
{
  if (GET_CODE (operand) == CONST_INT)
    {
      /* We really need to do this masking because 0x80 in QImode is
         represented as -128 for example.  */
      if (exact_log2 (INTVAL (operand) & GET_MODE_MASK (mode)) >= 0)
	return 1;
    }
  return 0;
}


/*  Following function checks weather operand in mode mode has
    only one bit = 0 in it or not  */


int
single_zero_operand (rtx operand, enum machine_mode mode ATTRIBUTE_UNUSED)
{
  if (GET_CODE (operand) == CONST_INT)
    {
      /* We really need to do this masking because 0x80 in QImode is
         represented as -128 for example.  */
      if (exact_log2 (~INTVAL (operand) & GET_MODE_MASK (mode)) >= 0)
	return 1;
    }
  return 0;
}

/* Output HI mode move instructions  */
/* Alternative 0  Register / Postincrement to Register
   Alternative 1  Register to Pre decrement
   Alternative 2  Immediate to register
   Alternative 3  Memory to register
   Alternative 4  Register to memory  */


const char *
output_movhi_insn (rtx * operands)
{

  if (which_alternative == 0)
    {
      if (TARGET_LARGE)
	{
	  if (GET_CODE (operands[1]) != REG)
	    {
	      if (REGNO (XEXP (operands[1], 0)) > 1)
		/* output_asm_insn("extp #0x2,#0x1",XEXP(operands[1],0));  */
		output_asm_insn ("extp %f1,#0x1", XEXP (operands[1], 0));
	    }
	  return "mov %T0,%T1";
	}
      else
	return "mov %T0,%T1";
    }
  else if (which_alternative <= 2)
    {

      if (which_alternative == 2 && GET_CODE (operands[1]) == SYMBOL_REF)
	return "mov %T0,#SOF:%e1";
      return "mov %T0,%T1";
    }

  else if (which_alternative == 3)
    {
      if (GET_CODE (XEXP (operands[1], 0)) == CONST_INT)
	{
	  rtx xop[2];
	  rtx p_no[2];
	  xop[0] = operands[0];
	  xop[1] = XEXP (operands[1], 0);

	  if (TARGET_LARGE)
	    {
	      if (INTVAL (XEXP (operands[1], 0)) > 0xffff)
		{
		  p_no[0] =
		    gen_rtx_CONST_INT (HImode,
				       INTVAL (XEXP (operands[1], 0)) /
				       0x4000);
		  xop[1] =
		    gen_rtx_CONST_INT (HImode,
				       INTVAL (XEXP (operands[1], 0)) %
				       0x4000);
		  output_asm_insn ("mov %T0,%1", xop);
		  output_asm_insn ("extp %0,#0x1", p_no);
		  output_asm_insn ("mov [%T0],%T0", operands);
		  return "";
		}
	    }
	  output_asm_insn ("mov %T0,%1", xop);
	  output_asm_insn ("mov %T0,[%T0]", xop);
	  return "";
	}
      if (TARGET_LARGE)
	{
	  if (GET_CODE (XEXP (operands[1], 0)) == SYMBOL_REF
	      || GET_CODE (XEXP (operands[1], 0)) == CONST)
	    {

	      /* Following can be used for checking constant
	         if(!CONSTANT_POOL_ADDRESS_P(operands[1]))  */

	      if (GET_CODE (XEXP (operands[1], 0)) == CONST)
		{
		  output_asm_insn ("extp #pag: %T1,#0x1", operands);

		  return "mov %T0,pof:%T1";

		}

	      if (SYMBOL_REF_FLAGS (XEXP (operands[1], 0)) & 1)
		return "mov %T0,#SOF:%T1";
	      else
		output_asm_insn ("extp #pag: %T1,#0x1", operands);

	      return "mov %T0,pof:%T1";
	    }
	  else
	    {
	      if (GET_CODE (XEXP (operands[1], 0)) != PLUS
		  && REGNO (XEXP (operands[1], 0)) != 0)
		{
		  output_asm_insn ("extp %d1,#0x1", operands);
		}
	      return "mov	%e0,%e1";
	    }

	}
      else
	return "mov %T0,%T1";
    }
  else if (which_alternative == 4)
    {
      if (GET_CODE (XEXP (operands[0], 0)) == CONST_INT)
	{
	  rtx p_no[2];
	  rtx xop[2];
	  xop[1] = operands[1];
	  xop[0] = XEXP (operands[0], 0);
	  if (TARGET_LARGE)
	    {
	      if (INTVAL (XEXP (operands[0], 0)) > 0xffff)
		{
		  p_no[0] =
		    gen_rtx_CONST_INT (HImode,
				       INTVAL (XEXP (operands[0], 0)) /
				       0x4000);
		  output_asm_insn ("mov [-r0],%f1", xop);
		  xop[0] =
		    gen_rtx_CONST_INT (HImode,
				       INTVAL (XEXP (operands[0], 0)) %
				       0x4000);
		  output_asm_insn ("mov %f1,%0", xop);
		  output_asm_insn ("extp %0,#0x1", p_no);
		  output_asm_insn ("mov [%f1],%T1", operands);
		  output_asm_insn ("mov %f1,[r0+]", xop);
		  return "";
		}
	    }
	  output_asm_insn ("mov [-r0],%f1", xop);
	  output_asm_insn ("mov %f1,%0", xop);
	  output_asm_insn ("mov [%f1],%T1", xop);
	  output_asm_insn ("mov %f1,[r0+]", xop);
	  return "";
	}
      if (TARGET_LARGE)
	{
	  if (GET_CODE (XEXP (operands[0], 0)) == SYMBOL_REF
	      || GET_CODE (XEXP (operands[0], 0)) == CONST)
	    {
	      /* if(!MEM_SCALAR_P(operands[0])||SYMBOL_REF_FLAGS(XEXP(operands[0],0))!=2)
	         if(!CONSTANT_POOL_ADDRESS_P(XEXP(operands[0],0)))  */
	      output_asm_insn ("extp #pag: %T0,#0x1", operands);
	      return "mov pof:%T0,%T1";
	    }
	  else
	    {
	      if (GET_CODE (XEXP (operands[0], 0)) != PLUS
		  && REGNO (XEXP (operands[0], 0)) != 0)
		{
		  output_asm_insn ("extp %d0,#0x1", operands);
		}
	      return "mov	%e0,%e1";
	    }
	}
      else
	return "mov %T0,%T1";
    }
}

/* Output call instructions  */

const char *
output_call_insn (rtx * operands)
{
  char str[20];
  if (TARGET_LARGE)
    {
      if (REG_P (operands[1]))	/*Indirect Call */
	{
	  rtx label = gen_label_rtx ();

	  output_asm_insn ("push csp", operands);
	  output_asm_insn ("calli cc_UC,[%1]", operands);
	  return " ";
	}
      else if (REG_P (XEXP (operands[1], 0)))	/*Indirect Call */
	{
	  rtx label = gen_label_rtx ();
	  output_asm_insn ("push csp", operands);
	  output_asm_insn ("calli cc_UC,%1", operands);
	  return " ";
	}
      else
	{
	  if (strcmp ((XSTR (XEXP (operands[1], 0), 0)), "__main") == 0)
	    return "";
	  return "calls #SEG:%1,#SOF:%1";
	  return "";
	}
    }
  else				/* Call for Tiny Model  */
    {
      if (GET_CODE (operands[1]) == MEM && REG_P (XEXP (operands[1], 0)))
	return "calli cc_UC,%1";
      else if (REG_P (operands[1]))
	return "calli cc_UC,[%1]";	/* Indirect Call  */
      else if (GET_CODE (XEXP (operands[1], 0)) != PLUS)
	return "calla cc_UC,%1";
      else
	return " ";
    }
}


/* Output call instructions  */

const char *
output_call_insn1 (rtx * operands)
{
  if (TARGET_LARGE)		/* Large target Calls  */
    {
      if (REG_P (operands[0]))
	{
	  output_asm_insn ("push csp", operands);
	  output_asm_insn ("calli cc_UC,[%0]", operands);
	  return " ";
	}
      else if (REG_P (XEXP (operands[0], 0)))	/* Indirect Call  */
	{
	  rtx label = gen_label_rtx ();
	  output_asm_insn ("push csp", operands);
	  output_asm_insn ("calli cc_UC,%0", operands);
	  return " ";
	}
      else
	{
	  if (strcmp ((XSTR (XEXP (operands[0], 0), 0)), "__main") == 0)
	    return " ";
	  else
	    return "calls #SEG:%0 ,#SOF:%0";
	  return "";
	}
    }
  else				/* Call for tiny Model  */
    {
      if (GET_CODE (operands[0]) == MEM && REG_P (XEXP (operands[0], 0)))
	return "calli cc_UC,%0";
      else if (REG_P (operands[0]))
	return "calli cc_UC,[%0]";
      else
	{
	  if (strcmp ((XSTR (XEXP (operands[0], 0), 0)), "__main") == 0)
	    return " ";
	  else
	    return "calla cc_UC, %0";
	}
    }
}

/* Output cmppsi instruction  */
const char *
output_cmppsi_insn (rtx * operands)
{
  rtx label = gen_label_rtx ();
  if (which_alternative == 0)
    {
      output_asm_insn ("cmp %f0,%f1", operands);
      output_asm_insn ("jmpr cc_NE,%l0", &label);
      output_asm_insn ("cmp %e0,%e1", operands);
      (*targetm.asm_out.internal_label) (asm_out_file, "L",
					 CODE_LABEL_NUMBER (label));
    }


  else if (which_alternative == 1)
    {
      output_asm_insn ("cmp %f0,%e1", operands);
      output_asm_insn ("jmpr cc_NE,%l0", &label);
      output_asm_insn ("cmp %e0,%f1", operands);
      (*targetm.asm_out.internal_label) (asm_out_file, "L",
					 CODE_LABEL_NUMBER (label));
    }
  return " ";
}


/* Output move instruction for Pointers in large memory model  */
/*
Alternative:0 Register to Register
            1 Immediate to Register
            2 Register to memory
            3 Memory to Register
            4 Register to Predecrement  */

const char *
output_movpsi_insn (rtx * operands)
{
  switch (which_alternative)
    {
      /* Register to Register move  */
    case 0:
      {
	if (REGNO (operands[1]) == 1)
	  {
	    /* output_asm_insn("mov %f0,#0x2",operands);  */
	    output_asm_insn ("mov %f0,DPP2", operands);
	    output_asm_insn ("mov %e0,%e1", operands);
	    return "";
	  }
	else
	  {
	    if ((REGNO (operands[1]) - REGNO (operands[0])) == 1)
	      {
		output_asm_insn ("mov %e0,%e1", operands);
		return "mov %f0,%f1";
	      }
	    else
	      {
		output_asm_insn ("mov %f0,%f1", operands);
		return "mov %e0,%e1";
	      }
	  }
	break;
      }

      /*Immediate to register move  */
    case 1:
      {
	if (GET_CODE (operands[1]) == SYMBOL_REF
	    || GET_CODE (operands[1]) == CONST)
	  {
	    if (GET_CODE (operands[1]) == SYMBOL_REF)
	      {
		/*tree decl;
		   if(SYMBOL_REF_DECL (operands[1]))

		   decl = SYMBOL_REF_DECL (operands[1]);

		   if(TREE_CODE (decl) != VAR_DECL && TREE_CODE (decl)!=STRING_CST) */
		if (SYMBOL_REF_FLAGS (operands[1]) & 1)
		  {
		    output_asm_insn ("mov %e0,#SOF:%e1", operands);
		    return " ";
		  }
		else
		  {
		    output_asm_insn ("mov %f0,#PAG:%e1", operands);
		    output_asm_insn ("mov %e0,#POF:%e1", operands);
		    return "";
		  }
	      }
	    else
	      {
		output_asm_insn ("mov %f0,#PAG:%e1", operands);
		output_asm_insn ("mov %e0,#POF:%e1", operands);
	      }
	    return " ";
	  }
	else
	  {
	    if (INTVAL (operands[1]) != 0)
	      {
		/* output_asm_insn ("mov %f0,DPP2",operands); */
		rtx op[3];
		op[0] = operands[0];
		op[1] =
		  gen_rtx_CONST_INT (HImode, INTVAL (operands[1]) / 0x4000);
		if (GET_CODE (operands[1]) == LABEL_REF)
		  {
		    output_asm_insn ("mov %f0,#PAG:%f1", operands);
		    return "mov %e0,#POF:%e1";
		  }
		output_asm_insn ("mov %f0,#0x3", op);
		return "mov %e0,%f1";
	      }
	    else
	      {
		output_asm_insn ("mov %f0,#0x0", operands);
		return "mov %e0,#0x0";
	      }
	  }
      }

      /* Register to memory move  */
    case 2:
      {
	if (GET_CODE (XEXP (operands[0], 0)) == SYMBOL_REF
	    || GET_CODE (XEXP (operands[0], 0)) == CONST)
	  {
	    output_asm_insn ("extp #pag:%e0,#0x2", operands);
	    if (REGNO (operands[1]) == 1)
	      return "mov pof:%f0,DPP2\n\tmov    pof:%e0,%e1";
	    else
	      return "mov pof:%f0,%f1\n\tmov    pof:%e0,%e1";
	  }
	else
	  {
	    if (GET_CODE (XEXP (operands[0], 0)) == PLUS)
	      {
		if (REGNO (XEXP (XEXP (operands[0], 0), 0)) != 1
		    && REGNO (XEXP (XEXP (operands[0], 0), 0)) != 0)
		  {
		    output_asm_insn ("extp %Y0,#0x2", operands);
		  }
	      }
	    else
	      {
		output_asm_insn ("extp %d0,#0x2", operands);
	      }
	    return "mov	%f0,%f1\n\tmov	%e0,%e1";
	  }
      }
      /* Memory to register move */
    case 3:
      {
	if (GET_CODE (XEXP (operands[1], 0)) == SYMBOL_REF
	    || GET_CODE (XEXP (operands[1], 0)) == CONST)
	  {
	    /* Original */
	    output_asm_insn ("extp #pag:%e1,#0x2", operands);
	    return "mov %f0,pof:%f1\n\tmov    %e0,pof:%e1";
	  }
	else
	  {
	    if (GET_CODE (XEXP (operands[1], 0)) == PLUS)
	      {
		if (REGNO (XEXP (XEXP (operands[1], 0), 0)) != 1
		    && REGNO (XEXP (XEXP (operands[1], 0), 0)) != 0)
		  {
		    output_asm_insn ("extp %Y1,#0x2", operands);
		    if (REGNO (XEXP (XEXP (operands[1], 0), 0)) ==
			(REGNO (operands[0]) + 1))
		      return "mov  %e0,%e1\n\tmov  %f0,%f1";
		  }
	      }
	    else
	      {
		output_asm_insn ("extp %d1,#0x2", operands);
	      }
	    return "mov	%f0,%f1\n\tmov	%e0,%e1";
	  }
      }
    case 4:
      return "mov	%e0,%f1\n\tmov	%e0,%e1";
    }
}


/* Output addition specially for pointers for Large target  */

const char *
output_addpsi_insn (rtx insn1, rtx * operands, int flag)
{
  rtx xop[3];
  switch (which_alternative)
    {
    case 0:
      {
	if (flag == 0)
	  {
	    if (REGNO (operands[2]) < 2)
	      {
		output_asm_insn ("add %e0,%e2", operands);

		return "mov %f0,DPP2";
	      }
	    else
	      {
		output_asm_insn ("add %e0,%e2", operands);

		return "addc %f0,%f2";
	      }
	  }
	else if (flag == 1)
	  {
	    if (REGNO (operands[2]) < 2)
	      {
		output_asm_insn ("sub %e0,%e2", operands);

		return "mov %f0,DPP2";
	      }
	    else
	      {
		output_asm_insn ("sub %e0,%e2", operands);
		return "sub %f0,%f2";
	      }
	  }
      }
    case 1:
      {
	xop[0] = operands[0];
	xop[2] = gen_rtx_CONST_INT (VOIDmode, INTVAL (operands[2]) & 0xffff);
	if (flag == 0)
	  output_asm_insn ("add %e0,%f2", xop);
	else if (flag == 1)
	  output_asm_insn ("sub  %e0,%f2", xop);
	return " ";
      }
    }
}


/*  Following function will output a particular operand to target assembly file  */
/*
Codes used:
X: Used to output  registers for QI mode
T: Used for outputting HI mode registers
e: Used in SI / SF mode
f: Used in SI / SF mode .In this case register with next immediate high number is printed as operand
c: In case of [Rn] used to print operand as [Rn+1] used in SI/Sf modes
d: In case of [Rn] used to print operand as Rn+1 used in SI/Sf modes
D: In case of [Rn] used to print operand as Rn used in SI/Sf modes
Y: in case of [Rn+#0xaaa] used to print operand as Rn+1 used in SI/Sf modes  */

void
print_operand (FILE * file, rtx x, int code)
{
  switch (code)
    {
    case 'X':
      if (GET_CODE (x) == REG)
	{
	  if (REGNO (x) <= 7)
	    fprintf (file, "%s", byte_reg (x, 0));
	  else
	    fprintf (file, "%s", names_big[REGNO (x)]);
	}
      else if (GET_CODE (x) == CONST_INT)
	fprintf (file, "#0x%x", (unsigned char) INTVAL (x));
      else
	goto def;
      break;
    case 'T':
      if (GET_CODE (x) == REG)
	fprintf (file, "%s", names_big[REGNO (x)]);
      else
	goto def;
      break;
    case 'e':
      switch (GET_CODE (x))
	{
	case REG:
	  fprintf (file, "%s", names_big[REGNO (x)]);
	  break;
	case MEM:
	  print_operand (file, x, 0);
	  break;
	case CONST_INT:
	  fprintf (file, "#%ld", ((INTVAL (x) >> 16) & 0xffff));
	  break;
	case CONST_DOUBLE:
	  {
	    long val;
	    REAL_VALUE_TYPE rv;
	    REAL_VALUE_FROM_CONST_DOUBLE (rv, x);
	    REAL_VALUE_TO_TARGET_SINGLE (rv, val);
	    fprintf (file, "#0x%x", ((val >> 16) & 0xffff));
	    break;
	  }
	case SYMBOL_REF:
	case LABEL_REF:
	case CONST:
	  print_operand_address (file, x);
	  break;
	default:
	  abort ();
	  break;
	}
      break;
    case 'f':
      switch (GET_CODE (x))
	{
	case REG:
	  fprintf (file, "%s", names_big[REGNO (x) + 1]);
	  break;
	case MEM:
	  x = adjust_address (x, HImode, 2);
	  print_operand (file, x, 0);
	  break;
	case CONST_INT:
	  fprintf (file, "#%ld", INTVAL (x) & 0xffff);
	  break;
	case CONST_DOUBLE:
	  {
	    long val;
	    REAL_VALUE_TYPE rv;
	    REAL_VALUE_FROM_CONST_DOUBLE (rv, x);
	    REAL_VALUE_TO_TARGET_SINGLE (rv, val);
	    fprintf (file, "#0x%x", (val & 0xffff));
	    break;
	  }
	case SYMBOL_REF:
	case LABEL_REF:
	case CONST:
	  print_operand_address (file, x);
	  break;
	default:
	  abort ();
	}
      break;
    case 'i':
      {
	fprintf (file, "%ld", INTVAL (x) & 0xffff);
	break;
      }
    case 'c':
      {
	int reg_no;

	fprintf (file, "[%s]", names_big[REGNO (XEXP (x, 0)) + 1]);
	break;
      }
    case 'd':
      {
	int reg_no;
	fprintf (file, "%s", names_big[REGNO (XEXP (x, 0)) + 1]);
	break;
      }

      /* To output reg contained in mem in Large Taregt option */
    case 'D':
      {
	int reg_no;
	fprintf (file, "%s", names_big[REGNO (XEXP (x, 0))]);
	break;
      }
    case 'Y':
      {
	int reg_no;
	fprintf (file, "%s", names_big[REGNO (XEXP (XEXP (x, 0), 0)) + 1]);
	break;
      }
    case 's':
      fprintf (file, "%d", INTVAL (x));
      break;
    default:
    def:
      switch (GET_CODE (x))
	{
	case REG:
	  switch (GET_MODE (x))
	    {
	    case QImode:
	      fprintf (file, "%s", byte_reg (x, 0));
	      break;
	    case HImode:
	    case SImode:
	    case PSImode:
	    case SFmode:
	      fprintf (file, "%s", names_big[REGNO (x)]);
	      break;
	    default:
	      abort ();
	    }
	  break;
	case MEM:
	  {
	    rtx addr = XEXP (x, 0);

	    if (GET_CODE (addr) == SYMBOL_REF || GET_CODE (addr) == LABEL_REF
		|| GET_CODE (addr) == CONST)
	      output_address (addr);
	    else
	      {
		fprintf (file, "[");
		output_address (addr);
		fprintf (file, "]");
	      }
	  }
	  break;
	case CONST_INT:
	case CONST:
	case LABEL_REF:
	case SYMBOL_REF:
	  fprintf (file, "#");
	  print_operand_address (file, x);
	  break;
	case CONST_DOUBLE:
	  {
	    long val;
	    REAL_VALUE_TYPE rv;
	    REAL_VALUE_FROM_CONST_DOUBLE (rv, x);
	    REAL_VALUE_TO_TARGET_SINGLE (rv, val);
	    fprintf (file, "#0x%x", (unsigned short) val);
	    break;
	  }
	default:
	  break;
	}
    }
}

void
print_operand_address (FILE * file, rtx addr)
{
  switch (GET_CODE (addr))
    {
    case REG:
      fprintf (file, "%s", names_big[REGNO (addr)]);
      break;
    case PRE_DEC:
      fprintf (file, "-%s", names_big[REGNO (XEXP (addr, 0))]);
      break;
    case POST_INC:
      fprintf (file, "%s+", names_big[REGNO (XEXP (addr, 0))]);
      break;
    case PLUS:
      if (GET_CODE (XEXP (addr, 0)) == REG)
	{
	  print_operand_address (file, XEXP (addr, 0));
	  fprintf (file, "+#");
	  print_operand_address (file, XEXP (addr, 1));
	}
      else
	{
	  print_operand_address (file, XEXP (addr, 0));
	  fprintf (file, "+#");
	  print_operand_address (file, XEXP (addr, 1));
	}
      break;
    case CONST_INT:
      {
	int n = INTVAL (addr);
	n = (int) (short) n;
	fprintf (file, "0x%x", (unsigned short) n);
	break;
      }
    default:
      output_addr_const (file, addr);
      break;
    }
}


  /* Following function emits the code for function prologue
     It takes in account weather frame pointer is needed or not
     Also among other activities it will check for certain attributes
     and genrate that particular code. Also it will generate code
     different related to different target options  */

static void
xc16x_output_function_prologue (FILE * file, HOST_WIDE_INT size)
{
  register int regno;
  int val;
  if (xc16x_disable_interrupt_function_p (current_function_decl))
    fprintf (file, "\t bclr psw.11\n");
  if (xc16x_interrupt_noreturn_function_p (current_function_decl))
    return;

/* Following statement will round up the value to nearest big even number  */
  size = (size + 1) & ~1;

/* Following code checks frame pointer requirement  */
  if (frame_pointer_needed)
    {
      /* Save the frame pointer  */
      fprintf (file, "\t mov  [-r%d],r%d\n", STACK_POINTER_REGNUM,
	       FRAME_POINTER_REGNUM);
      /* Set new frame pointer  */
      if (!xc16x_using_regno_function_p (current_function_decl))
	fprintf (file, "\t mov  r%d,r%d\n", FRAME_POINTER_REGNUM,
		 STACK_POINTER_REGNUM);
    }

/* Here  attribute relate functions should be called to check for the existance of a
   certain attribute and generate the code correspondingly  */

/* if (xc16x_using_regno_function_p (current_function_decl))
 fprintf(file,"%d",fun_arg);  */

/* switch the register bank with provided along with attribute  */
  if (xc16x_using_regno_function_p (current_function_decl))
    {
      val =
	TREE_INT_CST_LOW (TREE_VALUE
			  (TREE_VALUE
			   (lookup_attribute
			    ("using_regno",
			     DECL_ATTRIBUTES (current_function_decl)))));
      fprintf (file, "\t mov %d,r0 \n", val);
      fprintf (file, "\t scxt cp,#%d \n", val);
      /* fprintf(file,"\t reti \n"); */
      fprintf (file, "\t mov  r%d,r%d\n", FRAME_POINTER_REGNUM,
	       STACK_POINTER_REGNUM);
    }

/* Push DPP0 on the stack  */
  if (xc16x_interrupt_function_p (current_function_decl))
    {
      if (!TARGET_NO_DPPSAVE)
	fprintf (file, "push DPP0\n");
    }
  if (size < 0xffff)
    fprintf (file, "\t sub  r%d,#%d\n", STACK_POINTER_REGNUM, size + (0));
  else
    error ("Stack size is greater that 64K");
  if (xc16x_interrupt_function_p (current_function_decl)
      && !xc16x_using_regno_function_p (current_function_decl))
    {
      for (regno = 2; regno < FIRST_PSEUDO_REGISTER; regno++)
	if (regs_ever_live[regno])
	  fprintf (file, "\tpush r%d\n", (regno));
    }
  else
    {
      for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
	if (regs_ever_live[regno] && !call_used_regs[regno])
	  /* fprintf(file,"\tmov [-r%d],r%d\n",STACK_POINTER_REGNUM,(regno)); */
	  fprintf (file, "\t push r%d \n", (regno));
    }
}

static void
xc16x_output_function_epilogue (FILE * file, HOST_WIDE_INT size)
{
  register int regno;
  if (xc16x_disable_interrupt_function_p (current_function_decl))
    fprintf (file, "\t bset psw.11\n");

  /* If function has been provided with function_noreturn it wont generate any epilogue  */

  if (xc16x_interrupt_noreturn_function_p (current_function_decl))
    return;
  size = (size + 1) & ~1;
  if (xc16x_interrupt_function_p (current_function_decl)
      && !xc16x_using_regno_function_p (current_function_decl))
    {
      for (regno = FIRST_PSEUDO_REGISTER - 1; regno >= 2; regno--)
	if (regs_ever_live[regno])
	  fprintf (file, "\tpop r%d\n", (regno));
    }
  else
    {
      for (regno = FIRST_PSEUDO_REGISTER - 1; regno >= 0; regno--)
	if (regs_ever_live[regno] && !call_used_regs[regno])
	  /* fprintf(file,"\tmov r%d,[r%d+]\n",(regno),STACK_POINTER_REGNUM); */
	  fprintf (file, "\t pop r%d \n", (regno));
    }
  if (size)
    fprintf (file, "\t add  r%d,#%d\n", STACK_POINTER_REGNUM, size + (0));

  /* Following function checks weather function is interrupt handler
     If so it will further check for target option -mno-dppsave
     if option is absent it will pop DPPO  */

  if (xc16x_interrupt_function_p (current_function_decl))
    {
      if (!TARGET_NO_DPPSAVE)
	fprintf (file, "pop DPP0\n");
    }

  /* Following function checks weather using_regno attribute is provided
     for the current function if so it will pop cp and emit reti
     instruction  */

  if (xc16x_using_regno_function_p (current_function_decl))
    {
      fprintf (file, "\t pop cp \n");
      if (frame_pointer_needed)
	fprintf (file, "\t mov  r%d,[r%d+]\n", FRAME_POINTER_REGNUM,
		 STACK_POINTER_REGNUM);
      fprintf (file, "\t reti \n");
      return;
    }

  /* Else  it will emit only reti instruction  */
  else if (xc16x_interrupt_function_p (current_function_decl))
    {
      if (frame_pointer_needed)
	fprintf (file, "\t mov  r%d,[r%d+]\n", FRAME_POINTER_REGNUM,
		 STACK_POINTER_REGNUM);
      fprintf (file, "\t reti \n");
      return;
    }
  if (frame_pointer_needed)
    {
      /* Restore the frame pointer  */
      fprintf (file, "\t mov  r%d,[r%d+]\n", FRAME_POINTER_REGNUM,
	       STACK_POINTER_REGNUM);
    }
  if (TARGET_LARGE)
    /* Changed from rets to ret  */
    fprintf (file, "rets\n");
  else
    fprintf (file, "ret\n");
}

  /* Following structure is regarding attribute specification
     Important fields are
     name : name of the attribute to be supported
     min-len,max len : length of arguments passed with attributes
     handler : function checking validity */

const struct attribute_spec xc16x_attribute_table[] = {
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler } */
  {"interrupt_handler", 0, 0, true, false, false,
   xc16x_handle_fndecl_attribute},
  {"disable_intr", 0, 0, true, false, false, xc16x_handle_fndecl_attribute},
  {"interrupt_noreturn", 0, 0, true, false, false,
   xc16x_handle_fndecl_attribute},
  {"using_regno", 1, 1, true, false, false, xc16x_handle_fndecl1_attribute},
  {"far_data", 0, 0, true, false, false, NULL},
  {NULL, 0, 0, false, false, false, NULL}
};

/* Handle an attribute requiring a FUNCTION_DECL; arguments as in
   struct attribute_spec.handler.  */

static tree
xc16x_handle_fndecl_attribute (tree * node, tree name,
			       tree args ATTRIBUTE_UNUSED,
			       int flags ATTRIBUTE_UNUSED,
			       bool * no_add_attrs)
{
  if (TREE_CODE (*node) != FUNCTION_DECL)
    {
      warning ("`%s' attribute only applies to functions",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }
  return NULL_TREE;
}

/* Handle using_regno attribute and store the value of argument passed with attribute in
 variable fun_arg  */

static tree
xc16x_handle_fndecl1_attribute (tree * node, tree name,
				tree args ATTRIBUTE_UNUSED,
				int flags ATTRIBUTE_UNUSED,
				bool * no_add_attrs)
{
  if (TREE_CODE (*node) != FUNCTION_DECL)
    {
      warning ("`%s' attribute only applies to functions",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }
  fun_arg = TREE_INT_CST_LOW (TREE_VALUE (args));
  return NULL_TREE;
}

/*  Following function will check weather interrupt_handler attribute is provided
    or not  */

static int
xc16x_interrupt_function_p (tree func)
{
  tree a;
  if (TREE_CODE (func) != FUNCTION_DECL)
    return 0;
  a = lookup_attribute ("interrupt_handler", DECL_ATTRIBUTES (func));
  return a != NULL_TREE;
}

/*  Following function will check weather using_regno attribute is provided
    or not  */

static int
xc16x_using_regno_function_p (tree func)
{
  tree a;
  if (xc16x_interrupt_function_p (func))
    {
      if (TREE_CODE (func) != FUNCTION_DECL)
	return 0;

      a = lookup_attribute ("using_regno", DECL_ATTRIBUTES (func));
      return a != NULL_TREE;
    }
  else
    {
      /* warning("Missing interrupt handler attribute");  */
      return 0;
    }
}

/*  Following function will check weather far attribute is provided or not to variable
    or not  */

static int
xc16x_far_data (tree decl)
{
  tree a;
  if (TREE_CODE (decl) != VAR_DECL)
    return 0;
  a = lookup_attribute ("far_data", DECL_ATTRIBUTES (decl));
  return a != NULL_TREE;
}

/*  Following function will check weather disable_intr attribute is provided
    or not  */

static int
xc16x_disable_interrupt_function_p (tree func)
{
  tree a;
  if (TREE_CODE (func) != FUNCTION_DECL)
    return 0;
  a = lookup_attribute ("disable_intr", DECL_ATTRIBUTES (func));
  return a != NULL_TREE;
}

/*  Following function will check weather interrupt_noreturn attribute is provided
    or not */

static int
xc16x_interrupt_noreturn_function_p (tree func)
{
  tree a;
  if (xc16x_interrupt_function_p (func))
    {
      if (TREE_CODE (func) != FUNCTION_DECL)
	return 0;
      a = lookup_attribute ("interrupt_noreturn", DECL_ATTRIBUTES (func));
      return a != NULL_TREE;
    }
  else
    {
      /*warning("Missing interrupt handler attribute"); */
      return 0;
    }
}

/* Generate  attributes for decls from pragmas.  We convert
   all the pragmas to corresponding attributes. by examining corresponding
   flags set for corresponding pragmas  */

static void
xc16x_insert_attributes (tree node, tree * attributes)
{
  if (TREE_CODE (node) == FUNCTION_DECL)
    {
      if (pragma_interrupt_handler)
	{
	  pragma_interrupt_handler = 0;

	  /* Add an 'interrupt_handler' attribute.  */
	  *attributes = tree_cons (get_identifier ("interrupt_handler"),
				   NULL, *attributes);
	}
      if (pragma_disable_intr)
	{
	  pragma_disable_intr = 0;

	  /* Add disable_intr attribute  */
	  *attributes = tree_cons (get_identifier ("disable_intr"),
				   NULL, *attributes);
	}
      if (pragma_interrupt_noreturn)
	{
	  pragma_interrupt_noreturn = 0;
	  *attributes = tree_cons (get_identifier ("interrupt_noreturn"),
				   NULL, *attributes);
	}
    }
}

/*  Following function is initialization function
    for pragma disable_intr and will set corresponding flag  */

void
xc16x_disable_intr (struct cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  pragma_disable_intr = 1;
}

/*  Following function is initialization function
    for pragma interrupt_handler and will set corresponding flag  */

void
xc16x_interrupt_handler (struct cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  pragma_interrupt_handler = 1;
}

/*  Following function is initialization function
    for pragma interrupt_noreturn and will set corresponding flag  */

void
xc16x_interrupt_noreturn (struct cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  pragma_interrupt_noreturn = 1;
}

/* Return true if OP is a valid source operand for an integer move
   instruction.  */

int
general_operand_src1 (rtx op, enum machine_mode mode)
{
  if (GET_MODE (op) == mode
      && GET_CODE (op) == MEM && GET_CODE (XEXP (op, 0)) == POST_INC)
    return 1;
  return general_operand (op, mode);
}

/* Return true if OP is a valid source operand for an QI mode move
   instruction.  */

int
general_operand_dst_byte1 (rtx op, enum machine_mode mode)
{
  if (GET_CODE (op) == REG)
    {
      if (REGNO (op) > 7)
	return 0;
    }
  if (GET_MODE (op) == mode
      && GET_CODE (op) == MEM && GET_CODE (XEXP (op, 0)) == PRE_DEC)
    return 1;
  return general_operand (op, mode);
}

/* Return true if OP is a valid destination operand for an integer move
   instruction.  */

int
general_operand_dst1 (rtx op, enum machine_mode mode)
{
  if (GET_MODE (op) == mode
      && GET_CODE (op) == MEM && (GET_CODE (XEXP (op, 0)) == PRE_DEC))
    return 1;
  return general_operand (op, mode);
}

/* Following function will check weather given operand op is valid source
   operand such that it should not be any indirect base+offset form  */

int
general_operand_src_mem1 (rtx op, enum machine_mode mode)
{
  if (GET_CODE (op) == MEM && GET_CODE (XEXP (op, 0)) == CONST_INT)
    return 0;
  if (GET_CODE (op) == MEM
      && ((GET_CODE (XEXP (op, 0)) == SYMBOL_REF)
	  || (GET_CODE (XEXP (op, 0))) == CONST))
    return 1;
  return register_operand (op, mode);
}

/* Following function will check weather given operand op is valid destination
   operand such that it should not be any indirect base+offset form   */

int
general_operand_dst_mem1 (rtx op, enum machine_mode mode)
{
  if (GET_CODE (op) == MEM && GET_CODE (XEXP (op, 0)) == CONST_INT)
    return 0;
  if (GET_CODE (op) == MEM
      && ((GET_CODE (XEXP (op, 0)) == SYMBOL_REF)
	  || (GET_CODE (XEXP (op, 0))) == CONST))
    return 1;
  return nonmemory_operand (op, mode);
}

int
general_call_operand1 (rtx op, enum machine_mode mode)
{
  if (register_operand (op, 0))
    return 1;
  if (GET_CODE (XEXP (op, 0)) == REG || GET_CODE (XEXP (op, 0)) == SYMBOL_REF)
    return 1;
  return 0;
  return general_operand (op, mode);
}

void
xc16x_ext_libcall (rtx symref)
{
  if (strcmp (XSTR (symref, 0), "__main") != 0)
    {
      fprintf (asm_out_file, "\n .global ");
      fprintf (asm_out_file, "%s\n", XSTR (symref, 0));
    }
}

/* Following function calculates length for a function call it accepts rtx
   operands and number as integer which distinguishes for which funtion to calculate length  */

unsigned int
compute_call_length (rtx * operands, int number)
{
  enum machine_mode mode = GET_MODE (operands[number]);
  if (TARGET_LARGE)
    {
      if (REG_P (operands[number]))
	return 22;
      else if (REG_P (XEXP (operands[number], 0)))
	{
	  return 22;
	}
      else
	{
	  if (strcmp ((XSTR (XEXP (operands[number], 0), 0)), "__main") == 0)
	    return 0;
	  else
	    return 4;
	}
    }
  else
    {
      if (GET_CODE (operands[number]) == MEM
	  && REG_P (XEXP (operands[number], 0)))
	return 2;
      else
	return 4;
    }
}

/* Following function returns the length of add and subtract instructions  */

unsigned int
compute_add_length (rtx * operands)
{
  enum machine_mode mode = GET_MODE (operands[0]);
  if (TARGET_LARGE)
    {
      switch (mode)
	{
	case QImode:
	case HImode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 2;
	    case 1:
	      return 4;
	    case 2:
	    case 3:
	    case 4:
	      return 8;
	    }
	case SImode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 4;
	    case 1:
	      return 8;
	    case 2:
	      return 12;
	    case 3:
	      return 12;
	    case 4:
	      return 12; /* FIXME */
	    }
	}
    }
  else
    {
      switch (mode)
	{
	case QImode:
	case HImode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 2;
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	      return 4;
	    }
	case SImode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 4;
	    case 1:
	    case 2:
	    case 3:
	      return 8;
	    case 4:
	      return 8; /* FIXME */
	    }
	}
    }
}

unsigned int
compute_mov_length (rtx * operands)
{
  enum machine_mode mode = GET_MODE (operands[0]);
  if (TARGET_LARGE)
    {
      switch (mode)
	{
	case QImode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 2;
	    case 1:
	      return 2;
	    case 2:
	      return 8;
	    case 3:
	      return 8;
	    case 4:
	      return 8;
	    }
	  break;
	case HImode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 2;
	    case 1:
	      return 2;
	    case 2:
	      return 8;
	    case 3:
	      if (GET_CODE (XEXP (operands[1], 0)) == SYMBOL_REF
		  || GET_CODE (XEXP (operands[1], 0)) == CONST)
		return 8;
	      else if (GET_CODE (XEXP (operands[1], 0)) != PLUS
		       && REGNO (XEXP (operands[1], 0)) != 0)
		return 8;
	      else
		return 4;
	    case 4:
	      if (GET_CODE (XEXP (operands[0], 0)) == SYMBOL_REF
		  || GET_CODE (XEXP (operands[0], 0)) == CONST)
		return 8;
	      else if (GET_CODE (XEXP (operands[0], 0)) != PLUS
		       && REGNO (XEXP (operands[0], 0)) != 0)
		return 8;
	      else
		return 4;
	    }
	  break;
	case SImode:
	case SFmode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 4;
	    case 1:
	      return 4;
	    case 2:
	      return 12;
	    case 3:
	      if (GET_CODE (XEXP (operands[1], 0)) == SYMBOL_REF
		  || GET_CODE (XEXP (operands[1], 0)) == CONST)
		return 12;
	      else if (GET_CODE (XEXP (operands[1], 0)) != PLUS
		       && REGNO (XEXP (operands[1], 0)) != 0)
		return 12;
	      else
		return 8;
	    case 4:
	      if (GET_CODE (XEXP (operands[0], 0)) == SYMBOL_REF
		  || GET_CODE (XEXP (operands[0], 0)) == CONST)
		return 12;
	      else if (GET_CODE (XEXP (operands[0], 0)) != PLUS
		       && REGNO (XEXP (operands[0], 0)) != 0)
		return 12;
	      else
		return 8;
	    }
	  break;
	}
    }
  else
    {
      switch (mode)
	{
	case QImode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 2;
	    case 1:
	      return 2;
	    case 2:
	      return 4;
	    case 3:
	      return 4;
	    case 4:
	      return 4;
	    }
	  break;
	case HImode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 2;
	    case 1:
	      return 2;
	    case 2:
	      return 4;
	    case 3:
	      return 4;
	    case 4:
	      return 4;
	    }
	  break;
	case SImode:
	case SFmode:
	  switch (which_alternative)
	    {
	    case 0:
	      return 4;
	    case 1:
	      return 4;
	    case 2:
	      return 8;
	    case 3:
	      return 8;
	    case 4:
	      return 8;
	    }
	  break;
	}
    }
}

/* If the next function argument with MODE and TYPE is to be passed in
   a register, return a reg RTX for the hard register in which to pass
   the argument.  CUM represents the state after the last argument.
   If the argument is to be pushed, NULL_RTX is returned.  */

rtx
function_arg (int cum, enum machine_mode mode, tree type, int named)
{
  rtx result = NULL_RTX;
  const char *fname;
  int regpass = 5;
  /* Never pass unnamed arguments in registers.  */
  if (!named)
    return NULL_RTX;
  if (regpass)
    {
      int size;
      if (mode == BLKmode)
	size = int_size_in_bytes (type);
      else
	size = GET_MODE_SIZE (mode);
      if (cum < (NREGS_FOR_REG_PARM * UNITS_PER_WORD - 1)
	  && (size + cum) <= NREGS_FOR_REG_PARM * UNITS_PER_WORD)
	result = gen_rtx_REG (mode, 8 + (cum) / UNITS_PER_WORD);
    }
  return result;
}
extern int target_flags;
xc16x_file_start (void)
{
  default_file_start ();
  if (TARGET_LARGE)
    fputs ("\t.xc16xl\n", asm_out_file);
  else if (TARGET_SMALL)
    fputs ("\t.xc16xs\n", asm_out_file);
  else
    fputs ("\t.xc16x\n", asm_out_file);
}

#undef  TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE xc16x_output_function_prologue
#undef  TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE xc16x_output_function_epilogue
#undef TARGET_ASM_FILE_START_FILE_DIRECTIVE
#define TARGET_ASM_FILE_START_FILE_DIRECTIVE true
#undef TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE xc16x_attribute_table
#undef TARGET_ASM_FILE_START
#define TARGET_ASM_FILE_START xc16x_file_start
#undef TARGET_DEFAULT_TARGET_FLAGS
#define TARGET_DEFAULT_TARGET_FLAGS TARGET_DEFAULT
#undef TARGET_INSERT_ATTRIBUTES
#define TARGET_INSERT_ATTRIBUTES xc16x_insert_attributes
#undef TARGET_ASM_ALIGNED_HI_OP
#define TARGET_ASM_ALIGNED_HI_OP "\t.word\t"
#undef TARGET_ASM_EXTERNAL_LIBCALL
#define TARGET_ASM_EXTERNAL_LIBCALL xc16x_ext_libcall
struct gcc_target targetm = TARGET_INITIALIZER;
