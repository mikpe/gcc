;; Constants definitions for META
;; Copyright (C) 2007 Imagination Technologies Ltd

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

;; UNSPEC 
(define_constants
 [(UNSPEC_GOTOFF                        1)
  (UNSPEC_GOT                           2)
  (UNSPEC_PLT                           3)
  (UNSPEC_PIC                           4)
  (UNSPEC_PIC_BASE                      5)

  (UNSPEC_PROLOGUE_USE                  6)
  (UNSPEC_CONCAT                        7)
  (UNSPEC_SIBCALL                       8)
  (UNSPEC_SIBCALL_VALUE                 9)

  (UNSPEC_RET_COND                      10)
  (UNSPEC_RET_COND_INVERTED             11)

  (UNSPEC_METAG_BSWAP                   12)
  (UNSPEC_METAG_BSWAPLL                 13)

  (UNSPEC_MINIM_JUMP_TABLE              14)

  (UNSPEC_FIRST_TLS                     15)
  (UNSPEC_TLS                           15)
  (UNSPEC_TLSGD                         16)
  (UNSPEC_TLSLDM                        17)
  (UNSPEC_TLSLDO                        18)
  (UNSPEC_TLSIE                         19)
  (UNSPEC_TLSLE                         20)
  (UNSPEC_LAST_TLS                      20)])


;; UNSPEC VOLATILE
(define_constants
 [(VUNSPEC_DCACHE_PRELOAD               1)
  (VUNSPEC_DCACHE                       2)
  (VUNSPEC_DCACHE_REFRESH               3)
  (VUNSPEC_BLOCKAGE                     4)
  (VUNSPEC_EPILOGUE                     5)
  (VUNSPEC_EH_RETURN                    6)
  (VUNSPEC_META2_CACHERD                7)
  (VUNSPEC_META2_CACHERL                8)
  (VUNSPEC_META2_CACHEWD                9)
  (VUNSPEC_META2_CACHEWL               10)
  (VUNSPEC_TTMOV                       11)
  (VUNSPEC_TTREC                       12)])


;; Register
(define_constants
 [(D0Ar6_REG                            2)
  (D1Ar5_REG                            3)
  (D0Ar4_REG                            4)
  (D1Ar3_REG                            5)
  (D0Ar2_REG                            6)
  (D1Ar1_REG                            7)

  (D0Re0_REG                            0)
  (D1Re0_REG                            1)
  (D0FrT_REG                            8)
  (D1RtP_REG                            9)

  (FIRST_DATA_REG                       0)
  (D0_0_REG                             0)
  (D1_0_REG                             1)
  (D0_1_REG                             2)
  (D1_1_REG                             3)
  (D0_2_REG                             4)
  (D1_2_REG                             5)
  (D0_3_REG                             6)
  (D1_3_REG                             7)
  (D0_4_REG                             8)
  (D1_4_REG                             9)
  (D0_5_REG                             10)
  (D1_5_REG                             11)
  (D0_6_REG                             12)
  (D1_6_REG                             13)
  (D0_7_REG                             14)
  (D1_7_REG                             15)
  (FIRST_ECH_DATA_REG                   16)
  (D0_8_REG                             16)
  (D1_8_REG                             17)
  (D0_9_REG                             18)
  (D1_9_REG                             19)
  (D0_10_REG                            20)
  (D1_10_REG                            21)
  (D0_11_REG                            22)
  (D1_11_REG                            23)
  (D0_12_REG                            24)
  (D1_12_REG                            25)
  (D0_13_REG                            26)
  (D1_13_REG                            27)
  (D0_14_REG                            28)
  (D1_14_REG                            29)
  (D0_15_REG                            30)
  (D1_15_REG                            31)
  (LAST_DATA_REG                        31)
 
  (A0StP_REG                            32)
  (A1GbP_REG                            33)
  (A0FrP_REG                            34)
  (A1LbP_REG                            35)

  (PIC_REG                              35)

  (FIRST_ADDR_REG                       32)
  (A0_0_REG                             32)
  (A1_0_REG                             33)
  (A0_1_REG                             34)
  (A1_1_REG                             35)
  (A0_2_REG                             36)
  (A1_2_REG                             37)
  (A0_3_REG                             38)
  (A1_3_REG                             39)
  (FIRST_ECH_ADDR_REG                   40)
  (A0_4_REG                             40)
  (A1_4_REG                             41)
  (A0_5_REG                             42)
  (A1_5_REG                             43)
  (A0_6_REG                             44)
  (A1_6_REG                             45)
  (A0_7_REG                             46)
  (A1_7_REG                             47)
  (LAST_ADDR_REG                        47)

  (FRAME_REG                            48)
  (CC_REG                               49)
  (ARGP_REG                             50)
  (RAPF_REG                             51)
  (CPC0_REG                             52)
  (CPC1_REG                             53)
  (PC_REG                               54)
  (TXRPT_REG                            55)
  
  (FIRST_FP_REG                         56)
  (FX_0_REG                             56)
  (FX_1_REG                             57)
  (FX_2_REG                             58)
  (FX_3_REG                             59)
  (FX_4_REG                             60)
  (FX_5_REG                             61)
  (FX_6_REG                             62)
  (FX_7_REG                             63)
  (FX_8_REG                             64)
  (FX_9_REG                             65)
  (FX_10_REG                            66)
  (FX_11_REG                            67)
  (FX_12_REG                            68)
  (FX_13_REG                            69)
  (FX_14_REG                            70)
  (FX_15_REG                            71)
  (LAST_FP_REG                          71)
  (TTREC_REG                            72)
  (TTRECL_REG                           73)
  (LAST_REG                             73)])

;; Exception handling - dwarf2 call frame unwinder
(define_constants
 [(EH_RETURN_FIRST_DATA_REG             2)
  (EH_RETURN_LAST_DATA_REG              3)
  (EH_RETURN_STACKADJ_REG               4)])
