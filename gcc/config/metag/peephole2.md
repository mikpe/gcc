;; Machine description for GNU compiler,
;; Imagination Technologies Meta version.
;; Copyright (C) 2007
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


;; ====PRE_INC

;; ----------------------------------------------------------------------------
;; Recognising DI/SI/HI/QI store pre-inc/dec/modify
;; ----------------------------------------------------------------------------

(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op" "")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))
   (set (match_operand:MODES       2 "memory_operand"    "")
        (match_operand:<MODE>      3 "metag_register_op" ""))]
  "metag_same_regclass_p (operands[0], operands[1])
   && !metag_same_regclass_p (operands[0], operands[3])
   && rtx_equal_p (operands[0], XEXP (operands[2], 0))
   && (GET_MODE_SIZE (<MODE>mode) <= UNITS_PER_WORD
       || !metag_same_regclass_p (operands[0], gen_rtx_REG (SImode, REGNO (operands[3]) + 1)))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    rtx mem        = gen_rtx_MEM (<MODE>mode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[3]));

    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[2]);
    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
)

(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op" "")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_<mode>" "")))
   (set (match_operand:MODES       2 "memory_operand"       "")    
        (match_operand:<MODE>      3 "metag_register_op" ""))]
  "rtx_equal_p (operands[0], XEXP (operands[2], 0))"
  [(const_int 0)]
  {
    rtx pre, mem, insn;

    if (INTVAL (operands[1]) == GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_INC (SImode, operands[0]);
    else if (INTVAL (operands[1]) == -GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[1]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
      }

    mem  = gen_rtx_MEM (<MODE>mode, pre);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[2]);
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[3]));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
)

;; ----------------------------------------------------------------------------

;; ----------------------------------------------------------------------------
;; Recognising DI/SI/HI/QI load pre-inc/dec/modify
;; ----------------------------------------------------------------------------

(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op" "")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))
   (set (match_operand:<MODE>      2 "metag_register_op" "")
        (match_operand:MODES       3 "memory_operand"    ""))]
  "metag_same_regclass_p (operands[0], operands[1])
   && rtx_equal_p (operands[0], XEXP (operands[3], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    rtx mem        = gen_rtx_MEM (<MODE>mode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[2], mem));

    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
)

(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op"    "")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_<mode>" "")))
   (set (match_operand:<MODE>      2 "metag_register_op"    "")
        (match_operand:MODES       3 "memory_operand"       ""))]
  "rtx_equal_p (operands[0], XEXP (operands[3], 0))"
  [(const_int 0)]
  {
    rtx pre, mem, insn;

    if (INTVAL (operands[1]) == GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_INC (SImode, operands[0]);
    else if (INTVAL (operands[1]) == -GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[1]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
      }

    mem  = gen_rtx_MEM (<MODE>mode, pre);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);
    insn = emit_insn (gen_rtx_SET (VOIDmode, operands[2], mem));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
)

;; ----------------------------------------------------------------------------

;; ----------------------------------------------------------------------------
;; Recognising zero extend SI load pre-inc/dec/modify
;; ----------------------------------------------------------------------------

(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op" "")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))
   (set (match_operand:SI          2 "metag_register_op" "")
        (zero_extend:SI
          (match_operand:EXTSI     3 "memory_operand"    "")))]
  "metag_same_regclass_p (operands[0], operands[1])
   && rtx_equal_p (operands[0], XEXP (operands[3], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    rtx mem        = gen_rtx_MEM (<MODE>mode, pre_modify);
    rtx zextend    = gen_rtx_ZERO_EXTEND (SImode, mem);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[2], zextend));

    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
)

(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op"    "")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_<mode>" "")))
   (set (match_operand:SI          2 "metag_register_op"    "")
        (zero_extend:SI
          (match_operand:EXTSI     3 "memory_operand"       "")))]
  "rtx_equal_p (operands[0], XEXP (operands[3], 0))"
  [(const_int 0)]
  {
    rtx pre, mem, zextend, insn;

    if (INTVAL (operands[1]) == GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_INC (SImode, operands[0]);
    else if (INTVAL (operands[1]) == -GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[1]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
      }

    mem     = gen_rtx_MEM (<MODE>mode, pre);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);
    zextend = gen_rtx_ZERO_EXTEND (SImode, mem);
    insn    = emit_insn (gen_rtx_SET (VOIDmode, operands[2], zextend));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
)

;; ----------------------------------------------------------------------------
;; Recognising zero extend HI load pre-inc/dec/modify
;; ----------------------------------------------------------------------------

(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op" "")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op" "")))
   (set (match_operand:HI          2 "metag_register_op" "")
        (zero_extend:HI 
          (match_operand:EXTHI     3 "memory_operand"    "")))]
  "metag_same_regclass_p (operands[0], operands[1])
   && rtx_equal_p (operands[0], XEXP (operands[3], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    rtx mem        = gen_rtx_MEM (<MODE>mode, pre_modify);
    rtx zextend    = gen_rtx_ZERO_EXTEND (HImode, mem);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[2], zextend));

    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
)

