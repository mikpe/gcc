;; Machine description for GNU compiler,
;; Imagination Technologies Meta version.
;; Copyright (C) 2007
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

(define_automaton "metag")

;; The UI and U2U instuctions have different ports to write results
(define_cpu_unit "UI_port, U2U_port, load_port" "metag")

;; The FPU ports to write results
(define_cpu_unit "FP_port1, FP_port2" "metag")

;; The FPU pipeline
(define_cpu_unit "mas1, mas2, mas3, mas4, mas5, recip1, recip2, recip3" "metag")

;; All instructions have to be fetched!
(define_cpu_unit "issue" "metag")

;; Multi-cycle operation stages
(define_cpu_unit "opfetch, execute" "metag")

(define_cpu_unit "branch, load, read, mult" "metag")

;; Simple case, single cycle operations
(define_insn_reservation "UI" 1
                         (eq_attr "type" "fast")
                         "issue, UI_port")

;; WORK NEEDED: This needs checking, does a swap need to write each register on separate cycles?
(define_insn_reservation "UIswap" 1
                         (eq_attr "type" "swap")
                         "issue, UI_port, UI_port")

;; Unknown type insns are user supplied ASM, invalid should not occur and nop is...
(define_insn_reservation "UIother" 1
                         (eq_attr "type" "unknown, invalid, nop, block")
                         "issue, UI_port")

;; Multiply has 2 cycle latency (only MULD comes through here)
(define_insn_reservation "UImult" 2
                         (eq_attr "type" "mult")
                         "issue, mult, UI_port")

;; Multiple UI instruction have a complex latency
(define_insn_reservation "UIUI" 2
                         (eq_attr "type" "two")
                         "issue, issue + UI_port, UI_port")

(define_insn_reservation "UIUIUI" 3
                         (eq_attr "type" "three")
                         "issue, issue + UI_port, issue + UI_port, UI_port")

(define_insn_reservation "UIUIUIUI" 4
                         (eq_attr "type" "four")
                         "issue, issue + UI_port, issue + UI_port, issue + UI_port, UI_port")

(define_insn_reservation "UIUIUIUIUI" 5
                         (eq_attr "type" "five")
                         "issue, issue + UI_port, issue + UI_port, issue + UI_port, issue + UI_port, UI_port")

;; Inter unit operations
;; Results are written on 3rd cycle but are ready to be used on 2nd cycle using feedback path
(define_insn_reservation "U2U" 2
                         (eq_attr "type" "slow")
                         "issue, opfetch, execute, U2U_port")

(define_insn_reservation "U2UU2U" 3
                         (eq_attr "type" "slowslow")
                         "issue, issue + opfetch, opfetch + execute, execute + U2U_port, U2U_port")

;; META 2 FPU

(define_reservation "FP_ports" "FP_port1 | FP_port2")
(define_reservation "mas" "mas1, mas2, mas3, mas4, mas5")
(define_reservation "recip" "recip1, recip2, recip3")

(define_insn_reservation "FP" 1
                         (eq_attr "type" "FPfast")
                         "issue, FP_ports")

(define_insn_reservation "FPmas" 5
                         (eq_attr "type" "FPmas")
                         "issue, mas, FP_ports")

(define_insn_reservation "FPrecip" 3
                         (eq_attr "type" "FPrecip")
                         "issue, recip, FP_ports")

(define_insn_reservation "FPrecipmas" 8
                         (eq_attr "type" "FPrecipmas")
                         "issue, recip, FP_ports + issue, mas, FP_ports")

;; META 2_1
(define_insn_reservation "branch_2_1" 5
                         (and (eq_attr "metacore" "metac_2_1")
                              (eq_attr "type" "branch"))
                         "issue, branch*3, UI_port")

(define_insn_reservation "load_2_1" 3
                         (and (eq_attr "metacore" "metac_2_1")
                              (eq_attr "type" "load"))
                         "issue, load, load_port")

(define_insn_reservation "read_2_1" 3
                         (and (eq_attr "metacore" "metac_2_1")
                              (eq_attr "type" "read"))
                         "issue, read, load_port")

;; META 1_2
(define_insn_reservation "branch_1_2" 5
                         (and (eq_attr "metacore" "metac_1_2")
                              (eq_attr "type" "branch"))
                         "issue, branch*3, UI_port")

(define_insn_reservation "load_1_2" 3
                         (and (eq_attr "metacore" "metac_1_2")
                              (eq_attr "type" "load"))
                         "issue, load, load_port")

(define_insn_reservation "read_1_2" 3
                         (and (eq_attr "metacore" "metac_1_2")
                              (eq_attr "type" "read"))
                         "issue, read, load_port")
