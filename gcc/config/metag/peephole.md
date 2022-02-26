;; Machine description for GNU compiler,
;; Imagination Technologies Meta version.
;; Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
;; Imagination Technologies Ltd

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it under
;; the terms of the GNU General Public License as published by the Free
;; Software Foundation; either version 3, or (at your option) any later
;; version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT ANY
;; WARRANTY; without even the implied warranty of MERCHANTABILITY or
;; FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
;; for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;;- peephole patterns

;; set full condition flags during move, flags from source value
(define_peephole
  [(set (match_operand:SI 0 "metag_register_op" "")
        (match_operand:SI 1 "metag_datareg_op"  ""))
   (set (reg:<MODE> CC_REG)
        (compare:CCANY 
           (match_dup 1) 
           (const_int 0)))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) <= LAST_ADDR_REG
   && METAG_DATA_REG_P (REGNO (operands[1]))"
  "SUBS\\t%0, %1, #0\\t\\t%@ (*movs rd 1 OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])
 
;; set full condition flags during move, flags from dest value
(define_peephole
  [(set (match_operand:SI 0 "metag_register_op" "")
        (match_operand:SI 1 "metag_datareg_op"  ""))
   (set (reg:<MODE> CC_REG)
        (compare:CCANY
           (match_dup 0)
           (const_int 0)))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) <= LAST_ADDR_REG
   && METAG_DATA_REG_P (REGNO (operands[1]))"
  "SUBS\\t%0, %1, #0\\t\\t%@ (*movs rd 0 OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

;; set condition flags during sign extension of a hi value
(define_peephole
  [(set (match_operand:SI                 0 "metag_datareg_op" "")
        (sign_extend:SI (match_operand:HI 1 "metag_datareg_op" "")))
   (set (reg:CCZNC CC_REG)
        (compare:<MODE>
           (match_dup 0) 
           (const_int 0)))]
  "!metag_cond_exec_p ()"
  "XSDSW\\t%0, %1\\t\\t%@ (*exts hisi dd OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

;; set condition flags during sign extension of a qi value
(define_peephole
  [(set (match_operand:SI                 0 "metag_datareg_op" "")
        (sign_extend:SI (match_operand:QI 1 "metag_datareg_op" "")))
   (set (reg:CCZNC CC_REG)
        (compare:<MODE>
           (match_dup 0)
           (const_int 0)))]
  "!metag_cond_exec_p ()"
  "XSDSB\\t%0, %1\\t\\t%@ (*exts qisi dd OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

;; Detect oppurtunities for post-increments of DI mode stores
(define_peephole
  [(set (mem:DI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:DI          1 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 2 "metag_offset6_di"  "")))]
  "TARGET_METAC_1_1 && !metag_cond_exec_p ()"
  "SETL\\t[%0+%2++], %1, %t1\\t%@ (*store DI post_inc OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (match_operand:SI          0 "metag_register_op" "")
        (plus:SI (match_dup 0)
                 (match_operand:SI 2 "metag_offset6_di"  "")))
   (set (mem:DI (match_dup 0))
        (match_operand:DI          1 "metag_register_op" ""))]
  "TARGET_METAC_1_1 && !metag_cond_exec_p ()"
  "SETL\\t[%0++%2], %1, %t1\\t%@ (*store DI pre_inc OK)"
  [(set_attr "type" "fast")])

;; Detect oppurtunities for post-increments of stores - not 1_1
(define_peephole
  [(set (mem:SI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:SI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))]
  "0 && !metag_cond_exec_p ()
   && !TARGET_METAC_1_1"
  "SETD\\t[%0+%1++], %2\\t%@ (*sto si maar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:SI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:SI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_si"  "")))]
  "0 && !metag_cond_exec_p ()
   && !TARGET_METAC_1_1"
  "SETD\\t[%0+%1++], %2\\t%@ (*sto si miar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:HI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:HI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))]
  "0 && !metag_cond_exec_p ()
   && !TARGET_METAC_1_1"
  "SETW\\t[%0+%1++], %2\\t%@ (*sto hi maar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:HI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:HI          2 "metag_register_op" ""))
   (set (match_dup 0) 
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_hi"  "")))]
  "0 && !metag_cond_exec_p ()
   && !TARGET_METAC_1_1"
  "SETW\\t[%0+%1++], %2\\t%@ (*sto hi miar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:QI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:QI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))]
  "0 && !metag_cond_exec_p ()
   && !TARGET_METAC_1_1"
  "SETB\\t[%0+%1++], %2\\t%@ (*sto qi maar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:QI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:QI          2 "metag_register_op" ""))
   (set (match_dup 0) 
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_qi"  "")))]
  "0 && !metag_cond_exec_p ()
   && !TARGET_METAC_1_1"
  "SETB\\t[%0+%1++], %2\\t%@ (*sto qi miar OK)"
  [(set_attr "type" "fast")])

;; Detect oppurtunities for post-increments of stores - 1_1
(define_peephole
  [(set (mem:SI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:SI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))]
  "!metag_cond_exec_p ()
   && TARGET_METAC_1_1
   && metag_same_regclass_p (operands[0], operands[1])
   && !metag_same_regclass_p (operands[0], operands[2])"
  "SETD\\t[%0+%1++], %2\\t%@ (*sto si maar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:SI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:SI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_si"  "")))]
  "!metag_cond_exec_p ()
   && TARGET_METAC_1_1"
  "SETD\\t[%0+%1++], %2\\t%@ (*sto si miar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:HI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:HI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))]
  "!metag_cond_exec_p ()
   && TARGET_METAC_1_1
   && metag_same_regclass_p (operands[0], operands[1])
   && !metag_same_regclass_p (operands[0], operands[2])"
  "SETW\\t[%0+%1++], %2\\t%@ (*sto hi maar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:HI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:HI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_hi"  "")))]
  "!metag_cond_exec_p ()
   && TARGET_METAC_1_1"
  "SETW\\t[%0+%1++], %2\\t%@ (*sto hi miar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:QI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:QI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))]
  "!metag_cond_exec_p ()
   && TARGET_METAC_1_1
   && metag_same_regclass_p (operands[0], operands[1])
   && !metag_same_regclass_p (operands[0], operands[2])"
  "SETB\\t[%0+%1++], %2\\t%@ (*sto qi maar OK)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (mem:QI (match_operand:SI  0 "metag_register_op" ""))
        (match_operand:QI          2 "metag_register_op" ""))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_qi"  "")))]
  "!metag_cond_exec_p ()
   && TARGET_METAC_1_1"
  "SETB\\t[%0+%1++], %2\\t%@ (*sto qi miar OK)"
  [(set_attr "type" "fast")])