(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op"    "")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_<mode>" "")))
   (set (match_operand:HI          2 "metag_register_op"    "")
        (zero_extend:HI
          (match_operand:EXTHI     3 "memory_operand"       "")))]
  "rtx_equal_p (operands[0], XEXP (operands[3], 0))"
  [(const_int 0)]
  {
    rtx pre, mem, zextend, insn;

    if (INTVAL (operands[1]) == GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_INC (SImode, operands[0]);
    else if (INTVAL (operands[1]) == -GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[1]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
      }
  
    mem     = gen_rtx_MEM (<MODE>mode, pre);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);
    zextend = gen_rtx_ZERO_EXTEND (HImode, mem);
    insn    = emit_insn (gen_rtx_SET (VOIDmode, operands[2], zextend));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
)
;; ====PRE_INC

;; ====POST_INC

;; ----------------------------------------------------------------------------
;; Recognising DF/SF/DI/SI/HI/QI store post-inc/dec/modify
;; ----------------------------------------------------------------------------

(define_peephole2
  [(set (match_operand:MODES         0 "memory_operand"    "")
        (match_operand:<MODE>        1 "metag_register_op" ""))
   (set (match_operand:SI            2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_operand:SI   3 "metag_register_op" "")))]
  "metag_same_regclass_p (operands[3], operands[2])
   && !metag_same_regclass_p (operands[2], operands[1])
   && rtx_equal_p (operands[2], XEXP (operands[0], 0))
   && (GET_MODE_SIZE (<MODE>mode) <= UNITS_PER_WORD
       || !metag_same_regclass_p (operands[2], gen_rtx_REG (SImode, REGNO (operands[1]) + 1)))"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[2], operands[3]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[2], plus);
    rtx mem         = gen_rtx_MEM (<MODE>mode, post_modify);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[0]);

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
)

(define_peephole2
  [(set (match_operand:MODES         0 "memory_operand"       "")
        (match_operand:<MODE>        1 "metag_register_op"    ""))
   (set (match_operand:SI            2 "metag_register_op"    "")
        (plus:SI (match_dup 2)
                 (match_operand:SI   3 "metag_offset6_<mode>" "")))]
  "rtx_equal_p (operands[2], XEXP (operands[0], 0))"
  [(const_int 0)]
  {
    rtx post, mem, insn;

    if (INTVAL (operands[3]) == GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_INC (SImode, operands[2]);
    else if (INTVAL (operands[3]) == -GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_DEC (SImode, operands[2]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[2], operands[3]);

        post = gen_rtx_POST_MODIFY (SImode, operands[2], plus);
      }

    mem  = gen_rtx_MEM (<MODE>mode, post);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[0]);
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
)

;; ----------------------------------------------------------------------------

;; ----------------------------------------------------------------------------
;; Recognising DI/SI/HI/QI load post-inc/dec/modify
;; ----------------------------------------------------------------------------

(define_peephole2
  [(set (match_operand:<MODE>        0 "metag_register_op" "")
        (match_operand:MODES         1 "memory_operand"    ""))
   (set (match_operand:SI            2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_operand:SI   3 "metag_register_op" "")))]
  "metag_same_regclass_p (operands[3], operands[2])
   && rtx_equal_p (operands[2], XEXP (operands[1], 0))
   && REGNO (operands[0]) != REGNO (operands[3])
   && REGNO (operands[0]) != REGNO (operands[2])"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[2], operands[3]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[2], plus);
    rtx mem         = gen_rtx_MEM (<MODE>mode, post_modify);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[1]);

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
)

(define_peephole2
  [(set (match_operand:<MODE>        0 "metag_register_op"    "")
        (match_operand:MODES         1 "memory_operand"       ""))
   (set (match_operand:SI            2 "metag_register_op"    "") 
        (plus:SI (match_dup 2)
                 (match_operand:SI   3 "metag_offset6_<mode>" "")))]
  "REGNO (operands[0]) != REGNO (operands[2])
   && rtx_equal_p (operands[2], XEXP (operands[1], 0))"
  [(const_int 0)]
  {
    rtx post, mem, insn;

    if (INTVAL (operands[3]) == GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_INC (SImode, operands[2]);
    else if (INTVAL (operands[3]) == -GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_DEC (SImode, operands[2]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[2], operands[3]);

        post = gen_rtx_POST_MODIFY (SImode, operands[2], plus);
      }

    mem  = gen_rtx_MEM (<MODE>mode, post);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[1]);
    insn = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
)

;; ----------------------------------------------------------------------------

;; ----------------------------------------------------------------------------
;; Recognising zero extend SI load post-modify
;; ----------------------------------------------------------------------------

(define_peephole2
  [(set (match_operand:SI              0 "metag_register_op" "")
        (zero_extend:SI 
          (match_operand:EXTSI         1 "memory_operand"    "")))
   (set (match_operand:SI              2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_operand:SI     3 "metag_register_op" "")))]
  "metag_same_regclass_p (operands[3], operands[2])
   && REGNO (operands[0]) != REGNO (operands[3])
   && REGNO (operands[0]) != REGNO (operands[2])
   && rtx_equal_p (operands[2], XEXP (operands[1], 0))"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[2], operands[3]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[2], plus);
    rtx mem         = gen_rtx_MEM (<MODE>mode, post_modify);
    rtx zextend     = gen_rtx_ZERO_EXTEND (SImode, mem);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, operands[0], zextend));

    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[1]);

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
)