;; META 1_1
(define_insn_reservation "branch_1_1" 6
                         (and (eq_attr "metacore" "metac_1_1")
                              (eq_attr "type" "branch"))
                         "issue, branch*4, UI_port")

(define_insn_reservation "load_1_1" 8
                         (and (eq_attr "metacore" "metac_1_1")
                              (eq_attr "type" "load"))
                         "issue, load, load_port")

(define_insn_reservation "read_1_1" 8
                         (and (eq_attr "metacore" "metac_1_1")
                              (eq_attr "type" "read"))
                         "issue, read, load_port")
;; META 1_0
(define_insn_reservation "branch_1_0" 7
                         (and (eq_attr "metacore" "metac_1_0")
                              (eq_attr "type" "branch"))
                         "issue, branch*5, UI_port")

(define_insn_reservation "load_1_0" 10
                         (and (eq_attr "metacore" "metac_1_0")
                              (eq_attr "type" "load"))
                         "issue, load, load_port")

(define_insn_reservation "read_1_0" 9
                         (and (eq_attr "metacore" "metac_1_0")
                              (eq_attr "type" "read"))
                         "issue, read, load_port")

;; MGET latencies are complex... The minimum latency is specified here
;; and the more complex work is done in metag_sched_adjust_cost

;; META 1_2 and 2_1
(define_insn_reservation "UI2xld_2_1" 1
                         (and (ior (eq_attr "metacore" "metac_1_2")
                                   (eq_attr "metacore" "metac_2_1"))
                              (and (eq_attr "type" "twox")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, load_port, load_port")

(define_insn_reservation "UI3xld_2_1" 1
                         (and (ior (eq_attr "metacore" "metac_1_2")
                                   (eq_attr "metacore" "metac_2_1"))
                              (and (eq_attr "type" "threex")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, issue + load_port, load_port, load_port")

(define_insn_reservation "UI4xld_2_1" 1
                         (and (ior (eq_attr "metacore" "metac_1_2")
                                   (eq_attr "metacore" "metac_2_1"))
                              (and (eq_attr "type" "fourx")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, issue + load_port, issue + load_port, load_port, load_port")

(define_insn_reservation "UI5xld_2_1" 1
                         (and (ior (eq_attr "metacore" "metac_1_2")
                                   (eq_attr "metacore" "metac_2_1"))
                              (and (eq_attr "type" "fivex")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, issue + load_port, issue + load_port, issue + load_port, load_port, load_port")

;; META 1_1 - The stalls are not accurately modelled here
(define_insn_reservation "UI2xld_1_1" 7
                         (and (eq_attr "metacore" "metac_1_1")
                              (and (eq_attr "type" "twox")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, load*5, load_port, load_port")

(define_insn_reservation "UI3xld_1_1" 7
                         (and (eq_attr "metacore" "metac_1_1")
                              (and (eq_attr "type" "threex")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, issue, load *4, load_port, load_port, load_port")

(define_insn_reservation "UI4xld_1_1" 7
                         (and (eq_attr "metacore" "metac_1_1")
                              (and (eq_attr "type" "fourx")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, issue, issue, load*3, load_port, load_port, load_port, load_port")

(define_insn_reservation "UI5xld_1_1" 7
                         (and (eq_attr "metacore" "metac_1_1")
                              (and (eq_attr "type" "fivex")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, issue, issue, issue, load*2, load_port, load_port, load_port, load_port, load_port")

;; META 1_0 - The stalls are not accurately modelled here
(define_insn_reservation "UI2xld_1_0" 9
                         (and (eq_attr "metacore" "metac_1_0")
                              (and (eq_attr "type" "twox")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, load*8, load_port, load_port")

(define_insn_reservation "UI3xld_1_0" 9
                         (and (eq_attr "metacore" "metac_1_0")
                              (and (eq_attr "type" "threex")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, issue, load*7, load_port, load_port, load_port")

(define_insn_reservation "UI4xld_1_0" 9
                         (and (eq_attr "metacore" "metac_1_0")
                              (and (eq_attr "type" "fourx")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, issue, issue, load*6, load_port, load_port, load_port, load_port")

(define_insn_reservation "UI5xld_1_0" 9
                         (and (eq_attr "metacore" "metac_1_0")
                              (and (eq_attr "type" "fivex")
                                   (eq_attr "memaccess" "load")))
                         "issue, issue, issue, issue, issue, load*5, load_port, load_port, load_port, load_port, load_port")

(define_insn_reservation "UI2xst" 1
                         (and (eq_attr "type" "twox")
                              (eq_attr "memaccess" "store"))
                         "issue, issue")

(define_insn_reservation "UI3xst" 1
                         (and (eq_attr "type" "threex")
                              (eq_attr "memaccess" "store"))
                         "issue, issue, issue")

(define_insn_reservation "UI4xst" 1
                         (and (eq_attr "type" "fourx")
                              (eq_attr "memaccess" "store"))
                         "issue, issue, issue, issue")

(define_insn_reservation "UI5xst" 1
                         (and (eq_attr "type" "fivex")
                              (eq_attr "memaccess" "store"))
                         "issue, issue, issue, issue, issue")

;; Catch all the cond_exec cases that will cause single cycle ops to become 3 cycle
;; WORK NEEDED: optimize those cases where the condition is ALWAYS or NONE or if the
;;              condition value is known at issue (no updates to CC in the pipeline).
;;              In such cases the instruction becomes UI (if it were UI to start with).
(define_bypass 2 "UI, FP"
                 "UIother,UI,UIswap,UImult, \
                  UIUI,UIUIUI,UIUIUIUI,UIUIUIUIUI,U2U,U2UU2U, \
                  branch_2_1,load_2_1,read_2_1, \
                  branch_1_2,load_1_2,read_1_2, \
                  branch_1_1,load_1_1,read_1_1, \
                  branch_1_0,load_1_0,read_1_0, \
                  UI2xld_2_1,UI3xld_2_1,UI4xld_2_1,UI5xld_2_1, \
                  UI2xld_1_1,UI3xld_1_1,UI4xld_1_1,UI5xld_1_1, \
                  UI2xld_1_0,UI3xld_1_0,UI4xld_1_0,UI5xld_1_0, \
                  UI2xst,UI3xst,UI4xst,UI5xst,\
                  FP, FPmas, FPrecip, FPrecipmas"
                 "metag_consumer_is_cond_p")

;; SWAP has to write two registers so in effect takes 2 cycles rather than one so it is
;; a slightly slower case.
(define_bypass 4 "UIswap"
                 "UIother,UI,UIswap,UImult, \
                  UIUI,UIUIUI,UIUIUIUI,UIUIUIUIUI,U2U,U2UU2U, \
                  branch_2_1,load_2_1,read_2_1, \
                  branch_1_2,load_1_2,read_1_2, \
                  branch_1_1,load_1_1,read_1_1, \
                  branch_1_0,load_1_0,read_1_0, \
                  UI2xld_2_1,UI3xld_2_1,UI4xld_2_1,UI5xld_2_1, \
                  UI2xld_1_1,UI3xld_1_1,UI4xld_1_1,UI5xld_1_1, \
                  UI2xld_1_0,UI3xld_1_0,UI4xld_1_0,UI5xld_1_0, \
                  UI2xst,UI3xst,UI4xst,UI5xst, \
                  FP, FPmas, FPrecip, FPrecipmas"
                 "metag_consumer_is_cond_p")

;; Regalloc/reload needs registers to have the shortest possible lifetime in order to 
;; successfully allocate registers. D0Re0, D1Re0, D1Ar6, D0Ar5 are very popular
;; especially with modes that require 2 registers, load/store multiops,
;; base + 12bit offset memory operations, argument registers, return registers...

;; The automata is very pessimistic when it comes across insns with multiple opcodes
;; it assumes that they will never be split. In reality almost all are split and
;; therefore scheduling them as one block before reload has a negative effect on 
;; the associated registers lifetime (it increases). To counter this we bypass the
;; automata prior to reload to say that multi opcode insns actually have no latency,
;; i.e. ignore them completely, there is nothing that can be inferred.
;; After reload they are treated as proper insns.

;; The reduced lifetime gives the register allocator and reload more scope to fulfil
;; insns constraints.
(define_bypass 0 "UIUI,UIUIUI,UIUIUIUI,UIUIUIUIUI,U2U,U2UU2U"
                 "UI,UIswap,UIother,UImult,UIUI,UIUIUI,UIUIUIUI,UIUIUIUIUI, \
                  U2U,U2UU2U,FP,FPmas,FPrecip,FPrecipmas, \
                  branch_2_1,load_2_1,read_2_1, \
                  branch_1_2,load_1_2,read_1_2, \
                  branch_1_1,load_1_1,read_1_1, \
                  branch_1_0,load_1_0,read_1_0, \
                  UI2xld_2_1,UI3xld_2_1,UI4xld_2_1,UI5xld_2_1, \
                  UI2xld_1_1,UI3xld_1_1,UI4xld_1_1,UI5xld_1_1, \
                  UI2xld_1_0,UI3xld_1_0,UI4xld_1_0,UI5xld_1_0, \
                  UI2xst,UI3xst,UI4xst,UI5xst"
                 "metag_bypass_before_reload_p")

