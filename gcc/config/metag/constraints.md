;; Constraint definitions for META.
;; Copyright (C) 2007, 2010 Imagination Technologies Ltd

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

;; Register constraints

(define_register_constraint "d" "D_REGS"
  "data unit register")

(define_register_constraint "e" "D0_REGS"
  "data unit 0 register")

(define_register_constraint "f" "D1_REGS"
  "data unit 1 register")

(define_register_constraint "a" "A_REGS"
  "address unit register")
 
(define_register_constraint "h" "A0_REGS"
  "address unit 0 register")

(define_register_constraint "l" "A1_REGS"
  "address unit 1 register")

(define_register_constraint "be" "Be_REGS"
  "O2R register data unit 0")

(define_register_constraint "bf" "Bf_REGS"
  "O2R register data unit 1")

(define_register_constraint "bd" "Bd_REGS"
  "O2R register data unit")

(define_register_constraint "bh" "Bh_REGS"
  "O2R register address unit 0")

(define_register_constraint "bl" "Bl_REGS"
  "O2R register address unit 1")

(define_register_constraint "ba" "Ba_REGS"
  "O2R register address unit")

(define_register_constraint "br" "Br_REGS"
  "O2R register any unit")

(define_register_constraint "t" "nD0_REGS"
  "data unit 1, addr unit 0, addr unit 1 register")

(define_register_constraint "u" "nD1_REGS"
  "data unit 0, addr unit 0, addr unit 1 register")

(define_register_constraint "y" "nA0_REGS"
  "data unit 0, data unit 1, addr unit 1 register")

(define_register_constraint "z" "nA1_REGS"
  "data unit 0, addr unit 1, addr unit 0 register")

(define_register_constraint "q" "nBU_REGS"
  "not base unit register")

(define_register_constraint "Wx" "Wx_REGS"
  "control register i.e. TXRPT")

(define_register_constraint "WQh" "WQh_REGS"
  "A0 QuickRoT control registers A0.2 A0.3")

(define_register_constraint "WQl" "WQl_REGS"
  "A1 QuickRoT control registers A1.2 A1.3")

(define_register_constraint "Ye" "Ye_REGS"
  "data unit 0 register 12-bit offsetable")

(define_register_constraint "Yf" "Yf_REGS"
  "data unit 1 register 12-bit offsetable")

(define_register_constraint "Yd" "Yd_REGS"
  "data unit   register 12-bit offsetable")

(define_register_constraint "Yh" "Yh_REGS"
  "addr unit 0 register 12-bit offsetable")

(define_register_constraint "Yl" "Yl_REGS"
  "addr unit 1 register 12-bit offsetable")

(define_register_constraint "Ya" "Ya_REGS"
  "addr unit   register 12-bit offsetable")

(define_register_constraint "Yr" "Yr_REGS"
  "data/addr   register 12-bit offsetable")

(define_register_constraint "Yne" "nYe_REGS"
  "...")

(define_register_constraint "Ynf" "nYf_REGS"
  "...")

(define_register_constraint "Ynd" "nYd_REGS"
  "...")

(define_register_constraint "Ynh" "nYh_REGS"
  "...")

(define_register_constraint "Ynl" "nYl_REGS"
  "...")

(define_register_constraint "Yna" "nYa_REGS"
  "...")

(define_register_constraint "Ynr" "nYr_REGS"
  "...")

(define_register_constraint "ce" "metag_fpu_resources ? cD0_REGS : D0_REGS"
  "data 0 or float unit register")

(define_register_constraint "cf" "metag_fpu_resources ? cD1_REGS : D1_REGS"
  "data 1, or float unit register")

(define_register_constraint "cd" "metag_fpu_resources ? cD_REGS : D_REGS"
  "data, or float unit register")

(define_register_constraint "ch" "metag_fpu_resources ? cA0_REGS : A0_REGS"
  "addr 0 or float unit register")

(define_register_constraint "cl" "metag_fpu_resources ? cA1_REGS : A1_REGS"
  "addr 1 or float unit register")

(define_register_constraint "ca" "metag_fpu_resources ? cA_REGS : A_REGS"
  "addr or float unit register")

(define_register_constraint "cr" "metag_fpu_resources ? cDA_REGS : DA_REGS"
  "data, addr or float unit register")

(define_register_constraint "ct" "metag_fpu_resources ? cnD0_REGS : nD0_REGS"
  "data unit 1, addr unit 0, addr unit 1 or float unit register")

(define_register_constraint "cu" "metag_fpu_resources ? cnD1_REGS : nD1_REGS"
  "data unit 0, addr unit 0, addr unit 1 or float unit register")

(define_register_constraint "cy" "metag_fpu_resources ? cnA0_REGS : nA0_REGS"
  "data unit 0, data unit 0, addr unit 1 or float unit register")

(define_register_constraint "cz" "metag_fpu_resources ? cnA1_REGS : nA1_REGS"
  "data unit 0, data unit 1, addr unit 0 or float unit register")

(define_register_constraint "cx" "metag_fpu_resources ? FPC_REGS : NO_REGS"
  "floating point register")

(define_register_constraint "cp" "metag_fpu_resources ? FPP_REGS : NO_REGS"
  "floating point register pair")

