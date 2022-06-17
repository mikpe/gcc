;; GCC machine description for Infineon xc16x
;; Copyright (C)  2006 
;; Free Software Foundation, Inc.

;; Contributed by Shrirang Khisti on behalf of KPIT Cummins Infosystems Ltd.

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING.  If not, write to
;; the Free Software Foundation, 51 Franklin Street, Fifth Floor,
;; Boston, MA 02110-1301, USA.


;; This is a machine description file for target xc16x Infineon chip.
;; File contains basically define_insn and define_expand patterns for rtx generation and code   
;; generation.

;; Print operand must support
;; X - For outputting 8 byte register operand
;; f - For generating regopno+1 reg specially for 32 bit operation
;; l - For printing Label directly specially in jump instructions.
;; c,d,C,D,Y : Specially used for large memory model

;; PSI :- Partial single integer : It is specially taken as a mode for pointers for large target
;; memory model. It is useful to distinguish pointer operations by adding PSI mode

;; Set the attribute "type"  for the instructions
;; Right now only two types are defined arithmatic and branch.
;; Default type for any instruction will be arith
(include "predicates.md")
(define_attr "type" "branch,arith"
	     (const_string "arith"))

;; Set attribute length for the instructions
;; Here for branch instructions according to target jump its size is decided

(define_attr "length" "" 
  (cond [(eq_attr "type" "branch")
  (if_then_else (and (ge (minus (match_dup 0) (pc))
	(const_int -80))
	(le (minus (match_dup 0) (pc))
	(const_int 80)))
	(const_int 2) (const_int 4))]
  (const_int 2)))

