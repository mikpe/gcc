;; Scheduling description for Hexagon v3 architecture.
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
;; V3 ;;
;;----;;

(define_insn_reservation "v3_A" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "A"))
  "Slot0 | Slot1 | Slot2 | Slot3")

(define_insn_reservation "v3_X" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "X"))
  "Slot2 | Slot3")

(define_insn_reservation "v3_Load" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "Load"))
  "Slot0 | Slot1")

(define_insn_reservation "v3_Store" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "Store"))
  "Slot0")

(define_insn_reservation "v3_Allocframe" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "Allocframe"))
  "Slot0 + (Store0 | Store1)")

(define_insn_reservation "v3_LoadStore" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "LoadStore"))
  "Slot0 + Slot1")

(define_insn_reservation "v3_AStore" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "AStore"))
  "Slot0 + (Slot1 | Slot2 | Slot3)")

(define_insn_reservation "v3_ALoadStore" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "ALoadStore"))
  "Slot0 + Slot1 + (Slot2 | Slot3)")

(define_insn_reservation "v3_M" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "M"))
  "Slot2 | Slot3")

(define_insn_reservation "v3_S" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "S"))
  "Slot2 | Slot3")

(define_insn_reservation "v3_CR" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "CR"))
  "Slot3")

(define_insn_reservation "v3_loop" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "loop"))
  "Slot3 + PCadder + PCadder_dualjumps")

(define_insn_reservation "v3_J" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "J"))
  "(Slot2 | Slot3) + (PCadder | PCadder_dualjumps)
   + (control | control_dualjumps)")

(define_insn_reservation "v3_J_dotnew" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "J_dotnew"))
  "(Slot2 | Slot3) + PCadder + control")

(define_insn_reservation "v3_JR" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "JR"))
  "Slot2 + control")

(define_insn_reservation "v3_endloop0" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "endloop0"))
  "endloop0 + endloop0_dualjumps")

(define_insn_reservation "v3_endloop1" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "endloop1"))
  "endloop1 + endloop1_dualjumps")

(define_insn_reservation "v3_dcfetch" 1
  (and (eq_attr "arch" "v3") (eq_attr "type" "dcfetch"))
  "Slot0")
