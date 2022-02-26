;; Machine description for GNU compiler,
;; Imagination Technologies Meta version.
;; Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009, 2010
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

;; -----------------------------------------------------------------------------
;; | Recognising SI/HI/QI store pre-inc/dec/modify                              | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*sto_<mode>_pre_inc_dec_modify_disp_split"
  [(set (mem:MEMOP (plus:SI (match_operand:SI 0 "metag_regnofrm_op"   "+efhl")
                            (match_operand:SI 1 "metag_offset6_<mode>" "<O>")))
        (match_operand:<MODE>                 2 "metag_register_op"    "r"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 1)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
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
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[2]));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*sto_<mode>_pre_modify_reg_split"
  [(set (mem:MEMOP (plus:SI (match_operand:SI 0 "metag_regnofrm_op" "+%e,f,h,l")
                            (match_operand:SI 1 "metag_register_op"   "e,f,h,l")))
        (match_operand:<MODE>                 2 "metag_register_op"   "t,u,y,z"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 1)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    rtx mem        = gen_rtx_MEM (<MODE>mode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[2]));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising SI/HI/QI store post-inc/dec/modify                              | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*sto_<mode>_post_inc_dec_modify_disp_split"
  [(set (mem:MEMOP (match_operand:SI 0 "metag_regnofrm_op"   "+efhl"))
        (match_operand:<MODE>        1 "metag_register_op"    "r"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI   2 "metag_offset6_<mode>" "<O>")))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx post, mem, insn;
    
    if (INTVAL (operands[2]) == GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_INC (SImode, operands[0]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (<MODE>mode))
      post = gen_rtx_POST_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[2]);

        post = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
      }

    mem  = gen_rtx_MEM (<MODE>mode, post);
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*sto_<mode>_post_modify_reg_split"
  [(set (mem:MEMOP (match_operand:SI 0 "metag_regnofrm_op" "+e,f,h,l"))
        (match_operand:<MODE>        1 "metag_register_op"  "t,u,y,z"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI   2 "metag_register_op"  "e,f,h,l")))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[0], operands[2]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
    rtx mem         = gen_rtx_MEM (<MODE>mode, post_modify);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising QI/HI/SI load pre-inc/dec/modify                               | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*lod_<mode>_pre_inc_dec_modify_disp_split"
  [(set (match_operand:<MODE>                 0 "metag_register_op"    "=r")
        (mem:MEMOP (plus:SI (match_operand:SI 1 "metag_regnofrm_op"    "+efhl")
                            (match_operand:SI 2 "metag_offset6_<mode>" "<O>"))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx pre, mem, insn;

    if (INTVAL (operands[2]) == GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_INC (SImode, operands[1]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_DEC (SImode, operands[1]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[1], operands[2]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
      }

    mem        = gen_rtx_MEM (<MODE>mode, pre);
    insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*lod_<mode>_pre_modify_reg_split"
  [(set (match_operand:<MODE>                 0 "metag_register_op" "=r,r,r,r")
        (mem:MEMOP (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "+e,f,h,l")
                            (match_operand:SI 2 "metag_register_op"  "e,f,h,l"))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
    rtx mem        = gen_rtx_MEM (<MODE>mode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising DI store pre-inc/dec/modify                                   | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*sto_di_pre_inc_dec_modify_disp_split"
  [(set (mem:DI (plus:SI (match_operand:SI 0 "metag_regnofrm_op" "+efhl")
                         (match_operand:SI 1 "metag_offset6_di"   "O8")))
        (match_operand:DI                  2 "metag_register_op"  "r"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 1)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx pre, mem, insn;

    if (INTVAL (operands[1]) == GET_MODE_SIZE (DImode))
      pre = gen_rtx_PRE_INC (SImode, operands[0]);
    else if (INTVAL (operands[1]) == -GET_MODE_SIZE (DImode))
      pre = gen_rtx_PRE_DEC (SImode, operands[0]);
    else
    {
      rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[1]);

      pre = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    }

    mem  = gen_rtx_MEM (DImode, pre);
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[2]));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*sto_di_pre_modify_reg_split"
  [(set (mem:DI (plus:SI (match_operand:SI 0 "metag_regnofrm_op" "+e,f,h,l")
                         (match_operand:SI 1 "metag_register_op"  "e,f,h,l")))
        (match_operand:DI                  2 "metag_register_op"  "a,a,d,d"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 1)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    rtx mem        = gen_rtx_MEM (DImode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[2]));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------
;; | Recognising DI store post-inc/dec/modify                                   | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*sto_di_post_inc_dec_modify_disp_split"
  [(set (mem:DI (match_operand:SI  0 "metag_regnofrm_op" "+efhl"))
        (match_operand:DI          1 "metag_register_op"  "r"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 2 "metag_offset6_di"  "O8")))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx post, mem, insn;

    if (INTVAL (operands[2]) == GET_MODE_SIZE (DImode))
      post = gen_rtx_POST_INC (SImode, operands[0]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (DImode))
      post = gen_rtx_POST_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[2]);

        post = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
      }

    mem  = gen_rtx_MEM (DImode, post);
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*sto_di_post_modify_reg_split"
  [(set (mem:DI (match_operand:SI  0 "metag_regnofrm_op" "+e,f,h,l"))
        (match_operand:DI          1 "metag_register_op"  "a,a,d,d"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 2 "metag_register_op"  "e,f,h,l")))]

  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[0], operands[2]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
    rtx mem         = gen_rtx_MEM (DImode, post_modify);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising DI load pre-inc/dec/modify                                    | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*lod_di_pre_inc_dec_modify_disp_split"
  [(set (match_operand:DI                  0 "metag_register_op" "=r")
        (mem:DI (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "+efhl")
                         (match_operand:SI 2 "metag_offset6_di"   "O8"))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx pre, mem, insn;
    
    if (INTVAL (operands[2]) == GET_MODE_SIZE (DImode))
      pre = gen_rtx_PRE_INC (SImode, operands[1]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (DImode))
      pre = gen_rtx_PRE_DEC (SImode, operands[1]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[1], operands[2]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
      }
    
    mem  = gen_rtx_MEM (DImode, pre);
    insn = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "load")])

(define_insn_and_split "*lod_di_pre_modify_reg_split"
  [(set (match_operand:DI                  0 "metag_register_op" "=r,r,r,r")
        (mem:DI (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "+e,f,h,l")
                         (match_operand:SI 2 "metag_register_op"  "e,f,h,l"))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
    rtx mem        = gen_rtx_MEM (DImode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising DI load post-inc/dec/modify                                    | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*lod_di_post_inc_dec_modify_disp_split"
  [(set (match_operand:SI          0 "metag_regnofrm_op" "+efhl")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_di"   "O8")))
   (set (match_operand:DI          2 "metag_register_op" "=r")
        (mem:DI (match_dup 0)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx post, mem, insn;

    if (INTVAL (operands[1]) == GET_MODE_SIZE (DImode))
      post = gen_rtx_PRE_INC (SImode, operands[0]);
    else if (INTVAL (operands[1]) == -GET_MODE_SIZE (DImode))
      post = gen_rtx_PRE_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[1]);

        post = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
      }
    
    mem  = gen_rtx_MEM (DImode, post);
    insn = emit_insn (gen_rtx_SET (VOIDmode, operands[2], mem));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "load")])

(define_insn_and_split "*lod_di_post_modify_reg_split"
  [(set (match_operand:SI          0 "metag_regnofrm_op" "+e,f,h,l")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op"  "e,f,h,l")))
   (set (match_operand:DI          2 "metag_register_op" "=r,r,r,r")
        (mem:DI (match_dup 0)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
    rtx mem         = gen_rtx_MEM (DImode, post_modify);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, operands[2], mem));

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising zero extend EXTHI load pre-inc/dec/modify                      | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*lodz_<mode>hi_pre_inc_dec_modify_disp_split"
  [(set (match_operand:HI                        0 "metag_register_op"   "=r")
        (zero_extend:HI 
           (mem:EXTHI (plus:SI (match_operand:SI 1 "metag_regnofrm_op"   "+efhl")
                               (match_operand:SI 2 "metag_offset6_<mode>" "<O>")))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx pre, mem, zextend, insn;

    if (INTVAL (operands[2]) == GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_INC (SImode, operands[1]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_DEC (SImode, operands[1]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[1], operands[2]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
      }

    mem     = gen_rtx_MEM (<MODE>mode, pre);
    zextend = gen_rtx_ZERO_EXTEND (HImode, mem);
    insn    = emit_insn (gen_rtx_SET (VOIDmode, operands[0], zextend));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*lodz_<mode>hi_pre_modify_reg_split"
  [(set (match_operand:HI                        0 "metag_register_op" "=r,r,r,r")
        (zero_extend:HI
           (mem:EXTHI (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "+e,f,h,l")
                               (match_operand:SI 2 "metag_register_op"  "e,f,h,l")))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
    rtx mem        = gen_rtx_MEM (<MODE>mode, pre_modify);
    rtx zextend    = gen_rtx_ZERO_EXTEND (HImode, mem);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[0], zextend));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising zero extend EXTSI load pre-inc/dec/modify                      | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*lodz_<mode>si_pre_inc_dec_modify_disp_split"
  [(set (match_operand:SI                        0 "metag_register_op"   "=r")
        (zero_extend:SI 
           (mem:EXTSI (plus:SI (match_operand:SI 1 "metag_regnofrm_op"   "+efhl")
                               (match_operand:SI 2 "metag_offset6_<mode>" "<O>")))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx pre, mem, zextend, insn;

    if (INTVAL (operands[2]) == GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_INC (SImode, operands[1]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (<MODE>mode))
      pre = gen_rtx_PRE_DEC (SImode, operands[1]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[1], operands[2]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
      }

    mem     = gen_rtx_MEM (<MODE>mode, pre);
    zextend = gen_rtx_ZERO_EXTEND (SImode, mem);
    insn    = emit_insn (gen_rtx_SET (VOIDmode, operands[0], zextend));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*lodz_<mode>si_pre_modify_reg_split"
  [(set (match_operand:SI                        0 "metag_register_op" "=r,r,r,r")
        (zero_extend:SI
           (mem:EXTSI (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "+e,f,h,l")
                               (match_operand:SI 2 "metag_register_op"  "e,f,h,l")))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
    rtx mem        = gen_rtx_MEM (<MODE>mode, pre_modify);
    rtx zextend    = gen_rtx_ZERO_EXTEND (SImode, mem);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[0], zextend));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising SF store pre-inc/dec/modify                                    | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*sto_sf_pre_inc_dec_modify_disp_split"
  [(set (mem:SF
           (plus:SI (match_operand:SI 0 "metag_regnofrm_op"   "+efhl")
                    (match_operand:SI 1 "metag_offset6_sf"     "O4")))
        (match_operand:SF             2 "metag_register_op"    "r"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 1)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx pre, mem, insn;
    
    if (INTVAL (operands[1]) == GET_MODE_SIZE (SFmode))
      pre = gen_rtx_PRE_INC (SImode, operands[0]);
    else if (INTVAL (operands[1]) == -GET_MODE_SIZE (SFmode))
      pre = gen_rtx_PRE_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[1]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
      }

    mem  = gen_rtx_MEM (SFmode, pre);
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[2]));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*sto_sf_pre_modify_reg_split"
  [(set (mem:SF
           (plus:SI (match_operand:SI 0 "metag_regnofrm_op" "+%e,f,h,l")
                    (match_operand:SI 1 "metag_register_op"   "e,f,h,l")))
        (match_operand:SF             2 "metag_register_op"   "t,u,y,z"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 1)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    rtx mem        = gen_rtx_MEM (SFmode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[2]));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising SF store post-inc/dec/modify                                   | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*sto_sf_post_inc_dec_modify_disp_split"
  [(set (mem:SF (match_operand:SI    0 "metag_regnofrm_op"   "+efhl"))
        (match_operand:SF            1 "metag_register_op"    "r"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI   2 "metag_offset6_sf"     "O4")))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx post, mem, insn;
    
    if (INTVAL (operands[2]) == GET_MODE_SIZE (SFmode))
      post = gen_rtx_POST_INC (SImode, operands[0]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (SFmode))
      post = gen_rtx_POST_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[2]);

        post = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
      }

    mem  = gen_rtx_MEM (SFmode, post);
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*sto_sf_post_modify_reg_split"
  [(set (mem:SF (match_operand:SI    0 "metag_regnofrm_op" "+e,f,h,l"))
        (match_operand:SF            1 "metag_register_op"  "t,u,y,z"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI   2 "metag_register_op"  "e,f,h,l")))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[0], operands[2]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
    rtx mem         = gen_rtx_MEM (SFmode, post_modify);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising SF load pre-inc/dec/modify                                     | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*lod_sf_pre_inc_dec_modify_disp_split"
  [(set (match_operand:SF                  0 "metag_register_op" "=r")
        (mem:SF (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "+efhl")
                         (match_operand:SI 2 "metag_offset6_sf"   "O4"))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx pre, mem, insn;

    if (INTVAL (operands[2]) == GET_MODE_SIZE (SFmode))
      pre = gen_rtx_PRE_INC (SImode, operands[1]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (SFmode))
      pre = gen_rtx_PRE_DEC (SImode, operands[1]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[1], operands[2]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
      }

    mem        = gen_rtx_MEM (SFmode, pre);
    insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*lod_sf_pre_modify_reg_split"
  [(set (match_operand:SF                  0 "metag_register_op" "=r,r,r,r")
        (mem:SF (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "+e,f,h,l")
                         (match_operand:SI 2 "metag_register_op"  "e,f,h,l"))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
    rtx mem        = gen_rtx_MEM (SFmode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------
;; | Recognising DF load pre-inc/dec/modify                                    | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*lod_df_pre_inc_dec_modify_disp_split"
  [(set (match_operand:DF                  0 "metag_register_op" "=r")
        (mem:DF (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "+efhl")
                         (match_operand:SI 2 "metag_offset6_df"   "O8"))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx pre, mem, insn;
    
    if (INTVAL (operands[2]) == GET_MODE_SIZE (DFmode))
      pre = gen_rtx_PRE_INC (SImode, operands[1]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (DFmode))
      pre = gen_rtx_PRE_DEC (SImode, operands[1]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[1], operands[2]);

        pre = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
      }
    
    mem  = gen_rtx_MEM (DFmode, pre);
    insn = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "load")])

(define_insn_and_split "*lod_df_pre_modify_reg_split"
  [(set (match_operand:DF                  0 "metag_register_op" "=r,r,r,r")
        (mem:DF (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "+e,f,h,l")
                         (match_operand:SI 2 "metag_register_op"  "e,f,h,l"))))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[1], operands[2]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[1], plus);
    rtx mem        = gen_rtx_MEM (DFmode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, operands[0], mem));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising DF load post-inc/dec/modify                                    | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*lod_df_post_inc_dec_modify_disp_split"
  [(set (match_operand:SI          0 "metag_regnofrm_op" "+efhl")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_offset6_df"   "O8")))
   (set (match_operand:DF          2 "metag_register_op" "=r")
        (mem:DF (match_dup 0)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx post, mem, insn;

    if (INTVAL (operands[1]) == GET_MODE_SIZE (DFmode))
      post = gen_rtx_PRE_INC (SImode, operands[0]);
    else if (INTVAL (operands[1]) == -GET_MODE_SIZE (DFmode))
      post = gen_rtx_PRE_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[1]);

        post = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
      }
    
    mem  = gen_rtx_MEM (DFmode, post);
    insn = emit_insn (gen_rtx_SET (VOIDmode, operands[2], mem));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "load")])

(define_insn_and_split "*lod_df_post_modify_reg_split"
  [(set (match_operand:SI          0 "metag_regnofrm_op" "+e,f,h,l")
        (plus:SI (match_dup 0)
                 (match_operand:SI 1 "metag_register_op"  "e,f,h,l")))
   (set (match_operand:DF          2 "metag_register_op" "=r,r,r,r")
        (mem:DF (match_dup 0)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
    rtx mem         = gen_rtx_MEM (DFmode, post_modify);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, operands[2], mem));

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Recognising DF store pre-inc/dec/modify                                   | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*sto_df_pre_inc_dec_modify_disp_split"
  [(set (mem:DF (plus:SI (match_operand:SI 0 "metag_regnofrm_op" "+efhl")
                         (match_operand:SI 1 "metag_offset6_df"   "O8")))
        (match_operand:DF                  2 "metag_register_op"  "r"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 1)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx pre, mem, insn;

    if (INTVAL (operands[1]) == GET_MODE_SIZE (DFmode))
      pre = gen_rtx_PRE_INC (SImode, operands[0]);
    else if (INTVAL (operands[1]) == -GET_MODE_SIZE (DFmode))
      pre = gen_rtx_PRE_DEC (SImode, operands[0]);
    else
    {
      rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[1]);

      pre = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    }

    mem  = gen_rtx_MEM (DFmode, pre);
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[2]));

    if (auto_inc_p (pre))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*sto_df_pre_modify_reg_split"
  [(set (mem:DF (plus:SI (match_operand:SI 0 "metag_regnofrm_op" "+e,f,h,l")
                         (match_operand:SI 1 "metag_register_op"  "e,f,h,l")))
        (match_operand:DF                  2 "metag_register_op"  "a,a,d,d"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 1)))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus       = gen_rtx_PLUS (SImode, operands[0], operands[1]);
    rtx pre_modify = gen_rtx_PRE_MODIFY (SImode, operands[0], plus);
    rtx mem        = gen_rtx_MEM (DFmode, pre_modify);
    rtx insn       = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[2]));

    if (auto_inc_p (pre_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (pre_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------
;; | Recognising DF store post-inc/dec/modify                                   | 
;; -----------------------------------------------------------------------------

(define_insn_and_split "*sto_df_post_inc_dec_modify_disp_split"
  [(set (mem:DF (match_operand:SI  0 "metag_regnofrm_op" "+efhl"))
        (match_operand:DF          1 "metag_register_op"  "r"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 2 "metag_offset6_df"  "O8")))]
  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx post, mem, insn;

    if (INTVAL (operands[2]) == GET_MODE_SIZE (DFmode))
      post = gen_rtx_POST_INC (SImode, operands[0]);
    else if (INTVAL (operands[2]) == -GET_MODE_SIZE (DFmode))
      post = gen_rtx_POST_DEC (SImode, operands[0]);
    else
      {
        rtx plus = gen_rtx_PLUS (SImode, operands[0], operands[2]);

        post = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
      }

    mem  = gen_rtx_MEM (DFmode, post);
    insn = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    if (auto_inc_p (post))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])

(define_insn_and_split "*sto_df_post_modify_reg_split"
  [(set (mem:DF (match_operand:SI  0 "metag_regnofrm_op" "+e,f,h,l"))
        (match_operand:DF          1 "metag_register_op"  "a,a,d,d"))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_operand:SI 2 "metag_register_op"  "e,f,h,l")))]

  "TARGET_METAC_1_1
   && !reload_in_progress && !reload_completed"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    rtx plus        = gen_rtx_PLUS (SImode, operands[0], operands[2]);
    rtx post_modify = gen_rtx_POST_MODIFY (SImode, operands[0], plus);
    rtx mem         = gen_rtx_MEM (DFmode, post_modify);
    rtx insn        = emit_insn (gen_rtx_SET (VOIDmode, mem, operands[1]));

    if (auto_inc_p (post_modify))
      REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_INC, XEXP (post_modify, 0), REG_NOTES (insn));
    DONE;
  }
  [(set_attr "type" "fast")])