(define_peephole2
  [(set (match_operand:SI              0 "metag_register_op"   "")
        (zero_extend:SI
          (match_operand:EXTSI         1 "memory_operand"      "")))
   (set (match_operand:SI              2 "metag_register_op"   "")
        (plus:SI (match_dup 2)
                 (match_operand:SI     3 "metag_offset6_<mode>" "")))]
  "REGNO (operands[0]) != REGNO (operands[2])
   && rtx_equal_p (operands[2], XEXP (operands[1], 0))"
  [(const_int 0)]
  {
    rtx post, mem, zextend, insn;

    if (INTVAL (operands[3]) == GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_INC (SImode, operands[2]);
    else if (INTVAL (operands[3]) == -GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_DEC (SImode, operands[2]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[2], operands[3]);

        post = gen_rtx_POST_MODIFY (SImode, operands[2], plus);
      }

    mem     = gen_rtx_MEM (<MODE>mode, post);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[1]); 
    zextend = gen_rtx_ZERO_EXTEND (SImode, mem);
    insn    = emit_insn (gen_rtx_SET (VOIDmode, operands[0], zextend));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
)

;; ----------------------------------------------------------------------------
;; Recognising zero extend HI load post-inc/dec/modify
;; ----------------------------------------------------------------------------

(define_peephole2
  [(set (match_operand:HI              0 "metag_register_op" "")
        (zero_extend:HI 
          (match_operand:EXTHI         1 "memory_operand"    "")))
   (set (match_operand:SI              2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_operand:SI     3 "metag_register_op" "")))]
  "metag_same_regclass_p (operands[3], operands[2])
   && REGNO (operands[0]) != REGNO (operands[3])
   && REGNO (operands[0]) != REGNO (operands[2])
   && rtx_equal_p (operands[2], XEXP (operands[1], 0))"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[2], operands[3]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[2], plus);
    rtx mem         = gen_rtx_MEM (<MODE>mode, post_modify);
    rtx zextend     = gen_rtx_ZERO_EXTEND (HImode, mem);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, operands[0], zextend));

    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[1]);

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
)

(define_peephole2
  [(set (match_operand:HI              0 "metag_register_op"    "")
        (zero_extend:HI
          (match_operand:EXTHI         1 "memory_operand"       "")))
   (set (match_operand:SI              2 "metag_register_op"    "")
        (plus:SI (match_dup 2)
                 (match_operand:SI     3 "metag_offset6_<mode>" "")))]
  "REGNO (operands[0]) != REGNO (operands[2])
   && rtx_equal_p (operands[2], XEXP (operands[1], 0))"
  [(const_int 0)]
  {
    rtx post, mem, zextend, insn;

    if (INTVAL (operands[3]) == GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_INC (SImode, operands[2]);
    else if (INTVAL (operands[3]) == -GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_DEC (SImode, operands[2]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[2], operands[3]);

        post = gen_rtx_POST_MODIFY (SImode, operands[2], plus);
      }

    mem     = gen_rtx_MEM (<MODE>mode, post);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[1]);
    zextend = gen_rtx_ZERO_EXTEND (HImode, mem);
    insn    = emit_insn (gen_rtx_SET (VOIDmode, operands[0], zextend));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
)

;; ====POST_INC

;; ----------------------------------------------------------------------------
;; Fixup some obvious reg alloc losage for loads
;; ----------------------------------------------------------------------------

(define_peephole2
  [(set (match_operand:MEMOP  0 "metag_register_op"    "")
        (match_operand:<MODE> 1 "memory_operand"       ""))
   (set (match_operand:<MODE> 2 "metag_reg_nofloat_op" "")
        (match_dup 0))]
  "peep2_reg_dead_p (2, operands[0])"
  [(set (match_dup 2)
        (match_dup 1))]
  "")

;; ----------------------------------------------------------------------------

;; misc peephole2s

(define_peephole2
  [(set (match_operand:SI   0 "metag_register_op" "")
        (match_operand:SI   1 "metag_datareg_op"  ""))
   (set (reg:CCANY CC_REG)
        (compare:<MODE>
          (match_dup 0)
          (match_operand:SI 2 "metag_int_operand" "")))]
  "peep2_reg_dead_p (2, operands[0])"
  [(set (reg:<MODE> CC_REG)
        (compare:<MODE>
          (match_dup 1)
          (match_dup 2)))]
  "")

;;
(define_peephole2
  [(set (match_operand:SI   0 "metag_register_op" "")
        (match_operand:SI   1 "metag_datareg_op"  ""))
   (set (reg:CCANY CC_REG)
        (compare:<MODE>
          (match_dup 0)
          (match_operand:SI 2 "metag_datareg_op"  "")))]
  "peep2_reg_dead_p (2, operands[0])
   && metag_same_regclass_p (operands[0], operands[1])
   && !rtx_equal_p (operands[0], operands[2])"
  [(set (reg:<MODE> CC_REG)
        (compare:<MODE>
          (match_dup 1)
          (match_dup 2)))]
  "")

;; SImode swap
(define_peephole2
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "")
        (match_operand:SI 1 "metag_reg_nofloat_op" ""))
   (set (match_dup 1)
        (match_operand:SI 2 "metag_reg_nofloat_op" ""))
   (set (match_dup 2)
        (match_dup 0))]
  "!metag_same_regclass_p (operands[1], operands[2])
   && peep2_reg_dead_p (3, operands[0])"
  [(parallel
    [(set (match_dup 1)
          (match_dup 2))
     (set (match_dup 2)
          (match_dup 1))])]
  "")

