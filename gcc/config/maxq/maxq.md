;; Definitions of target machine for GCC.
;; Dallas MAXQ version generating coff
;; Copyright (C) 2004, 2005 Free Software Foundation, Inc.

;; Contributed by Konark Goel (konarkg@noida.hcltech.com)
;;	          Prabu Pariasamy (prabup@noida.hcltech.com)
;;  	          C Jaiprakash (cjaiprakash@noida.hcltech.com).

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
;; the Free Software Foundation, 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.

;;
;; MAXQ CONSTRAINTS:
;;
;; Macro REG_CLASS_FROM_LETTER in file maxq.h defines the register
;; constraint letters.
;;

;; Special formats used for outputting MAXQ instructions:
;;
;;   %L  --  print the lsb reg of an int
;;   %H  --  print the msb reg of an int
;;

;; Special predicates:
;;
;; reg_or_imm_operand        --  operand is either reg, subreg or const.
;; call_address_operand      --  operand is either reg, symbol_ref, label_ref or const.
;;

;; -------------------------------------------------------------------------
;; Constants
;; -------------------------------------------------------------------------

(define_constants [
(LC0_REG 15)
(GR_REG 17)
(ACC_REG 18)
(BP_REG  21)
(DP_REG  22)
(SP_REG  23)
(GRL_REG 26)
(GRH_REG 25)
(OFF_REG 24)
(DPC_REG 29)
(AP_REG  19)
(PSF 28)
(CMP_REG 30)
(MEM1 31)
(MEM2 32)
])


;; ATTRIBUTES:
;; The attributes just specify the instruction classes. Additional attributes
;; will be required if MAXQ family introduces pipelining features.

(define_attr "type" "logical,bit_operation,math,branch,data_transfer,nothing"
             (const_string "nothing"))


;;
;; ARITHMETIC INSTRUCTIONS
;;

;; -------------------------------------------------------------------------
;; Addition instructions
;; -------------------------------------------------------------------------

;; Operand 0 has to be a register operand and operand 2 cannot be
;; a memory operand.
;; For MAXQ10 we are moving the operand 1 to the GR register so that we are
;; able to access the lower bytes and higher byter through to separate
;; registers GRL and GRH, where GRL is the lower byte and GRH is the higher
;; byte of the GR register. Operand 2 is splitted into 2 registers. Result
;; of the lower byte addition is moved again into GRL register and result
;; of addition of higher bytes is moved to GRH register. Then the result
;; which is in GR register is moved to operand 0. Same logic is used in
;; all the arithmetic instructions for MAXQ10.
(define_expand "addhi3"
  [(parallel [(set (match_operand:HI 0 "register_operand" "")
                   (plus:HI (match_operand:HI 1 "general_operand" "")
                            (match_operand:HI 2 "reg_or_imm_operand" "")))])]
  ""
  "
    rtx reg;

    /* In case operand 2 is a virtual_stack_vars_rtx then force it to some
       pseudo reg */
    if (TARGET_MAXQ20 && operands[2] == virtual_stack_vars_rtx)
      {
	reg = gen_reg_rtx (HImode);
	emit_move_insn (reg, operands[2]);
	operands[2] = reg;
      }

    if (!TARGET_MAXQ20 && operands[2] == virtual_stack_vars_rtx)
      {
	reg = gen_reg_rtx (HImode);
	emit_move_insn (reg, operands[2]);
	operands[2] = reg;
      }
    if (TARGET_MAXQ20)
      {
	rtx r;
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, operands[1]);
	emit_insn (gen_addhi3_1 (operands[2]));
	emit_move_insn (operands[0], r);
	DONE;
      }
    else
      {
	rtx r, op3, op4;

	operands[1] = force_reg (HImode, operands[1]);

	if ((GET_CODE (operands[2]) == REG
	     && (REGNO (operands[2]) == BP_REG)))
	  {
	    rtx tmp;
	    tmp = operands[1];
	    operands[1] = operands[2];
	    operands[2] = tmp;
	  }
	if (CHECK_SYMBOL_REF (operands[2]))
	  {
	    operands[2] = force_reg (HImode, operands[2]);
	  }

	op3 = operand_subword (operands[2], 0, 1, HImode);
	op4 = operand_subword (operands[2], 1, 1, HImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_insn (gen_move_gr (operands[1]));
	emit_insn (gen_move_grl_1 (r));
	emit_insn (gen_addqi3_1 (op3));
	emit_insn (gen_move_grl (r));
	emit_insn (gen_move_grh_1 (r));
	emit_insn (gen_addc_1_qi (op4));
	emit_insn (gen_move_grh (r));
	emit_insn (gen_move_gr_1 (operands[0]));
	DONE;
      }
  ")

(define_expand "addqi3"
 [(parallel [(set (match_operand:QI 0 "register_operand"    "=rbq")
                  (plus:QI (match_operand:QI 1 "general_operand" "")
                           (match_operand:QI 2 "reg_or_imm_operand" "rbqi")))])]
  "!TARGET_MAXQ20"
  "
    {
      rtx r;
      r = gen_rtx_REG (QImode, ACC_REG);
      emit_move_insn (r, operands[1]);
      emit_insn (gen_addqi3_1 (operands[2]));
      emit_move_insn (operands[0], r);
      DONE;
    }
  ")

;The actual ALU instructions supported by MAXQ.

;; In case its an ACC to ACC addition simply do a shift left
(define_insn "addhi3_1"
  [(set (reg:HI ACC_REG)
             (plus:HI (reg:HI ACC_REG)
                      (match_operand:HI 0 "reg_or_imm_operand" "rbli,*q,*a")))
   (set (reg:HI PSF) (ltu:HI (reg:HI ACC_REG) (match_dup 0)))]
  "TARGET_MAXQ20"
  "*
  {
    if (GET_CODE (operands[0]) == REG && REGNO (operands[0]) == ACC_REG)
      return \"sla\\t\";
    return \"add\\t%0\";
  }"
 [(set_attr "type" "math")])

;; In case its an ACC to ACC addition simply do a shift left
(define_insn "addqi3_1"
  [(set (reg:QI ACC_REG)
        (plus:QI (reg:QI ACC_REG)
                 (match_operand:QI 0 "reg_or_imm_operand" "rbli,*q,*a")))
   (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 0)))]
  "!TARGET_MAXQ20"
  "*
  {
    if (GET_CODE (operands[0]) == REG && REGNO (operands[0]) == ACC_REG)
      return \"sla\\t\";
    return \"add\\t%0\";
  }"
  [(set_attr "type" "math")])

;; Add with carry
(define_insn "addc_1"
  [(set (reg:HI ACC_REG)
        (plus:HI (plus:HI (reg:HI ACC_REG)
                          (match_operand:HI 0 "reg_or_imm_operand" "rbli,*q"))
                 (reg:HI PSF)))]
  "TARGET_MAXQ20"
  "addc\\t%0"
  [(set_attr "type" "math")])

(define_insn "addc_1_qi"
 [(set (reg:QI ACC_REG)
             (plus:QI (plus:QI (reg:QI ACC_REG)
                               (match_operand:QI 0 "reg_or_imm_operand" "rbli,*q"))
                      (reg:QI PSF)))]
  "!TARGET_MAXQ20"
  "*
  if (IS_MAXQ10_DEST (operands[0]) || GET_CODE (operands[0]) == CONST_INT)
    return \"addc\\t%0\";
  else
    return \"move\\tDPC,#1h\\n\\tmove\\t@--DP[1],GRL\\n\\tmove\\tGR,%0\\n\\taddc\\tGRH\\n\\tmove\\tGRL,@DP[1]++\";
  ")

(define_insn "addc_1_qi_1"
 [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG)
             (match_operand:QI 0 "reg_or_imm_operand" "rbli,*q")))
  (use (reg:QI PSF))
  (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 0))) ]
  "!TARGET_MAXQ20"
  "*
  if (IS_MAXQ10_DEST (operands[0]) || GET_CODE (operands[0]) == CONST_INT)
    return \"addc\\t%0\";
  else
    return \"move\\tDPC,#1h\\n\\tmove\\t@--DP[1],GRL\\n\\tmove\\tGR,%0\\n\\taddc\\tGRH\\n\\tmove\\tGRL,@DP[1]++\";
  ")

;; First add lower words and then add higher words with carry.
;; low0  = low1  + low2
;; high0 = high1 + high2 + carry
(define_expand "addsi3"
  [(parallel [(set (match_operand:SI 0 "register_operand" "=rbl")
                   (plus:SI (match_operand:SI 1 "register_operand" "rbl")
                            (match_operand:SI 2 "reg_or_imm_operand" "rbli")))
   (clobber (reg:CC PSF))])]
  ""
  "
    if (!reload_completed && GET_CODE (operands[1]) == MEM)
      operands[1] = force_reg (SImode, operands[1]);

    if (TARGET_MAXQ20)
      {
	rtx r, op3, op4, op5, op6, op7, op8;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_addhi3_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	emit_insn (gen_addc_1 (op8));
	emit_move_insn (op6, r);
	DONE;
      }
    else
      {
	rtx r, op3, op4, op5, op6, op7, op8, op9, op10, op11, op12, op13,
	  op14;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);
	op9 = operand_subword (operands[0], 2, 1, SImode);
	op10 = operand_subword (operands[1], 2, 1, SImode);
	op11 = operand_subword (operands[2], 2, 1, SImode);
	op12 = operand_subword (operands[0], 3, 1, SImode);
	op13 = operand_subword (operands[1], 3, 1, SImode);
	op14 = operand_subword (operands[2], 3, 1, SImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_addqi3_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	if (op8 != const0_rtx)
	  emit_insn (gen_addc_1_qi_1 (op8));
	else
	  emit_insn (gen_addc_1_qi (op8));
	emit_move_insn (op6, r);
	emit_move_insn (r, op10);
	if (op11 != const0_rtx)
	  emit_insn (gen_addc_1_qi_1 (op11));
	else
	  emit_insn (gen_addc_1_qi (op11));
	emit_move_insn (op9, r);
	emit_move_insn (r, op13);
	if (op14 != const0_rtx)
	  emit_insn (gen_addc_1_qi_1 (op14));
	else
	  emit_insn (gen_addc_1_qi (op14));
	emit_move_insn (op12, r);
	emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (QImode, ACC_REG)));
	DONE;
      }
  ")

;; -------------------------------------------------------------------------
;; Subtraction instructions
;; -------------------------------------------------------------------------