(define_insn "*large_premodify123"
  [(set (mem :QI(pre_modify:PSI 
	(match_operand:PSI 0 "register_operand" "r")
	(plus: PSI (match_operand:PSI 1 "register_operand" "0")
	(match_operand:PSI 2 "immediate_operand" "i"))))
	(match_operand:QI 3 "general_operand" "g"))]                
  ""
  "*
{
  output_asm_insn(\"add %T0,%2\",operands);
  output_asm_insn(\"mov [-r0],%f0\",operands);
  output_asm_insn(\"mov %f0,%3\",operands);
  output_asm_insn(\"mov [%T0],%f0\",operands);
  return \"mov %f0,[r0+]\";
}
"
  [(set_attr "length" "16")])

(define_insn "*premodify"
  [(set (mem :QI(pre_modify:HI 
	(match_operand:HI 0 "register_operand" "r")
	(plus: HI (match_operand:HI 1 "register_operand" "0")
	(match_operand:HI 2 "immediate_operand" "i"))))
	(match_operand:QI 3 "general_operand" "g"))]                
  ""
  "*
{
  output_asm_insn(\"add %T0,%2\",operands);
  output_asm_insn(\"mov [-r0],%f0\",operands);
  output_asm_insn(\"mov %f0,%3\",operands);
  output_asm_insn(\"mov [%T0],%f0\",operands);
  return \"mov %f0,[r0+]\";
}"
  [(set_attr "length" "16")])

(define_expand "insv"
  [(set (zero_extract:HI (match_operand:HI 0 "register_operand" "")
	(match_operand:HI 1 "immediate_operand" "")
	(match_operand:HI 2 "general_operand" ""))
	(match_operand:HI 3 "register_operand" ""))]
  ""
  "if (INTVAL(operands[1]) != 1)
   FAIL;
  "
   )
    
(define_insn "*insv_si_1_n"
  [(set (zero_extract:HI (match_operand:HI 0 "register_operand" "+r")
	(const_int 1)
	(match_operand:HI 1 "immediate_operand" "n"))
	(match_operand:HI 2 "register_operand" "r"))]
  ""
  "bmov %0.%s1,%2.0"
  [(set_attr "length" "4")])
	
;;QI mode move instruction

(define_expand "movqi"
  [(set (match_operand:QI 0 "general_operand_dst" "")
	(match_operand:QI 1 "general_operand_src" ""))]
  ""
  "
  "
   )

(define_insn "*movqi"
  [(set (match_operand:QI 0 "general_operand_dst" "=r,<,b,b,m")
	(match_operand:QI 1 "general_operand_src" "r>,b,i,m,b"))]
  ""
  "*
  {
    switch(which_alternative)
      {
        case 4: 
          if (GET_CODE(XEXP(operands[0],0)) == CONST_INT)
	    {
	      rtx p_no[2];
	      rtx xop[2];                                                                                 
	      xop[1]=operands[1];
	      xop[0]=XEXP(operands[0],0);
	      if (TARGET_LARGE)
	        {
	          if (INTVAL(XEXP(operands[0],0)) > 0xffff)
	     	    {
	     	      p_no[0]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[0],0))/0x4000);
	     	      xop[0]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[0],0))%0x4000);
	     	      output_asm_insn(\"extp %0,#0x1\",p_no);
	     	      output_asm_insn(\"movb %i0,%X1\",xop);
	     	      return \"\";
	            }	     		      		
	     	}
  	      output_asm_insn(\"movb %i0,%X1\",xop); 		     
              return \"\";
	    } 
 	  if (TARGET_LARGE)
	    {
	      if (GET_CODE(XEXP(operands[0],0)) == SYMBOL_REF || GET_CODE(XEXP(operands[0],0)) == CONST)
	        {
	          output_asm_insn(\"extp #pag: %X0,#0x1\",operands);
		  return \"movb pof:%X0,%X1\";
	        }
	      else
	        {
	          if (GET_CODE(XEXP(operands[0],0)) != PLUS&&REGNO(XEXP(operands[0],0)) != 0)
	            {
		      output_asm_insn(\"extp %d0,#0x1\",operands);
		    } 
		  return \"movb %X0,%X1\"; 
                }  
  	    }
	  return \"movb	%X0,%X1\"; 
	case 3:
	  if (GET_CODE(XEXP(operands[1],0)) == CONST_INT)
	    {
	      rtx xop[2];
	      rtx p_no[2];
	      xop[0]=operands[0];
	      xop[1]=XEXP(operands[1],0);
	      if (TARGET_LARGE)
	        {
	          if (INTVAL(XEXP(operands[1],0)) > 0xffff)
	     	    {
	     	      p_no[0]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[1],0))/0x4000);
	     	      xop[1]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[1],0))%0x4000);
	     	      output_asm_insn(\"extp %0,#0x1\",p_no);
	     	      output_asm_insn(\"movb %X0,%i1\",operands);
	     	      return \"\";
	            }	     		      		
	     	}				     	    
	      output_asm_insn(\"movb %X0,%i1\",xop);
	      return \"\";
	    }
	  if (TARGET_LARGE)
	    {    	      		     
	      if (GET_CODE(XEXP(operands[1],0)) == SYMBOL_REF || GET_CODE(XEXP(operands[1],0)) == CONST)
	        {   
		  output_asm_insn(\"extp #pag: %X1,#0x1\",operands);
		  return \"movb %X0, pof:%X1\";
		  return \"\";
		}
	      else
	        {
		  if (GET_CODE(XEXP(operands[1],0)) != PLUS&&REGNO(XEXP(operands[1],0)) != 0)
		    {
		      output_asm_insn(\"extp  %d1,#0x1\",operands);
		    }
		  return \"movb %X0,%X1\"; 
		}
	    }
	  else
	    return \"movb %X0,%X1\";
	case 2:
	  return \"movb %X0,%X1\";
 	case 0:
 	  if (TARGET_LARGE)
	    {
	      if (GET_CODE(operands[1]) != REG)
                {
		  if (REGNO(XEXP(operands[1],0)) > 1)
		    output_asm_insn(\"extp %f1,#0x1\",XEXP(operands[1],0));
                  
                  if (REGNO(operands[0])<8)
                    return \"movb %X0,%T1\";
		  
		  else
		    return \"mov %T0,%T1\";	
		}
	      return \"mov %T0,%T1\";
	    }
	  else
	    {
	      if (GET_CODE(operands[1]) != REG)
	        return \"movb %X0,%T1\"; 
 		
 	      else
 	        {
 		  return \"mov %T0,%T1\";
 		}
 	    }
        default:
          return \"movb %X0,%X1\";
      }
}"
  [(set_attr "length" "2,2,8,8,8")])

;;HI mode :- Half integer move instructions(16 bit movement)
(define_expand "movhi"
  [(set (match_operand:HI 0 "general_operand" "")
	(match_operand:HI 1 "general_operand" ""))]
  ""
  "if (!register_operand (operand1, HImode)
      && !register_operand (operand0, HImode))
     {
       operands[1] = copy_to_mode_reg (HImode, operand1);
     }
  "
)
		
(define_insn "*movhi"
  [(set (match_operand:HI 0 "general_operand_dst" "=r,<,r,r,m")
	(match_operand:HI 1 "general_operand_src" "r>,r,i,m,r"))]
  "TARGET_TINY||TARGET_SMALL||TARGET_LARGE"
  "*
  {
    return output_movhi_insn(operands);
  }"
[(set (attr "length") (symbol_ref "compute_mov_length (operands)"))])
	
(define_expand "movsi"
  [(set (match_operand:SI 0 "general_operand" "")
        (match_operand:SI 1 "general_operand" ""))]
  ""
  "if (!register_operand (operand1, SImode)
      && !register_operand (operand0, SImode))
     {
       operands[1] = copy_to_mode_reg (SImode, operand1);
     }
  "
)
 
(define_insn "*movsi"
  [(set (match_operand:SI 0 "general_operand_dst" "=r,<,r,r,m")
        (match_operand:SI 1 "general_operand" "r,r,i,m,r"))]
  ""
  "*
  {
    switch (which_alternative)
      {
        case 4: 
          if (TARGET_LARGE)
 	    {
 	      if (GET_CODE(XEXP(operands[0],0)) == SYMBOL_REF || GET_CODE(XEXP(operands[0],0)) == CONST)
 	        {
 		  output_asm_insn(\"extp #PAG:%e0,#0x2\",operands);
		  return \"mov pof:%f0,%f1\;mov    pof:%e0,%e1\";
 	        }
 	      else
	        {
		  if (GET_CODE(XEXP(operands[0],0)) != PLUS && REGNO(XEXP(operands[0],0)) != 0)
		    {
		      output_asm_insn(\"extp %d0,#0x2\",operands);
		    }
		  return \"mov %f0,%f1\;mov %e0,%e1\"; 
 	        }
 	    }
 	  else
 	    return \"mov %f0,%f1\;mov %e0,%e1\"; 
 	case 3:
          if (TARGET_LARGE)
 	    {
 	      if (GET_CODE(XEXP(operands[1],0)) == SYMBOL_REF || GET_CODE(XEXP(operands[1],0)) == CONST)
 	         {
                   output_asm_insn(\"extp #pag: %e1,#0x2\",operands);
 		   return \"mov %f0,pof:%f1\;mov    %e0,pof:%e1\";
 		 }
 	      else
	        {
	          if (GET_CODE(XEXP(operands[1],0)) != PLUS && REGNO(XEXP(operands[1],0)) != 0)
		     {
		       output_asm_insn(\"extp %d1,#0x2\",operands);
		     }
		  return \"mov %f0,%f1\;mov %e0,%e1\"; 
 	        }
 	    }
 	  else
 	    {	
 	      if (GET_CODE(XEXP(operands[1],0)) == PLUS)
	        {
		  if (REGNO(XEXP(XEXP(operands[1],0),0)) == (REGNO(operands[0]) + 1))
		    return \"mov   %e0,%e1\;mov   %f0,%f1\";
		}
	      return \"mov	 %f0,%f1\;mov	%e0,%e1\";
 	    }
 	case 1:
 	  return \"mov %e0,%f1\;mov %e0,%e1\";
 	case 0:
 	  {
 	    if (REGNO(operands[0]) != (REGNO(operands[1]) + 1))
 	      return \"mov %e0,%e1\;mov %f0,%f1\";
 	    else
 	      return \"mov %f0,%f1\;mov %e0,%e1\";
 	  }
      }
    if (which_alternative == 2)
      {
        if (GET_CODE(operands[1]) == SYMBOL_REF&&TARGET_LARGE)
	  {
	    output_asm_insn (\"mov %f0,#PAG:%e1\",operands);
	    output_asm_insn (\"mov %e0,#POF:%e1\",operands);
	    return \"\";        
	  }
	else
	  { 
	    output_asm_insn (\"mov %f0,%e1\",operands);
	    output_asm_insn (\"mov %e0,%f1\",operands);
	    return \"\";        
 	  } 
      }
}"
  [(set (attr "length") (symbol_ref "compute_mov_length (operands)"))]
)	
 
;; Single float 32 bit move instructions
 
(define_insn "movsf"
  [(set (match_operand:SF 0 "general_operand_dst" "=r,<,r,r,m")
        (match_operand:SF 1 "general_operand" "r,r,i,m,r"))]
  ""
  "*
  {
    switch (which_alternative)
      {
        case 4: 
          if (GET_CODE(XEXP(operands[0],0)) == CONST_INT)
	    {
	      rtx p_no[2];
	      rtx xop[2];                                                                                 
	      xop[1]=operands[1];
	      xop[0]=XEXP(operands[0],0);
	      if (TARGET_LARGE)
	        {
		  if (INTVAL(XEXP(operands[0],0)) > 0xffff)
		    {
		      p_no[0]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[0],0))/0x4000);
		      xop[0]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[0],0))%0x4000);
		      operands[0]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[0],0))%0x4000+2);
		      output_asm_insn(\"extp %0,#0x2\",p_no);
		      output_asm_insn(\"mov %i0,%e1\",xop);
		      output_asm_insn(\"mov %i0,%f1\",operands);
		      return \"\";
		    }	     		      		
		}
		operands[0]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[0],0))+2);
		output_asm_insn(\"mov %i0,%e1\",xop); 		     
		output_asm_insn(\"mov %i0,%f1\",operands);
		return \"\";
	    }
  	  if (TARGET_LARGE)
	    {
	      if (GET_CODE(XEXP(operands[0],0)) == SYMBOL_REF || GET_CODE(XEXP(operands[0],0)) == CONST)
	        {
		  output_asm_insn(\"extp #pag: %e0,#0x2\",operands);
		  return \"mov pof:%f0,%f1\;mov    pof:%e0,%e1\";
		}
	      else
	       	{
	          if (GET_CODE(XEXP(operands[0],0)) != PLUS)
		    {
		      output_asm_insn(\"extp %d0,#0x2\",operands);
		    }
		  return \"mov %f0,%f1\;mov %e0,%e1\"; 
 	       	}		   
 	    }
  	  else
  	    return \"mov %f0,%f1\;mov %e0,%e1\"; 
  	case 3:
	  if (TARGET_LARGE)
 	    {
       	      if (GET_CODE(XEXP(operands[1],0)) == CONST_INT)
	        {
	          rtx xop[2];
		  rtx p_no[2];
		  xop[0]=operands[0];
		  xop[1]=XEXP(operands[1],0);
		  if (TARGET_LARGE)
		    {
		      if (INTVAL(XEXP(operands[1],0)) > 0xffff)
		        {
		    	  p_no[0]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[1],0))/0x4000);
		   	  xop[1]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[1],0))%0x4000);
		 	  operands[1]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[1],0))%0x4000+2);
	   		  output_asm_insn(\"extp %0,#0x1\",p_no);
	        	  output_asm_insn(\"mov %e0,%i1\",xop);
	        	  output_asm_insn(\"mov %f0,%i1\",operands);
			  return \"\";
		        }
		    }
		  operands[1]=gen_rtx_CONST_INT(HImode,INTVAL(XEXP(operands[1],0))%0x4000+2);
		  output_asm_insn(\"mov %e0,%i1\",xop);
		  output_asm_insn(\"mov %f0,%i1\",operands);
		  return \"\";
	        } 
 	      if (GET_CODE(XEXP(operands[1],0)) == SYMBOL_REF || GET_CODE(XEXP(operands[1],0)) == CONST)
 	        {
 	          output_asm_insn(\"extp #pag: %e1,#0x2\",operands);
		  return \" mov %f0,pof:%f1\;mov %e0,pof:%e1\";
 	        }
 	      else
	        {
	 	  if (GET_CODE(XEXP(operands[1],0)) != PLUS)
		    {
		      output_asm_insn(\"extp %d1,#0x2\",operands);
		    }
	          return \"mov %f0,%f1\;mov %e0,%e1\"; 
	        }
 	    }
 	  else
	    {	
  	      if (GET_CODE(XEXP(operands[1],0)) == PLUS)
                {
              	  if (REGNO(XEXP(XEXP(operands[1],0),0)) == (REGNO(operands[0]) + 1)) 
                    return \"mov %e0,%e1\;mov %f0,%f1\";
                }
	      return \"mov %f0,%f1\;mov	%e0,%e1\";
  	    }
  	case 1:
  	  return \"mov %e0,%f1\;mov %e0,%e1\";
  	case 0:
  	  {
	    if (REGNO(operands[0]) != (REGNO(operands[1]) + 1))
	      return \"mov %e0,%e1\;mov %f0,%f1\";
	    else
	      return \"mov %f0,%f1\;mov %e0,%e1\";
 	  }
      }
    if (which_alternative == 2)
      {
        output_asm_insn (\"mov %f0,%e1\",operands);
 	output_asm_insn (\"mov %e0,%f1\",operands);
 	return \"\";        
      }
}"
  [(set (attr "length") (symbol_ref "compute_mov_length (operands)"))]
)	
 
 ;;********************For pointer mode specially for Large Target*******
 
(define_insn "movpsi"
  [(set (match_operand:PSI 0 "general_operand_dst" "=r,r,m,r,<")
 	(match_operand:PSI 1 "general_operand" "r,i,r,m,r"))]
  "TARGET_LARGE"
  "* return output_movpsi_insn(operands);" 
  
[(set_attr "length" "8,8,8,8,8")])
  
(define_insn "addpsi3"
  [(set (match_operand:PSI 0 "general_operand" "=r,r")
	(plus:PSI (match_operand:PSI 1 "general_operand" "%0,0")
	   	  (match_operand:PSI 2 "general_operand" "r,n")))]
  "TARGET_LARGE"
  "* return output_addpsi_insn(insn,operands,0);"
)
  
(define_insn "cmppsi"
  [(set (cc0)
        (compare (match_operand:PSI 0 "register_operand" "r,r")
                 (match_operand:PSI 1 "general_operand" "r,n")))]
  "TARGET_LARGE"
  "* return output_cmppsi_insn(operands);"
		 
)
		 	
(define_insn "truncsipsi2"
  [(set (match_operand:PSI 0 "register_operand" "=r")
        (truncate:PSI (match_operand:SI 1 "general_operand" "ri")))]
  "TARGET_LARGE"
  "* 
{
  if (REGNO(operands[0]) != (REGNO(operands[1]) + 1))
    return \"mov %e0,%e1\;mov %f0,%f1\";
  else
    return \"mov %f0,%f1\;mov %e0,%e1\";
}")
        
(define_insn "extendpsisi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (sign_extend:SI (match_operand:PSI 1 "register_operand" "ri")))]
  "TARGET_LARGE"
  "*
{
  output_asm_insn(\"mov %e0,%e1\",operands);
  return \"mov %f0,%f1\";
}"
  )
 	
;;Call value RTL in PSI mode
 	
(define_insn ""
  [(set (match_operand  0 "" "=r")
        (call (match_operand:PSI 1 "general_operand" "ro")
        (match_operand:PSI  2 "general_operand" "g"))) 
  ]
  "TARGET_LARGE"
  "* return output_call_insn(operands);"

[(set (attr "length") (symbol_ref "compute_call_length (operands,1)"))])

(define_insn ""
  [(call (match_operand:PSI 0 "general_operand" "or")
   	 (match_operand:PSI 1 "general_operand" "g"))]
  "TARGET_LARGE"
  "* return output_call_insn1(operands);"
  [(set (attr "length") (symbol_ref "compute_call_length (operands,0)"))]
   )
 
(define_insn "ashrpsi3"
  [(set (match_operand:PSI 0 "register_operand" "=r")
  (ashiftrt:PSI (match_operand:PSI 1 "register_operand" "0")
  	        (match_operand:PSI 2 "general_operand" "rn")))]
  "TARGET_LARGE"
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT 
     && INTVAL (operands[2]) >= BITS_PER_WORD)
    return (\"mov %0,#0     \");
    return (\"ashr %0,%2\");
}"
  [(set_attr "length" "2")])
 
(define_insn "ashlpsi3"
  [(set (match_operand:PSI 0 "register_operand" "=r")
       	(ashift:PSI (match_operand:PSI 1 "register_operand" "0")
      		    (match_operand:PSI 2 "general_operand" "rn")))]
  "TARGET_LARGE"
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT 
     && INTVAL (operands[2]) >= BITS_PER_WORD)
  return (\"mov %0,#0    \");
  output_asm_insn(\"shl %0,#0x1\",operands);
  return (\"shl %f0,%2\");
}"
  [(set_attr "length" "2")])
    
(define_insn "subpsi3"
  [(set (match_operand:PSI 0 "general_operand" "=r,r")
 	(minus:PSI (match_operand:PSI 1 "general_operand" "%0,0")
        (match_operand:PSI 2 "general_operand" "r,n")))]
  "TARGET_LARGE"
  "* return output_addpsi_insn(insn,operands,1);"
  )
  
 ;;***************************************************************************
 ;;--------------------------------------------------------------------------------------------------
 ;;Following section contains instructions regarding truncate and extend
 ;;Truncate instructions

(define_insn "truncsihi2"
  [(set (match_operand:HI 0 "register_operand" "=r")
 	(truncate:HI (match_operand:SI 1 "register_operand" "ri")))]
  ""
  "mov %0,%1"
  [(set_attr "length" "2")])
 
(define_insn "trunchiqi2"
  [(set (match_operand:QI 0 "register_operand" "=r")
  	(truncate:QI (match_operand:HI 1 "register_operand" "ri")))]
  ""
  "*
{
  if (GET_CODE(operands[1]) == REG&&REGNO(operands[1]) == REGNO(operands[0]))
    return \"\";    
  
  if (REGNO(operands[0]) < 8 && REGNO(operands[1]) < 8)
    return \"movb %X0,%X1\";
  
  else
    return \"mov %T0,%T1\";
}"
  [(set_attr "length" "2")])

 ;;Following are extend instructions
(define_insn "extendqihi2"
  [(set (match_operand:HI 0 "register_operand" "=r")
 	(sign_extend:HI (match_operand:QI 1 "register_operand" "b")))]
  ""
  "movbs %0,%1"
  [(set_attr "length" "2")])
 
(define_insn "zero_extendqihi2"
  [(set (match_operand:HI 0 "register_operand" "=r")
 	(zero_extend:HI (match_operand:QI 1 "register_operand" "b")))]
  ""
  "movbz %0,%1"
  [(set_attr "length" "2")])
		
;;---------------------------------------------------------------------------------------------------
;;Following Section contains patterns for addition instructions
;;It will be in three modes
;;SImode : 32 bit addition
;;HImode : 16 bit addition
;;QImode : 8 bit addition

 ;;  ** QI mode adition : Add two byte operands. (8 bit addition) :
   
(define_expand "addqi3"
  [(set (match_operand:QI 0 "general_operand_src_mem" "")
      	(plus:QI (match_operand:QI 1 "general_operand_src_mem" "")
   		 (match_operand:QI 2 "general_operand_dst_mem" "")))]
  ""
  "
{
  if (GET_CODE (operands[0]) == MEM && (GET_CODE (XEXP (operands[0], 0)) == SYMBOL_REF))
    {
      if (GET_CODE(operands[2]) == CONST_INT)
      
      if (INTVAL(operands[2]) != 1 && INTVAL(operands[2]) != -1)
        operands[2] = copy_to_mode_reg (QImode, operand2);
    }
}
"
  )
   
(define_insn "*addqi3"
  [(set (match_operand:QI 0 "general_operand_src_mem" "=b,b,b,S")
       	(plus:QI (match_operand:QI 1 "general_operand_src_mem" "%0,0,0,0")
     		 (match_operand:QI 2 "general_operand_dst_mem" "b,n,S,i")))]
  ""
  "*
{
  if (which_alternative < 2)
    {
      output_asm_insn (\"addb %X0,%X2\", operands); 
      return \"\";
    }
  else if (which_alternative == 2)
    {
      if (TARGET_LARGE)
        {
          output_asm_insn (\"extp #PAG:%2,#0x1\",operands);
          return \"addb %X0,POf:%2\";  
        }
      else
        output_asm_insn (\"addb %X0,%X2\", operands); 
    }
  else 
    {
      if (TARGET_LARGE)
        {
          if (INTVAL(operands[2]) == 1)
            {
              return \"subb POF:%0,ONES\";
              output_asm_insn (\"extp #PAG:%0,#0x1\",operands);
            }
          else if (INTVAL(operands[2]) == -1)
            {
              output_asm_insn (\"extp #PAG:%0,#0x1\",operands);
              return \"addb POF:%0,ONES\";
            } 
          else
            return \"\";
        }
      else
        {
          if (INTVAL(operands[2]) == 1)
            return \"subb %0,ONES\";
          
          else if (INTVAL(operands[2]) == -1)
            return \"addb %0,ONES\";
          
          else
            return \"\";
        }
    }
}"
  [(set (attr "length") (symbol_ref "compute_add_length (operands)"))]
   )
   
;;Operand constraint X : It is related to substituting small register names like r4l, r4h etc.

;; Right now no attributes are used 
 
;;   ** HI mode addition : Add two word operands (16 bit addition)
   
;;In the following case last operand is regarding instruction in which 0th operand is not matched 
;;either with 1st or 2nd operand so we need to move that operand in operand 0 first and then perform addition
   
(define_expand "addhi3"
  [(set (match_operand:HI 0 "general_operand_src_mem" "")
       	(plus:HI (match_operand:HI 1 "general_operand_src_mem" "")
     		 (match_operand:HI 2 "general_operand_dst_mem" "")))]
  ""
  "
{
  if (GET_CODE (operands[0]) == MEM && (GET_CODE (XEXP (operands[0], 0)) == SYMBOL_REF))
    {
      if (GET_CODE(operands[2]) == CONST_INT)
      
      if (INTVAL(operands[2]) != 1 && INTVAL(operands[2]) != -1)
        operands[2] = copy_to_mode_reg (HImode, operand2);
    }
} 
  "
  )
	
(define_insn "*addhi3"
  [(set (match_operand:HI 0 "general_operand_src_mem" "=r,r,r,S,S")
       	(plus:HI (match_operand:HI 1 "general_operand_src_mem" "%0,0,0,0,0")
		 (match_operand:HI 2 "general_operand_dst_mem" "r,n,S,i,r")))]
  ""
  "*
{
  int temp;
  if (which_alternative == 4)
    {
      if (TARGET_LARGE)
        {
          output_asm_insn (\"extp #PAG:%0,#0x1\",operands);
          return \"add POF:%0,%2\";  
        }
      else
        return \"add %0,%2\"; 
    }
  if (which_alternative < 2)
    {
      output_asm_insn (\"add %0,%2\", operands); 
      return \"\";
    }
  else if (which_alternative == 2)
    {
      if (TARGET_LARGE)
        {
          output_asm_insn (\"extp #PAG:%2,#0x1\",operands);
          return \"add %0,POf:%2\";  
        }
      else
        return \"add %0,%2\"; 
    }
  else if (which_alternative == 3)
    {
      if (TARGET_LARGE)
        {
     	  if (INTVAL(operands[2]) == 1)
     	    {
     	      /* if(!MEM_SCALAR_P(operands[1])) */
     	      output_asm_insn (\"extp #PAG:%0,#0x1\",operands);
     	      return \"sub POF:%0,ONES\";
            }
          else if (INTVAL(operands[2]) == -1)
            {
              output_asm_insn (\"extp #PAG:%0,#0x1\",operands);
              return \"add POF:%0,ONES\";
            }
          else
            return \"\";
     	}
      else
        {
     	  if (INTVAL(operands[2]) == 1)
     	  return \"sub %0,ONES\";
     	  
     	  else if (INTVAL(operands[2]) == -1)
     	  return \"add %0,ONES\";
     	  
     	  else
     	  return \"\";
        }
    }
}
"
  [(set (attr "length") (symbol_ref "compute_add_length (operands)"))])
  
;;  ** SI mode addition : Add two double word operands (32 bit addition)
   
;; Here constraint "f" is used which is very useful in outputting higher 16 bits addition
;; It should increment register no by one and use new register no in the addition
(define_insn "addsi3"
  [(set (match_operand:SI 0 "general_operand_src_mem" "=r,r,r,S")
      	(plus:SI (match_operand:SI 1 "general_operand_dst_mem" "%0,0,0,0")
   		 (match_operand:SI 2 "general_operand_dst_mem" "r,n,S,r")))]
  ""
  "*
{
  if (which_alternative == 0)  
    {
      output_asm_insn (\"add %e0,%e2\",operands);
      return \"addc %f0,%f2\";
    }
  else if (which_alternative == 1)                      
    {
      rtx xop[3];
      xop[0]=operands[0];
      xop[1]=gen_rtx_CONST_INT( VOIDmode, INTVAL (operands[2]) >> 16) ;                                          
      xop[2] = gen_rtx_CONST_INT ( VOIDmode, INTVAL (operands[2]) & 0xffff);
      output_asm_insn (\"add %e0,%2\",xop);
      output_asm_insn (\"addc %f0,%1\",xop);
      return \"\";    
    }
  else if (which_alternative == 2)
    {
      if (TARGET_LARGE)
        {
          output_asm_insn(\"extp #PAG:%2,#0x2\",operands);
          output_asm_insn(\"add %e0,POF:%e2\",operands);
          return \"addc %f0,POF:%f2\";
        }
      else
        {
          output_asm_insn(\"add %e0,%e2\",operands);
          return \"addc %f0,%f2\";
        }
    }
  else if (which_alternative == 3)
    {
      if (TARGET_LARGE)
        {
          output_asm_insn(\"extp #PAG:%0,#0x2\",operands);
	  output_asm_insn(\"add POF:%e0,%e2\",operands);
	  return \"addc POF:%f0,%f2\";
        }
      else
        {
          output_asm_insn(\"add %e0,%e2\",operands);
	  return \"addc %f0,%f2\";
        }
    }
}"
  [(set (attr "length") (symbol_ref "compute_add_length (operands)"))])
   
;;-------------------------------------------------------------------------------------------------
;;Following Section contains patterns for subtraction instructions
;;It will be in three modes
;;SImode : 32 bit subtraction
;;HImode : 16 bit subtraction
;;QImode : 8 bit subtraction
  
;;QI mode subtraction
(define_insn "subqi3"
  [(set (match_operand:QI 0 "register_operand" "=b,b")
    	(minus:QI (match_operand:QI 1 "register_operand" "0,0")
       		  (match_operand:QI 2 "general_operand_dst_mem" "b,n")))]
  ""
  "*
{
  switch (which_alternative)
    {
      case 0:
        return \"subb %X0,%X2\";
      case 1:
        return \"subb %X0,%2\";
      case 2:
        if (TARGET_LARGE)
          {
            output_asm_insn(\"extp #PAG:%2,#0x1\",operands);
	    return \"subb %X0 , POF:%2\";
          }
        else
          return \"subb %X0,%X2\";
    }
}"
  [(set (attr "length") (symbol_ref "compute_add_length (operands)"))]
  )
   
;;HI mode subtraction
   
(define_insn "subhi3"
  [(set (match_operand:HI 0 "register_operand" "=r,r,r,S")
       	(minus:HI (match_operand:HI 1 "register_operand" "0,0,0,0")
       		  (match_operand:HI 2 "general_operand_dst_mem" "r,n,S,r")))]
  ""
  "*
{
  if (which_alternative == 3)
    {
      if (TARGET_LARGE) 
        {
	  output_asm_insn(\"extp #PAG:%0,#0x1\",operands);
	  return \"sub POF:%0 , %2\";
	}
      else
        return \"sub %0 , %2\";
    }
  else if (which_alternative < 2)
    return \"sub %0 , %2\";
  else
    {
      if (TARGET_LARGE) 
        {
          output_asm_insn(\"extp #PAG:%2,#0x1\",operands);
          return \"sub %0 , POF:%2\";
        }
      else
        return \"sub %0 , %2\";
    }
}"
  [(set (attr "length") (symbol_ref "compute_add_length (operands)"))]
  )
     
;; SI mode subtraction
(define_insn "subsi3"
  [(set (match_operand:SI 0 "general_operand_src_mem" "=r,r,r,S")
       	(minus:SI (match_operand:SI 1 "general_operand_src_mem" "0,0,0,0")
       		  (match_operand:SI 2 "general_operand_dst_mem" "r,n,S,r")))]
  ""
  "*
{
  if (which_alternative == 0)
    {
      output_asm_insn (\"sub %e0,%e2\",operands);
      output_asm_insn (\"subc %f0,%f2\",operands);  
      return \"\";   
    }
  else if (which_alternative == 1)
    {
      rtx xop[3];
      xop[0]  = operands[0];
      xop[1] = gen_rtx_CONST_INT (VOIDmode, INTVAL (operands[2]) >> 16);
      xop[2] = gen_rtx_CONST_INT ( VOIDmode, INTVAL (operands[2]) & 0xffff);       
      output_asm_insn (\"sub  %e0 , %2 \",operands);
      output_asm_insn (\"subc  %f0 , %1\",operands);
      return \"\";
    } 
  else if (which_alternative == 2)
    {
      if (TARGET_LARGE)
        {
          output_asm_insn(\"extp #PAG:%2,#0x2\",operands);
          output_asm_insn(\"sub %e0,POF:%e2\",operands);
          return \"subc %f0,POF:%f2\";
        }
      else
        {
          output_asm_insn(\"sub %e0,%e2\",operands);
          return \"subc %f0,%f2\";
        } 
    }
  else if (which_alternative == 3)
    {
      if (TARGET_LARGE)
        {
          output_asm_insn(\"extp #PAG:%0,#0x2\",operands);
       	  output_asm_insn(\"sub POF:%e0,%e2\",operands);
       	  return \"subc POF:%f0,%f2\";
        }
      else
        {
          output_asm_insn(\"sub %e0,%e2\",operands);
       	  return \"subc %f0,%f2\";
        }
    }
}
"
  [(set (attr "length") (symbol_ref "compute_add_length (operands)"))])
     
;;---------------------------------------------------------------------------------------------------   

;; Folowing section generates multiply instructions , in

;; HI mode : Word multiply
;; SI mode : Double word multiply  
 
;; HI mode multiplication
(define_insn "umulqihi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
  	(mult:HI (zero_extend:HI (match_operand:QI 1 "register_operand" "%0"))
  		 (zero_extend:HI (match_operand:QI 2 "register_operand" "r"))))]
  ""
  "*
{
  output_asm_insn (\"movbz %T2,%X2\", operands);
  output_asm_insn (\"movbz %T1,%X1\", operands);
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"mulu %T1,%T2\", operands);
  output_asm_insn (\"mov %T0,MDL\", operands);
  return \"\"; 
  }"
  [(set_attr "length" "12")])
  
(define_insn "mulpsi3"
  [(set (match_operand:PSI 0 "register_operand" "=r")
        (mult :PSI (match_operand:PSI 1 "register_operand" "%0")
                   (match_operand:PSI 2 "register_operand" "r")
      )
  )]
  "TARGET_LARGE"
  "*
{
  return \"\";
}"  
  )
(define_insn "mulhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
  	(mult:HI (match_operand:HI 1 "register_operand" "%0")
  		 (match_operand:HI 2 "register_operand" "r")))]
  ""
  "*
{
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"mul %1,%2\", operands);
  output_asm_insn (\"mov %0,MDL\", operands);
  return \"\";
}"
  [(set_attr "length" "8")])
  
(define_insn "mulqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
    	(mult:QI (match_operand:QI 1 "register_operand" "%0")
    		 (match_operand:QI 2 "register_operand" "r")))]
  ""
  "*
{
  output_asm_insn (\"movbs %T2,%X2\", operands);
  output_asm_insn (\"movbs %T1,%X1\", operands);
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"mul %T1,%T2\", operands);
  output_asm_insn (\"mov %T0,MDL\", operands);
  return \"\";
}" 
  [(set_attr "length" "12")])
   
;;SI mode multiplication
   
(define_insn "mulsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
     	(mult:SI (match_operand:SI 1 "register_operand" "%0")
  		 (match_operand:SI 2 "register_operand" "r")))]
  ""
  "*
{
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"mulu %f0,%e2\",operands);
  output_asm_insn (\"mov %f0,MDL\",operands);
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"mulu %f2,%e0\",operands);
  output_asm_insn (\"add %f0,MDL\",operands);
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"mulu %e0,%e2\",operands);
  output_asm_insn (\"add %f0,MDH\",operands);
  output_asm_insn (\"mov %e0,MDL\",operands);
  return \"\";
}" 
  [(set_attr "length" "28")])
  
;;--------------------------------------------------------------------------------------------------
;;Following section generates division and modulo instruction in
;;HI mode : That is 16 bit  by 16 bit division is carried out
;;Quotient is stored in register MDL and remainder is stored in register MDH
  
;;Signed 16 bit division
 
(define_insn "divhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
  	(div:HI (match_operand:HI 1 "register_operand" "r")
  		(match_operand:HI 2 "register_operand" "r")))]
  ""
  "*
{
  output_asm_insn (\"mov MDL,%1\", operands);
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"div %2\", operands);
  output_asm_insn (\"mov %0,MDL\", operands);
  return \"\";
}" 
  [(set_attr "length" "10")])
  
;;Unsigned 16 bit division
  
(define_insn "udivhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
  	(udiv:HI (match_operand:HI 1 "register_operand" "r")
  		 (match_operand:HI 2 "register_operand" "r")))]
  ""
  "*
{
  output_asm_insn (\"mov MDL,%1\", operands);
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"divu %2\", operands);
  output_asm_insn (\"mov %0,MDL\", operands);
  return \"\";
}" 
  [(set_attr "length" "10")])
 
(define_insn "modhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
  	(mod:HI (match_operand:HI 1 "register_operand" "r")
  		(match_operand:HI 2 "register_operand" "r")))]
  ""
  "*
{
  output_asm_insn (\"mov MDL,%1\", operands);
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"div %2\", operands);
  output_asm_insn (\"mov %0,MDH\", operands);
  return \"\";
}" 
  [(set_attr "length" "10")])
  
(define_insn "umodhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
  	(umod:HI (match_operand:HI 1 "register_operand" "r")
  		 (match_operand:HI 2 "register_operand" "r")))]
  ""
  "*
{
  output_asm_insn (\"mov MDL,%1\", operands);
  output_asm_insn (\"atomic #0x1\", operands);
  output_asm_insn (\"divu %2\", operands);
  output_asm_insn (\"mov %0,MDH\", operands);
  return \"\";
}" 
  [(set_attr "length" "10")])

;;---------------------------------------------------------------------------------------------------

;; Following section generates assebly instructions regarding logical operations 
;; such as negation , complement ,and , or and xor ,negation ,complement etc.
 
;; Right now support to bclr and bset instructions is  added through and/or instructions  
;;And instruction in 
;; QI mode : byte anding
;; HI mode : Word anding
;; SI mode : Double word anding

;;QI mode and instruction

(define_insn "andqi3"
  [(set (match_operand:QI 0 "register_operand" "=b,b")
	(and:QI (match_operand:QI 1 "register_operand" "%0,0")
		(match_operand:QI 2 "general_operand" "b,n")))]
  ""
  "* return op_and_insn(operands);"
  [(set_attr "length" "2,4")])

;;HI mode and instruction

(define_insn "andhi3"
  [(set (match_operand:HI 0 "general_operand" "=r,r")
	(and:HI (match_operand:HI 1 "general_operand" "%0,0")
		(match_operand:HI 2 "general_operand" "r,n")))]
  ""
  "
  and %0,%2
  "
  [(set_attr "length" "2,4")]) 
  
 ;; SI mode and instruction 
 
(define_insn "andsi3"
  [(set (match_operand:SI 0 "general_operand" "=r,r")
        (and:SI (match_operand:SI 1 "general_operand" "%0,0")
   		(match_operand:SI 2 "general_operand" "r,n")))]
  ""
  "*
{
  if (REG_P (operands[2]))
    {
      output_asm_insn (\"and %e0,%e2\",operands);
      output_asm_insn (\"and %f0,%f2\",operands);
    }
  else
    {
      rtx xop[3];
      xop[0]  = operands[0];
      xop[1] = gen_rtx_CONST_INT ( VOIDmode,INTVAL (operands[2]) >> 16);
      xop[2] = gen_rtx_CONST_INT ( VOIDmode, INTVAL (operands[2]) & 0xffff);
      output_asm_insn (\"and %e0,%2\",xop);
      output_asm_insn (\"and %f0,%1\",xop);
    }
  return \"\";
}"
  [(set_attr "length" "4,8")])
   
  ;; Following section generates OR instructions in
  ;; QI mode : byte oring
  ;; HI mode : Word oring
  ;; SI mode : Double word oring

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
        (ior:SI (match_operand:SI 1 "register_operand" "%0,0")
  	        (match_operand:SI 2 "general_operand" "r,n")))]
  ""
  "*
{
  CC_STATUS_INIT;
  if (REG_P (operands[2]))
    {
      output_asm_insn (\"or %e0,%e2\",operands);
      output_asm_insn (\"or %f0,%f2\",operands);
    }
  else
    {
      rtx xop[3];
      xop[0]  = operands[0];
      xop[1] = gen_rtx_CONST_INT ( VOIDmode, INTVAL (operands[2]) >> 16);
      xop[2] = gen_rtx_CONST_INT ( VOIDmode, INTVAL (operands[2]) & 0xffff);
      output_asm_insn (\"or %e0,%2\",xop);
      output_asm_insn (\"or %f0,%1\",xop);
    }
  return \"\";
}"
  [(set_attr "length" "4,8")])
  
(define_insn "iorhi3"
  [(set (match_operand:HI 0 "general_operand" "=r,r")
  	(ior:HI (match_operand:HI 1 "general_operand" "%0,0")
  	        (match_operand:HI 2 "general_operand" "r,n")))]
  ""
  "or %0,%2"
  [(set_attr "length" "2,4")])
  
(define_insn "iorqi3"
  [(set (match_operand:QI 0 "register_operand" "=b,b")
  	(ior:QI (match_operand:QI 1 "register_operand" "%0,0")
  		(match_operand:QI 2 "general_operand" "b,n")))]
  ""
  "* return op_or_insn(operands);"
  [(set_attr "length" "2,4")])
    
;; Following section generates XOR instructions in
;; QI mode : byte xoring
;; HI mode : Word xoring
;; SI mode : Double word xoring
  
(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
  	(xor:SI (match_operand:SI 1 "register_operand" "%0,0")
  		(match_operand:SI 2 "general_operand" "r,n")))]
  ""
  "*
{
  if (REG_P (operands[2]))
    {
      output_asm_insn (\"xor %e0,%e2\",operands);
      output_asm_insn (\"xor %f0,%f2\",operands);
    }
  else
    {
      rtx xop[3];
      xop[0]  = operands[0];
      xop[1] = gen_rtx_CONST_INT ( VOIDmode, INTVAL (operands[2]) >> 16);
      xop[2] = gen_rtx_CONST_INT ( VOIDmode, INTVAL (operands[2]) & 0xffff);
      output_asm_insn (\"xor %e0,%2\",xop);
      output_asm_insn (\"xor %f0,%1\",xop);
    }
  return \"\";
}"
  [(set_attr "length" "4,8")])
  
(define_insn "xorhi3"
  [(set (match_operand:HI 0 "general_operand" "=r,r")
  	(xor:HI (match_operand:HI 1 "general_operand" "%0,0")
  		(match_operand:HI 2 "general_operand" "r,n")))]
  ""
  "xor %0,%2"
  [(set_attr "length" "2,4")])
  
(define_insn "xorqi3"
  [(set (match_operand:QI 0 "register_operand" "=b,b")
  	(xor:QI (match_operand:QI 1 "register_operand" "%0,0")
  		(match_operand:QI 2 "general_operand" "b,b")))]
  ""
  "xorb %X0,%X2"
  [(set_attr "length" "2,4")])
  
;;Following section generates negation instructions in
;;HI mode : word negation
;;QI mode : byte negation
;;SI mode : double word negation
  
;;HI mode negation
  
(define_insn "neghi2"
  [(set (match_operand:HI 0 "register_operand" "=r")
  	(neg:HI (match_operand:HI 1 "register_operand" "0")))]
  ""
  "neg %0"
  [(set_attr "length" "2")])
  
;;QI mode negation
 
(define_insn "negqi2"
  [(set (match_operand:QI 0 "register_operand" "=r")
  	(neg:QI (match_operand:QI 1 "register_operand" "0")))]
  ""
  "negb %X0"
  [(set_attr "length" "2")])
  
;;SI mode negation
  
(define_insn "negsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
    	(neg:SI (match_operand:SI 1 "register_operand" "0")))]
  ""
  "*
{
  output_asm_insn (\"neg %0\",operands);
  output_asm_insn (\"cpl %f0\",operands);
  return \"addc %f0,#0\";
}
"
  [(set_attr "length" "8")])
  
;;sf mode negation
  
(define_insn "negsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
  	(neg:SF (match_operand:SF 1 "register_operand" "0")))]
  ""
  "bmovn %f0.15,%f0.15"		
  [(set_attr "length" "4")])
  
;;Following section generates 1's Complement instruction in
;;QI mode : byte complement
;;HI mode : word complement
;;SI mode : double word complement
  
;;SI mode complement
  
(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
  	(not:SI (match_operand:SI 1 "register_operand" "0")))]
  ""
  "*
{
  output_asm_insn (\"cpl %0\",operands);
  output_asm_insn (\"cpl %f0\",operands);
  return \"\";
}"
  [(set_attr "length" "4")])
  
;;HI mode complement
  
(define_insn "one_cmplhi2"
  [(set (match_operand:HI 0 "register_operand" "=r")
  	(not:HI (match_operand:HI 1 "register_operand" "0")))]
  ""
  "cpl %0"
  [(set_attr "length" "2")])
  
;;QI mode complement
  
(define_insn "one_cmplqi2"
  [(set (match_operand:QI 0 "register_operand" "=r")
  	(not:QI (match_operand:QI 1 "register_operand" "0")))]
  ""
  "cplb %X0"
  [(set_attr "length" "2")])
   
;; Following section generates instruction regarding absolute
   
(define_insn "abssf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
   	(abs:SF (match_operand:SF 1 "register_operand" "0")))]
  ""
  "bclr %f0.15"
  [(set_attr "length" "2")])
    
;;--------------------------------------------------------------------------------------------
;;Following section generates  shifting instructions like
;; Arithmatic shift left
;; Shift left/right in
;; HImode -  That is for word operand
   
;;Arithmatic shift left 
    
(define_insn "ashlhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
    	(ashift:HI (match_operand:HI 1 "register_operand" "0")
    		   (match_operand:HI 2 "general_operand" "rn")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT && INTVAL (operands[2]) >= BITS_PER_WORD)
    return (\"mov %0,#0    \");
    
  if (INTVAL (operands[2]) < 0)
    return (\"mov %0,#0    \");  
    
  return (\"shl %0,%2;ashl insn\");
}"
  [(set_attr "length" "2")])
    
;;Arithmatic shift right in HImode
    
(define_insn "ashrhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
    	(ashiftrt:HI (match_operand:HI 1 "register_operand" "0")
    		     (match_operand:HI 2 "general_operand" "rn")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT && INTVAL (operands[2]) >= BITS_PER_WORD)
    return (\"mov %0,#0     \");
    
  if(INTVAL (operands[2]) < 0)
    return (\"mov %0,#0    \");  
 
  return (\"ashr %0,%2\");
}"
  [(set_attr "length" "2")])
    
;;Logical shift right in HImode
    
(define_insn "lshrhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
    	(lshiftrt:HI (match_operand:HI 1 "register_operand" "0")
    		     (match_operand:HI 2 "general_operand" "rn")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT && INTVAL (operands[2]) >= BITS_PER_WORD)
    return (\"mov %0,#0    \");
    
  if (INTVAL (operands[2]) < 0)
    return (\"mov %0,#0    \");  
    
  return (\"shr %0,%2\");
}"
  [(set_attr "length" "2")] )
    
;;----------------------------------------------------------------------------------------------
;;Following part generates the rotate (left/right) instructions
;;In HImode - Word operand only
    
;;Rotate right in HImode
(define_insn "rotrhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
       	(rotatert:HI (match_operand:HI 1 "register_operand" "0")
       		     (match_operand:HI 2 "general_operand" "rn")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT && INTVAL (operands[2]) >= BITS_PER_WORD)
    return (\"mov %0,#0  \");
    
  return (\"ror %0,%2\");
  
}"
  [(set_attr "length" "2")])
    
;;Rotate left in HImode
    
(define_insn "rotlhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
       	(rotate:HI (match_operand:HI 1 "register_operand" "0")
       		   (match_operand:HI 2 "general_operand" "rn")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT && INTVAL (operands[2]) >= BITS_PER_WORD)
    return (\"mov %0,#0    \");
    
  return (\"rol %0,%2\");
}"
  [(set_attr "length" "2")]  )
    
;;------------------------------------------------------------------------------------------------
;;Following section outputs various jump instructions
;;Here l is used for directly printing lable reference
;; call output_addr_const function which will print the label 
    
;;Jump if equal
(define_insn "beq"
  [(set (pc)
    	(if_then_else (eq (cc0)
        (const_int 0))
        (label_ref (match_operand 0 "" ""))
        (pc)))
  ]
  ""
  "*
{
  /* TBD : This comment part for jump insns is still under review */
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_EQ,%l0\";
  
  else  
    return \"jmpa cc_EQ,%l0\";            
                    
}"
  [(set_attr "type" "branch")])
    
;;Jump if not equal to
    
(define_insn "bne"
  [(set (pc)
    	(if_then_else (ne (cc0)
        (const_int 0))
        (label_ref (match_operand 0 "" ""))
        (pc)))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_NE,%l0\";
  else  
    return \"jmpa cc_NE,%l0\";            
}"
  [(set_attr "type" "branch")])
    
;;Jump if greater than
   
(define_insn "bgt"
  [(set (pc)
    	(if_then_else (gt (cc0)
        (const_int 0))
        (label_ref (match_operand 0 "" ""))
        (pc)))]
  ""
  "*
{
  if (get_attr_length (insn) == 2 && optimize < 2)
    return \"jmpr cc_SGT,%l0\";
  else  
    return \"jmpa cc_SGT,%l0\";
}"
  [(set (attr "length") (if_then_else (and (ge (minus (match_dup 0) (pc))
				      (const_int -126))
			              (le (minus (match_dup 0) (pc))
				      (const_int 126)))
		                      (const_int 2) (const_int 4)))]
  )
    
;;Jump if unsigned greater than
    
(define_insn "bgtu"
  [(set (pc)
        (if_then_else (gtu (cc0)
        (const_int 0))
        (label_ref (match_operand 0 "" ""))
        (pc)))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_UGT,%l0\";
  else  
    return \"jmpa cc_UGT,%l0\";            
}"
  [(set_attr "type" "branch")])
    
;;Jump if less than
    
(define_insn "blt"
  [(set (pc)
    	(if_then_else (lt (cc0)
    	(const_int 0))
    	(label_ref (match_operand 0 "" ""))
    	(pc)))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_SLT,%l0\";
  else  
    return \"jmpa cc_SLT,%l0\";            
}"
  [(set_attr "type" "branch")])
    
;;Jump if unsigned less than
    
(define_insn "bltu"
  [(set (pc)
    	(if_then_else (ltu (cc0)
        (const_int 0))
    	(label_ref (match_operand 0 "" ""))
    	(pc)))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_ULT,%l0\";
  else  
    return \"jmpa cc_ULT,%l0\";            
}"
  [(set_attr "type" "branch")])
    
;;Jump if greater than or equal to
(define_insn "bge"
  [(set (pc)
        (if_then_else (ge (cc0)
        (const_int 0))
    	(label_ref (match_operand 0 "" ""))
    	(pc)))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_SGE,%l0\";
  else  
    return \"jmpa cc_SGE,%l0\";            
}"
  [(set_attr "type" "branch")])
    
;;Jump if unsigned greater than equal to
(define_insn "bgeu"
  [(set (pc)
    	(if_then_else (geu (cc0)
        (const_int 0))
        (label_ref (match_operand 0 "" ""))
        (pc)))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_UGE,%l0\";
  else  
    return \"jmpa cc_UGE,%l0\";            
}"
  [(set_attr "type" "branch")])
    
;;Jump if less than or equal to
(define_insn "ble"
  [(set (pc)
    	(if_then_else (le (cc0)
        (const_int 0))
        (label_ref (match_operand 0 "" ""))
        (pc)))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_SLE,%l0\";
  else  
    return \"jmpa cc_SLE,%l0\";            
}"
  [(set_attr "type" "branch")])
    
;;Jump if unsigned less than or equal to
(define_insn "bleu"
  [(set (pc)
    	(if_then_else (leu (cc0)
        (const_int 0))
        (label_ref (match_operand 0 "" ""))
        (pc)))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_ULE,%l0\";
  else  
    return \"jmpa cc_ULE,%l0\";            
}"
  [(set_attr "type" "branch")])

(define_insn ""
  [(set (pc)
    	(if_then_else (eq (cc0)
        (const_int 0))
    	(pc)
    	(label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_NE,%l0\";
  else  
    return \"jmpa cc_NE,%l0\";            
}"
  [(set_attr "type" "branch")] )
    
(define_insn ""
  [(set (pc)
    	(if_then_else (ne (cc0)
        (const_int 0))
    	(pc)
    	(label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_EQ,%l0\";
  else  
    return \"jmpa cc_EQ,%l0\";            
}"
  [(set_attr "type" "branch")] )
    
(define_insn ""
  [(set (pc)
    	(if_then_else (gt (cc0)
        (const_int 0))
        (pc)
        (label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_SLE,%l0\";
  else  
    return \"jmpa cc_SLE,%l0\";            
}"
  [(set_attr "type" "branch")] )
    
(define_insn ""
  [(set (pc)
    	(if_then_else (gtu (cc0)
        (const_int 0))
        (pc)
        (label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_ULE,%l0\";
  else  
    return \"jmpa cc_ULE,%l0\";            
}"
  [(set_attr "type" "branch")] )
    
(define_insn ""
  [(set (pc)
    	(if_then_else (lt (cc0)
        (const_int 0))
        (pc)
        (label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_SGE,%l0\";
  else  
    return \"jmpa cc_SGE,%l0\";            
}"
  [(set_attr "type" "branch")] )
    
(define_insn ""
  [(set (pc)
    	(if_then_else (ltu (cc0)
        (const_int 0))
        (pc)
    	(label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_UGE,%l0\";
  else  
    return \"jmpa cc_UGE,%l0\";            
}"
  [(set_attr "type" "branch")] )
    
(define_insn ""
  [(set (pc)
    	(if_then_else (ge (cc0)
        (const_int 0))
        (pc)
        (label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_SLT,%l0\";
  else  
    return \"jmpa cc_SLT,%l0\";            
}"
  [(set_attr "type" "branch")] )
    
(define_insn ""
  [(set (pc)
    	(if_then_else (geu (cc0)
        (const_int 0))
        (pc)
        (label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_ULT,%l0\";
  else  
    return \"jmpa cc_ULT,%l0\";            
}"
  [(set_attr "type" "branch")] )
    
(define_insn ""
  [(set (pc)
    	(if_then_else (le (cc0)
        (const_int 0))
        (pc)
        (label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_SGT,%l0\";
  else  
    return \"jmpa cc_SGT,%l0\";            
}"
  [(set_attr "type" "branch")] )
    
(define_insn ""
  [(set (pc)
    	(if_then_else (leu (cc0)
        (const_int 0))
        (pc)
        (label_ref (match_operand 0 "" ""))))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_UGT,%l0\";
  else 
    return \"jmpa cc_UGT,%l0\";            
}"
  [(set_attr "type" "branch")] )
  
;; Following RTL generates unconditional jump instruction
  
(define_insn "jump"
  [(set (pc)
    	(label_ref (match_operand 0 "" "")))
  ]
  ""
  "*
{
  if (get_attr_length(insn) == 2 && optimize < 2)
    return \"jmpr cc_UC,%l0\";
  else 
    return \"jmpa cc_UC,%l0\";            
}"
  [(set_attr "type" "branch")]  )
  
;;To generate indirect jump
  
(define_insn "indirect_jump"
  [(set (pc) (match_operand 0 "general_operand" "rmn"))
        (clobber (match_scratch:SI 1 "=&r"))
  ]
  ""
  "mov %1,%0;\\n\\tjmpi cc_UC,[%1]"
  [(set_attr "length" "8")] )

;; RTL to match for nop instruction.
   
(define_insn "nop"
  [(const_int 0)]
  ""
  "")
      
;;---------------------------------------------------------------------------------------------------------------------  
    
;;Following are the call instructions
;;Call has two variants 
;; 1.Call to a function which do not returns a value (Insn name used is call)
;; 2.Call to a function which returns some value (Insn name used is call_value)
   
(define_insn "call"
  [(call (match_operand:HI 0 "general_operand" "or")
   	 (match_operand:HI 1 "general_operand" "g"))]
  ""
  "* return output_call_insn1(operands);"
  [(set (attr "length") (symbol_ref "compute_call_length (operands,0)"))]
  )
   
(define_expand "call_value"
  [(set (match_operand:HI 0 "" "=r")
        (call (match_operand:HI 1 "general_call_operand" "ro")
      	      (match_operand:HI 2 "general_operand" "g"))) 
  ]
  ""
  "
{
  if (GET_CODE(XEXP(operands[1],0)) == PLUS)              
    FAIL;
}" ) 
 
(define_insn "*call_value"
  [(set (match_operand 0 "" "=r")
        (call (match_operand:HI 1 "general_call_operand" "rR")
	      (match_operand:HI 2 "general_operand" "g"))) 
  ]
  ""
  "* return output_call_insn(operands);"
  [(set (attr "length") (symbol_ref "compute_call_length (operands,1)"))]
  )

;;Following are the table jump insn formats for HI and PSI modes

(define_expand "tablejump"
  [(parallel [(set (pc) (match_operand 0 "register_operand" ""))
	                (use (label_ref (match_operand 1 "" "")))])]
  ""
  "")

(define_insn "*tablejump"
  [(set (pc) (match_operand:HI 0 "register_operand" "r"))
             (use (label_ref (match_operand 1 "" "")))]
  ""
  "jmpi	cc_UC ,[%0]"
)

(define_insn "*tablejump_large"
  [(set (pc) (match_operand:PSI 0 "register_operand" "r"))
             (use (label_ref (match_operand 1 "" "")))]
  ""
  "jmpi	cc_UC ,[%0]"
)
;;*******************************************************************
;;Following section contains all kind of compare instructions
;;In Qi,Hi and Si mode

;;Byte compare

(define_insn "cmpqi"
  [(set (cc0)
	(compare (match_operand:QI 0 "register_operand" "b,b")
		 (match_operand:QI 1 "nonmemory_operand" "b,n")))]
  ""
  "@
  cmpb	%X0,%X1
  cmpb  %X0,%X1"
  [(set_attr "length" "2,4")]
  )

;;Compare operands in SI mode

(define_insn "cmpsi"
  [(set (cc0)
 	(compare (match_operand:SI 0 "register_operand" "r,r")
 		 (match_operand:SI 1 "nonmemory_operand" "r,n")))]
  ""
  "*
{
  return output_cmppsi_insn(operands);
}
"
  )

;;compare operands in HI mode

(define_insn "cmphi"
  [(set (cc0)
	(compare (match_operand:HI 0 "register_operand" "r,r,r")
		 (match_operand:HI 1 "nonmemory_operand" "r,n,V")))]
  ""
  "*
{
  if (which_alternative < 2)
    return \"cmp   %T0,%T1\";
  else
    {
      if (TARGET_LARGE)
        {
          output_asm_insn(\"extp #PAG:%T1,#0x1\",operands);
          return \"cmp %T0,POF:%T1\";
        }
      else
        return \"cmp   %T0,%T1\";
    }
}"
  [(set_attr "length" "2,4,8")] 
  )  
	
;;----------------------------------------------------------------------------------------------------------------------	
;;Peephole optimization 
;;Following peephole optimization is used to generate bmovn instruction
;;In the following case 
;; cmpl Rwn
;; mov Rwm,Rwl
;; bmovn Rwn.i,Rwm.j are combined into
 
;; mov Rwn,Rwl
;; bmovn Rwn.i,Rwm.j
  	
(define_peephole
  [(set (match_operand:QI 0 "register_operand" "=r")
  	(not:QI (match_operand:QI 1 "register_operand" "0")))
   
   (set (match_operand:QI 2 "register_operand" "r")
	(match_operand:QI 3 "register_operand" "r"))	

   (set (zero_extract:HI (match_operand:HI 4 "register_operand" "")
			 (match_operand:HI 5 "immediate_operand" "")
			 (match_operand:HI 6 "general_operand" ""))
			 (match_operand:HI 7 "general_operand" ""))]
   "REGNO(operands[0]) == REGNO(operands[7]) && REGNO(operands[2]) == REGNO(operands[4])"
     {
       output_asm_insn(\"mov %2,%3\",operands);
       return \"bmovn %4.%s5,%7.0\";
     }
   )
	
(define_insn "lshrpsi3"
  [(set (match_operand:PSI 0 "register_operand" "=r")
	(lshiftrt:PSI (match_operand:PSI 1 "register_operand" "0")
		      (match_operand:PSI 2 "general_operand" "rn")))]
  "TARGET_LARGE"
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT && INTVAL (operands[2]) >= BITS_PER_WORD)
    return (\"mov %0,#0     \");
  return (\"shr %0,%2\");
}"
  [(set_attr "length" "2")])
