;; Constraint definitions for the RCA CDP1802 processor.
;; Copyright (C) 2024 Free Software Foundation, Inc.
;; Contributed by Mikael Pettersson <mikpelinux@gmail.com>
;;
;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3, or (at your option)
;; any later version.
;;
;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;; Register constraints.

(define_register_constraint "a" "A_REGS"
  "The first argument register")

(define_register_constraint "b" "B_REGS"
  "The second argument register")

(define_register_constraint "c" "GENERAL_REGS"
  "Any general-purpose register except the stack pointer"
  "regno != 2")

(define_register_constraint "z" "Z_REGS"
  "Hidden argument register")

;; Integer constraints.

(define_constraint "I"
  "An integer constant between -7 and 7"
  (and (match_code "const_int")
       (match_test "IN_RANGE (ival, -7, 7)")))

;; Extra constraints.

(define_constraint "Q"
  "A register push operation."
  (and (match_code "mem")
       (match_code "pre_dec" "0")
       (match_test "XEXP (XEXP (op, 0), 0) == stack_pointer_rtx")))

(define_constraint "R"
  "A register pop operation."
  (and (match_code "mem")
       (match_code "post_inc" "0")
       (match_test "XEXP (XEXP (op, 0), 0) == stack_pointer_rtx")))

(define_memory_constraint "S"
  "A post-increment memory access."
  (and (match_code "mem")
       (match_code "post_inc" "0")
       (match_test "REG_P (XEXP (XEXP (op, 0), 0))")))

(define_memory_constraint "T"
  "A post-decrement memory access."
  (and (match_code "mem")
       (match_code "post_dec" "0")
       (match_test "REG_P (XEXP (XEXP (op, 0), 0))")))

(define_constraint "Z"
  "Zero."
  (and (match_code "const_int")
       (match_test "ival == 0")))