(define_expand "subhi3"
 [(parallel [(set (match_operand:HI 0 "register_operand"    "")
                  (minus:HI (match_operand:HI 1 "general_operand" "")
                            (match_operand:HI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    if (TARGET_MAXQ20)
      {
	rtx r;
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, operands[1]);
	emit_insn (gen_subhi_1 (operands[2]));
	emit_move_insn (operands[0], r);
	DONE;
      }
    else
      {
	rtx r, op3, op4;

	operands[1] = force_reg (HImode, operands[1]);

	if (CHECK_SYMBOL_REF (operands[2]))
	  {
	    operands[2] = force_reg (HImode, operands[2]);
	  }

	emit_insn (gen_move_gr (operands[1]));
	op3 = operand_subword (operands[2], 0, 1, HImode);
	op4 = operand_subword (operands[2], 1, 1, HImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_insn (gen_move_grl_1 (r));
	emit_insn (gen_subqi_1 (op3));
	emit_insn (gen_move_grl (r));
	emit_insn (gen_move_grh_1 (r));
	emit_insn (gen_subb_1_qi (op4));
	emit_insn (gen_move_grh (r));
	emit_insn (gen_move_gr_1 (operands[0]));
	DONE;
      }
  ")

(define_expand "subqi3"
 [(parallel [(set (match_operand:QI 0 "register_operand"    "")
                  (minus:QI (match_operand:QI 1 "general_operand" "")
                            (match_operand:QI 2 "reg_or_imm_operand" "rbqi")))])]
  "!TARGET_MAXQ20"
  "
    {
      rtx r;
      r = gen_rtx_REG (QImode, ACC_REG);
      emit_move_insn (r, operands[1]);
      emit_insn (gen_subqi_1 (operands[2]));
      emit_move_insn (operands[0], r);
      DONE;
    }
  ")

;The actual ALU instructions supported by MAXQ.

(define_insn "subhi_1"
 [(set (reg:HI ACC_REG)
            (minus:HI (reg:HI ACC_REG)
                      (match_operand:HI 0 "reg_or_imm_operand" "rbli,*q")))
  (set (reg:HI PSF)(ltu:HI (reg:HI ACC_REG) (match_dup 0)))]
  "TARGET_MAXQ20"
  "sub\\t%0"
  [(set_attr "type" "math")])

(define_insn "subqi_1"
 [(set (reg:QI ACC_REG)
            (minus:QI (reg:QI ACC_REG)
                      (match_operand:QI 0 "reg_or_imm_operand" "rbli,*q")))
  (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 0)))]
  "!TARGET_MAXQ20"
  "sub\\t%0"
  [(set_attr "type" "math")])

;; Subtract with borrow

(define_insn "subb_1"
  [(set (reg:HI ACC_REG)
        (minus:HI (minus:HI (reg:HI ACC_REG)
                            (match_operand:HI 0 "reg_or_imm_operand" "rbli,*q"))
                  (reg:HI PSF)))]
  "TARGET_MAXQ20"
  "subb\\t%0"
  [(set_attr "type" "math")])

(define_insn "subb_1_qi"
 [(set (reg:QI ACC_REG)
       (minus:QI (minus:QI (reg:QI ACC_REG)
                           (match_operand:QI 0 "reg_or_imm_operand" "rbli,*q"))
                 (reg:QI PSF)))]
  "!TARGET_MAXQ20"
  "*
  if (IS_MAXQ10_DEST (operands[0]) || GET_CODE (operands[0]) == CONST_INT)
    return \"subb\\t%0\";
  else
    return \"move\\tDPC,#1h\\n\\tmove\\t@--DP[1],GRL\\n\\tmove\\tGR,%0\\n\\tsubb\\tGRH\\n\\tmove\\tGRL,@DP[1]++\";
  ")

;; First subtract lower words then subtract higher words with borrow.
(define_expand "subsi3"
  [(parallel [(set (match_operand:SI 0 "register_operand" "=rbl")
                   (minus:SI (match_operand:SI 1 "register_operand" "rbl")
                             (match_operand:SI 2 "reg_or_imm_operand" "rbli")))
              (clobber (reg:HI PSF))])]
  ""
  "
    if (GET_CODE (operands[1]) == MEM)
      operands[1] = force_reg (SImode, operands[1]);

    if (GET_CODE (operands[2]) == MEM)
      operands[2] = force_reg (SImode, operands[2]);

    if (TARGET_MAXQ20)
      {
	rtx r, op3, op4, op5, op6, op7, op8;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);

	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_subhi_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	emit_insn (gen_subb_1 (op8));
	emit_move_insn (op6, r);
	DONE;
      }
    else
      {
	rtx r, op3, op4, op5, op6, op7, op8, op9, op10, op11, op12, op13,
	  op14;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);
	op9 = operand_subword (operands[0], 2, 1, SImode);
	op10 = operand_subword (operands[1], 2, 1, SImode);
	op11 = operand_subword (operands[2], 2, 1, SImode);
	op12 = operand_subword (operands[0], 3, 1, SImode);
	op13 = operand_subword (operands[1], 3, 1, SImode);
	op14 = operand_subword (operands[2], 3, 1, SImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_subqi_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	emit_insn (gen_subb_1_qi (op8));
	emit_move_insn (op6, r);
	emit_move_insn (r, op10);
	emit_insn (gen_subb_1_qi (op11));
	emit_move_insn (op9, r);
	emit_move_insn (r, op13);
	emit_insn (gen_subb_1_qi (op14));
	emit_move_insn (op12, r);
	DONE;
      }
  ")


;; -------------------------------------------------------------------------
;; Logical instructions
;; -------------------------------------------------------------------------

(define_expand "andsi3"
 [(parallel [(set (match_operand:SI 0 "register_operand"    "")
                  (and:SI (match_operand:SI 1 "general_operand" "")
                          (match_operand:SI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    if (!reload_completed && GET_CODE (operands[1]) == MEM)
      operands[1] = force_reg (SImode, operands[1]);

    if (!reload_completed && GET_CODE (operands[2]) == MEM)
      operands[2] = force_reg (SImode, operands[2]);

    if (TARGET_MAXQ20)
      {
	rtx r, op3, op4, op5, op6, op7, op8;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_andhi3_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	emit_insn (gen_andhi3_1 (op8));
	emit_move_insn (op6, r);
	DONE;
      }
    else
      {
	rtx r, op3, op4, op5, op6, op7, op8, op9, op10, op11, op12, op13,
	  op14;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);
	op9 = operand_subword (operands[0], 2, 1, SImode);
	op10 = operand_subword (operands[1], 2, 1, SImode);
	op11 = operand_subword (operands[2], 2, 1, SImode);
	op12 = operand_subword (operands[0], 3, 1, SImode);
	op13 = operand_subword (operands[1], 3, 1, SImode);
	op14 = operand_subword (operands[2], 3, 1, SImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_andqi3_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	emit_insn (gen_andqi3_1 (op8));
	emit_move_insn (op6, r);
	emit_move_insn (r, op10);
	emit_insn (gen_andqi3_1 (op11));
	emit_move_insn (op9, r);
	emit_move_insn (r, op13);
	emit_insn (gen_andqi3_1 (op14));
	emit_move_insn (op12, r);
	DONE;
      }
  ")

(define_expand "andhi3"
 [(parallel [(set (match_operand:HI 0 "register_operand"    "")
                  (and:HI (match_operand:HI 1 "general_operand" "")
                          (match_operand:HI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    if (TARGET_MAXQ20)
      {
	rtx r;
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, operands[1]);
	emit_insn (gen_andhi3_1 (operands[2]));
	emit_move_insn (operands[0], r);
	DONE;
      }
    else
      {
	rtx r, op3, op4;

	operands[1] = force_reg (HImode, operands[1]);

	if (CHECK_SYMBOL_REF (operands[2]))
	  {
	    operands[2] = force_reg (HImode, operands[2]);
	  }

	op3 = operand_subword (operands[2], 0, 1, HImode);
	op4 = operand_subword (operands[2], 1, 1, HImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_insn (gen_move_gr (operands[1]));
	emit_insn (gen_move_grl_1 (r));
	emit_insn (gen_andqi3_1 (op3));
	emit_insn (gen_move_grl (r));
	emit_insn (gen_move_grh_1 (r));
	emit_insn (gen_andqi3_2 (op4));
	emit_insn (gen_move_grh (r));
	emit_insn (gen_move_gr_1 (operands[0]));
	DONE;
      }
  ")

(define_expand "andqi3"
 [(parallel [(set (match_operand:QI 0 "register_operand"    "")
                  (and:QI (match_operand:QI 1 "general_operand" "")
                          (match_operand:QI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    {
      rtx r;
      r = gen_rtx_REG (QImode, ACC_REG);
      emit_move_insn (r, operands[1]);
      emit_insn (gen_andqi3_1 (operands[2]));
      emit_move_insn (operands[0], r);
      DONE;
    }
  ")

;The actual ALU instructions supported by MAXQ.

(define_insn "andhi3_1"
  [(set (reg:HI ACC_REG)
             (and:HI (reg:HI ACC_REG)
                     (match_operand:HI 0 "reg_or_imm_operand" "rbli,*q")))
   (clobber (reg:HI PSF))]
  "TARGET_MAXQ20"
  "and\\t%0"
  [(set_attr "type" "math")])

(define_insn "andqi3_1"
 [(set (reg:QI ACC_REG)
             (and:QI (reg:QI ACC_REG)
                     (match_operand:QI 0 "reg_or_imm_operand" "rbli,*q")))
  (clobber (reg:HI PSF))]
  ""
  "and\\t%0"
  [(set_attr "type" "math")])

;; This extra pattern is required as we are modifying the contents of the GRL
;; register in the instructions but we also need to preserve its previous value
;; so we are pushing GRL and the popping it.
(define_insn "andqi3_2"
 [(set (reg:QI ACC_REG)
             (and:QI (and:QI (reg:QI ACC_REG)
                             (match_operand:QI 0 "reg_or_imm_operand" "rbli,*q"))
                     (reg:QI PSF)))
  (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 0)))
  (use (reg:QI ACC_REG))]
  "!TARGET_MAXQ20"
  "*
  {
    if (IS_MAXQ10_DEST (operands[0]) || GET_CODE (operands[0]) == CONST_INT)
      return \"and\\t%0\";
    else
      return \"move\\tDPC,#1h\\n\\tmove\\t@--DP[1],GRL\\n\\tmove\\tGR,%0\\n\\tand\\tGRH\\n\\tmove\\tGRL,@DP[1]++\";
  }")

(define_expand "iorsi3"
 [(parallel [(set (match_operand:SI 0 "register_operand"    "")
                  (ior:SI (match_operand:SI 1 "general_operand" "")
                          (match_operand:SI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    if (!reload_completed && GET_CODE (operands[1]) == MEM)
      operands[1] = force_reg (SImode, operands[1]);

    if (!reload_completed && GET_CODE (operands[2]) == MEM)
      operands[2] = force_reg (SImode, operands[2]);

    if (TARGET_MAXQ20)
      {
	rtx r, op3, op4, op5, op6, op7, op8;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_iorhi3_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	emit_insn (gen_iorhi3_1 (op8));
	emit_move_insn (op6, r);
	DONE;
      }
    else
      {
	rtx r, op3, op4, op5, op6, op7, op8, op9, op10, op11, op12, op13,
	  op14;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);
	op9 = operand_subword (operands[0], 2, 1, SImode);
	op10 = operand_subword (operands[1], 2, 1, SImode);
	op11 = operand_subword (operands[2], 2, 1, SImode);
	op12 = operand_subword (operands[0], 3, 1, SImode);
	op13 = operand_subword (operands[1], 3, 1, SImode);
	op14 = operand_subword (operands[2], 3, 1, SImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_iorqi3_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	emit_insn (gen_iorqi3_1 (op8));
	emit_move_insn (op6, r);
	emit_move_insn (r, op10);
	emit_insn (gen_iorqi3_1 (op11));
	emit_move_insn (op9, r);
	emit_move_insn (r, op13);
	emit_insn (gen_iorqi3_1 (op14));
	emit_move_insn (op12, r);
	DONE;
      }
  ")

(define_expand "iorhi3"
 [(parallel [(set (match_operand:HI 0 "register_operand"    "")
                  (ior:HI (match_operand:HI 1 "general_operand" "")
                          (match_operand:HI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    if (TARGET_MAXQ20)
      {
	rtx r;
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, operands[1]);
	emit_insn (gen_iorhi3_1 (operands[2]));
	emit_move_insn (operands[0], r);
	DONE;
      }
    else
      {
	rtx r, op3, op4;

	operands[1] = force_reg (HImode, operands[1]);

	if (CHECK_SYMBOL_REF (operands[2]))
	  operands[2] = force_reg (HImode, operands[2]);

	op3 = operand_subword (operands[2], 0, 1, HImode);
	op4 = operand_subword (operands[2], 1, 1, HImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_insn (gen_move_gr (operands[1]));
	emit_insn (gen_move_grl_1 (r));
	emit_insn (gen_iorqi3_1 (op3));
	emit_insn (gen_move_grl (r));
	emit_insn (gen_move_grh_1 (r));
	emit_insn (gen_iorqi3_2 (op4));
	emit_insn (gen_move_grh (r));
	emit_insn (gen_move_gr_1 (operands[0]));
	DONE;
      }
  ")

(define_expand "iorqi3"
 [(parallel [(set (match_operand:QI 0 "register_operand"    "")
                  (ior:QI (match_operand:QI 1 "general_operand" "")
                          (match_operand:QI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    {
      rtx r;
      r = gen_rtx_REG (QImode, ACC_REG);
      emit_move_insn (r, operands[1]);
      emit_insn (gen_iorqi3_1 (operands[2]));
      emit_move_insn (operands[0], r);
      DONE;
    }
   ")

;The actual ALU instructions supported by MAXQ.

(define_insn "iorhi3_1"
  [(set (reg:HI ACC_REG)
              (ior:HI (reg:HI ACC_REG)
                      (match_operand:HI 0 "reg_or_imm_operand" "rbli,*q")))
   (clobber (reg:HI PSF))]
  "TARGET_MAXQ20"
  "or\\t%0"
  [(set_attr "type" "math")])

(define_insn "iorqi3_1"
 [(set (reg:QI ACC_REG)
             (ior:QI (reg:QI ACC_REG)
             (match_operand:QI 0 "reg_or_imm_operand" "rbli,*q")))
  (clobber (reg:HI PSF))]
  ""
  "or\\t%0"
  [(set_attr "type" "math")])

(define_insn "iorqi3_1_grl"
 [(set (reg:QI ACC_REG)
             (ior:QI (reg:QI ACC_REG)
                     (reg:QI GRL_REG)))
  (clobber (reg:HI PSF))
  (use (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "or\\tGRL"
  [(set_attr "type" "math")])

;; This extra pattern is required as we are modifying the contents of the GRL
;; register in the instructions but we also need to preserve its previous value
;; so we are pushing GRL and the popping it.
(define_insn "iorqi3_2"
 [(set (reg:QI ACC_REG)
             (ior:QI (ior:QI (reg:QI ACC_REG)
                             (match_operand:QI 0 "reg_or_imm_operand" "rbli,*q"))
                             (reg:QI PSF)))
  (clobber (reg:HI PSF))]
  "!TARGET_MAXQ20"
  "*
  {
    if (IS_MAXQ10_DEST (operands[0])
        || GET_CODE (operands[0]) == CONST_INT)
      return \"or\\t%0\";
    else
      return \"move\\tDPC,#1h\\n\\tmove\\t@--DP[1],GRL\\n\\tmove\\tGR,%0\\n\\tor\\tGRH\\n\\tmove\\tGRL,@DP[1]++\";
  }
  ")

(define_expand "xorsi3"
 [(parallel [(set (match_operand:SI 0 "register_operand"    "")
                  (xor:SI (match_operand:SI 1 "general_operand" "")
                          (match_operand:SI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    if (!reload_completed && GET_CODE (operands[1]) == MEM)
      operands[1] = force_reg (SImode, operands[1]);

    if (!reload_completed && GET_CODE (operands[2]) == MEM)
      operands[2] = force_reg (SImode, operands[2]);

    if (TARGET_MAXQ20)
      {
	rtx r, op3, op4, op5, op6, op7, op8;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_xorhi3_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	emit_insn (gen_xorhi3_1 (op8));
	emit_move_insn (op6, r);
	DONE;
      }
    else
      {
	rtx r, op3, op4, op5, op6, op7, op8, op9, op10, op11, op12, op13,
	  op14;
	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[2], 0, 1, SImode);
	op6 = operand_subword (operands[0], 1, 1, SImode);
	op7 = operand_subword (operands[1], 1, 1, SImode);
	op8 = operand_subword (operands[2], 1, 1, SImode);
	op9 = operand_subword (operands[0], 2, 1, SImode);
	op10 = operand_subword (operands[1], 2, 1, SImode);
	op11 = operand_subword (operands[2], 2, 1, SImode);
	op12 = operand_subword (operands[0], 3, 1, SImode);
	op13 = operand_subword (operands[1], 3, 1, SImode);
	op14 = operand_subword (operands[2], 3, 1, SImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_xorqi3_1 (op5));
	emit_move_insn (op3, r);
	emit_move_insn (r, op7);
	emit_insn (gen_xorqi3_1 (op8));
	emit_move_insn (op6, r);
	emit_move_insn (r, op10);
	emit_insn (gen_xorqi3_1 (op11));
	emit_move_insn (op9, r);
	emit_move_insn (r, op13);
	emit_insn (gen_xorqi3_1 (op14));
	emit_move_insn (op12, r);
	DONE;
      }
  ")

(define_expand "xorhi3"
 [(parallel [(set (match_operand:HI 0 "register_operand"    "")
                  (xor:HI (match_operand:HI 1 "general_operand" "")
                          (match_operand:HI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    if (TARGET_MAXQ20)
      {
	rtx r;
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, operands[1]);
	emit_insn (gen_xorhi3_1 (operands[2]));
	emit_move_insn (operands[0], r);
	DONE;
      }
    else
      {
	rtx r, op3, op4;

	operands[1] = force_reg (HImode, operands[1]);

	if (CHECK_SYMBOL_REF (operands[2]))
	  {
	    operands[2] = force_reg (HImode, operands[2]);
	  }

	op3 = operand_subword (operands[2], 0, 1, HImode);
	op4 = operand_subword (operands[2], 1, 1, HImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_insn (gen_move_gr (operands[1]));
	emit_insn (gen_move_grl_1 (r));
	emit_insn (gen_xorqi3_1 (op3));
	emit_insn (gen_move_grl (r));
	emit_insn (gen_move_grh_1 (r));
	if (GET_CODE (operands[2]) != CONST_INT)
	  emit_insn (gen_xorqi3_2 (op3));
	else
	  emit_insn (gen_xorqi3_1 (op4));
	emit_insn (gen_move_grh (r));
	emit_insn (gen_move_gr_1 (operands[0]));
	DONE;
      }
  ")

(define_expand "xorqi3"
 [(parallel [(set (match_operand:QI 0 "register_operand"    "")
                  (xor:QI (match_operand:QI 1 "general_operand" "")
                          (match_operand:QI 2 "reg_or_imm_operand" "rbqi")))])]
  ""
  "
    {
      rtx r;
      r = gen_rtx_REG (QImode, ACC_REG);
      emit_move_insn (r, operands[1]);
      emit_insn (gen_xorqi3_1 (operands[2]));
      emit_move_insn (operands[0], r);
      DONE;
    }
  ")

;The actual ALU instructions supported by MAXQ.

(define_insn "xorhi3_1"
 [(set (reg:HI ACC_REG)
             (xor:HI (reg:HI ACC_REG)
                     (match_operand:HI 0 "reg_or_imm_operand" "rbi,*q")))
  (clobber (reg:HI PSF))]
  "TARGET_MAXQ20"
  "xor\\t%0"
  [(set_attr "type" "math")])

(define_insn "xorqi3_1"
 [(set (reg:QI ACC_REG)
             (xor:QI (reg:QI ACC_REG)
                     (match_operand:QI 0 "reg_or_imm_operand" "rbi,*q")))
   (clobber (reg:HI PSF))]
  ""
  "xor\\t%0"
  [(set_attr "type" "math")])

;; This extra pattern is required as we are modifying the contents of the GRL
;; register in the instructions but we also need to preserve its previous value
;; so we are pushing GRL and the popping it.
(define_insn "xorqi3_2"
 [(set (reg:QI ACC_REG)
             (xor:QI (xor:QI (reg:QI ACC_REG)
                             (match_operand:QI 0 "register_operand" "rb,*q"))
                     (reg:QI PSF)))
   (clobber (reg:HI PSF))]
  "!TARGET_MAXQ20"
  "*
  {
    if (IS_MAXQ10_DEST (operands[0])
        || GET_CODE (operands[0]) == CONST_INT)
      return \"xor\\t%H0\";
    else
      return \"move\\tDPC,#1h\\n\\tmove\\t@--DP[1],GRL\\n\\tmove\\tGR,%0\\n\\txor\\tGRH\\n\\tmove\\tGRL,@DP[1]++\";
  }")

;; This extra pattern is required as we are modifying the contents of the GRL
;; and GRH register in the instructions but we also need to preserve its previous
;; value so we are pushing GRL and GRH, and then popping them.
(define_insn "xorqi3_3"
 [(set (reg:QI ACC_REG)
             (xor:QI (xor:QI (reg:QI ACC_REG)
                             (match_operand:QI 0 "immediate_operand" "i"))
                     (reg:QI PSF)))
  (clobber  (reg:HI  PSF))]
  "!TARGET_MAXQ20"
  "*
  {
    if (IS_MAXQ10_DEST (operands[0]) || GET_CODE (operands[0]) == CONST_INT)
      return \"xor\\t%0\";
    else
      return \"move\tDPC,#1h\n\tmove\t@--DP[1],GRH\n\tmove\t@--DP[1],GRL\n\tmove\tGR,%0\n\txor\tGRH\n\tmove\tGRL,@DP[1]++\n\tmove\tGRH,@DP[1]++\";
  }")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                   DATA TRANSFER                                                                    ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "movsi"
  [(set (match_operand:SI 0 "general_operand" "rbm")
        (match_operand:SI 1 "general_operand" "rbi")) ]
  ""
  "
    if (!reload_completed && GET_CODE (operands[0]) == MEM
	&& GET_CODE (operands[1]) == MEM)
      operands[1] = force_reg (SImode, operands[1]);

    if (GET_CODE (operands[0]) == MEM
	&& GET_CODE (XEXP (operands[0], 0)) != PRE_DEC)
      {
	if (TARGET_MAXQ20)
	  emit_insn (gen_rtx_CLOBBER
		     (VOIDmode, gen_rtx_REG (HImode, ACC_REG)));
	else
	  emit_insn (gen_rtx_CLOBBER
		     (VOIDmode, gen_rtx_REG (QImode, ACC_REG)));
	emit_insn (gen_movsi_mem_2 (operands[0], operands[1]));
	DONE;
      }
    else if (GET_CODE (operands[1]) == MEM
	     && GET_CODE (XEXP (operands[1], 0)) != POST_INC)
      {
	if (TARGET_MAXQ20)
	  emit_insn (gen_rtx_CLOBBER
		     (VOIDmode, gen_rtx_REG (HImode, ACC_REG)));
	else
	  emit_insn (gen_rtx_CLOBBER
		     (VOIDmode, gen_rtx_REG (QImode, ACC_REG)));
	emit_insn (gen_movsi_mem_1 (operands[0], operands[1]));
	DONE;
      }
    else if (GET_CODE (operands[0]) == MEM
	     && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
      {
	if (TARGET_MAXQ20)
	  {
	    rtx op1, op2;
	    op1 = operand_subword (operands[1], 1, 1, SImode);
	    op2 = operand_subword (operands[1], 0, 1, SImode);
	    emit_insn (gen_pushhi (op1));
	    emit_insn (gen_pushhi (op2));
	    DONE;
	  }
	else
	  {
	    rtx op1, op2, op3, op4;
	    op1 = operand_subword (operands[1], 0, 1, SImode);
	    op2 = operand_subword (operands[1], 1, 1, SImode);
	    op3 = operand_subword (operands[1], 2, 1, SImode);
	    op4 = operand_subword (operands[1], 3, 1, SImode);
	    emit_insn (gen_pushqi (op4));
	    emit_insn (gen_pushqi (op3));
	    emit_insn (gen_pushqi (op2));
	    emit_insn (gen_pushqi (op1));
	    DONE;
	  }
      }
    else if (GET_CODE (operands[1]) == MEM
	     && GET_CODE (XEXP (operands[1], 0)) == POST_INC)
      {
	if (TARGET_MAXQ20)
	  {
	    rtx op1, op2;
	    op1 = operand_subword (operands[0], 1, 1, SImode);
	    op2 = operand_subword (operands[0], 0, 1, SImode);
	    emit_insn (gen_pophi (op2));
	    emit_insn (gen_pophi (op1));
	    DONE;
	  }
	else
	  {
	    rtx op1, op2, op3, op4;
	    op1 = operand_subword (operands[1], 0, 1, SImode);
	    op2 = operand_subword (operands[1], 1, 1, SImode);
	    op3 = operand_subword (operands[1], 2, 1, SImode);
	    op4 = operand_subword (operands[1], 3, 1, SImode);
	    emit_insn (gen_popqi (op1));
	    emit_insn (gen_popqi (op2));
	    emit_insn (gen_popqi (op3));
	    emit_insn (gen_popqi (op4));
	    DONE;
	  }
      }
    else if (GET_CODE (operands[0]) != MEM && GET_CODE (operands[1]) != MEM)
      {
	if (TARGET_MAXQ20)
	  {
	    rtx op1, op2, op3, op4;
	    op1 = operand_subword (operands[0], 0, 1, SImode);
	    op2 = operand_subword (operands[0], 1, 1, SImode);
	    op3 = operand_subword (operands[1], 0, 1, SImode);
	    op4 = operand_subword (operands[1], 1, 1, SImode);
	    emit_move_insn (op1, op3);
	    emit_move_insn (op2, op4);
	    DONE;
	  }
	else
	  {
	    emit_insn (gen_rtx_CLOBBER
		       (VOIDmode, gen_rtx_REG (QImode, ACC_REG)));
	  }
      }
  ")

(define_insn "movsi_reg"
  [(set (match_operand:SI 0 "register_operand" "=rb")
        (match_operand:SI 1 "register_operand" "rb")) ]
  "!TARGET_MAXQ20"
  "#")

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "register_operand" "")) ]
  "!TARGET_MAXQ20"
  [
   (set (match_dup 2) (match_dup 6))
   (set (match_dup 3) (match_dup 7))
   (set (match_dup 4) (match_dup 8))
   (set (match_dup 5) (match_dup 9))
  ]
  "
  {
    operands[2] = operand_subword (operands[0], 0, 1, SImode);
    operands[3] = operand_subword (operands[0], 1, 1, SImode);
    operands[4] = operand_subword (operands[0], 2, 1, SImode);
    operands[5] = operand_subword (operands[0], 3, 1, SImode);
    operands[6] = operand_subword (operands[1], 0, 1, SImode);
    operands[7] = operand_subword (operands[1], 1, 1, SImode);
    operands[8] = operand_subword (operands[1], 2, 1, SImode);
    operands[9] = operand_subword (operands[1], 3, 1, SImode);
  }
  ")

(define_insn "movsi_imm"
  [(set (match_operand:SI 0 "register_operand" "=rb")
        (match_operand:SI 1 "immediate_operand" "i")) ]
  "!TARGET_MAXQ20"
  "#")

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "immediate_operand" "")) ]
  "!TARGET_MAXQ20"
  [
   (set (match_dup 2) (match_dup 6))
   (set (match_dup 3) (match_dup 7))
   (set (match_dup 4) (match_dup 8))
   (set (match_dup 5) (match_dup 9))
  ]
  "
  {
    operands[2] = operand_subword (operands[0], 0, 1, SImode);
    operands[3] = operand_subword (operands[0], 1, 1, SImode);
    operands[4] = operand_subword (operands[0], 2, 1, SImode);
    operands[5] = operand_subword (operands[0], 3, 1, SImode);
    operands[6] = operand_subword (operands[1], 0, 1, SImode);
    operands[7] = operand_subword (operands[1], 1, 1, SImode);
    operands[8] = operand_subword (operands[1], 2, 1, SImode);
    operands[9] = operand_subword (operands[1], 3, 1, SImode);
  }
  ")

(define_insn "movsi_mem_1"
  [(set (match_operand:SI 0 "register_operand" "=rb")
        (match_operand:SI 1 "memory_operand" "m")) ]
  ""
  "#")

(define_insn "movsi_mem_2"
  [(set (match_operand:SI 0 "memory_operand" "=m")
        (match_operand:SI 1 "reg_or_imm_operand" "rbi")) ]
  ""
  "#")

; Split SImode move into two HImode moves 
 
(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "memory_operand" ""))]
  "TARGET_MAXQ20"
  [(set (match_dup 2) (match_dup 4))
   (set (match_dup 3) (match_dup 5))]
  "
  {
    rtx  base;
    operands[2] = operand_subword (operands[0], 0, 1, SImode);
    operands[3] = operand_subword (operands[0], 1, 1, SImode);
    base = XEXP (operands[1], 0);
    operands[4] = gen_rtx_MEM (HImode, base);
    operands[5] = gen_rtx_MEM (HImode, gen_rtx_PLUS (HImode, base, GEN_INT(2)));
  }
  ")

; Split SImode move into 4 QImode moves 

(define_split
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "memory_operand" "")) ]
  "!TARGET_MAXQ20"
  [
   (set (match_dup 2) (match_dup 6))
   (set (match_dup 3) (match_dup 7))
   (set (match_dup 4) (match_dup 8))
   (set (match_dup 5) (match_dup 9))
  ]
  "
  {
    rtx base;
    operands[2] = operand_subword (operands[0], 0, 1, SImode);
    operands[3] = operand_subword (operands[0], 1, 1, SImode);
    operands[4] = operand_subword (operands[0], 2, 1, SImode);
    operands[5] = operand_subword (operands[0], 3, 1, SImode);
    base = XEXP (operands[1], 0);
    operands[6] = gen_rtx_MEM (QImode, base);
    operands[7] = gen_rtx_MEM (QImode, gen_rtx_PLUS (HImode, base, GEN_INT(1)));
    operands[8] = gen_rtx_MEM (QImode, gen_rtx_PLUS (HImode, base, GEN_INT(2)));
    operands[9] = gen_rtx_MEM (QImode, gen_rtx_PLUS (HImode, base, GEN_INT(3)));
  }
  ")

; Split SImode move into two HImode moves

(define_split
  [(set (match_operand:SI 0 "memory_operand" "")
        (match_operand:SI 1 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  [
   (set (match_dup 2) (match_dup 4))
   (set (match_dup 3) (match_dup 5))
  ]
  "
  {
    rtx base;
    operands[4] = operand_subword (operands[1], 0, 1, SImode);
    operands[5] = operand_subword (operands[1], 1, 1, SImode);
    base = XEXP (operands[0], 0);
    operands[2] = gen_rtx_MEM (HImode, base);
    operands[3] = gen_rtx_MEM (HImode, gen_rtx_PLUS (HImode, base, GEN_INT(2)));
  }
  ")

; Split SImode move into 4 QImode moves

(define_split
  [(set (match_operand:SI 0 "memory_operand" "")
        (match_operand:SI 1 "reg_or_imm_operand" "")) ]
  "!TARGET_MAXQ20"
  [
   (set (match_dup 2) (match_dup 6))
   (set (match_dup 3) (match_dup 7))
   (set (match_dup 4) (match_dup 8))
   (set (match_dup 5) (match_dup 9))
  ]
  "
  {
    rtx base;
    operands[6] = operand_subword (operands[1], 0, 1, SImode);
    operands[7] = operand_subword (operands[1], 1, 1, SImode);
    operands[8] = operand_subword (operands[1], 2, 1, SImode);
    operands[9] = operand_subword (operands[1], 3, 1, SImode);
    base = XEXP (operands[0], 0);
    operands[2] = gen_rtx_MEM (QImode, base);
    operands[3] = gen_rtx_MEM (QImode, gen_rtx_PLUS (HImode, base, GEN_INT(1)));
    operands[4] = gen_rtx_MEM (QImode, gen_rtx_PLUS (HImode, base, GEN_INT(2)));
    operands[5] = gen_rtx_MEM (QImode, gen_rtx_PLUS (HImode, base, GEN_INT(3)));
  }
  ")

;; MAXQ doesn't support mem to mem move so we force one of the operands to 
;; a register in case both the operands are memory operands
(define_expand "movhi"
  [(set (match_operand:HI 0 "general_operand" "")
        (match_operand:HI 1 "general_operand" ""))]
  ""
  "
    if (TARGET_MAXQ20 && operands[1] == virtual_stack_vars_rtx)
      emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (HImode, ACC_REG)));

    if (!reload_completed && GET_CODE (operands[0]) == MEM
	&& GET_CODE (operands[1]) == MEM)
      operands[1] = force_reg (HImode, operands[1]);

    /* Move instructions involving memory will modify the accumulator
       register due to its use in arithmetic instructions emitted for address 
       calculations. */
    if (TARGET_MAXQ20
	&& (GET_CODE (operands[0]) == MEM || GET_CODE (operands[1]) == MEM))
      emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (HImode, ACC_REG)));

    if (!TARGET_MAXQ20
	&& (GET_CODE (operands[0]) == MEM || GET_CODE (operands[1]) == MEM))
      emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (QImode, ACC_REG)));

    if (!TARGET_MAXQ20 && operands[1] == virtual_stack_vars_rtx)
      emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (QImode, ACC_REG)));

    /* In case of MAXQ10 the move will be modifying the contents of the
       GRL,GRH and GR registers so we emit the clobber for each register in
       parallel to the actual move instruction */
    if (!TARGET_MAXQ20
	&& !(GET_CODE (operands[0]) == MEM
	     && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
	&& !(GET_CODE (operands[1]) == MEM
	     && GET_CODE (XEXP (operands[1], 0)) == POST_INC))
      {
	rtvec vec;
	vec =
	  gen_rtvec (4, gen_rtx_SET (VOIDmode, operands[0], operands[1]),
		     gen_rtx_CLOBBER (VOIDmode,
				      gen_rtx_REG (QImode, GRL_REG)),
		     gen_rtx_CLOBBER (VOIDmode,
				      gen_rtx_REG (QImode, GRH_REG)),
		     gen_rtx_CLOBBER (VOIDmode,
				      gen_rtx_REG (HImode, GR_REG)));
	emit_insn (gen_rtx_PARALLEL (VOIDmode, vec));
	DONE;
      }
  ")

(define_insn "movhi_1"
 [(set (match_operand:HI 0 "memory_operand" "=m,m")
        (match_operand:HI 1 "reg_or_imm_operand" "rbi,*t"))]
  "TARGET_MAXQ20"
  "#")

(define_insn "movhi_10"
 [(set (match_operand:HI 0 "memory_operand" "=m,m")
        (match_operand:HI 1 "reg_or_imm_operand" "rbi,*t"))
  (clobber (reg:QI GRL_REG))
  (clobber (reg:QI GRH_REG))
  (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "#")

(define_insn "movhi_2"
  [(set (match_operand:HI 0 "register_operand" "=rb,*t")
        (match_operand:HI 1 "memory_operand" "m,m"))]
  "TARGET_MAXQ20"
  "#")

(define_insn "movhi_20"
 [(set (match_operand:HI 0 "register_operand" "=rb,*t")
       (match_operand:HI 1 "memory_operand" "m,m"))
  (clobber (reg:QI GRL_REG))
  (clobber (reg:QI GRH_REG))
  (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "#")

(define_insn "reg_mov"
  [(set (match_operand:HI 0 "register_operand" "=rb,*t,rbl,*t")
        (match_operand:HI 1 "register_operand" "rb,rbl,*t,*t"))]
  "TARGET_MAXQ20"
  "move\\t%0, %1")

;MEM1, MEM2 are not hard registers but memory locations.
;Memory @(BP+0) and @(BP+2) are represented by MEM1 and MEM2 registers

(define_insn "mem1_dst_mov"
 [(set (reg:HI MEM1) (reg:HI ACC_REG))]
  "TARGET_MAXQ20"
  "move\\tGR, ACC\\n\\tmove\\tOFFS, #0h\\n\\tmove\\t@BP[OFFS],GRL\\n\\tmove\\t@BP[++OFFS],GRH")

(define_insn "mem1_dst_mov_10"
 [(set (reg:QI MEM1) (reg:QI ACC_REG))]
  "!TARGET_MAXQ20"
  "move\\tOFFS, #0h\\n\\tmove\\t@BP[OFFS],ACC")

(define_insn "mem2_dst_mov"
 [(set (reg:HI MEM2) (reg:HI DP_REG))]
  ""
  "move\\tGR, DP[0]\\n\\tmove\\tOFFS, #2h\\n\\tmove\\t@BP[OFFS],GRL\\n\\tmove\\t@BP[++OFFS],GRH")
 
(define_insn "mem1_src_mov"
 [(set (reg:HI ACC_REG) (reg:HI MEM1))]
  "TARGET_MAXQ20"
  "move\\tOFFS, #0h\\n\\tmove\\tGRL, @BP[OFFS++]\\n\\tmove\\tGRH, @BP[OFFS]\\n\\tmove\\tACC, GR")

(define_insn "mem1_arc_mov_10"
 [(set (reg:QI ACC_REG) (reg:QI MEM1))]
  "!TARGET_MAXQ20"
  "move\\tOFFS, #0h\\n\\tmove\\tACC, @BP[OFFS]")

(define_insn "mem2_src_mov"
 [(set (reg:HI DP_REG) (reg:HI MEM2))]
  ""
  "move\\tOFFS, #2h\\n\\tmove\\tGRL, @BP[OFFS++]\\n\\tmove\\tGRH, @BP[OFFS]\\n\\tmove\\tDP[0], GR")

(define_insn "move_gr"
 [(set (reg:HI GR_REG) (match_operand:HI 0 "reg_or_imm_operand" "rb,*t,i"))
  (clobber (reg:QI GRH_REG))
  (clobber (reg:QI GRL_REG))
  (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "*
  {
  if (!IS_MAXQ10_DEST (operands[0]))
    return \"move\\tGR, %0\";
  else
    return \"move\\tGRL, %L0\\n\\tmove\\tGRH, %H0\";
  }
  ")

(define_insn "reg_mov_q10"
  [(set (match_operand:HI 0 "register_operand" "=rb,*t,rb,*t")
        (match_operand:HI 1 "register_operand"  "rb,rb,*t,*t"))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "*
  {
  if (REGNO (operands[0]) == REGNO (operands[1]))
    {
      return \"\";
    }

  if (IS_MAXQ10_DEST (operands[0])
      && (!IS_MAXQ10_DEST (operands[1])
           || REGNO (operands[1]) == ACC_REG))
    {
      return \"move\\tGR, %1\\n\\tmove\\t%L0, GRL\\n\\tmove\\t%H0, GRH\";
    }
  else if (IS_MAXQ10_DEST (operands[1])
           && (!IS_MAXQ10_DEST (operands[0])
                || REGNO (operands[0]) == ACC_REG))
    {
      return \"move\\tGRL, %L1\\n\\tmove\tGRH, %H1\\n\\tmove\\t%0, GR\";
    }
  else if (IS_MAXQ10_DEST (operands[0])
           && IS_MAXQ10_DEST (operands[1]))
    {
      return \"move\\tGRL, %L1\\n\\tmove\tGRH, %H1\\n\\tmove\\t%L0,GRL\\n\\tmove\\t%H0, GRH\";
    }

  else if (!IS_MAXQ10_DEST (operands[0])
        && !IS_MAXQ10_DEST (operands[1]))
    {
      return \"move\\t%0, %1\";
    }
   else
      return \"\";
  }")


(define_insn "move_gr_gr"
  [(set (reg:HI GR_REG) (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "")

(define_insn "move_gr_1"
 [(set (match_operand:HI 0 "register_operand" "=rb,*t") (reg:HI GR_REG))
  (use (reg:QI GRL_REG))
  (use (reg:QI GRH_REG))]
  "!TARGET_MAXQ20"
  "*
  {
  if (!IS_MAXQ10_DEST (operands[0]) )
    {
      return \"move\\t%0, GR\";
    }
  else
    {
      return \"move\\t%L0, GRL\\n\\tmove\\t%H0, GRH\";
    }
  }
  ")

(define_insn "dpc_mov"
  [(set (reg:HI DPC_REG)
        (match_operand:HI 0 "immediate_operand" ""))]
  ""
  "move\\tDPC, %0")

(define_insn "reg_mov_1"
  [(set (match_operand:HI 0 "register_operand" "=rb,*t")
        (match_operand:HI 1 "immediate_operand" ""))]
  "TARGET_MAXQ20"
  "move\\t%0, %1")

(define_insn "reg_mov_1_q10"
  [(set (match_operand:HI 0 "register_operand" "=rb,*t")
        (match_operand:HI 1 "immediate_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "*
  {
    if (!IS_MAXQ10_DEST (operands[0]))
      return \"move\\t%0, %1\"; 
    else
      return \"move\\tGR,%1\\n\\tmove\\t%L0,GRL\\n\\tmove\\t%H0,GRH\";
  }
  ")

(define_insn ""
  [(set (mem:HI (reg:HI DP_REG))
        (match_operand:HI 0 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (mem:HI (reg:HI DP_REG))
        (match_operand:HI 0 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
   "!TARGET_MAXQ20"
   "#")

; There is no direct memory move on MAXQ for HImode, We use GR register as temp. register
; Since auto increment is used DP register is also affected
; Data pointer selection is also implicitly changed by mem write operation 
(define_split
  [(set (mem:HI  (reg:HI DP_REG)) 
        (match_operand:HI 0 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  [
   (clobber (reg:HI DPC_REG))
   (set (reg:HI GR_REG) (match_dup 0) )
   (parallel[(set (mem:HI (reg:HI DP_REG)) (reg:HI GR_REG))
             (clobber (reg:HI DPC_REG))
             (clobber (reg:HI DP_REG))
             (clobber (reg:HI GR_REG))])]
  "")

(define_split
  [(set (mem:HI  (reg:HI DP_REG)) 
        (match_operand:HI 0 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20"
  [
   (clobber (reg:HI DPC_REG))
   (parallel [(set (reg:HI GR_REG) (match_dup 0) )
              (clobber (reg:QI GRH_REG))
   	      (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (parallel [(set (mem:HI (reg:HI DP_REG)) (reg:HI GR_REG))
              (clobber (reg:HI DPC_REG))
              (clobber (reg:HI DP_REG))
              (clobber (reg:HI GR_REG))])]
  "")

(define_insn "mem_mov_dst_dp_gr"
  [(set (mem:HI (reg:HI DP_REG))
        (reg:HI GR_REG))
   (clobber (reg:HI DPC_REG))
   (clobber (reg:HI DP_REG))
   (clobber (reg:HI GR_REG))]
  ""
  "move\\t@DP[0],GRL\\n\\tmove\\t@++DP[0],GRH")


(define_insn ""
  [(set (mem:HI (reg:HI BP_REG))
        (match_operand:HI 0 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (mem:HI (reg:HI BP_REG))
        (match_operand:HI 0 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20"
  "#")

; use GR register as temp. register
; only @BP[OFFS] is valid addressing mode, where OFFS is hard register
; move 0 to OFFS register to access the memory @BP
 
(define_split
  [(set (mem:HI (reg:HI BP_REG))
        (match_operand:HI 0 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  [
   (set (reg:HI GR_REG) (match_dup 0))
   (set (reg:HI OFF_REG)(const_int 0))
   (parallel[(set (mem:HI (plus:HI (reg:HI BP_REG)(reg:HI OFF_REG))) (reg:HI GR_REG))
	     (use (reg:HI OFF_REG))
	     (clobber (reg:HI GR_REG))])]
  "")

(define_split
  [(set (mem:HI (reg:HI BP_REG))
        (match_operand:HI 0 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  [
   (parallel [(set (reg:HI GR_REG) (match_dup 0))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (set (reg:HI OFF_REG)(const_int 0))
   (set (reg:HI DPC_REG)(const_int 2))
   (parallel[(set (mem:HI (plus:HI (reg:HI BP_REG)(reg:HI OFF_REG))) (reg:HI GR_REG))
	     (use (reg:HI OFF_REG))
	     (clobber (reg:HI GR_REG))])]
  "")


(define_insn ""
  [(set (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 0 "immediate_operand" "")))
        (match_operand:HI 1 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 0 "immediate_operand" "")))
        (match_operand:HI 1 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20"
  "#")

; GR is temp register.
; OFFS is 8-bit register, so move the offset value to OFFS only if offset <=254

(define_split
  [(set (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 0 "immediate_operand" "")))
        (match_operand:HI 1 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20 && INTVAL (operands[0]) >= 0 && INTVAL (operands[0]) <= 254"
  [
   (set (reg:HI GR_REG) (match_dup 1))
   (set (reg:HI OFF_REG)(match_dup 0))
   (parallel [(set (mem:HI (plus:HI (reg:HI BP_REG)(reg:HI OFF_REG))) (reg:HI GR_REG))
             (use (reg:HI OFF_REG))
             (clobber (reg:HI GR_REG))])
  ]
  "")

(define_split
  [(set (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 0 "immediate_operand" "")))
        (match_operand:HI 1 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20 && INTVAL (operands[0]) >= 0 && INTVAL (operands[0]) <= 254"
  [
   (parallel [(set (reg:HI GR_REG) (match_dup 1) )
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG)) ])
   (set (reg:HI OFF_REG)(match_dup 0) )
   (set (reg:HI DPC_REG)(const_int 2) )
   (parallel [(set (mem:HI (plus:HI (reg:HI BP_REG)(reg:HI OFF_REG))) (reg:HI GR_REG))
              (use (reg:HI OFF_REG))
              (clobber (reg:HI GR_REG))])
  ]
  "")

;; In case the offset value is greater than 254 then add the contents of the base and
;; offset register and then access the memory location through DP register (@DP[0])
(define_split
  [(set (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 0 "immediate_operand" "")))
        (match_operand:HI 1 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20 && INTVAL (operands[0]) > 254"
  [
   (set (reg:HI ACC_REG) (reg:HI BP_REG))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 0)))
              (set (reg:HI PSF)(ltu:HI (reg:HI ACC_REG) (match_dup 0)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (mem:HI (reg:HI DP_REG))(match_dup 1))
  ]
  "")

(define_split
  [(set (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 0 "immediate_operand" "")))
        (match_operand:HI 1 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20 && INTVAL (operands[0]) > 254 "
  [
   (parallel [(set (reg:HI GR_REG) (reg:HI BP_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG)) ])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 2)))
              (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 2)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
	      (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])	
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 3)) 
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (parallel [(set (mem:HI (reg:HI DP_REG))(match_dup 1) )
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
  ]
  "
    operands[2] = operand_subword (operands[0], 0, 1, HImode);
    operands[3] = operand_subword (operands[0], 1, 1, HImode);
  ")

; move GR to @BP[OFFS] 

(define_insn "mem_mov_dst_bp_gr"
  [(set (mem:HI (plus:HI (reg:HI BP_REG)(reg:HI OFF_REG)))
        (reg:HI GR_REG) )
   (use (reg:HI OFF_REG)) 
   (clobber (reg:HI GR_REG))] 
  ""
  "move\\t@BP[OFFS],GRL\\n\\tmove\\t@BP[++OFFS],GRH")

; On MAXQ there are only three data pointers BP, DP0, DP1
; Out of these, BP is frame poiner, DP1 is stack pointer
; So to avoid spill failures, We allow all general registers as base registers
; Finally this will be converted into valid addressing modes. 

(define_insn ""
  [(set (mem:HI (plus:HI (match_operand:HI 0 "register_operand" "b")
                         (match_operand:HI 1 "immediate_operand" "")))
        (match_operand:HI 2 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (mem:HI (plus:HI (match_operand:HI 0 "register_operand" "b")
                         (match_operand:HI 1 "immediate_operand" "")))
        (match_operand:HI 2 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "#")

; REG + offset is not a valid addressing mode on MAXQ.
; So first we calculate this value (REG+offset) then generate the memory insn.
; value of REG + offset (operands[0] + operands[1]) is calculated by following insns
; move ACC, REG
; add offset
; move DP[0], ACC
; Result is stored at DP[0], then valid memory move is generated using DP[0]

(define_split
  [(set (mem:HI (plus:HI (match_operand:HI 0 "register_operand" "")
                         (match_operand:HI 1 "immediate_operand" "")))
	(match_operand:HI 2 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20 && !(reload_completed || reload_in_progress)"
  [
   (set (reg:HI ACC_REG) (match_dup 0))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 1)))
              (set (reg:HI PSF)(ltu:HI (reg:HI ACC_REG) (match_dup 1)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (mem:HI (reg:HI DP_REG))(match_dup 2))
  ]
  "")

; ACC and DP registers are used for address calculation
; while reloading these registers need to be saved before using them.
; We use MEM1 and MEM2 registers to save ACC, DP registers
 
(define_split
  [(set (mem:HI (plus:HI (match_operand:HI 0 "register_operand" "")
                         (match_operand:HI 1 "immediate_operand" "")))
        (match_operand:HI 2 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20 && (reload_completed || reload_in_progress)"
  [
   (set (reg:HI MEM1) (reg:HI ACC_REG))
   (set (reg:HI MEM2) (reg:HI DP_REG))
   (set (reg:HI ACC_REG) (match_dup 0))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 1)))
              (set (reg:HI PSF)(ltu:HI (reg:HI ACC_REG) (match_dup 1)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (mem:HI (reg:HI DP_REG))(match_dup 2) )
   (set (reg:HI ACC_REG) (reg:HI MEM1))
   (set (reg:HI DP_REG) (reg:HI MEM2))
  ]
  "")

(define_split
  [(set (mem:HI (plus:HI (match_operand:HI 0 "register_operand" "")
        (match_operand:HI 1 "immediate_operand" "")))
        (match_operand:HI 2 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20 && !(reload_completed || reload_in_progress)"
  [
   (parallel [(set (reg:HI GR_REG) (match_dup 0))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG)) ])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG)) ])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 3)))
    	      (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 3)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG))) ])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 4))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG))) ])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (parallel [(set (mem:HI (reg:HI DP_REG))(match_dup 2) )
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
  ]
  "
    operands[3] = operand_subword (operands[1], 0, 1, HImode);
    operands[4] = operand_subword (operands[1], 1, 1, HImode);
  ")

(define_split
  [(set (mem:HI (plus:HI (match_operand:HI 0 "register_operand" "")
        (match_operand:HI 1 "immediate_operand" "")))
        (match_operand:HI 2 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20 && (reload_completed || reload_in_progress)"
  [

   (set (reg:QI MEM1) (reg:QI ACC_REG))
   (set (reg:HI MEM2) (reg:HI DP_REG))
   (parallel [(set (reg:HI GR_REG) (match_dup 0))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG)) ])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG)) ])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 3)))
              (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 3)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG))) ])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 4))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG))) ])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (parallel [(set (mem:HI (reg:HI DP_REG))(match_dup 2) )
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
   (set (reg:QI ACC_REG) (reg:QI MEM1))
   (set (reg:HI DP_REG) (reg:HI MEM2))
  ]
  "
    operands[3] = operand_subword (operands[1], 0, 1, HImode);
    operands[4] = operand_subword (operands[1], 1, 1, HImode);
  ")


(define_insn ""
  [(set (mem:HI  (match_operand:HI 0 "register_operand" "b"))
        (match_operand:HI 1 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (mem:HI  (match_operand:HI 0 "register_operand" "b"))
        (match_operand:HI 1 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "#")

; move the general register to DP[0] the generate memory move insn. 
(define_split
  [(set (mem:HI  (match_operand:HI 0 "register_operand" ""))
        (match_operand:HI 1 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  [
   (set (reg:HI DP_REG) (match_dup 0))
   (clobber (reg:HI DPC_REG))
   (set (mem:HI (reg:HI DP_REG))(match_dup 1))
  ]
  "")

(define_split
  [(set (mem:HI (match_operand:HI 0 "register_operand" ""))
        (match_operand:HI 1 "reg_or_imm_operand" ""))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20"
  [
   (parallel [(set (reg:HI DP_REG) (match_dup 0))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
   (clobber (reg:HI DPC_REG))
   (parallel [(set (mem:HI (reg:HI DP_REG))(match_dup 1) )
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
  ]
  "")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (reg:HI DP_REG)))]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (reg:HI DP_REG)))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20"
  "#")


(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI  (reg:HI DP_REG)))]
  "TARGET_MAXQ20"
  [
   (set (reg:HI DPC_REG) (const_int 0))
   (parallel [(set (match_dup 0) (mem:HI (reg:HI DP_REG)))
              (clobber (reg:HI DP_REG))
              (clobber (reg:HI GR_REG))
              (use (reg:HI DPC_REG))])
  ]
  "")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI  (reg:HI DP_REG)))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20 && !IS_MAXQ10_DEST (operands[0])"
  [
   (set (reg:HI DPC_REG) (const_int 0))
   (parallel [(set (match_dup 0) (mem:HI  (reg:HI DP_REG)))
              (clobber (reg:HI DP_REG))
              (clobber (reg:HI GR_REG))
              (use (reg:HI DPC_REG))])
  ]
  "")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (reg:HI DP_REG)))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20 && IS_MAXQ10_DEST (operands[0])"
  [
   (set (reg:HI DPC_REG) (const_int 0))
   (parallel [(set (match_dup 2) (mem:QI  (reg:HI DP_REG)))
               (clobber (reg:HI DP_REG))
               (use (reg:HI DPC_REG))])
   (parallel [(set (match_dup 3) (mem:QI  (reg:HI DP_REG)))
              (clobber (reg:HI DP_REG))
              (use (reg:HI DPC_REG))])
  ]
  "
    operands[2] = operand_subword (operands[0], 0, 1, HImode);
    operands[3] = operand_subword (operands[0], 1, 1, HImode);
  ")


(define_insn "mem_mov_src_dp_gr_1"
  [(set (match_operand:HI 0 "register_operand" "=rb,*a") 
        (mem:HI (reg:HI DP_REG)))
   (clobber (reg:HI DP_REG))
   (clobber (reg:HI GR_REG))
   (use (reg:HI DPC_REG))]
  ""
  "*
  {
  if (TARGET_MAXQ20 || !IS_MAXQ10_DEST (operands[0]))
    return \"move\\tGRL,@DP[0]++\\n\\tmove\\tGRH,@DP[0]\\n\\tmove\\t%0,GR\";
  else
    return \"move\\t%L0,@DP[0]++\\n\\tmove\\t%H0,@DP[0]\";
  }
  ")

(define_insn "mem_mov_src_dp_gr"
  [(set (reg:HI GR_REG) (mem:HI (reg:HI DP_REG)))
   (clobber (reg:HI DP_REG))]
  ""
  "move\\tGRL,@DP[0]++\\n\\tmove\\tGRH,@DP[0]")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (reg:HI BP_REG)))]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (reg:HI BP_REG)))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20"
  "#")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (reg:HI BP_REG)))]
  "TARGET_MAXQ20"
  [
   (set (reg:HI OFF_REG)(const_int 0))
   (set (reg:HI DPC_REG)(const_int 2))
   (parallel [(set (match_dup 0) (mem:HI (plus:HI (reg:HI BP_REG) (reg:HI OFF_REG))))
              (use (reg:HI DPC_REG))
              (clobber (reg:HI GR_REG))])
   (clobber (reg:HI DPC_REG))]
  "")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (reg:HI BP_REG)))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20 && !IS_MAXQ10_DEST (operands[0])"
  [
   (set (reg:HI OFF_REG) (const_int 0))
   (set (reg:HI DPC_REG) (const_int 2))
   (parallel [(set (match_dup 0) (mem:HI (plus:HI (reg:HI BP_REG) (reg:HI OFF_REG))))
              (use (reg:HI DPC_REG))
              (clobber (reg:HI GR_REG))])
   (clobber (reg:HI DPC_REG))]
  "")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI  (reg:HI BP_REG)))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20 && IS_MAXQ10_DEST (operands[0])"
  [
   (set (reg:HI OFF_REG) (const_int 0))
   (set (reg:HI DPC_REG) (const_int 2) )
   (parallel[(set (match_dup 2) (mem:QI (plus:HI (reg:HI BP_REG) (reg:HI OFF_REG))) )
             (use (reg:HI DPC_REG))
             (clobber (reg:HI OFF_REG))])
   (clobber (reg:HI OFF_REG))
   (parallel[(set (match_dup 3) (mem:QI (plus:HI (reg:HI BP_REG) (reg:HI OFF_REG))) )
             (use (reg:HI DPC_REG))
             (clobber (reg:HI OFF_REG))])
   (clobber (reg:HI DPC_REG))
  ]
  "
    operands[2] = operand_subword (operands[0], 0, 1, HImode);
    operands[3] = operand_subword (operands[0], 1, 1, HImode);
  ")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 1 "immediate_operand" ""))))]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 1 "immediate_operand" ""))))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20"
  "#")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 1 "immediate_operand" ""))))]
  "TARGET_MAXQ20 && INTVAL (operands[1]) > 254"
  [
   (set (reg:HI ACC_REG) (reg:HI BP_REG))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 1)))
              (set (reg:HI PSF)(ltu:HI (reg:HI ACC_REG) (match_dup 1)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (match_dup 0) (mem:HI (reg:HI DP_REG)))
  ]
  "")


(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (reg:HI BP_REG)(match_operand:HI 1 "immediate_operand" ""))))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20 && INTVAL(operands[1]) > 254"
  [
   (parallel [(set (reg:HI GR_REG) (reg:HI BP_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 2)))
    	      (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 2)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG)) ])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 3))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (parallel [(set (match_dup 0) (mem:HI (reg:HI DP_REG)))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
  ]
  "
    operands[2] = operand_subword (operands[1], 0, 1, HImode);
    operands[3] = operand_subword (operands[1], 1, 1, HImode);
  ")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (reg:HI BP_REG) (match_operand:HI 1 "immediate_operand" ""))))]
  "TARGET_MAXQ20 && (INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 254)" 
  [
   (set (reg:HI OFF_REG)(match_dup 1))
   (set (reg:HI DPC_REG)(const_int 2))
   (parallel[(set (match_dup 0) (mem:HI (plus:HI (reg:HI BP_REG)(reg:HI OFF_REG))))
             (use (reg:HI DPC_REG))
             (clobber (reg:HI GR_REG))])
   (clobber (reg:HI GR_REG))
   (clobber (reg:HI DPC_REG))
  ]
 "")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (reg:HI BP_REG) (match_operand:HI 1 "immediate_operand" ""))))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20 && !IS_MAXQ10_DEST (operands[0]) && (INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 254)" 
  [
   (set (reg:HI OFF_REG) (match_dup 1) )
   (set (reg:HI DPC_REG)(const_int 2))
   (parallel[(set (match_dup 0) (mem:HI (plus:HI (reg:HI BP_REG)(reg:HI OFF_REG))) )
             (use (reg:HI DPC_REG))
             (clobber (reg:HI GR_REG))])
   (clobber (reg:HI DPC_REG))
  ]
  "")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (reg:HI BP_REG) (match_operand:HI 1 "immediate_operand" ""))))
    (clobber (reg:QI GRL_REG))
    (clobber (reg:QI GRH_REG))
    (clobber (reg:HI GR_REG))]
  "(!TARGET_MAXQ20 && IS_MAXQ10_DEST (operands[0])) && (INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 254)" 
  [
   (set (reg:HI OFF_REG) (match_dup 1))
   (set (reg:HI DPC_REG) (const_int 2))
   (parallel[(set (match_dup 2) (mem:QI (plus:HI (reg:HI BP_REG) (reg:HI OFF_REG))))
             (use (reg:HI DPC_REG))
             (clobber (reg:HI OFF_REG))])
   (clobber (reg:HI OFF_REG))
   (parallel[(set (match_dup 3) (mem:QI (plus:HI (reg:HI BP_REG) (reg:HI OFF_REG))))
             (use (reg:HI DPC_REG))
             (clobber (reg:HI OFF_REG))])
   (clobber (reg:HI DPC_REG))
  ]
  "
    operands[2] = operand_subword (operands[0], 0, 1, HImode);
    operands[3] = operand_subword (operands[0], 1, 1, HImode);
  ")

(define_insn "mem_mov_src_bp_gr_1"
  [(set (match_operand:HI 0 "register_operand" "=rb,*a") 
	(mem:HI (plus:HI (reg:HI BP_REG) (reg:HI OFF_REG))))
   (use (reg:HI DPC_REG))
   (clobber (reg:HI GR_REG))]
  ""
  "*
  {
  if (TARGET_MAXQ20 || !IS_MAXQ10_DEST (operands[0]))
    return \"move\\tGRL,@BP[OFFS++]\\n\\tmove\\tGRH,@BP[OFFS]\\n\\tmove\\t%0,GR\";
  else
    return \"move\\t%L0,@BP[OFFS++]\\n\\tmove\\t%H0,@BP[OFFS]\";
  }
  ")

(define_insn "mem_mov_src_bp_gr"
  [(set (reg:HI GR_REG) (mem:HI (plus:HI (reg:HI BP_REG) (reg:HI OFF_REG))))
   (use (reg:HI DPC_REG))]
  ""
  "move\\tGRL,@BP[OFFS++]\\n\\tmove\\tGRH,@BP[OFFS]")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (match_operand:HI 1 "register_operand" "b")
        (match_operand:HI 2 "immediate_operand" ""))))]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (match_operand:HI 1 "register_operand" "b")
        (match_operand:HI 2 "immediate_operand" ""))))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "#")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (match_operand:HI 1 "register_operand" "")
        (match_operand:HI 2 "immediate_operand" ""))))]
  "TARGET_MAXQ20 && !(reload_completed || reload_in_progress)"
  [
   (set (reg:HI ACC_REG) (match_dup 1))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 2)))
              (set (reg:HI PSF)(ltu:HI (reg:HI ACC_REG) (match_dup 2)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (match_dup 0) (mem:HI (reg:HI DP_REG)))
  ]
  "")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
	(mem:HI (plus:HI (match_operand:HI 1 "register_operand" "")
        (match_operand:HI 2 "immediate_operand" ""))))]
  "TARGET_MAXQ20 && (reload_completed || reload_in_progress)"
  [
   (set (reg:HI MEM1) (reg:HI ACC_REG))
   (set (reg:HI MEM2) (reg:HI DP_REG))
   (set (reg:HI ACC_REG) (match_dup 1))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 2)))
              (set (reg:HI PSF)(ltu:HI (reg:HI ACC_REG) (match_dup 2)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (match_dup 0) (mem:HI (reg:HI DP_REG)))
   (set (reg:HI ACC_REG) (reg:HI MEM1))
   (set (reg:HI DP_REG) (reg:HI MEM2))
  ]
  "")


(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (match_operand:HI 1 "register_operand" "")
        (match_operand:HI 2 "immediate_operand" ""))))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20 && !(reload_completed || reload_in_progress)"
  [
   (parallel [(set (reg:HI GR_REG) (match_dup 1))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 3)))
              (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 3)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 4))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
    (clobber (reg:HI DPC_REG))
    (parallel [(set (match_dup 0) (mem:HI (reg:HI DP_REG)))
               (clobber (reg:QI GRL_REG))
               (clobber (reg:QI GRH_REG))
               (clobber (reg:HI GR_REG))])
  ]
  "
    operands[3] = operand_subword (operands[2], 0, 1, HImode);
    operands[4] = operand_subword (operands[2], 1, 1, HImode);
  ")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI (plus:HI (match_operand:HI 1 "register_operand" "")
        (match_operand:HI 2 "immediate_operand" ""))))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20 && (reload_completed || reload_in_progress)"
  [
   (set (reg:QI MEM1) (reg:QI ACC_REG))
   (set (reg:HI MEM2) (reg:HI DP_REG))
   (parallel [(set (reg:HI GR_REG) (match_dup 1))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 3)))
              (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 3)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 4))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
    (clobber (reg:HI DPC_REG))
    (parallel [(set (match_dup 0) (mem:HI (reg:HI DP_REG)))
               (clobber (reg:QI GRL_REG))
               (clobber (reg:QI GRH_REG))
               (clobber (reg:HI GR_REG))])
   (set (reg:QI ACC_REG) (reg:QI MEM1))
   (set (reg:HI DP_REG) (reg:HI MEM2))
  ]
  "
    operands[3] = operand_subword (operands[2], 0, 1, HImode);
    operands[4] = operand_subword (operands[2], 1, 1, HImode);
  ")


(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI  (match_operand:HI 1 "register_operand" "b")) )]
  "TARGET_MAXQ20"
  "#")

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI  (match_operand:HI 1 "register_operand" "b")))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG)) ]
  "!TARGET_MAXQ20"
  "#")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI  (match_operand:HI 1 "register_operand" "")))]
  "TARGET_MAXQ20"
  [
   (set (reg:HI DP_REG) (match_dup 1))
   (clobber (reg:HI DPC_REG))
   (set (match_dup 0) (mem:HI (reg:HI DP_REG)))
  ]
  "")