;; DImode swap
(define_peephole2
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "")
        (match_operand:SI 1 "metag_reg_nofloat_op" ""))
   (set (match_operand:SI 2 "metag_reg_nofloat_op" "")
        (match_operand:SI 3 "metag_reg_nofloat_op" ""))
   (set (match_dup 1)
        (match_operand:SI 4 "metag_reg_nofloat_op" ""))
   (set (match_dup 3)
        (match_operand:SI 5 "metag_reg_nofloat_op" ""))
   (set (match_dup 4)
        (match_dup 0))
   (set (match_dup 5)
        (match_dup 2))]
  "   !metag_same_regclass_p (operands[3], operands[4])
   && !metag_same_regclass_p (operands[1], operands[5])
   && !metag_same_regclass_p (operands[4], operands[5])
   && !metag_same_regclass_p (operands[1], operands[3])
   && peep2_reg_dead_p (5, operands[0])
   && peep2_reg_dead_p (6, operands[2])"
  [(parallel
     [(set (match_dup 3)
           (match_dup 4))
      (set (match_dup 4)
           (match_dup 3))])
   (parallel
     [(set (match_dup 1)
           (match_dup 5))
      (set (match_dup 5)
           (match_dup 1))])
   (parallel
     [(set (match_dup 4)
           (match_dup 5))
      (set (match_dup 5)
           (match_dup 4))])
   (parallel
     [(set (match_dup 1)
           (match_dup 3))
      (set (match_dup 3)
           (match_dup 1))])]
  "")

(define_peephole2
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "")
        (match_operand:SI 1 "metag_reg_nofloat_op" ""))
   (set (match_dup 1)
        (match_operand:SI 2 "metag_reg_nofloat_op" ""))
   (set (match_dup 2)
        (match_dup 0))]
  "!metag_same_regclass_p (operands[1], operands[2])"
  [(set (match_dup 0)
        (match_dup 1))
   (parallel
    [(set (match_dup 1)
          (match_dup 2))
     (set (match_dup 2)
          (match_dup 1))])]
  "")

;; set full condition flags during move, flags from source value
(define_peephole2
  [(set (match_operand:SI 0 "metag_register_op" "")
        (match_operand:SI 1 "metag_datareg_op"  ""))
   (set (reg:CCANY CC_REG)
        (compare:<MODE>
          (match_dup 1)
          (const_int 0)))]
  "REGNO (operands[0]) <= LAST_ADDR_REG"
  [(parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (match_dup 1)
            (const_int 0)))
     (set (match_dup 0)
          (match_dup 1))])]
   "")

;; set full condition flags during move, flags from dest value
(define_peephole2
  [(set (match_operand:SI 0 "metag_register_op" "")
        (match_operand:SI 1 "metag_datareg_op"  ""))
   (set (reg:CCANY CC_REG)
        (compare:<MODE>
          (match_dup 0)
          (const_int 0)))]
  "REGNO (operands[0]) <= LAST_ADDR_REG"
  [(parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (match_dup 1)
            (const_int 0)))
     (set (match_dup 0)
          (match_dup 1))])]
  "")

;; set condition flags during sign extension of a hi value
(define_peephole2
  [(set (match_operand:SI                 0 "metag_register_op" "")
        (sign_extend:SI (match_operand:HI 1 "metag_register_op" "")))
   (set (reg:CCZNC CC_REG)
        (compare:<MODE>
          (match_dup 0)
          (const_int 0)))]
  ""
  [(parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (sign_extend:SI (match_dup 1))
            (const_int 0)))
     (set (match_dup 0)
          (sign_extend:SI (match_dup 1)))])]
  "")

;; set condition flags during sign extension of a qi value
(define_peephole2
  [(set (match_operand:SI                 0 "metag_register_op" "")
        (sign_extend:SI (match_operand:QI 1 "metag_register_op" "")))
   (set (reg:CCZNC CC_REG)
        (compare:<MODE>
          (match_dup 0)
          (const_int 0)))]
  ""
  [(parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (sign_extend:SI (match_dup 1))
            (const_int 0)))
     (set (match_dup 0)
          (sign_extend:SI (match_dup 1)))])]
  "")

