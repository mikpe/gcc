;; Machine description for GNU compiler,
;; Imagination Technologies Meta version.
;; Copyright (C) 2008
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

;; READ THIS...

;; DSP peephole transformations are mostly templated. There are 3 templates:
;; 1) 3OPREG   - This is the set of operations that have 3 register operands
;;               2 input and 1 output.
;; 2) 3OPIMM16 - This is the set of operations that have 3 operands
;;               1 register input, 1 16 bit immediate input, 1 register output
;; 2) 3OPIMM5  - This is the set of operations that have 3 operands
;;               1 register input, 1 5 bit immediate input, 1 register output

;; Special code attributes:

;; <commutative>
;; Indicates if an insn code has commutative input operands.
;; <dualunitimmcondition>
;; Special immediate conditions for dual unit operations.
;; <expander>
;; The name of the insn suitable for use in rtl generation.
;; <MNEMONIC>
;; The assembler nmemonic for the given insn code.

;; Dual unit conditions:

;; The conditions for all DSP transformations are checked in the functions
;; called metag_dsp_xxxxxxx_operands. These functions check that the operands
;; in one instruction are an exact mirror of the operands of another
;; instruction.
;; For example metag_dsp_rrr_operands returns true for the following 2
;; instructions regardless of which order they appear (D1 or D0 first):
;;
;; (set (reg D0Re0 [D0.0])
;;      (operation (reg D0Ar6 [D0.1])
;;                 (reg D0Ar4 (D0.2])))
;; (set (reg D1Re0 [D1.0])
;;      (operation (reg D1Ar5 [D1.1])
;;                 (reg D1Ar3 (D1.2])))

(define_peephole
  [(set (match_operand:SI            0 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI 1 "metag_datareg_op" "")
                   (match_operand:SI 2 "metag_datareg_op" "")))
   (set (match_operand:SI            3 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI 4 "metag_datareg_op" "")
                   (match_operand:SI 5 "metag_datareg_op" "")))]
   "TARGET_DSP && !metag_cond_exec_p ()
    && metag_dsp_rrr_operands (operands, <commutative>)"
   "DL\\t<MNEMONIC>\\t%0, %1, %2\\t%@ (*<MNEMONIC>\\t%3, %4, %5)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (match_operand:SI            3 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI 4 "metag_datareg_op" "")
                   (match_operand:SI 5 "metag_datareg_op" "")))
   (set (match_operand:SI            0 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI 1 "metag_datareg_op" "")
                   (match_operand:SI 2 "metag_datareg_op" "")))]
   "TARGET_DSP && !metag_cond_exec_p ()
    && metag_dsp_rrr_operands (operands, <commutative>)"
   "DL\\t<MNEMONIC>\\t%3, %4, %5\\t%@ (*<MNEMONIC>\\t%0, %1, %2)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (match_operand:SI              0 "metag_datareg_op" "")
        (3OPIMM16:SI (match_dup 0)
                     (match_operand:SI 1 "metag_16bit_op"   "")))
   (set (match_operand:SI              2 "metag_datareg_op" "")
        (3OPIMM16:SI (match_dup 2)
                     (match_dup 1)))]
   "TARGET_DSP && !metag_cond_exec_p ()
    && metag_dsp_ri16_operands (operands)
    <dualunitimmcondition>"
   "DL\\t<MNEMONIC>\\t%0, %0, %1\\t%@ (*<MNEMONIC>\\t%2, %2, %1)"
  [(set_attr "type" "fast")])

(define_peephole
  [(set (match_operand:SI              0 "metag_datareg_op" "")
        (3OPIMM5:SI (match_operand:SI  1 "metag_datareg_op" "")
                    (match_operand:SI  2 "metag_5bit_op"    "")))
   (set (match_operand:SI              3 "metag_datareg_op" "")
        (3OPIMM5:SI (match_operand:SI  4 "metag_datareg_op" "")
                    (match_dup 2)))]
   "TARGET_DSP && !metag_cond_exec_p ()
    && metag_dsp_rri5_operands (operands)
    <dualunitimmcondition>"
   "DL\\t<MNEMONIC>\\t%0, %1, %2\\t%@ (*<MNEMONIC>\\t%3, %4, %2)"
  [(set_attr "type" "fast")])

;; DSP MIN/MAX