(define_split
  [(set (match_operand:HI 0 "register_operand" "")
        (mem:HI  (match_operand:HI 1 "register_operand" "")))
   (clobber (reg:QI GRL_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  [
   (parallel [(set (reg:HI DP_REG) (match_dup 1))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
   (clobber (reg:HI DPC_REG))
   (parallel [(set (match_dup 0) (mem:HI (reg:HI DP_REG)))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
  ]
  "")

(define_insn "off_mov"
  [(set (reg:HI OFF_REG)
        (match_operand:HI 0 "immediate_operand" ""))]
  ""
  "move\\tOFFS, %0")

(define_expand "movqi"
  [(set (match_operand:QI 0 "general_operand" "")
        (match_operand:QI 1 "general_operand"  ""))]
  ""
  "
    {
      if (!reload_completed && GET_CODE (operands[0]) == MEM
	  && GET_CODE (operands[1]) == MEM)
	operands[1] = force_reg (QImode, operands[1]);

      if (TARGET_MAXQ20
	  && (GET_CODE (operands[0]) == MEM || GET_CODE (operands[1]) == MEM))
	emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (HImode, ACC_REG)));

      if (!TARGET_MAXQ20
	  && (GET_CODE (operands[0]) == MEM || GET_CODE (operands[1]) == MEM))
	{
	  emit_insn (gen_rtx_CLOBBER
		     (VOIDmode, gen_rtx_REG (QImode, ACC_REG)));
	  emit_insn (gen_rtx_CLOBBER
		     (VOIDmode, gen_rtx_REG (HImode, GR_REG)));
	  emit_insn (gen_rtx_CLOBBER
		     (VOIDmode, gen_rtx_REG (QImode, GRL_REG)));
	  emit_insn (gen_rtx_CLOBBER
		     (VOIDmode, gen_rtx_REG (QImode, GRH_REG)));
	}
    }
  ")

(define_insn "movqi_1"
  [(set (match_operand:QI 0 "memory_operand" "=m,m")
        (match_operand:QI 1 "reg_or_imm_operand" "rbi,*t"))]
  "!((GET_CODE (operands[0]) == MEM) && (REGNO (XEXP (operands[0], 0)) == DP_REG))"
  "#")

(define_insn "moveq1_2"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t")
        (match_operand:QI 1 "memory_operand" "m,m"))]
  "!((GET_CODE (operands[1]) == MEM) && (REGNO (XEXP (operands[1], 0)) == DP_REG))"
  "#")

(define_insn "reg_mov_qi"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t,rbl,*t")
        (match_operand:QI 1 "register_operand" "rb,rbl,*t,*t"))]
  "TARGET_MAXQ20"
  "move\\t%0, %1")

(define_insn "reg_mov_qi_10"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t,rbl,*t")
        (match_operand:QI 1 "register_operand" " rb,rbl,*t,*t"))]
  "!TARGET_MAXQ20"
  "move\\t%0, %1")