;; eliminate redundant move
(define_peephole2
  [(set (match_operand:MEMOP  0 "metag_register_op" "")
        (match_operand:<MODE> 1 "metag_regorint_op" ""))
   (set (match_operand:<MODE> 2 "metag_register_op" "")
        (match_dup 0))]
  "peep2_reg_dead_p (2, operands[0])
   && metag_move_valid_p (operands[2], operands[1])"
  [(set (match_dup 2)
        (match_dup 1))]
  "")

;;
(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op" "")
        (plus:SI (match_operand:SI 1 "metag_datareg_op"  "")
                 (match_operand:SI 2 "metag_register_op" "")))
   (set (reg:CCZNC CC_REG)
        (compare:<MODE>
          (match_dup 0)
          (const_int 0)))]
  ""
  [(parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (plus:SI (match_dup 1)
                     (match_dup 2))
            (const_int 0)))
    (set (match_dup 0)
         (plus:SI (match_dup 1)
                  (match_dup 2)))])]
  "")

;;
(define_peephole2
  [(set (match_operand:SI           0 "metag_register_op" "")
        (minus:SI (match_operand:SI 1 "metag_datareg_op"  "")
                  (match_operand:SI 2 "metag_register_op" "")))
   (set (reg:CCZNC CC_REG)
        (compare:<MODE>
           (match_dup 0)
           (const_int 0)))]
  ""
  [(parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (minus:SI (match_dup 1)
                      (match_dup 2))
            (const_int 0)))
     (set (match_dup 0)
          (minus:SI (match_dup 1)
                    (match_dup 2)))])]
  "")
;;
(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op" "")
        (plus:SI (match_operand:SI 1 "metag_datareg_op"  "")
                 (match_operand:SI 2 "metag_smallint_op" "")))
   (set (reg:CCZNC CC_REG)
        (compare:<MODE>
           (match_dup 0)
           (const_int 0)))]
  ""
  [(parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (plus:SI (match_dup 1)
                     (match_dup 2))
            (const_int 0)))
     (set (match_dup 0)
          (plus:SI (match_dup 1)
                   (match_dup 2)))])]
  "")

(define_peephole2
  [(set (match_operand:SI          0 "metag_register_op" "")
        (high:SI (match_operand:SI 1 "metag_symglobal_op" "")))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 2 "const_int_operand" "")))]
  "!METAG_FLAG_PIC"
  [(set (match_dup 0)
        (high:SI (match_dup 3)))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 3)))]
  "operands[3] = gen_rtx_CONST (SImode,
                                gen_rtx_PLUS (SImode,
                                              operands[1],
                                              operands[2]));")

(define_peephole2
  [(set (match_operand:SI                             0 "metag_register_op"  "")
        (high:SI (const:SI (plus:SI (match_operand:SI 1 "metag_symglobal_op" "")
                                    (match_operand:SI 2 "const_int_operand"  "")))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const:SI (plus:SI (match_dup 1)
                                      (match_dup 2)))))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI                    3 "const_int_operand"  "")))]
  "!METAG_FLAG_PIC"
  [(set (match_dup 0)
        (high:SI (match_dup 4)))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 4)))]
  "operands[4] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]));
   operands[4] = gen_rtx_CONST (SImode,
                                gen_rtx_PLUS (SImode,
                                              operands[1],
                                              operands[4]));")

;; Combine a load/store with pre address arithmetic into 
;;         a load/store with base + offset addressing.
;; Where the intermidiate address register dies in the load/store