(define_peephole
  [(parallel
     [(set (match_operand:SI            0 "metag_datareg_op" "")
           (MINMAX:SI (match_operand:SI 1 "metag_datareg_op" "")
                      (match_operand:SI 2 "metag_datareg_op" "")))
      (clobber (reg:CC CC_REG))])
   (parallel
     [(set (match_operand:SI            3 "metag_datareg_op" "")
           (MINMAX:SI (match_operand:SI 4 "metag_datareg_op" "")
                      (match_operand:SI 5 "metag_datareg_op" "")))
      (clobber (reg:CC CC_REG))])]
   "TARGET_DSP && !metag_cond_exec_p ()
    && metag_dsp_rrr_operands (operands, true)"
   "DL\\t<MNEMONIC>\\t%0, %1, %2\\t%@ (*<MNEMONIC>\\t%3, %4, %5)"
  [(set_attr "type" "fast")
   (set_attr "ccstate" "ccx")])

;; DSP ABS peephole

(define_peephole
  [(parallel
     [(set (match_operand:SI         0 "metag_datareg_op" "")
           (abs:SI (match_operand:SI 1 "metag_datareg_op" "")))
      (clobber (reg:CC CC_REG))])
   (parallel
     [(set (match_operand:SI         2 "metag_datareg_op" "")
           (abs:SI (match_operand:SI 3 "metag_datareg_op" "")))
      (clobber (reg:CC CC_REG))])]
   "TARGET_DSP && !metag_cond_exec_p ()
    && metag_dsp_rr_operands (operands)"
   "DL\\tABS\\t%0, %1\\t%@ *ABS\\t%2, %3)"
  [(set_attr "type" "fast")
   (set_attr "ccstate" "ccx")])

;; DSP MOV peephole

(define_peephole
  [(set (match_operand:SI 0 "metag_datareg_op" "")
        (match_operand:SI 1 "metag_datareg_op" ""))
   (set (match_operand:SI 2 "metag_datareg_op" "")
        (match_operand:SI 3 "metag_datareg_op" ""))]
   "TARGET_DSP && !metag_cond_exec_p ()
    && metag_dsp_rr_operands (operands)"
   "DL\\tMOV\\t%0, %1\\t%@ (*MOV\\t%2, %3)"
  [(set_attr "type" "fast")])

;; DSP Math with flags peepholes

