;; Machine description for GNU compiler,
;; Imagination Technologies Meta version.
;; Copyright (C) 2008, 2013
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

;;- instruction definitions

;;- @@The original PO technology requires these to be ordered by speed,
;;- @@    so that assigner will pick the fastest.

;;- See file "rtl.def" for documentation on define_insn, match_*, et. al.

;;- When naming insn's (operand 0 of define_insn) be careful about using
;;- names from other targets machine descriptions.



(define_insn_and_split "*movv2sfrr"
  [(set (match_operand:V2SF 0 "metag_fpreg_or_dreg_op" "=cx,cx,d, d")
        (match_operand:V2SF 1 "metag_fpreg_or_dreg_op"  "cx,d, cx,d"))]
  "TARGET_FPU_SIMD"
  "@
   FL\\tMOV\\t%0,%1\\t%@ (*mov v2sf rr)
   #
   #
   #"
  "&& reload_completed"
  [(set (match_dup 2)
        (match_dup 3))
   (set (match_dup 4)
        (match_dup 5))]
  {
    operands[2] = gen_rtx_REG (SImode, REGNO (operands[0]));
    operands[3] = gen_rtx_REG (SImode, REGNO (operands[1]));
    operands[4] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
    operands[5] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
  }
  [(set_attr "type" "FPfast")])

(define_insn_and_split "*movv2sfri"
  [(set (match_operand:V2SF 0 "metag_fpreg_op"       "=cx")
        (match_operand:V2SF 1 "metag_vector_float_op" "vcf"))]
  "TARGET_FPU_SIMD"
  {
    if (rtx_equal_p (CONST_VECTOR_ELT (operands[1], 0),
                     CONST_VECTOR_ELT (operands[1], 1)))
      return "FL\\tMOV\\t%0,#%h1";
    else
      return "#";
  }
  "&& reload_completed
   && !rtx_equal_p (CONST_VECTOR_ELT (operands[1], 0),
                    CONST_VECTOR_ELT (operands[1], 1))"
  [(set (match_dup 2)
        (match_dup 4))
   (set (match_dup 3)
        (match_dup 5))]
  {
    operands[2] = gen_rtx_REG (SFmode, REGNO (operands[0]));
    operands[3] = gen_rtx_REG (SFmode, REGNO (operands[0]) + 1);
    operands[4] = CONST_VECTOR_ELT (operands[1], 0);
    operands[5] = CONST_VECTOR_ELT (operands[1], 1);
  }
  [(set_attr "type" "FPfast")])

(define_expand "movv2sf"
  [(set (match_operand:V2SF 0 "nonimmediate_operand" "")
        (match_operand:V2SF 1 "general_operand"      ""))]
  "TARGET_FPU_SIMD"
  {
    if (MEM_P (operands[0]) && !REG_P (operands[1]))
      {
        /* All except mem = const, mem = mem, or mem = addr can be done quickly */
        if (!no_new_pseudos)
          operands[1] = force_reg (V2SFmode, operands[1]);
      }
    else if (GET_CODE(operands[1]) == CONST_VECTOR)
      if ( (!metag_fphalf_imm_op (CONST_VECTOR_ELT (operands[1], 0), SFmode)
            || !metag_fphalf_imm_op (CONST_VECTOR_ELT (operands[1], 1), SFmode)))
        {
          emit_move_insn (simplify_gen_subreg (SFmode, operands[0], V2SFmode, 0),
                          CONST_VECTOR_ELT (operands[1], 0));
          emit_move_insn (simplify_gen_subreg (SFmode, operands[0], V2SFmode, UNITS_PER_WORD),
                          CONST_VECTOR_ELT (operands[1], 1));
          DONE;
        }
  }
)

;; -----------------------------------------------------------------------------
;; | Matching V2SF load [post/pre]_[inc/dec/modify]
;; -----------------------------------------------------------------------------