(define_insn "move_grh"
 [(set (reg:QI GRH_REG) (match_operand:QI 0 "reg_or_imm_operand" "r,*a,i"))
  (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))]
  "!TARGET_MAXQ20"
  "move\\tGRH, %0")

(define_insn "move_grl"
 [(set (reg:QI GRL_REG) (match_operand:QI 0 "reg_or_imm_operand" "r,*a,i"))
  (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))]
  "!TARGET_MAXQ20"
  "move\\tGRL, %0")

(define_insn "move_grh_1"
 [(set (match_operand:QI 0 "register_operand" "=r,*a") (reg:QI GRH_REG))
  (use (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "move\\t%0,GRH" )

(define_insn "move_grl_1"
 [(set (match_operand:QI 0 "register_operand" "=r,*a") (reg:QI GRL_REG))
  (use (reg:HI GR_REG))]
  "!TARGET_MAXQ20"
  "move\\t%0,GRL")

(define_insn "reg_mov_qi_1"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t")
        (match_operand:QI 1 "immediate_operand" ""))]
  ""
  "move\\t%0, %1")

(define_insn  "mem_mov_dst_dp_qi"
  [(set (mem:QI  (reg:HI DP_REG)) 
        (match_operand:QI 0 "reg_or_imm_operand" "rb,*t"))]
  ""
  "move\\t@DP[0], %0")

(define_insn ""
  [(set (mem:QI  (reg:HI BP_REG))
        (match_operand:QI 0 "reg_or_imm_operand" ""))]
  ""
  "#")

(define_split
  [(set (mem:QI  (reg:HI BP_REG))
        (match_operand:QI 0 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  [
   (set (reg:HI DPC_REG) (const_int 2))
   (parallel [(set (mem:QI (plus:HI (reg:HI BP_REG) (const_int 0))) (match_dup 0))
             (use (reg:HI OFF_REG))])
   (clobber (reg:HI DPC_REG))
  ]
  "")

(define_split
  [(set (mem:QI  (reg:HI BP_REG))
        (match_operand:QI 0 "reg_or_imm_operand" ""))]
  "!TARGET_MAXQ20 && reload_completed"
  [
   (set (reg:HI DPC_REG) (const_int 2))
   (parallel [(set (mem:QI (plus:HI (reg:HI BP_REG) (const_int 0))) (match_dup 0))
              (use (reg:HI OFF_REG))])
   (clobber (reg:HI DPC_REG))
  ]
  "")

(define_insn ""
  [(set (mem:QI (plus:HI (reg:HI BP_REG) (match_operand:HI 0 "immediate_operand" "")))
        (match_operand:QI 1 "reg_or_imm_operand" ""))]
  ""
  "#")

(define_split
  [(set (mem:QI (plus:HI (reg:HI BP_REG) (match_operand:HI 0 "immediate_operand" "")))
        (match_operand:QI 1 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20 && INTVAL (operands[0]) >= 0 && INTVAL (operands[0]) <= 255"
  [
   (set (reg:HI DPC_REG) (const_int 2))
   (parallel [(set (mem:QI (plus:HI (reg:HI BP_REG) (match_dup 0))) (match_dup 1))
              (use (reg:HI OFF_REG))])
   (clobber (reg:HI DPC_REG))
  ]
  "")

(define_split
  [(set (mem:QI (plus:HI (reg:HI BP_REG) (match_operand:HI 0 "immediate_operand" "")))
        (match_operand:QI 1 "reg_or_imm_operand" ""))]
  "!TARGET_MAXQ20 && INTVAL (operands[0]) >= 0 && INTVAL (operands[0]) <= 255 && reload_completed"
  [
   (set (reg:HI DPC_REG) (const_int 2))
   (parallel [(set (mem:QI (plus:HI (reg:HI BP_REG) (match_dup 0))) (match_dup 1))
              (use (reg:HI OFF_REG))])
   (clobber (reg:HI DPC_REG))
  ]
  "")

(define_split
  [(set (mem:QI (plus:HI (reg:HI BP_REG) (match_operand:HI 0 "immediate_operand" "")))
        (match_operand:QI 1 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20 && INTVAL (operands[0]) > 254"
  [
   (set (reg:HI ACC_REG) (reg:HI BP_REG))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 0)))
              (set (reg:HI PSF)(ltu:HI (reg:HI ACC_REG) (match_dup 0)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (mem:QI (reg:HI DP_REG))(match_dup 1))
  ]
  "")

(define_split
  [(set (mem:QI (plus:HI (reg:HI BP_REG)(match_operand:HI 0 "immediate_operand" "")))
        (match_operand:QI 1 "reg_or_imm_operand" ""))]
  "!TARGET_MAXQ20 && INTVAL (operands[0]) > 254"
  [
   (parallel [(set (reg:HI GR_REG) (reg:HI BP_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 2)))
              (set (reg:HI PSF) (ltu:HI (reg:QI ACC_REG) (match_dup 2)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 3))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (set (mem:QI (reg:HI DP_REG))(match_dup 1))
   (clobber (reg:HI GR_REG))
  ]
  "
    operands[2] = operand_subword (operands[0], 0, 1, HImode);
    operands[3] = operand_subword (operands[0], 1, 1, HImode);
  ")

(define_insn "mem_mov_dst_bp_gr_qi"
  [(set (mem:QI (plus:HI (reg:HI BP_REG) (match_operand:HI 0 "immediate_operand" "")))
        (match_operand:QI 1 "reg_or_imm_operand" "rbi,*t"))
   (use (reg:HI OFF_REG))]
  ""
  "move\\tOFFS,%0\\n\\tmove\\t@BP[OFFS], %1")

(define_insn ""
  [(set (mem:QI (plus:HI (match_operand:HI 0 "register_operand" "b")
        (match_operand:HI 1 "immediate_operand" "")))
        (match_operand:QI 2 "reg_or_imm_operand" ""))]
  ""
  "#")

(define_split
  [(set (mem:QI (plus:HI (match_operand:HI 0 "register_operand" "")
        (match_operand:HI 1 "immediate_operand" "")))
	(match_operand:QI 2 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20 && !(reload_completed || reload_in_progress)"
  [
   (set (reg:HI ACC_REG) (match_dup 0))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 1)))
              (set (reg:HI PSF) (ltu:HI (reg:HI ACC_REG) (match_dup 1)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (mem:QI (reg:HI DP_REG)) (match_dup 2))
  ]
  "")

(define_split
  [(set (mem:QI (plus:HI (match_operand:HI 0 "register_operand" "")
        (match_operand:HI 1 "immediate_operand" "")))
        (match_operand:QI 2 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20 && (reload_completed || reload_in_progress)"
  [
   (set (reg:HI MEM1) (reg:HI ACC_REG))
   (set (reg:HI MEM2) (reg:HI DP_REG))
   (set (reg:HI ACC_REG) (match_dup 0))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 1)))
              (set (reg:HI PSF) (ltu:HI (reg:HI ACC_REG) (match_dup 1)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (mem:QI (reg:HI DP_REG)) (match_dup 2))
   (set (reg:HI ACC_REG) (reg:HI MEM1))
   (set (reg:HI DP_REG) (reg:HI MEM2))
  ]
  "")

(define_split
  [(set (mem:QI (plus:HI (match_operand:HI 0 "register_operand" "")
        (match_operand:HI 1 "immediate_operand" "")))
        (match_operand:QI 2 "reg_or_imm_operand" ""))]
  "!TARGET_MAXQ20 && !(reload_completed || reload_in_progress)"
  [
   (parallel [(set (reg:HI GR_REG) (match_dup 0))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 3)))
              (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 3)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 4))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (set (mem:QI (reg:HI DP_REG))(match_dup 2))
   (clobber (reg:HI GR_REG))
  ]
  "
    operands[3] = operand_subword (operands[1], 0, 1, HImode);
    operands[4] = operand_subword (operands[1], 1, 1, HImode);
  ")

(define_split
  [(set (mem:QI (plus:HI (match_operand:HI 0 "register_operand" "")
        (match_operand:HI 1 "immediate_operand" "")))
        (match_operand:QI 2 "reg_or_imm_operand" ""))]
  "!TARGET_MAXQ20 && (reload_completed || reload_in_progress)"
  [
   (set (reg:QI MEM1) (reg:QI ACC_REG))
   (set (reg:HI MEM2) (reg:HI DP_REG))
   (parallel [(set (reg:HI GR_REG) (match_dup 0))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 3)))
              (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 3)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 4))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (set (mem:QI (reg:HI DP_REG))(match_dup 2))
   (clobber (reg:HI GR_REG))
   (set (reg:QI ACC_REG) (reg:QI MEM1))
   (set (reg:HI DP_REG) (reg:HI MEM2))
  ]
  "
    operands[3] = operand_subword (operands[1], 0, 1, HImode);
    operands[4] = operand_subword (operands[1], 1, 1, HImode);
  ")

(define_insn ""
  [(set (mem:QI  (match_operand:HI 0 "register_operand" "b"))
        (match_operand:QI 1 "reg_or_imm_operand" ""))]
  ""
  "#")

(define_split
  [(set (mem:QI (match_operand:HI 0 "register_operand" ""))
        (match_operand:QI 1 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20"
  [
   (set (reg:HI DP_REG) (match_dup 0))
   (clobber (reg:HI DPC_REG))
   (set (mem:QI (reg:HI DP_REG)) (match_dup 1))
  ]
  "")

(define_split
  [(set (mem:QI  (match_operand:HI 0 "register_operand" ""))
        (match_operand:QI 1 "reg_or_imm_operand" ""))]
  "!TARGET_MAXQ20"
  [
   (parallel [(set (reg:HI DP_REG) (match_dup 0))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
   (clobber (reg:HI DPC_REG))
   (set (mem:QI (reg:HI DP_REG))(match_dup 1))
  ]
  "")

(define_insn "mem_mov_src_dp_qi"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t")
        (mem:QI  (reg:HI DP_REG)))]
  "TARGET_MAXQ20"
  "move\\t%0,@DP[0]")

(define_insn "mem_mov_src_dp_qi_10"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t")
        (mem:QI  (reg:HI DP_REG)))
   (clobber (reg:HI DP_REG))
   (use (reg:HI DPC_REG))]
  "!TARGET_MAXQ20"
  "move\\t%0,@DP[0]++")

(define_insn "*"
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (reg:HI BP_REG)))]
  ""
  "#")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (reg:HI BP_REG)))]
  "TARGET_MAXQ20"
  [
   (set (reg:HI DPC_REG) (const_int 2))
   (parallel [(set (match_dup 0) (mem:QI (plus:HI (reg:HI BP_REG) (const_int 0) )))
              (use (reg:HI DPC_REG))
              (use (reg:HI OFF_REG))])
  ]
  "")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI  (reg:HI BP_REG)))]
  "!TARGET_MAXQ20 && reload_completed"
  [
   (set (reg:HI DPC_REG) (const_int 2) )
   (parallel [(set (match_dup 0) (mem:QI (plus:HI (reg:HI BP_REG) (const_int 0))) )
              (use (reg:HI DPC_REG))
              (clobber (reg:HI OFF_REG))])
  ]
  "")