(define_peephole
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPREG:SI (match_operand:SI 1 "metag_datareg_op" "")
                                  (match_operand:SI 2 "metag_datareg_op" ""))
                       (const_int 0)))
              (set (match_operand:SI                0 "metag_datareg_op" "")
                   (3OPREG:SI (match_dup 1)
                              (match_dup 2)))])
   (set (match_operand:SI                           3 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI                4 "metag_datareg_op" "")
                   (match_operand:SI                5 "metag_datareg_op" "")))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rrr_operands (operands, <commutative>)"
  "DL\\t<MNEMONIC>S\\t%0, %1, %2\\t%@ (*<MNEMONIC>S\\t%3, %4, %5)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(set (match_operand:SI                           3 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI                4 "metag_datareg_op" "")
                   (match_operand:SI                5 "metag_datareg_op" "")))
   (parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPREG:SI (match_operand:SI 1 "metag_datareg_op" "")
                                  (match_operand:SI 2 "metag_datareg_op" ""))
                       (const_int 0)))
              (set (match_operand:SI                0 "metag_datareg_op" "")
                   (3OPREG:SI (match_dup 1)
                              (match_dup 2)))])]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rrr_operands (operands, <commutative>)"
  "DL\\t<MNEMONIC>S\\t%0, %1, %2\\t%@ (*<MNEMONIC>S\\t%3, %4, %5)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPREG:SI (match_operand:SI 4 "metag_datareg_op" "")
                       (match_operand:SI 5 "metag_datareg_op" ""))
            (const_int 0)))
   (set (match_operand:SI                0 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI     1 "metag_datareg_op" "")
                   (match_operand:SI     2 "metag_datareg_op" "")))
   (set (match_operand:SI                3 "metag_datareg_op" "")
        (3OPREG:SI (match_dup 4)
                   (match_dup 5)))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rrr_operands (operands, <commutative>)"
  "DL\\t<MNEMONIC>S\\t%3, %4, %5\\t%@ (*<MNEMONIC>S\\t%0, %1, %2)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPIMM16:SI (match_operand:SI 0 "metag_datareg_op" "")
                                    (match_operand:SI 1 "metag_16bit_op"   ""))
                       (const_int 0)))
              (set (match_dup 0)
                   (3OPIMM16:SI (match_dup 0)
                                (match_dup 1)))])
   (set (match_operand:SI                             2 "metag_datareg_op" "")
        (3OPIMM16:SI (match_dup 2)
                     (match_dup 1)))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_ri16_operands (operands)
   <dualunitimmcondition>"
  "DL\\t<MNEMONIC>S\\t%0, %0, %1\\t%@ (*<MNEMONIC>S\\t%2, %2, %1)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPIMM5:SI (match_operand:SI 1 "metag_datareg_op" "")
                                   (match_operand:SI 2 "metag_5bit_op"   ""))
                       (const_int 0)))
              (set (match_operand:SI                 0 "metag_datareg_op" "")
                   (3OPIMM5:SI (match_dup 1)
                               (match_dup 2)))])
   (set (match_operand:SI                            3 "metag_datareg_op" "")
        (3OPIMM5:SI (match_operand:SI                4 "metag_datareg_op" "")
                    (match_dup 2)))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rri5_operands (operands)
   <dualunitimmcondition>"
  "DL\\t<MNEMONIC>S\\t%0, %1, %2\\t%@ (*<MNEMONIC>S\\t%3, %4, %2)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(set (match_operand:SI                             2 "metag_datareg_op" "")
        (3OPIMM16:SI (match_dup 2)
                     (match_operand:SI                1 "metag_16bit_op"   "")))
   (parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPIMM16:SI (match_operand:SI 0 "metag_datareg_op" "")
                                    (match_dup 1))
                       (const_int 0)))
              (set (match_dup 0)
                   (3OPIMM16:SI (match_dup 0)
                                (match_dup 1)))])]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_ri16_operands (operands)
   <dualunitimmcondition>"
  "DL\\t<MNEMONIC>S\\t%0, %0, %1\\t%@ (*<MNEMONIC>S\\t%2, %2, %1)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(set (match_operand:SI                            3 "metag_datareg_op" "")
        (3OPIMM5:SI (match_operand:SI                4 "metag_datareg_op" "")
                    (match_operand:SI                2 "metag_5bit_op"   "")))
   (parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPIMM5:SI (match_operand:SI 1 "metag_datareg_op" "")
                                   (match_dup 2))
                       (const_int 0)))
              (set (match_operand:SI                 0 "metag_datareg_op" "")
                   (3OPIMM5:SI (match_dup 1)
                               (match_dup 2)))])]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rri5_operands (operands)
   <dualunitimmcondition>"
  "DL\\t<MNEMONIC>S\\t%0, %1, %2\\t%@ (*<MNEMONIC>S\\t%3, %4, %2)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPIMM16:SI (match_operand:SI 2 "metag_datareg_op" "")
                         (match_operand:SI 1 "metag_16bit_op"   ""))
            (const_int 0)))
   (set (match_operand:SI                  0 "metag_datareg_op" "")
        (3OPIMM16:SI (match_dup 0)
                     (match_dup 1)))
   (set (match_dup 2)
        (3OPIMM16:SI (match_dup 2)
                     (match_dup 1)))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_ri16_operands (operands)
   <dualunitimmcondition>"
  "DL\\t<MNEMONIC>S\\t%2, %2, %1\\t%@ (*<MNEMONIC>S\\t%0, %0, %1)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPIMM5:SI (match_operand:SI 4 "metag_datareg_op" "")
                        (match_operand:SI 2 "metag_5bit_op"    ""))
            (const_int 0)))
   (set (match_operand:SI                 0 "metag_datareg_op" "")
        (3OPIMM5:SI (match_operand:SI     1 "metag_datareg_op" "")
                    (match_dup 2)))
   (set (match_operand:SI                 3 "metag_datareg_op" "")
        (3OPIMM5:SI (match_dup 4)
                    (match_dup 2)))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rri5_operands (operands)
   <dualunitimmcondition>"
  "DL\\t<MNEMONIC>S\\t%3, %4, %2\\t%@ (*<MNEMONIC>S\\t%0, %1, %2)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPIMM16:SI (match_operand:SI 2 "metag_datareg_op" "")
                         (match_operand:SI 1 "metag_16bit_op"   ""))
            (const_int 0)))
   (set (match_dup 2)
        (3OPIMM16:SI (match_dup 2)
                     (match_dup 1)))
   (set (match_operand:SI                  0 "metag_datareg_op" "")
        (3OPIMM16:SI (match_dup 0)
                     (match_dup 1)))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_ri16_operands (operands)
   <dualunitimmcondition>"
  "DL\\t<MNEMONIC>S\\t%2, %2, %1\\t%@ (*<MNEMONIC>S\\t%0, %0, %1)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_peephole
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPIMM5:SI (match_operand:SI 4 "metag_datareg_op" "")
                        (match_operand:SI 2 "metag_5bit_op"    ""))
            (const_int 0)))
   (set (match_operand:SI                 3 "metag_datareg_op" "")
        (3OPIMM5:SI (match_dup 4)
                    (match_dup 2)))
   (set (match_operand:SI                 0 "metag_datareg_op" "")
        (3OPIMM5:SI (match_operand:SI     1 "metag_datareg_op" "")
                    (match_dup 2)))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rri5_operands (operands)
   <dualunitimmcondition>"
  "DL\\t<MNEMONIC>S\\t%3, %4, %2\\t%@ (*<MNEMONIC>S\\t%0, %1, %2)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

