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

(define_insn "*movv2sirr"
  [(set (match_operand:V2SI 0 "metag_datareg_op" "=d")
        (match_operand:V2SI 1 "metag_datareg_op"  "d"))]
  "TARGET_DSP"
  "DL\\tMOV\\t%0,%1\\t%@ (*mov v2si rr)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*movv2siri"
  [(set (match_operand:V2SI 0 "metag_datareg_op"   "=d")
        (match_operand:V2SI 1 "metag_vector_int_op" "vci"))]
  "TARGET_DSP
   && !reload_completed
   && !reload_in_progress"
  "#"
  ""
  [(set (match_dup 2)
        (match_dup 4))
   (set (match_dup 3)
        (match_dup 5))]
  {
    operands[2] = simplify_gen_subreg (SImode, operands[0], V2SImode, 0);
    operands[3] = simplify_gen_subreg (SImode, operands[0], V2SImode, UNITS_PER_WORD);
    operands[4] = XVECEXP (operands[1], 0, 0);
    operands[5] = XVECEXP (operands[1], 0, 1);
  }
  [(set_attr "type" "fast")])

(define_expand "movv2si"
  [(set (match_operand:V2SI 0 "nonimmediate_operand" "")
        (match_operand:V2SI 1 "general_operand"      ""))]
  "TARGET_DSP"
  {
    if (MEM_P (operands[0]) && !REG_P (operands[1]))
      {
        /* All except mem = const, mem = mem, or mem = addr can be done quickly */
        if (!no_new_pseudos)
          operands[1] = force_reg (V2SImode, operands[1]);
      }

  }
)

(define_insn "*lod_v2si"
  [(set (match_operand:V2SI 0 "metag_datareg_op" "=d")
        (match_operand:V2SI 1 "memory_operand"    "m"))]
  "TARGET_DSP"
  "GETL\\t%0, %t0, %1\\t%@ (*lod v2si rm OK)"
  [(set_attr "memaccess" "load")])

(define_insn "*sto_v2si"
  [(set (match_operand:V2SI 0 "memory_operand"  "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:V2SI 1 "metag_datareg_op" "r, a, a, d, d, !*da"))]
  "TARGET_DSP && !reload_completed"
  "SETL\\t%0, %1, %t1\\t%@ (*sto v2si rm OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_v2si_postreload"
  [(set (match_operand:V2SI 0 "memory_operand"  "=Tr,Te,Tf,Th,Tl")
        (match_operand:V2SI 1 "metag_datareg_op" "r, a, a, d, d"))]
  "TARGET_DSP && reload_completed"
  "SETL\\t%0, %1, %t1\\t%@ (*sto v2si rm OK)"
  [(set_attr "type" "fast")])

(define_expand "vec_setv2si"
  [(match_operand:V2SI 0 "metag_datareg_op"  "=d")
   (match_operand:SI   1 "metag_register_op"  "da")
   (match_operand      2 "const_int_operand"  "i")]
  "TARGET_DSP"
  {
    rtx tmp = gen_reg_rtx (SImode);

    rtx tmp2 = gen_rtx_PARALLEL (VOIDmode, gen_rtvec (1, GEN_INT (1 - INTVAL (operands[2]))));
    tmp2 = gen_rtx_VEC_SELECT (SImode, operands[0], tmp2);
    emit_insn (gen_rtx_SET (VOIDmode, tmp, tmp2));

    if (INTVAL (operands[2]) == 0)
      tmp = gen_rtx_VEC_CONCAT (V2SImode, tmp, operands[1]);
    else
      tmp = gen_rtx_VEC_CONCAT (V2SImode, operands[1], tmp);
    emit_insn (gen_rtx_SET (VOIDmode, operands[0], tmp));
    DONE;
  })
  
(define_insn_and_split "*vec_concatv2si"
  [(set (match_operand:V2SI                0 "metag_register_op" "=d")
        (vec_concat:V2SI (match_operand:SI 1 "metag_register_op"  "d")
                         (match_operand:SI 2 "metag_register_op"  "d")))]
  "TARGET_DSP"
  "#"
  "&& reload_completed"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 3)
        (match_dup 2))]
  {
    operands[3] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
    operands[0] = gen_rtx_REG (SImode, REGNO (operands[0]));
  }
  [(set_attr "type" "two")])

