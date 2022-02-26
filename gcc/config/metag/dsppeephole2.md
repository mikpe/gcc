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

;; See comment at the top of dsppeephole.md for information about
;; dual unit DSP support in the metag backend.

;; The metag_dsp_peephole2_xxxxxx_convert functions used in these
;; rules promote operands to V2SI mode. They are all structured such
;; that they can be used for both flag setting and non-flag setting
;; rules.

;; DSP Math peephole2s

(define_peephole2
  [(set (match_operand:SI            0 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI 1 "metag_datareg_op" "")
                   (match_operand:SI 2 "metag_datareg_op" "")))
   (set (match_operand:SI            3 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI 4 "metag_datareg_op" "")
                   (match_operand:SI 5 "metag_datareg_op" "")))]
   "TARGET_DSP
    && metag_dsp_rrr_operands (operands, <commutative>)"
  [(set (match_dup 3)
        (3OPREG:V2SI (match_dup 4)
                     (match_dup 5)))]
  {
    metag_dsp_peephole2_rrr_convert (operands);
  })

(define_peephole2
  [(set (match_operand:SI              0 "metag_datareg_op" "")
        (3OPIMM16:SI (match_dup 0)
                     (match_operand:SI 1 "metag_16bit_op"   "")))
   (set (match_operand:SI              2 "metag_datareg_op" "")
        (3OPIMM16:SI (match_dup 2)
                     (match_dup 1)))]
   "TARGET_DSP
    && metag_dsp_ri16_operands (operands)
    <dualunitimmcondition>"
  [(set (match_dup 2)
        (3OPIMM16:V2SI (match_dup 2)
                       (match_dup 3)))]
  {
    metag_dsp_peephole2_ri16_convert (operands);
  })

(define_peephole2
  [(set (match_operand:SI             0 "metag_datareg_op" "")
        (3OPIMM5:SI (match_operand:SI 1 "metag_datareg_op" "")
                    (match_operand:SI 2 "metag_5bit_op"    "")))
   (set (match_operand:SI             3 "metag_datareg_op" "")
        (3OPIMM5:SI (match_operand:SI 4 "metag_datareg_op" "")
                    (match_dup 2)))]
   "TARGET_DSP
    && metag_dsp_rri5_operands (operands)
    <dualunitimmcondition>"
  [(set (match_dup 3)
        (3OPIMM5:V2SI (match_dup 4)
                      (match_dup 5)))]
  {
    metag_dsp_peephole2_rri5_convert (operands);
  })

;; DSP MUL peephole

;; MUL is not supported due to default DSP arithmetic mode being 16x16

;; DSP MIN/MAX

(define_peephole2
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
  [(parallel
     [(set (match_dup 3)
           (MINMAX:V2SI (match_dup 4)
                        (match_dup 5)))
      (clobber (reg:CC CC_REG))])]
  {
    metag_dsp_peephole2_rrr_convert (operands);
  })

;; DSP ABS peephole

(define_peephole2
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
  [(parallel
     [(set (match_dup 2)
           (abs:V2SI (match_dup 3)))
      (clobber (reg:CC CC_REG))])]
  {
    metag_dsp_peephole2_rr_convert (operands);
  })

;; DSP MOV peephole

(define_peephole2
  [(set (match_operand:SI 0 "metag_datareg_op" "")
        (match_operand:SI 1 "metag_datareg_op" ""))
   (set (match_operand:SI 2 "metag_datareg_op" "")
        (match_operand:SI 3 "metag_datareg_op" ""))]
   "TARGET_DSP && !metag_cond_exec_p ()
    && metag_dsp_rr_operands (operands)"
  [(set (match_dup 2)
        (match_dup 3))]
  {
    metag_dsp_peephole2_rr_convert (operands);
  })

;; DSP Math with flags peepholes

(define_peephole2
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
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPREG:SI (match_dup 1)
                                  (match_dup 2))
                       (const_int 0)))
              (set (match_dup 3)
                   (3OPREG:V2SI (match_dup 4)
                                (match_dup 5)))])]
  {
    metag_dsp_peephole2_rrr_convert (operands);
  })

(define_peephole2
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPREG:SI (match_operand:SI 1 "metag_datareg_op" "")
                       (match_operand:SI 2 "metag_datareg_op" ""))
            (const_int 0)))
   (set (match_operand:SI                0 "metag_datareg_op" "")
        (3OPREG:SI (match_dup 1)
                   (match_dup 2)))
   (set (match_operand:SI                3 "metag_datareg_op" "")
        (3OPREG:SI (match_operand:SI     4 "metag_datareg_op" "")
                   (match_operand:SI     5 "metag_datareg_op" "")))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rrr_operands (operands, <commutative>)"
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPREG:SI (match_dup 1)
                                  (match_dup 2))
                       (const_int 0)))
              (set (match_dup 3)
                   (3OPREG:V2SI (match_dup 4)
                                (match_dup 5)))])]
  {
    metag_dsp_peephole2_rrr_convert (operands);
  })