(define_insn ""
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (plus:HI (reg:HI BP_REG) (match_operand:HI 1 "immediate_operand" ""))))]
  ""
  "#")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
	(mem:QI (plus:HI (reg:HI BP_REG) (match_operand:HI 1 "immediate_operand" ""))))]
  "TARGET_MAXQ20 && INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 255"
  [
    (set (reg:HI DPC_REG)(const_int 2) )
    (parallel [(set (match_dup 0) (mem:QI (plus:HI (reg:HI BP_REG)(match_dup 1))))
               (use (reg:HI DPC_REG))
               (use (reg:HI OFF_REG))])
  ]
  "")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
	(mem:QI (plus:HI (reg:HI BP_REG) (match_operand:HI 1 "immediate_operand" ""))))]
  "!TARGET_MAXQ20 && INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 255 && reload_completed"
  [
   (set (reg:HI DPC_REG) (const_int 2) )
   (parallel[(set (match_dup 0) (mem:QI (plus:HI (reg:HI BP_REG) (match_dup 1))))
             (use (reg:HI DPC_REG))
             (clobber (reg:HI OFF_REG))])
  ]
  "")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (plus:HI (reg:HI BP_REG)(match_operand:HI 1 "immediate_operand" ""))))]
  "TARGET_MAXQ20 && INTVAL (operands[1]) > 254"
  [
   (set (reg:HI ACC_REG) (reg:HI BP_REG))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 1)))
              (set (reg:HI PSF) (ltu:HI (reg:HI ACC_REG) (match_dup 1)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (reg:HI DPC_REG) (const_int 0))
   (set (match_dup 0) (mem:QI (reg:HI DP_REG)))
  ]
  "")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (plus:HI (reg:HI BP_REG)(match_operand:HI 1 "immediate_operand" ""))))]
  "!TARGET_MAXQ20 && INTVAL (operands[1]) > 254 && reload_completed"
  [
   (parallel [(set (reg:HI GR_REG) (reg:HI BP_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG)) ])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 2)))
              (set (reg:HI PSF)(ltu:HI (reg:QI ACC_REG) (match_dup 2)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 3))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (set (reg:HI DPC_REG) (const_int 0))
   (parallel [(set (match_dup 0) (mem:QI (reg:HI DP_REG)))
              (clobber (reg:HI DP_REG))
              (use (reg:HI DPC_REG))])
  ]
  "
    operands[2] = operand_subword (operands[1], 0, 1, HImode);
    operands[3] = operand_subword (operands[1], 1, 1, HImode);
  ")