(define_expand "vec_extractv2si"
  [(match_operand:SI   0 "metag_register_op" "=da")
   (match_operand:V2SI 1 "metag_register_op"  "d")
   (match_operand      2 "const_int_operand"  "i")]
  "TARGET_DSP"
  {
    rtx tmp = gen_rtx_PARALLEL (VOIDmode, gen_rtvec (1, operands[2]));
    tmp = gen_rtx_VEC_SELECT (SImode, operands[1], tmp);
    emit_insn (gen_rtx_SET (VOIDmode, operands[0], tmp));
    DONE;
  })

(define_insn "*vec_selectv2si"
  [(set (match_operand:SI                        0 "metag_register_op" "=r")
        (vec_select:SI (match_operand:V2SI       1 "metag_datareg_op"   "d")
                       (parallel [(match_operand 2 "const_int_operand"  "i")])))]
  "TARGET_DSP"
  {
    switch (INTVAL (operands[2]))
      {
        case 0:
          return "MOV\\t%0, %1\\t%@ (*vec_select v2si 0)";
        case 1:
          return "MOV\\t%0, %t1\\t%@ (*vec_select v2si 1)";
        default:
          gcc_unreachable ();
      }
  }
  [(set_attr "type" "fast")])

(define_expand "vec_initv2si"
  [(match_operand:V2SI 0 "metag_datareg_op" "=d")
   (match_operand      1 ""                  "")]
  "TARGET_DSP"
  {
    rtx val0 = force_reg (SImode, XVECEXP (operands[1], 0, 0));
    rtx val1 = force_reg (SImode, XVECEXP (operands[1], 0, 1));
    rtx tmp = gen_rtx_VEC_CONCAT (V2SImode, val0, val1);
    emit_insn (gen_rtx_SET (VOIDmode, operands[0], tmp));
    DONE;
  })

(define_insn "*<expander>v2siddi16"
  [(set (match_operand:V2SI                0 "metag_datareg_op"     "+d")
        (3OPIMM16:V2SI (match_dup 0)
                       (match_operand:V2SI 1 "metag_vector_16bit_op" "v16")))]
  "TARGET_DSP"
  "DL\\t<MNEMONIC>\\t\\t%0,%0,%1\\t%@(*<MNEMONIC> v2si ddi)"
  [(set_attr "type" "fast")])

(define_insn "*<expander>v2siddi5"
  [(set (match_operand:V2SI               0 "metag_datareg_op"    "=d")
        (3OPIMM5:V2SI (match_operand:V2SI 1 "metag_datareg_op"     "d")
                      (match_operand:V2SI 2 "metag_vector_5bit_op" "vc5")))]
  "TARGET_DSP"
  "DL\\t<MNEMONIC>\\t\\t%0,%1,%2\\t%@(*<MNEMONIC> v2si ddi)"
  [(set_attr "type" "fast")])

(define_insn "<expander>v2si3"
  [(set (match_operand:V2SI              0 "metag_datareg_op" "=d")
        (3OPREG:V2SI (match_operand:V2SI 1 "metag_datareg_op"  "d")
                     (match_operand:V2SI 2 "metag_datareg_op"  "d")))]
  "TARGET_DSP"
  "DL\\t<MNEMONIC>\\t\\t%0,%1,%2\\t%@(*<MNEMONIC> v2si ddd)"
  [(set_attr "type" "fast")])

(define_insn "absv2si2"
  [(set (match_operand:V2SI           0 "metag_datareg_op" "=d")
        (abs:V2SI (match_operand:V2SI 1 "metag_datareg_op"  "d")))
   (clobber (reg:CC CC_REG))]
  "TARGET_DSP"
  "DL\\tABS\\t\\t%0,%1\\t%@(*abs v2si dd)"
  [(set_attr "type" "fast")
   (set_attr "ccstate" "ccx")])

(define_insn "negv2si2"
  [(set (match_operand:V2SI           0 "metag_datareg_op" "=d")
        (neg:V2SI (match_operand:V2SI 1 "metag_datareg_op"  "d")))]
  "TARGET_DSP"
  "DL\\tNEG\\t\\t%0,%1\\t%@(*neg v2si dd)"
  [(set_attr "type" "fast")])