;; loads
(define_peephole2
  [(set (match_operand:SI           0 "metag_register_op"    "")
        (plus:SI (match_operand:SI  1 "metag_register_op"    "")
                 (match_operand:SI  2 "metag_offset6_<mode>" "")))
   (set (match_operand:<MODE>       3 "metag_register_op"    "")
        (match_operand:MEMOP        4 "memory_operand"       ""))]
  "peep2_reg_dead_p (2, operands[0])
   && rtx_equal_p (operands[0], XEXP (operands[4], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    emit_insn (gen_rtx_SET (VOIDmode, operands[3], mem));

    DONE;
  }
)

(define_peephole2
  [(set (match_operand:SI           0 "metag_register_op"     "")
        (plus:SI (match_operand:SI  1 "metag_reg12bit_op"     "")
                 (match_operand:SI  2 "metag_offset12_<mode>" "")))
   (set (match_operand:<MODE>       3 "metag_reg_nofloat_op"  "")
        (match_operand:MEMOP        4 "memory_operand"        ""))]
  "peep2_reg_dead_p (2, operands[0])
   && rtx_equal_p (operands[0], XEXP (operands[4], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    emit_insn (gen_rtx_SET (VOIDmode, operands[3], mem));

    DONE;
  }
)

;; load zero_extend HI
(define_peephole2
  [(set (match_operand:SI           0 "metag_register_op"    "")
        (plus:SI (match_operand:SI  1 "metag_register_op"    "")
                 (match_operand:SI  2 "metag_offset6_<mode>" "")))
   (set (match_operand:HI           3 "metag_register_op"    "")
        (zero_extend:HI 
          (match_operand:EXTHI      4 "memory_operand"       "")))]
  "peep2_reg_dead_p (2, operands[0])
   && rtx_equal_p (operands[0], XEXP (operands[4], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx mem        = gen_rtx_MEM (<MODE>mode, plus);
    rtx zextend    = gen_rtx_ZERO_EXTEND (HImode, mem);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    emit_insn (gen_rtx_SET (VOIDmode, operands[3], zextend));

    DONE;
  }
)

(define_peephole2
  [(set (match_operand:SI           0 "metag_register_op"     "")
        (plus:SI (match_operand:SI  1 "metag_reg12bit_op"     "")
                 (match_operand:SI  2 "metag_offset12_<mode>" "")))
   (set (match_operand:HI           3 "metag_reg_nofloat_op"  "")
        (zero_extend:HI 
          (match_operand:EXTHI      4 "memory_operand"        "")))]
  "peep2_reg_dead_p (2, operands[0])
   && rtx_equal_p (operands[0], XEXP (operands[4], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx mem        = gen_rtx_MEM (<MODE>mode, plus);
    rtx zextend    = gen_rtx_ZERO_EXTEND (HImode, mem);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    emit_insn (gen_rtx_SET (VOIDmode, operands[3], zextend));

    DONE;
  }
)

;; load zero_extend SI
(define_peephole2
  [(set (match_operand:SI           0 "metag_register_op"    "")
        (plus:SI (match_operand:SI  1 "metag_register_op"    "")
                 (match_operand:SI  2 "metag_offset6_<mode>" "")))
   (set (match_operand:SI           3 "metag_register_op"    "")
        (zero_extend:SI
          (match_operand:EXTSI      4 "memory_operand"       "")))]
  "peep2_reg_dead_p (2, operands[0])
   && rtx_equal_p (operands[0], XEXP (operands[4], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx mem        = gen_rtx_MEM (<MODE>mode, plus);
    rtx zextend    = gen_rtx_ZERO_EXTEND (SImode, mem);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    emit_insn (gen_rtx_SET (VOIDmode, operands[3], zextend));

    DONE;
  }
)

(define_peephole2
  [(set (match_operand:SI           0 "metag_register_op"     "")
        (plus:SI (match_operand:SI  1 "metag_reg12bit_op"     "")
                 (match_operand:SI  2 "metag_offset12_<mode>" "")))
   (set (match_operand:SI           3 "metag_reg_nofloat_op"  "")
        (zero_extend:SI
          (match_operand:EXTSI      4 "memory_operand"        "")))]
  "peep2_reg_dead_p (2, operands[0])
   && rtx_equal_p (operands[0], XEXP (operands[4], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx mem        = gen_rtx_MEM (<MODE>mode, plus);
    rtx zextend    = gen_rtx_ZERO_EXTEND (SImode, mem);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    emit_insn (gen_rtx_SET (VOIDmode, operands[3], zextend));

    DONE;
  }
)

;; store QI/HI/SI
(define_peephole2
  [(set (match_operand:SI                0 "metag_register_op"    "")
        (plus:SI (match_operand:SI       1 "metag_register_op"    "")
                 (match_operand:SI       2 "metag_offset6_<mode>" "")))
   (set (match_operand:MEMOP             3 "memory_operand"       "")
                   (match_operand:<MODE> 4 "metag_register_op"    ""))]
  "peep2_reg_dead_p (2, operands[0])
   && REGNO (operands[0]) != REGNO (operands[4])
   && rtx_equal_p (operands[0], XEXP (operands[3], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);
    emit_insn (gen_rtx_SET (VOIDmode, mem, operands[4]));

    DONE;
  }
)

(define_peephole2
  [(set (match_operand:SI                0 "metag_register_op"     "")
        (plus:SI (match_operand:SI       1 "metag_reg12bit_op"     "")
                 (match_operand:SI       2 "metag_offset12_<mode>" "")))
   (set (match_operand:MEMOP             3 "memory_operand"        "")
                   (match_operand:<MODE> 4 "metag_reg_nofloat_op"  ""))]
  "peep2_reg_dead_p (2, operands[0])
   && REGNO (operands[0]) != REGNO (operands[4])
   && rtx_equal_p (operands[0], XEXP (operands[3], 0))"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);
    emit_insn (gen_rtx_SET (VOIDmode, mem, operands[4]));

    DONE;
  }
)

;; QI/HI->SI zero_extend load removing unneccessary temporary

(define_peephole2
  [(set (match_operand:SI                       0 "metag_register_op" "")
        (high:SI (match_operand:SI              1 "metag_symglobal_op" "")))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))
   (set (match_operand:SI                       2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_dup 0)))
   (set (match_operand:SI                       3 "metag_register_op" "")
        (zero_extend:SI
          (match_operand:EXTSI                  4 "memory_operand"    "")))]
  "!METAG_FLAG_PIC
   && peep2_reg_dead_p (4, operands[2])
   && peep2_reg_dead_p (3, operands[0])
   && GET_MODE (XEXP (operands[4], 0)) == SImode
   && GET_CODE (XEXP (operands[4], 0)) == PLUS
   && rtx_equal_p (operands[2], XEXP (XEXP (operands[4], 0), 0))
   && const_int_operand (XEXP (XEXP (operands[4], 0), 1), SImode)
   && metag_regno_same_unit_p (REGNO (operands[2]), REGNO (operands[0]))"

  [(const_int 0)]
  {
    rtx plus, mem, zextend;
    operands[5] = XEXP (XEXP (operands[4], 0), 1);
    operands[6] = gen_rtx_CONST (SImode,
                                 gen_rtx_PLUS (SImode,
                                               operands[1],
                                               operands[5]));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_HIGH (SImode, 
                                          operands[6])));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_LO_SUM (SImode, 
                                            operands[0], 
                                            operands[6])));

    plus       = gen_rtx_PLUS (SImode, operands[2], operands[0]);
    mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    zextend    = gen_rtx_ZERO_EXTEND (SImode, mem);
    emit_insn (gen_rtx_SET (VOIDmode, operands[3], zextend));
    DONE;
  }
)