;; Detect oppurtunities for post-increments of loads

(define_peephole
  [(set (match_operand:DI          0 "metag_register_op" "")
        (mem:DI (match_operand:SI  1 "metag_register_op" "")))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI 2 "metag_offset6_di"  "")))]
  "!metag_cond_exec_p ()"
  "GETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DI post_inc OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:SI          1 "metag_register_op" "")
        (plus:SI (match_dup 1)
                 (match_operand:SI 2 "metag_offset6_di"  "")))
   (set (match_operand:DI          0 "metag_register_op" "")
        (mem:DI (match_dup 1)))]
  "!metag_cond_exec_p ()"
  "GETL\\t%0, %t0, [%1++%2]\\t%@ (*load DI pre_inc OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:SI          0 "metag_register_op" "")
        (mem:SI (match_operand:SI  1 "metag_register_op" "")))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI 2 "metag_register_op" "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])
   && REGNO (operands[0]) != REGNO (operands[2])
   && metag_same_regclass_p (operands[1], operands[2])"
  "GETD\\t%0, [%1+%2++]\\t%@ (*lod si rmaa OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:SI          0 "metag_register_op" "")
        (mem:SI (match_operand:SI  1 "metag_register_op" "")))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI 2 "metag_offset6_si"  "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])"
  "GETD\\t%0, [%1+%2++]\\t%@ (*lod si rmia OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:HI          0 "metag_register_op" "")
        (mem:HI (match_operand:SI  1 "metag_register_op" "")))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI 2 "metag_register_op" "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])
   && REGNO (operands[0]) != REGNO (operands[2])
   && metag_same_regclass_p (operands[1], operands[2])"
  "GETW\\t%0, [%1+%2++]\\t%@ (*lod hi rmaa OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:HI          0 "metag_register_op" "")
        (mem:HI (match_operand:SI  1 "metag_register_op" "")))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI 2 "metag_offset6_hi"  "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])"
  "GETW\\t%0, [%1+%2++]\\t%@ (*lod hi rmia OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:SI           0 "metag_register_op" "")
        (zero_extend:SI
          (mem:HI (match_operand:SI 1 "metag_register_op" ""))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI  2 "metag_register_op" "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])
   && REGNO (operands[0]) != REGNO (operands[2])
   && metag_same_regclass_p (operands[1], operands[2])"
  "GETW\\t%0, [%1+%2++]\\t%@ (*lodz hi rmaa OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:SI           0 "metag_register_op" "")
        (zero_extend:SI
          (mem:HI (match_operand:SI 1 "metag_register_op" ""))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI  2 "metag_offset6_hi"  "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])"
  "GETW\\t%0, [%1+%2++]\\t%@ (*lodz hi rmia OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:QI          0 "metag_register_op" "")
        (mem:QI (match_operand:SI  1 "metag_register_op" "")))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI 2 "metag_register_op" "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])
   && REGNO (operands[0]) != REGNO (operands[2])
   && metag_same_regclass_p (operands[1], operands[2])"
  "GETB\\t%0, [%1+%2++]\\t%@ (*lod qi rmaa OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:QI          0 "metag_register_op" "")
        (mem:QI (match_operand:SI  1 "metag_register_op" "")))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI 2 "metag_offset6_qi"  "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])"
  "GETB\\t%0, [%1+%2++]\\t%@ (*lod qi rmia OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:SI           0 "metag_register_op" "")
        (zero_extend:SI
          (mem:QI (match_operand:SI 1 "metag_register_op" ""))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI  2 "metag_register_op" "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])
   && REGNO (operands[0]) != REGNO (operands[2])
   && metag_same_regclass_p (operands[1], operands[2])"
  "GETB\\t%0, [%1+%2++]\\t%@ (*lodz qi rmaa OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:SI           0 "metag_register_op" "")
        (zero_extend:SI
          (mem:QI (match_operand:SI 1 "metag_register_op" ""))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_operand:SI  2 "metag_offset6_qi"  "")))]
  "!metag_cond_exec_p ()
   && REGNO (operands[0]) != REGNO (operands[1])"
  "GETB\\t%0, [%1+%2++]\\t%@ (*lodz qi rmia OK)"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "")
        (match_operand:SI 1 "metag_reg_nofloat_op" ""))
   (set (match_dup 1)
        (match_operand:SI 2 "metag_reg_nofloat_op" ""))
   (set (match_dup 2)
        (match_dup 0))]
   "!metag_same_regclass_p (operands[1], operands[2])
    && dead_or_set_p (insn, operands[0])"
   "SWAP%?\\t%1, %2"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")])