(define_insn "<expander>v2si3"
  [(set (match_operand:V2SI              0 "metag_datareg_op" "=d")
        (MINMAX:V2SI (match_operand:V2SI 1 "metag_datareg_op"  "d")
                     (match_operand:V2SI 2 "metag_datareg_op"  "d")))
   (clobber (reg:CC CC_REG))]
  "TARGET_DSP"
  "DL\\t<MNEMONIC>\\t\\t%0,%1,%2\\t%@(*<MNEMONIC> v2si ddd)"
  [(set_attr "type" "fast")
   (set_attr "ccstate" "ccx")])

;; OP + flag set

(define_insn "*<expander>sv2siddi16"
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPIMM16:SI (match_operand:SI 2 "metag_datareg_op"       "d")
                         (match_operand:SI 3 "metag_16bit_op"         "KIP"))
            (const_int 0)))
   (set (match_operand:V2SI                0 "metag_datareg_op"      "+d")
        (3OPIMM16:V2SI (match_dup 0)
                       (match_operand:V2SI 1 "metag_vector_16bit_op"  "v16")))]
  "TARGET_DSP
   && (REGNO (operands[2]) == REGNO (operands[0])
       || (REGNO (operands[2]) == REGNO (operands[0]) + 1))
   && INTVAL (operands[3]) == INTVAL (CONST_VECTOR_ELT (operands[1], 0))"
  {
    if (REGNO (operands[3]) == REGNO (operands[1]))
      return "DL\\t<MNEMONIC>S\\t\\t%0,%0,%1\\t%@(*<MNEMONIC>S v2si ddi)";
    else
      return "DL\\t<MNEMONIC>S\\t\\t%t0,%t0,%t1\\t%@(*<MNEMONIC>S v2si ddi)";
  }
  [(set_attr "type" "fast")])

(define_insn "*<expander>sv2siddi5"
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPIMM5:SI (match_operand:SI 3 "metag_datareg_op"      "d")
                        (match_operand:SI 4 "metag_5bit_op"         "L"))
            (const_int 0)))
   (set (match_operand:V2SI               0 "metag_datareg_op"     "=d")
        (3OPIMM5:V2SI (match_operand:V2SI 1 "metag_datareg_op"      "d")
                      (match_operand:V2SI 2 "metag_vector_5bit_op"  "v16")))]
  "TARGET_DSP
   && (REGNO (operands[3]) == REGNO (operands[1])
       || (REGNO (operands[3]) == REGNO (operands[1]) + 1))
   && INTVAL (operands[4]) == INTVAL (CONST_VECTOR_ELT (operands[2], 0))"
  {
    if (REGNO (operands[3]) == REGNO (operands[1]))
      return "DL\\t<MNEMONIC>S\\t\\t%0,%1,%2\\t%@(*<MNEMONIC>S v2si di)";
    else
      return "DL\\t<MNEMONIC>S\\t\\t%t0,%t1,%t2\\t%@(*<MNEMONIC>S v2si di)";
  }
  [(set_attr "type" "fast")])

(define_insn "*<expander>sv2si3"
  [(set (reg:CC_NOOV CC_REG)
        (compare:CC_NOOV
            (3OPREG:SI (match_operand:SI 3 "metag_datareg_op"  "d")
                       (match_operand:SI 4 "metag_datareg_op"  "d"))
            (const_int 0)))
   (set (match_operand:V2SI              0 "metag_datareg_op" "=d")
        (3OPREG:V2SI (match_operand:V2SI 1 "metag_datareg_op"  "d")
                     (match_operand:V2SI 2 "metag_datareg_op"  "d")))]
  "TARGET_DSP
   && ((REGNO (operands[3]) == REGNO (operands[1])
        && REGNO (operands[4]) == REGNO (operands[2]))
       || (REGNO (operands[3]) == REGNO (operands[1]) + 1
           && REGNO (operands[4]) == REGNO (operands[2]) + 1))"
  {
    if (REGNO (operands[3]) == REGNO (operands[1]))
      return "DL\\t<MNEMONIC>S\\t\\t%0,%1,%2\\t%@(*<MNEMONIC>S v2si ddd)";
    else
      return "DL\\t<MNEMONIC>S\\t\\t%t0,%t1,%t2\\t%@(*<MNEMONIC>S v2si ddd)";
  }
  [(set_attr "type" "fast")])