;; QI->HI zero_extend load removing unneccessary temporary

(define_peephole2
  [(set (match_operand:SI                       0 "metag_register_op" "")
        (high:SI (match_operand:SI              1 "metag_symglobal_op" "")))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))
   (set (match_operand:SI                       2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_dup 0)))
   (set (match_operand:HI                       3 "metag_register_op" "")
        (zero_extend:HI
          (match_operand:EXTHI                  4 "memory_operand"    "")))]
  "!METAG_FLAG_PIC
   && peep2_reg_dead_p (4, operands[2])
   && peep2_reg_dead_p (3, operands[0])
   && GET_MODE (XEXP (operands[4], 0)) == SImode
   && GET_CODE (XEXP (operands[4], 0)) == PLUS
   && rtx_equal_p (operands[2], XEXP (XEXP (operands[4], 0), 0))
   && const_int_operand (XEXP (XEXP (operands[4], 0), 1), SImode)
   && metag_regno_same_unit_p (REGNO (operands[2]), REGNO (operands[0]))"
  [(const_int 0)]
  {
    rtx plus, mem, zextend;
    operands[5] = XEXP (XEXP (operands[4], 0), 1);
    operands[6] = gen_rtx_CONST (SImode,
                                 gen_rtx_PLUS (SImode,
                                               operands[1],
                                               operands[5]));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_HIGH (SImode, 
                                          operands[6])));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_LO_SUM (SImode, 
                                            operands[0], 
                                            operands[6])));

    plus       = gen_rtx_PLUS (SImode, operands[2], operands[0]);
    mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    zextend    = gen_rtx_ZERO_EXTEND (HImode, mem);
    emit_insn (gen_rtx_SET (VOIDmode, operands[3], zextend));
    DONE;
  }
)

;; QI, HI and SI mode load, removing unneccessary temporary

(define_peephole2
  [(set (match_operand:SI                      0 "metag_register_op" "")
        (high:SI (match_operand:SI             1 "metag_symglobal_op" "")))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))
   (set (match_operand:SI                      2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_dup 0)))
   (set (match_operand:<MODE>                  3 "metag_register_op" "")
        (match_operand:MEMOP                   4 "memory_operand"    ""))]
  "!METAG_FLAG_PIC
   && peep2_reg_dead_p (4, operands[2])
   && peep2_reg_dead_p (3, operands[0])
   && GET_MODE (XEXP (operands[4], 0)) == SImode
   && GET_CODE (XEXP (operands[4], 0)) == PLUS
   && rtx_equal_p (operands[2], XEXP (XEXP (operands[4], 0), 0))
   && const_int_operand (XEXP (XEXP (operands[4], 0), 1), SImode)
   && metag_regno_same_unit_p (REGNO (operands[2]), REGNO (operands[0]))"
  [(const_int 0)]
  {
    rtx plus, mem;
    operands[5] = XEXP (XEXP (operands[4], 0), 1);
    operands[6] = gen_rtx_CONST (SImode,
                                 gen_rtx_PLUS (SImode,
                                               operands[1],
                                               operands[5]));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_HIGH (SImode, 
                                          operands[6])));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_LO_SUM (SImode, 
                                            operands[0], 
                                            operands[6])));

    plus       = gen_rtx_PLUS (SImode, operands[2], operands[0]);
    mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    emit_insn (gen_rtx_SET (VOIDmode, operands[3], mem));
    DONE;
  }
)

;; QI/HI->SI zero_extend when result register same as temporary address register

