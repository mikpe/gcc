;; Predicate definitions for META
;; Copyright (C) 2007, 2010, 2013 Imagination Technologies Ltd

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

(define_predicate "metag_register_op"
(match_code "subreg,reg")
{
  unsigned int regno;

  if (!register_operand (op, mode))
    return false;

  if (SUBREG_P (op))
    {
      op = SUBREG_REG (op);

      /* A subreg may refer to a mem before reload_completed,
       * it's treated as a register because reload will reload it
       * into one if required. */
      if (!reload_completed && MEM_P (op))
        return true;
    }

  if (!REG_P (op))
    return false;

  regno = REGNO (op);

  if (regno >= FIRST_PSEUDO_REGISTER)
    return true;

  return (regno != TXRPT_REG && regno != TTREC_REG);
})

;; to used only in "move rules" as src predicate
(define_predicate "metag_move_src_op"
(match_code "subreg,reg")
{
  if (!register_operand (op, mode))
    return false;

  if (SUBREG_P (op)) 
    op = SUBREG_REG (op);

  return REG_P (op);
})

;; to be used in "move rules" as dst predicate
(define_predicate "metag_move_dst_op"
(match_code "subreg,reg")
{
  if (!register_operand (op, mode))
    return false;

  if (SUBREG_P (op))
    op = SUBREG_REG (op);

  return REG_P (op);
})

(define_predicate "metag_reg_nofloat_op"
(match_code "subreg,reg")
{
  unsigned int regno;

  if (!register_operand (op, mode))
    return false;

  if (SUBREG_P (op))
    {
      op = SUBREG_REG (op);

      /* A subreg may refer to a mem before reload_completed,
       * it's treated as a register because reload will reload it
       * into one if required. */
      if (!reload_completed && MEM_P (op))
        return true;
    }

  if (!REG_P (op))
    return false;

  regno = REGNO (op);

  if (regno >= FIRST_PSEUDO_REGISTER)
    return true;

  return (regno  != TXRPT_REG && regno != TTREC_REG)
          && !METAG_FPC_REG_P (regno);
})

;; Any hard data register
(define_predicate "metag_hard_datareg_op"
(match_code "reg")
{
  unsigned int regno = REGNO (op);

  return mode == GET_MODE (op)
         && METAG_DATA_REG_P (regno);
})

;; Any hard address register
(define_predicate "metag_hard_addrreg_op"
(match_code "reg")
{
  unsigned int regno = REGNO (op);

  return mode == GET_MODE (op)
         && METAG_ADDR_REG_P (regno);
})

;; Any hard gen register
(define_predicate "metag_hard_genreg_op"
(ior (match_operand 0 "metag_hard_datareg_op")
     (match_operand 0 "metag_hard_addrreg_op")))

(define_predicate "metag_regnofrm_op"
(match_code "subreg,reg")
{
  if (metag_reg_nofloat_op (op, mode))
    {
      /* Subreg can hide a lot of non-reg things that we don't want! */
      if (SUBREG_P (op))
        op = SUBREG_REG (op);

      /* Reject all stack frame related pointers. */
      return !metag_frame_related_rtx (op);
   }

  return false;
})


(define_predicate "metag_regframe_op"
(match_code "subreg,reg")
{
  if (metag_register_op (op, mode))
    {
      /* Subreg can hide a lot of non-reg things that we don't want! */
      if (SUBREG_P (op))
        op = SUBREG_REG (op);

      /* Accept only stack relasted pointers */
      return metag_frame_related_rtx (op);
    }

  return false;
})

(define_predicate "metag_regorint_op"
(match_code "subreg,reg,const_int")
{
  /* Integer constants are allowed */
  if (CONST_INT_P (op))
    return true;

  return metag_register_op (op, mode);
})

