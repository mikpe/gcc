;; Scheduling description for Hexagon v1 architecture.
;; Copyright 2012 Free Software Foundation, Inc.

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

;;----;;
;; V1 ;;
;;----;;

(define_insn_reservation "v1_A" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "A"))
  "Slot0 | Slot1 | Slot2 | Slot3")

(define_insn_reservation "v1_X" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "X"))
  "Slot2 | Slot3")

(define_insn_reservation "v1_Load" 1
  (and (ne (symbol_ref "hexagon_dual_memory_accesses") (const_int 0))
       (and (eq_attr "arch" "v1") (eq_attr "type" "Load")))
  "Slot0 | Slot1")

(define_insn_reservation "v1_possibly_uncached_Load" 1
  (and (eq (symbol_ref "hexagon_dual_memory_accesses") (const_int 0))
       (and (eq_attr "arch" "v1") (eq_attr "type" "Load")))
  "Slot0")

(define_insn_reservation "v1_Store" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "Store"))
  "Slot0")

(define_insn_reservation "v1_Allocframe" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "Allocframe"))
  "Slot0 + (Store0 | Store1)")

(define_insn_reservation "v1_LoadStore" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "LoadStore"))
  "Slot0 + Slot1")

(define_insn_reservation "v1_AStore" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "AStore"))
  "Slot0 + (Slot1 | Slot2 | Slot3)")

(define_insn_reservation "v1_ALoadStore" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "ALoadStore"))
  "Slot0 + Slot1 + (Slot2 | Slot3)")

(define_insn_reservation "v1_M" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "M"))
  "Slot2")

(define_insn_reservation "v1_S" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "S"))
  "Slot3")

(define_insn_reservation "v1_CR" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "CR"))
  "Slot3")

(define_insn_reservation "v1_loop" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "loop"))
  "Slot3 + PCadder")

(define_insn_reservation "v1_J" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "J"))
  "(Slot2 | Slot3) + PCadder + control")

(define_insn_reservation "v1_JR" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "JR"))
  "Slot2 + control")

(define_insn_reservation "v1_endloop0" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "endloop0"))
  "endloop0")

(define_insn_reservation "v1_endloop1" 1
  (and (eq_attr "arch" "v1") (eq_attr "type" "endloop1"))
  "endloop1")
