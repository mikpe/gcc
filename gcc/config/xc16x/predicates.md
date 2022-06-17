;; Predicate definitions for Infineon xc16x.
;; Copyright (C) 2006 Free Software Foundation, Inc.
;; 
;; Contributed by Shrirang Khisti On behalf of KPIT Cummins Infosystems Ltd.
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

;; Return true if OP is a valid source operand for an integer move
;; instruction.

(define_predicate "general_operand_src"
  (match_code "const_int,const_double,const,symbol_ref,label_ref,subreg,reg,mem")
{
 return general_operand_src1(op,mode);

})

;; Return true if OP is a valid destination operand for an integer
;; move instruction.

(define_predicate "general_operand_dst_byte"
  (match_code "const_int,const_double,const,symbol_ref,label_ref,subreg,reg,mem")
{
    return general_operand_dst_byte1 (op, mode);
})

;; Likewise the second operand.

(define_predicate "general_call_operand"
  (match_code "const_int,const_double,const,symbol_ref,label_ref,subreg,reg,mem")
{

  return general_call_operand1 (op, mode);
})

;; Return true if OP is a suitable first operand for a general
;; arithmetic insn such as "add".

(define_predicate "general_operand_dst_mem"
  (match_code "const_int,const_double,const,symbol_ref,label_ref,subreg,reg,mem")
{

  return general_operand_dst_mem1 (op, mode);
})

;; Check that an operand is either a register or an unsigned 4-bit
;; constant.

(define_predicate "general_operand_src_mem"
  (match_code "const_int,const_double,const,symbol_ref,label_ref,subreg,reg,mem")
{
 return general_operand_src_mem1 (op, mode);

})

;; Check that an operand is either a register or an unsigned 4-bit
;; constant.

(define_predicate "general_operand_dst"
  (match_code "const_int,const_double,const,symbol_ref,label_ref,subreg,reg,mem")
{
 return general_operand_dst1 (op, mode);
})