(define_peephole2
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
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPIMM16:SI (match_dup 0)
                                    (match_dup 1))
                       (const_int 0)))
              (set (match_dup 2)
                   (3OPIMM16:V2SI (match_dup 2)
                                  (match_dup 3)))])]
  {
    metag_dsp_peephole2_ri16_convert (operands);
  })

(define_peephole2
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPIMM5:SI (match_operand:SI 1 "metag_datareg_op" "")
                                   (match_operand:SI 2 "metag_5bit_op"    ""))
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
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPIMM5:SI (match_dup 1)
                                   (match_dup 2))
                       (const_int 0)))
              (set (match_dup 3)
                   (3OPIMM5:V2SI (match_dup 4)
                                 (match_dup 5)))])]
  {
    metag_dsp_peephole2_rri5_convert (operands);
  })

(define_peephole2
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPIMM16:SI (match_operand:SI 0 "metag_datareg_op" "")
                         (match_operand:SI 1 "metag_16bit_op"   ""))
            (const_int 0)))
   (set (match_dup 0)
        (3OPIMM16:SI (match_dup 0)
                     (match_dup 1)))
   (set (match_operand:SI                  2 "metag_datareg_op" "")
        (3OPIMM16:SI (match_dup 2)
                     (match_dup 1)))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_ri16_operands (operands)
   <dualunitimmcondition>"
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPIMM16:SI (match_dup 0)
                                    (match_dup 1))
                       (const_int 0)))
              (set (match_dup 2)
                   (3OPIMM16:V2SI (match_dup 2)
                                  (match_dup 3)))])]
  {
    metag_dsp_peephole2_ri16_convert (operands);
  })

(define_peephole2
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPIMM5:SI (match_operand:SI 1 "metag_datareg_op" "")
                        (match_operand:SI 2 "metag_5bit_op"    ""))
            (const_int 0)))
   (set (match_operand:SI                 0 "metag_datareg_op" "")
        (3OPIMM5:SI (match_dup 1)
                    (match_dup 2)))
   (set (match_operand:SI                 3 "metag_datareg_op" "")
        (3OPIMM5:SI (match_operand:SI     4 "metag_datareg_op" "")
                    (match_dup 2)))]
  "TARGET_DSP && !metag_cond_exec_p ()
   && metag_dsp_rri5_operands (operands)
   <dualunitimmcondition>"
  [(parallel [(set (reg:CC_NOOV CC_REG)
                   (compare:CC_NOOV
                       (3OPIMM5:SI (match_dup 1)
                                   (match_dup 2))
                       (const_int 0)))
              (set (match_dup 3)
                   (3OPIMM5:V2SI (match_dup 4)
                                 (match_dup 5)))])]
  {
    metag_dsp_peephole2_rri5_convert (operands);
  })

;; DSP OP + MOV

(define_peephole2
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
   && peep2_reg_dead_p (3, operands[0])
   && peep2_reg_dead_p (4, operands[3])"
  [(set (match_dup 6)
        (3OPREG:V2SI (match_dup 1)
                     (match_dup 2)))]
  {
    metag_dsp_peephole2_rrr_mov_convert (operands);
  })

;; DSP MUL + MOV

;; MUL is not supported due to default dsp arithmetic mode being 16x16

;; DSP ABS + MOV

(define_peephole2
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
   && peep2_reg_dead_p (2, operands[0])
   && peep2_reg_dead_p (4, operands[2])"
  [(parallel
      [(set (match_dup 4)
            (abs:SI (match_dup 1)))
       (clobber (reg:CC CC_REG))])]
  {
    metag_dsp_peephole2_rr_mov_convert (operands);
  })

;; DSP MIN/MAX + MOV

(define_peephole2
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
   && peep2_reg_dead_p (2, operands[0])
   && peep2_reg_dead_p (4, operands[3])"
  [(parallel
      [(set (match_dup 6)
            (MINMAX:SI (match_dup 1)
                       (match_dup 2)))
       (clobber (reg:CC CC_REG))])]
  {
    metag_dsp_peephole2_rrr_mov_convert (operands);
  })

;; END DSP Peephole2s