(define_insn "mem_mov_src_bp_qi"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t") 
        (mem:QI (plus:HI (reg:HI BP_REG)(match_operand:HI 1  "immediate_operand" ""))))
        (use (reg:HI DPC_REG))
        (use (reg:HI OFF_REG))]
  "TARGET_MAXQ20"
  "move\\tOFFS, %1\\n\\tmove\\t%0, @BP[OFFS]")

(define_insn "mem_mov_src_bp_qi_10"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t")
        (mem:QI (plus:HI (reg:HI BP_REG) (match_operand:HI 1 "immediate_operand" ""))))
   (use (reg:HI DPC_REG))
   (clobber (reg:HI OFF_REG))]
  "!TARGET_MAXQ20"
  "move\\tOFFS, %1\\n\\tmove\\t%0, @BP[OFFS++]")

(define_insn "mem_mov_src_bp_qi_10_1"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t")
        (mem:QI (plus:HI (reg:HI BP_REG) (reg:HI OFF_REG) )))
        (use (reg:HI DPC_REG))
        (clobber (reg:HI OFF_REG))]
  "!TARGET_MAXQ20"
  "move\\t%0, @BP[OFFS++]")

(define_insn ""
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (plus:HI (match_operand:HI 1 "register_operand" "b")
        (match_operand:HI 2 "immediate_operand" ""))))]
  ""
  "#")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (plus:HI (match_operand:HI 1 "register_operand" "")
        (match_operand:HI 2 "immediate_operand" ""))))]
  "TARGET_MAXQ20 && !(reload_completed || reload_in_progress)"
  [
   (set (reg:HI ACC_REG) (match_dup 1))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 2)))
              (set (reg:HI PSF) (ltu:HI (reg:HI ACC_REG) (match_dup 2)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (reg:HI DPC_REG) (const_int 0))
   (set (match_dup 0) (mem:QI (reg:HI DP_REG)))
  ]
  "")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (plus:HI (match_operand:HI 1 "register_operand" "")
        (match_operand:HI 2 "immediate_operand" ""))))]
  "TARGET_MAXQ20 && (reload_completed || reload_in_progress)"
  [
   (set (reg:HI MEM1) (reg:HI ACC_REG))
   (set (reg:HI MEM2) (reg:HI DP_REG))
   (set (reg:HI ACC_REG) (match_dup 1))
   (parallel [(set (reg:HI ACC_REG) (plus:HI (reg:HI ACC_REG) (match_dup 2)))
              (set (reg:HI PSF) (ltu:HI (reg:HI ACC_REG) (match_dup 2)))])
   (set (reg:HI DP_REG) (reg:HI ACC_REG))
   (clobber (reg:HI DPC_REG))
   (set (reg:HI DPC_REG) (const_int 0))
   (set (match_dup 0) (mem:QI (reg:HI DP_REG)))
   (set (reg:HI ACC_REG) (reg:HI MEM1))
   (set (reg:HI DP_REG) (reg:HI MEM2))
  ]
  "")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (plus:HI (match_operand:HI 1 "register_operand" "")
        (match_operand:HI 2 "immediate_operand" ""))) )]
  "!TARGET_MAXQ20 && !(reload_completed || reload_in_progress)"
  [
   (parallel [(set (reg:HI GR_REG) (match_dup 1))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG)) 
              (clobber (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 3)))
    	      (set (reg:HI PSF) (ltu:HI (reg:QI ACC_REG) (match_dup 3)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 4))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (set (reg:HI DPC_REG) (const_int 0))
   (parallel [(set (match_dup 0) (mem:QI (reg:HI DP_REG)))
              (clobber (reg:HI DP_REG))
              (use (reg:HI DPC_REG))])
  ]
  "
    operands[3] = operand_subword (operands[2], 0, 1, HImode);
    operands[4] = operand_subword (operands[2], 1, 1, HImode);
  ")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI (plus:HI (match_operand:HI 1 "register_operand" "")
        (match_operand:HI 2 "immediate_operand" ""))) )]
  "!TARGET_MAXQ20 && (reload_completed || reload_in_progress)"
  [
   (set (reg:QI MEM1) (reg:QI ACC_REG))
   (set (reg:HI MEM2) (reg:HI DP_REG))
   (parallel [(set (reg:HI GR_REG) (match_dup 1))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRL_REG))
              (use (reg:HI GR_REG))])
   (parallel [(set (reg:QI ACC_REG) (plus:QI (reg:QI ACC_REG) (match_dup 3)))
              (set (reg:HI PSF) (ltu:HI (reg:QI ACC_REG) (match_dup 3)))])
   (parallel [(set (reg:QI GRL_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI ACC_REG) (reg:QI GRL_REG)))])
   (parallel [(set (reg:QI ACC_REG) (reg:QI GRH_REG))
              (use (reg:HI GR_REG))])
   (set (reg:QI ACC_REG)
              (plus:QI (plus:QI (reg:QI ACC_REG)
                                (match_dup 4))
                       (reg:QI PSF)))
   (parallel [(set (reg:QI GRH_REG) (reg:QI ACC_REG))
              (set (reg:HI GR_REG) (xor:HI (reg:QI GR_REG) (reg:QI GRH_REG)))])
   (parallel [(set (reg:HI DP_REG) (reg:HI GR_REG))
              (use (reg:QI GRL_REG))
              (use (reg:QI GRH_REG))])
   (clobber (reg:HI DPC_REG))
   (set (reg:HI DPC_REG) (const_int 0))
   (parallel [(set (match_dup 0) (mem:QI (reg:HI DP_REG)))
              (clobber (reg:HI DP_REG))
              (use (reg:HI DPC_REG))])
   (set (reg:QI ACC_REG) (reg:QI MEM1))
   (set (reg:HI DP_REG) (reg:HI MEM2))
  ]
  "
    operands[3] = operand_subword (operands[2], 0, 1, HImode);
    operands[4] = operand_subword (operands[2], 1, 1, HImode);
  ")

(define_insn ""
  [(set (match_operand:QI 0 "register_operand" "=rl,*t")
        (mem:QI  (match_operand:HI 1 "register_operand" "b,b")))]
  ""
 "#")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI  (match_operand:HI 1 "register_operand" "")))]
  "TARGET_MAXQ20"
  [
   (set (reg:HI DP_REG) (match_dup 1))
   (set (reg:HI DPC_REG) (const_int 0))
   (set (match_dup 0) (mem:QI (reg:HI DP_REG)))
  ]
  "")

(define_split
  [(set (match_operand:QI 0 "register_operand" "")
        (mem:QI  (match_operand:HI 1 "register_operand" "")))]
  "!TARGET_MAXQ20"
  [ 
   (parallel [(set (reg:HI DP_REG) (match_dup 1))
              (clobber (reg:QI GRL_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:HI GR_REG))])
   (set (reg:HI DPC_REG)(const_int 0))
   (parallel [(set (match_dup 0) (mem:QI (reg:HI DP_REG)))
              (clobber (reg:HI DP_REG))
              (use (reg:HI DPC_REG))])
  ]
  "")

(define_expand "extendqihi2"
  [(parallel [(set (match_operand:HI 0 "register_operand" "=rb,rb,*t,*t")
                   (sign_extend:HI (match_operand:QI 1 "reg_or_imm_operand" "rb,*t,rbl,*t")))
              (clobber (reg:HI GR_REG))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG))])] 
  ""
  "")

