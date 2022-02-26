;; Constraint definitions for Qualcomm Hexagon
;; Copyright (C) 2011 Free Software Foundation, Inc.
;; Contributed by Mentor Graphics.

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

(define_register_constraint "Rg" "GENERAL_REGS"
 "General registers @code{r0}-@code{r31}.")

(define_register_constraint "Rp" "PREDICATE_REGS"
 "Predicate registers.")

(define_register_constraint "Rc" "CONTROL_REGS"
 "Control registers.")

(define_register_constraint "Rq" "STACK_REGS"
 "Stack Pointer.")

;; IsN means signed two's complement integers in [-2^(N-1), 2^(N-1) - 1].

(define_constraint "Is4"
  "Immediate value in the range @minus{}8-7."
  (and (match_code "const_int")
       (match_test "ival >= -8 && ival < 8")))

(define_constraint "Is5"
  "Immediate value in the range @minus{}16-15."
  (and (match_code "const_int")
       (match_test "ival >= -16 && ival < 16")))

(define_constraint "Is6"
  "Immediate value in the range @minus{}32-31."
  (and (match_code "const_int")
       (match_test "ival >= -32 && ival < 32")))

(define_constraint "Is7"
  "Immediate value in the range @minus{}64-63."
  (and (match_code "const_int")
       (match_test "ival >= -64 && ival < 64")))

(define_constraint "Is8"
  "Immediate value in the range @minus{}128-127."
  (and (match_code "const_int")
       (match_test "ival >= -128 && ival < 128")))

; Formerly "Is10".

(define_constraint "IsA"
  "Immediate value in the range @minus{}512-511."
  (and (match_code "const_int")
       (match_test "ival >= -512 && ival < 512")))

; Formerly "Is12".

(define_constraint "IsC"
  "Immediate value in the range @minus{}2048-2047."
  (and (match_code "const_int")
       (match_test "ival >= -2048 && ival < 2048")))

; Formerly "Is16".

(define_constraint "IsG"
  "Immediate value in the range @minus{}32768-32767."
  (and (match_code "const_int")
       (match_test "ival >= -32768 && ival < 32768")))

;; IuN means unsigned integers in [0, 2^N - 1].

; Sometimes formerly written "Iu00".

(define_constraint "Iu0"
  "Immediate zero."
  (and (match_code "const_int")
       (match_test "ival == 0")))

; Sometimes formerly written "Iu01".