(define_predicate "metag_okbindex_op"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O1 (op)")))

(define_predicate "metag_okwindex_op"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O2 (op)")))

(define_predicate "metag_okdindex_op"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O4 (op)")))

(define_predicate "metag_oklindex_op"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O8 (op)")))

(define_predicate "metag_int_operand"
(and (match_code "const_int")
     (match_test "METAG_LETTER_FOR_CONST (op) != 0")))

(define_predicate "metag_smallint_op"
(and (match_code "const_int")
     (match_test "(INTVAL (op) >= -255 && INTVAL (op) <= 255)")))

(define_predicate "metag_bigint_op"
(match_code "const_int"))

(define_predicate "symbolic_operand"
(match_code "symbol_ref"))

(define_predicate "metag_symsmall_op"
(and (match_code "symbol_ref")
     (match_test "METAG_SYMBOL_FLAG_SMALL_P (op)")))

(define_predicate "metag_symlarge_op"
(and (match_code "symbol_ref")
     (match_test "METAG_SYMBOL_FLAG_LARGE_P (op)")))

(define_predicate "metag_symglobal_op"
(and (match_code "symbol_ref")
     (match_test "METAG_SYMBOL_FLAG_GLOBAL_P (op)")))

(define_predicate "metag_symdirect_op"
(and (match_code "symbol_ref")
     (match_test  "METAG_SYMBOL_FLAG_DIRECT_P (op)")))

(define_predicate "metag_call_addr"
(and (match_code "mem")
     (ior (match_test "SYMBOL_REF_P (XEXP (op, 0))")
          (match_test "REG_P (XEXP (op, 0))"))))

(define_predicate "code_address"
(match_code "label_ref,symbol_ref")
{
  /* Label references are the easy bit */
  if (LABEL_REF_P (op))
    return true;

  if (METAG_FLAG_PIC)
    return false;

  return SYMBOL_REF_P (op)
         && !METAG_SYMBOL_FLAG_SMALL_P (op)
         && !METAG_SYMBOL_FLAG_LARGE_P (op)
         && !METAG_SYMBOL_FLAG_GLOBAL_P (op)
         && (SYMBOL_REF_TLS_MODEL (op) == TLS_MODEL_NONE);
})

(define_predicate "metag_cc_reg"
(match_code "reg")
{
  if (mode == VOIDmode)
    {
      mode = GET_MODE (op);
      if (GET_MODE_CLASS (mode) != MODE_CC)
        return false;
    }

  return (mode == GET_MODE (op)
          && REG_P (op)
          && REGNO (op) == MCC_REGNUM);
})

(define_predicate "metag_cc_noov_reg"
(match_code "reg")
{
  if (mode == VOIDmode)
    {
      mode = GET_MODE (op);
      if (GET_MODE_CLASS (mode) != MODE_CC)
        return false;
    }

  return (mode == GET_MODE (op)
          && REG_P (op)
          && REGNO (op) == MCC_REGNUM);
})

(define_predicate "metag_cc_z_reg"
(match_code "reg")
{
  if (mode == VOIDmode)
    {
      mode = GET_MODE (op);
      if (GET_MODE_CLASS (mode) != MODE_CC)
        return false;
    }

  return (mode == GET_MODE (op)
          && REG_P (op)
          && REGNO (op) == MCC_REGNUM);
})

(define_predicate "metag_cc_fp_reg"
(match_code "reg")
{
  if (mode == VOIDmode)
    {
      mode = GET_MODE (op);
      if (GET_MODE_CLASS (mode) != MODE_CC)
        return false;
    }

  return (mode == GET_MODE (op)
          && REG_P (op)
          && REGNO (op) == MCC_REGNUM);
})

(define_predicate "metag_cc_fp_q_reg"
(match_code "reg")
{
  if (mode == VOIDmode)
    {
      mode = GET_MODE (op);
      if (GET_MODE_CLASS (mode) != MODE_CC)
        return false;
    }

  return (mode == GET_MODE (op)
          && REG_P (op)
          && REGNO (op) == MCC_REGNUM);
})

(define_predicate "load_multiop"
(match_code "parallel")
{
  HOST_WIDE_INT     count = XVECLEN (op, 0);
  unsigned int      dst_regno;
  enum machine_mode xfer_mode;
  unsigned int      word_size;
  unsigned int      lastreg;
  rtx               src_addr;
  enum reg_class    dst_regclass;
  HOST_WIDE_INT     i;
  rtx               elt;
  rtx               src;
  rtx               dst;

  elt = XVECEXP (op, 0, 0);
  if (count < 2 || count > 8 || GET_CODE (elt) != SET)
    return false;

  src = SET_SRC (elt);
  dst = SET_DEST (elt);

  if (GET_CODE (src) != PLUS)
    return false;

  elt = XVECEXP (op, 0, 1);
  if (GET_CODE (elt) != SET)
    return false;

  /* Work out if this is SImode or DImode case */
  xfer_mode = GET_MODE (SET_SRC (elt));
  word_size = GET_MODE_SIZE (xfer_mode);

  if (word_size == 4 && count <= 2)
    return false;

  /* Now check it more carefully */
  if (!REG_P (dst)
      || !REG_P (XEXP (src, 0))
      || REGNO (XEXP (src, 0)) != REGNO (dst)
      || !CONST_INT_P (XEXP (src, 1))
      || INTVAL (XEXP (src, 1)) != (HOST_WIDE_INT)((count - 1) * word_size))
    return false;

  src = SET_SRC (elt);
  dst = SET_DEST (elt);

  /* Perform a quick check so we don't blow up below.  */
  if (!REG_P (dst) || !MEM_P (src))
    return false;

  lastreg      = REGNO (dst);
  dst_regno    = REGNO (dst);
  src_addr     = XEXP (src, 0);
  dst_regclass = METAG_REGNO_REG_CLASS (dst_regno);

  for (i = 2; i < count; i++)
  {
    elt = XVECEXP (op, 0, i);
    if (GET_CODE (elt) != SET)
      return false;

    src = SET_SRC (elt);
    dst = SET_DEST (elt);

    if (!REG_P (dst)
        || GET_MODE (dst) != xfer_mode
        || REGNO (dst) - dst_regno > 2 * word_size
        || REGNO (dst) <= lastreg
        || METAG_REGNO_REG_CLASS (REGNO (dst)) != dst_regclass
        || !MEM_P (src)
        || GET_MODE (src) != xfer_mode
        || GET_CODE (XEXP (src, 0)) != PLUS
        || !rtx_equal_p (XEXP (XEXP (src, 0), 0), src_addr)
        || !CONST_INT_P (XEXP (XEXP (src, 0), 1))
        || INTVAL (XEXP (XEXP (src, 0), 1)) != (HOST_WIDE_INT)((i - 1) * word_size))
      return false;

    lastreg = REGNO (dst);
  }

  return true;
})

(define_predicate "store_multiop"
(match_code "parallel")
{
  HOST_WIDE_INT     count = XVECLEN (op, 0);
  unsigned int      src_regno;
  enum machine_mode xfer_mode;
  unsigned int      word_size;
  unsigned int      lastreg;
  enum reg_class    src_regclass;
  rtx               dst_addr;
  HOST_WIDE_INT     i;
  rtx               elt;
  rtx               src;
  rtx               dst;

  elt = XVECEXP (op, 0, 0);
  if (count <= 2 || count > 9 || GET_CODE (elt) != SET)
    return false;

  dst = SET_DEST (elt);
  src = SET_SRC (elt);

  if (GET_CODE (src) != PLUS)
    return false;

  elt = XVECEXP (op, 0, 1);
  if (GET_CODE (elt) != SET)
    return false;

  /* Work out if this is SImode or DImode case */
  xfer_mode = GET_MODE (SET_DEST (elt));
  word_size = GET_MODE_SIZE (xfer_mode);

  /* Now check it more carefully */
  if (!REG_P (dst)
      || !REG_P (XEXP (src, 0))
      || REGNO (XEXP (src, 0)) != REGNO (dst)
      || !CONST_INT_P (XEXP (src, 1))
      || INTVAL (XEXP (src, 1)) != (HOST_WIDE_INT)((count - 1) * word_size))
    return false;

  src = SET_SRC (elt);
  dst = SET_DEST (elt);

  /* Perform a quick check so we don't blow up below.  */
  if (!MEM_P (dst) || !REG_P (src))
    return false;

  lastreg      = REGNO (src);
  src_regno    = REGNO (src);
  dst_addr     = XEXP (dst, 0);
  src_regclass = METAG_REGNO_REG_CLASS (src_regno);

  for (i = 2; i < count; i++)
  {
    elt = XVECEXP (op, 0, i);
    if (GET_CODE (elt) != SET)
      return false;

    src = SET_SRC (elt);
    dst = SET_DEST (elt);

    if (!REG_P (src)
        || GET_MODE (src) != xfer_mode
        || REGNO (src) - src_regno > 2 * word_size
        || REGNO (src) <= lastreg
        || METAG_REGNO_REG_CLASS (REGNO (src)) != src_regclass
        || !MEM_P (dst)
        || GET_MODE (dst) != xfer_mode
        || GET_CODE (XEXP (dst, 0)) != PLUS
        || !rtx_equal_p (XEXP (XEXP (dst, 0), 0), dst_addr)
        || !CONST_INT_P (XEXP (XEXP (dst, 0), 1))
        || INTVAL (XEXP (XEXP (dst, 0), 1)) != (HOST_WIDE_INT)((i - 1) * word_size))
      return false;

    lastreg = REGNO (src);
  }

  return true;
})

(define_predicate "metag_datareg_op"
(and (match_code "subreg,reg")
     (match_operand 0 "metag_register_op"))
{
  unsigned int regno;

  if (SUBREG_P (op))
    {
      op = SUBREG_REG (op);

      /* A subreg may refer to a mem before reload_completed,
       * it's treated as a register because reload will reload it
       * into one if required. */
      if (!reload_completed && MEM_P (op))
        return true;
    }

  if (!REG_P (op))
    return false;

  regno = REGNO (op);

  if (regno >= FIRST_PSEUDO_REGISTER)
    return true;

  return METAG_DATA_REG_P (regno);
})

(define_predicate "metag_addrreg_op"
(and (match_code "subreg,reg")
     (match_operand 0 "metag_register_op"))
{
  unsigned int regno;

  if (SUBREG_P (op))
    {
      op = SUBREG_REG (op);

      /* A subreg may refer to a mem before reload_completed,
       * it's treated as a register because reload will reload it
       * into one if required. */
      if (!reload_completed && MEM_P (op))
        return true;
    }

  if (!REG_P (op))
    return false;

  regno = REGNO (op);

  if (regno >= FIRST_PSEUDO_REGISTER)
    return true;

  return METAG_ADDR_REG_P (regno);
})

(define_predicate "metag_fpreg_op"
(and (match_code "subreg,reg")
     (match_operand 0 "metag_register_op"))
{
  unsigned int regno;

  if (SUBREG_P (op))
    {
      op = SUBREG_REG (op);

      /* A subreg may refer to a mem before reload_completed.
       * it's treated as a register because reload will reload it
       * into one if required. */
      if (!reload_completed && MEM_P (op))
        return true;
    }

  if (!REG_P (op))
    return false;

  regno = REGNO (op);

  if (regno >= FIRST_PSEUDO_REGISTER)
    return true;

  return METAG_FPC_REG_P (regno);
})

(define_predicate "metag_fpreg_or_dreg_op"
(and (match_code "subreg,reg")
     (match_operand 0 "metag_register_op"))
{
  unsigned int regno;

  if (SUBREG_P (op))
    {
      op = SUBREG_REG (op);

      /* A subreg may refer to a mem before reload_completed,
       * it's treated as a register because reload will reload it
       * into one if required. */
      if (!reload_completed && MEM_P (op))
        return true;
    }

  if (!REG_P (op))
    return false;

  regno = REGNO (op);

  if (regno >= FIRST_PSEUDO_REGISTER)
    return true;

  return METAG_FPC_REG_P (regno) || METAG_DATA_REG_P (regno);
})

(define_predicate "metag_txrpt_op"
(and (match_code "reg")
     (match_test "REGNO (op) == TXRPT_REGNUM")))

(define_predicate "metag_ttrec_op"
(and (match_code "reg")
     (match_test "REGNO (op) == TTREC_REGNUM")))

;; Return true if OP is a valid TXRPT_REGNUM
;; source operand that can be loaded quickly for the given mode
(define_predicate "metag_txrpt_src_op"
(ior (and (match_code "const_int")
          (match_test "satisfies_constraint_I (op)
                       || satisfies_constraint_K (op)
                       || satisfies_constraint_P (op)
                       || satisfies_constraint_J (op)"))
     (match_code "reg")))

(define_predicate "metag_16bit_op"
(and (match_code "const_int")
     (match_test "-32768 <= INTVAL (op) && INTVAL (op) <= 32767")))

(define_predicate "metag_vector_float_op"
(match_code "const_vector")
{
  int nunits = GET_MODE_NUNITS (mode);

  if (GET_CODE (op) == CONST_VECTOR
      && CONST_VECTOR_NUNITS (op) == nunits)
    {
      int i;

      for (i = 0; i < nunits; ++i)
        {
          rtx x = CONST_VECTOR_ELT (op, i);

          if (!metag_fphalf_imm_op (x, GET_MODE_INNER (mode)))
            return false;
        }
      return true;
    }
  return false;
})

(define_predicate "metag_vector_int_op"
(match_code "const_vector")
{
  int nunits = GET_MODE_NUNITS (mode);

  if (GET_CODE (op) == CONST_VECTOR
      && CONST_VECTOR_NUNITS (op) == nunits)
    {
      int i;

      for (i = 0; i < nunits; ++i)
        {
          rtx x = CONST_VECTOR_ELT (op, i);

          if (!CONST_INT_P (x))
            return false;
        }
      return true;
    }
  return false;
})

(define_predicate "metag_vector_16bit_op"
  (match_code "const_vector")
{
  int nunits = GET_MODE_NUNITS (mode);

  if (GET_CODE (op) == CONST_VECTOR
      && CONST_VECTOR_NUNITS (op) == nunits)
    {
      int i;
      HOST_WIDE_INT val = INTVAL (CONST_VECTOR_ELT (op, 0));

      for (i = 0; i < nunits; ++i)
        {
          rtx x = CONST_VECTOR_ELT (op, i);

          if (!metag_16bit_op (x, GET_MODE_INNER (mode)))
            return false;
          if (val != INTVAL (x))
            return false;
        }
      return true;
    }
  return false;
})

(define_predicate "metag_5bit_op"
(and (match_code "const_int")
     (match_test "0 <= INTVAL (op) && INTVAL (op) <= 31")))

(define_predicate "metag_vector_5bit_op"
  (match_code "const_vector")
{
  int nunits = GET_MODE_NUNITS (mode);

  if (GET_CODE (op) == CONST_VECTOR
      && CONST_VECTOR_NUNITS (op) == nunits)
    {
      int i;
      HOST_WIDE_INT val = INTVAL (CONST_VECTOR_ELT (op, 0));

      for (i = 0; i < nunits; ++i)
        {
          rtx x = CONST_VECTOR_ELT (op, i);

          if (!metag_5bit_op (x, GET_MODE_INNER (mode)))
            return false;
          if (val != INTVAL (x))
            return false;
        }
      return true;
    }
  return false;
})

(define_predicate "metag_K_operand"
(and (match_code "const_int")
     (match_test "satisfies_constraint_K (op)")))
            
(define_predicate "metag_L_operand"
(and (match_code "const_int")
     (match_test "satisfies_constraint_L (op)")))

(define_predicate "metag_J_operand"
(and (match_code "const_int")
     (match_test "satisfies_constraint_J (op)")))

(define_predicate "metag_O0_operand"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O0 (op)")))

(define_predicate "metag_O3_operand"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O3 (op)")))
            
(define_predicate "metag_P_operand"
(and (match_code "const_int")
     (match_test "satisfies_constraint_P (op)")))

(define_predicate "metag_KP_operand"
(and (match_code "const_int")
     (ior (match_test "satisfies_constraint_K (op)")
          (match_test "satisfies_constraint_P (op)"))))

(define_predicate "metag_KIP_operand"
(and (match_code "const_int")
     (ior (match_test "satisfies_constraint_K (op)")
          (ior (match_test "satisfies_constraint_P (op)")
               (match_test "satisfies_constraint_I (op)")))))

(define_predicate "metag_pic_reg"
(and (match_code "reg")
     (ior (match_test "op == pic_offset_table_rtx")
          (and (match_code "reg")
               (match_test "REGNO (op) == PIC_OFFSET_TABLE_REGNUM")))))

(define_predicate "metag_offset12_qi"
(and (match_code "const_int")
     (match_test "satisfies_constraint_Z1 (op)")))

(define_predicate "metag_offset12_hi"
(and (match_code "const_int")
     (match_test "satisfies_constraint_Z2 (op)")))

(define_predicate "metag_offset12_si"
(and (match_code "const_int")
     (match_test "satisfies_constraint_Z4 (op)")))

(define_predicate "metag_offset12_di"
(and (match_code "const_int")
     (match_test "satisfies_constraint_Z8 (op)")))

(define_predicate "metag_offset12_sf"
(and (match_code "const_int")
     (match_test "satisfies_constraint_Z4 (op)")))

(define_predicate "metag_offset12_df"
(and (match_code "const_int")
     (match_test "satisfies_constraint_Z8 (op)")))

(define_predicate "metag_offset12_v2si"
(and (match_code "const_int")
     (match_test "satisfies_constraint_Z8 (op)")))

(define_predicate "metag_offset12_v2sf"
(and (match_code "const_int")
     (match_test "satisfies_constraint_Z8 (op)")))

(define_predicate "metag_offset6_qi"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O1 (op)")))

(define_predicate "metag_offset6_hi"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O2 (op)")))

(define_predicate "metag_offset6_si"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O4 (op)")))

(define_predicate "metag_offset6_di"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O8 (op)")))

(define_predicate "metag_offset6_sf"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O4 (op)")))

(define_predicate "metag_offset6_df"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O8 (op)")))

(define_predicate "metag_offset6_v2si"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O8 (op)")))

(define_predicate "metag_offset6_v2sf"
(and (match_code "const_int")
     (match_test "satisfies_constraint_O8 (op)")))

(define_predicate "metag_reg12bit_op"
(match_code  "subreg,reg")
{
  if (metag_register_op (op, mode))
    {
      if (SUBREG_P (op))
        op = SUBREG_REG (op);

      if (REG_P (op))
        {
          unsigned regno = REGNO (op);

          if (regno == INVALID_REGNUM)
            return false;
          else if (regno >= FIRST_PSEUDO_REGISTER)
            return !reload_completed;
          else if (metag_regno12bit_p (regno))
            return true;
          else if (!reload_completed && !reload_in_progress)
            return (regno == FRAME_REG || regno == ARGP_REG);
        }
    }

  return false;
})

(define_predicate "metag_fphalf_imm_op"
(match_code "const_double")
{
  bool inexact = false;

  if (mode != SFmode && mode != DFmode)
    return false;

  metag_const_double_to_hp (op, &inexact);
  return !inexact;
})

(define_predicate "metag_fpreg_or_imm_op"
(ior (and (match_code "subreg,reg")
          (match_operand 0 "metag_register_op"))
     (match_code "const_double"))
{
  unsigned int regno;

  if (SUBREG_P (op))
    {
      op = SUBREG_REG (op);

      /* A subreg may refer to a mem before reload_completed.
       * it's treated as a register because reload will reload it
       * into one if required. */
      if (!reload_completed && MEM_P (op))
        return true;
    }

  if (!REG_P(op))
    return GET_CODE (op) == CONST_DOUBLE
           && GET_MODE_CLASS (mode) == MODE_FLOAT;

  regno = REGNO (op);

  if (regno >= FIRST_PSEUDO_REGISTER)
    return true;

  return METAG_FPC_REG_P (regno);
})

(define_predicate "metag_fpreg_or_fpzero_imm_op"
(ior (and (match_code "subreg,reg")
          (match_operand 0 "metag_register_op"))
     (match_code "const_double"))
{
  unsigned int regno;

  if (SUBREG_P (op)) {
    op = SUBREG_REG (op);

    /* A subreg may refer to a mem before reload_completed.
     * it's treated as a register because reload will reload it
     * into one if required. */
    if (!reload_completed && MEM_P (op))
      return true;
  }

  if (!REG_P (op))
    return GET_CODE (op) == CONST_DOUBLE
           && GET_MODE_CLASS (mode) == MODE_FLOAT
           && op == CONST0_RTX (mode);

  regno = REGNO (op);

  if (regno >= FIRST_PSEUDO_REGISTER)
    return true;

  return METAG_FPC_REG_P (regno);
})

(define_predicate "metag_fpone_imm_op"
  (match_code "const_double")
{
  return satisfies_constraint_H (op);
})