(define_peephole2
  [(set (match_operand:SI                       0 "metag_register_op" "")
        (high:SI (match_operand:SI              1 "metag_symglobal_op" "")))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))
   (set (match_operand:SI                       2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_dup 0)))
   (set (match_dup 2)
        (zero_extend:SI 
          (match_operand:EXTSI                  3 "memory_operand"    "")))]
  "!METAG_FLAG_PIC
   && peep2_reg_dead_p (3, operands[0])
   && GET_MODE (XEXP (operands[3], 0)) == SImode
   && GET_CODE (XEXP (operands[3], 0)) == PLUS
   && rtx_equal_p (operands[2], XEXP (XEXP (operands[3], 0), 0))
   && const_int_operand (XEXP (XEXP (operands[3], 0), 1), SImode)
   && metag_regno_same_unit_p (REGNO (operands[2]), REGNO (operands[0]))"
  [(const_int 0)]
  {
    rtx plus, mem, zextend;
    operands[4] = XEXP (XEXP (operands[3], 0), 1);
    operands[5] = gen_rtx_CONST (SImode,
                                 gen_rtx_PLUS (SImode,
                                               operands[1],
                                               operands[4]));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_HIGH (SImode, 
                                          operands[5])));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_LO_SUM (SImode, 
                                            operands[0], 
                                            operands[5])));

    plus       = gen_rtx_PLUS (SImode, operands[2], operands[0]);
    mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);
    zextend    = gen_rtx_ZERO_EXTEND (SImode, mem);
    emit_insn (gen_rtx_SET (VOIDmode, operands[2], zextend));
    DONE;
  }
)

;; result register same as temporary address.

(define_peephole2
  [(set (match_operand:SI                     0 "metag_register_op" "")
        (high:SI (match_operand:SI            1 "metag_symglobal_op" "")))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))
   (set (match_operand:SI                     2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_dup 0)))
   (set (match_operand:<MODE>                 3 "metag_register_op" "")
        (match_operand:MEMOP                  4 "memory_operand"    ""))]

  "!METAG_FLAG_PIC
   && peep2_reg_dead_p (3, operands[0])
   && REGNO (operands[3]) == REGNO (operands[2])
   && GET_MODE (XEXP (operands[4], 0)) == SImode
   && GET_CODE (XEXP (operands[4], 0)) == PLUS
   && rtx_equal_p (operands[2], XEXP (XEXP (operands[4], 0), 0))
   && const_int_operand (XEXP (XEXP (operands[4], 0), 1), SImode)
   && metag_regno_same_unit_p (REGNO (operands[2]), REGNO (operands[0]))"
  [(const_int 0)]
  {
    rtx plus, mem;
    operands[5] = XEXP (XEXP (operands[4], 0), 1);
    operands[6] = gen_rtx_CONST (SImode,
                                 gen_rtx_PLUS (SImode,
                                               operands[1],
                                               operands[5]));
    operands[7] = gen_rtx_REG (<MODE>mode, REGNO (operands[2]));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_HIGH (SImode, 
                                          operands[6])));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_LO_SUM (SImode, 
                                            operands[0], 
                                            operands[6])));

    plus       = gen_rtx_PLUS (SImode, operands[2], operands[0]);
    mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[4]);
    emit_insn (gen_rtx_SET (VOIDmode, operands[7], mem));
    DONE;
  }
)

;; QI, HI and SI mode store, removing unneccessary temporary

(define_peephole2
  [(set (match_operand:SI                     0 "metag_register_op" "")
        (high:SI (match_operand:SI            1 "metag_symglobal_op" "")))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))
   (set (match_operand:SI                     2 "metag_register_op" "")
        (plus:SI (match_dup 2)
                 (match_dup 0)))
   (set (match_operand:MEMOP                  3 "memory_operand"    "")
        (match_operand:<MODE>                 4 "metag_register_op" "") )]
  "!METAG_FLAG_PIC
   && peep2_reg_dead_p (4, operands[2])
   && peep2_reg_dead_p (3, operands[0])
   && GET_MODE (XEXP (operands[3], 0)) == SImode
   && GET_CODE (XEXP (operands[3], 0)) == PLUS
   && rtx_equal_p (operands[2], XEXP (XEXP (operands[3], 0), 0))
   && const_int_operand (XEXP (XEXP (operands[3], 0), 1), SImode)
   && metag_regno_same_unit_p (REGNO (operands[2]), REGNO (operands[0]))
   && !metag_regno_same_unit_p (REGNO (operands[2]), REGNO (operands[4]))"
  [(const_int 0)]
  {
    operands[5] = XEXP (XEXP (operands[3], 0), 1);
    operands[6] = gen_rtx_CONST (SImode,
                                 gen_rtx_PLUS (SImode,
                                               operands[1],
                                               operands[5]));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_HIGH (SImode, 
                                          operands[6])));

    emit_insn (gen_rtx_SET (VOIDmode, 
                            operands[0], 
                            gen_rtx_LO_SUM (SImode, 
                                            operands[0], 
                                            operands[6])));

    rtx plus       = gen_rtx_PLUS (SImode, operands[2], operands[0]);
    rtx mem        = gen_rtx_MEM (<MODE>mode, plus);
    MEM_VOLATILE_P (mem) = MEM_VOLATILE_P (operands[3]);
    emit_insn (gen_rtx_SET (VOIDmode, mem, operands[4]));
    DONE;
  }
)