(define_constraint "Iu1"
  "Immediate zero or one."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 2")))

(define_constraint "Iu2"
  "Immediate value in the range 0-3."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 4")))

(define_constraint "Iu3"
  "Immediate value in the range 0-7."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 8")))

(define_constraint "Iu4"
  "Immediate value in the range 0-15."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 16")))

(define_constraint "Iu5"
  "Immediate value in the range 0-31."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 32")))

(define_constraint "Iu6"
  "Immediate value in the range 0-63."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 64")))

(define_constraint "Iu7"
  "Immediate value in the range 0-127."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 128")))

(define_constraint "Iu8"
  "Immediate value in the range 0-255."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 256")))

(define_constraint "Iu9"
  "Immediate value in the range 0-511."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 512")))

; Formerly "Iu10".

(define_constraint "IuA"
  "Immediate value in the range 0-1023."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 1024")))

; Formerly "Iu16".

(define_constraint "IuG"
  "Immediate value in the range 0-65535."
  (and (match_code "const_int")
       (match_test "ival >= 0 && ival < 65536")))

;; InN means negative integers in [-(2^N - 1), 0].

(define_constraint "In5"
  "Immediate value in the range @minus{}31-0."
  (and (match_code "const_int")
       (match_test "ival > -32 && ival <= 0")))

(define_constraint "In8"
  "Immediate value in the range @minus{}255-0."
  (and (match_code "const_int")
       (match_test "ival > -256 && ival <= 0")))

;; ImN means signed magnitude integers in [-(2^(N-1) - 1), 2^(N-1) - 1].

(define_constraint "Im6"
  "Immediate value in the range @minus{}31-31."
  (and (match_code "const_int")
       (match_test "ival > -32 && ival < 32")))

(define_constraint "Im9"
  "Immediate value in the range @minus{}255-255."
  (and (match_code "const_int")
       (match_test "ival > -256 && ival < 256")))

;; J[sunm]N_M means I[sunm]N shifted left M bits.  Only one such constraint is
;; defined...

(define_constraint "Ju6_2"
  "Immediate value in the range 0-252, divisible by 4."
  (and (match_code "const_int")
       (match_test "ival == (ival & 0xfc)")))

;; TODO: Review uses of these single-valued constraints.

; Formerly "K-1".

(define_constraint "Km1"
  "Immediate -1."
  (and (match_code "const_int")
       (match_test "ival == -1")))

(define_constraint "K01"
  "Immediate 1."
  (and (match_code "const_int")
       (match_test "ival == 1")))

(define_constraint "K16"
  "Immediate 16."
  (and (match_code "const_int")
       (match_test "ival == 16")))

(define_constraint "K32"
  "Immediate 32."
  (and (match_code "const_int")
       (match_test "ival == 32")))

; Formerly "Ku7p1".

(define_constraint "Ku7"
  "Immediate value in the range 1-128."
  (and (match_code "const_int")
       (match_test "ival >= 1 && ival <= 128")))

; Formerly "Ks8p1".

(define_constraint "K8p"
  "Immediate value in the range @minus{}127-128."
  (and (match_code "const_int")
       (match_test "ival > -128 && ival <= 128")))

; Formerly "Ku5_3p8".

(define_constraint "K82"
  "Immediate value in the range 8-256."
  (and (match_code "const_int")
       (match_test "ival >= 8 && ival <= 256")))

; Formerly "Ku9p1".

(define_constraint "Ku9"
  "Immediate value in the range 1-512."
  (and (match_code "const_int")
       (match_test "ival > 0 && ival <= 512")))

; Formerly "Ks10p1".

(define_constraint "KXp"
  "Immediate value in the range @minus{}511-512."
  (and (match_code "const_int")
       (match_test "ival > -512 && ival <= 512")))

; Formerly "Ks8s8".

(define_constraint "K88"
  "Immediate value that can be formed using combine(#s8,#s8)."
  (and (match_code "const_int")
       (match_test "const_ok_for_s8s8 (ival)")))

; Formerly "Ks8s32".

(define_constraint "K83"
  "Immediate value that can be formed using combine(#s8,#s32)."
  (and (match_code "const_int")
       (match_test "TARGET_V4_FEATURES")
       (match_test "const_ok_for_s8s32 (ival)")))

; Formerly "Ks32s8".

(define_constraint "K38"
  "Immediate value that can be formed using combine(#s32,#s8)."
  (and (match_code "const_int")
       (match_test "TARGET_V4_FEATURES")
       (match_test "const_ok_for_s32s8 (ival)")))

; Formerly "Ks16s32".

(define_constraint "K13"
  "Immediate value that can be formed using combine(#s16,#s32)."
  (and (match_code "const_int")
       (match_test "TARGET_V4_FEATURES")
       (match_test "const_ok_for_s16s32 (ival)")))

; Formerly "Ks32s16".

(define_constraint "K31"
  "Immediate value that can be formed using combine(#s32,#s16)."
  (and (match_code "const_int")
       (match_test "TARGET_V4_FEATURES")
       (match_test "const_ok_for_s32s16 (ival)")))

; Formerly "Konehot32".

(define_constraint "K1h"
  "Immediate value that is a power of 2 representable in 32 bits."
  (and (match_code "const_int")
       (match_test "const_ok_for_onehot (ival)")))

; Formerly "Konenot32".

(define_constraint "K1n"
  "Immediate value that has exactly one bit clear in 32 bits."
  (and (match_code "const_int")
       (match_test "const_ok_for_onehot ((~ival) & 0xffffffffull)")))

(define_constraint "Kp2"
  "A power of two between 1 and 128, inclusive."
  (and (match_code "const_int")
       (match_test "const_ok_for_onehot (ival) && ival <= 128")))

(define_constraint "G"
  "Constant zero (floating-point)."
  (and (match_code "const_double")
       (match_test "op == CONST0_RTX (mode)")))

; Users of this constraint should probably be using "i" instead.

(define_constraint "Q"
  "A constant."
  (match_test "CONSTANT_P (op)"))

; Formerly "Anoext"

(define_memory_constraint "Anx"
  "An address suitable for loads and stores, without an extension."
  (and (match_code "mem")
       (match_test "hexagon_noext_mem_operand (mode, op, false)")))

; Formerly "Acond"

(define_memory_constraint "Aco"
  "An address suitable for conditional loads and stores."
  (and (match_code "mem")
       (match_test "hexagon_cond_mem_operand (mode, op, false)")))

; Formerly "Aecond"

(define_memory_constraint "Aec"
  "An address suitable for conditional loads and stores, allowing an immediate
   extender."
  (and (match_code "mem")
       (match_test "hexagon_cond_mem_operand (mode, op, true)")))

(define_memory_constraint "Asi"
  "An address suitable for store immediate."
  (and (match_code "mem")
       (match_test "hexagon_storeimm_mem_operand (mode, op)")))

; Formerly "Acsi"

(define_memory_constraint "Acs"
  "An address suitable for conditional store immediate."
  (and (match_code "mem")
       (match_test "hexagon_storeimm_mem_operand (mode, op)")))

; Formerly "Amemop"

(define_memory_constraint "Amo"
  "An address suitable for read-modify-write memory operations."
  (and (match_code "mem")
       (match_test "hexagon_rmw_mem_operand (mode, op, false)")))

; Formerly "Aememop"

(define_memory_constraint "Aem"
  "An address suitable for read-modify-write memory operations, allowing an
   immediate extender."
  (and (match_code "mem")
       (match_test "hexagon_rmw_mem_operand (mode, op, true)")))

; Formerly "Adm3"

(define_memory_constraint "Ad3"
  "An address suitable for load and store sub-instructions with an #u3
   offset."
  (and (match_code "mem")
       (match_test "hexagon_subinst_mem_operand (mode, op, 3)")))

; Formerly "Adm4"

(define_memory_constraint "Ad4"
  "An address suitable for load and store sub-instructions with an #u4
   offset."
  (and (match_code "mem")
       (match_test "hexagon_subinst_mem_operand (mode, op, 4)")))

; Formerly "Admsp5"

(define_memory_constraint "Au5"
  "An address suitable for sub-instructions with SP and a #u5 offset."
  (and (match_code "mem")
       (match_test "hexagon_subinst_stack_mem_operand (mode, op, 5)")))

; Formerly "Admsp6"

(define_memory_constraint "As6"
  "An address suitable for sub-instructions with SP and a #s6 offset."
  (and (match_code "mem")
       (match_test "hexagon_subinst_stack_mem_operand (mode, op, -6)")))
