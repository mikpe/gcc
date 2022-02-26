;; Machine description for Thread-Local Storage,
;; Imagination Technologies Meta version.
;; Copyright (C) 2009, 2010
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

;;Global dynamic insns to set up the call to __tls_get_addr
;;These insns will generate the same code for PIC and non PIC cases
(define_insn "*tls_gd_sum"
  [(set (match_operand:SI                               0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                      1 "metag_reg_nofloat_op"  "0")
                 (const:SI (unspec [(match_operand:SI   2 "tgd_symbolic_operand"  "")] UNSPEC_TLSGD))))]
  "METAG_HAVE_TLS"
  "ADD\\t%0, %1, #(%c2@TLSGD)"
  [(set_attr "type" "fast")])

(define_insn_and_split "tls_gd"
  [(set (match_operand:SI                                     0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                            1 "metag_pic_reg"         "a")
                 (const:SI (unspec [(match_operand:SI         2 "tgd_symbolic_operand"  "")] UNSPEC_TLSGD))))]
  "METAG_HAVE_TLS && operands[1] == pic_offset_table_rtx"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (const:SI (unspec [(match_dup 2)] UNSPEC_TLSGD))))]
   ""
   [])

;;Local dynamic insns to set up the call to __tls_get_addr to get the address of the start of the current module. 
;;These insns will generate the same code for PIC and non PIC cases
(define_insn "*tls_ldm_sum"
  [(set (match_operand:SI                             0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                    1 "metag_reg_nofloat_op"  "0")
                 (const:SI (unspec [(match_operand:SI 2 "tld_symbolic_operand"  "")] UNSPEC_TLSLDM))))]
  "METAG_HAVE_TLS"
  "ADD\\t%0, %1, #(%c2@TLSLDM)"
  [(set_attr "type" "fast")])

(define_insn_and_split "tls_ldm"
  [(set (match_operand:SI                             0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                    1 "metag_pic_reg"         "a")
                 (const:SI (unspec [(match_operand:SI 2 "tld_symbolic_operand"  "")] UNSPEC_TLSLDM))))]
  "METAG_HAVE_TLS && operands[1] == pic_offset_table_rtx"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (const:SI (unspec [(match_dup 2)] UNSPEC_TLSLDM))))]
   ""
   [])

;;Local dynamic insns to compute the location of the TLS object from the start of the current TLS block.
(define_insn "*tls_ldo_high"
  [(set (match_operand:SI                                      0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                             1 "metag_reg_nofloat_op"  "0")
                 (high:SI (const:SI (unspec [(match_operand:SI 2 "tld_symbolic_operand"  "")] UNSPEC_TLSLDO)))))]
  "METAG_HAVE_TLS"
  "ADDT\\t%0, %1, #HI(%c2@TLSLDO)"
  [(set_attr "type" "fast")])

(define_insn "*tls_ldo_lo_sum"
  [(set (match_operand:SI                               0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI                    1 "metag_reg_nofloat_op"  "0")
                   (const:SI (unspec [(match_operand:SI 2 "tld_symbolic_operand"  "")] UNSPEC_TLSLDO))))]
  "METAG_HAVE_TLS"
  "ADD\\t%0, %1, #LO(%c2@TLSLDO)"
  [(set_attr "type" "fast")])

(define_insn_and_split "tls_ldo"
  [(set (match_operand:SI                             0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                    1 "metag_reg_nofloat_op"  "da")
    	         (const:SI (unspec [(match_operand:SI 2 "tld_symbolic_operand"  "")] UNSPEC_TLSLDO))))]
  "METAG_HAVE_TLS"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (high:SI (const:SI (unspec [(match_dup 2)] UNSPEC_TLSLDO)))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const:SI (unspec [(match_dup 2)] UNSPEC_TLSLDO))))]
   ""
   [])

;;Initial exec insn for PIC.
(define_insn "tls_ie"
  [(set (match_operand:SI                                     0 "metag_reg_nofloat_op" "=da")
        (mem:SI (plus:SI (match_operand:SI                    1 "metag_pic_reg"         "a")
                         (const:SI (unspec [(match_operand:SI 2 "tie_symbolic_operand" "")] UNSPEC_TLSIE)))))]
  "METAG_HAVE_TLS && operands[1] == pic_offset_table_rtx"
  "GETD\t%0, [A1LbP+#(%c2@TLSIE)]")

;;Local exec insns
(define_insn "*tls_le_high"
  [(set (match_operand:SI                                      0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                             1 "metag_reg_nofloat_op"  "0")
                 (high:SI (const:SI (unspec [(match_operand:SI 2 "tle_symbolic_operand"  "")] UNSPEC_TLSLE)))))]
  "METAG_HAVE_TLS"
  "ADDT\\t%0, %1, #HI(%c2@TLSLE)"
  [(set_attr "type" "fast")])

(define_insn "*tls_le_lo_sum"
  [(set (match_operand:SI                               0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI                    1 "metag_reg_nofloat_op"  "0")
                   (const:SI (unspec [(match_operand:SI 2 "tle_symbolic_operand"  "")] UNSPEC_TLSLE))))]
  "METAG_HAVE_TLS"
  "ADD\\t%0, %1, #LO(%c2@TLSLE)"
  [(set_attr "type" "fast")])

(define_insn_and_split "tls_le"
  [(set (match_operand:SI                             0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                    1 "metag_reg_nofloat_op"  "da")
    	         (const:SI (unspec [(match_operand:SI 2 "tle_symbolic_operand"  "")] UNSPEC_TLSLE))))]
  "METAG_HAVE_TLS"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (high:SI (const:SI (unspec [(match_dup 2)] UNSPEC_TLSLE)))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const:SI (unspec [(match_dup 2)] UNSPEC_TLSLE))))]
   ""
   [])

;;
;; Predicates
;;

;; Return true if OP is a symbolic operand for the TLS Global Dynamic model.
(define_predicate "tgd_symbolic_operand"
  (and (match_code "symbol_ref")
       (match_test "SYMBOL_REF_TLS_MODEL (op) == TLS_MODEL_GLOBAL_DYNAMIC")))

;; Return true if OP is a symbolic operand for the TLS Local Dynamic model.
(define_predicate "tld_symbolic_operand"
  (and (match_code "symbol_ref")
       (match_test "SYMBOL_REF_TLS_MODEL (op) == TLS_MODEL_LOCAL_DYNAMIC")))

;; Return true if OP is a symbolic operand for the TLS Initial Exec model.
(define_predicate "tie_symbolic_operand"
  (and (match_code "symbol_ref")
       (match_test "SYMBOL_REF_TLS_MODEL (op) == TLS_MODEL_INITIAL_EXEC")))

;; Return true if OP is a symbolic operand for the TLS Local Exec model.
(define_predicate "tle_symbolic_operand"
  (and (match_code "symbol_ref")
       (match_test "SYMBOL_REF_TLS_MODEL (op) == TLS_MODEL_LOCAL_EXEC")))

;; end of file