;; GRXL register contains the sign-extended value of GRL register. So move the 
;; operand 1 to GRL register and then move the GRXL register to GRL register. 
(define_insn "extendqihi2_1"
  [(set (match_operand:HI 0 "register_operand" "=rb,rb,*t,*t")
        (sign_extend:HI (match_operand:QI 1 "reg_or_imm_operand" "rb,*t,rbl,*t")))
   (clobber (reg:HI GR_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:QI GRL_REG))]
  "TARGET_MAXQ20"
  "move\\tGRL,%1\\n\\tmove\\t%0,GRXL"
)

(define_insn "extendqihi2_10"
  [(set (match_operand:HI 0 "register_operand" "=rb,rbl")
        (sign_extend:HI (match_operand:QI 1 "reg_or_imm_operand" "rb,*t")))
   (clobber (reg:HI GR_REG))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:QI GRL_REG))]
  "!TARGET_MAXQ20"
  "*
  {
   if (!IS_MAXQ10_DEST (operands[0]))
     return \"move\\tGRL,%1\\n\\tmove\\t%0,GRXL\";
   else
     return \"move\\tGRL,%1\\n\\tmove\\tGR,GRXL\\n\\tmove\\t%L0,GRL\\n\\tmove\\t%H0,GRH\";
  }
  "
)