(define_insn "*lod_v2sf_post_inc"
  [(set (match_operand:V2SF                      0 "metag_fpreg_op"       "=cx")
        (mem:V2SF (post_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_FPU_SIMD"
  "F\\tGETL\\t%0, %t0, [%1++]\\t%@ (*load V2SF post_inc OK)"
  [(set_attr "type" "load")])
 
(define_insn "*lod_v2sf_post_dec"
  [(set (match_operand:V2SF                      0 "metag_fpreg_op"       "=cx")
        (mem:V2SF (post_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_FPU_SIMD"
  "F\\tGETL\\t%0, %t0, [%1--]\\t%@ (*load V2SF post_dec OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_v2sf_pre_inc"
  [(set (match_operand:V2SF                     0 "metag_fpreg_op"       "=cx")
        (mem:V2SF (pre_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_FPU_SIMD"
  "F\\tGETL\\t%0, %t0, [++%1]\\t%@ (*load V2SF pre_inc OK)"
  [(set_attr "type" "load")])
 
(define_insn "*lod_v2sf_pre_dec"
  [(set (match_operand:V2SF                     0 "metag_fpreg_op"       "=cx")
        (mem:V2SF (pre_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_FPU_SIMD"
  "F\\tGETL\\t%0, %t0, [--%1]\\t%@ (*load V2SF pre_dec OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_v2sf_post_modify_disp"
  [(set (match_operand:V2SF                     0 "metag_fpreg_op"       "=cx")
        (mem:V2SF (post_modify:SI
                     (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                     (plus:SI (match_dup 1)
                              (match_operand:SI 2 "metag_offset6_v2sf"    "O8")))))]
  "TARGET_FPU_SIMD"
  "F\\tGETL\\t%0, %t0, [%1+%2++]\\t%@ (*load V2SF post_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_v2sf_post_modify_reg"
  [(set (match_operand:V2SF                     0 "metag_fpreg_op"       "=cx,cx,cx,cx")
        (mem:V2SF (post_modify:SI
                     (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                     (plus:SI (match_dup 1)
                              (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  "TARGET_FPU_SIMD"
  "F\\tGETL\\t%0, %t0, [%1+%2++]\\t%@ (*load V2SF post_modify_reg OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_v2sf_pre_modify_disp"
  [(set (match_operand:V2SF                     0 "metag_fpreg_op"       "=cx")
        (mem:V2SF (pre_modify:SI
                     (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                     (plus:SI (match_dup 1)
                              (match_operand:SI 2 "metag_offset6_v2sf"    "O8")))))]
  "TARGET_FPU_SIMD"
  "F\\tGETL\\t%0, %t0, [%1++%2]\\t%@ (*load V2SF pre_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_v2sf_pre_modify_reg"
  [(set (match_operand:V2SF                     0 "metag_fpreg_op"       "=cx,cx,cx,cx")
        (mem:V2SF (pre_modify:SI
                     (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                     (plus:SI (match_dup 1)
                              (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  "TARGET_FPU_SIMD"
  "F\\tGETL\\t%0, %t0, [%1++%2]\\t%@ (*load V2SF pre_modify_reg OK)"
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

(define_insn "*lod_v2sf_off6"
  [(set (match_operand:V2SF                  0 "metag_fpreg_op"       "=cx")
        (mem:V2SF (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                           (match_operand:SI 2 "metag_offset6_v2sf"    "O8"))))]
  "TARGET_FPU_SIMD"
  "F\\tGETL\\t%0, %t0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_v2sf_mem"
  [(set (match_operand:V2SF 0 "metag_datareg_op" "=d")
        (match_operand:V2SF 1 "memory_operand"    "m"))]
  "TARGET_FPU_SIMD"
  "GETL\\t%0, %t0, %1\\t%@ (*lod v2sf rm OK)"
  [(set_attr "memaccess" "load")])

;; -----------------------------------------------------------------------------
;; | Matching V2SF store [post/pre]_[inc/dec/modify]
;; -----------------------------------------------------------------------------

(define_insn "*sto_v2sf_post_inc"
  [(set (mem:V2SF (post_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:V2SF                      1 "metag_fpreg_op"        "cx"))]
  "TARGET_FPU_SIMD"
  "F\\tSETL\\t[%0++], %1, %t1\\t%@ (*store V2SF post_inc OK)"
  [(set_attr "type" "fast")])
  
(define_insn "*sto_v2sf_post_dec"
  [(set (mem:V2SF (post_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:V2SF                      1 "metag_fpreg_op"        "cx"))]
  "TARGET_FPU_SIMD"
  "F\\tSETL\\t[%0--], %1, %t1\\t%@ (*store V2SF post_dec OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_v2sf_pre_inc"
  [(set (mem:V2SF (pre_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:V2SF                     1 "metag_fpreg_op"        "cx"))]
  "TARGET_FPU_SIMD"
  "F\\tSETL\\t[++%0], %1, %t1\\t%@ (*store V2SF pre_inc OK)"
  [(set_attr "type" "fast")])
  
(define_insn "*sto_v2sf_pre_dec"
  [(set (mem:V2SF (pre_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:V2SF                     1 "metag_fpreg_op"        "cx"))]
  "TARGET_FPU_SIMD"
  "F\\tSETL\\t[--%0], %1, %t1\\t%@ (*store V2SF pre_dec OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_v2sf_post_modify_disp"
  [(set (mem:V2SF (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_v2sf"    "O8"))))
        (match_operand:V2SF                   2 "metag_fpreg_op"        "cx"))]
  "TARGET_FPU_SIMD"
  "F\\tSETL\\t[%0+%1++], %2, %t2\\t%@ (*store V2SF post_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_v2sf_post_modify_reg"
  [(set (mem:V2SF (post_modify:SI
                     (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                     (plus:SI (match_dup 0)
                              (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:V2SF                     2 "metag_fpreg_op"        "cx,cx,cx,cx"))]
  "TARGET_FPU_SIMD"
  "F\\tSETL\\t[%0+%1++], %2, %t2\\t%@ (*store V2SF post_modify_reg OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_v2sf_pre_modify_disp"
  [(set (mem:V2SF (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_v2sf"    "O8"))))
        (match_operand:V2SF                   2 "metag_fpreg_op"        "cx"))]
  "TARGET_FPU_SIMD"
  "F\\tSETL\\t[%0++%1], %2, %t2\\t%@ (*store V2SF pre_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_v2sf_pre_modify_reg"
  [(set (mem:V2SF (pre_modify:SI
                     (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                     (plus:SI (match_dup 0)
                              (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:V2SF                     2 "metag_fpreg_op"        "cx,cx,cx,cx"))]
  "TARGET_FPU_SIMD"
  "F\\tSETL\\t[%0++%1], %2, %t2\\t%@ (*store V2SF pre_modify_reg OK)"
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

(define_insn "*sto_v2sf_off6"
  [(set (mem:V2SF (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "da")
                           (match_operand:SI 1 "metag_offset6_v2sf"   "O8")))
        (match_operand:V2SF                  2 "metag_fpreg_op"       "cx"))]
  "TARGET_FPU_SIMD"
  "F\\tSETL\\t[%0+%1], %2, %t2"
  [(set_attr "type" "fast")])

(define_insn "*sto_v2sf_mem"
  [(set (match_operand:V2SF 0 "memory_operand"   "=m")
        (match_operand:V2SF 1 "metag_datareg_op"  "d"))]
  "TARGET_FPU_SIMD"
  "SETL\\t%0, %1, %t1\\t%@ (*sto v2sf rm OK)"
  [(set_attr "type" "FPfast")])

; Movement instructions

(define_insn "abs<mode>2"
  [(set (match_operand:FLMODES            0 "metag_fpreg_op" "=cx")
        (abs:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")))]
  "<FCONDITION>"
  "F<FW>\\tABS%?\\t%0,%1"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

;;(define_insn "*mmovsf_d_to_f"
;;  [(match_parallel          0 "metag_mmov_valid"
;;    [(set (match_operand:SF 1 "metag_fpreg_op"   "=cx")
;;          (match_operand:SF 2 "metag_datareg_op" "d"  ))
;;     (set (match_operand:SF 3 "metag_fpreg_op"   "=cx")
;;          (match_operand:SF 4 "metag_datareg_op" "d"  ))])]
;;  "TARGET_FPU"
;;  {
;;    switch (XVECLEN(operands[0], 0))
;;      {
;;      case 2: 
;;        return "F\\tMMOV\\t%1,%3,%2,%4";
;;      case 3: 
;;        return "F\\tMMOV\\t%1,%3,%5,%2,%4,%6";
;;      case 4: 
;;        return "F\\tMMOV\\t%1,%3,%5,%7,%2,%4,%6,%8";
;;      case 5: 
;;        return "F\\tMMOV\\t%1,%3,%5,%7,%9,%2,%4,%6,%8,%10";
;;      case 6: 
;;        return "F\\tMMOV\\t%1,%3,%5,%7,%9,%11,%2,%4,%6,%8,%10,%12";
;;      case 7: 
;;        return "F\\tMMOV\\t%1,%3,%5,%7,%9,%11,%13,%2,%4,%6,%8,%10,%12,%14";
;;      case 8: 
;;        return "F\\tMMOV\\t%1,%3,%5,%7,%9,%11,%13,%15,%2,%4,%6,%8,%10,%12,%14,%16";
;;      default:
;;        gcc_unreachable ();
;;      }
;;  })

(define_insn "*mov_<mode>_imm"
  [(set (match_operand:FMODES 0 "metag_fpreg_op"      "=cx")
        (match_operand:<MODE> 1 "metag_fphalf_imm_op"  "ci"))]
  "<FCONDITION>"
  "F<FW>\\tMOV\\t%0,#%h1"
  [(set_attr "type" "FPfast")])

(define_insn "neg<mode>2"
  [(set (match_operand:FLMODES            0 "metag_fpreg_op" "=cx")
        (neg:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")))]
  "<FCONDITION>"
  "F<FW>\\tNEG%?\\t%0,%1"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

; TODO: PACK
; TODO: SWAP

; Comparison Operations

(define_expand "cmp<mode>"
  [(match_operand:FMODES 0 "metag_fpreg_op"        "")
   (match_operand:<MODE> 1 "metag_fpreg_or_imm_op" "")]
  "<FCONDITION>"
  {
    enum machine_mode mode;

    /* These are processed via the conditional branch define_expand's later */
    metag_compare_op0 = operands[0];
    metag_compare_op1 = operands[1];

    mode = GET_MODE (operands[1]);

    /* Have to do register to register comparison for big constants */
    if (CONST_DOUBLE_P (operands[1]) && operands[1] != CONST0_RTX (mode))
        metag_compare_op1 = force_reg (mode, operands[1]);

    DONE;
  })

(define_insn "*cmpsf<CCFP:mode>"
  [(set (reg:CCFP CC_REG)
        (compare:CCFP 
          (match_operand:SF 0 "metag_fpreg_op"               "cx,cx")
          (match_operand:SF 1 "metag_fpreg_or_fpzero_imm_op" "cx,G")))]
  "TARGET_FPU"
  "@
  F<CCQ>\\tCMP%?\\t%0,%1
  F<CCQ>\\tCMP%?\\t%0,#%h1"
  [(set_attr "type" "FPfast")])

(define_insn "*cmpdf<CCFP:mode>"
  [(set (reg:CCFP CC_REG)
        (compare:CCFP 
          (match_operand:DF 0 "metag_fpreg_op"               "cx,cx")
          (match_operand:DF 1 "metag_fpreg_or_fpzero_imm_op" "cx,G")))]
  "TARGET_FPU && !metag_fpu_single"
  "@
  FD<CCQ>\\tCMP%?\\t%0,%1
  FD<CCQ>\\tCMP%?\\t%0,#%h1"
  [(set_attr "type" "FPfast")])

(define_insn "*abscmpsf<CCFP:mode>2"
  [(set (reg:CCFP CC_REG)
        (compare:CCFP 
          (abs:SF (match_operand:SF 0 "metag_fpreg_op"               "cx,cx"))
          (abs:SF (match_operand:SF 1 "metag_fpreg_or_fpzero_imm_op" "cx,G"))))]
  "TARGET_FPU"
  "@
  FA<CCQ>\\tCMP%?\\t%0,%1
  FA<CCQ>\\tCMP%?\\t%0,#%h1"
  [(set_attr "type" "FPmas")])

(define_insn "*abscmpdf<CCFP:mode>2"
  [(set (reg:CCFP CC_REG)
        (compare:CCFP 
          (abs:DF (match_operand:DF 0 "metag_fpreg_op"               "cx,cx"))
          (abs:DF (match_operand:DF 1 "metag_fpreg_or_fpzero_imm_op" "cx,G"))))]
  "TARGET_FPU && !metag_fpu_single"
  "@
  FDA<CCQ>\\tCMP%?\\t%0,%1
  FDA<CCQ>\\tCMP%?\\t%0,#%h1"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_insn "smax<mode>3"
  [(set (match_operand:FLMODES             0 "metag_fpreg_op" "=cx")
        (smax:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                     (match_operand:<MODE> 2 "metag_fpreg_op"  "cx")))
   (clobber (reg:CC CC_REG))]
  "<FCONDITION>"
  "F<FW>\\tMAX%?\\t%0,%1,%2"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")
   (set_attr "ccstate" "ccx")])

(define_insn "smin<mode>3"
  [(set (match_operand:FLMODES             0 "metag_fpreg_op" "=cx")
        (smin:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                     (match_operand:<MODE> 2 "metag_fpreg_op"  "cx")))
   (clobber (reg:CC CC_REG))]
  "<FCONDITION>"
  "F<FW>\\tMIN%?\\t%0,%1,%2"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")
   (set_attr "ccstate" "ccx")])

; Data Conversion
(define_insn "extendsfdf2"
  [(set (match_operand:DF                  0 "metag_fpreg_op" "=cx")
        (float_extend:DF (match_operand:SF 1 "metag_fpreg_op"  "cx")))]
  "TARGET_FPU && !metag_fpu_single"
  "F\\tFTOD%?\\t%0,%1"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_insn "truncdfsf2"
  [(set (match_operand:SF                    0 "metag_fpreg_op" "=cx")
        (float_truncate:SF (match_operand:DF 1 "metag_fpreg_op"  "cx")))]
  "TARGET_FPU && !metag_fpu_single"
  "F\\tDTOF%?\\t%0,%1"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

;; HFmode isnt supported at the moment but the rules would be something like:
;;
;;(define_insn "truncsfhf2"
;;  [(set (match_operand:HF           0 "metag_fpreg_op" "=cx")
;;        (float:HF (match_operand:SF 1 "metag_fpreg_op"  "cx")))]
;;  "TARGET_FPU"
;;  "F\\tFTOH%?\\t%0,%1"
;;  )
;;
;;(define_insn "truncdfhf2"
;;  [(set (match_operand:HF           0 "metag_fpreg_op" "=cx")
;;        (float:HF (match_operand:DF 1 "metag_fpreg_op"  "cx")))]
;;  "TARGET_FPU"
;;  "F\\tDTOH%?\\t%0,%1"
;;  )

(define_insn "fix_trunc<mode>si2"
  [(set (match_operand:SI              0 "metag_fpreg_op" "=cx")
        (fix:SI (match_operand:FSMODES 1 "metag_fpreg_op"  "cx")))]
  "<FCONDITION>"
  "FZ\\t<FT>TOI%?\\t%0,%1"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_expand "fix_truncsfdi2"
  [(set (match_dup 2)
        (float_extend:DF (match_operand:SF 1 "metag_fpreg_op" "")))
   (set (match_operand:DI                  0 "metag_fpreg_op" "")
        (fix:DI (match_dup 2)))]
  "TARGET_FPU && !metag_fpu_single"
  {
    operands[2] = gen_reg_rtx (DFmode);
  })

(define_insn "fix_truncdfdi2"
  [(set (match_operand:DI         0 "metag_fpreg_op" "=cx")
        (fix:DI (match_operand:DF 1 "metag_fpreg_op"  "cx")))]
  "TARGET_FPU && !metag_fpu_single"
  "FZ\\tDTOL%?\\t%0,%1"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_expand "fixuns_truncsfdi2"
  [(set (match_operand:DI                  0 "metag_fpreg_op" "")
        (unsigned_fix:DI (match_operand:SF 1 "metag_fpreg_op" "")))]
  "TARGET_FPU && !metag_fpu_single"
  {
    rtx dscr       = gen_reg_rtx (SImode);
    rtx fscr       = gen_reg_rtx (SFmode);
    rtx fscr_as_si = gen_rtx_SUBREG (SImode, fscr, 0);
    rtx fscr2      = gen_reg_rtx (SFmode);
    rtx fdscr      = gen_reg_rtx (DFmode);
    rtx rdhi       = simplify_gen_subreg (SImode, operands[0], DImode, 4);
    rtx temp_operands[1];

    /* single precision 2^63 is 0x5F000000 */
    emit_move_insn (fscr_as_si,  
                    gen_int_mode (0x5F000000, SImode));

    /* Is input in 'difficult' range */
    emit_insn (gen_cmpsf (operands[1], fscr));
    gen_metag_compare (GE, temp_operands, 0); 

    /* Copy input to scratch */
    emit_move_insn (fscr2, operands[1]);

    /*  If it is then wrap around once (note, we dont have to
     *  deal with the case where it's still difficult, C doesnt define
     *  overflow behaviour */
    emit_insn (gen_rtx_SET (VOIDmode, fscr2,
                       gen_rtx_IF_THEN_ELSE (SFmode,
                                             gen_rtx_GE (VOIDmode, temp_operands[0],
                                                         const0_rtx),
                                             gen_rtx_MINUS (SFmode, operands[1],
                                                            fscr),
                                             fscr2)));

    /* Extend to double before DI conversion */
    emit_insn (gen_rtx_SET (DFmode, fdscr,
                            gen_rtx_FLOAT_EXTEND (DFmode, fscr2)));

    /* Convert to DI */
    emit_insn (gen_rtx_SET (DImode, operands[0],
                            gen_rtx_FIX (DImode, fdscr)));

    /* Restore truncated value from earlier */
    emit_insn (gen_rtx_SET (SImode, dscr, 
                            gen_int_mode (0x80000000, SImode)));
    emit_insn (gen_rtx_SET (VOIDmode, rdhi,
                       gen_rtx_IF_THEN_ELSE (SImode,
                                             gen_rtx_GE (VOIDmode, temp_operands[0],
                                                         const0_rtx),
                                             gen_rtx_PLUS (SImode, rdhi,
                                                           dscr),
                                             rdhi)));
    DONE;
  })

(define_expand "fixuns_truncdfdi2"
  [(set (match_operand:DI                  0 "metag_fpreg_op" "")
        (unsigned_fix:DI (match_operand:DF 1 "metag_fpreg_op" "")))]
  "TARGET_FPU && !metag_fpu_single"
  {
    rtx dscr   = gen_reg_rtx (SImode);
    rtx fscr   = gen_reg_rtx (DFmode);
    rtx fscrlo_as_si = gen_rtx_SUBREG (SImode, fscr, 0);
    rtx fscrhi_as_si = gen_rtx_SUBREG (SImode, fscr, 4);
    rtx fdscr  = gen_reg_rtx (DFmode);
    rtx rdhi   = simplify_gen_subreg (SImode, operands[0], DImode, 4);
    rtx temp_operands[1];

    /* double precision 2^63 is 0x43e00000 00000000*/
    emit_move_insn (fscrhi_as_si, 
                    gen_int_mode (0x43e00000, SImode));
    emit_move_insn (fscrlo_as_si, const0_rtx);

    /* Is input in 'difficult' range */
    emit_insn (gen_cmpdf (operands[1], fscr));
    gen_metag_compare (GE, temp_operands, 0); 

    /* Copy input to scratch */
    emit_move_insn (fdscr, operands[1]);

    /*  If it is then wrap around once (note, we dont have to
     *  deal with the case where it's still difficult, C doesnt define
     *  overflow behaviour */
    emit_insn (gen_rtx_SET (VOIDmode, fdscr,
                       gen_rtx_IF_THEN_ELSE (DFmode,
                                             gen_rtx_GE (VOIDmode, temp_operands[0],
                                                                   const0_rtx),
                                             gen_rtx_MINUS (DFmode, operands[1],
                                                            fscr),
                                             fdscr)));

    /* Convert to DI */
    emit_insn (gen_rtx_SET (DImode, operands[0],
                            gen_rtx_FIX (DImode, fdscr)));

    /* Restore truncated value from earlier */
    emit_insn (gen_rtx_SET (SImode, dscr, 
                            gen_int_mode (0x80000000, SImode)));
    emit_insn (gen_rtx_SET (VOIDmode, rdhi,
                       gen_rtx_IF_THEN_ELSE (SImode,
                                             gen_rtx_GE (VOIDmode, temp_operands[0],
                                                         const0_rtx),
                                             gen_rtx_PLUS (SImode, rdhi,
                                                           dscr),
                                             rdhi)));

    DONE;
  })

(define_expand "fixuns_truncsfsi2"
  [(set (match_operand:SI                  0 "metag_fpreg_op" "")
        (unsigned_fix:SI (match_operand:SF 1 "metag_fpreg_op" "")))]
  "TARGET_FPU && metag_fpu_single"
  {
    if (metag_fpu_single)
      {
        rtx dscr   = gen_reg_rtx (SImode);
        rtx fscr   = gen_reg_rtx (SFmode);
        rtx fscr_as_si = gen_rtx_SUBREG (SImode, fscr, 0);
        rtx fscr2  = gen_reg_rtx (SFmode);
        rtx temp_operands[1];
    
        /* single precision 2^31 is 0x4F000000 */
        emit_move_insn (fscr_as_si, 
                        gen_int_mode (0x4f000000, SImode));
    
        /* Is input in 'difficult' range */
        emit_insn (gen_cmpsf (operands[1], fscr));
        gen_metag_compare (GE, temp_operands, 0); 
    
        /* Copy input to scratch */
        emit_move_insn (fscr2, operands[1]);
    
        /*  If it is then wrap around once (note, we dont have to
         *  deal with the case where it's still difficult, C doesnt define
         *  overflow behaviour */
        emit_insn (gen_rtx_SET (VOIDmode, fscr2,
                           gen_rtx_IF_THEN_ELSE (SFmode,
                                                 gen_rtx_GE (VOIDmode, temp_operands[0],
                                                                       const0_rtx),
                                                 gen_rtx_MINUS (SFmode, operands[1],
                                                                fscr),
                                                 fscr2)));
    
        /* Convert to SI */
        emit_insn (gen_rtx_SET (SImode, operands[0],
                                gen_rtx_FIX (SImode, fscr2)));
    
        /* Restore truncated value from earlier */
        emit_insn (gen_rtx_SET (SImode, dscr, 
                                gen_int_mode (0x80000000, SImode)));
        emit_insn (gen_rtx_SET (VOIDmode, operands[0],
                           gen_rtx_IF_THEN_ELSE (SImode,
                                                 gen_rtx_GE (VOIDmode, temp_operands[0],
                                                             const0_rtx),
                                                 gen_rtx_PLUS (SImode, operands[0],
                                                               dscr),
                                                 operands[0])));
    
        DONE;
      }
    else
      {
        rtx op2 = gen_reg_rtx (DFmode);
        rtx op3 = gen_reg_rtx (DImode);
    
        emit_insn (gen_extendsfdf2 (op2, operands[1]));
        emit_insn (gen_fix_truncdfdi2 (op3, op2));
        emit_move_insn (operands[0], gen_rtx_SUBREG (SImode, op3, 0));
        DONE;
      }
  })


; DTOX, FTOX, DTOXL not supported
(define_insn "floatsi<mode>2"
  [(set (match_operand:FSMODES          0 "metag_fpreg_op" "=cx")
        (float:<MODE> (match_operand:SI 1 "metag_fpreg_op"  "cx")))]
  "<FCONDITION>"
  "F\\tITO<FT>%?\\t%0,%1"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_expand "floatdisf2"
  [(set (match_dup 2)
        (float:DF (match_operand:DI 1 "metag_fpreg_op" "")))
   (set (match_operand:SF           0 "metag_fpreg_op" "")
        (float_truncate:SF (match_dup 2)))]
  "TARGET_FPU && !metag_fpu_single"
  { 
    operands[2] = gen_reg_rtx (DFmode);
  })

(define_insn "floatdidf2"
  [(set (match_operand:DF           0 "metag_fpreg_op" "=cx")
        (float:DF (match_operand:DI 1 "metag_fpreg_op"  "cx")))]
  "TARGET_FPU && !metag_fpu_single"
  "F\\tLTOD%?\\t%0,%1"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_expand "floatunshi<mode>2"
  [(set (match_dup:SI 2)
        (zero_extend:SI (match_operand:HI 1 "metag_reg_nofloat_op" "")))
   (set (match_operand:FMODES             0 "metag_fpreg_op"       "")
        (float:<MODE> (match_dup 2)))]
  "<FCONDITION>"
  {
    operands[2] = gen_reg_rtx (SImode);
  })

(define_expand "floatunssidf2"
  [(set (match_dup 2)
        (zero_extend:DI (match_operand:SI 1 "metag_register_op" "")))
   (set (match_operand:DF                 0 "metag_fpreg_op"    "")
        (float:DF (match_dup 2)))]
  "TARGET_FPU && !metag_fpu_single"
  {
    operands[2] = gen_reg_rtx (DImode);
  })

(define_expand "floatunsdidf2"
  [(set (match_operand:DF           0 "metag_fpreg_op"    "")
        (unsigned_float:DF (match_operand:DI 1 "metag_register_op" "")))]
  "TARGET_FPU && !metag_fpu_single"
  {
    metag_expand_didf2 (operands[0], operands[1]);
    DONE;
  })

(define_expand "floatunsdisf2"
  [(set (match_dup 2)
        (unsigned_float:DF (match_operand:DI 1 "metag_register_op" "")))
   (set (match_operand:SF           0 "metag_fpreg_op"    "")
        (float_truncate:SF (match_dup 2)))]
  "TARGET_FPU && !metag_fpu_single"
  {
    operands[2] = gen_reg_rtx (DFmode);

    metag_expand_didf2 (operands[2], operands[1]);

    emit_insn (gen_truncdfsf2 (operands[0], operands[2]));
    DONE;
  })

(define_expand "floatunssisf2"
  [(set (match_operand:SF           0 "metag_fpreg_op"    "")
        (unsigned_float:SF (match_operand:SI 1 "metag_register_op" "")))]
  "TARGET_FPU && metag_fpu_single"
  {
    if (metag_fpu_single)
      {
        rtx dscr        = gen_reg_rtx (SImode);
        rtx fscr2       = gen_reg_rtx (SFmode);
        rtx fscr2_as_si = gen_rtx_SUBREG (SImode, fscr2, 0);
        rtx temp_operands[1];
    
        /* Test to see if rs is in the difficult range (> 2^31) */
        emit_move_insn (dscr, operands[1]);
        metag_compare_op0 = gen_rtx_AND (SImode, dscr, 
                                         gen_int_mode (0x80000000, SImode));
        metag_compare_op1 = const0_rtx;
        gen_metag_compare (NE, temp_operands, 0); 
    
        /* Drop the 2^31 component */
        emit_insn (gen_andsi3 (dscr, dscr, 
                               gen_int_mode (0x7fffffff, SImode)));
   
        /* Convert to single */
        emit_insn (gen_floatsisf2 (operands[0], dscr));
    
        /* Prepare 2^31 in single precision */
        emit_move_insn (fscr2_as_si,  
                        gen_int_mode (0x4f000000, SImode));
    
        /* Add on the missing 2^31 if requried */
        emit_insn (gen_rtx_SET (VOIDmode, operands[0],
                           gen_rtx_IF_THEN_ELSE (SFmode,
                                                 gen_rtx_NE (VOIDmode, temp_operands[0],
                                                             const0_rtx),
                                                 gen_rtx_PLUS (SFmode, operands[0], fscr2),
                                                 operands[0])));
    
        DONE;
      }
    else
      {
        rtx op2 = gen_reg_rtx (DImode);
        rtx op3 = gen_reg_rtx (DFmode);
    
        emit_insn (gen_zero_extendsidi2 (op2, operands[1]));
        emit_insn (gen_floatdidf2 (op3, op2));
        emit_insn (gen_truncdfsf2 (operands[0], op3));
        DONE;
      }
  })

; Basic Arithmetic
; SFmode
(define_insn "add<mode>3"
  [(set (match_operand:FLMODES             0 "metag_fpreg_op" "=cx")
        (plus:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                     (match_operand:<MODE> 2 "metag_fpreg_op"  "cx")))]
  "<FCONDITION>"
  "F<FW>\\tADD%?\\t%0,%1,%2"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_insn "*add<FMODES:mode>_if_<CCALL:mode>_cxcxcxcx"
  [(set (match_operand:FMODES                               0 "metag_fpreg_op"     "=cx")
        (if_then_else:FMODES (match_operator                1 "comparison_operator"
                          [(match_operand:CCALL             2 "metag_<mode>_reg"   "")
                           (const_int 0)])
                         (plus:FMODES (match_operand:FMODES 3 "metag_fpreg_op"      "cx")
                                      (match_operand:FMODES 4 "metag_fpreg_op"      "cx"))
                         (match_operand:FMODES              5 "metag_fpreg_op"      "0")))]
  "<FCONDITION>"
  "F<FMODES:FW>\\tADD%z1\\t%0,%3,%4"
  [(set_attr "type"    "FPmas")
   (set_attr "ccstate" "xcc")])

(define_insn "*nadd<mode>3"
  [(set (match_operand:FLMODES               0 "metag_fpreg_op" "=cx")
        (neg:<MODE>
          (plus:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                       (match_operand:<MODE> 2 "metag_fpreg_op"  "cx"))))]
  "<FCONDITION>"
  "F<FW>I\\tADD%?\\t%0,%1,%2"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_insn "mul<mode>3"
  [(set (match_operand:FLMODES             0 "metag_fpreg_op" "=cx")
        (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                     (match_operand:<MODE> 2 "metag_fpreg_op"  "cx")))]
  "<FCONDITION>"
  "F<FW>\\tMUL%?\\t%0,%1,%2"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_insn "*nmul<mode>3"
  [(set (match_operand:FLMODES               0 "metag_fpreg_op" "=cx")
        (neg:<MODE>
          (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                       (match_operand:<MODE> 2 "metag_fpreg_op"  "cx"))))]
  "<FCONDITION>"
  "F<FW>I\\tMUL%?\\t%0,%1,%2"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_insn "sub<mode>3"
  [(set (match_operand:FLMODES              0 "metag_fpreg_op" "=cx")
        (minus:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                      (match_operand:<MODE> 2 "metag_fpreg_op"  "cx")))]
  "<FCONDITION>"
  "F<FW>\\tSUB%?\\t%0,%1,%2"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])

(define_insn "*sub<FMODES:mode>_if_<CCALL:mode>_cxcxcxcx"
  [(set (match_operand:FMODES                                    0 "metag_fpreg_op"     "=cx")
        (if_then_else:FMODES (match_operator                     1 "comparison_operator"
                            [(match_operand:CCALL                2 "metag_<mode>_reg"   "")
                               (const_int 0)])
                             (minus:FMODES (match_operand:FMODES 3 "metag_fpreg_op"      "cx")
                                           (match_operand:FMODES 4 "metag_fpreg_op"      "cx"))
                             (match_operand:FMODES               5 "metag_fpreg_op"      "0")))]
  "<FCONDITION>"
  "F<FMODES:FW>\\tSUB%z1\\t%0,%3,%4"
  [(set_attr "type"    "FPmas")
   (set_attr "ccstate" "xcc")])

(define_insn "*nsub<mode>3"
  [(set (match_operand:FLMODES                0 "metag_fpreg_op" "=cx")
        (neg:<MODE>
          (minus:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                        (match_operand:<MODE> 2 "metag_fpreg_op"  "cx"))))]
  "<FCONDITION>"
  "F<FW>I\\tSUB%?\\t%0,%1,%2"
  [(set_attr "type" "FPmas")
   (set_attr "predicable" "yes")])


; Extended Floating Point Insn's
; SFmode

; TODO MUZ
(define_insn "*muladd<mode>3_fused"
  [(set (match_operand:FLMODES               0 "metag_fpreg_op" "=cx")
        (plus:<MODE>
          (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                       (match_operand:<MODE> 2 "metag_fpreg_op"  "cx"))
          (match_operand:<MODE>              3 "metag_fpreg_op"  "0")))]
  "<FCONDITION>"
  "F<FW>\\tMUZ\\t%0,%1,%2"
  [(set_attr "type" "FPmas")])

(define_insn "*muladd1<mode>3_fused"
  [(set (match_operand:FLMODES               0 "metag_fpreg_op"     "=cx")
        (plus:<MODE>
          (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"      "cx")
                       (match_operand:<MODE> 2 "metag_fpreg_op"      "cx"))
          (match_operand:<MODE>              3 "metag_fpone_imm_op"  "H")))]
  "<FCONDITION>"
  "F<FW>\\tMUZ1\\t%0,%1,%2"
  [(set_attr "type" "FPmas")])

(define_insn "*mulsub<mode>3_fused"
  [(set (match_operand:FLMODES               0 "metag_fpreg_op" "=cx")
        (minus:<MODE>
          (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                       (match_operand:<MODE> 2 "metag_fpreg_op"  "cx"))
          (match_operand:<MODE>              3 "metag_fpreg_op"  "0")))]
  "<FCONDITION>"
  "F<FW>\\tMUZS\\t%0,%1,%2"
  [(set_attr "type" "FPmas")])

(define_insn "*mulsub1<mode>3_fused"
  [(set (match_operand:FLMODES               0 "metag_fpreg_op"     "=cx")
        (minus:<MODE>
          (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"      "cx")
                       (match_operand:<MODE> 2 "metag_fpreg_op"      "cx"))
          (match_operand:<MODE>              3 "metag_fpone_imm_op"  "H")))]
  "<FCONDITION>"
  "F<FW>\\tMUZS1\\t%0,%1,%2"
  [(set_attr "type" "FPmas")])

(define_insn "*nmuladd<mode>3_fused"
  [(set (match_operand:FLMODES                 0 "metag_fpreg_op" "=cx")
        (neg:<MODE>
          (plus:<MODE>
            (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                         (match_operand:<MODE> 2 "metag_fpreg_op"  "cx"))
            (match_operand:<MODE>              3 "metag_fpreg_op"  "0"))))]
  "<FCONDITION>"
  "F<FW>I\\tMUZ\\t%0,%1,%2"
  [(set_attr "type" "FPmas")])

(define_insn "*nmuladd1<mode>3_fused"
  [(set (match_operand:FLMODES                 0 "metag_fpreg_op"    "=cx")
        (neg:<MODE>
          (plus:<MODE>
            (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"     "cx")
                         (match_operand:<MODE> 2 "metag_fpreg_op"     "cx"))
            (match_operand:<MODE>              3 "metag_fpone_imm_op" "H"))))]
  "<FCONDITION>"
  "F<FW>I\\tMUZ1\\t%0,%1,%2"
  [(set_attr "type" "FPmas")])

(define_insn "*nmulsub<mode>3_fused"
  [(set (match_operand:FLMODES                 0 "metag_fpreg_op" "=cx")
        (neg:<MODE>
          (minus:<MODE>
            (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                         (match_operand:<MODE> 2 "metag_fpreg_op"  "cx"))
            (match_operand:<MODE>              3 "metag_fpreg_op"  "0"))))]
  "<FCONDITION>"
  "F<FW>I\\tMUZS\\t%0,%1,%2"
  [(set_attr "type" "FPmas")])

(define_insn "*nmulsub1<mode>3_fused"
  [(set (match_operand:FLMODES                 0 "metag_fpreg_op"     "=cx")
        (neg:<MODE>
          (minus:<MODE>
            (mult:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"      "cx")
                         (match_operand:<MODE> 2 "metag_fpreg_op"      "cx"))
            (match_operand:<MODE>              3 "metag_fpone_imm_op"  "H"))))]
  "<FCONDITION>"
  "F<FW>I\\tMUZS1\\t%0,%1,%2"
  [(set_attr "type" "FPmas")])

;; Divides etc

(define_expand "divsf3"
  [(set (match_operand:SF         0 "metag_fpreg_op" "")
        (div:SF (match_operand:SF 1 "metag_fpreg_op" "")
                (match_operand:SF 2 "metag_fpreg_op" "")))]
  "TARGET_FPU && TARGET_FAST_MATH"
  {
  })

(define_expand "divdf3"
  [(set (match_operand:DF          0 "metag_fpreg_op" "")
        (div:DF (match_dup 3)
                (match_operand:DF  2 "metag_fpreg_op" "")))
   (set (match_dup 4)
        (neg:DF (minus:DF (mult:DF (match_dup 2)
                                   (match_dup 0))
                          (match_dup 3))))
   (set (match_dup 0)
        (plus:DF (mult:DF (match_dup 4)
                          (match_dup 0))
                 (match_dup 0)))
   (set (match_dup 0)
        (mult:DF (match_operand:DF 1 "metag_fpreg_op" "")
                 (match_dup 0)))]
  "TARGET_FPU && !metag_fpu_single && TARGET_FAST_MATH"
  {
    operands[3] = CONST1_RTX (DFmode);
    operands[4] = gen_reg_rtx (DFmode);
  })

(define_insn_and_split "*div<mode>2_fast"
  [(set (match_operand:FMODES             0 "metag_fpreg_op" "=&cx")
        (div:<MODE> (match_operand:<MODE> 1 "metag_fpreg_op"  "cx")
                    (match_operand:<MODE> 2 "metag_fpreg_op"  "cx")))]
  "<FCONDITION> && TARGET_FAST_MATH"
  "#"
  "&& 1"
  [(const_int 0)]
  {
    emit_insn (gen_rcp<mode>2 (operands[0], CONST1_RTX (<MODE>mode), operands[2]));
    emit_insn (gen_mul<mode>3 (operands[0], operands[0], operands[1]));
    DONE;
  }
  [(set_attr "type" "FPrecipmas")])

(define_insn "rcp<mode>2"
  [(set (match_operand:FMODES             0 "metag_fpreg_op"     "=cx")
        (div:<MODE> (match_operand:<MODE> 1 "metag_fpone_imm_op"  "H")
                    (match_operand:<MODE> 2 "metag_fpreg_op"      "cx")))]
  "<FCONDITION> && TARGET_FAST_MATH"
  {
    if (TARGET_FLUSH_TO_ZERO)
      return "F<FW>Z\\tRCP\\t%0,%2";
    else
      return "F<FW>\\tRCP\\t%0,%2";
  }
  [(set_attr "type" "FPrecip")])

(define_insn "*rsq<mode>2"
  [(set (match_operand:FMODES                 0 "metag_fpreg_op"     "=cx")
        (div:<MODE> (match_operand:<MODE>     1 "metag_fpone_imm_op"  "H")
           (sqrt:<MODE> (match_operand:<MODE> 2 "metag_fpreg_op"      "cx"))))]
  "<FCONDITION> && TARGET_FAST_MATH"
  {
    if (TARGET_FLUSH_TO_ZERO)
      return "F<FW>Z\\tRSQ\\t%0,%2";
    else
      return "F<FW>\\tRSQ\\t%0,%2";
  }
  [(set_attr "type" "FPrecip")])

; ADDRE, MULRE, and SUBRE need vector modes
; Memory operations all use core instrutions with U[sd]=9