;; Integer constraints

(define_constraint "I"
  "...."
  (and (match_code "const_int")
       (match_test "(ival >= -32768 && ival <= -256) || (ival >=  256 && ival <= 65535)")))

(define_constraint "J"
  "...."
  (and (match_code "const_int")
       (match_test "(ival & 0x0000FFFF) == 0")))

(define_constraint "O0"
  "...."
  (and (match_code "const_int")
       (match_test "(ival & 0xFFFF) == 0")))

(define_constraint "O3"
  "...."
  (and (match_code "const_int")
       (match_test "((ival >> 16) & 0x0000FFFF) == 0")))

(define_constraint "K"
  "..."
  (and (match_code "const_int")
       (match_test "0 <= ival && ival <= 255")))

(define_constraint "L"
  "..."
  (and (match_code "const_int")
       (match_test "0 <= ival && ival <= 31")))

(define_constraint "M"
  "..."
  (and (match_code "const_int")
       (match_test "((ival >> 16) & 0xFFFF) == 0x0000FFFF")))

(define_constraint "N"
  "..."
  (and (match_code "const_int")
       (match_test "((ival & 0x0000FFFF) == 0x0000FFFF)")))

(define_constraint "O1"
  "..."
  (and (match_code "const_int")
       (match_test "-32 <= ival && ival < 32")))

(define_constraint "O2"
  "..."
  (and (match_code "const_int")
       (match_test "(-64 <= ival && ival < 64) && (ival & 1) == 0")))

(define_constraint "O4"
  "..."
  (and (match_code "const_int")
       (match_test "(-128 <= ival && ival < 128) && (ival & 3) == 0")))

(define_constraint "O8"
  "..."
  (and (match_code "const_int")
       (match_test "(-256 <= ival && ival < 256) && (ival & 7) == 0")))

(define_constraint "P"
  "..."
  (and (match_code "const_int")
       (match_test "-255 <= ival && ival < 0")))

(define_constraint "vci"
  "..."
  (and (match_code "const_vector")
       (match_test "GET_MODE_INNER (mode) == SImode")))

(define_constraint "vcf"
  "..."
  (and (match_code "const_vector")
       (match_test "GET_MODE_INNER (mode) == SFmode")))

(define_constraint "vc5"
  "..."
  (and (match_code "const_vector")
       (match_test "metag_vector_5bit_op (op, mode)")))

(define_constraint "v16"
  "..."
  (and (match_code "const_vector")
       (match_test "metag_vector_16bit_op (op, mode)")))

;; Floating-point constraints

(define_constraint "G"
  "Floating-point zero."
  (and (match_code "const_double")
       (match_test "GET_MODE_CLASS (mode) == MODE_FLOAT && op == CONST0_RTX (mode)")))

(define_constraint "H"
  "Floating-point one."
  (and (match_code "const_double")
       (match_test "GET_MODE_CLASS (mode) == MODE_FLOAT && op == CONST1_RTX (mode)")))

(define_constraint "ci"
  "Floating-point immediates in half precision"
  (and (match_code "const_double")
       (match_test "GET_MODE_CLASS (mode) == MODE_FLOAT && metag_fphalf_imm_op (op, mode)")))

;; General constraints

(define_constraint "Th"
  "@internal"
  (and (match_test "metag_mem_base_p (op, A0_REGS)")
       (match_test "metag_legitimate_address_p (XEXP (op, 0), GET_MODE (op), true)")))

(define_constraint "Tl"
  "@internal"
  (and (match_test "metag_mem_base_p (op, A1_REGS)")
       (match_test "metag_legitimate_address_p (XEXP (op, 0), GET_MODE (op), true)")))

(define_constraint "Te"
  "@internal"
  (and (match_test "metag_mem_base_p (op, D0_REGS)")
       (match_test "metag_legitimate_address_p (XEXP (op, 0), GET_MODE (op), true)")))

(define_constraint "Tf"
  "@internal"
  (and (match_test "metag_mem_base_p (op, D1_REGS)")
       (match_test "metag_legitimate_address_p (XEXP (op, 0), GET_MODE (op), true)")))

(define_constraint "Tr"
  "@internal"
  (and (match_test "metag_legitimate_address_p (XEXP (op, 0), GET_MODE (op), true)")
       (not (match_test "GET_CODE (XEXP (op, 0)) == PLUS
                         && metag_regs_ok_for_base_offset_p (XEXP (XEXP (op, 0), 0),
                                                             XEXP (XEXP (op, 0), 1),
                                                             true)"))))

(define_constraint "Z1"
  "..."
  (and (match_code "const_int")
       (match_test "-2048 <= ival && ival < 2048")))

(define_constraint "Z2"
  "..."
  (and (match_code "const_int")
       (match_test "-4096 <= ival && ival < 4096 && (ival & 1) == 0")))

(define_constraint "Z4"
  "..."
  (and (match_code "const_int")
       (match_test "-8192 <= ival && ival < 8192 && (ival & 3) == 0")))

(define_constraint "Z8"
  "..."
  (and (match_code "const_int")
       (match_test "-16384 <= ival && ival < 16384 && (ival & 7) == 0")))

