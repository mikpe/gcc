;; -*- buffer-read-only: t -*-
;; Generated automatically from genmova.sh
;; Copyright (C) 2004-2025 Free Software Foundation, Inc.
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
(define_insn ""
  [(set (match_operand:QI 0 "register_operand" "=r,r")
	(plus:QI (mult:QI (match_operand:QI 1 "h8300_dst_operand" "0,rQ")
			   (const_int 2))
		 (match_operand:QI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:QI 0 "register_operand" "=r,r")
	(plus:QI (ashift:QI (match_operand:QI 1 "h8300_dst_operand" "0,rQ")
			   (const_int 1))
		 (match_operand:QI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:QI 0 "register_operand" "=r,r")
	(plus:QI (mult:QI (match_operand:QI 1 "h8300_dst_operand" "0,rQ")
			   (const_int 4))
		 (match_operand:QI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:QI 0 "register_operand" "=r,r")
	(plus:QI (ashift:QI (match_operand:QI 1 "h8300_dst_operand" "0,rQ")
			   (const_int 2))
		 (match_operand:QI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(plus:HI (zero_extend:HI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		 (match_operand:HI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/b.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(mult:HI (zero_extend:HI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 2)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(plus:HI (mult:HI (zero_extend:HI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 2))
		 (match_operand:HI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(and:HI (mult:HI (subreg:HI (match_operand:QI 1 "memory_operand" "m") 0)
			  (const_int 2))
		(const_int 510)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(plus:HI (and:HI (mult:HI (subreg:HI (match_operand:QI 1 "memory_operand" "m") 0)
				   (const_int 2))
			 (const_int 510))
		 (match_operand:HI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(and:HI (mult:HI (match_operand:HI 1 "register_operand" "0")
			  (const_int 2))
		(const_int 510)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(plus:HI (and:HI (mult:HI (match_operand:HI 1 "register_operand" "0")
				   (const_int 2))
			 (const_int 510))
		 (match_operand:HI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(ashift:HI (zero_extend:HI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 1)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(plus:HI (ashift:HI (zero_extend:HI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 1))
		 (match_operand:HI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(and:HI (ashift:HI (subreg:HI (match_operand:QI 1 "memory_operand" "m") 0)
			  (const_int 1))
		(const_int 510)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(plus:HI (and:HI (ashift:HI (subreg:HI (match_operand:QI 1 "memory_operand" "m") 0)
				   (const_int 1))
			 (const_int 510))
		 (match_operand:HI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(and:HI (ashift:HI (match_operand:HI 1 "register_operand" "0")
			  (const_int 1))
		(const_int 510)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(plus:HI (and:HI (ashift:HI (match_operand:HI 1 "register_operand" "0")
				   (const_int 1))
			 (const_int 510))
		 (match_operand:HI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(mult:HI (zero_extend:HI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 4)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(plus:HI (mult:HI (zero_extend:HI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 4))
		 (match_operand:HI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(and:HI (mult:HI (subreg:HI (match_operand:QI 1 "memory_operand" "m") 0)
			  (const_int 4))
		(const_int 1020)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(plus:HI (and:HI (mult:HI (subreg:HI (match_operand:QI 1 "memory_operand" "m") 0)
				   (const_int 4))
			 (const_int 1020))
		 (match_operand:HI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(and:HI (mult:HI (match_operand:HI 1 "register_operand" "0")
			  (const_int 4))
		(const_int 1020)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(plus:HI (and:HI (mult:HI (match_operand:HI 1 "register_operand" "0")
				   (const_int 4))
			 (const_int 1020))
		 (match_operand:HI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(ashift:HI (zero_extend:HI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 2)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(plus:HI (ashift:HI (zero_extend:HI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 2))
		 (match_operand:HI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(and:HI (ashift:HI (subreg:HI (match_operand:QI 1 "memory_operand" "m") 0)
			  (const_int 2))
		(const_int 1020)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(plus:HI (and:HI (ashift:HI (subreg:HI (match_operand:QI 1 "memory_operand" "m") 0)
				   (const_int 2))
			 (const_int 1020))
		 (match_operand:HI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(and:HI (ashift:HI (match_operand:HI 1 "register_operand" "0")
			  (const_int 2))
		(const_int 1020)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r")
	(plus:HI (and:HI (ashift:HI (match_operand:HI 1 "register_operand" "0")
				   (const_int 2))
			 (const_int 1020))
		 (match_operand:HI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (zero_extend:SI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/b.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(mult:SI (zero_extend:SI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 2)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (mult:SI (zero_extend:SI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 2))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (mult:SI (subreg:SI (match_operand:QI 1 "memory_operand" "m") 0)
			  (const_int 2))
		(const_int 510)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (mult:SI (subreg:SI (match_operand:QI 1 "memory_operand" "m") 0)
				   (const_int 2))
			 (const_int 510))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (mult:SI (match_operand:SI 1 "register_operand" "0")
			  (const_int 2))
		(const_int 510)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (mult:SI (match_operand:SI 1 "register_operand" "0")
				   (const_int 2))
			 (const_int 510))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(ashift:SI (zero_extend:SI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 1)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (ashift:SI (zero_extend:SI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 1))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (subreg:SI (match_operand:QI 1 "memory_operand" "m") 0)
			  (const_int 1))
		(const_int 510)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (ashift:SI (subreg:SI (match_operand:QI 1 "memory_operand" "m") 0)
				   (const_int 1))
			 (const_int 510))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (match_operand:SI 1 "register_operand" "0")
			  (const_int 1))
		(const_int 510)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (ashift:SI (match_operand:SI 1 "register_operand" "0")
				   (const_int 1))
			 (const_int 510))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(mult:SI (zero_extend:SI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 4)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (mult:SI (zero_extend:SI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 4))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (mult:SI (subreg:SI (match_operand:QI 1 "memory_operand" "m") 0)
			  (const_int 4))
		(const_int 1020)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (mult:SI (subreg:SI (match_operand:QI 1 "memory_operand" "m") 0)
				   (const_int 4))
			 (const_int 1020))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (mult:SI (match_operand:SI 1 "register_operand" "0")
			  (const_int 4))
		(const_int 1020)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (mult:SI (match_operand:SI 1 "register_operand" "0")
				   (const_int 4))
			 (const_int 1020))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(ashift:SI (zero_extend:SI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 2)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (ashift:SI (zero_extend:SI (match_operand:QI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 2))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (subreg:SI (match_operand:QI 1 "memory_operand" "m") 0)
			  (const_int 2))
		(const_int 1020)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (ashift:SI (subreg:SI (match_operand:QI 1 "memory_operand" "m") 0)
				   (const_int 2))
			 (const_int 1020))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (match_operand:SI 1 "register_operand" "0")
			  (const_int 2))
		(const_int 1020)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%X1.b),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (ashift:SI (match_operand:SI 1 "register_operand" "0")
				   (const_int 2))
			 (const_int 1020))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%X1.b),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(plus:HI (mult:HI (match_operand:HI 1 "h8300_dst_operand" "0,rQ")
			   (const_int 2))
		 (match_operand:HI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(plus:HI (ashift:HI (match_operand:HI 1 "h8300_dst_operand" "0,rQ")
			   (const_int 1))
		 (match_operand:HI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(plus:HI (mult:HI (match_operand:HI 1 "h8300_dst_operand" "0,rQ")
			   (const_int 4))
		 (match_operand:HI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:HI 0 "register_operand" "=r,r")
	(plus:HI (ashift:HI (match_operand:HI 1 "h8300_dst_operand" "0,rQ")
			   (const_int 2))
		 (match_operand:HI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (zero_extend:SI (match_operand:HI 1 "h8300_dst_operand" "0,rQ"))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/b.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(mult:SI (zero_extend:SI (match_operand:HI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 2)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (mult:SI (zero_extend:SI (match_operand:HI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 2))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (mult:SI (subreg:SI (match_operand:HI 1 "memory_operand" "m") 0)
			  (const_int 2))
		(const_int 131070)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (mult:SI (subreg:SI (match_operand:HI 1 "memory_operand" "m") 0)
				   (const_int 2))
			 (const_int 131070))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (mult:SI (match_operand:SI 1 "register_operand" "0")
			  (const_int 2))
		(const_int 131070)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (mult:SI (match_operand:SI 1 "register_operand" "0")
				   (const_int 2))
			 (const_int 131070))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(ashift:SI (zero_extend:SI (match_operand:HI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 1)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (ashift:SI (zero_extend:SI (match_operand:HI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 1))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (subreg:SI (match_operand:HI 1 "memory_operand" "m") 0)
			  (const_int 1))
		(const_int 131070)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (ashift:SI (subreg:SI (match_operand:HI 1 "memory_operand" "m") 0)
				   (const_int 1))
			 (const_int 131070))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (match_operand:SI 1 "register_operand" "0")
			  (const_int 1))
		(const_int 131070)))]
  "TARGET_H8300SX"
  "mova/w.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (ashift:SI (match_operand:SI 1 "register_operand" "0")
				   (const_int 1))
			 (const_int 131070))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/w.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(mult:SI (zero_extend:SI (match_operand:HI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 4)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (mult:SI (zero_extend:SI (match_operand:HI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 4))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (mult:SI (subreg:SI (match_operand:HI 1 "memory_operand" "m") 0)
			  (const_int 4))
		(const_int 262140)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (mult:SI (subreg:SI (match_operand:HI 1 "memory_operand" "m") 0)
				   (const_int 4))
			 (const_int 262140))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (mult:SI (match_operand:SI 1 "register_operand" "0")
			  (const_int 4))
		(const_int 262140)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (mult:SI (match_operand:SI 1 "register_operand" "0")
				   (const_int 4))
			 (const_int 262140))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(ashift:SI (zero_extend:SI (match_operand:HI 1 "h8300_dst_operand" "0,rQ"))
		  (const_int 2)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (ashift:SI (zero_extend:SI (match_operand:HI 1 "h8300_dst_operand" "0,rQ"))
			   (const_int 2))
		 (match_operand:SI 2 "immediate_operand" "i,i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (subreg:SI (match_operand:HI 1 "memory_operand" "m") 0)
			  (const_int 2))
		(const_int 262140)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (ashift:SI (subreg:SI (match_operand:HI 1 "memory_operand" "m") 0)
				   (const_int 2))
			 (const_int 262140))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (match_operand:SI 1 "register_operand" "0")
			  (const_int 2))
		(const_int 262140)))]
  "TARGET_H8300SX"
  "mova/l.l @(0,%T1.w),%S0"
  [(set_attr "length_table" "mova_zero")])

(define_insn ""
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (and:SI (ashift:SI (match_operand:SI 1 "register_operand" "0")
				   (const_int 2))
			 (const_int 262140))
		 (match_operand:SI 2 "immediate_operand" "i")))]
  "TARGET_H8300SX"
  "mova/l.l @(%o2%C2,%T1.w),%S0"
  [(set_attr "length_table" "mova")])

