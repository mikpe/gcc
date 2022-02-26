;; Scheduling description for Hexagon v4 architecture.
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
;; V4 ;;
;;----;;

(define_insn_reservation "v4_A" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "A"))
  "Slot0 | Slot1 | Slot2 | Slot3")

(define_insn_reservation "v4_X" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "X"))
  "Slot2 | Slot3")

(define_insn_reservation "v4_Load" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "Load"))
  "Slot0 | Slot1")

(define_insn_reservation "v4_Store" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "Store"))
  "(Slot0 | Slot1) + (Store0 | Store1)")

(define_insn_reservation "v4_Allocframe" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "Allocframe"))
  "Slot0 + (Store0 | Store1)")

(define_insn_reservation "v4_LoadStore" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "LoadStore"))
  "Slot0 + Slot1")

(define_insn_reservation "v4_AStore" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "AStore"))
  "((Slot0 + (Slot1 | Slot2 | Slot3)) | (Slot1 + (Slot2 | Slot3)))
   + (Store0 | Store1)")

(define_insn_reservation "v4_ALoadStore" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "ALoadStore"))
  "Slot0 + Slot1 + (Slot2 | Slot3)")

(define_insn_reservation "v4_Memop" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "Memop"))
  "Slot0 + Store0 + Store1")

(define_insn_reservation "v4_NewValue" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "NewValue"))
  "Slot0 + Store0 + Store1")

(define_insn_reservation "v4_NewValueJump" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "NewValueJump"))
  "Slot0 + Store0 + Store1 + PCadder + control + control_dualjumps")

(define_insn_reservation "v4_ENewValueJump" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "ENewValueJump"))
  "ESlot0 + Store0 + Store1 + PCadder + control + control_dualjumps")

(define_insn_reservation "v4_AJ" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "AJ"))
  "((Slot2 + (Slot0 | Slot1 | Slot3)) | (Slot3 + (Slot0 | Slot1 | Slot2)))
   + (PCadder | PCadder_dualjumps) + (control | control_dualjumps)")

(define_insn_reservation "v4_EAJ" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "EAJ"))
  "((ESlot2 + (Slot0 | Slot1 | Slot3)) | (ESlot3 + (Slot0 | Slot1 | Slot2)))
   + (PCadder | PCadder_dualjumps) + (control | control_dualjumps)")

(define_insn_reservation "v4_XJ" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "XJ"))
  "(Slot2 + Slot3) + (PCadder | PCadder_dualjumps)
   + (control | control_dualjumps)")

(define_insn_reservation "v4_EXJ" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "EXJ"))
  "(ESlot2 + Slot3) + (PCadder | PCadder_dualjumps)
   + (control | control_dualjumps)")

(define_insn_reservation "v4_M" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "M"))
  "Slot2 | Slot3")

(define_insn_reservation "v4_S" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "S"))
  "Slot2 | Slot3")

(define_insn_reservation "v4_CR" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "CR"))
  "Slot3")

(define_insn_reservation "v4_loop" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "loop"))
  "Slot3 + PCadder + control_dualjumps + endloop0 + endloop0_dualjumps
   + endloop1 + endloop1_dualjumps")

(define_insn_reservation "v4_Eloop" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "Eloop"))
  "ESlot3 + PCadder + control_dualjumps + endloop0 + endloop0_dualjumps
   + endloop1 + endloop1_dualjumps")

(define_insn_reservation "v4_J" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "J"))
  "(Slot2 | Slot3) + (PCadder | PCadder_dualjumps)
   + (control | control_dualjumps)")

(define_insn_reservation "v4_JR" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "JR"))
  "Slot2 + control + control_dualjumps")

(define_insn_reservation "v4_endloop0" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "endloop0"))
  "endloop0 + endloop0_dualjumps")

(define_insn_reservation "v4_endloop1" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "endloop1"))
  "endloop1 + endloop1_dualjumps")

(define_insn_reservation "v4_EA" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "EA"))
  "ESlot0 | ESlot1 | ESlot2 | ESlot3")

(define_insn_reservation "v4_EX" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "EX"))
  "ESlot2 | ESlot3")

(define_insn_reservation "v4_ELoad" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "ELoad"))
  "ESlot0 | ESlot1")

(define_insn_reservation "v4_EStore" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "EStore"))
  "(ESlot0 | ESlot1) + (Store0 | Store1)")

(define_insn_reservation "v4_EMemop" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "EMemop"))
  "ESlot0 + Store0 + Store1")

(define_insn_reservation "v4_ENewValue" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "ENewValue"))
  "ESlot0 + Store0 + Store1")

(define_insn_reservation "v4_EM" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "EM"))
  "ESlot2 | ESlot3")

(define_insn_reservation "v4_ES" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "ES"))
  "ESlot2 | ESlot3")

(define_insn_reservation "v4_ECR" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "ECR"))
  "ESlot3")

(define_insn_reservation "v4_EJ" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "EJ"))
  "(ESlot2 | ESlot3) + (PCadder | PCadder_dualjumps)
   + (control | control_dualjumps)")

(define_insn_reservation "v4_EJR" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "EJR"))
  "ESlot2 + control + control_dualjumps")

(define_insn_reservation "v4_dcfetch" 1
  (and (eq_attr "arch" "v4") (eq_attr "type" "dcfetch"))
  "Slot0 + Store0 + Store1")