(define_expand "zero_extendqihi2"
  [(set (match_operand:HI 0 "register_operand" "=rb,rb,*t,*t")
        (zero_extend:HI (match_operand:QI 1 "reg_or_imm_operand" "rbi,*t,rbli,*t")))]
  ""
  "
    {
      rtx r;
      if (TARGET_MAXQ20)
	{
	  r = gen_rtx_REG (QImode, ACC_REG);
	  emit_move_insn (r, operands[1]);
	  operands[1] = r;
	}
      if (!TARGET_MAXQ20)
	{
	  emit_insn (gen_move_grl (operands[1]));
	  emit_insn (gen_move_grh (GEN_INT (0)));
	  emit_insn (gen_move_gr_1 (operands[0]));
	  DONE;
	}
    }
  ")	

(define_insn "zero_extendqihi2_1"
  [(set (match_operand:HI 0 "register_operand" "=rb,rb,*t,*t")
        (zero_extend:HI (reg:QI ACC_REG)))]
  "TARGET_MAXQ20"
  "and\\t#00ffh\\t\\n\\tmove\\t%0, ACC"
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                          LOGICAL                                                   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; As the accumulator register in MAXQ10 is 8-bits wide so we will do the negation for
;; for MAXQ10 as 
;; low0  = ~low1  + 1
;; high0 = ~high1 + carry
(define_expand "neghi2"
 [(parallel [(set (match_operand:HI 0 "register_operand"    "")
                  (neg:HI (match_operand:HI 1 "general_operand" "")))
             (clobber (reg:CC PSF))])]
  ""
  "
    if (TARGET_MAXQ20)
      {
	rtx r;
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, operands[1]);
	emit_insn (gen_neghi2_1 ());
	emit_move_insn (operands[0], r);
	DONE;
      }
    else
      {
	rtx r, op3, op4, op5, op6;

	if (CHECK_SYMBOL_REF (operands[1]))
	  {
	    operands[1] = force_reg (HImode, operands[1]);
	  }

	op3 = operand_subword (operands[0], 0, 1, HImode);
	op4 = operand_subword (operands[1], 0, 1, HImode);
	op5 = operand_subword (operands[0], 1, 1, HImode);
	op6 = operand_subword (operands[1], 1, 1, HImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_one_cmplqi2_1 ());
	emit_insn (gen_addqi3_1 (GEN_INT (1)));
	emit_insn (gen_pushpsf ());
	emit_move_insn (op3, r);
	emit_move_insn (r, op6);
	emit_insn (gen_one_cmplqi2_1 ());
	emit_insn (gen_poppsf ());
	emit_insn (gen_addc_1_qi (GEN_INT (0)));
	emit_move_insn (op5, r);
	DONE;
      }
  ")

(define_expand "negqi2"
 [(parallel [(set (match_operand:QI 0 "register_operand"    "")
                  (neg:QI (match_operand:QI 1 "general_operand" "")))
             (clobber (reg:CC PSF))])]
  "!TARGET_MAXQ20"
  "
    {
      rtx r;
      r = gen_rtx_REG (QImode, ACC_REG);
      emit_move_insn (r, operands[1]);
      emit_insn (gen_negqi2_1 ());
      emit_move_insn (operands[0], r);
      DONE;
    }
  ")

(define_insn "neghi2_1"
  [(set (reg:HI ACC_REG) (neg:HI (reg:HI ACC_REG)))
   (clobber (reg:CC PSF))
   (use (reg:HI ACC_REG))]
  "TARGET_MAXQ20"
  "neg\\t")

(define_insn "negqi2_1"
 [(set (reg:QI ACC_REG) (neg:QI (reg:QI ACC_REG) ) )
  (clobber (reg:CC PSF))
  (use (reg:QI ACC_REG))]
  "!TARGET_MAXQ20"
  "neg\\t")

(define_expand "one_cmplsi2"
 [(parallel [(set (match_operand:SI 0 "register_operand"    "")
                  (not:SI (match_operand:SI 1 "general_operand" "")))])]
  ""
  "
    if (TARGET_MAXQ20)
      {
	rtx r, op3, op4, op5, op6;
	if (GET_CODE (operands[1]) == MEM)
	  operands[1] = force_reg (SImode, operands[1]);

	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[0], 1, 1, SImode);
	op6 = operand_subword (operands[1], 1, 1, SImode);

	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_one_cmplhi2_1 ());
	emit_move_insn (op3, r);

	emit_move_insn (r, op6);
	emit_insn (gen_one_cmplhi2_1 ());
	emit_move_insn (op5, r);
	DONE;
      }
    else
      {
	rtx r, op3, op4, op5, op6, op7, op8, op9, op10;

	if (CHECK_SYMBOL_REF (operands[1]))
	  {
	    operands[1] = force_reg (SImode, operands[1]);
	  }

	op3 = operand_subword (operands[0], 0, 1, SImode);
	op4 = operand_subword (operands[1], 0, 1, SImode);
	op5 = operand_subword (operands[0], 1, 1, SImode);
	op6 = operand_subword (operands[1], 1, 1, SImode);
	op7 = operand_subword (operands[0], 2, 1, SImode);
	op8 = operand_subword (operands[1], 2, 1, SImode);
	op9 = operand_subword (operands[0], 3, 1, SImode);
	op10 = operand_subword (operands[1], 3, 1, SImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_move_insn (r, op4);
	emit_insn (gen_one_cmplqi2_1 ());
	emit_move_insn (op3, r);
	emit_move_insn (r, op6);
	emit_insn (gen_one_cmplqi2_1 ());
	emit_move_insn (op5, r);
	emit_move_insn (r, op8);
	emit_insn (gen_one_cmplqi2_1 ());
	emit_move_insn (op7, r);
	emit_move_insn (r, op10);
	emit_insn (gen_one_cmplqi2_1 ());
	emit_move_insn (op9, r);
	DONE;
      }
  ")

(define_expand "one_cmplhi2"
  [(parallel [(set (match_operand:HI 0 "register_operand" "")
                   (not:HI (match_operand:HI 1 "general_operand" "")))])]
  ""
  "
    if (TARGET_MAXQ20)
      {
	rtx r;
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, operands[1]);
	emit_insn (gen_one_cmplhi2_1 ());
	emit_move_insn (operands[0], r);
	DONE;
      }
    else
      {
	rtx r;
	operands[1] = force_reg (HImode, operands[1]);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_insn (gen_move_gr (operands[1]));
	emit_insn (gen_move_grl_1 (r));
	emit_insn (gen_one_cmplqi2_1 ());
	emit_insn (gen_move_grl (r));
	emit_insn (gen_move_grh_1 (r));
	emit_insn (gen_one_cmplqi2_1 ());
	emit_insn (gen_move_grh (r));
	emit_insn (gen_move_gr_1 (operands[0]));
	DONE;
      }
  ")

(define_expand "one_cmplqi2"
 [(parallel [(set (match_operand:QI 0 "register_operand"    "")
                  (not:QI (match_operand:QI 1 "general_operand" "")))
             (clobber (reg:CC PSF))])]
  ""
  "
    {
      rtx r;
      r = gen_rtx_REG (QImode, ACC_REG);
      emit_move_insn (r, operands[1]);
      emit_insn (gen_one_cmplqi2_1 ());
      emit_move_insn (operands[0], r);
      DONE;
    }
  ")

(define_insn "one_cmplhi2_1"
  [(set (reg:HI ACC_REG)(not:HI (reg:HI ACC_REG)))
   (clobber (reg:CC PSF))
   (use (reg:HI ACC_REG))]
  "TARGET_MAXQ20"
  "cpl\\t")

(define_insn "one_cmplqi2_1"
  [(set (reg:QI ACC_REG)(not:QI (reg:QI ACC_REG)))
   (clobber (reg:CC PSF))
   (use (reg:QI ACC_REG))]
  ""
  "*
  if(TARGET_MAXQ20)
    return \"cpl\\n\\tand\\t#00ffh\\t\";
  else
    return \"cpl\\t\"; 
  ")

;; MAXQ architeccture provides instructions SLA/SLA2/SLA4 for left shift
;; which shift by 1/2/4 bit respectively. So we emit different instructions
;; depending upon the value by which shift is needed
;; For MAXQ10 as accumulator register is 8 bits, we use a combination of 
;; SLA and RLC (Rotate Left with Carry) to shift left.
(define_expand "ashlhi3"
 [(parallel [(set (match_operand:HI 0 "register_operand"    "")
                  (ashift:HI (match_operand:HI 1 "general_operand" "")
                             (match_operand:HI 2 "general_operand" "")))])]
  ""
  "
    {

      if (TARGET_MAXQ20)
	{
	  rtx r;
	  int no, i, n;
	  r = gen_rtx_REG (HImode, ACC_REG);

	  if (GET_CODE (operands[2]) != CONST_INT)
	    FAIL;
	  else
	    {
	      emit_move_insn (r, operands[1]);
	      no = INTVAL (operands[2]);

	      /* If the no. of bits by which operand 1 has to be shifted is
	         greater than 16 then directly move 0 to the result */
	      if (no >= 16)
		{
		  emit_move_insn (operands[0], GEN_INT (0));
		  DONE;
		}
	      n = no / 4;
	      no -= n * 4;
	      for (i = 0; i < n; i++)
		emit_insn (gen_ashlhi3_1 (GEN_INT (4)));
	      n = no / 2;
	      no -= n * 2;

	      if (n != 0)
		emit_insn (gen_ashlhi3_1 (GEN_INT (2)));

	      if (no != 0)
		emit_insn (gen_ashlhi3_1 (GEN_INT (1)));

	      emit_move_insn (operands[0], r);
	      DONE;
	    }
	}
      else
	{
	  rtx op3, op4, op5, op6, r;
	  int i, no;

	  if (GET_CODE (operands[2]) != CONST_INT)
	    FAIL;
	  else
	    {
	      operands[1] = force_reg (HImode, operands[1]);
	      r = gen_rtx_REG (QImode, ACC_REG);
	      no = INTVAL (operands[2]);

	      /* If the no. of bits by which operand 1 has to be shifted is
	         greater than 16 then directly move 0 to the result */
	      if (no >= 16)
		{
		  emit_move_insn (operands[0], GEN_INT (0));
		  DONE;
		}
	      if (no == 8)
		{
		  op3 = operand_subword (operands[0], 0, 1, HImode);
		  op4 = operand_subword (operands[1], 0, 1, HImode);
		  op5 = operand_subword (operands[0], 1, 1, HImode);
		  op6 = operand_subword (operands[1], 1, 1, HImode);
		  emit_move_insn (op3, GEN_INT (0));
		  emit_move_insn (op5, op4);
		  DONE;
		}

	      emit_insn (gen_move_gr (operands[1]));
	      for (i = 0; i < no; i++)
		{
		  emit_insn (gen_move_grl_1 (r));
		  emit_insn (gen_ashlhi3_10 (GEN_INT (1)));
		  emit_insn (gen_move_grl (r));
		  emit_insn (gen_move_grh_1 (r));
		  emit_insn (gen_ashlhi3_10 (GEN_INT (2)));
		  emit_insn (gen_move_grh (r));
		}
	      emit_insn (gen_move_gr_1 (operands[0]));
	      DONE;
	    }
	}
    }
  ")

(define_insn "ashlhi3_1"
 [(set (reg:HI ACC_REG)
             (ashift:HI (reg:HI ACC_REG)
             (match_operand:HI 0 "immediate_operand" "")))
  (use (reg:HI ACC_REG))]
  "TARGET_MAXQ20"
  "*
  if (INTVAL (operands[0]) == 1)
    return \"sla\\t\";
  else if (INTVAL (operands[0]) == 2)
    return \"sla2\\t \";
  else if (INTVAL (operands[0]) == 4)
    return \"sla4\\t \";
  else
    return \"\";
  ")

(define_insn "ashlhi3_10"
 [(set (reg:QI ACC_REG)
             (ashift:QI (reg:QI ACC_REG)
             (match_operand:QI 0 "immediate_operand" "")))
  (use (reg:QI ACC_REG))]
  "!TARGET_MAXQ20"
  "*
  if (INTVAL (operands[0]) == 1)
    return \"sla\\t\";
  else if (INTVAL (operands[0]) == 2)
    return \"rlc\\t \";
  else
    return \"\";")

;; MAXQ architeccture provides instructions SRA/SRA2/SRA4 for right shift
;; which shift by 1/2/4 bit respectively. So we emit different instructions
;; depending upon the value by which shift is needed
;; For MAXQ10 as accumulator register is 8 bits, we use a combination of 
;; SRA and RRC (Rotate Right with Carry) to shift right.
(define_expand "ashrhi3"
 [(parallel [(set (match_operand:HI 0 "register_operand" "")
                  (ashiftrt:HI (match_operand:HI 1 "general_operand" "")
                               (match_operand:HI 2 "general_operand" "")))])]
  ""
  "
    if (TARGET_MAXQ20)
      {
	rtx r;
	int no, i, n;
	r = gen_rtx_REG (HImode, ACC_REG);
	if (GET_CODE (operands[2]) != CONST_INT)
	  FAIL;
	else
	  {
	    emit_move_insn (r, operands[1]);
	    no = INTVAL (operands[2]);
	    if (no > 16)
	      no = 16;
	    n = no / 4;
	    no -= n * 4;
	    for (i = 0; i < n; i++)
	      emit_insn (gen_ashrhi3_1 (GEN_INT (4)));
	    n = no / 2;
	    no -= n * 2;
	    if (n != 0)
	      emit_insn (gen_ashrhi3_1 (GEN_INT (2)));
	    if (no != 0)
	      emit_insn (gen_ashrhi3_1 (GEN_INT (1)));
	    emit_move_insn (operands[0], r);
	    DONE;
	  }
      }
    else
      {
	rtx r;
	int i, no;

	if (GET_CODE (operands[2]) != CONST_INT)
	  FAIL;
	else
	  {
	    operands[1] = force_reg (HImode, operands[1]);
	    r = gen_rtx_REG (QImode, ACC_REG);
	    no = INTVAL (operands[2]);
	    if (no > 16)
	      no = 16;
	    emit_insn (gen_move_gr (operands[1]));
	    for (i = 0; i < no; i++)
	      {
		emit_insn (gen_move_grh_1 (r));
		emit_insn (gen_ashrhi3_10 (GEN_INT (1)));
		emit_insn (gen_move_grh (r));
		emit_insn (gen_move_grl_1 (r));
		emit_insn (gen_ashrhi3_10 (GEN_INT (2)));
		emit_insn (gen_move_grl (r));
	      }
	    emit_insn (gen_move_gr_1 (operands[0]));
	    DONE;
	  }
      }
  ")

(define_insn "ashrhi3_1"
 [(set (reg:HI ACC_REG)
             (ashiftrt:HI (reg:HI ACC_REG)
                          (match_operand:HI 0 "immediate_operand" "")))
  (use (reg:HI ACC_REG))]
  "TARGET_MAXQ20"
  "*
  if (INTVAL (operands[0]) == 1)
    return \"sra\\t\";
  else if (INTVAL (operands[0]) == 2)
    return \"sra2\\t\";
  else if (INTVAL (operands[0]) == 4)
    return \"sra4\\t\";
  else
    return \"\";")

(define_insn "ashrhi3_10"
 [(set (reg:QI ACC_REG)
             (ashiftrt:QI (reg:QI ACC_REG)
                          (match_operand:QI 0 "immediate_operand" "")))
  (use (reg:QI ACC_REG))]
  "!TARGET_MAXQ20"
  "*
  if (INTVAL (operands[0]) == 1)
    return \"sra\\t\";
  else if (INTVAL (operands[0]) == 2)
    return \"rrc\\t\";
  else
    return \"\";")

(define_expand "cmphi"
 [(set (reg:CC PSF)
       (compare:CC (match_operand:HI 0 "reg_or_imm_operand" "rbi")
                   (match_operand:HI 1 "reg_or_imm_operand" "rbi")))]
  "TARGET_MAXQ20"
  "
    maxq_compare_op0 = operands[0];
    maxq_compare_op1 = operands[1];
    emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (HImode, ACC_REG)));
    DONE;")

(define_expand "cmpqi"
 [(set (reg:CC PSF)
       (compare:CC (match_operand:QI 0 "reg_or_imm_operand" "rbi")
                   (match_operand:QI 1 "reg_or_imm_operand" "rbi")))]
  "!TARGET_MAXQ20"
  "
    maxq_compare_op0 = operands[0];
    maxq_compare_op1 = operands[1];
    emit_insn (gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (QImode, ACC_REG)));
    DONE;")

(define_expand "cmphi_1"
 [(set (reg:CC PSF)
       (compare:CC (match_operand:HI 0 "reg_or_imm_operand" "rbi")
                   (match_operand:HI 1 "reg_or_imm_operand" "rbi")))]
  "TARGET_MAXQ20"
  "
    rtx r, tmp;
    if (GET_CODE (operands[0]) == MEM)
      operands[0] = force_reg (HImode, operands[0]);

    if (GET_CODE (operands[1]) == MEM)
      operands[1] = force_reg (HImode, operands[1]);

    if (maxq_signed_cmp)
      {
	tmp = gen_reg_rtx (HImode);
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, operands[1]);
	emit_insn (gen_xorhi3_1 (GEN_INT (-32768)));
	emit_move_insn (tmp, r);
	emit_move_insn (r, operands[0]);
	emit_insn (gen_xorhi3_1 (GEN_INT (-32768)));
	emit_insn (gen_cmphi_2 (tmp));
      }
    else
      {
	r = gen_rtx_REG (HImode, ACC_REG);
	emit_move_insn (r, operands[0]);
	emit_insn (gen_cmphi_2 (operands[1]));
      }
    DONE;
  ")

(define_expand "cmpqi_1"
 [(set (reg:CC PSF)
       (compare:CC (match_operand:QI 0 "reg_or_imm_operand" "rbi")
                   (match_operand:QI 1 "reg_or_imm_operand" "rbi")))]
  "!TARGET_MAXQ20"
  "
    rtx r, tmp;
    if (GET_CODE (operands[0]) == MEM)
      operands[0] = force_reg (QImode, operands[0]);

    if (GET_CODE (operands[1]) == MEM)
      operands[1] = force_reg (QImode, operands[1]);

    if (maxq_signed_cmp)
      {
	tmp = gen_reg_rtx (QImode);
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_move_insn (r, operands[1]);
	emit_insn (gen_xorqi3_1 (GEN_INT (-128)));
	emit_move_insn (tmp, r);
	emit_move_insn (r, operands[0]);
	emit_insn (gen_xorqi3_1 (GEN_INT (-128)));
	emit_insn (gen_cmpqi_2 (tmp));
      }
    else
      {
	r = gen_rtx_REG (QImode, ACC_REG);
	emit_move_insn (r, operands[0]);
	emit_insn (gen_cmpqi_2 (operands[1]));
      }
    DONE;
  ")

;; We are using sub instruction for comparision as the compare instruction
;; on MAXQ architecture just checks for equality/non-equality
;; CMP_REG is a fake register used in the comparision patterns to avoid
;; compiler determining the result of the comparision beforehand and remove
;; some required instructions during optimization
(define_insn "cmphi_2"
 [(set (reg:CC PSF) (compare:CC (minus:HI (reg:HI ACC_REG)
                                (match_operand:HI 0 "reg_or_imm_operand" "rbi"))
                    (reg:HI CMP_REG)))
 (set (reg:HI ACC_REG)(minus:HI (reg:HI ACC_REG) (match_dup 0)))]
  ""
  "sub\\t%0")

(define_insn "cmpqi_2"
 [(set (reg:CC PSF) (compare:CC (minus:QI (reg:QI ACC_REG)
                                (match_operand:QI 0 "reg_or_imm_operand" "rbi"))
                    (reg:QI CMP_REG)))
 (set (reg:QI ACC_REG)(minus:QI (reg:QI ACC_REG) (match_dup 0)))]
  "!TARGET_MAXQ20"
  "sub\\t%0")


(define_insn "indirect_jump"
 [(set (pc) (match_operand:HI 0 "register_operand" "rb"))]
  ""
  "*
  {
    if (!TARGET_MAXQ20 && IS_MAXQ10_DEST (operands[0]))
      return \"move\\tPFX[0],%H0\\n\\tjump\\t%L0\";
    else
      return \"jump\\t%0\";
  }
  ")

(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0 "" "")))]
  ""
  "jump\\t%l0")

(define_expand "ble"
  [(set (pc)
        (if_then_else (le (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    rtx tmp;
    maxq_signed_cmp = 1;
    tmp = maxq_compare_op0;
    maxq_compare_op0 = maxq_compare_op1;
    maxq_compare_op1 = tmp;
    emit_insn (gen_bge (operands[0]));
    DONE;
  }
  ")

(define_expand "bge"
  [(set (pc)
        (if_then_else (ge (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    maxq_signed_cmp = 1;
    maxq_gen_compare_reg (GE, maxq_compare_op0, maxq_compare_op1);
  }
  ")

(define_expand "blt"
  [(set (pc)
        (if_then_else (lt (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    maxq_signed_cmp = 1;
    maxq_gen_compare_reg (LT, maxq_compare_op0, maxq_compare_op1);
  }
  ")

(define_expand "bgt"
  [(set (pc)
        (if_then_else (gt (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    rtx tmp;
    maxq_signed_cmp = 1;
    tmp = maxq_compare_op0;
    maxq_compare_op0 = maxq_compare_op1;
    maxq_compare_op1 = tmp;
    emit_insn (gen_blt (operands[0]));
    DONE;
  }
  ")

(define_expand "beq"
  [(set (pc)
        (if_then_else (eq (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    maxq_signed_cmp = 0;
    maxq_gen_compare_reg (EQ, maxq_compare_op0, maxq_compare_op1);
  }
  ")

(define_expand "bne"
  [(set (pc)
        (if_then_else (ne (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    maxq_signed_cmp = 0;
    maxq_gen_compare_reg (NE, maxq_compare_op0, maxq_compare_op1);
  }
  ")

(define_expand "bleu"
  [(set (pc)
        (if_then_else (leu (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    rtx tmp;
    maxq_signed_cmp = 0;
    tmp = maxq_compare_op0;
    maxq_compare_op0 = maxq_compare_op1;
    maxq_compare_op1 = tmp;
    emit_insn (gen_bgeu (operands[0]));
    DONE;
  }
  ")

(define_expand "bgeu"
  [(set (pc)
        (if_then_else (geu (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    maxq_signed_cmp = 0;
    maxq_gen_compare_reg (GEU, maxq_compare_op0, maxq_compare_op1);
  }
  ")

(define_expand "bltu"
  [(set (pc)
        (if_then_else (ltu (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    maxq_signed_cmp = 0;
    maxq_gen_compare_reg (LTU, maxq_compare_op0, maxq_compare_op1);
  }
  ")

(define_expand "bgtu"
  [(set (pc)
        (if_then_else (gtu (reg:CC PSF) (reg:HI CMP_REG))
                      (label_ref:HI (match_operand 0 "" ""))
                      (pc)))]
  ""
  "
  {
    rtx tmp;
    maxq_signed_cmp = 0;
    tmp = maxq_compare_op0;
    maxq_compare_op0 = maxq_compare_op1;
    maxq_compare_op1 = tmp;
    emit_insn (gen_bltu (operands[0]));
    DONE;
  }
  ")

(define_insn "branch"
  [(set (pc) (if_then_else (match_operator 0 "comparison_operator"
                           [(reg:CC PSF) (reg:HI CMP_REG)])
                           (label_ref (match_operand 1 "" ""))
                           (pc)))]
  ""
  "*
  {
    switch (GET_CODE (operands[0]))
      {
        case LT:
        case GT:
        case LTU:
        case GTU:
                  return \"jump\\tc, %l1\";
        case LE:
        case GE:
        case LEU:
        case GEU:
                  return \"jump\\tnc, %l1\\n\\tjump\\tz, %l1\";
        case EQ:
                  return \"jump\\tz, %l1\";
        case NE:
                  return \"jump\\tnz, %l1\";
        default:
                  return \"error\";
      }
  }
  ")


;;
;; CALL
;;

(define_insn "*laj"
 [(call (mem:HI (match_operand:HI 0 "call_address_operand" "Ur"))
                (match_operand:HI 1 "general_operand" ""))] 
  ""
  "*
  {
    if (!TARGET_MAXQ20 && IS_MAXQ10_DEST (operands[0]))
      return \"move\\tPFX[0],%H0\\n\\tcall\\t%L0\";
    else
      return \"call\\t%0\";
  }
  ")

(define_expand "call"
  [(call (match_operand:HI 0 "" "")
              (match_operand:HI 1 "general_operand" ""))]
  ""
  "
    {
      if (!reload_completed && GET_CODE (operands[0]) == MEM
	  && !call_address_operand (XEXP (operands[0], 0), Pmode))
	operands[0] =
	  gen_rtx_MEM (HImode, force_reg (Pmode, XEXP (operands[0], 0)));
    }
  ")

(define_insn "*lajv"
  [(set (match_operand 0 "register_operand" "=r")
        (call (mem:HI (match_operand:HI 1 "call_address_operand" "Ur"))
              (match_operand:HI 2 "general_operand" "")))]
  ""
  "*
  {
    if (!TARGET_MAXQ20 && IS_MAXQ10_DEST (operands[1]))
      return \"move\\tPFX[0],%H1\\n\\tcall\\t%L1\";
    else
      return \"call\\t%1\";
  }
  ")

(define_expand "call_value"
  [(set (match_operand 0 "" "")
                   (call (match_operand:HI 1 "" "")
                         (match_operand:HI 2 "general_operand" "")))]
  ""
  "
    {
      if (!reload_completed && GET_CODE (operands[0]) == MEM
	  && !call_address_operand (XEXP (operands[1], 0), Pmode))
	operands[0] =
	  gen_rtx_MEM (HImode, force_reg (Pmode, XEXP (operands[1], 0)));
    }
  ")

;; For MAXQ10 in case register is 8 bit wide move the higher address byte 
;; to the PFX[0] register which is used to provide the higher address byte
;; of a 16 bit address in the MAXQ architecture
(define_insn "tablejump"
  [(set (pc) (match_operand:HI 0 "register_operand" "rb"))
   (use (label_ref (match_operand 1 "" "")))]
  ""
  "*
  {
    if (!TARGET_MAXQ20 && IS_MAXQ10_DEST (operands[0]))
      return \"move\\tPFX[0],%H0\\n\\tjump\\t%L0\";
    else
      return \"jump\\t %0\";
  }
  ")
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                   PROLOGUE/EPILOGUE                                                                ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "prologue"
  [(const_int 0)]
  ""
  "
  {
    maxq_expand_prologue ();
    DONE;
  }
  ")

(define_insn "prologue_end"
  [(const_int 1)]
  ""
  "*
  return \"move\\tGR, @SP--\\n\\tmove\\t@--DP[1], GRH\\n\\tmove\\t@--DP[1], GRL\\n\\tmove\\t@++SP, GR\";
  ")

(define_expand "epilogue"
  [(return)]
  ""
  "
  {
    maxq_expand_epilogue ();
  }
  ")

(define_insn "return_1"
  [(return)]
  ""
  "ret\\t")


(define_insn "return_from_epilogue"
  [(return)]
  "reload_completed"
  "ret\\t")


;;
;; NOP
;;
(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                    PUSH/POP                                                        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "pushqi"
  [(set (mem:QI (pre_dec:HI (reg:HI SP_REG)))
        (match_operand:QI 0 "register_operand" "rb,*t"))]
  ""
  "move\\tDPC,#1\\n\\tmove\\t@--DP[1],%0"
)

(define_insn "pushqi_imm"
  [(set (mem:QI (pre_dec:HI (reg:HI SP_REG)))
        (match_operand:QI 0 "immediate_operand" "i"))]
  ""
  "move\\tDPC,#1\\n\\tmove\\t@--DP[1],%0"
)

(define_insn "pushpsf"
  [(set (mem:QI (pre_dec:HI (reg:HI SP_REG)))
        (reg:QI PSF))]
  ""
  "move\\tDPC,#1\\n\\tmove\\t@--DP[1], PSF"
)

(define_insn "pushgrl"
  [(set (mem:QI (pre_dec:HI (reg:HI SP_REG)))
        (reg:QI GRL_REG))]
  ""
  "move\\tDPC,#1\\n\\tmove\\t@--DP[1], GRL"
)

(define_expand "popqi"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t")
   (mem:QI (post_inc:HI (reg:HI SP_REG))))]
  ""
  "")

(define_insn "poppsf"
  [(set (reg:QI PSF) (mem:QI (post_inc:HI (reg:HI SP_REG))))]
  ""
  "move\\tDPC,#1h\\n\\tmove\\tPSF,@DP[1]++"
)

(define_insn "popgrl"
  [(set (reg:QI GRL_REG)
        (mem:QI (post_inc:HI (reg:HI SP_REG))))]
  ""
  "move\\tDPC,#1h\\n\\tmove\\tGRL,@DP[1]++"
)

(define_insn "popqi_1"
  [(set (match_operand:QI 0 "register_operand" "=rb,*t")
        (mem:QI (post_inc:HI (reg:HI SP_REG))))]
  ""
  "move\\tDPC,#1h\\n\\tmove\\t%0,@DP[1]++"
)

(define_insn "pushhi"
  [(set (mem:HI (pre_dec:HI (reg:HI SP_REG)))
        (match_operand:HI 0 "reg_or_imm_operand" "irb,*t"))]
  ""
  "#")

(define_split
  [(set (mem:HI (pre_dec:HI (reg:HI SP_REG)))
        (match_operand:HI 0 "reg_or_imm_operand" ""))]
  "TARGET_MAXQ20" 
  [
   (set (reg:HI GR_REG) (match_dup 0))
   (clobber (reg:QI GRH_REG))
   (clobber (reg:QI GRL_REG)) 
   (parallel [(set (mem:HI (pre_dec:HI (reg:HI SP_REG)))
                   (reg:HI GR_REG))
              (use (reg:HI DP_REG))])]
  "")
	
(define_split
  [(set (mem:HI (pre_dec:HI (reg:HI SP_REG)))
        (match_operand:HI 0 "reg_or_imm_operand" ""))]
  "!TARGET_MAXQ20" 
  [
   (parallel [(set (reg:HI GR_REG) (match_dup 0))
              (clobber (reg:QI GRH_REG))
              (clobber (reg:QI GRL_REG)) 
              (clobber (reg:HI GR_REG)) ])
   (parallel [(set (mem:HI (pre_dec:HI (reg:HI SP_REG)))
                   (reg:HI GR_REG))
              (use (reg:HI DP_REG))])]
  "")
	
(define_insn "pushhi_1"
  [(set (mem:HI (pre_dec:HI (reg:HI SP_REG)))
        (reg:HI GR_REG))
    	(use (reg:HI DP_REG))]
  ""
  "move\\t@--DP[1],GRH\\n\\tmove\\t@--DP[1],GRL ")

(define_expand "pophi"
  [(set (match_operand:HI 0 "register_operand" "=rbqa")
    (mem:HI (post_inc:HI (reg:HI SP_REG))) )]
  ""
  "
  {
     rtx gr;
     gr = gen_rtx_REG (HImode, GR_REG);
     emit_insn (gen_pophi_1 ());
     if (TARGET_MAXQ20) 
       emit_move_insn (operands[0], gr);
     else
       emit_insn (gen_move_gr_1 (operands[0]));
     DONE;
  }
  ")
 
(define_insn "pophi_1"
  [(set (reg:HI GR_REG)
    (mem:HI (post_inc:HI (reg:HI SP_REG))))]
  ""
  "move\\tDPC,#1h\\n\\tmove\\tGRL,@DP[1]++\\n\\tmove\\tGRH,@DP[1]++")	