(define_peephole
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "")
        (match_operand:SI 1 "metag_reg_nofloat_op" ""))
   (set (match_dup 1)
        (match_operand:SI 2 "metag_reg_nofloat_op" ""))
   (set (match_dup 2)
        (match_dup 0))]
   "!metag_same_regclass_p (operands[1], operands[2])"
   "SWAP%?\\t%1, %2\\t\;MOV%?\\t%0, %2"
  [(set_attr "type" "two")
   (set_attr "cond" "yes")])
   
;; Fixup some obvious reg alloc losage for loads

(define_peephole
  [(set (match_operand:QI 0 "metag_register_op"  "")
        (match_operand:QI 1 "memory_operand"     ""))
   (set (match_operand:QI 2 "metag_reg_nofloat_op"  "")
        (match_dup 0))]
   "!metag_cond_exec_p ()
    && dead_or_set_p (insn, operands[0])"
   "GETB\\t%2, %1"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:HI 0 "metag_register_op"  "")
        (match_operand:HI 1 "memory_operand"     ""))
   (set (match_operand:HI 2 "metag_reg_nofloat_op"  "")
        (match_dup 0))]
   "!metag_cond_exec_p ()
    && dead_or_set_p (insn, operands[0])"
   "GETW\\t%2, %1"
  [(set_attr "type" "load")])

(define_peephole
  [(set (match_operand:SI 0 "metag_register_op"  "")
        (match_operand:SI 1 "memory_operand"     ""))
   (set (match_operand:SI 2 "metag_reg_nofloat_op"  "")
        (match_dup 0))]
   "!metag_cond_exec_p ()
    && dead_or_set_p (insn, operands[0])"
   "GETD\\t%2, %1"
  [(set_attr "type" "load")])

;; misc peepholes
(define_peephole
  [(set (match_operand:SI    0 "metag_register_op" "")
        (match_operand:SI    1 "metag_datareg_op"  ""))
   (set (reg:<MODE> CC_REG)
        (compare:CCANY 
           (match_dup 0)
           (match_operand:SI 2 "metag_int_operand" "")))]
   "dead_or_set_p (insn, operands[0])"
   "CMP%?\\t%1, %2"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")])

(define_peephole
  [(set (match_operand:SI    0 "metag_register_op" "")
        (match_operand:SI    1 "metag_datareg_op"  ""))
   (set (reg:<MODE> CC_REG)
        (compare:CCANY
           (match_dup 0)
           (match_operand:SI 2 "metag_datareg_op"  "")))]
   "dead_or_set_p (insn, operands[0])
    && metag_same_regclass_p (operands[0], operands[1])"
   "CMP%?\\t%1, %2"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")])

;; This is an EVIL peephole. We should delete it!
(define_peephole
  [(set (match_operand:SI 0 "metag_register_op" "")
        (match_operand:SI 1 "metag_regorint_op" ""))
   (set (match_operand:SI 2 "metag_register_op" "")
        (match_dup 0))]
  "!metag_cond_exec_p ()
   && dead_or_set_p (insn, operands[0])
   && metag_move_valid_p (operands[2], operands[1])"
  {
    if ((REG_P (operands[1]) && metag_fpcreg_p (REGNO (operands[1])))
        || metag_fpcreg_p (REGNO (operands[2])))
      return "F\\tMOV%?\\t%2, %1";
    else if (metag_J_operand (operands[1], SImode))
      return "MOVT%?\\t%2, %1";
    else
      return "MOV%?\\t%2, %1";
  }
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")])

;; end of file
