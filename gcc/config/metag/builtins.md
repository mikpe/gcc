;; Machine description for GNU compiler,
;; Imagination Technologies Meta version.
;; Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
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

;;- meta builtins functions

;; "__builtin_dcache_preload" data cache preload
(define_insn "dcache_preload"
  [(set (reg:SI RAPF_REG)
        (unspec_volatile:SI [(match_operand:SI 0 "metag_register_op" "r")] VUNSPEC_DCACHE_PRELOAD))]
  "TARGET_BUILTINS_METAC_1_1 || TARGET_BUILTINS_METAC_1_2 || TARGET_BUILTINS_METAC_2_1"
  "MOV\\tRAPF,%0\\t\\t%@ (*prefetch OK)"
  [(set_attr "type" "fast")])

;; "__builtin_dcache_flush" data cache flush
(define_insn "dcache_flush"
  [(unspec_volatile [(match_operand:SI 0 "metag_register_op" "r")
                     (match_operand:SI 1 "metag_register_op" "r")] VUNSPEC_DCACHE)]
  "TARGET_BUILTINS_METAC_1_2 || TARGET_BUILTINS_METAC_2_1"
  "DCACHE\\t[%0],%1\\t\\t%@ (*flush OK)"
  [(set_attr "type" "fast")])

;; "__builtin_dcache_refresh" data cache refresh
(define_insn "dcache_refresh"
  [(set (reg:SI RAPF_REG)
        (unspec_volatile:SI [(match_operand:SI 0 "metag_register_op" "d")
                             (match_operand:SI 1 "metag_register_op" "a")] VUNSPEC_DCACHE_REFRESH))]
  "TARGET_BUILTINS_METAC_1_1 || TARGET_BUILTINS_METAC_1_2 || TARGET_BUILTINS_METAC_2_1"
  "*
{
  if (TARGET_BUILTINS_METAC_1_1)
    output_asm_insn (\"SETB\\t[%0],%1\\t\\t%@ (*refresh ...\\n\\tLSL\\tRAPF,%0,#6\\t\\t%@ ... OK)\",
                     operands);
  else
    output_asm_insn (\"DCACHE\\t[%0],%1\\t\\t%@ (*refresh ...\\n\\tADD\\tRAPF,%0,#0\\t\\t%@ ... OK)\",
                     operands);
  return \"\";
}"
  [(set_attr "type" "two")])

;; "__builtin_meta2_cacherd"
(define_insn "meta2_cacherd"
  [(set (match_operand:SI 0 "metag_register_op" "=r")
        (unspec_volatile:SI [(match_operand:SI 1 "metag_register_op" "r")] VUNSPEC_META2_CACHERD))]
  "TARGET_BUILTINS_METAC_2_1"
  "CACHERD\\t%0,[%1]\\t\\t%@ (*cacherd OK)"
  [(set_attr "type" "fast")])

;; "__builtin_meta2_cacherl"
(define_insn "meta2_cacherl"
  [(set (match_operand:DI 0 "metag_register_op" "=r")
        (unspec_volatile:DI [(match_operand:SI 1 "metag_register_op" "r")] VUNSPEC_META2_CACHERL))]
  "TARGET_BUILTINS_METAC_2_1"
  "CACHERL\\t%0,%t0,[%1]\\t\\t%@ (*cacherl OK)"
  [(set_attr "type" "fast")])

;; "__builtin_meta2_cachewd"
(define_insn "meta2_cachewd"
  [(unspec_volatile [(match_operand:SI 0 "metag_register_op" "r")
                     (match_operand:SI 1 "metag_register_op" "r")] VUNSPEC_META2_CACHEWD)]
  "TARGET_BUILTINS_METAC_2_1"
  "CACHEWD\\t[%0],%1\\t\\t%@ (*cachewd OK)"
  [(set_attr "type" "fast")])

;; "__builtin_meta2_cachewl"
(define_insn "meta2_cachewl"
  [(unspec_volatile [(match_operand:SI 0 "metag_register_op" "r")
                     (match_operand:DI 1 "metag_register_op" "r")] VUNSPEC_META2_CACHEWL)]
  "TARGET_BUILTINS_METAC_2_1"
  "CACHEWL\\t[%0],%1,%t1\\t\\t%@ (*cachewl OK)"
  [(set_attr "type" "fast")])

; "__builtin_metag_bswap"
(define_insn "metag_bswap"
  [(set (match_operand:SI             0 "metag_register_op" "=e,f")
        (unspec:SI [(match_operand:SI 1 "metag_register_op"  "e,f")] UNSPEC_METAG_BSWAP))]
  "TARGET_BUILTINS_METAC_2_1 && metag_meta2_bex_enabled"
  "BEXD\\t%0,%1\\t\\t%@ (*bswap OK)"
  [(set_attr "type" "fast")])

; "__builtin_metag_bswapll"
(define_insn "metag_bswapll"
  [(set (match_operand:DI             0 "metag_register_op" "=d")
        (unspec:DI [(match_operand:DI 1 "metag_register_op"  "d")] UNSPEC_METAG_BSWAPLL))]
  "TARGET_BUILTINS_METAC_2_1 && metag_meta2_bex_enabled"
  "BEXL\\t%t0,%1\\t\\t%@ (*bswapll OK)"
  [(set_attr "type" "fast")])

;; end of file
