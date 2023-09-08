;; Operand and operator predicates for the GCC PDP10 port.
;; Copyright (C) 2007 Free Software Foundation, Inc.

;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.
;;
;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING.  If not, write to
;; the Free Software Foundation, 51 Franklin Street, Fifth Floor,
;; Boston, MA 02110-1301, USA.


;; Operator predicates.

(define_predicate "pdp10_comparison_operator"
  (match_code "eq, ne, le, lt, ge, gt"))

(define_predicate "pdp10_equality_operator"
  (match_code "eq, ne"))

(define_predicate "pdp10_rotate_operator"
  (match_code "rotate, rotatert"))




;; Operand predicates.

(define_predicate "reg_or_mem_operand"
  (ior (match_operand 0 "register_operand")
        (match_operand 0 "memory_operand")))

(define_predicate "pdp10_maybe_volatile_memory_operand"
  (match_code "mem")
  {
	int saved = volatile_ok;
	int result;

	volatile_ok = 1;
	result = memory_operand (op, mode);
	volatile_ok = saved;
	return result;
  })

(define_predicate "pdp10_push_operand"
  (match_code "mem")
  {
  	rtx opaddr = XEXP (op, 0);
	rtx opreg;
  	if (GET_CODE(opaddr) != PRE_INC)
  		return 0;
	opreg = XEXP (XEXP (op, 0), 0);
  		
  /* Since PUSH is a slow instruction, only allow registers other than
     then stack pointer when optimizing for small code size.  */

	return (optimize_size	|| (REGNO(opreg) == STACK_POINTER_REGNUM));
  })

(define_predicate "pdp10_pop_operand"
  (match_code "mem")
  {
  	rtx opaddr = XEXP (op, 0);
	rtx opreg;
  	if (GET_CODE(opaddr) != POST_DEC)
  		return 0;
	opreg = XEXP (XEXP (op, 0), 0);
  		
  /* Since PUSH is a slow instruction, only allow registers other than
     then stack pointer when optimizing for small code size.  */

	return (optimize_size	|| (REGNO(opreg) == STACK_POINTER_REGNUM));
  })

(define_predicate "pdp10_const_double_0_operand"
  (match_code "const_double")
	{
	  REAL_VALUE_TYPE r;
	  long y[3];

	  REAL_VALUE_FROM_CONST_DOUBLE (r, op);
	  REAL_VALUE_TO_TARGET_DOUBLE (r, y);

	  switch (mode)
	    {
	    case SFmode:
	      switch HOST_BITS_PER_LONG
		{
		case 32:
		  return
		    (y[0] & 0xffffffffUL) == 0
		    && (y[1] & 0xf0000000UL) == 0;
		case 36:	
		  return (y[0] & 0xfffffffffUL) == 0;
		case 64:
		  return (y[0] & 0xfffffffff0000000UL) == 0;
		default:
		  return 0;
		}
	      break;
	    case DFmode:
	      switch HOST_BITS_PER_LONG
		{
		case 32:
		  return
		    (y[0] & 0xffffffffUL) == 0
		    && (y[1] & 0xffffffffUL) == 0
		    && (y[2] & 0xff000000UL) == 0;
		case 36:
		  return 
		    (y[0] &  0xfffffffffUL) == 0
		    && (y[1] &0xfffffffffUL) == 0;
		case 64:
		  return 
		    (y[0] & 0xffffffffffffffffUL) == 0
		    && (y[1] & 0xff00000000000000UL) == 0;
		default:
		  return 0;
		}
	    default:
	      return 0;
	    }
	}
  )

(define_special_predicate "byte_pointer_operand"
  (match_code "reg, mem")
  {
     enum machine_mode opmode = GET_MODE(op);
     int ispointermode = (opmode == HPmode || 
	 		opmode == Q9Pmode ||
	 		opmode == Q8Pmode ||
	 		opmode == Q7Pmode ||
	 		opmode == Q6Pmode ||
	 		/* for now maintain compatibility with old style pointers
	 		    -mtc 7/10/2006
	 		*/
	 		opmode == mode || opmode == SImode);
    
    if (REG_P(op))
	return ispointermode;
    else
	{
	return address_operand(XEXP (op, 0), opmode);
	}
  })