;; DSP OP + MOV

(define_peephole
  [(set (match_operand:SI            0 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI 1 "metag_datareg_op" "")
                   (match_operand:SI 2 "metag_datareg_op" "")))
   (set (match_operand:SI            3 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI 4 "metag_datareg_op" "")
                   (match_operand:SI 5 "metag_datareg_op" "")))
   (set (match_operand:SI            6 "metag_datareg_op" "")
        (match_dup 0))
   (set (match_operand:SI            7 "metag_datareg_op" "")
        (match_dup 3))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rrr_mov_operands (operands, <commutative>)
   && dead_or_set_p (PREV_INSN (insn), operands[0])
   && dead_or_set_p (insn, operands[3])"
  "DL\\t<MNEMONIC>\\t%6, %1, %2\\t%@ (*<MNEMONIC>\\t%7, %4, %5)"
  [(set_attr "type"    "fast")])

;; DSP ABS + MOV

(define_peephole
  [(parallel
      [(set (match_operand:SI         0 "metag_reg_nofloat_op" "")
            (abs:SI (match_operand:SI 1 "metag_datareg_op"     "")))
       (clobber (reg:CC CC_REG))])
   (set (match_operand:SI             4 "metag_datareg_op"     "")
        (match_dup 0))
   (parallel   
      [(set (match_operand:SI         2 "metag_reg_nofloat_op" "")
            (abs:SI (match_operand:SI 3 "metag_datareg_op"     "")))
       (clobber (reg:CC CC_REG))])
   (set (match_operand:SI             5 "metag_datareg_op"     "")
        (match_dup 2))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rr_rr_mov_operands (operands)
   && dead_or_set_p (PREV_INSN (PREV_INSN (insn)), operands[0])
   && dead_or_set_p (insn, operands[2])"
  "DL\\tABS\\t%4, %1\\t%@ (*ABS\\t%5, %2)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "ccx")])

;; DSP MIN/MAX + MOV

(define_peephole
  [(parallel
      [(set (match_operand:SI            0 "metag_datareg_op" "")
            (MINMAX:SI (match_operand:SI 1 "metag_datareg_op" "")
                       (match_operand:SI 2 "metag_datareg_op" "")))
       (clobber (reg:CC CC_REG))])
   (set (match_operand:SI                6 "metag_datareg_op" "")
        (match_dup 0))
   (parallel
      [(set (match_operand:SI            3 "metag_datareg_op" "")
            (MINMAX:SI (match_operand:SI 4 "metag_datareg_op" "")
                       (match_operand:SI 5 "metag_datareg_op" "")))
       (clobber (reg:CC CC_REG))])
   (set (match_operand:SI                7 "metag_datareg_op" "")
        (match_dup 3))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rrr_mov_operands (operands, <commutative>)
   && dead_or_set_p (PREV_INSN (PREV_INSN (insn)), operands[0])
   && dead_or_set_p (insn, operands[3])"
  "DL\\t<MNEMONIC>\\t%6, %1, %2\\t%@ (*<MNEMONIC>\\r%7, %4, %5)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "ccx")])

;; END DSP Peepholes

