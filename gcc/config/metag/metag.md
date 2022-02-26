;; Machine description for GNU compiler,
;; Imagination Technologies Meta version.
;; Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2010,
;;               2013
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

;;- instruction definitions

;;- @@The original PO technology requires these to be ordered by speed,
;;- @@    so that assigner will pick the fastest.

;;- See file "rtl.def" for documentation on define_insn, match_*, et. al.

;;- When naming insn's (operand 0 of define_insn) be careful about using
;;- names from other targets machine descriptions.


;; MODE macros
(define_mode_macro CCALL  [CC_FP CC_FP_Q CC CC_NOOV CC_Z])

(define_mode_macro CCFP   [CC_FP CC_FP_Q])

(define_mode_macro CCANY  [CC CC_NOOV CC_Z])

(define_mode_macro CCZNC  [CC_NOOV CC_Z])

(define_mode_macro IMODES [QI HI SI DI])

(define_mode_macro FMODES [SF DF])

(define_mode_macro FLMODES [SF DF V2SF])

; Floating point storage modes
; This is same as FMODES at the moment but HF would go here when it's supported
; When HF is added some insns need to be split as HF conversions have a single
; cycle stall whereas others have 5 cycle stalls
(define_mode_macro FSMODES [SF DF])

(define_mode_macro MODES  [QI HI SI DI SF DF V2SF V2SI])

(define_mode_macro EXTDI  [QI HI SI])

(define_mode_macro EXTSI  [QI HI])

(define_mode_macro EXTHI  [QI])

(define_mode_macro MEMOP  [QI HI SI]) 

;; MODE attrs

(define_mode_attr  S     [(DI "8") (SI "4") (HI "2") (QI "1") (SF "4") (DF "8") (V2SF "8") (V2SI "8")])

(define_mode_attr  W     [(DI "L") (SI "D") (HI "W") (QI "B") (SF "D") (DF "L") (V2SF "L") (V2SI "L")])

(define_mode_attr  P     [(DI "l") (SI "d") (HI "w") (QI "b") (SF "d") (DF "l") (V2SF "l") (V2SI "l")])

(define_mode_attr  O     [(DI "O8") (SI "O4") (HI "O2") (QI "O1") (SF "O4") (DF "O8") (V2SF "O8") (V2SI "O8")])

(define_mode_attr  Z     [(DI "Z8") (SI "Z4") (HI "Z2") (QI "Z1") (SF "Z4") (DF "Z8") (V2SF "Z8") (V2SI "Z8")])

(define_mode_attr  FT    [(SF "F") (DF "D")])
(define_mode_attr  FW    [(SF "" ) (DF "D") (DI "D") (V2SF "L")])
(define_mode_attr  FCONDITION [(SF "TARGET_FPU") (DF "(TARGET_FPU && !metag_fpu_single)") (V2SF "TARGET_FPU_SIMD")])

(define_mode_attr  CCQ   [(CC_FP "")  (CC_FP_Q "Q")])

;; CODE macros

(define_code_macro CCCOND [eq ne gt gtu lt ltu ge geu le leu])

(define_code_macro CCFPCOND [eq ne
                             gt ge lt le unordered ltgt ordered
                             ungt unge unlt unle uneq])

(define_code_macro CCANYCOND [eq ne
                              gtu ltu geu leu
                              gt ge lt le unordered ordered
                              ungt unge unlt unle])

;; These code macros significantly simplify the peephole and vector patterns required
;; for supporting dual unit DSP operations

(define_code_macro 3OPREG    [plus minus and ior xor ashift ashiftrt lshiftrt])
(define_code_macro 3OPIMM16  [plus minus and ior xor])
(define_code_macro 3OPIMM5   [ashift ashiftrt lshiftrt])
(define_code_macro MINMAX    [smin smax])

;; CODE attrs

;; These code attributes are used for peephole and vector patterns that support
;; dual unit DSP operations

(define_code_attr MNEMONIC [(plus "ADD") (minus "SUB") (and "AND") (ior "OR") (xor "XOR")
                            (ashift "LSL") (ashiftrt "ASR") (lshiftrt "LSR") (smin "MIN")
                            (smax "MAX")])

(define_code_attr commutative [(plus "true") (minus "false") (and "true") (ior "true") (xor "true")
                               (ashift "false") (ashiftrt "false") (lshiftrt "false") (smin "true")
                               (smax "true")])

(define_code_attr expander [(plus "add") (minus "sub") (and "and") (ior "ior") (xor "xor")
                            (ashift "ashl") (ashiftrt "ashr") (lshiftrt "lshr") (smin "smin")
                            (smax "smax")])

(define_code_attr dualunitimmcondition [(plus "&& !(TARGET_METAC_1_0 && (INTVAL (operands[1]) & 1) == 1)")
                                        (minus "&& !(TARGET_METAC_1_0 && (INTVAL (operands[1]) & 1) == 1)")
                                        (and "") (ior "") (xor "") (ashift "") (ashiftrt "") (lshiftrt "")
                                        (smin "") (smax "")])

;; constants


(include "constants.md")
(include "predicates.md")
(include "constraints.md")
(include "pipeline.md")

;; Insn type.  Used to default other attribute values.
(define_attr "type"
  "fast,swap,slow,two,twox,three,threex,\
   load,read,four,fourx,five,fivex,sixx,\
   sevenx,mult,slowslow,nop,block,branch,\
   FPfast,FPrecip,FPmas,FPrecipmas,unknown,invalid"
  (const_string "unknown"))

;; Indiate of insn is suitable for register renaming
(define_attr "rename" "no,yes" (const_string "yes"))

;; Indicate if insn is conditional
(define_attr "cond" "no,yes" (const_string "no"))

;; Conditional execution attribute, implicitly used when creating predicated insns
(define_attr "predicable" "no,yes" (const_string "no"))

;; Type of memory operation
(define_attr "memaccess" "load,store,none" (const_string "none"))

;; O2R hints
;; op2op1 - operand 2 is O2R if not in the same unit as operand 1
;; op1op0 - operand 1 is O2R if not in the same unit as operand 0
(define_attr "o2rhint" "op2op1,op1op0,none" (const_string "none"))

;; Insn ccstate. Used to track interaction with the condition flags

; xcc means that the condition codes are used by the insn in the process of
; outputting code

; set means that the purpose of the insn is to set the condition codes.

; ccx means that the condition codes are altered in an undefined manner, if
; they are altered at all

; ncc means that the condition codes are neither altered nor affect the
; output of this insn

(define_attr "ccstate" "xcc,set,fastset,fastfastset,ccx,ncc" (const_string "ncc"))

;; Length (in # of insns).
(define_attr "length" ""
  (cond [(eq_attr "type" "two,slowslow,read") (const_int 8)
         (eq_attr "type" "three")             (const_int 12)
         (eq_attr "type" "four")              (const_int 16)
         (eq_attr "type" "five")              (const_int 20)]
                                              (const_int 4)))

;; User supplied instructions
(define_asm_attributes
  [(set_attr "type" "unknown")
   (set_attr "ccstate" "ccx")
   (set_attr "length" "4")
   (set_attr "cond" "no")
   (set_attr "predicable" "no")
   (set_attr "memaccess" "none")
   (set_attr "o2rhint" "none")
   (set_attr "rename" "no")])


;; Metac core revision.
;; Used to select meta core revision specific insn scheduling.
;; this attribute must exactly match the processor_type enumeration in metag.h

(define_attr "metacore" "metac_1_0,metac_1_1,metac_1_2,metac_0_1,metac_2_1"
  (const (symbol_ref "metacore")))

;; prologue/epilogue
(define_expand "prologue"
  [(clobber (const_int 0))]
  ""
  {
    metag_expand_prologue ();
    DONE;
  }
)

(define_expand "epilogue"
  [(return)]
  ""
  {
    metag_expand_epilogue (false);
    emit_jump_insn (gen_return_internal ());
    DONE;
  }
)

(define_expand "sibcall_epilogue"
  [(return)]
  ""
  {
    metag_expand_epilogue (true);
    DONE;
  }
)

(define_insn "prologue_use"
  [(unspec:SI [(match_operand:SI 0 "register_operand" "r")] UNSPEC_PROLOGUE_USE)]
  ""
  "%@%@ %0 needed for prologue"
)

;; Patterns for exception handling

(define_expand "eh_return"
  [(use (match_operand:SI 0 "register_operand" "r"))]
  ""
  {
    emit_insn (gen_eh_return_internal (operands[0]));
    DONE;
  }
)

(define_insn_and_split "eh_return_internal"
  [(unspec_volatile [(match_operand:SI 0 "register_operand" "r")] VUNSPEC_EH_RETURN)]
  ""
  "#"
  "reload_completed"
  [(const_int 0)]
  {
    metag_expand_set_return_address (operands[0]);
    DONE;
  }
)


(define_insn "blockage"
  [(unspec_volatile [(const_int 0)] VUNSPEC_BLOCKAGE)]
  ""
  ""
  [(set_attr "length" "0")
   (set_attr "type"   "block")])


;; move instructions

(define_insn "swapsi"
 [(set (match_operand:SI 0 "metag_reg_nofloat_op" "+e,f,h,l")
       (match_operand:SI 1 "metag_reg_nofloat_op" "+t,u,y,z"))
  (set (match_dup 1)
       (match_dup 0))]
 ""
 "SWAP%?\\t%0, %1\\t%@ (*swap SI OK)"
 [(set_attr "type" "swap")
  (set_attr "predicable" "yes")
  (set_attr "cond" "yes")])

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; movsi is made up of many parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "movsi"
  [(set (match_operand:SI 0 "nonimmediate_operand" "")
        (match_operand:SI 1 "general_operand"      ""))]
  ""
  {

    /* The output template that was here has been moved in the
       metag_emit_move_sequence function in metag.c */
    metag_emit_move_sequence (operands, SImode);
  }
)

;; The problem:
;; GCC cannot differentiate a legitimate address from an illegitimate one when the only
;; factor affecting the split is the class that the register being loaded to or stored
;; from belongs to.

;; This problem affects FX registers with SImode values in them. FX registers can only
;; be stored and loaded from register+6bit offset whereas all other registers can be
;; stored and loaded from register+12bit offset if the base register supports it.

;; The solution:
;; Prevent reload from having a free-for-all on determining legitimate addresses. The
;; fallback case in mov_si therefore DOES NOT contain the FP register class in either
;; of the memory constraint alternatives. Reload will fix this by reloading any FX
;; register into another register as required.

;; The caveat:
;; This solution prevents FX registers being reloaded directly to and from memory. To
;; solve this, the 6 and 12 bit load/store insns are above the fallback mov_si
;; pattern so as to allow FX registers to be reloaded to and from memory directly.

;; WORK NEEDED: Prove this does actually allow FX registers holding SImode values to 
;; be directly loaded and stored to base+6bit offset addresses.

(define_insn "*sto_si_1_1_off12"
  [(set (mem:SI (plus:SI (match_operand:SI 0 "metag_reg12bit_op"    "da,Yr")
                         (match_operand:SI 1 "metag_offset12_si"    "O4,Z4")))
        (match_operand:SI                  2 "metag_reg_nofloat_op" "da,da"))]
  "TARGET_METAC_1_1"
  "SETD\\t[%0+%1], %2"
  [(set_attr "type" "fast")])

(define_insn "*sto_si_1_1_off6"
  [(set (mem:SI (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "da")
                         (match_operand:SI 1 "metag_offset6_si"     "O4")))
        (match_operand:SI                  2 "metag_register_op"    "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0+%1], %2\\t%@ (*sto SI off6 OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*lod_si_off12"
  [(set (match_operand:SI                  0 "metag_reg_nofloat_op" "=da,da")
        (mem:SI (plus:SI (match_operand:SI 1 "metag_reg12bit_op"     "da,Yr")
                         (match_operand:SI 2 "metag_offset12_si"     "O4,Z4"))))]
  ""
  "GETD\\t%0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_si_off"
  [(set (match_operand:SI                  0 "metag_register_op"    "=cr")
        (mem:SI (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                         (match_operand:SI 2 "metag_offset6_si"      "O4"))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1+%2]\\t%@ (*lod SI off6 OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; NOTE "move si" needs to handle r <- r along with r <- m and m <- r variants together.
;; Otherwise one can end up with a "impossible reload" error if an argument to inline asm
;; fails to be assigned a register and thus needs to be fixed up by reload. This isn't
;; well documented and the underlying issue can go undetected.

;; movsi - all the register to register moves

;; this variant mov_si is used if generated by reload and needs to handle memory
(define_insn "*mov_si_reload"
  [(set (match_operand:SI 0 "metag_move_dst_op" "=e,f,h,l,r,!*Wx,  r,  ?*cx,*cx,!d,  cr,!*m")
        (match_operand:SI 1 "metag_move_src_op"  "e,f,h,l,r,  r,  !*Wx,?*cx,!d, *cx,!*m, cr"))]
  "reload_in_progress || reload_completed"
  {
    switch (which_alternative)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        if (METAG_FPC_REG_P (REGNO (operands[0])) || METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\\tMOV%?\\t%0, %1"; 
        else
          return "MOV%?\\t%0, %1"; 
      case 10:
        if (METAG_FPC_REG_P (REGNO (operands[0])))
          return "F\\tGETD\\t%0, %1";
        else
          return "GETD\\t%0, %1";
      case 11:
        if (METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\\tSETD\\t%0, %1";
        else
          return "SETD\\t%0, %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,slow,fast,slow,slow,load,fast")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,no,no")
   (set_attr "memaccess" "none,none,none,none,none,none,none,none,none,none,load,store")
   (set_attr "predicable" "yes")])

;; this variant mov_si is used if NOT generated by reload and doesn't handle memory
(define_insn "*mov_si"
  [(set (match_operand:SI 0 "metag_register_op" "=e,f,h,l,r,?*cx,*cx,!d")
        (match_operand:SI 1 "metag_register_op"  "e,f,h,l,r,?*cx,!d, *cx"))]
  ""
  {
    switch (which_alternative)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        if (METAG_FPC_REG_P (REGNO (operands[0])) || METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\\tMOV%?\\t%0, %1"; 
        else
          return "MOV%?\\t%0, %1"; 
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "fast,fast,fast,fast,slow,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

(define_insn "ttmov_si"
  [(set (match_operand:SI                      0 "metag_reg_nofloat_op" "=da")
        (unspec_volatile:SI [(match_operand:SI 1 "metag_reg_nofloat_op"  "da")] VUNSPEC_TTMOV))]
  ""
  "TTMOV%?\\t%0, %1 %@ H/W Tracing. Normal MOV but with value also sent to trace port"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")
   (set_attr "rename" "no")])

(define_insn "ttrec"
  [(set (match_operand:DI                      0 "metag_ttrec_op"       "=Wx")
        (unspec_volatile:DI [(match_operand:DI 1 "metag_reg_nofloat_op"  "da")] VUNSPEC_TTREC))]
  ""
  "MOVL%?\\t%0, %1, %t1 %@ H/W Tracing. Functionally a NOP. Values sent to trace port"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")
   (set_attr "rename" "no")])

(define_insn "*mov_si_rxrpt_r"
  [(set (match_operand:SI 0 "metag_txrpt_op"    "=Wx")
        (match_operand:SI 1 "metag_register_op"  "r"))]
  ""
  "MOV%?\\t%0, %1"
  [(set_attr "type" "fast")
   (set_attr "predicable" "yes")])

(define_insn "*mov_si_r_txrpt"
  [(set (match_operand:SI 0 "metag_register_op"  "=r")
        (match_operand:SI 1 "metag_txrpt_op"      "Wx"))]
  ""
  "MOV%?\\t%0, %1"
  [(set_attr "type" "fast")
   (set_attr "predicable" "yes")])

(define_insn "*mov_si_txrpt_ri"
  [(set (match_operand:SI 0 "metag_txrpt_op"     "=Wx,Wx,Wx")
        (match_operand:SI 1 "metag_txrpt_src_op" "KIP,J, r"))]
  ""
  "@
   MOV\\t%0, %1
   MOVT\\t%0, %1
   MOV%?\\t%0, %1"
  [(set_attr "type" "fast,fast,fast")
   (set_attr "cond" "no,no,yes")])

;; movsi - all the immediate to register sets
(define_insn "*cond_<mode>_mov_si_zero"
  [(cond_exec
     (match_operator          1 "comparison_operator"
       [(match_operand:CCALL  2 "metag_<mode>_reg"     "")
        (const_int 0)])
     (set (match_operand:SI   0 "metag_reg_nofloat_op" "=d,a")
          (const_int 0)))]
  "TARGET_COND_EXEC_OPTIMIZE"
  "SUB%?\\t%0, %0, %0"
  [(set_attr "type" "fast,slow")])

(define_insn "*mov_si_zero"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=d,a")
        (const_int 0))]
  "TARGET_COND_EXEC_OPTIMIZE"
  {
    if (metag_cond_exec_p ())
      return "SUB%?\\t%0, %0, %0";
    else
      return "MOV\\t%0, #0";
  }
  [(set_attr "type" "fast,slow")
   (set_attr "cond" "yes")])

(define_insn "*set_si_KIP"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:SI 1 "metag_KIP_operand"     "KIP"))]
  ""
  "MOV\\t%0, %1\\t\\t%@ (*set si rKIP OK)"
   [(set_attr "type" "fast")])

(define_insn "*set_si_J"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:SI 1 "metag_J_operand"       "J"))]
  ""
  "MOVT\\t%0, %1\\t\\t%@ (*set si rJ OK)"
   [(set_attr "type" "fast")])

(define_insn "*set_si_HI"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:SI 1 "metag_O0_operand"      "O0"))]
  ""
  "MOVT\\t%0, #HI(%c1)\\t%@ (*set si rO0 OK)"
  [(set_attr "type" "fast")])

(define_insn "*set_si_LO"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:SI 1 "metag_O3_operand"      "O3"))]
  ""
  "MOVT\\t%0, #LO(%c1)\\t%@ (*set si rO3 OK)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=da, da,da")
        (match_operand:SI 1 "metag_bigint_op"       "IPK,J, n"))]
  ""
  "#"
  "reload_completed"
  [(const_int 0)]
  {
    metag_split_movsi_immediate (operands);
    DONE;
  }
  [(set_attr "type" "fast,fast,two")])

;; movsi - symbol_ref to register sets
(define_insn "*set_si_symbol_got"
  [(set (match_operand:SI                                  0 "metag_reg_nofloat_op" "=da")
        (mem:SI (plus:SI (match_operand:SI                 1 "metag_pic_reg"         "a")
                         (const (unspec [(match_operand:SI 2 "symbolic_operand"      "")] UNSPEC_GOT)))))]
  "METAG_FLAG_PIC&& operands[1] == pic_offset_table_rtx"
  "GETD\\t%0, [%1+#(%c2@GOT)]\\t%@ (*set si r-picsym OK)"
  [(set_attr "type" "load")])

(define_insn "*add_si_HI_symbol_gotoff"
  [(set (match_operand:SI                                   0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                          1 "metag_reg_nofloat_op"  "0")
                 (high:SI (const (unspec [(match_operand:SI 2 "symbolic_operand"      "")] UNSPEC_GOTOFF)))))]
  "METAG_FLAG_PIC"
  "ADDT\\t%0, %1, #HI(%c2@GOTOFF)"
  [(set_attr "type" "fast")])

(define_insn "*mov_si_HI_symbol_gotoff"
  [(set (match_operand:SI                          0 "metag_reg_nofloat_op" "=da")
        (high:SI (const (unspec [(match_operand:SI 1 "symbolic_operand"      "")] UNSPEC_GOTOFF))))]
  "METAG_FLAG_PIC"
  "MOVT\\t%0, #HI(%c1@GOTOFF)"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_symbol_gotoff"
  [(set (match_operand:SI                            0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI                 1 "metag_reg_nofloat_op"  "0")
                   (const (unspec [(match_operand:SI 2 "symbolic_operand"      "")] UNSPEC_GOTOFF))))]
  "METAG_FLAG_PIC"
  "ADD\\t%0, %1, #LO(%c2@GOTOFF)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_symbol_gotoff"
  [(set (match_operand:SI                          0 "metag_reg_nofloat_op" "=&da")
        (plus:SI (match_operand:SI                 1 "metag_pic_reg"          "a")
                 (const (unspec [(match_operand:SI 2 "symbolic_operand"       "")] UNSPEC_GOTOFF))))]
  "METAG_FLAG_PIC&& operands[1] == pic_offset_table_rtx"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (high:SI (const (unspec [(match_dup 2)] UNSPEC_GOTOFF)))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const (unspec [(match_dup 2)] UNSPEC_GOTOFF))))]
  ""
  [(set_attr "type" "three")])

(define_insn_and_split "*set_si_symbol_gotoff_val"
  [(set (match_operand:SI                 0 "metag_reg_nofloat_op" "=&da")
        (const (unspec [(match_operand:SI 1 "symbolic_operand"       "")] UNSPEC_GOTOFF)))]
  "METAG_FLAG_PIC"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (high:SI (const (unspec [(match_dup 1)] UNSPEC_GOTOFF))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const (unspec [(match_dup 1)] UNSPEC_GOTOFF))))]
  ""
  [(set_attr "type" "two")])

(define_insn "*add_si_HI_symbol_large"
  [(set (match_operand:SI                0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI       1 "metag_reg_nofloat_op"  "0")
                 (high:SI (match_operand 2 "metag_symlarge_op"     ""))))]
  "OG_ENABLED"
  "ADDT\\t%0, %1, #HI(OG(%c2))"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_symbol_large"
  [(set (match_operand:SI            0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0")
                   (match_operand    2 "metag_symlarge_op"     "")))]
  "OG_ENABLED"
  "ADD\\t%0, %1, #LO(OG(%c2))"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_symbol_large"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=&l,!&da")
        (match_operand:SI 1 "metag_symlarge_op"    ""))]
  "OG_ENABLED"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 2))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (high:SI (match_dup 1))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))]
  {
    operands[2] = gen_rtx_REG (SImode, A1GbP_REG);
  }
  [(set_attr "type" "three")])

(define_insn "*set_si_HI_symbol_global"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op"  "=da")
        (high:SI (match_operand:SI 1 "metag_symglobal_op"    "")))]
  "!METAG_FLAG_PIC"
  "MOVT\\t%0,#HI(%c1)"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_symbol_global"
  [(set (match_operand:SI            0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0")
                   (match_operand:SI 2 "metag_symglobal_op"    "" )))]
  "!METAG_FLAG_PIC"
  "ADD\\t%0, %1, #LO(%c2)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_symbol_global"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:SI 1 "metag_symglobal_op"    ""))]
  "!METAG_FLAG_PIC"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (high:SI (match_dup 1)))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))]
  ""
  [(set_attr "type" "two")])

(define_insn "*set_si_symbol_small"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:SI 1 "metag_symsmall_op"     ""))]
  "OG_ENABLED"
  "GETD\\t%0, [A1GbP+#OGA(%c1)]\\t%@ (*set si r-ssym OK)"
  [(set_attr "type" "load")])

(define_insn "*set_si_symbol_off_small"
  [(set (match_operand:SI                    0 "metag_reg_nofloat_op" "=da")
        (const:SI (plus:SI (match_operand:SI 1 "metag_symsmall_op"     "")
                           (match_operand:SI 2 "const_int_operand"     ""))))]
  "OG_ENABLED"
  "GETD\\t%0, [A1GbP+#OGA(%c1+%c2)]\\t%@ (*set si r-ssym OK)"
  [(set_attr "type" "load")])

(define_insn "*add_si_HI_symbol_off_pic"
  [(set (match_operand:SI                                       0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                              1 "metag_reg_nofloat_op"  "0")
                 (high:SI (const
                            (plus:SI (unspec [(match_operand:SI 2 "symbolic_operand"      "")] UNSPEC_GOTOFF)
                                     (match_operand:SI          3 "const_int_operand"     ""))))))]
  "METAG_FLAG_PIC"
  "ADDT\\t%0, %1, #HI(%c2@GOTOFF+%c3)"
  [(set_attr "type" "fast")])

(define_insn "*mov_si_HI_symbol_off_pic"
  [(set (match_operand:SI                              0 "metag_reg_nofloat_op" "=da")
        (high:SI (const
                   (plus:SI (unspec [(match_operand:SI 1 "symbolic_operand"      "")] UNSPEC_GOTOFF)
                            (match_operand:SI          2 "const_int_operand"     "")))))]
  "METAG_FLAG_PIC"
  "MOVT\\t%0, #HI(%c1@GOTOFF+%c2)"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_symbol_off_pic"
  [(set (match_operand:SI                                0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI                     1 "metag_reg_nofloat_op"  "0")
                   (const
                     (plus:SI (unspec [(match_operand:SI 2 "symbolic_operand"      "")] UNSPEC_GOTOFF)
                              (match_operand:SI          3 "const_int_operand"     "")))))]
  "METAG_FLAG_PIC"
  "ADD\\t%0, %1, #LO(%c2@GOTOFF+%c3)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_symbol_off_pic"
  [(set (match_operand:SI                              0 "metag_reg_nofloat_op" "=&da")
        (plus:SI (match_operand:SI                     1 "metag_pic_reg"          "a")
                 (const
                   (plus:SI (unspec [(match_operand:SI 2 "symbolic_operand"       "")] UNSPEC_GOTOFF)
                            (match_operand:SI          3 "const_int_operand"      "")))))]
  "METAG_FLAG_PIC&& operands[1] == pic_offset_table_rtx"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (high:SI (const
                            (plus:SI (unspec [(match_dup 2)] UNSPEC_GOTOFF)
                                     (match_dup 3))))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const
                     (plus:SI (unspec [(match_dup 2)] UNSPEC_GOTOFF)
                              (match_dup 3)))))]
  ""
  [(set_attr "type" "three")])

(define_insn_and_split "*set_si_symbol_off_pic_val"
  [(set (match_operand:SI                     0 "metag_reg_nofloat_op" "=&da")
        (const
          (plus:SI (unspec [(match_operand:SI 1 "symbolic_operand"       "")] UNSPEC_GOTOFF)
                   (match_operand:SI          2 "const_int_operand"      ""))))]
  "METAG_FLAG_PIC"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (high:SI (const
                   (plus:SI (unspec [(match_dup 1)] UNSPEC_GOTOFF)
                            (match_dup 2)))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const
                     (plus:SI (unspec [(match_dup 1)] UNSPEC_GOTOFF)
                              (match_dup 2)))))]
  ""
  [(set_attr "type" "two")])

(define_insn "*add_si_HI_symbol_off_large"
  [(set (match_operand:SI                                      0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                             1 "metag_reg_nofloat_op"  "0")
                 (high:SI (const:SI (plus:SI (match_operand:SI 2 "metag_symlarge_op"     "")
                                             (match_operand:SI 3 "const_int_operand"     ""))))))]
  "OG_ENABLED"
  "ADDT\\t%0, %1, #HI(OG(%c2+%c3))"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_symbol_off_large"
  [(set (match_operand:SI                               0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI                    1 "metag_reg_nofloat_op"  "0")
                   (const:SI (plus:SI (match_operand:SI 2 "metag_symlarge_op"     "")
                                      (match_operand:SI 3 "const_int_operand"     "")))))]
  "OG_ENABLED"
  "ADD\\t%0, %1, #LO(OG(%c2+%c3))"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_symbol_off_large"
  [(set (match_operand:SI                    0 "metag_reg_nofloat_op" "=l,!da")
        (const:SI (plus:SI (match_operand:SI 1 "metag_symlarge_op"    "")
                           (match_operand:SI 2 "const_int_operand"    ""))))]
  "OG_ENABLED"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 3))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (high:SI (match_dup 4))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 4)))]
  {
    operands[3] = gen_rtx_REG (SImode, A1GbP_REG);
    operands[4] = gen_rtx_CONST (SImode, 
                                 gen_rtx_PLUS (SImode,
                                               operands[1],
                                               operands[2]));
  }
  [(set_attr "type" "three,four")])

(define_insn "*set_si_HI_symbol_off_global"
  [(set (match_operand:SI                             0 "metag_reg_nofloat_op" "=da")
        (high:SI (const:SI (plus:SI (match_operand:SI 1 "metag_symglobal_op"    "")
                                    (match_operand:SI 2 "const_int_operand"     "")))))]
  "!METAG_FLAG_PIC"
  "MOVT\\t%0,#HI(%c1+%c2)"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_symbol_off_global"
  [(set (match_operand:SI                               0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI                    1 "metag_reg_nofloat_op"  "0")
                   (const:SI (plus:SI (match_operand:SI 2 "metag_symglobal_op"    "" )
                                      (match_operand:SI 3 "const_int_operand"     "" )))))]
  "!METAG_FLAG_PIC"
  "ADD\\t%0, %1, #LO(%c2+%c3)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_symbol_off_global"
  [(set (match_operand:SI                    0 "metag_reg_nofloat_op" "=da")
        (const:SI (plus:SI (match_operand:SI 1 "metag_symglobal_op"    "")
                           (match_operand:SI 2 "const_int_operand"     ""))))]
  "!METAG_FLAG_PIC"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (high:SI (match_dup 3)))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 3)))]
  {
    operands[3] = gen_rtx_CONST (SImode,
                                 gen_rtx_PLUS (SImode,
                                               operands[1],
                                               operands[2]));
  }
  [(set_attr "type" "two")])

;; movsi - code address to register sets
(define_insn "*set_si_HI_label"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=da")
        (high:SI (match_operand:SI 1 "code_address"          "")))]
  "!METAG_FLAG_PIC"
  "MOVT\\t%0,#HI(%c1)"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_label"
  [(set (match_operand:SI            0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0")
                   (match_operand:SI 2 "code_address"          "" )))]
  "!METAG_FLAG_PIC"
  "ADD\\t%0, %1, #LO(%c2)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_label"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:SI 1 "code_address"          ""))]
  "!METAG_FLAG_PIC"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (high:SI (match_dup 1)))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 1)))]
  ""
  [(set_attr "type" "two")])

(define_insn "*set_si_HI_label_off"
  [(set (match_operand:SI                             0 "metag_reg_nofloat_op" "=da")
        (high:SI (const:SI (plus:SI (match_operand:SI 1 "code_address"          "")
                                    (match_operand:SI 2 "const_int_operand"     "")))))]
  "!METAG_FLAG_PIC"
  "MOVT\\t%0,#HI(%c1+%c2)"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_label_off"
  [(set (match_operand:SI                               0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI                    1 "metag_reg_nofloat_op"  "0")
                   (const:SI (plus:SI (match_operand:SI 2 "code_address"          "" )
                                      (match_operand:SI 3 "const_int_operand"     "" )))))]
  "!METAG_FLAG_PIC"
  "ADD\\t%0, %1, #LO(%c2+%c3)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_label_off"
  [(set (match_operand:SI                    0 "metag_reg_nofloat_op" "=da")
        (const:SI (plus:SI (match_operand:SI 1 "code_address"          "")
                           (match_operand:SI 2 "const_int_operand"     ""))))]
  "!METAG_FLAG_PIC"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (high:SI (match_dup 3)))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (match_dup 3)))]
  {
    operands[3] = gen_rtx_CONST (SImode, 
                                 gen_rtx_PLUS (SImode,
                                               operands[1],
                                               operands[2]));
  }
  [(set_attr "type" "two")])

(define_insn "*add_si_HI_label_pic"
  [(set (match_operand:SI                                              0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                                     1 "metag_reg_nofloat_op"  "0")
                 (high:SI (const (unspec [(label_ref:SI (match_operand 2 ""                      ""))] UNSPEC_GOTOFF)))))]
  "METAG_FLAG_PIC"
  "ADDT\\t%0, %0, #HI(%c2@GOTOFF)"
  [(set_attr "type" "fast")])

(define_insn "*mov_si_HI_label_pic"
  [(set (match_operand:SI                                     0 "metag_reg_nofloat_op" "=da")
        (high:SI (const (unspec [(label_ref:SI (match_operand 1 ""                      ""))] UNSPEC_GOTOFF))))]
  "METAG_FLAG_PIC"
  "MOVT\\t%0, #HI(%c1@GOTOFF)"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_label_pic"
  [(set (match_operand:SI                                       0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI                            1 "metag_reg_nofloat_op"  "0")
                   (const (unspec [(label_ref:SI (match_operand 2 ""                      ""))] UNSPEC_GOTOFF))))]
  "METAG_FLAG_PIC"
  "ADD\\t%0, %0, #LO(%c2@GOTOFF)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_label_pic"
  [(set (match_operand:SI                                     0 "metag_reg_nofloat_op" "=&da")
        (plus:SI (match_operand:SI                            1 "metag_pic_reg"          "a")
                 (const (unspec [(label_ref:SI (match_operand 2 ""                       ""))] UNSPEC_GOTOFF))))]
  "METAG_FLAG_PIC&& operands[1] == pic_offset_table_rtx"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (high:SI (const (unspec [(label_ref:SI (match_dup 2))] UNSPEC_GOTOFF)))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const (unspec [(label_ref:SI (match_dup 2))] UNSPEC_GOTOFF))))]
  ""
  [(set_attr "type" "three")])

(define_insn_and_split "*set_si_label_pic_val"
  [(set (match_operand:SI                            0 "metag_reg_nofloat_op" "=&da")
        (const (unspec [(label_ref:SI (match_operand 1 ""                       ""))] UNSPEC_GOTOFF)))]
  "METAG_FLAG_PIC"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (high:SI (const (unspec [(label_ref:SI (match_dup 1))] UNSPEC_GOTOFF))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const (unspec [(label_ref:SI (match_dup 1))] UNSPEC_GOTOFF))))]
  ""
  [(set_attr "type" "two")])

(define_insn "*add_si_HI_label_off_pic"
  [(set (match_operand:SI                                                  0 "metag_reg_nofloat_op" "=da")
        (plus:SI (match_operand:SI                                         1 "metag_reg_nofloat_op"  "0")
                 (high:SI (const
                            (plus:SI (unspec [(label_ref:SI (match_operand 2 ""                      ""))] UNSPEC_GOTOFF)
                                     (match_operand:SI                     3 "const_int_operand"     ""))))))]
  "METAG_FLAG_PIC"
  "ADDT\\t%0, %1, #HI(%c2@GOTOFF+%c3)"
  [(set_attr "type" "fast")])

(define_insn "*mov_si_HI_label_off_pic"
  [(set (match_operand:SI                                         0 "metag_reg_nofloat_op" "=da")
        (high:SI (const
                   (plus:SI (unspec [(label_ref:SI (match_operand 1 ""                      ""))] UNSPEC_GOTOFF)
                            (match_operand:SI                     2 "const_int_operand"     "")))))]
  "METAG_FLAG_PIC"
  "MOVT\\t%0, #HI(%c1@GOTOFF+%c2)"
  [(set_attr "type" "fast")])

(define_insn "*add_si_LO_label_off_pic"
  [(set (match_operand:SI                                           0 "metag_reg_nofloat_op" "=da")
        (lo_sum:SI (match_operand:SI                                1 "metag_reg_nofloat_op"  "0")
                   (const
                     (plus:SI (unspec [(label_ref:SI (match_operand 2 ""                      ""))] UNSPEC_GOTOFF)
                              (match_operand:SI                     3 "const_int_operand"     "")))))]
  "METAG_FLAG_PIC"
  "ADD\\t%0, %0, #LO(%c2@GOTOFF+%c3)"
  [(set_attr "type" "fast")])

(define_insn_and_split "*set_si_label_off_pic"
  [(set (match_operand:SI                                         0 "metag_reg_nofloat_op" "=&da")
        (plus:SI (match_operand:SI                                1 "metag_pic_reg"          "a")
                 (const
                   (plus:SI (unspec [(label_ref:SI (match_operand 2 ""                       ""))] UNSPEC_GOTOFF)
                            (match_operand:SI                     3 "const_int_operand"      "")))))]
  "METAG_FLAG_PIC"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (match_dup 1))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (high:SI (const
                            (plus:SI (unspec [(label_ref:SI (match_dup 2))] UNSPEC_GOTOFF)
                                     (match_dup 3))))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const
                     (plus:SI (unspec [(label_ref:SI (match_dup 2))] UNSPEC_GOTOFF)
                              (match_dup 3)))))]
  ""
  [(set_attr "type" "three")])

(define_insn_and_split "*set_si_label_off_pic_val"
  [(set (match_operand:SI                                0 "metag_reg_nofloat_op" "=&da")
        (const
          (plus:SI (unspec [(label_ref:SI (match_operand 1 ""                       ""))] UNSPEC_GOTOFF)
                   (match_operand:SI                     2 "const_int_operand"      ""))))]
  "METAG_FLAG_PIC"
  "#"
  "&& SPLIT_HI_LO_SUM_EARLY"
  [(set (match_dup 0)
        (high:SI (const
                   (plus:SI (unspec [(label_ref:SI (match_dup 1))] UNSPEC_GOTOFF)
                            (match_dup 2)))))
   (set (match_dup 0)
        (lo_sum:SI (match_dup 0)
                   (const
                     (plus:SI (unspec [(label_ref:SI (match_dup 1))] UNSPEC_GOTOFF)
                              (match_dup 2)))))]
  ""
  [(set_attr "type" "two")])

;; -----------------------------------------------------------------------------
;; | Matching SI store post/pre_inc/dec/modify and emitting ASM                | 
;; | ** These rules MUST come before the put_si_1_1 rule **                    |
;; -----------------------------------------------------------------------------

(define_insn "*sto_si_post_inc"
  [(set (mem:SI (post_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:SI                      1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0++], %1\\t%@ (*store SI post_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])
  
(define_insn "*sto_si_post_dec"
  [(set (mem:SI (post_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:SI                      1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0--], %1\\t%@ (*store SI post_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_si_pre_inc"
  [(set (mem:SI (pre_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:SI                     1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[++%0], %1\\t%@ (*store SI pre_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])
  
(define_insn "*sto_si_pre_dec"
  [(set (mem:SI (pre_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:SI                     1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[--%0], %1\\t%@ (*store SI pre_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_si_post_modify_disp"
  [(set (mem:SI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_si"      "O4,O4,O4,O4"))))
        (match_operand:SI                     2 "metag_register_op"     "t, u, y, z"))]
  "!TARGET_METAC_1_1"
  "SETD\\t[%0+%1++], %2\\t%@ (*store SI post_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_si_post_modify_disp_1_1"
  [(set (mem:SI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_si"      "O4"))))
        (match_operand:SI                     2 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0+%1++], %2\\t%@ (*store SI post_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_si_post_modify_reg"
  [(set (mem:SI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:SI                     2 "metag_register_op"     "ct,cu,cy,cz"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0+%1++], %2\\t%@ (*store SI post_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_si_pre_modify_disp"
  [(set (mem:SI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_si"      "O4,O4,O4,O4"))))
        (match_operand:SI                     2 "metag_reg_nofloat_op"  "t, u, y, z"))]
  "!TARGET_METAC_1_1"
  "SETD\\t[%0++%1], %2\\t%@ (*store SI pre_modify_disp OK)"
  [(set_attr "type" "fast,fast,fast,fast")])

(define_insn "*sto_si_pre_modify_disp_1_1"
  [(set (mem:SI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_si"      "O4"))))
        (match_operand:SI                     2 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0++%1], %2\\t%@ (*store SI pre_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_si_pre_modify_reg_1_1"
  [(set (mem:SI (pre_modify:SI 
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:SI                     2 "metag_register_op"     "ct,cu,cy,cz"))]
  ""
  {
    static const char fmt[] = "F\\tSETD\\t[%0++%1], %2\\t%@ (*store SI pre_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Non-side effecting base+offset store SI and catchall store SI cases       |
;; -----------------------------------------------------------------------------

;; movsi - base+index register to memory (stors's)
(define_insn "*sto_si_mar"
  [(set (mem:SI (plus:SI (match_operand:SI 0 "metag_regnofrm_op" "%e, f, h, l")
                         (match_operand:SI 1 "metag_regnofrm_op" " e, f, h, l")))
        (match_operand:SI                  2 "metag_register_op" "ct,cu,cy,cz"))]
  ""
  {
    static const char fmt[] = "F\\tSETD\\t[%0+%1], %2\\t%@ (*sto si mar OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type"   "fast")
   (set_attr "length" "4,4,4,4")])

;; movsi - register to memory (stores) some are fast, rest match spillsi below
(define_insn "*sto_si_reg_indirect"
  [(set (mem:SI (match_operand:SI 0 "metag_reg_nofloat_op" "e,f,h,l,!*da"))
        (match_operand:SI         1 "metag_register_op"    "t,u,y,z, *da"))]
  "TARGET_COND_EXEC_OPTIMIZE && !TARGET_METAC_1_1"
  "@
   SETD%?\\t[%0], %1\\t%@ (*sto si [e]t OK)
   SETD%?\\t[%0], %1\\t%@ (*sto si [f]u OK)
   SETD%?\\t[%0], %1\\t%@ (*sto si [h]y OK)
   SETD%?\\t[%0], %1\\t%@ (*sto si [l]z OK)
   #"
  [(set_attr "type" "fast,fast,fast,fast,invalid")
   (set_attr "cond" "yes,yes,yes,yes,no")])

(define_insn "*sto_si"
  [(set (match_operand:SI 0 "memory_operand"    "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:SI 1 "metag_register_op"  "r, t, u, y, z, !*da"))]
  "!TARGET_METAC_1_1 && !reload_completed"
  "@
   SETD\\t%0, %1\\t%@ (*sto si [r]r OK)
   SETD\\t%0, %1\\t%@ (*sto si [e]t OK)
   SETD\\t%0, %1\\t%@ (*sto si [f]u OK)
   SETD\\t%0, %1\\t%@ (*sto si [h]y OK)
   SETD\\t%0, %1\\t%@ (*sto si [l]z OK)
   SETD\\t%0, %1\\t%@ (*sto si [m]r OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_si_postreload"
  [(set (match_operand:SI 0 "memory_operand"    "=Tr,Te,Tf,Th,Tl")
        (match_operand:SI 1 "metag_register_op"  "r, t, u, y, z"))]
  "!TARGET_METAC_1_1 && reload_completed"
 "@
  SETD\\t%0, %1\\t%@ (*sto si [r]r OK)
  SETD\\t%0, %1\\t%@ (*sto si [e]r OK)
  SETD\\t%0, %1\\t%@ (*sto si [f]r OK)
  SETD\\t%0, %1\\t%@ (*sto si [h]r OK)
  SETD\\t%0, %1\\t%@ (*sto si [l]r OK)"
  [(set_attr "type" "fast")])

;; movsi - all the register to register|memory moves
(define_insn "*sto_si_reg_indirect_1_1"
  [(set (mem:SI (match_operand:SI 0 "metag_reg_nofloat_op" "da"))
        (match_operand:SI         1 "metag_register_op"    "cr"))]
  "TARGET_COND_EXEC_OPTIMIZE && TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD%?\\t[%0], %1\\t%@ (*sto si [r]r OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_si_1_1"
  [(set (match_operand:SI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:SI 1 "metag_reg_nofloat_op"  "r, t, u, y, z, !*da"))]
  "TARGET_METAC_1_1 && !reload_completed"
  "@
   SETD\\t%0, %1\\t%@ (*sto si [r]r OK)
   SETD\\t%0, %1\\t%@ (*sto si [e]t OK)
   SETD\\t%0, %1\\t%@ (*sto si [f]u OK)
   SETD\\t%0, %1\\t%@ (*sto si [h]y OK)
   SETD\\t%0, %1\\t%@ (*sto si [l]z OK)
   SETD\\t%0, %1\\t%@ (*sto si [m]r OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_si_1_1_postreload"
  [(set (match_operand:SI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl")
        (match_operand:SI 1 "metag_reg_nofloat_op"  "r, t, u, y, z"))]
  "TARGET_METAC_1_1 && reload_completed"
  "@
   SETD\\t%0, %1\\t%@ (*sto si [r]r OK)
   SETD\\t%0, %1\\t%@ (*sto si [e]t OK)
   SETD\\t%0, %1\\t%@ (*sto si [f]u OK)
   SETD\\t%0, %1\\t%@ (*sto si [h]y OK)
   SETD\\t%0, %1\\t%@ (*sto si [l]z OK)"
  [(set_attr "type" "fast")])

;; spillsi - register to memory (stores) from source/dest in same bank
(define_split
  [(set (match_operand:SI 0 "memory_operand"    "")
        (match_operand:SI 1 "metag_register_op" ""))]
  "!TARGET_METAC_1_1
   && reload_completed
   && metag_slow_store (operands[0], operands[1])"
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 0)
        (match_dup 2))]
  {
    operands[2] = metag_gen_safe_temp (SImode, operands[1]);
  }
)

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Matching SI load post/pre_inc/dec/modify and emitting ASM                 | 
;; -----------------------------------------------------------------------------

(define_insn "*lod_si_post_inc"
  [(set (match_operand:SI                      0 "metag_register_op"    "=cr")
        (mem:SI (post_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1++]\\t%@ (*load SI post_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_si_post_dec"
  [(set (match_operand:SI                      0 "metag_register_op"    "=cr")
        (mem:SI (post_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1--]\\t%@ (*load SI post_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_si_pre_inc"
  [(set (match_operand:SI                     0 "metag_register_op"    "=cr")
        (mem:SI (pre_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETD\\t%0, [++%1]\\t%@ (*load SI pre_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_si_pre_dec"
  [(set (match_operand:SI                     0 "metag_register_op"    "=cr")
        (mem:SI (pre_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETD\\t%0, [--%1]\\t%@ (*load SI pre_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_si_post_modify_disp"
  [(set (match_operand:SI                     0 "metag_register_op"    "=cr")
        (mem:SI (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_si"      "O4")))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1+%2++]\\t%@ (*load SI post_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_si_post_modify_reg"
  [(set (match_operand:SI                     0 "metag_register_op"    "=cr,cr,cr,cr")
        (mem:SI (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1+%2++]\\t%@ (*load SI post_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_si_pre_modify_disp"
  [(set (match_operand:SI                     0 "metag_register_op"    "=cr")
        (mem:SI (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_si"      "O4")))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1++%2]\\t%@ (*load SI pre_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_si_pre_modify_reg"
  [(set (match_operand:SI                     0 "metag_register_op"    "=cr,cr,cr,cr")
        (mem:SI (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 1)                                                       
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1++%2]\\t%@ (*load SI pre_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Non-side effecting base+offset load SI and catchall load SI               |
;; -----------------------------------------------------------------------------

(define_insn "*lod_si"
  [(set (match_operand:SI         0 "metag_register_op"    "=cr")
        (mem:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1]";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; movsi - base+index memory to register (loads)
(define_insn "*lod_si_rma"
  [(set (match_operand:SI                  0 "metag_register_op" "=cr,cr,cr,cr")
        (mem:SI (plus:SI (match_operand:SI 1 "metag_regnofrm_op"  "e, f, h, l")
                         (match_operand:SI 2 "metag_regnofrm_op"  "e, f, h, l"))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1+%2]\\t%@ (*lod si rma OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; Removed FPC alternative owing to lack of 12bit offset support
;; base+12bit load to FX occurs during conversion of virtual_stack_args
(define_insn "*lod_si_mem"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:SI 1 "memory_operand"        "m"))]
  ""
  "GETD\\t%0, %1\\t%@ (*lod si rm OK)"
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; movhi is made up of many parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "movhi"
  [(set (match_operand:HI 0 "nonimmediate_operand" "")
        (match_operand:HI 1 "general_operand"      ""))]
  ""
  {
    if (MEM_P (operands[0]))
      {
        /* All except mem = const or mem = mem can be done quickly */
        operands[1] = force_reg (HImode, operands[1]);
      }
  }
)

;; NOTE "move hi" needs to handle r <- r along with r <- m and m <- r variants together.
;; Otherwise one can end up with a "impossible reload" error if an argument to inline asm
;; fails to be assigned a register and thus needs to be fixed up by reload. This isn't
;; well documented and the underlying issue can go undetected.

;; movhi - all the register to register moves

;; this variant mov_hi is used if generated by reload and needs to handle memory
(define_insn "*mov_hi_reload"
  [(set (match_operand:HI 0 "metag_move_dst_op" "=e,f,h,l,r,!*Wx,  r,  cx,d, cx,da, cr,!*m")
        (match_operand:HI 1 "metag_move_src_op"  "e,f,h,l,r,  r,  !*Wx,cx,cx,d, da,!*m, cr"))]
  "reload_in_progress || reload_completed"
  {
    switch (which_alternative)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
        if (METAG_FPC_REG_P (REGNO (operands[0])) || METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\\tMOV%?\\t%0, %1";
        else
          return "MOV%?\\t%0, %1";
      case 11:
        if (METAG_FPC_REG_P (REGNO (operands[0])))
          return "F\\tGETW\\t%0, %1";
        else
          return "GETW\\t%0, %1";
      case 12:
        if (METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\\tSETW\\t%0, %1";
        else
          return "SETW\\t%0, %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,fast,slow,slow,slow,load,fast")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,no,no")
   (set_attr "memaccess" "none,none,none,none,none,none,none,none,none,none,none,load,store")
   (set_attr "predicable" "yes")])

;; this variant mov_hi is used if NOT generated by reload and doesn't handle memory
(define_insn "*mov_hi"
  [(set (match_operand:HI 0 "metag_register_op" "=e,f,h,l,cx,d, cx,da")
        (match_operand:HI 1 "metag_register_op"  "e,f,h,l,cx,cx,d, da"))]
  ""
  "MOV%?\\t%0, %1\\t\\t%@ (*mov hi rr OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; movhi - all the immediate to register sets
(define_insn "*set_hi"
  [(set (match_operand:HI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:HI 1 "metag_int_operand"     "KIP"))]
  ""
  "MOV\\t%0, %1\\t\\t%@ (*set hi rI OK)"
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------
 
;; -----------------------------------------------------------------------------
;; | Matching HI store post/pre_inc/dec/modify and emitting ASM                | 
;; -----------------------------------------------------------------------------

(define_insn "*sto_hi_post_inc"
  [(set (mem:HI (post_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:HI                      1 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETW\\t[%0++], %1\\t%@ (*store HI post_inc OK)"
  [(set_attr "type" "fast")])
  
(define_insn "*sto_hi_post_dec"
  [(set (mem:HI (post_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:HI                      1 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETW\\t[%0--], %1\\t%@ (*store HI post_dec OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_hi_pre_inc"
  [(set (mem:HI (pre_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:HI                     1 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETW\\t[++%0], %1\\t%@ (*store HI pre_inc OK)"
  [(set_attr "type" "fast")])
  
(define_insn "*sto_hi_pre_dec"
  [(set (mem:HI (pre_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:HI                     1 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETW\\t[--%0], %1\\t%@ (*store HI pre_dec OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_hi_post_modify_disp"
  [(set (mem:HI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_hi"      "O2,O2,O2,O2"))))
        (match_operand:HI                     2 "metag_reg_nofloat_op"  "t, u, y, z"))]
  "!TARGET_METAC_1_1"
  "SETW\\t[%0+%1++], %2\\t%@ (*store HI post_modify_disp OK)"
  [(set_attr "type" "fast,fast,fast,fast")])

(define_insn "*sto_hi_post_modify_disp_1_1"
  [(set (mem:HI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_hi"      "O2"))))
        (match_operand:HI                     2 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETW\\t[%0+%1++], %2\\t%@ (*store HI post_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_hi_post_modify_reg"
  [(set (mem:HI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e,f,h,l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e,f,h,l"))))
        (match_operand:HI                     2 "metag_reg_nofloat_op"  "t,u,y,z"))]
  ""
  "SETW\\t[%0+%1++], %2\\t%@ (*store HI post_modify_reg OK)"
  [(set_attr "type" "fast,fast,fast,fast")])

(define_insn "*sto_hi_pre_modify_disp"
  [(set (mem:HI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_hi"      "O2,O2,O2,O2"))))
        (match_operand:HI                     2 "metag_reg_nofloat_op"  "t, u, y, z"))]
  "!TARGET_METAC_1_1"
  "SETW\\t[%0++%1], %2\\t%@ (*store HI pre_modify_disp OK)"
  [(set_attr "type" "fast,fast,fast,fast")])

(define_insn "*sto_hi_pre_modify_disp_1_1"
  [(set (mem:HI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_hi"      "O2"))))
        (match_operand:HI                     2 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETW\\t[%0++%1], %2\\t%@ (*store HI pre_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_hi_pre_modify_reg"
  [(set (mem:HI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e,f,h,l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e,f,h,l"))))
        (match_operand:HI                     2 "metag_reg_nofloat_op"  "t,u,y,z"))]
  ""
  "SETW\\t[%0++%1], %2\\t%@ (*store HI pre_modify_reg OK)"
  [(set_attr "type" "fast,fast,fast,fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Non-side effecting base+offset store HI and catchall store HI cases       |
;; -----------------------------------------------------------------------------

;; movhi - base+index register to memory (stors's)
(define_insn "*sto_hi_mar"
  [(set (mem:HI (plus:SI (match_operand:SI 0 "metag_regnofrm_op"    "%e,f,h,l")
                         (match_operand:SI 1 "metag_regnofrm_op"    " e,f,h,l")))
        (match_operand:HI                  2 "metag_reg_nofloat_op" " t,u,y,z"))]
  ""
  "SETW\\t[%0+%1], %2\\t%@ (*sto hi mar OK)"
  [(set_attr "type"   "fast")
   (set_attr "length" "4,4,4,4")])

;; movhi - register to memory (stores) some are fast, rest match spillhi below
(define_insn "*sto_hi_reg_indirect"
  [(set (mem:HI (match_operand:SI 0 "metag_reg_nofloat_op" "e,f,h,l,!*da"))
        (match_operand:HI         1 "metag_reg_nofloat_op" "t,u,y,z, *da"))]
  "TARGET_COND_EXEC_OPTIMIZE && !TARGET_METAC_1_1"
  "@
   SETW%?\\t[%0], %1\\t%@ (*sto hi [e]t OK)
   SETW%?\\t[%0], %1\\t%@ (*sto hi [f]u OK)
   SETW%?\\t[%0], %1\\t%@ (*sto hi [h]y OK)
   SETW%?\\t[%0], %1\\t%@ (*sto hi [l]z OK)
   #"
  [(set_attr "type" "fast,fast,fast,fast,invalid")
   (set_attr "cond" "yes,yes,yes,yes,no")])

(define_insn "*sto_hi"
  [(set (match_operand:HI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:HI 1 "metag_reg_nofloat_op"  "r, t, u, y, z, !*da"))]
  "!TARGET_METAC_1_1 && !reload_completed"
  "@
   SETW\\t%0, %1\\t%@ (*sto hi [r]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [e]t OK)
   SETW\\t%0, %1\\t%@ (*sto hi [f]u OK)
   SETW\\t%0, %1\\t%@ (*sto hi [h]y OK)
   SETW\\t%0, %1\\t%@ (*sto hi [l]z OK)
   SETW\\t%0, %1\\t%@ (*sto hi [m]r OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_hi_postreload"
  [(set (match_operand:HI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl")
        (match_operand:HI 1 "metag_reg_nofloat_op"  "r, t, u, y, z"))]
  "!TARGET_METAC_1_1 && reload_completed"
  "@
   SETW\\t%0, %1\\t%@ (*sto hi [r]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [e]t OK)
   SETW\\t%0, %1\\t%@ (*sto hi [f]u OK)
   SETW\\t%0, %1\\t%@ (*sto hi [h]y OK)
   SETW\\t%0, %1\\t%@ (*sto hi [l]z OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_hi_reg_indirect_1_1"
  [(set (mem:HI (match_operand:SI 0 "metag_reg_nofloat_op" "da"))
        (match_operand:HI         1 "metag_reg_nofloat_op" "da"))]
  "TARGET_COND_EXEC_OPTIMIZE && TARGET_METAC_1_1"
  "SETW%?\\t[%0], %1\\t%@ (*sto hi [r]r OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_hi_1_1_off12"
  [(set (mem:HI (plus:SI (match_operand:SI 0 "metag_reg12bit_op"    "da,Yr")
                         (match_operand:SI 1 "metag_offset12_hi"    "O2,Z2")))
        (match_operand:HI                  2 "metag_reg_nofloat_op" "da,da"))]
  "TARGET_METAC_1_1"
  "SETW\\t[%0+%1], %2"
  [(set_attr "type" "fast")])

(define_insn "*sto_hi_1_1_off6"
  [(set (mem:HI (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "da")
                         (match_operand:SI 1 "metag_offset6_hi"     "O2")))
        (match_operand:HI                  2 "metag_reg_nofloat_op" "da"))]
  "TARGET_METAC_1_1"
  "SETW\\t[%0+%1], %2"
  [(set_attr "type" "fast")])

(define_insn "*sto_hi_1_1"
  [(set (match_operand:HI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:HI 1 "metag_reg_nofloat_op"  "r, t, u, y, z, !*da"))]
  "TARGET_METAC_1_1 && !reload_completed"
  "@
   SETW\\t%0, %1\\t%@ (*sto hi [r]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [e]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [f]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [h]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [l]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [m]r OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_hi_1_1_postreload"
  [(set (match_operand:HI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl")
        (match_operand:HI 1 "metag_reg_nofloat_op"  "r, t, u, y, z"))]
  "TARGET_METAC_1_1 && reload_completed"
  "@
   SETW\\t%0, %1\\t%@ (*sto hi [r]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [e]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [f]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [h]r OK)
   SETW\\t%0, %1\\t%@ (*sto hi [l]r OK)"
  [(set_attr "type" "fast")])

;; spillhi - register to memory (stores) from source/dest in same bank
(define_split
  [(set (match_operand:HI 0 "memory_operand"    "")
        (match_operand:HI 1 "metag_register_op" ""))]
  "!TARGET_METAC_1_1
   && reload_completed
   && metag_slow_store (operands[0], operands[1])"
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 0)
        (match_dup 2))]
  {
    operands[2] = metag_gen_safe_temp (HImode, operands[1]);
  }
)

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Matching HI load post/pre_inc/dec/modify and emitting ASM                 | 
;; -----------------------------------------------------------------------------

(define_insn "*lod_hi_post_inc"
  [(set (match_operand:HI                      0 "metag_reg_nofloat_op" "=da")
        (mem:HI (post_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  "GETW\\t%0, [%1++]\\t%@ (*load HI post_inc OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_post_dec"
  [(set (match_operand:HI                      0 "metag_reg_nofloat_op" "=da")
        (mem:HI (post_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  "GETW\\t%0, [%1--]\\t%@ (*load HI post_dec OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_pre_inc"
  [(set (match_operand:HI                     0 "metag_reg_nofloat_op" "=da")
        (mem:HI (pre_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  "GETW\\t%0, [++%1]\\t%@ (*load HI pre_inc OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_pre_dec"
  [(set (match_operand:HI                     0 "metag_reg_nofloat_op" "=da")
        (mem:HI (pre_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  "GETW\\t%0, [--%1]\\t%@ (*load HI pre_dec OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_post_modify_disp"
  [(set (match_operand:HI                     0 "metag_reg_nofloat_op" "=da")
        (mem:HI (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_hi"      "O2")))))]
  ""
  "GETW\\t%0, [%1+%2++]\\t%@ (*load HI post_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_post_modify_reg"
  [(set (match_operand:HI                     0 "metag_reg_nofloat_op" "=da,da,da,da")
        (mem:HI (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  ""
  "GETW\\t%0, [%1+%2++]\\t%@ (*load HI post_modify_reg OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_pre_modify_disp"
  [(set (match_operand:HI                     0 "metag_reg_nofloat_op" "=da")
        (mem:HI (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_hi"      "O2")))))]
  ""
  "GETW\\t%0, [%1++%2]\\t%@ (*load HI pre_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_pre_modify_reg"
  [(set (match_operand:HI                     0 "metag_reg_nofloat_op" "=da,da,da,da")
        (mem:HI (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e,f,h,l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f,h,l")))))]
  ""
  "GETW\\t%0, [%1++%2]\\t%@ (*load HI pre_modify_reg OK)"
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Non-side effecting base+offset load HI and catchall load HI               |
;; -----------------------------------------------------------------------------

(define_insn "*lod_hi"
  [(set (match_operand:HI         0 "metag_reg_nofloat_op" "=da")
        (mem:HI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")))]
  ""
  "GETW\\t%0, [%1]\\t%@ (*lod hi rma OK)"
  [(set_attr "type" "load")])

;; movhi - base+index memory to register (loads)
(define_insn "*lod_hi_rma"
  [(set (match_operand:HI                  0 "metag_reg_nofloat_op" "=da,da,da,da")
        (mem:HI (plus:SI (match_operand:SI 1 "metag_regnofrm_op"    "%e, f, h, l ")
                         (match_operand:SI 2 "metag_regnofrm_op"     "e, f, h, l "))))]
  ""
  "GETW\\t%0, [%1+%2]\\t%@ (*lod hi rma OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_off12"
  [(set (match_operand:HI                  0 "metag_reg_nofloat_op" "=da,da")
        (mem:HI (plus:SI (match_operand:SI 1 "metag_reg12bit_op"     "da,Yr")
                         (match_operand:SI 2 "metag_offset12_hi"     "O2,Z2"))))]
  ""
  "GETW\\t%0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_off"
  [(set (match_operand:HI                  0 "metag_reg_nofloat_op" "=da")
        (mem:HI (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                         (match_operand:SI 2 "metag_offset6_hi"      "O2"))))]
  ""
  "GETW\\t%0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_hi_mem"
  [(set (match_operand:HI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:HI 1 "memory_operand"        "m"))]
  ""
  "GETW\\t%0, %1\\t%@ (*lod hi rm OK)"
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; movqi is made up of many parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "movqi"
  [(set (match_operand:QI 0 "nonimmediate_operand" "")
        (match_operand:QI 1 "general_operand"      ""))]
  ""
  {
    if (MEM_P (operands[0]))
      {
        /* All except mem = const or mem = mem can be done quickly */
        operands[1] = force_reg (QImode, operands[1]);
      }
  }
)

;; NOTE "move qi" needs to handle r <- r along with r <- m and m <- r variants together.
;; Otherwise one can end up with a "impossible reload" error if an argument to inline asm
;; fails to be assigned a register and thus needs to be fixed up by reload. This isn't
;; well documented and the underlying issue can go undetected.

;; movqi - all the register to register moves

;; this variant mov_qi is used if generated by reload and needs to handle memory
(define_insn "*mov_qi_reload"
  [(set (match_operand:QI 0 "metag_move_dst_op" "=e,f,h,l,r,!*Wx,  r, cx,cd,?da, cr,!*m")
        (match_operand:QI 1 "metag_move_src_op"  "e,f,h,l,r,  r, !*Wx,cx,cd,?da,!*m, cr"))]
  "reload_in_progress || reload_completed"
  {
    switch (which_alternative)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        if (METAG_FPC_REG_P (REGNO (operands[0])) || METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\\tMOV%?\\t%0, %1";
        else
          return "MOV%?\\t%0, %1";
      case 10:
        if (METAG_FPC_REG_P (REGNO (operands[0])))
          return "F\\tGETB\\t%0, %1";
        else
          return "GETB\\t%0, %1";
      case 11:
        if (METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\\tSETB\\t%0, %1";
        else
          return "SETB\\t%0, %1";
      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,fast,slow,slow,load,fast")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,no,no")
   (set_attr "memaccess" "none,none,none,none,none,none,none,none,none,none,load,store")
   (set_attr "predicable" "yes")])

;; this variant mov_qi is used if NOT generated by reload and doesn't handle memory
(define_insn "*mov_qi"
  [(set (match_operand:QI 0 "metag_register_op" "=e,f,h,l,cx,cd,?da")
        (match_operand:QI 1 "metag_register_op"  "e,f,h,l,cx,cd,?da"))]
  ""
  {
    static const char fmt[] = "F\\tMOV%?\\t%0, %1\\t\\t%@ (*mov qi rr OK)";

    return &fmt[(METAG_FPC_REG_P (REGNO (operands[0])) || METAG_FPC_REG_P (REGNO (operands[1]))) ? 0 : 2];
  }
  [(set_attr "type" "fast,fast,fast,fast,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; movqi - all the immediate to register sets
(define_insn "*set_qi"
  [(set (match_operand:QI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:QI 1 "metag_int_operand"     "KP"))]
  ""
  "MOV\\t%0, %1\\t\\t%@ (*set qi rI OK)"
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------
;; | Matching QI store post/pre_inc/dec/modify and emitting ASM                | 
;; -----------------------------------------------------------------------------

(define_insn "*sto_qi_post_inc"
  [(set (mem:QI (post_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:QI                      1 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETB\\t[%0++], %1\\t%@ (*store QI post_inc OK)"
  [(set_attr "type" "fast")])
  
(define_insn "*sto_qi_post_dec"
  [(set (mem:QI (post_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:QI                      1 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETB\\t[%0--], %1\\t%@ (*store QI post_dec OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_qi_pre_inc"
  [(set (mem:QI (pre_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:QI                     1 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETB\\t[++%0], %1\\t%@ (*store QI pre_inc OK)"
  [(set_attr "type" "fast")])
  
(define_insn "*sto_qi_pre_dec"
  [(set (mem:QI (pre_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:QI                     1 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETB\\t[--%0], %1\\t%@ (*store QI pre_dec OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_qi_post_modify_disp"
  [(set (mem:QI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_qi"      "O1,O1,O1,O1"))))
        (match_operand:QI                     2 "metag_reg_nofloat_op"  "t, u, y, z"))]
  "!TARGET_METAC_1_1"
  "SETB\\t[%0+%1++], %2\\t%@ (*store QI post_modify_disp OK)"
  [(set_attr "type" "fast,fast,fast,fast")])

(define_insn "*sto_qi_post_modify_disp_1_1"
  [(set (mem:QI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_qi"      "O1"))))
        (match_operand:QI                     2 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETB\\t[%0+%1++], %2\\t%@ (*store QI post_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_qi_post_modify_reg"
  [(set (mem:QI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e,f,h,l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e,f,h,l"))))
        (match_operand:QI                     2 "metag_reg_nofloat_op"  "t,u,y,z"))]
  ""
  "SETB\\t[%0+%1++], %2\\t%@ (*store QI post_modify_reg OK)"
  [(set_attr "type" "fast,fast,fast,fast")])

(define_insn "*sto_qi_pre_modify_disp"
  [(set (mem:QI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_qi"      "O1,O1,O1,O1"))))
        (match_operand:QI                     2 "metag_reg_nofloat_op"  "t, u, y, z"))]
  "!TARGET_METAC_1_1"
  "SETB\\t[%0++%1], %2\\t%@ (*store QI pre_modify_disp OK) @2"
  [(set_attr "type" "fast,fast,fast,fast")])

(define_insn "*sto_qi_pre_modify_disp_1_1"
  [(set (mem:QI (pre_modify:SI 
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_qi"      "O1"))))
        (match_operand:QI                     2 "metag_reg_nofloat_op"  "da"))]
  "TARGET_METAC_1_1"
  "SETB\\t[%0++%1], %2\\t%@ (*store QI pre_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_qi_pre_modify_reg"
  [(set (mem:QI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e,f,h,l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e,f,h,l"))))
        (match_operand:QI                     2 "metag_reg_nofloat_op"  "t,u,y,z"))]
  ""
  "SETB\\t[%0++%1], %2\\t%@ (*store QI pre_modify_reg OK)"
  [(set_attr "type" "fast,fast,fast,fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Non-side effecting base+offset store QI and catchall store QI cases       |
;; -----------------------------------------------------------------------------

;; movqi - base+index register to memory (stors's)
(define_insn "*sto_qi_mar"
  [(set (mem:QI (plus:SI (match_operand:SI 0 "metag_regnofrm_op"    "%e,f,h,l")
                         (match_operand:SI 1 "metag_regnofrm_op"    " e,f,h,l")))
        (match_operand:QI                  2 "metag_reg_nofloat_op" " t,u,y,z"))]
  ""
  "SETB\\t[%0+%1], %2\\t%@ (*sto qi mar OK)"
  [(set_attr "type"   "fast")
   (set_attr "length" "4,4,4,4")])

;; movqi - register to memory (stores) some are fast, rest match spillqi below
(define_insn "*sto_qi_reg_indirect"
  [(set (mem:QI (match_operand:SI 0 "metag_reg_nofloat_op" "e,f,h,l,!*da"))
        (match_operand:QI         1 "metag_reg_nofloat_op" "t,u,y,z, *da"))]
  "TARGET_COND_EXEC_OPTIMIZE && !TARGET_METAC_1_1"
  "@
   SETB%?\\t[%0], %1\\t%@ (*sto qi [e]t OK)
   SETB%?\\t[%0], %1\\t%@ (*sto qi [f]u OK)
   SETB%?\\t[%0], %1\\t%@ (*sto qi [h]y OK)
   SETB%?\\t[%0], %1\\t%@ (*sto qi [l]z OK)
   #"
  [(set_attr "type" "fast,fast,fast,fast,invalid")
   (set_attr "cond" "yes,yes,yes,yes,no")])

(define_insn "*sto_qi"
  [(set (match_operand:QI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:QI 1 "metag_reg_nofloat_op"  "r, t, u, y, z, !*da"))]
  "!TARGET_METAC_1_1 && !reload_completed"
  "@
   SETB\\t%0, %1\\t%@ (*sto qi [r]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [e]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [f]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [h]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [l]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [m]r OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_qi_postreload"
  [(set (match_operand:QI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl")
        (match_operand:QI 1 "metag_reg_nofloat_op"  "r, t, u, y, z"))]
  "!TARGET_METAC_1_1 && reload_completed"
  "@
   SETB\\t%0, %1\\t%@ (*sto qi [r]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [e]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [f]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [h]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [l]r OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_qi_1_1_reg_indirect"
  [(set (mem:QI (match_operand:SI 0 "metag_reg_nofloat_op" "da"))
        (match_operand:QI         1 "metag_reg_nofloat_op" "da"))]
  "TARGET_COND_EXEC_OPTIMIZE && TARGET_METAC_1_1"
  "SETB%?\\t[%0], %1\\t%@ (*sto qi [r]r OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_qi_1_1_off12"
  [(set (mem:QI (plus:SI (match_operand:SI 0 "metag_reg12bit_op"    "da,Yr")
                         (match_operand:SI 1 "metag_offset12_qi"    "O1,Z1")))
        (match_operand:QI                  2 "metag_reg_nofloat_op" "da,da"))]
  "TARGET_METAC_1_1"
  "SETB\\t[%0+%1], %2"
  [(set_attr "type" "fast")])

(define_insn "*sto_qi_1_1_off6"
  [(set (mem:QI (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "da")
                         (match_operand:SI 1 "metag_offset6_qi"     "O1")))
        (match_operand:QI                  2 "metag_reg_nofloat_op" "da"))]
  "TARGET_METAC_1_1"
  "SETB\\t[%0+%1], %2"
  [(set_attr "type" "fast")])

(define_insn "*sto_qi_1_1"
  [(set (match_operand:QI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:QI 1 "metag_reg_nofloat_op"  "r, t, u, y, z, !*da"))]
  "TARGET_METAC_1_1 && !reload_completed"
  "@
   SETB\\t%0, %1\\t%@ (*sto qi [r]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [e]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [f]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [h]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [l]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [m]r OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_qi_1_1_postreload"
  [(set (match_operand:QI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl")
        (match_operand:QI 1 "metag_reg_nofloat_op"  "r, t, u, y, z"))]
  "TARGET_METAC_1_1 && reload_completed"
  "@
   SETB\\t%0, %1\\t%@ (*sto qi [r]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [e]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [f]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [h]r OK)
   SETB\\t%0, %1\\t%@ (*sto qi [l]r OK)"
  [(set_attr "type" "fast")])

;; spillqi - register to memory (stores) from source/dest in same bank
(define_split
  [(set (match_operand:QI 0 "memory_operand"    "")
        (match_operand:QI 1 "metag_register_op" ""))]
  "!TARGET_METAC_1_1
   && reload_completed
   && metag_slow_store (operands[0], operands[1])"
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 0)
        (match_dup 2))]
  {
    operands[2] = metag_gen_safe_temp (QImode, operands[1]);
  }
)

;; -----------------------------------------------------------------------------
;; | Matching QI load post/pre_inc/dec/modify and emitting ASM                 | 
;; -----------------------------------------------------------------------------

(define_insn "*lod_qi_post_inc"
  [(set (match_operand:QI                      0 "metag_reg_nofloat_op" "=da")
        (mem:QI (post_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  "GETB\\t%0, [%1++]\\t%@ (*load QI post_inc OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_post_dec"
  [(set (match_operand:QI                      0 "metag_reg_nofloat_op" "=da")
        (mem:QI (post_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  "GETB\\t%0, [%1--]\\t%@ (*load QI post_dec OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_pre_inc"
  [(set (match_operand:QI                     0 "metag_reg_nofloat_op" "=da")
        (mem:QI (pre_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  "GETB\\t%0, [++%1]\\t%@ (*load QI pre_inc OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_pre_dec"
  [(set (match_operand:QI                     0 "metag_reg_nofloat_op" "=da")
        (mem:QI (pre_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  "GETB\\t%0, [--%1]\\t%@ (*load QI pre_dec OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_post_modify_disp"
  [(set (match_operand:QI                     0 "metag_register_op"    "=da")
        (mem:QI (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_qi"      "O1")))))]
  ""
  "GETB\\t%0, [%1+%2++]\\t%@ (*load QI post_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_post_modify_reg"
  [(set (match_operand:QI                     0 "metag_reg_nofloat_op" "=da,da,da,da")
        (mem:QI (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  ""
  "GETB\\t%0, [%1+%2++]\\t%@ (*load QI post_modify_reg OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_pre_modify_disp"
  [(set (match_operand:QI                     0 "metag_reg_nofloat_op" "=da")
        (mem:QI (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_qi"      "O1")))))]
  ""
  "GETB\\t%0, [%1++%2]\\t%@ (*load QI pre_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_pre_modify_reg"
  [(set (match_operand:QI                     0 "metag_reg_nofloat_op" "=da,da,da,da")
        (mem:QI (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  ""
  "GETB\\t%0, [%1++%2]\\t%@ (*load QI pre_modify_reg OK)"
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Non-side effecting base+offset load QI and catchall load QI               |
;; -----------------------------------------------------------------------------

(define_insn "*lod_qi"
  [(set (match_operand:QI         0 "metag_reg_nofloat_op" "=da")
        (mem:QI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")))]
  ""
  "GETB\\t%0, [%1]\\t%@ (*lod qi rma OK)"
  [(set_attr "type" "load")])

;; movqi - base+index memory to register (loads)
(define_insn "*lod_qi_rma"
  [(set (match_operand:QI             0 "metag_reg_nofloat_op" "=da,da,da,da")
   (mem:QI (plus:SI (match_operand:SI 1 "metag_regnofrm_op"     "e, f, h, l")
                    (match_operand:SI 2 "metag_regnofrm_op"     "e, f, h, l"))))]
  ""
  "GETB\\t%0, [%1+%2]\\t%@ (*lod qi rma OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_off12"
  [(set (match_operand:QI                  0 "metag_reg_nofloat_op" "=da,da")
        (mem:QI (plus:SI (match_operand:SI 1 "metag_reg12bit_op"     "da,Yr")
                         (match_operand:SI 2 "metag_offset12_qi"     "O1,Z1"))))]
  ""
  "GETB\\t%0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_off"
  [(set (match_operand:QI                  0 "metag_reg_nofloat_op" "=da")
        (mem:QI (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                         (match_operand:SI 2 "metag_offset6_qi"      "O1"))))]
  ""
  "GETB\\t%0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_qi_mem"
  [(set (match_operand:QI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:QI 1 "memory_operand"        "m"))]
  ""
  "GETB\\t%0, %1"
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; movsf is made up of many parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "movsf"
  [(set (match_operand:SF 0 "nonimmediate_operand" "")
        (match_operand:SF 1 "general_operand"      ""))]
  ""
  {
    if (MEM_P (operands[0]))
      {
        /* All except mem = const or mem = mem can be done quickly */
        operands[1] = force_reg (SFmode, operands[1]);
      }
  }
)

;; NOTE "move sf" needs to handle r <- r along with r <- m and m <- r variants together.
;; Otherwise one can end up with a "impossible reload" error if an argument to inline asm
;; fails to be assigned a register and thus needs to be fixed up by reload. This isn't
;; well documented and the underlying issue can go undetected.

;; movsf - all the register to register moves

;; this variant mov_sf is used if generated by reload and needs to handle memory
(define_insn "*mov_sf_reload"
  [(set (match_operand:SF 0 "metag_move_dst_op" "=cx,cx,d ,d,a,da, cr,!*m")
        (match_operand:SF 1 "metag_move_src_op"  "cx,d, cx,d,a,da,!*m, cr"))]
  "reload_in_progress || reload_completed"
  {
    switch (which_alternative)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        if (METAG_FPC_REG_P (REGNO (operands[0])) || METAG_FPC_REG_P (REGNO (operands[1])))
            return "F\\tMOV%?\\t%0, %1";
        else
          return "MOV%?\\t%0, %1";
      case 6:
        if (METAG_FPC_REG_P (REGNO (operands[0])))
          return "F\\tGETD\\t%0, %1";
        else
          return "GETD\\t%0, %1";
      case 7:
        if (METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\tSETD\\t%0, %1";
        else
          return "SETD\\t%0, %1";      default:
        gcc_unreachable ();
      }
  }
  [(set_attr "type" "fast,slow,slow,fast,fast,slow,load,fast")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,no,no")
   (set_attr "memaccess" "none,none,none,none,none,none,load,store")
   (set_attr "predicable" "yes")])

;; this variant mov_sf is used if NOT generated by reload and doesn't handle memory
(define_insn "*mov_sf"
  [(set (match_operand:SF 0 "metag_register_op" "=cx,cx,d ,d,a,da")
        (match_operand:SF 1 "metag_register_op"  "cx,d, cx,d,a,da"))]
  ""
  {
    static const char fmt[] = "F\\tMOV%?\\t%0, %1\\t\\t%@ (*mov sf rr OK)";

    return &fmt[(METAG_FPC_REG_P (REGNO (operands[0])) || METAG_FPC_REG_P (REGNO (operands[1]))) ? 0 : 2];
  }
  [(set_attr "type" "fast,slow,slow,fast,fast,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; movsf - all the immediate to register sets
(define_insn_and_split "*set_sf"
  [(set (match_operand:SF 0 "metag_register_op" "=da,cx")
        (match_operand:SF 1 "immediate_operand"  "i, ci"))]
  ""
  "@
  #
  F\\tMOV\\t%0,#%h1"
  "reload_completed
   && (!METAG_FPC_REG_P (REGNO (operands[0]))
       || !metag_fphalf_imm_op (operands[1], SFmode))"
  [(const_int 0)]
  {
    metag_split_movsf_immediate (operands);
    DONE;
  }
  [(set_attr "type" "two")])

;; movsf - register to memory (stores) some are fast, rest match spillsf below
(define_insn "*sto_sf_reg_indirect"
  [(set (mem:SF (match_operand:SI 0 "metag_reg_nofloat_op" "e,f,h,l,!*da"))
        (match_operand:SF         1 "metag_register_op"    "t,u,y,z, *da"))]
  "TARGET_COND_EXEC_OPTIMIZE && !TARGET_METAC_1_1"
  "@
  SETD%?\\t[%0], %1\\t%@ (*sto sf [e]t OK)
  SETD%?\\t[%0], %1\\t%@ (*sto sf [f]u OK)
  SETD%?\\t[%0], %1\\t%@ (*sto sf [h]y OK)
  SETD%?\\t[%0], %1\\t%@ (*sto sf [l]z OK)
  #"
  [(set_attr "type" "fast,fast,fast,fast,invalid")
   (set_attr "cond" "yes,yes,yes,yes,no")])

(define_insn "*sto_sf_post_inc"
  [(set (mem:SF (post_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:SF                      1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0++], %1\\t%@ (*store SF post_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])
  
(define_insn "*sto_sf_post_dec"
  [(set (mem:SF (post_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:SF                      1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0--], %1\\t%@ (*store SF post_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_sf_pre_inc"
  [(set (mem:SF (pre_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:SF                     1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[++%0], %1\\t%@ (*store SF pre_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])
  
(define_insn "*sto_sf_pre_dec"
  [(set (mem:SF (pre_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:SF                     1 "metag_reg_nofloat_op"  "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[--%0], %1\\t%@ (*store SF pre_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_sf_post_modify_disp"
  [(set (mem:SF (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_sf"      "O4,O4,O4,O4"))))
        (match_operand:SF                     2 "metag_register_op"     "t, u, y, z"))]
  "!TARGET_METAC_1_1"
  "SETD\\t[%0+%1++], %2\\t%@ (*store SF post_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_sf_post_modify_disp_1_1"
  [(set (mem:SF (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_sf"      "O4"))))
        (match_operand:SF                     2 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0+%1++], %2\\t%@ (*store SF post_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_sf_post_modify_reg"
  [(set (mem:SF (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l ")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:SF                     2 "metag_register_op"     "ct,cu,cy,cz"))]
  ""
  {
    static const char fmt[] = "F\\tSETD\\t[%0+%1++], %2\\t%@ (*store SF post_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_sf_pre_modify_disp"
  [(set (mem:SF (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_sf"      "O4,O4,O4,O4"))))
        (match_operand:SF                     2 "metag_reg_nofloat_op"  "t, u, y, z"))]
  "!TARGET_METAC_1_1"
  "SETD\\t[%0++%1], %2\\t%@ (*store SF pre_modify_disp OK)"
  [(set_attr "type" "fast,fast,fast,fast")])

(define_insn "*sto_sf_pre_modify_disp_1_1"
  [(set (mem:SF (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_sf"      "O4"))))
        (match_operand:SF                     2 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0++%1], %2\\t%@ (*store SF pre_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_sf_pre_modify_reg"
  [(set (mem:SF (pre_modify:SI 
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:SF                     2 "metag_register_op"     "ct,cu,cy,cz"))]
  ""
  {
    static const char fmt[] = "F\\tSETD\\t[%0++%1], %2\\t%@ (*store SF pre_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; movsf - register to memory (stores) some are fast, rest match spillsf below
(define_insn "*sto_sf"
  [(set (match_operand:SF 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:SF 1 "metag_reg_nofloat_op"  "da,t, u, y, z, !*da"))]
  "!TARGET_METAC_1_1 && !reload_completed"
  "@
   SETD\\t%0, %1\\t%@ (*sto sf [r]da OK)
   SETD\\t%0, %1\\t%@ (*sto sf [e]t OK)
   SETD\\t%0, %1\\t%@ (*sto sf [f]u OK)
   SETD\\t%0, %1\\t%@ (*sto sf [h]y OK)
   SETD\\t%0, %1\\t%@ (*sto sf [l]z OK)
   SETD\\t%0, %1\\t%@ (*sto sf [m]da OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_sf_postreload"
  [(set (match_operand:SF 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl")
        (match_operand:SF 1 "metag_reg_nofloat_op"  "da,t, u, y, z"))]
  "!TARGET_METAC_1_1 && reload_completed"
  "@
   SETD\\t%0, %1\\t%@ (*sto sf [r]da OK)
   SETD\\t%0, %1\\t%@ (*sto sf [e]t OK)
   SETD\\t%0, %1\\t%@ (*sto sf [f]u OK)
   SETD\\t%0, %1\\t%@ (*sto sf [h]y OK)
   SETD\\t%0, %1\\t%@ (*sto sf [l]z OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_sf_1_1_reg_indirect"
  [(set (mem:SF (match_operand:SI 0 "metag_reg_nofloat_op" "da"))
        (match_operand:SF         1 "metag_register_op"    "cr"))]
  "TARGET_COND_EXEC_OPTIMIZE && TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD%?\\t[%0], %1\\t%@ (*sto sf [r]r OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_sf_1_1_off12"
  [(set (mem:SF (plus:SI (match_operand:SI 0 "metag_reg12bit_op"    "da,Yr")
                         (match_operand:SI 1 "metag_offset12_sf"    "O4,Z4")))
        (match_operand:SF                  2 "metag_reg_nofloat_op" "da,da"))]
  "TARGET_METAC_1_1"
  "SETD\\t[%0+%1], %2"
  [(set_attr "type" "fast")])

(define_insn "*sto_sf_1_1_off6"
  [(set (mem:SF (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "da")
                         (match_operand:SI 1 "metag_offset6_sf"     "O4")))
        (match_operand:SF                  2 "metag_register_op"    "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETD\\t[%0+%1], %2";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_sf_1_1"
  [(set (match_operand:SF 0 "memory_operand"    "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:SF 1 "metag_register_op"  "da,t ,u, y, z, !*da"))]
  "TARGET_METAC_1_1 && !reload_completed"
  "SETD\\t%0, %1\\t%@ (*sto sf [m]r OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_sf_1_1_postreload"
  [(set (match_operand:SF 0 "memory_operand"    "=Tr,Te,Tf,Th,Tl")
        (match_operand:SF 1 "metag_register_op"  "da,t, u, y, z"))]
  "TARGET_METAC_1_1 && reload_completed"
  "SETD\\t%0, %1\\t%@ (*sto sf [m]r OK)"
  [(set_attr "type" "fast")])

;; spillsf - register to memory (stores) from source/dest in same bank
(define_split
  [(set (match_operand:SF 0 "memory_operand"    "")
        (match_operand:SF 1 "metag_register_op" ""))]
  "!TARGET_METAC_1_1
   && reload_completed
   && metag_slow_store (operands[0], operands[1])"
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 0)
        (match_dup 2))]
  {
    operands[2] = metag_gen_safe_temp (SFmode, operands[1]);
  }
)

;; -----------------------------------------------------------------------------
;; | Matching SF load post/pre_inc/dec/modify and emitting ASM                 | 
;; -----------------------------------------------------------------------------

(define_insn "*lod_sf_post_inc"
  [(set (match_operand:SF                      0 "metag_register_op"    "=cr")
        (mem:SF (post_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1++]\\t%@ (*load SF post_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf_post_dec"
  [(set (match_operand:SF                      0 "metag_register_op"    "=cr")
        (mem:SF (post_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1--]\\t%@ (*load SF post_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf_pre_inc"
  [(set (match_operand:SF                     0 "metag_register_op"    "=cr")
        (mem:SF (pre_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETD\\t%0, [++%1]\\t%@ (*load SF pre_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf_pre_dec"
  [(set (match_operand:SF                     0 "metag_register_op"    "=cr")
        (mem:SF (pre_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETD\\t%0, [--%1]\\t%@ (*load SF pre_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf_post_modify_disp"
  [(set (match_operand:SF                     0 "metag_register_op"    "=cr")
        (mem:SF (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_sf"      "O4")))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1+%2++]\\t%@ (*load SF post_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf_post_modify_reg"
  [(set (match_operand:SF                     0 "metag_register_op"    "=cr,cr,cr,cr")
        (mem:SF (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1+%2++]\\t%@ (*load SF post_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf_pre_modify_disp"
  [(set (match_operand:SF                     0 "metag_register_op"    "=cr")
        (mem:SF (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_sf"      "O4")))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1++%2]\\t%@ (*load SF pre_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf_pre_modify_reg"
  [(set (match_operand:SF                     0 "metag_register_op"    "=cr,cr,cr,cr")
        (mem:SF (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1++%2]\\t%@ (*load SF pre_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; movsf - memory to register (loads)
(define_insn "*lod_sf_rma"
  [(set (match_operand:SF                  0 "metag_register_op" "=cr,cr,cr,cr")
        (mem:SF (plus:SI (match_operand:SI 1 "metag_regnofrm_op"  "e, f, h, l")
                         (match_operand:SI 2 "metag_regnofrm_op"  "e, f, h, l"))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1+%2]";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf_off12"
  [(set (match_operand:SF                  0 "metag_reg_nofloat_op" "=da,da")
        (mem:SF (plus:SI (match_operand:SI 1 "metag_reg12bit_op"     "da,Yr")
                         (match_operand:SI 2 "metag_offset12_sf"     "O4,Z4"))))]
  ""
  "GETD\\t%0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_sf_off"
  [(set (match_operand:SF                  0 "metag_register_op"    "=cr")
        (mem:SF (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                         (match_operand:SI 2 "metag_offset6_sf"      "O4"))))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1+%2]";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf"
  [(set (match_operand:SF         0 "metag_register_op"    "=cr")
        (mem:SF (match_operand:SI 1 "metag_reg_nofloat_op"  "da")))]
  ""
  {
    static const char fmt[] = "F\\tGETD\\t%0, [%1]";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_sf_mem"
  [(set (match_operand:SF 0 "metag_reg_nofloat_op" "=da")
        (match_operand:SF 1 "memory_operand"        "m"))]
  ""
  "GETD\\t%0, %1\\t%@ (*lod sf rm OK)"
  [(set_attr "type" "load")])

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; movdi is made up of many parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "movdi"
  [(set (match_operand:DI 0 "nonimmediate_operand" "")
        (match_operand:DI 1 "general_operand"      ""))]
  ""
  {
    if (MEM_P (operands[0]))
      {
        if (!no_new_pseudos)
          {
            /* All except mem = const or mem = mem can be done quickly */
            operands[1] = force_reg (DImode, operands[1]);
          }
      }
  }
)

;; NOTE "move di" needs to handle r <- r along with r <- m and m <- r variants together.
;; Otherwise one can end up with a "impossible reload" error if an argument to inline asm
;; fails to be assigned a register and thus needs to be fixed up by reload. This isn't
;; well documented and the underly issue can do undetected.

;; movdi - register to register forms

;; this variant mov_di is used if generated by reload and needs to handle memory
(define_insn "*mov_di_reload"
  [(set (match_operand:DI 0 "metag_move_dst_op" "=d,a,d, cx,cx,?da, cr,!*m")
        (match_operand:DI 1 "metag_move_src_op"  "d,a,cx,d, cx,?da,!*m, cr"))]
  "reload_in_progress || reload_completed"
  {
    switch (which_alternative)
      {
      case 0:
      case 1:
      case 5:
        return "#";
      case 2:
      case 3:
      case 4:
        if (metag_fpu_single)
          return "#";
        else
          return "FD\\tMOV\\t%0,%1";
      case 6:
        if (METAG_FPC_REG_P (REGNO (operands[0])))
          return "F\\tGETL\\t%0,%t0,%1";
        else
          return "GETL\\t%0,%t0,%1";
      case 7:
        if (METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\\tSETL\\t%0,%1,%t1";
        else
          return "SETL\\t%0,%1,%t1";
      default:
        gcc_unreachable();
      }
  }
  [(set_attr "type" "two,two,two,two,two,slowslow,load,fast")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,no,no")
   (set_attr "memaccess" "none,none,none,none,none,none,load,store")
   (set_attr "predicable" "yes")])

;; this variant mov_di is used if NOT generated by reload and doesn't handle memory
(define_insn_and_split "*mov_di"
  [(set (match_operand:DI 0 "metag_register_op" "=d,a,d, cx,cx,?da")
        (match_operand:DI 1 "metag_register_op"  "d,a,cx,d, cx,?da"))]
  ""
  {
    switch (which_alternative)
      {
      case 0:
      case 1:
      case 5:
        return "#";
      case 2:
      case 3:
      case 4:
        if (metag_fpu_single)
          return "#";
        else
          return "FD\\tMOV\\t%0,%1";
      default:
        gcc_unreachable();
      }
  }
  "reload_completed"
  [(const_int 0)]
  {
    /* WORK NEEDED: When in hard-float mode, FL MOV will do a dual
                    unit MOV to FCC regs */
    if (TARGET_DSP
        && metag_datareg_p (REGNO (operands[0]))
        && metag_datareg_p (REGNO (operands[1]))) 
      {
        operands[0] = gen_rtx_REG (V2SImode, REGNO (operands[0]));
        operands[1] = gen_rtx_REG (V2SImode, REGNO (operands[1]));
        emit_insn (gen_rtx_SET (VOIDmode, operands[0], operands[1]));
      }
    else
      metag_split_movdi (operands);
    DONE;
  }
  [(set_attr "type" "two,two,two,two,two,slowslow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

(define_insn_and_split "*set_di"
  [(set (match_operand:DI 0 "metag_reg_nofloat_op" "=d,a")
        (match_operand:DI 1 "immediate_operand"     "i,i"))]
  ""
  "#"
  "reload_completed"
  [(const_int 0)]
  {
    metag_split_movdi_immediate (operands);
    DONE;
  }
  [(set_attr "type" "four")])


;; -----------------------------------------------------------------------------
;; | Matching DI store post/pre_inc/dec/modify and emitting ASM                | 
;; -----------------------------------------------------------------------------

(define_insn "*sto_di_post_inc_concat"
  [(set (mem:DI (post_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da,da")))
        (unspec:DI          [(match_operand:SI 1 "metag_reg_nofloat_op"  "e, h")
                             (match_operand:SI 2 "metag_reg_nofloat_op"  "f, l")] UNSPEC_CONCAT))]
  "TARGET_METAC_1_1"
  "SETL\\t[%0++], %1, %2"
  [(set_attr "type" "fast")])

(define_insn "*sto_di_post_inc"
  [(set (mem:DI (post_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:DI                      1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0++], %1, %t1\\t%@ (*store DI post_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])
  
(define_insn "*sto_di_post_dec"
  [(set (mem:DI (post_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:DI                      1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0--], %1, %t1\\t%@ (*store DI post_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_di_pre_inc"
  [(set (mem:DI (pre_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:DI                     1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[++%0], %1, %t1\\t%@ (*store DI pre_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])
  
(define_insn "*sto_di_pre_dec"
  [(set (mem:DI (pre_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:DI                     1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[--%0], %1, %t1\\t%@ (*store DI pre_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_di_post_modify_disp"
  [(set (mem:DI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+d, a")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_di"      "O8,O8"))))
        (match_operand:DI                     2 "metag_register_op"     "a, d"))]
  "!TARGET_METAC_1_1"
  "SETL\\t[%0+%1++], %2, %t2\\t%@ (*store DI post_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_di_post_modify_disp_1_1"
  [(set (mem:DI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_di"      "O8"))))
        (match_operand:DI                     2 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0+%1++], %2, %t2\\t%@ (*store DI post_modify_disp_1_1 OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_di_post_modify_reg"
  [(set (mem:DI (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:DI                     2 "metag_register_op"     "ca,ca,cd,cd"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0+%1++], %2, %t2\\t%@ (*store DI post_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_di_pre_modify_disp"
  [(set (mem:DI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+d, a")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_di"      "O8,O8"))))
        (match_operand:DI                     2 "metag_reg_nofloat_op"  "a, d"))]
  "!TARGET_METAC_1_1"
  "SETL\\t[%0++%1], %2, %t2\\t%@ (*store DI pre_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_di_pre_modify_disp_1_1"
  [(set (mem:DI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_di"      "O8"))))
        (match_operand:DI                     2 "metag_reg_nofloat_op"  "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0++%1], %2, %t2\\t%@ (*store DI pre_modify_disp_1_1 OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_di_pre_modify_reg"
  [(set (mem:DI (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:DI                     2 "metag_register_op"     "ca,ca,cd,cd"))]
  ""
  {
    static const char fmt[] = "F\\tSETL\\t[%0++%1], %2, %t2\\t%@ (*store DI pre_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Non-side effecting base+offset store DI and catchall store DI cases       |
;; -----------------------------------------------------------------------------

;; movdi - register to memory (stores) some are fast, rest match spilldi below
(define_insn "*sto_di_cond_exec_concat"
  [(set (mem:DI     (match_operand:SI 0 "metag_reg_nofloat_op" "a,d,!da"))
        (unspec:DI [(match_operand:SI 1 "metag_reg_nofloat_op" "d,a, da")
                    (match_operand:SI 2 "metag_reg_nofloat_op" "d,a, da")] UNSPEC_CONCAT))]
  "TARGET_COND_EXEC_OPTIMIZE
   && !TARGET_METAC_1_1
   && reload_completed
   && !(   metag_regno_same_unit_p (REGNO (operands[0]), REGNO (operands[1]))
        || metag_regno_same_unit_p (REGNO (operands[0]), REGNO (operands[2])))"
  "SETL%?\\t[%0], %1, %2\\t%@ (*sto di OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_di_cond_exec"
  [(set (mem:DI (match_operand:SI 0 "metag_reg_nofloat_op" "a,d,!da"))
        (match_operand:DI         1 "metag_reg_nofloat_op" "d,a, da"))]
  "TARGET_COND_EXEC_OPTIMIZE
   && !TARGET_METAC_1_1
   && reload_completed
   && !(   metag_regno_same_unit_p (REGNO (operands[0]), REGNO (operands[1]))
        || metag_regno_same_unit_p (REGNO (operands[0]), REGNO (operands[1]) + 1))"
  "SETL%?\\t[%0], %1, %t1\\t%@ (*sto di OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_di_concat"
  [(set (match_operand:DI             0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!m")
        (unspec:DI [(match_operand:SI 1 "metag_reg_nofloat_op"  "r, a, a, d, d, da")
                    (match_operand:SI 2 "metag_reg_nofloat_op"  "r, a, a, d, d, da")] UNSPEC_CONCAT))]
  "!TARGET_METAC_1_1"
  "SETL\\t%0, %1, %2"
  [(set_attr "type" "fast")])

(define_insn "*sto_di_off12"
  [(set (mem:DI (plus:SI (match_operand:SI 0 "metag_reg12bit_op"    "e, f, h, l, Ye,Yf,Yh,Yl")
                         (match_operand:SI 1 "metag_offset12_di"    "O8,O8,O8,O8,Z8,Z8,Z8,Z8")))
        (match_operand:DI                  2 "metag_reg_nofloat_op" "a, a, d, d, a, a, d, d"))]
  "!TARGET_METAC_1_1"
  "SETL\\t[%0+%1], %2, %t2"
  [(set_attr "type" "fast")])

(define_insn "*sto_di_off6"
  [(set (mem:DI (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "e, f, h, l")
                         (match_operand:SI 1 "metag_offset6_di"     "O8,O8,O8,O8")))
        (match_operand:DI                  2 "metag_register_op"    "a, a, d, d"))]
  "!TARGET_METAC_1_1"
  "SETL\\t[%0+%1], %2, %t2"
  [(set_attr "type" "fast")])

(define_insn "*sto_di"
  [(set (match_operand:DI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:DI 1 "metag_reg_nofloat_op"  "r, a, a, d, d, !*da"))]
  "!TARGET_METAC_1_1 && !reload_completed"
  "SETL\\t%0, %1, %t1"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_di_postreload"
  [(set (match_operand:DI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl")
        (match_operand:DI 1 "metag_reg_nofloat_op"  "r, a, a, d, d"))]
  "!TARGET_METAC_1_1 && reload_completed"
  "SETL\\t%0, %1, %t1"
  [(set_attr "type" "fast")])

;; movdi - base+index register to memory (stores)
(define_insn "*sto_di_mar"
  [(set (mem:DI (plus:SI (match_operand:SI 0 "metag_regnofrm_op" "%e, f, h, l")
                         (match_operand:SI 1 "metag_regnofrm_op" " e, f, h, l")))
        (match_operand:DI                  2 "metag_register_op" "ca,ca,cd,cd"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0+%1], %2, %t2\\t\\t%@ (*sto di mar OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type"   "fast")
   (set_attr "length" "4,4,4,4")])

(define_insn "*sto_di_reg_indirect_concat_1_1"
  [(set (mem:DI     (match_operand:SI 0 "metag_reg_nofloat_op" "da"))
        (unspec:DI [(match_operand:SI 1 "metag_reg_nofloat_op" "da")
                    (match_operand:SI 2 "metag_reg_nofloat_op" "da")] UNSPEC_CONCAT))]
  "TARGET_COND_EXEC_OPTIMIZE && TARGET_METAC_1_1"
  "SETL%?\\t[%0], %1, %2\\t%@ (*sto di OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_di_reg_indirect_1_1"
  [(set (mem:DI (match_operand:SI 0 "metag_reg_nofloat_op" "da"))
        (match_operand:DI         1 "metag_reg_nofloat_op" "da"))]
  "TARGET_COND_EXEC_OPTIMIZE && TARGET_METAC_1_1"
  "SETL%?\\t[%0], %1, %t1\\t%@ (*sto di OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_di_concat_1_1"
  [(set             (match_operand:DI 0 "memory_operand"    "=m, m")
        (unspec:DI [(match_operand:SI 1 "metag_register_op"  "da,cx")
                    (match_operand:SI 2 "metag_register_op"  "da,cx")] UNSPEC_CONCAT))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t%0, %1, %2\\t\\t%@ (*sto di [r]r OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_di_1_1_off12"
  [(set (mem:DI (plus:SI (match_operand:SI 0 "metag_reg12bit_op"    "da,Yr")
                         (match_operand:SI 1 "metag_offset12_di"    "O8,Z8")))
        (match_operand:DI                  2 "metag_reg_nofloat_op" "da,da"))]
  "TARGET_METAC_1_1"
  "SETL\\t[%0+%1], %2, %t2"
  [(set_attr "type" "fast")])

(define_insn "*sto_di_1_1_off6"
  [(set (mem:DI (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "da")
                         (match_operand:SI 1 "metag_offset6_di"     "O8")))
        (match_operand:DI                  2 "metag_register_op"    "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0+%1], %2, %t2";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_di_1_1"
  [(set (match_operand:DI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:DI 1 "metag_reg_nofloat_op"  "r, a, a, d, d, !*da"))]
  "TARGET_METAC_1_1 && !reload_completed"
  "SETL\\t%0, %1, %t1\\t\\t%@ (*sto di [r]r OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_di_1_1_postreload"
  [(set (match_operand:DI 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl")
        (match_operand:DI 1 "metag_reg_nofloat_op"  "r, a, a, d, d"))]
  "TARGET_METAC_1_1 && reload_completed"
  "SETL\\t%0, %1, %t1\\t\\t%@ (*sto di [r]r OK)"
  [(set_attr "type" "fast")])

;; spilldi - register to memory (stores) from source/dest in same bank
(define_split
  [(set (match_operand:DI 0 "memory_operand"    "")
        (match_operand:DI 1 "metag_register_op" ""))]
  "!TARGET_METAC_1_1
   && reload_completed
   && metag_slow_store (operands[0], operands[1])"
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 0)
        (match_dup 2))]
  {
    operands[2] = metag_gen_safe_temp (DImode, operands[1]);
  }
)

;; -----------------------------------------------------------------------------

;; movdi - memory to register (loads)

;; -----------------------------------------------------------------------------
;; | Matching DI store [post/pre]_[inc/dec/modify] and emitting ASM             | 
;; -----------------------------------------------------------------------------

(define_insn "*lod_di_concat_post_inc"
 [(set  (unspec:DI          [(match_operand:SI 0 "metag_reg_nofloat_op" "=da")
                             (match_operand:SI 1 "metag_reg_nofloat_op" "=da")] UNSPEC_CONCAT)
        (mem:DI (post_inc:SI (match_operand:SI 2 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  "GETL\\t%0, %1, [%2++]\\t%@ (*load DI post_inc OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_di_post_inc"
  [(set (match_operand:DI                      0 "metag_register_op"    "=cr")
        (mem:DI (post_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1++]\\t%@ (*load DI post_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])
 
(define_insn "*lod_di_post_dec"
  [(set (match_operand:DI                      0 "metag_register_op"    "=cr")
        (mem:DI (post_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1--]\\t%@ (*load DI post_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_di_pre_inc"
  [(set (match_operand:DI                     0 "metag_register_op"    "=cr")
        (mem:DI (pre_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [++%1]\\t%@ (*load DI pre_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])
 
(define_insn "*lod_di_pre_dec"
  [(set (match_operand:DI                     0 "metag_register_op"    "=cr")
        (mem:DI (pre_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da"))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [--%1]\\t%@ (*load DI pre_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_di_post_modify_disp"
  [(set (match_operand:DI                     0 "metag_register_op"    "=cr")
        (mem:DI (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_di"      "O8")))))]
  ""
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DI post_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_di_post_modify_reg"
  [(set (match_operand:DI                     0 "metag_register_op"    "=cr,cr,cr,cr")
        (mem:DI (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  ""
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DI post_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_di_pre_modify_disp"
  [(set (match_operand:DI                     0 "metag_register_op"    "=cr")
        (mem:DI (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_di"      "O8")))))]
  ""
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1++%2]\\t%@ (*load DI pre_modify_disp OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_di_pre_modify_reg"
  [(set (match_operand:DI                     0 "metag_register_op"    "=cr,cr,cr,cr")
        (mem:DI (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l")))))]
  ""
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1++%2]\\t%@ (*load DI pre_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; -----------------------------------------------------------------------------
;; | Non-side effecting base+offset load DI and catchall load DI               |
;; -----------------------------------------------------------------------------

(define_insn "*lod_di"
  [(set (match_operand:DI         0 "metag_register_op"  "=cr")
        (mem:DI (match_operand:SI 1 "metag_regnofrm_op"   "da")))]
  ""
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1]\\t%@ (*lod di rma OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; movdi - base+index memory to register (loads)
(define_insn "*lod_di_rma"
  [(set (match_operand:DI                  0 "metag_register_op" "=cr,cr,cr,cr")
        (mem:DI (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "%e, f, h, l")
                         (match_operand:SI 2 "metag_regnofrm_op"  "e, f, h, l"))))]
  ""
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1+%2]\\t%@ (*lod qi rma OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_di_concat"
  [(set (unspec:DI [(match_operand:SI 0 "metag_reg_nofloat_op" "=da")
                    (match_operand:SI 1 "metag_reg_nofloat_op" "=da")] UNSPEC_CONCAT)
                    (match_operand:DI 2 "memory_operand"        "m"))]
  ""
  "GETL\\t%0, %1, %2"
  [(set_attr "type" "load")])

(define_insn "*lod_di_off12"
  [(set (match_operand:DI                  0 "metag_reg_nofloat_op" "=da,da")
        (mem:DI (plus:SI (match_operand:SI 1 "metag_reg12bit_op"     "da,Yr")
                         (match_operand:SI 2 "metag_offset12_di"     "O8,Z8"))))]
  ""
  "GETL\\t%0, %t0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_di_off6"
  [(set (match_operand:DI                  0 "metag_register_op"    "=cr")
        (mem:DI (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                         (match_operand:SI 2 "metag_offset6_di"      "O8"))))]
  ""
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1+%2]";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_di_mem"
  [(set (match_operand:DI 0 "metag_reg_nofloat_op" "=da")
        (match_operand:DI 1 "memory_operand"        "m"))]
  ""
  "GETL\\t%0, %t0, %1\\t%@ (*lod di rm)"
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; movdf is made up of many parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "movdf"
  [(set (match_operand:DF 0 "nonimmediate_operand" "")
        (match_operand:DF 1 "general_operand"      ""))]
  ""
  {
    if (MEM_P (operands[0]))
      {
        /* All except mem = const or mem = mem can be done quickly */
        operands[1] = force_reg (DFmode, operands[1]);
      }

    if (metag_fpu_single
        && REG_P (operands[0])
        && METAG_FPC_REG_P (REGNO (operands[0]))
        && CONST_DOUBLE_P (operands[1]))
      FAIL;
  }
)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; and these are the movdf parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;

;; NOTE "move df" needs to handle r <- r along with r <- m and m <- r variants together.
;; Otherwise one can end up with a "impossible reload" error if an argument to inline asm
;; fails to be assigned a register and thus needs to be fixed up by reload. This isn't
;; well documented and the underlying issue can go undetected.

;; movdf - register to register forms

;; this variant mov_df is used if generated by reload and needs to handle memory
(define_insn "*mov_df_reload"
  [(set (match_operand:DF 0 "metag_move_dst_op" "=cx,d, cx,d,a,da, cr,!*m")
        (match_operand:DF 1 "metag_move_src_op"  "cx,cx,d, d,a,da,!*m, cr"))]
  "reload_in_progress || reload_completed"
  {
    switch (which_alternative)
      {
      case 0:
        if (!metag_fpu_single)
          return "FL\\tMOV%?\\t%0,%1";
        else
          return "#";
      case 1:
      case 2:
        return "F\\tMOVL\\t%0,%1";
      case 3:
        if (TARGET_DSP)
          return "DL\\tMOV\\t%0, %1";
        /* Fall through */
      case 4:
      case 5:
        return "#";
      case 6:
        if (METAG_FPC_REG_P (REGNO (operands[0])))
          return "F\\tGETL\\t%0,%t0,%1";
        else
          return "GETL\\t%0,%t0,%1";
      case 7:
        if (METAG_FPC_REG_P (REGNO (operands[1])))
          return "F\\tSETL\\t%0,%1,%t1";
        else
          return "SETL\\t%0,%1,%t1";
      default:
        gcc_unreachable();
      }
  }
  [(set_attr "type" "fast,fast,fast,two,two,slowslow,load,slow")
   (set_attr "cond" "yes,no,no,yes,yes,yes,no,no")
   (set_attr "memaccess" "none,none,none,none,none,none,load,store")
   (set_attr "predicable" "no")])

;; this variant mov_df is used if NOT generated by reload and doesn't handle memory
(define_insn_and_split "*mov_df"
  [(set (match_operand:DF 0 "metag_register_op" "=cx,d, cx,d,a,da")
        (match_operand:DF 1 "metag_register_op"  "cx,cx,d, d,a,da"))]
  ""
  {
    switch (which_alternative)
      {
      case 0:
        if (!metag_fpu_single)
          return "FL\\tMOV%?\\t%0,%1";
        else
          return "#";
      case 1:
      case 2:
        return "F\\tMOVL\\t%0,%1";
      case 3:
        if (TARGET_DSP)
          return "DL\\tMOV\\t%0, %1";
        /* Fall through */
      case 4:
      case 5:
        return "#";
      default:
        gcc_unreachable();
      }
  }
  "reload_completed"
  [(const_int 0)]
  {
    metag_split_movdf (operands);
    DONE;
  }
  [(set_attr "type" "fast,fast,fast,two,two,slowslow")
   (set_attr "cond" "yes,no,no,yes,yes,yes")
   (set_attr "predicable" "no")])

;; movdf - immediate to register forms
(define_insn_and_split "*set_df"
  [(set (match_operand:DF 0 "metag_register_op" "=d,a,cx")
        (match_operand:DF 1 "immediate_operand"  "i,i,ci"))]
  "!metag_fpu_single"
  {
    switch (which_alternative)
      {
      case 0:
      case 1:
        return "#";
      case 2:
          return "FD\\tMOV\\t%0,#%h1";
      default:
        gcc_unreachable();
      }
  }
  "&& reload_completed
   && (!METAG_FPC_REG_P (REGNO (operands[0]))
       || !metag_fphalf_imm_op (operands[1], DFmode))"
  [(const_int 0)]
  {
    metag_split_movdf_immediate (operands);
    DONE;
  }
  [(set_attr "type" "four")])

(define_insn_and_split "*set_df_fpu_single"
  [(set (match_operand:DF 0 "metag_register_op" "=d,a")
        (match_operand:DF 1 "immediate_operand"  "i,i"))]
  "metag_fpu_single"
  "#"
  "&& reload_completed"
  [(const_int 0)]
  {
    metag_split_movdf_immediate (operands);
    DONE;
  }
  [(set_attr "type" "four")])

;; -----------------------------------------------------------------------------

(define_insn "*sto_df_post_inc"
  [(set (mem:DF (post_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:DF                      1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0++], %1, %t1\\t%@ (*store DF post_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])
  
(define_insn "*sto_df_post_dec"
  [(set (mem:DF (post_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:DF                      1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0--], %1, %t1\\t%@ (*store DF post_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_df_pre_inc"
  [(set (mem:DF (pre_inc:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:DF                     1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[++%0], %1, %t1\\t%@ (*store DF pre_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])
  
(define_insn "*sto_df_pre_dec"
  [(set (mem:DF (pre_dec:SI (match_operand:SI 0 "metag_reg_nofloat_op" "+da")))
        (match_operand:DF                     1 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[--%0], %1, %t1\\t%@ (*store DF pre_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_df_post_modify_disp"
  [(set (mem:DF (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+d, a")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_df"      "O8,O8"))))
        (match_operand:DF                     2 "metag_register_op"     "a, d"))]
  "!TARGET_METAC_1_1"
  "SETL\\t[%0+%1++], %2, %t2\\t%@ (*store DF post_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_df_post_modify_disp_1_1"
  [(set (mem:DF (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_df"      "O8"))))
        (match_operand:DF                     2 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0+%1++], %2, %t2\\t%@ (*store DF post_modify_disp_1_1 OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_df_post_modify_reg"
  [(set (mem:DF (post_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:DF                     2 "metag_register_op"     "ca,ca,cd,cd"))]
  ""
  {
    static const char fmt[] = "F\\tSETL\\t[%0+%1++], %2, %t2\\t%@ (*store DF post_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_df_pre_modify_disp"
  [(set (mem:DF (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+d, a")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_df"      "O8,O8"))))
        (match_operand:DF                     2 "metag_reg_nofloat_op"  "a, d"))]
  "!TARGET_METAC_1_1"
  "SETL\\t[%0++%1], %2, %t2\\t%@ (*store DF pre_modify_disp OK)"
  [(set_attr "type" "fast")])

(define_insn "*sto_df_pre_modify_disp_1_1"
  [(set (mem:DF (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+da")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_offset6_df"      "O8"))))
        (match_operand:DF                     2 "metag_register_op"     "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0++%1], %2, %t2\\t%@ (*store DF pre_modify_disp_1_1 OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

(define_insn "*sto_df_pre_modify_reg"
  [(set (mem:DF (pre_modify:SI
                   (match_operand:SI          0 "metag_reg_nofloat_op" "+e, f, h, l")
                   (plus:SI (match_dup 0)
                            (match_operand:SI 1 "metag_reg_nofloat_op"  "e, f, h, l"))))
        (match_operand:DF                     2 "metag_register_op"     "ca,ca,cd,cd"))]
  ""
  {
    static const char fmt[] = "F\\tSETL\\t[%0++%1], %2, %t2\\t%@ (*store DF pre_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

;; -----------------------------------------------------------------------------

;; movdf - register to memory forms (stores)
(define_insn "*sto_df_reg_indirect"
  [(set (mem:DF (match_operand:SI 0 "metag_reg_nofloat_op" "a,d,ad,!da"))
        (match_operand:DF         1 "metag_register_op"    "d,a,cx, da"))]
  "TARGET_COND_EXEC_OPTIMIZE
   && !TARGET_METAC_1_1
   && reload_completed
   && !(  metag_regno_same_unit_p (REGNO (operands[0]), REGNO (operands[1]))
       || metag_regno_same_unit_p (REGNO (operands[0]), REGNO (operands[1]) + 1))"
  {
    static const char fmt[] = "F\\tSETL%?\\t[%0], %1, %t1\\t%@ (*sto df OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_df"
  [(set (match_operand:DF 0 "memory_operand"    "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:DF 1 "metag_register_op"  "da,a, a, d, d, !*da"))]
  "!TARGET_METAC_1_1 && !reload_completed"
  "SETL\\t%0, %1, %t1"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_df_postreload"
  [(set (match_operand:DF 0 "memory_operand"    "=Tr,Te,Tf,Th,Tl")
        (match_operand:DF 1 "metag_register_op"  "da,a, a, d, d"))]
  "!TARGET_METAC_1_1 && reload_completed"
  "SETL\\t%0, %1, %t1"
  [(set_attr "type" "fast")])

(define_insn "*sto_df_1_1_reg_indirect"
  [(set (mem:DF (match_operand:SI 0 "metag_reg_nofloat_op" "da"))
        (match_operand:DF         1 "metag_register_op"    "cr"))]
  "TARGET_COND_EXEC_OPTIMIZE && TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL%?\\t[%0], %1, %t1\\t%@ (*sto df [r]r OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[1])) ? 0 : 2];
  }
  [(set_attr "type" "fast")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*sto_df_1_1_off12"
  [(set (mem:DF (plus:SI (match_operand:SI 0 "metag_reg12bit_op"    "da,Yr")
                         (match_operand:SI 1 "metag_offset12_df"    "O8,Z8")))
        (match_operand:DF                  2 "metag_reg_nofloat_op" "da,da"))]
  "TARGET_METAC_1_1"
  "SETL\\t[%0+%1], %2, %t2"
  [(set_attr "type" "fast")])

(define_insn "*sto_df_1_1_off6"
  [(set (mem:DF (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "da")
                         (match_operand:SI 1 "metag_offset6_df"     "O8")))
        (match_operand:DF                  2 "metag_register_op"    "cr"))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tSETL\\t[%0+%1], %2, %t2";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[2])) ? 0 : 2];
  }
  [(set_attr "type" "fast")])

;; movdf - register to memory forms (stores)
(define_insn "*sto_df_1_1"
  [(set (match_operand:DF 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl,!*m")
        (match_operand:DF 1 "metag_reg_nofloat_op"  "da, a, a, d, d,!*da"))]
  "TARGET_METAC_1_1 && !reload_completed"
  "SETL\\t%0, %1, %t1\\t\\t%@ (*sto df [r]r OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,invalid")])

(define_insn "*sto_df_1_1_postreload"
  [(set (match_operand:DF 0 "memory_operand"       "=Tr,Te,Tf,Th,Tl")
        (match_operand:DF 1 "metag_reg_nofloat_op"  "da, a, a, d, d"))]
  "TARGET_METAC_1_1 && reload_completed"
  "SETL\\t%0, %1, %t1\\t\\t%@ (*sto df [r]r OK)"
  [(set_attr "type" "fast")])

;; spilldf - register to memory (stores) from source/dest in same bank
(define_split
  [(set (match_operand:DF 0 "memory_operand"    "")
        (match_operand:DF 1 "metag_register_op" ""))]
  "!TARGET_METAC_1_1
   && reload_completed
   && metag_slow_store (operands[0], operands[1])"
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 0)
        (match_dup 2))]
  {
    operands[2] = metag_gen_safe_temp (DFmode, operands[1]);
  }
)

;; -----------------------------------------------------------------------------

;; movdf - memory to register (loads)

;; -----------------------------------------------------------------------------
;; | Matching DF store [post/pre]_[inc/dec/modify]
;; -----------------------------------------------------------------------------

(define_insn "*lod_df_post_inc"
  [(set (match_operand:DF                      0 "metag_register_op"    "=da,cx")
        (mem:DF (post_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da,da"))))]
  "TARGET_METAC_1_1"
  "@
  GETL\\t%0, %t0, [%1++]\\t%@ (*load DF post_inc OK)
  F\\tGETL\\t%0, %t0, [%1++]\\t%@ (*load DF post_inc OK)"
  [(set_attr "type" "load")])
 
(define_insn "*lod_df_post_dec"
  [(set (match_operand:DF                      0 "metag_register_op"    "=da,cx")
        (mem:DF (post_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da,da"))))]
  "TARGET_METAC_1_1"
  "@
  GETL\\t%0, %t0, [%1--]\\t%@ (*load DF post_dec OK)
  F\\tGETL\\t%0, %t0, [%1--]\\t%@ (*load DF post_dec OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_df_pre_inc"
  [(set (match_operand:DF                     0 "metag_register_op"    "=da,cx")
        (mem:DF (pre_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da,da"))))]
  "TARGET_METAC_1_1"
  "@
  GETL\\t%0, %t0, [++%1]\\t%@ (*load DF pre_inc OK)
  F\\tGETL\\t%0, %t0, [++%1]\\t%@ (*load DF pre_inc OK)"
  [(set_attr "type" "load")])
 
(define_insn "*lod_df_pre_dec"
  [(set (match_operand:DF                     0 "metag_register_op"    "=da,cx")
        (mem:DF (pre_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da,da"))))]
  "TARGET_METAC_1_1"
  "@
  GETL\\t%0, %t0, [--%1]\\t%@ (*load DF pre_dec OK)
  F\\tGETL\\t%0, %t0, [--%1]\\t%@ (*load DF pre_dec OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_df_post_modify_disp"
  [(set (match_operand:DF                     0 "metag_register_op"    "=da,cx")
        (mem:DF (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da,da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_df"      "O8,O8")))))]
  ""
  "@
  GETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_disp OK)
  F\\tGETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_df_post_modify_reg"
  [(set (match_operand:DF                     0 "metag_register_op"    "=da,da,da,da,cx,cx,cx,cx")
        (mem:DF (post_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l, e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l, e, f, h, l")))))]
  ""
  "@
  GETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_reg OK)
  GETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_reg OK)
  GETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_reg OK)
  GETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_reg OK)
  F\\tGETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_reg OK)
  F\\tGETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_reg OK)
  F\\tGETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_reg OK)
  F\\tGETL\\t%0, %t0, [%1+%2++]\\t%@ (*load DF post_modify_reg OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_df_pre_modify_disp"
  [(set (match_operand:DF                     0 "metag_register_op"    "=da,cx")
        (mem:DF (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+da,da")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_offset6_df"      "O8,O8")))))]
  ""
  "@
  GETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_disp OK)
  F\\tGETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lod_df_pre_modify_reg"
  [(set (match_operand:DF                     0 "metag_register_op"    "=da,da,da,da,cx,cx,cx,cx")
        (mem:DF (pre_modify:SI
                   (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l, e, f, h, l")
                   (plus:SI (match_dup 1)
                            (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l, e, f, h, l")))))]
  ""
  "@
  GETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_reg OK)
  GETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_reg OK)
  GETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_reg OK)
  GETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_reg OK)
  F\\tGETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_reg OK)
  F\\tGETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_reg OK)
  F\\tGETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_reg OK)
  F\\tGETL\\t%0, %t0, [%1++%2]\\t%@ (*load DF pre_modify_reg OK)"
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

(define_insn "*lod_df_concat"
  [(set (unspec:DF [(match_operand:SI 0 "metag_reg_nofloat_op" "=da")
                    (match_operand:SI 1 "metag_reg_nofloat_op" "=da")] UNSPEC_CONCAT)
                    (match_operand:DF 2 "memory_operand"        "m"))]
  ""
  "GETL\\t%0, %1, %2"
  [(set_attr "type" "load")])

(define_insn "*lod_df_off12"
  [(set (match_operand:DF                  0 "metag_reg_nofloat_op" "=da,da")
        (mem:DF (plus:SI (match_operand:SI 1 "metag_reg12bit_op"     "da,Yr")
                         (match_operand:SI 2 "metag_offset12_df"     "O8,Z8"))))]
  ""
  "GETL\\t%0, %t0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_df_off6"
  [(set (match_operand:DF                  0 "metag_register_op"    "=da,cx")
        (mem:DF (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da,da")
                         (match_operand:SI 2 "metag_offset6_df"      "O8,O8"))))]
  ""
  "@
  GETL\\t%0, %t0, [%1+%2]
  F\\tGETL\\t%0, %t0, [%1+%2]"
  [(set_attr "type" "load")])

(define_insn "*lod_df"
  [(set (match_operand:DF         0 "metag_register_op"    "=cr")
        (mem:DF (match_operand:SI 1 "metag_reg_nofloat_op"  "da")))]
  ""
  {
    static const char fmt[] = "F\\tGETL\\t%0, %t0, [%1]";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lod_df_mem"
  [(set (match_operand:DF 0 "metag_reg_nofloat_op" "=da")
        (match_operand:DF 1 "memory_operand"        "m"))]
  ""
  "GETL\\t%0, %t0, %1\\t%@ (*lod df rm OK)"
  [(set_attr "type" "load")])

;; Memory bloc xfer insn

(define_expand "movmemqi"
  [(match_operand:BLK 0 "general_operand"   "")
   (match_operand:BLK 1 "general_operand"   "")
   (match_operand:SI  2 "const_int_operand" "")
   (match_operand:SI  3 "const_int_operand" "")]
  ""
  {
    if (metag_gen_movmemqi (operands))
      DONE;
    FAIL;
  }
)

;; Transfer instructions 64-bit and 32-bit

;; loads DImode

(define_insn "*ldmdi_7"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 56)))
     (set (match_operand:DI          3 "metag_hard_datareg_op" "=d")
          (mem:DI (match_dup 2)))
     (set (match_operand:DI          4 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 8))))
     (set (match_operand:DI          5 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 16))))
     (set (match_operand:DI          6 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 24))))
     (set (match_operand:DI          7 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 32))))
     (set (match_operand:DI          8 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 40))))
     (set (match_operand:DI          9 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 48))))])]
  "XVECLEN (operands[0], 0) == 7 + 1"
  "MGETL\\t%3, %4, %5, %6, %7, %8, %9, [%1++]\\t%@ (lodmr di OK)"
  [(set_attr "type"      "fivex")
   (set_attr "memaccess" "load")
   (set_attr "rename"    "no")])

(define_insn "*ldmdi_6"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 48)))
     (set (match_operand:DI          3 "metag_hard_datareg_op" "=d")
          (mem:DI (match_dup 2)))
     (set (match_operand:DI          4 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 8))))
     (set (match_operand:DI          5 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 16))))
     (set (match_operand:DI          6 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 24))))
     (set (match_operand:DI          7 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 32))))
     (set (match_operand:DI          8 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 40))))])]
  "XVECLEN (operands[0], 0) == 6 + 1"
  "MGETL\\t%3, %4, %5, %6, %7, %8, [%1++]\\t%@ (lodmr di OK)"
  [(set_attr "type"      "fivex")
   (set_attr "memaccess" "load")
   (set_attr "rename"    "no")])

(define_insn "*ldmdi_5"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 40)))
     (set (match_operand:DI          3 "metag_hard_datareg_op" "=d")
          (mem:DI (match_dup 2)))
     (set (match_operand:DI          4 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 8))))
     (set (match_operand:DI          5 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 16))))
     (set (match_operand:DI          6 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 24))))
     (set (match_operand:DI          7 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 32))))])]
  "XVECLEN (operands[0], 0) == 5 + 1"
  "MGETL\\t%3, %4, %5, %6, %7, [%1++]\\t%@ (lodmr di OK)"
  [(set_attr "type" "fivex")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

(define_insn "*ldmdi_4"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 32)))
     (set (match_operand:DI          3 "metag_hard_datareg_op" "=d")
          (mem:DI (match_dup 2)))
     (set (match_operand:DI          4 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 8))))
     (set (match_operand:DI          5 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 16))))
     (set (match_operand:DI          6 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 24))))])]
  "XVECLEN (operands[0], 0) == 4 + 1"
  "MGETL\\t%3, %4, %5, %6, [%1++]\\t%@ (lodmr di OK)"
  [(set_attr "type" "fourx")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

(define_insn "*ldmdi_3"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 24)))
     (set (match_operand:DI          3 "metag_hard_datareg_op" "=d")
          (mem:DI (match_dup 2)))
     (set (match_operand:DI          4 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 8))))
     (set (match_operand:DI          5 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 16))))])]
  "XVECLEN (operands[0], 0) == 3 + 1"
  "MGETL\\t%3, %4, %5, [%1++]\\t%@ (lodmr di OK)"
  [(set_attr "type" "threex")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

(define_insn "*ldmdi_2"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 16)))
     (set (match_operand:DI          3 "metag_hard_datareg_op" "=d")
          (mem:DI (match_dup 2)))
     (set (match_operand:DI          4 "metag_hard_datareg_op" "=d")
          (mem:DI (plus:SI (match_dup 2)
                           (const_int 8))))])]
  "XVECLEN (operands[0], 0) == 2 + 1"
  "MGETL\\t%3, %4, [%1++]\\t%@ (lodmr di OK)"
  [(set_attr "type" "twox")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

;; Loads SImode

(define_insn "*ldmsi_7"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 28)))
     (set (match_operand:SI          3 "metag_hard_datareg_op" "=d")
          (mem:SI (match_dup 2)))
     (set (match_operand:SI          4 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 4))))
     (set (match_operand:SI          5 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 8))))
     (set (match_operand:SI          6 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 12))))
     (set (match_operand:SI          7 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 16))))
     (set (match_operand:SI          8 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 20))))
     (set (match_operand:SI          9 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 24))))])]
  "XVECLEN (operands[0], 0) == 7 + 1"
  "MGETD\\t%3, %4, %5, %6, %7, %8, %9, [%1]\\t%@ (lodmr si OK)"
  [(set_attr "type" "fivex")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

(define_insn "*ldmsi_6"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 24)))
     (set (match_operand:SI          3 "metag_hard_datareg_op" "=d")
          (mem:SI (match_dup 2)))
     (set (match_operand:SI          4 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 4))))
     (set (match_operand:SI          5 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 8))))
     (set (match_operand:SI          6 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 12))))
     (set (match_operand:SI          7 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 16))))
     (set (match_operand:SI          8 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 20))))])]
  "XVECLEN (operands[0], 0) == 6 + 1"
  "MGETD\\t%3, %4, %5, %6, %7, [%1]\\t%@ (lodmr si OK)"
  [(set_attr "type" "fivex")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

(define_insn "*ldmsi_5"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 20)))
     (set (match_operand:SI          3 "metag_hard_datareg_op" "=d")
          (mem:SI (match_dup 2)))
     (set (match_operand:SI          4 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 4))))
     (set (match_operand:SI          5 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 8))))
     (set (match_operand:SI          6 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 12))))
     (set (match_operand:SI          7 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 16))))])]
  "XVECLEN (operands[0], 0) == 5 + 1"
  "MGETD\\t%3, %4, %5, %6, %7, [%1]\\t%@ (lodmr si OK)"
  [(set_attr "type" "fivex")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

(define_insn "*ldmsi_4"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 16)))
     (set (match_operand:SI          3 "metag_hard_datareg_op" "=d")
          (mem:SI (match_dup 2)))
     (set (match_operand:SI          4 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 4))))
     (set (match_operand:SI          5 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 8))))
     (set (match_operand:SI          6 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 12))))])]
  "XVECLEN (operands[0], 0) == 4 + 1"
  "MGETD\\t%3, %4, %5, %6, [%1]\\t%@ (lodmr si OK)"
  [(set_attr "type" "fourx")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

(define_insn "*ldmsi_3"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 12)))
     (set (match_operand:SI          3 "metag_hard_datareg_op" "=d")
          (mem:SI (match_dup 2)))
     (set (match_operand:SI          4 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 4))))
     (set (match_operand:SI          5 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 8))))])]
  "XVECLEN (operands[0], 0) == 3 + 1"
  "MGETD\\t%3, %4, %5, [%1]\\t%@ (lodmr si OK)"
  [(set_attr "type" "threex")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

(define_insn "*ldmsi_2"
  [(match_parallel                   0 "load_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 8)))
     (set (match_operand:SI          3 "metag_hard_datareg_op" "=d")
          (mem:SI (match_dup 2)))
     (set (match_operand:SI          4 "metag_hard_datareg_op" "=d")
          (mem:SI (plus:SI (match_dup 2)
                           (const_int 4))))])]
  "XVECLEN (operands[0], 0) == 2 + 1"
  "MGETD\\t%3, %4, [%1]\\t%@ (lodmr si OK)"
  [(set_attr "type" "twox")
   (set_attr "memaccess" "load")
   (set_attr "rename" "no")])

;; stores DImode
(define_insn "*stmdi_8"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 64)))
     (set (mem:DI (match_dup 2))
          (match_operand:DI          3  "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:DI          4  "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 16)))
          (match_operand:DI          5  "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 24)))
          (match_operand:DI          6  "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 32)))
          (match_operand:DI          7  "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 40)))
          (match_operand:DI          8  "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 48)))
          (match_operand:DI          9  "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 56)))
          (match_operand:DI          10 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 8 + 1"
  "MSETL\\t[%1++], %3, %4, %5, %6, %7, %8, %9, %10\\t%@ (*stomr di OK)"
  [(set_attr "type" "fivex")
   (set_attr "memaccess" "store")
   (set_attr "rename" "no")])

(define_insn "*stmdi_7"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 56)))
     (set (mem:DI (match_dup 2))
          (match_operand:DI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:DI          4 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 16)))
          (match_operand:DI          5 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 24)))
          (match_operand:DI          6 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 32)))
          (match_operand:DI          7 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 40)))
          (match_operand:DI          8 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 48)))
          (match_operand:DI          9 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 7 + 1"
  "MSETL\\t[%1++], %3, %4, %5, %6, %7, %8, %9\\t%@ (*stomr di OK)"
  [(set_attr "type" "fivex")
   (set_attr "memaccess" "store")
   (set_attr "rename" "no")])

(define_insn "*stmdi_6"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 48)))
     (set (mem:DI (match_dup 2))
          (match_operand:DI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:DI          4 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 16)))
          (match_operand:DI          5 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 24)))
          (match_operand:DI          6 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 32)))
          (match_operand:DI          7 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 40)))
          (match_operand:DI          8 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 6 + 1"
  "MSETL\\t[%1++], %3, %4, %5, %6, %7, %8\\t%@ (*stomr di OK)"
  [(set_attr "type"      "fivex")
   (set_attr "memaccess" "store")
   (set_attr "rename"    "no")])

(define_insn "*stmdi_5"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 40)))
     (set (mem:DI (match_dup 2))
          (match_operand:DI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:DI          4 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 16)))
          (match_operand:DI          5 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 24)))
          (match_operand:DI          6 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 32)))
          (match_operand:DI          7 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 5 + 1"
  "MSETL\\t[%1++], %3, %4, %5, %6, %7\\t%@ (*stomr di OK)"
  [(set_attr "type"      "fivex")
   (set_attr "memaccess" "store")
   (set_attr "rename"    "no")])

(define_insn "*stmdi_4"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 32)))
     (set (mem:DI (match_dup 2))
          (match_operand:DI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:DI          4 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 16)))
          (match_operand:DI          5 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 24)))
          (match_operand:DI          6 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 4 + 1"
  "MSETL\\t[%1++], %3, %4, %5, %6\\t%@ (*stomr di OK)"
  [(set_attr "type" "fourx")
   (set_attr "memaccess" "store")
   (set_attr "rename" "no")])

(define_insn "*stmdi_3"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 24)))
     (set (mem:DI (match_dup 2))
          (match_operand:DI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:DI          4 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 16)))
          (match_operand:DI          5 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 3 + 1"
  "MSETL\\t[%1++], %3, %4, %5\\t%@ (*stomr di OK)"
  [(set_attr "type" "threex")
   (set_attr "memaccess" "store")
   (set_attr "rename" "no")])

(define_insn "*stmdi_2"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_reg_nofloat_op"  "=da")
          (plus:SI (match_operand:SI 2 "metag_reg_nofloat_op"   "1")
                   (const_int 16)))
     (set (mem:DI (match_dup 2))
          (match_operand:DI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:DI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:DI          4 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 2 + 1"
  "MSETL\\t[%1++], %3, %4\\t%@ (*stomr di OK)"
  [(set_attr "type" "twox")
   (set_attr "memaccess" "store")
   (set_attr "rename" "no")])

;; Stores SImode

(define_insn "*stmsi_5"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_addrreg_op"      "=a")
          (plus:SI (match_operand:SI 2 "metag_addrreg_op"       "1")
                   (const_int 20)))
     (set (mem:SI (match_dup 2))
          (match_operand:SI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 4)))
          (match_operand:SI          4 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:SI          5 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 12)))
          (match_operand:SI          6 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 16)))
          (match_operand:SI          7 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 5 + 1"
  "MSETD\\t[%1], %3, %4, %5, %6, %7\\t%@ (*stomr si OK)"
  [(set_attr "type" "fivex")
   (set_attr "memaccess" "store")
   (set_attr "rename" "no")])

(define_insn "*stmsi_4"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_addrreg_op"      "=a")
          (plus:SI (match_operand:SI 2 "metag_addrreg_op"       "1")
                   (const_int 16)))
     (set (mem:SI (match_dup 2))
          (match_operand:SI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 4)))
          (match_operand:SI          4 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:SI          5 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 12)))
          (match_operand:SI          6 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 4 + 1"
  "MSETD\\t[%1], %3, %4, %5, %6\\t%@ (*stomr si OK)"
  [(set_attr "type" "fourx")
   (set_attr "memaccess" "store")
   (set_attr "rename" "no")])

(define_insn "*stmsi_3"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_addrreg_op"      "=a")
          (plus:SI (match_operand:SI 2 "metag_addrreg_op"       "1")
                   (const_int 12)))
     (set (mem:SI (match_dup 2))
          (match_operand:SI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 4)))
          (match_operand:SI          4 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 8)))
          (match_operand:SI          5 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 3 + 1"
  "MSETD\\t[%1], %3, %4, %5\\t%@ (*stomr si OK)"
  [(set_attr "type" "threex")
   (set_attr "memaccess" "store")
   (set_attr "rename" "no")])

(define_insn "*stmsi_2"
  [(match_parallel                   0 "store_multiop"
    [(set (match_operand:SI          1 "metag_addrreg_op"      "=a")
          (plus:SI (match_operand:SI 2 "metag_addrreg_op"       "1")
                   (const_int 8)))
     (set (mem:SI (match_dup 2))
          (match_operand:SI          3 "metag_hard_datareg_op"  "d"))
     (set (mem:SI (plus:SI (match_dup 2)
                           (const_int 4)))
          (match_operand:SI          4 "metag_hard_datareg_op"  "d"))])]
  "XVECLEN (operands[0], 0) == 2 + 1"
  "MSETD\\t[%1], %3, %4\\t%@ (*stomr si OK)"
  [(set_attr "type" "twox")
   (set_attr "memaccess" "store")
   (set_attr "rename" "no")])

;; add instructions

;; Addition insns.

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; adddi3                             ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "adddi3"
  [(parallel
    [(set (match_operand:DI          0 "metag_reg_nofloat_op" "")
          (plus:DI (match_operand:DI 1 "metag_reg_nofloat_op" "")
                   (match_operand:DI 2 "metag_reg_nofloat_op" "")))
     (clobber (reg:CC CC_REG))])]
  ""
  "")

(define_insn "*adddi3_dsp"
  [(set (match_operand:DI          0 "metag_datareg_op" "=d")
        (plus:DI (match_operand:DI 1 "metag_datareg_op" "%d")
                 (match_operand:DI 2 "metag_datareg_op"  "d")))
   (clobber (reg:CC CC_REG))]
  "TARGET_DSP"
  "DL\\tADDS\\t%0, %1, %2\\t%@ (*ADD\\t%t0, %t1, %t2)\;ADDCS\\t%t0, %t0, #1"
  [(set_attr "type" "two")
   (set_attr "ccstate" "ccx")])

(define_insn_and_split "*adddi3"
  [(set (match_operand:DI          0 "metag_datareg_op" "=d")
        (plus:DI (match_operand:DI 1 "metag_datareg_op" "%d")
                 (match_operand:DI 2 "metag_datareg_op"  "d")))
   (clobber (reg:CC CC_REG))]
  ""
  "#"
  "SPLIT_EARLY"
  [(parallel
     [(set (reg:CC_NOOV CC_REG)
           (compare:CC_NOOV
             (plus:SI (match_dup 5)
                      (match_dup 7))
             (const_int 0)))
      (set (match_dup 3)
           (plus:SI (match_dup 5)
                    (match_dup 7)))])
   (set (match_dup 4)
        (plus:SI (match_dup 6)
                 (match_dup 8)))
   (set (match_dup 4)
        (if_then_else:SI (ltu (reg:CC_NOOV CC_REG)
                              (const_int 0))
          (plus:SI (match_dup 4)
                   (const_int 1))
          (match_dup 4)))]
  {
    if (reload_completed)
      {
        operands[3] = gen_rtx_REG (SImode, REGNO (operands[0]));
        operands[4] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);

        operands[5] = gen_rtx_REG (SImode, REGNO (operands[1]));
        operands[6] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);

        operands[7] = gen_rtx_REG (SImode, REGNO (operands[2]));
        operands[8] = gen_rtx_REG (SImode, REGNO (operands[2]) + 1);
      }
    else
      {
        operands[3] = simplify_gen_subreg (SImode, operands[0], DImode, 0);
        operands[4] = simplify_gen_subreg (SImode, operands[0], DImode, UNITS_PER_WORD);

        operands[5] = simplify_gen_subreg (SImode, operands[1], DImode, 0);
        operands[6] = simplify_gen_subreg (SImode, operands[1], DImode, UNITS_PER_WORD);

        operands[7] = simplify_gen_subreg (SImode, operands[2], DImode, 0);
        operands[8] = simplify_gen_subreg (SImode, operands[2], DImode, UNITS_PER_WORD);
      }
  }
  [(set_attr "type" "three")
   (set_attr "ccstate" "ccx")])

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; addsi3 is made up of many parts..  ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "addsi3"
  [(set (match_operand:SI          0 "metag_register_op" "")
        (plus:SI (match_operand:SI 1 "metag_register_op" "")
                 (match_operand:SI 2 "metag_regorint_op" "")))]
  ""
  {
    if (metag_frame_related_rtx (operands[2]))
      {
        /* Put the magic frame registers first */
        rtx temp = operands[1];

        operands[1] = operands[2];
        operands[2] = temp;
      }

    if (!reload_completed
        && !reload_in_progress
        && metag_frame_related_rtx (operands[1]))
      {
        /* Ensure reg+reg adds do not combine regs that may be eliminated */
        operands[1] = force_reg (SImode, operands[1]);
      }
  }
)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; these are the addsi3 parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;

;; control register = datareg + smallint
(define_insn "*add_si_cdi"
  [(set (match_operand:SI          0 "metag_txrpt_op"    "=Wx,Wx")
        (plus:SI (match_operand:SI 1 "metag_datareg_op"  "%d, d")
                 (match_operand:SI 2 "metag_smallint_op"  "K, P")))]
  ""
  "@
   ADD\\t%0, %1, %2\\t%@ (*add si cdK OK)
   SUB\\t%0, %1, #%n2\\t%@ (*add si cdP OK)"
  [(set_attr "type" "slow,slow")])

;; control register = reg +/- reg
(define_insn "*sub_si_crr"
  [(set (match_operand:SI           0 "metag_txrpt_op"    "=Wx,Wx,Wx,Wx")
        (minus:SI (match_operand:SI 1 "metag_regnofrm_op"  "e, f, h, l")
                  (match_operand:SI 2 "metag_regnofrm_op"  "e, f, h, l")))]
   ""
   "SUB\\t%0, %1, %2\\t%@ (*sub si crr OK)"
  [(set_attr "type" "slow,slow,slow,slow")])

(define_insn "*add_si_crr"
  [(set (match_operand:SI          0 "metag_txrpt_op"    "=Wx,Wx,Wx,Wx")
        (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "%e, f, h, l")
                 (match_operand:SI 2 "metag_regnofrm_op"  "e, f, h, l")))]
  ""
  "ADD\\t%0, %1, %2\\t%@ (*add si crr OK)"
  [(set_attr "type" "slow,slow,slow,slow")])

;; register + register ops - generic case
(define_insn "*add_si_rrr"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=e,f,h,l,da,da,da,da")
        (plus:SI (match_operand:SI 1 "metag_regnofrm_op"    "%e,f,h,l,e, f, h, l")
                 (match_operand:SI 2 "metag_regnofrm_op"     "e,f,h,l,e, f, h, l")))]
  "!TARGET_METAC_1_1"
  "ADD%?\\t%0, %1, %2"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; register + register ops - v1.1 case
(define_insn "*add_si_rrb_1_1"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=e, f, h, l, da,da,da,da")
        (plus:SI (match_operand:SI 1 "metag_regnofrm_op"    "%e, f, h, l, e, f, h, l")
                 (match_operand:SI 2 "metag_regnofrm_op"     "be,bf,bh,bl,be,bf,bh,bl")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "ADD%?\\t%0, %1, %2"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

(define_insn "*add_si_rrb_1_1_if_<mode>"
  [(set (match_operand:SI                           0 "metag_reg_nofloat_op" "=e, f, h, l, da,da,da,da")
        (if_then_else:SI (match_operator            1 "comparison_operator"
                          [(match_operand:CCALL     2 "metag_<mode>_reg" "")
                           (const_int 0)])
                         (plus:SI (match_operand:SI 3 "metag_regnofrm_op"    "%e, f, h, l, e, f, h, l")
                                  (match_operand:SI 4 "metag_regnofrm_op"     "be,bf,bh,bl,be,bf,bh,bl"))
                         (match_operand:SI          5 "metag_reg_nofloat_op"  "0, 0, 0, 0, 0, 0, 0, 0")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "ADD%z1\\t%0, %3, %4"
  [(set_attr "type"    "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "ccstate" "xcc")])

;; register + register ops - minim case
(define_insn "*add_si_rrb_minim"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op"  "=e,!e, f,!f, h,!h, l,!l, da,!da,da,!da,da,!da,da,!da")
        (plus:SI (match_operand:SI 1 "metag_regnofrm_op"     "%e, e, f, f, h, h, l, l, e,  e, f,  f, h,  h, l,  l")
                 (match_operand:SI 2 "metag_regnofrm_op"      "e, be,f, bf,h, bh,l, bl,e,  be,f,  bf,h,  bh,l,  bl")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "ADD%?\\t%0, %1, %2"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,fast,slow,slow,slow,slow,slow,slow,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "*add_si_rrb_minim_if_<mode>"
  [(set (match_operand:SI                           0 "metag_reg_nofloat_op" "=e,!e, f,!f, h,!h, l,!l, da,!da,da,!da,da,!da,da,!da")
        (if_then_else:SI (match_operator            1 "comparison_operator"
                          [(match_operand:CCALL     2 "metag_<mode>_reg" "")
                           (const_int 0)])
                         (plus:SI (match_operand:SI 3 "metag_regnofrm_op"    "%e, e, f, f, h, h, l, l, e,  e, f,  f, h,  h, l,  l")
                                  (match_operand:SI 4 "metag_regnofrm_op"     "e, be,f, bf,h, bh,l, bl,e,  be,f,  bf,h,  bh,l,  bl"))
                         (match_operand:SI          5 "metag_reg_nofloat_op"  "0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 0,  0")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "ADD%z1\\t%0, %3, %4"
  [(set_attr "type"    "fast,fast,fast,fast,fast,fast,fast,fast,slow,slow,slow,slow,slow,slow,slow,slow")
   (set_attr "ccstate" "xcc")])

;; Seperate stack frame related register+register adds
(define_insn "*add_si_index_frame"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=h,da,!da")
        (plus:SI (match_operand:SI 1 "metag_regframe_op"    "%h,h,  h")
                 (match_operand:SI 2 "metag_regnofrm_op"     "h,h,  bh")))]
  "!TARGET_METAC_1_1"
  "ADD%?\\t%0, %1, %2\\t%@ (*add si rfrmh OK)"
  [(set_attr "type" "fast,slow,three")
   (set_attr "cond" "yes,yes,yes")
   (set_attr "predicable" "yes")])

;; Sadly instantiate_virtual_regs can be really dumb some times
(define_insn "*add_si_index_frame2"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=h,da,!da")
        (plus:SI (match_operand:SI 1 "metag_regnofrm_op"    "%h,h,  h")
                 (match_operand:SI 2 "metag_regframe_op"     "h,h,  bh")))]
  "!TARGET_METAC_1_1"
  "ADD%?\\t%0, %1, %2\\t%@ (*add si rhfrm OK)"
  [(set_attr "type" "fast,slow,three")
   (set_attr "cond" "yes,yes,yes")
   (set_attr "predicable" "yes")])

;; Seperate stack frame related register+register adds - v1.1 case
(define_insn "*add_si_index_frame_1_1"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=h, da")
        (plus:SI (match_operand:SI 1 "metag_regframe_op"    "%h, h")
                 (match_operand:SI 2 "metag_regnofrm_op"     "bh,bh")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "ADD%?\\t%0, %1, %2\\t%@ (*add si rfrmb OK)"
  [(set_attr "type" "fast,slow")
   (set_attr "cond" "yes,yes")
   (set_attr "predicable" "yes")])

;; Seperate stack frame related register+register adds - minim case
(define_insn "*add_si_index_frame_minim"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=h,!h, da,!da")
        (plus:SI (match_operand:SI 1 "metag_regframe_op"    "%h, h, h,  h")
                 (match_operand:SI 2 "metag_regnofrm_op"     "h, bh,h,  bh")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "ADD%?\\t%0, %1, %2\\t%@ (*add si rfrmb OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

;; Sadly instantiate_virtual_regs can be really dumb some times - v1.1 case
(define_insn "*add_si_index_frame2_1_1"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=h, da")
        (plus:SI (match_operand:SI 1 "metag_regnofrm_op"    "%h, h")
                 (match_operand:SI 2 "metag_regframe_op"     "bh,bh")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "ADD%?\\t%0, %1, %2\\t%@ (*add si rbfrm OK)"
  [(set_attr "type" "fast,slow")
   (set_attr "cond" "yes,yes")
   (set_attr "predicable" "yes")])

;; Sadly instantiate_virtual_regs can be really dumb some times - minim case
(define_insn "*add_si_index_frame2_minim"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=h,!h, da,!da")
        (plus:SI (match_operand:SI 1 "metag_regnofrm_op"    "%h, h, h,  h")
                 (match_operand:SI 2 "metag_regframe_op"     "h, bh,h,  bh")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "ADD%?\\t%0, %1, %2\\t%@ (*add si rbfrm OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

;; spill_frame - cannot really add frame value to something else
(define_split
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "")
        (plus:SI (match_operand:SI 1 "metag_regframe_op"    "")
                 (match_operand:SI 2 "metag_reg_nofloat_op" "")))]
  "!TARGET_METAC_1_1
   && reload_completed
   && METAG_REGNO_REG_CLASS (REGNO (operands[2])) != A0_REGS"
  [(set (match_dup 3)
        (match_dup 2))
   (set (match_dup 0)
        (plus:SI (match_dup 1)
                 (match_dup 3)))]
  {
    gcc_assert (A0_SCRATCH != INVALID_REGNUM && fixed_regs[A0_SCRATCH]);
    operands[3] = gen_rtx_REG (SImode, A0_SCRATCH);
  }
)

(define_split
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "")
        (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                 (match_operand:SI 2 "metag_regframe_op"    "")))]
  "!TARGET_METAC_1_1
   && reload_completed
   && METAG_REGNO_REG_CLASS (REGNO (operands[1])) != A0_REGS"
  [(set (match_dup 3)
        (match_dup 1))
   (set (match_dup 0)
        (plus:SI (match_dup 3)
                 (match_dup 2)))]
  {
    gcc_assert (A0_SCRATCH != INVALID_REGNUM && fixed_regs[A0_SCRATCH]);
    operands[3] = gen_rtx_REG (SImode, A0_SCRATCH);
  }
)

;; register + immediate ops
(define_insn "*add_si_rri"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=e,e,f,f,h,h,l,l,da,da,da,da,da,da,da,da")
        (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,e,f,f,h,h,l,l,d, d, a, a, 0, 0, 0, 0")
                 (match_operand:SI 2 "const_int_operand"     "K,P,K,P,K,P,K,P,K, P, K, P, I, J, O3,i")))]
  "!TARGET_MINIM_CORE"
  "@
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si eeK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si eeP)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si ffK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si ffP)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si hhK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si hhP OK)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si llK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si llP OK)
   ADD\\t%0, %1, %2\\t\\t%@ (*add si rdK OK)
   SUB\\t%0, %1, #%n2\\t\\t%@ (*add si rdP OK)
   ADD\\t%0, %1, %2\\t\\t%@ (*add si daaK OK)
   SUB\\t%0, %1, #%n2\\t\\t%@ (*add si daaP OK)
   ADD\\t%0, %1, %2\\t%@ (*add si da0I OK)
   ADDT\\t%0, %1, #HI(%c2)\\t%@ (*add si da0J OK)
   ADDT\\t%0, %1, #LO(%c2)\\t%@ (*add si da03 OK)
   #"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,fast,slow,slow,slow,slow,fast,fast,fast,two")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes,no,no,no,no,no,no,no,no")])

;; register + immediate ops
(define_insn "*add_si_rri"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=da,da,!e,!e,!f,!f,!h,!h,!l,!l,da,da,da,da,da,da,da,da")
        (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%0, 0,  e, e, f, f, h, h, l, l,d, d, a, a, 0, 0, 0, 0")
                 (match_operand:SI 2 "const_int_operand"     "K, P,  K, P, K, P, K, P, K, P,K, P, K, P, I, J, O3,i")))]
  "TARGET_MINIM_CORE"
  "@
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si r0K OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si r0P)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si eeK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si eeP)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si ffK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si ffP)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si hhK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si hhP OK)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si llK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si llP OK)
   ADD\\t%0, %1, %2\\t\\t%@ (*add si rdK OK)
   SUB\\t%0, %1, #%n2\\t\\t%@ (*add si rdP OK)
   ADD\\t%0, %1, %2\\t\\t%@ (*add si daaK OK)
   SUB\\t%0, %1, #%n2\\t\\t%@ (*add si daaP OK)
   ADD\\t%0, %1, %2\\t%@ (*add si da0I OK)
   ADDT\\t%0, %1, #HI(%c2)\\t%@ (*add si da0J OK)
   ADDT\\t%0, %1, #LO(%c2)\\t%@ (*add si da03 OK)
   #"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,fast,fast,fast,slow,slow,slow,slow,fast,fast,fast,two")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,no,no,no,no,no,no,no,no")])

;; Split the above add_si_rri if it needs more than one insn
(define_split
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "")
        (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                 (match_operand:SI 2 "const_int_operand"    "")))]
  "reload_completed
   && REGNO (operands[0]) == REGNO (operands[1]) && !METAG_CONST_OK_FOR_LETTERS_KPIJO3 (operands[2])"
  [(set (match_dup 0)
        (plus:SI (match_dup 1)
                 (match_dup 3)))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 4)))]
  {
    HOST_WIDE_INT value = INTVAL (operands[2]);

    operands[3] = gen_int_mode (value & 0xFFFF0000, SImode);

    operands[4] = gen_int_mode (value & 0x0000FFFF, SImode);
  }
)

;; conditional version for specific cases of add_si_rri
(define_insn "*cond_<mode>_add_si_rri"
  [(cond_exec
     (match_operator                 3 "comparison_operator"
       [(match_operand:CCALL         4 "metag_<mode>_reg"       "")
        (const_int 0)])
     (set (match_operand:SI          0 "metag_hard_genreg_op"  "=e,e,f,f,h,h,l,l")
          (plus:SI (match_operand:SI 1 "metag_hard_genreg_op"  "%e,e,f,f,h,h,l,l")
                   (match_operand:SI 2 "metag_KP_operand"       "K,P,K,P,K,P,K,P"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])"
  "@
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si eeK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si eeP)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si ffK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si ffP)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si hhK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si hhP OK)
   ADD%?\\t%0, %1, %2\\t\\t%@ (*add si llK OK)
   SUB%?\\t%0, %1, #%n2\\t\\t%@ (*add si llP OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,fast")
   (set_attr "cond" "no")])

(define_insn_and_split"*adds_si_neg_<mode>_rr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (match_operand:SI         1 "metag_datareg_op"      "e, f, e, f")
          (neg:SI (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf"))))
   (clobber (match_scratch:SI       0                        "=e, f, r, r"))]
  "TARGET_METAC_1_1"
  "#"
  "&& reload_completed"
  [(parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (plus:SI (match_dup 1)
                     (match_dup 2))
            (const_int 0)))
     (set (match_dup 0)
          (plus:SI (match_dup 1)
                   (match_dup 2)))])]
  ""
  [(set_attr "type" "fast")
   (set_attr "ccstate" "set")])

;; register + register|immediate ops that set the flags
(define_insn "*adds_<mode>_si_rrr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "%e,f,e,f")
                   (match_operand:SI 2 "metag_regnofrm_op"  "e,f,e,f"))
          (const_int 0)))
   (set (match_operand:SI            0 "metag_register_op" "=e,f,r,r")
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "!TARGET_METAC_1_1"
  "ADDS\\t%0, %1, %2\\t%@ (*adds si rrr OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

;; register + register|immediate ops that set the flags - v1.1 case
(define_insn "*adds_<mode>_si_rrr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "%e, f, e, f")
                   (match_operand:SI 2 "metag_regnofrm_op"  "be,bf,be,bf"))
          (const_int 0)))
   (set (match_operand:SI            0 "metag_register_op" "=e, f, r, r")
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "TARGET_METAC_1_1"
  "ADDS\\t%0, %1, %2\\t%@ (*adds si rrr OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

(define_insn_and_split "*adds_si_r_<mode>_symglobal_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (match_operand:SI 1 "metag_symglobal_op" "")
          (const_int 0)))
   (set (match_operand:SI   0 "metag_datareg_op" "=d")
        (match_dup 1))]
  "TARGET_METAC_1_1"
  "#"
  "&& reload_completed"
  [(set (match_dup 0)
        (high:SI (match_dup 1)))
   (parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (lo_sum:SI (match_dup 0)
                       (match_dup 1))
            (const_int 0)))
     (set (match_dup 0)
          (lo_sum:SI (match_dup 0)
                     (match_dup 1)))])]
  ""
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_insn "*adds_<mode>_si_txrpt_ri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (plus:SI (match_operand:SI 1 "metag_datareg_op" "%d, d")
                   (match_operand:SI 2 "metag_smallint_op" "K, P"))
          (const_int 0)))
   (set (match_operand:SI            0 "metag_txrpt_op"   "=Wx,Wx")
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "reload_completed"
  "@
   ADDS\\t%0, %1, #%2
   SUBS\\t%0, %1, #%n2"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")])

(define_insn "*adds_<mode>_si_rri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,e,f,f,d,d,0,0,0, 0")
                   (match_operand:SI 2 "const_int_operand"     "K,P,K,P,K,P,I,J,O3,i"))
          (const_int 0)))
   (set (match_operand:SI            0 "metag_register_op"    "=e,e,f,f,r,r,d,d,d, d")
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
 ""
 "@
  ADDS\\t%0, %1, %2\\t\\t%@ (*adds si eeK OK)
  SUBS\\t%0, %1, #%n2\\t\\t%@ (*adds si eeP OK)
  ADDS\\t%0, %1, %2\\t\\t%@ (*adds si ffK OK)
  SUBS\\t%0, %1, #%n2\\t\\t%@ (*adds si ffP OK)
  ADDS\\t%0, %1, %2\\t\\t%@ (*adds si rdK OK)
  SUBS\\t%0, %1, #%n2\\t\\t%@ (*adds si rdP OK)
  ADDS\\t%0, %1, %2\\t%@ (*adds si d0I OK)
  ADDST\\t%0, %1, #HI(%c2)\\t%@ (*adds si d0J OK)
  ADDS\\t%0, %1, #LO(%c2)\\t%@ (*adds si d0O3 OK)
  #"
 [(set_attr "type"    "fast,fast,fast,fast,slow,slow,fast,fast,fast,two")
  (set_attr "ccstate" "set,set,set,set,set,set,set,set,set,fastset")])

;; Split the above insn if it needs more than one insn
(define_split
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                   (match_operand:SI 2 "const_int_operand" ""))
          (const_int 0)))
   (set (match_operand:SI            0 "metag_reg_nofloat_op" "")
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "reload_completed
   && REGNO (operands[0]) == REGNO (operands[1]) && !METAG_CONST_OK_FOR_LETTERS_KPIJO3 (operands[2])"
  [(set (match_dup 0)
        (plus:SI (match_dup 1)
                 (match_dup 3)))
   (parallel 
     [(set (reg:<MODE> CC_REG)
           (compare:<MODE>
             (plus:SI (match_dup 0)
                      (match_dup 4))
             (const_int 0)))
      (set (match_dup 0)
           (plus:SI (match_dup 0)
                    (match_dup 4)))])]
  {
    HOST_WIDE_INT value = INTVAL (operands[2]);

    operands[3] = gen_int_mode (value & 0xFFFF0000, SImode);

    operands[4] = gen_int_mode (value & 0x0000FFFF, SImode);
  }
)

;; register + register|immediate ops that set the flags using a scratch
(define_insn "*tadds_<mode>_si_rrr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (plus:SI (match_operand:SI 1 "metag_regnofrm_op"  "%e,f,e,f")
                   (match_operand:SI 2 "metag_regnofrm_op"   "e,f,e,f"))
          (const_int 0)))
   (clobber (match_scratch:SI        0                      "=e,f,r,r"))]
  "!TARGET_METAC_1_1"
  "ADDS\\t%0, %1, %2\\t%@ (*tadd si rrr OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

;; register + register|immediate ops that set the flags using a scratch - v1.1
(define_insn "*tadds_<mode>_si_rrr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "%e, f, e, f")
                   (match_operand:SI 2 "metag_regnofrm_op"  "be,bf,be,bf"))
          (const_int 0)))
   (clobber (match_scratch:SI        0                     "=e, f, r, r"))]
  "TARGET_METAC_1_1"
  "ADDS\\t%0, %1, %2\\t%@ (*tadd si rrb OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

(define_insn "*tadds_rr_cc_noov_address_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (lo_sum:SI (match_operand:SI 1 "metag_datareg_op"  "0")
                     (match_operand:SI 2 "code_address"      ""))
          (const_int 0)))
   (set (match_operand:SI              0 "metag_datareg_op" "=d")
        (lo_sum:SI (match_dup 1)
                   (match_dup 2)))]
  "TARGET_METAC_1_1"
  "ADDS\\t%0, %1, #LO(%c2)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_insn_and_split "*tadds_si_cc_noov_address"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (match_operand:SI   1 "code_address"  "")
          (const_int 0)))
   (clobber (match_scratch:SI 0                "=d"))]
  "TARGET_METAC_1_1 && 1"
  "#"
  "&& reload_completed"
  [(set (match_dup 0)
        (high:SI (match_dup 1)))
   (parallel
    [(set (reg:<MODE> CC_REG)
          (compare:CCZNC
            (lo_sum:SI (match_dup 0)
                       (match_dup 1))
            (const_int 0)))
     (set (match_dup 0)
          (lo_sum:SI (match_dup 0)
                     (match_dup 1)))])]
  ""
  [(set_attr "type"    "two")
   (set_attr "ccstate" "set")])

(define_insn "*tadds_rr_<mode>_symglobal_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (lo_sum:SI (match_operand:SI 1 "metag_datareg_op"   "0")
                     (match_operand:SI 2 "metag_symglobal_op" ""))
          (const_int 0)))
   (set (match_operand:SI              0 "metag_datareg_op"  "=d")
        (lo_sum:SI (match_dup 1)
                   (match_dup 2)))]
  "TARGET_METAC_1_1"
  "ADDS\\t%0, %1, #LO(%c2)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_insn_and_split "*tadds_<mode>_si_symglobal_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (match_operand:SI   1 "metag_symglobal_op"  "")
          (const_int 0)))
   (clobber (match_scratch:SI 0                      "=d"))]
  "TARGET_METAC_1_1 && 0"
  "#"
  "&& reload_completed"
  [(set (match_dup 0)
        (high:SI (match_dup 1)))
   (parallel
    [(set (reg:<MODE> CC_REG)
          (compare:CCZNC
            (lo_sum:SI (match_dup 0)
                       (match_dup 1))
            (const_int 0)))
     (set (match_dup 0)
          (lo_sum:SI (match_dup 0)
                     (match_dup 1)))])]
  ""
  [(set_attr "type"   "fast")
   (set_attr "ccstate" "set")])

(define_insn "*tadds_si_cc_rri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,e,f,f,d,d,0,0,0, 0")
                   (match_operand:SI 2 "const_int_operand"     "K,P,K,P,K,P,I,J,O3,i"))
          (const_int 0)))
   (clobber (match_scratch:SI        0                        "=e,e,f,f,r,r,d,d,d, d"))]
 ""
 "@
  ADDS\\t%0, %1, %2\\t\\t%@ (*tadd si eeK OK)
  SUBS\\t%0, %1, #%n2\\t\\t%@ (*tadd si eeP OK)
  ADDS\\t%0, %1, %2\\t\\t%@ (*tadd si ffK OK)
  SUBS\\t%0, %1, #%n2\\t\\t%@ (*tadd si ffP OK)
  ADDS\\t%0, %1, %2\\t\\t%@ (*tadd si rdK OK)
  SUBS\\t%0, %1, #%n2\\t\\t%@ (*tadd si rdP OK)
  ADDS\\t%0, %1, %2\\t%@ (*tadd si d0I OK)
  ADDST\\t%0, %1, #HI(%c2)\\t%@ (*tadd si d0J OK)
  ADDS\\t%0, %1, #LO(%c2)\\t%@ (*tadd si d0O3 OK)
  #"
 [(set_attr "type"    "fast,fast,fast,fast,slow,slow,fast,fast,fast,two")
  (set_attr "ccstate" "set,set,set,set,set,set,set,set,set,fastset")])

;; Split the above insn if it needs more that one insn.
(define_split
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                   (match_operand:SI 2 "const_int_operand"    ""))
          (const_int 0)))
   (clobber (match_scratch:SI        0                        ""))]
  "reload_completed
   && REGNO (operands[0]) == REGNO (operands[1]) && !METAG_CONST_OK_FOR_LETTERS_KPIJO3 (operands[2])"
  [(set (match_dup 0)
        (plus:SI (match_dup 1)
                 (match_dup 3)))
   (parallel
     [(set (reg:<MODE> CC_REG)
           (compare:<MODE>
             (plus:SI (match_dup 0)
                      (match_dup 4))
             (const_int 0)))
      (clobber (match_dup 0))])]
  {
    HOST_WIDE_INT value = INTVAL (operands[2]);

    operands[3] = gen_int_mode (value & 0xFFFF0000, SImode);

    operands[4] = gen_int_mode (value & 0x0000FFFF, SImode);
  }
)

;; subtract instructions

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; subdi3 expander                    ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "subdi3"
  [(parallel
    [(set (match_operand:DI           0 "metag_reg_nofloat_op" "")
          (minus:DI (match_operand:DI 1 "metag_reg_nofloat_op" "")
                    (match_operand:DI 2 "metag_reg_nofloat_op" "")))
     (clobber (reg:CC CC_REG))])]
  ""
  "")

(define_insn "*subdi3_dsp"
  [(set (match_operand:DI           0 "metag_datareg_op" "=d")
        (minus:DI (match_operand:DI 1 "metag_datareg_op"  "d")
                  (match_operand:DI 2 "metag_datareg_op"  "d")))
   (clobber (reg:CC CC_REG))]
  "TARGET_DSP"
  "DL\\tSUBS\\t%0, %1, %2\\t%@ (*SUB\\t%t0, %t1, %t2)\;SUBCS\\t%t0, %t0, #1"
  [(set_attr "type" "two")
   (set_attr "ccstate" "ccx")])

(define_insn_and_split "*subdi3"
  [(set (match_operand:DI           0 "metag_datareg_op" "=d")
        (minus:DI (match_operand:DI 1 "metag_datareg_op"  "d")
                  (match_operand:DI 2 "metag_datareg_op"  "d")))
   (clobber (reg:CC CC_REG))]
  ""
  "#"
  "SPLIT_EARLY"
  [(parallel
     [(set (reg:CC_NOOV CC_REG)
           (compare:CC_NOOV
             (minus:SI (match_dup 5)
                       (match_dup 7))
             (const_int 0)))
      (set (match_dup 3)
           (minus:SI (match_dup 5)
                     (match_dup 7)))])
   (set (match_dup 4)
        (minus:SI (match_dup 6)
                 (match_dup 8)))
   (set (match_dup 4)
        (if_then_else:SI (ltu (reg:CC_NOOV CC_REG)
                              (const_int 0))
          (plus:SI (match_dup 4)
                   (const_int -1))
          (match_dup 4)))]
  {
    if (reload_completed)
      {
        operands[3] = gen_rtx_REG (SImode, REGNO (operands[0]));
        operands[4] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);

        operands[5] = gen_rtx_REG (SImode, REGNO (operands[1]));
        operands[6] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);

        operands[7] = gen_rtx_REG (SImode, REGNO (operands[2]));
        operands[8] = gen_rtx_REG (SImode, REGNO (operands[2]) + 1);
      }
    else
      {
        operands[3] = simplify_gen_subreg (SImode, operands[0], DImode, 0);
        operands[4] = simplify_gen_subreg (SImode, operands[0], DImode, UNITS_PER_WORD);

        operands[5] = simplify_gen_subreg (SImode, operands[1], DImode, 0);
        operands[6] = simplify_gen_subreg (SImode, operands[1], DImode, UNITS_PER_WORD);

        operands[7] = simplify_gen_subreg (SImode, operands[2], DImode, 0);
        operands[8] = simplify_gen_subreg (SImode, operands[2], DImode, UNITS_PER_WORD);
    }
  }
  [(set_attr "type" "three")
   (set_attr "ccstate" "ccx")])

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; subsi3 is made up of many parts..  ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "subsi3"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "")
        (minus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                  (match_operand:SI 2 "metag_reg_nofloat_op" "")))]
  ""
  {
    if (!reload_completed
        && !reload_in_progress
        && REG_P (operands[2]))
      {
        if (metag_frame_related_rtx (operands[1]))
          {
            /* Ensure reg-reg adds do not combine regs that may be eliminated */
            rtx reg = gen_reg_rtx (SImode);

            emit_move_insn (reg, operands[1]);
            operands[1] = force_reg (SImode, reg);
          }
        else if (metag_frame_related_rtx (operands[2]))
          {
            /* Ensure reg-reg adds do not combine regs that may be eliminated */
            rtx reg = gen_reg_rtx (SImode);

            emit_move_insn (reg, operands[2]);
            operands[2] = force_reg (SImode, reg);
          }
      }
  }
)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; and these are the subsi3 parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;

;; register - register ops
(define_insn "*sub_si_rrr"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "=e,f,h,l,da,da,da,da")
        (minus:SI (match_operand:SI 1 "metag_regnofrm_op"     "e,f,h,l,e, f, h, l")
                  (match_operand:SI 2 "metag_regnofrm_op"     "e,f,h,l,e, f, h, l")))]
  "!TARGET_METAC_1_1"
  "SUB%?\\t%0, %1, %2\\t%@ (*sub si rrr OK)"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; register - register ops - v1.1 case
(define_insn "*sub_si_rrr_1_1"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "=e, f, h, l, da,da,da,da")
        (minus:SI (match_operand:SI 1 "metag_regnofrm_op"     "e, f, h, l, e, f, h, l")
                  (match_operand:SI 2 "metag_regnofrm_op"     "be,bf,bh,bl,be,bf,bh,bl")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "SUB%?\\t%0, %1, %2\\t%@ (*sub si rrb OK)"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; register - register ops - minim case
(define_insn "*sub_si_rrr_minim"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "=e,!e, f,!f, h,!h, l,!l, da,!da, da,!da, da,!da, da,!da")
        (minus:SI (match_operand:SI 1 "metag_regnofrm_op"     "e, e, f, f, h, h, l, l, e,  e,  f,  f,  h,  h,  l,  l")
                  (match_operand:SI 2 "metag_regnofrm_op"     "e, be,f, bf,h, bh,l, bl,e,  be, f,  bf, h,  bh, l,  bl")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "SUB%?\\t%0, %1, %2\\t%@ (*sub si rrb OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,fast,slow,slow,slow,slow,slow,slow,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

;; Seperate stack frame related register-register subs
(define_insn "*sub_si_index_frame"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "=h,da,!da")
        (minus:SI (match_operand:SI 1 "metag_regframe_op"     "h,h,  h")
                  (match_operand:SI 2 "metag_regnofrm_op"     "h,h,  bh")))]
  "!TARGET_METAC_1_1"
  "SUB%?\\t%0, %1, %2\\t%@ (*sub si rfrmh OK)"
  [(set_attr "type" "fast,slow,three")
   (set_attr "cond" "yes,yes,yes")
   (set_attr "predicable" "yes")])

;; Subs are not commutative so second case required
(define_insn "*sub_si_index_frame2"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "=h,da,!da")
        (minus:SI (match_operand:SI 1 "metag_regnofrm_op"     "h,h,  h")
                  (match_operand:SI 2 "metag_regframe_op"     "h,h,  bh")))]
  "!TARGET_METAC_1_1"
  "SUB%?\\t%0, %1, %2\\t%@ (*sub si rhfrm OK)"
  [(set_attr "type" "fast,slow,three")
   (set_attr "cond" "yes,yes,yes")
   (set_attr "predicable" "yes")])

;; Seperate stack frame related register-register subs - v1.1 case
(define_insn "*sub_si_index_frame_1_1"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "=h, da")
        (minus:SI (match_operand:SI 1 "metag_regframe_op"     "h, h")
                  (match_operand:SI 2 "metag_regnofrm_op"     "bh,bh")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "SUB%?\\t%0, %1, %2\\t%@ (*sub si rfrmb OK)"
  [(set_attr "type" "fast,slow")
   (set_attr "cond" "yes,yes")
   (set_attr "predicable" "yes")])

;; Seperate stack frame related register-register subs - minim case
(define_insn "*sub_si_index_frame_minim"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "=h,!h, da,!da")
        (minus:SI (match_operand:SI 1 "metag_regframe_op"     "h, h, h,  h")
                  (match_operand:SI 2 "metag_regnofrm_op"     "h, bh,h,  bh")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "SUB%?\\t%0, %1, %2\\t%@ (*sub si rfrmb OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

;; Subs are not commutative so second case required - v1.1 case
(define_insn "*sub_si_index_frame2_1_1"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "=h, da")
        (minus:SI (match_operand:SI 1 "metag_regnofrm_op"     "h, h")
                  (match_operand:SI 2 "metag_regframe_op"     "bh,bh")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "SUB%?\\t%0, %1, %2\\t%@ (*sub si rbfrm OK)"
  [(set_attr "type" "fast,slow")
   (set_attr "cond" "yes,yes")
   (set_attr "predicable" "yes")])

;; Subs are not commutative so second case required - minim case
(define_insn "*sub_si_index_frame2_minim"
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "=h,!h, da,!da")
        (minus:SI (match_operand:SI 1 "metag_regnofrm_op"     "h, h, h,  h")
                  (match_operand:SI 2 "metag_regframe_op"     "h, bh,h,  bh")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "SUB%?\\t%0, %1, %2\\t%@ (*sub si rbfrm OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

;; spill_frame - cannot really sub frame value to something else
(define_split
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "")
        (minus:SI (match_operand:SI 1 "metag_regframe_op"    "")
                  (match_operand:SI 2 "metag_reg_nofloat_op" "")))]
  "!TARGET_METAC_1_1
   && reload_completed
   && METAG_REGNO_REG_CLASS (REGNO (operands[2])) != A0_REGS"
  [(set (match_dup 3)
        (match_dup 2))
   (set (match_dup 0)
        (minus:SI (match_dup 1)
                  (match_dup 3)))]
  {
    gcc_assert (A0_SCRATCH != INVALID_REGNUM && fixed_regs[A0_SCRATCH]);
    operands[3] = gen_rtx_REG (SImode, A0_SCRATCH);
  }
)
 
(define_split
  [(set (match_operand:SI           0 "metag_reg_nofloat_op" "")
        (minus:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                  (match_operand:SI 2 "metag_regframe_op"    "")))]
  "!TARGET_METAC_1_1
   && reload_completed
   && METAG_REGNO_REG_CLASS (REGNO (operands[1])) != A0_REGS"
  [(set (match_dup 3)
        (match_dup 1))
   (set (match_dup 0)
        (minus:SI (match_dup 3)
                  (match_dup 2)))]
  {
    gcc_assert (A0_SCRATCH != INVALID_REGNUM && fixed_regs[A0_SCRATCH]);
    operands[3] = gen_rtx_REG (SImode, A0_SCRATCH);
  }
)

;; register - register|immediate ops that set the flags
(define_insn "*subs_<mode>_si_rrr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (minus:SI (match_operand:SI 1 "metag_datareg_op"   "e,f,e,f")
                    (match_operand:SI 2 "metag_datareg_op"   "e,f,e,f"))
          (const_int 0)))
   (set (match_operand:SI             0 "metag_register_op" "=e,f,r,r")
        (minus:SI (match_dup 1)
                  (match_dup 2)))]
  "!TARGET_METAC_1_1"
  "SUBS\\t%0, %1, %2\\t%@ (*subs si rrr OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

;; register - register|immediate ops that set the flags - v1.1 case
(define_insn "*subs_<mode>_si_rrr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (minus:SI (match_operand:SI 1 "metag_datareg_op"      "e, f, e, f")
                    (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf"))
          (const_int 0)))
   (set (match_operand:SI             0 "metag_register_op"    "=e, f, r, r")
        (minus:SI (match_dup 1)
                  (match_dup 2)))]
  "TARGET_METAC_1_1"
  "SUBS\\t%0, %1, %2\\t%@ (*subs si rrb OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")
   (set_attr "o2rhint" "op2op1")])

;; register - register|immediate ops that set the flags only -> compares
(define_insn "*tsub_<mode>_si_rrr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (minus:SI (match_operand:SI 0 "metag_datareg_op" "e,f")
                    (match_operand:SI 1 "metag_datareg_op" "e,f"))
          (const_int 0)))]
  "!TARGET_METAC_1_1"
  "CMP\\t%0, %1\\t%@ (*tsub si dd OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")])

;; register - register ops that set the flags only -> compares - v1.1 case
(define_insn "*tsub_<mode>_si_rrr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (minus:SI (match_operand:SI 0 "metag_datareg_op" "e, f")
                    (match_operand:SI 1 "metag_datareg_op" "be,bf"))
          (const_int 0)))]
  "TARGET_METAC_1_1"
  "CMP\\t%0, %1\\t%@ (*tsub si db OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")
   (set_attr "o2rhint" "op1op0")])

;; signed multiply instructions

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; mulsi3 is made up of many parts..  ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "mulsi3"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "")
        (mult:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                 (match_operand:SI 2 "metag_regorint_op"    "")))]
  ""
  {
    if (metag_frame_related_rtx (operands[2]))
      {
        /* Put the magic frame registers first */
        rtx temp = operands[1];

        operands[1] = operands[2];
        operands[2] = temp;
      }

    if (!TARGET_METAC_1_1 && !reload_completed && !reload_in_progress)
      {
        if (CONST_INT_P (operands[2]) 
            && metag_frame_related_rtx (operands[1]))
          {
            /* Ensure reg*const do not combine reg that may be eliminated */
            rtx reg = gen_reg_rtx (SImode);

            emit_move_insn (reg, operands[1]);
            operands[1] = force_reg (SImode, reg);
          }
      }

    if (!CONST_INT_P (operands[2])
        || (   !satisfies_constraint_I (operands[2])
            && !satisfies_constraint_J (operands[2])
            && !satisfies_constraint_K (operands[2])
            && !satisfies_constraint_P (operands[2])
            && !satisfies_constraint_O3(operands[2])))
      {
        /* All except reg = (reg * bigconst) can be done quickly */
        operands[2] = force_reg (SImode, operands[2]);
      }
  }
)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; and these are the mulsi3 parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;

;; register * register ops - v1.0 bug disables MULD Dn.x, Dm.x, Dm.y
(define_insn "*mul_si_rrr"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=e,f,a,a")
        (mult:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,f,e,f")
                 (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f,e,f")))]
  "!TARGET_METAC_1_1"
  "MULD\\t%0, %1, %2\\t%@ (*mul si rrr OK)"
  [(set_attr "type" "mult,mult,slowslow,slowslow")])

;; register * register ops v1.1
(define_insn "*mul_si_rrr_1_1"
  [(set (match_operand:SI          0 "metag_register_op"    "=e, f, r, r")
        (mult:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, f, e, f")
                 (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "MULD\\t%0, %1, %2\\t%@ (*mul si rrr OK)"
  [(set_attr "type" "mult,mult,slowslow,slowslow")
   (set_attr "o2rhint" "op2op1")])

;; register * register ops minim
(define_insn "*mul_si_rrr_minim"
  [(set (match_operand:SI          0 "metag_register_op"    "=e,!e, f,!f, r,!r, r,!r")
        (mult:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, e, f, f, e, e, f, f")
                 (match_operand:SI 2 "metag_reg_nofloat_op"  "e, be,f, bf,e, be,f, bf")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "MULD\\t%0, %1, %2\\t%@ (*mul si rrr OK)"
  [(set_attr "type" "mult,mult,mult,mult,slowslow,slowslow,slowslow,slowslow")
   (set_attr "o2rhint" "op2op1")])

;; register * immediate ops - v1.0 bug disables MULD Dn.x, Dm.x, #0xnn
(define_insn "*mul_si_rri"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=e,f,d, d,d, a,a,&e,&f")
        (mult:SI (match_operand:SI 1 "metag_regnofrm_op"     "e,f,0, 0,0, e,f, e, f")
                 (match_operand:SI 2 "metag_int_operand"     "K,K,IP,J,O3,K,K, i, i")))]
  "!TARGET_METAC_1_1"
  "@
   MULD\\t%0, %1, %2\\t\\t%@ (*mul si eeK OK)
   MULD\\t%0, %1, %2\\t\\t%@ (*mul si ffK OK)
   MULD\\t%0, %1, %2\\t%@ (*mul si d0I OK)
   MULDT\\t%0, %1, #HI(%c2)\\t%@ (*mul si d0J OK)
   MULD\\t%0, %1, #LO(%c2)\\t%@ (*mul si d0O3 OK)
   MULD\\t%0, %1, %2\\t\\t%@ (*mul si reK OK)
   MULD\\t%0, %1, %2\\t\\t%@ (*mul si rfK OK)
   #
   #"
  [(set_attr "type" "mult,mult,mult,mult,mult,slowslow,slowslow,four,four")])

;; Split the above insn if it needs more than one insn
(define_split
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "")
        (mult:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                 (match_operand:SI 2 "metag_int_operand"    "")))]
  "!TARGET_METAC_1_1
   && reload_completed
   && REGNO (operands[0]) == REGNO (operands[1]) && !METAG_CONST_OK_FOR_LETTERS_KPIJO3 (operands[2])"
  [(set (match_dup 0)
        (match_dup 3))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 4)))
   (set (match_dup 0)
        (mult:SI (match_dup 1)
                 (match_dup 0)))]
  {
    HOST_WIDE_INT value = INTVAL (operands[2]);

    operands[3] = gen_int_mode (value & 0xFFFF0000, SImode);

    operands[4] = gen_int_mode (value & 0x0000FFFF, SImode);
  }
)

;; register * immediate ops
(define_insn "*mul_si_rri_1_1"
  [(set (match_operand:SI          0 "metag_register_op"    "=r,d, d,d, &e,&f")
        (mult:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "d,0, 0,0,  e, f")
                 (match_operand:SI 2 "metag_int_operand"     "K,IP,J,O3, i, i")))]
  "TARGET_METAC_1_1"
  "@
   MULD\\t%0, %1, %2\\t\\t%@ (*mul si rdK OK)
   MULD\\t%0, %1, %2\\t%@ (*mul si d0I OK)
   MULDT\\t%0, %1, #HI(%c2)\\t%@ (*mul si d0J OK)
   MULD \\t%0, %1, #LO(%c2)\\t%@ (*mul si d0J OK)
   #
   #"
  [(set_attr "type" "slowslow,mult,mult,mult,four,four")])

;; Split the above insn if it needs more than one insn
(define_split
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "")
        (mult:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                 (match_operand:SI 2 "metag_int_operand"    "")))]
  "TARGET_METAC_1_1
   && reload_completed
   && REGNO (operands[0]) != REGNO (operands[1]) && !satisfies_constraint_K (operands[2])"
  [(set (match_dup 0)
        (match_dup 3))
   (set (match_dup 0)
        (plus:SI (match_dup 0)
                 (match_dup 4)))
   (set (match_dup 0)
        (mult:SI (match_dup 1)
                 (match_dup 0)))]
  {
    HOST_WIDE_INT value = INTVAL (operands[2]);

    operands[3] = gen_int_mode (value & 0xFFFF0000, SImode);

    operands[4] = gen_int_mode (value & 0x0000FFFF, SImode);
  }
)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; umulhisi3 is made up of many parts..  ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "umulhisi3"
  [(set (match_operand:SI                          0 "metag_reg_nofloat_op" "")
        (mult:SI (zero_extend:SI (match_operand:HI 1 "metag_reg_nofloat_op" ""))
                 (zero_extend:SI (match_operand:HI 2 "metag_regorint_op"    ""))))]
  ""
  {
    if (CONST_INT_P (operands[2]))
      {
        /* Mask off the unsigned immediate to zero extend */
        HOST_WIDE_INT ival = INTVAL (operands[2]) & GET_MODE_MASK (HImode);

        emit_move_insn (operands[0],
                        gen_rtx_MULT (SImode,
                                      gen_rtx_ZERO_EXTEND (SImode, operands[1]),
                                                           GEN_INT (ival)));
        DONE;
      }
  }
)
 
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; and these are the umulhisi3 parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
 
;; register * register ops
(define_insn "*umul_hisi_rrr"
  [(set (match_operand:SI             0 "metag_register_op"    "=e,f,r,r")
        (mult:SI (zero_extend:SI
                    (match_operand:HI 1 "metag_reg_nofloat_op" "%e,f,e,f"))
                 (zero_extend:SI
                    (match_operand:HI 2 "metag_reg_nofloat_op"  "e,f,e,f"))))]
  "!TARGET_METAC_1_1"
  "MULW%?\\t%0, %1, %2\\t%@ (*umul hisi rrr OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; register * register ops - v1.1 case
(define_insn "*umul_hisi_rrr_1_1"
  [(set (match_operand:SI            0 "metag_register_op"    "=e, f, r, r")
        (mult:SI (zero_extend:SI
                   (match_operand:HI 1 "metag_reg_nofloat_op" "%e, f, e, f"))
                 (zero_extend:SI
                   (match_operand:HI 2 "metag_reg_nofloat_op"  "be,bf,be,bf"))))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "MULW%?\\t%0, %1, %2\\t%@ (*umul hisi rrr OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes")
   (set_attr "predicable" "yes")
   (set_attr "o2rhint" "op2op1")])

;; register * register ops - minim case
(define_insn "*umul_hisi_rrr_minim"
  [(set (match_operand:SI            0 "metag_register_op"    "=e,!e, f,!f, r,!r, r,!r")
        (mult:SI (zero_extend:SI
                   (match_operand:HI 1 "metag_reg_nofloat_op" "%e, e, f, f, e, e, f, f"))
                 (zero_extend:SI
                   (match_operand:HI 2 "metag_reg_nofloat_op"  "e, be,f, bf,e, be,f, bf"))))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "MULW%?\\t%0, %1, %2\\t%@ (*umul hisi rrr OK)"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")
   (set_attr "o2rhint" "op2op1")])

;; register * immediate ops
(define_insn "*umul_hisi_rri"
  [(set (match_operand:SI            0 "metag_register_op"    "=e,f,r,d")
        (mult:SI (zero_extend:SI
                   (match_operand:HI 1 "metag_reg_nofloat_op"  "e,f,d,0"))
                 (match_operand:SI   2 "metag_int_operand"     "K,K,K,IP")))]
  ""
  "@
   MULW%?\\t%0, %1, %2\\t\\t%@ (*umul hisi eeK OK)
   MULW%?\\t%0, %1, %2\\t\\t%@ (*umul hisi ffK OK)
   MULW\\t%0, %1, %2\\t\\t%@ (*umul hisi rdK OK)
   MULW\\t%0, %1, %2\\t%@ (*umul hisi d0I OK)"
  [(set_attr "type" "fast,fast,slow,fast")
   (set_attr "cond" "yes,yes,no,no")])

;; conditional version for specific cases of umul_hisi_rri
(define_insn "*cond_<mode>_umul_hisi_rri"
  [(cond_exec 
     (match_operator                   3 "comparison_operator"
       [(match_operand:CCALL           4 "metag_<mode>_reg"  "")
        (const_int 0)])
     (set (match_operand:SI            0 "metag_datareg_op" "=e,f")
          (mult:SI (zero_extend:SI
                     (match_operand:HI 1 "metag_datareg_op"  "e,f"))
                   (match_operand:SI   2 "metag_K_operand"   "K,K"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])"
  "@
   MULW%?\\t%0, %1, %2\\t\\t%@ (*umul hisi eeK OK)
   MULW%?\\t%0, %1, %2\\t\\t%@ (*umul hisi ffK OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "no")])

;; signed divide instructions

; we don't have one..

;; signed modulus instruction

; we don't have one..

;; unsigned divide instruction

; we don't have one..

;; logical-and instructions

(define_insn "*anddi3_dsp"
  [(set (match_operand:DI         0 "metag_register_op" "=d")
        (and:DI (match_dup 0)
                (match_operand:DI 1 "metag_16bit_op"     "KIP")))]
   "TARGET_DSP"
   "DL\\tAND\\t%0, %0, %1\\t%@ (*AND\\t%t0, %t0, %1)"
  [(set_attr "type" "fast")])

(define_insn "*anddi3"
  [(set (match_operand:DI         0 "metag_register_op" "=d")
        (and:DI (match_operand:DI 1 "metag_register_op"  "d")
                (match_operand:DI 2 "metag_register_op"  "d")))]
  "TARGET_DSP"
  "DL\\tAND\\t%0, %1, %2\\t%@ (*AND\\t%t0, %t1, %t2)"
  [(set_attr "type" "fast")])

(define_expand "anddi3"
  [(set (match_operand:DI         0 "metag_register_op" "=d")
        (and:DI (match_operand:DI 1 "metag_register_op"  "d")
                (match_operand:DI 2 "metag_register_op"  "d")))]
  ""
  {
    if (!TARGET_DSP)
      FAIL;
  }
  )


;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; andsi3 is made up of many parts..  ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "andsi3"
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "")
        (and:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                (match_operand:SI 2 "metag_regorint_op"    "")))]
  ""
  {
    if (CONST_INT_P (operands[2])
        && METAG_LETTER_FOR_CONST (operands[2]) == 0)
      {
        /* Need to use M,N cases to implement op */
        rtx temp = (reload_in_progress || reload_completed)
                    ? operands[0] : gen_reg_rtx (SImode);
        HOST_WIDE_INT value = INTVAL (operands[2]);

        emit_insn (gen_rtx_SET (VOIDmode, temp,
                                gen_rtx_AND (SImode, operands[1],
                                             gen_int_mode (value | 0xFFFF0000, SImode))));

        emit_insn (gen_rtx_SET (VOIDmode, operands[0],
                                gen_rtx_AND (SImode, temp,
                                             gen_int_mode (value | 0x0000FFFF, SImode))));
        DONE;
      }
  }
)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; and these are the andsi3 parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;

;; register & register ops
(define_insn "*and_si_rrr"
  [(set (match_operand:SI         0 "metag_register_op"    "=e,f,r,r")
        (and:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,f,e,f")
                (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f,e,f")))]
  "!TARGET_METAC_1_1 || TARGET_MTX"
  "AND%?\\t%0, %1, %2\\t%@ (*and si rrr OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; register & register ops - v1.1 case
(define_insn "*and_si_rrr_1_1"
  [(set (match_operand:SI         0 "metag_register_op"    "=e, f, r, r")
        (and:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, f, e, f")
                (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE && !TARGET_MTX"
  "AND%?\\t%0, %1, %2\\t%@ (*and si rrr OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes")
   (set_attr "predicable" "yes")
   (set_attr "o2rhint" "op2op1")])

;; register & register ops - minim case
(define_insn "*and_si_rrr_minim"
  [(set (match_operand:SI         0 "metag_register_op"    "=e,!e, f,!f, r,!r, r,!r")
        (and:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, e, f, f, e, e, f, f")
                (match_operand:SI 2 "metag_reg_nofloat_op"  "e, be,f, bf,e, be,f, bf")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE && !TARGET_MTX"
  "AND%?\\t%0, %1, %2\\t%@ (*and si rrr OK)"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")
   (set_attr "o2rhint" "op2op1")])

;; register & immediate ops
(define_insn "*and_si_rri"
  [(set (match_operand:SI         0 "metag_register_op"    "=d, d,d,d,e,f,r")
        (and:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,e,f,d")
                (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K,K,K")))]
  "!TARGET_MINIM_CORE"
  "@
   AND\\t%0, %1, %2\\t%@ (*and si r0I OK)
   ANDT\\t%0, %1, #HI(%c2)\\t%@ (*and si d0J OK)
   ANDMB\\t%0, %1, #LO(%c2)\\t%@ (*and si d0M OK)
   ANDMT\\t%0, %1, #HI(%c2)\\t%@ (*and si d0N OK)
   AND%?\\t%0, %1, %2\\t\\t%@ (*and si eeK OK)
   AND%?\\t%0, %1, %2\\t\\t%@ (*and si ffK OK)
   AND\\t%0, %1, %2\\t\\t%@ (*and si rdK OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,slow")
   (set_attr "cond" "no,no,no,no,yes,yes,no")])

;; register & immediate ops
(define_insn "*and_si_rri"
  [(set (match_operand:SI         0 "metag_register_op"    "=d, d,d,d,d,!e,!f,!r")
        (and:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,0, e, f, d")
                (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K, K, K, K")))]
  "TARGET_MINIM_CORE"
  "@
   AND\\t%0, %1, %2\\t%@ (*and si r0I OK)
   ANDT\\t%0, %1, #HI(%c2)\\t%@ (*and si d0J OK)
   ANDMB\\t%0, %1, #LO(%c2)\\t%@ (*and si d0M OK)
   ANDMT\\t%0, %1, #HI(%c2)\\t%@ (*and si d0N OK)
   AND%?\\t%0, %1, %2\\t\\t%@ (*and si d0K OK)
   AND%?\\t%0, %1, %2\\t\\t%@ (*and si eeK OK)
   AND%?\\t%0, %1, %2\\t\\t%@ (*and si ffK OK)
   AND\\t%0, %1, %2\\t\\t%@ (*and si rdK OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,slow")
   (set_attr "cond" "no,no,no,no,yes,yes,yes,no")])

;; conditional version for specific cases of and_si_rri
(define_insn "*cond_<mode>_and_si_rri"
  [(cond_exec
     (match_operator                3 "comparison_operator"
       [(match_operand:CCALL        4 "metag_<mode>_reg" "")
        (const_int 0)])
     (set (match_operand:SI         0 "metag_datareg_op" "=e,f")
          (and:SI (match_operand:SI 1 "metag_datareg_op"  "e,f")
                  (match_operand:SI 2 "metag_K_operand"   "K,K"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])"
  "@
   AND%?\\t%0, %1, %2\\t\\t%@ (*and si eeK OK)
   AND%?\\t%0, %1, %2\\t\\t%@ (*and si ffK OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "no")])

;; test register ops setting the NOOV flags
(define_insn "*tst_<mode>_si_rr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (and:SI (match_operand:SI 0 "metag_datareg_op" "%e,f")
                  (match_operand:SI 1 "metag_datareg_op"  "e,f"))
          (const_int 0)))]
  "!TARGET_METAC_1_1"
  "TST\\t%0, %1\\t%@ (*tst si dd OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")])

;; test register ops setting the NOOV flags - v1.1 case
(define_insn "*tst_<mode>_si_rr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (and:SI (match_operand:SI 0 "metag_datareg_op"    "%e, f")
                  (match_operand:SI 1 "metag_reg_nofloat_op" "be,bf"))
          (const_int 0)))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "TST\\t%0, %1\\t%@ (*tst si db OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")
   (set_attr "o2rhint" "op1op0")])

;; test register ops setting the NOOV flags - minim case
(define_insn "*tst_<mode>_si_rr_minim"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (and:SI (match_operand:SI 0 "metag_datareg_op"  "%e,!e, f,!f")
                  (match_operand:SI 1 "metag_register_op"  "e, be,f, bf"))
          (const_int 0)))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "TST\\t%0, %1\\t%@ (*tst si db OK)"
  [(set_attr "type"    "fast,fast,fast,fast")
   (set_attr "ccstate" "set")
   (set_attr "o2rhint" "op1op0")])

(define_insn "*tst_<mode>_si_ri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (and:SI (match_operand:SI 0 "metag_reg_nofloat_op" "d,d, d,d,d")
                  (match_operand:SI 1 "metag_int_operand"    "K,IP,J,M,N"))
          (const_int 0)))]
  ""
  "@
   TST\\t%0, %1\\t%@ (*tst si dI OK)
   TST\\t%0, %1\\t%@ (*tst si dI OK)
   TSTT\\t%0, %1\\t%@ (*tst si dJ OK)
   TSTMB\\t%0, #LO(%c1)\\t%@ (*tst si dM OK)
   TSTMT\\t%0, #HI(%c1)\\t%@ (*tst si dN OK)"
  [(set_attr "type"    "fast,fast,fast,fast,fast")
   (set_attr "ccstate" "set,set,set,set,set")])

;; and register ops combined with test cases
(define_insn "*ands_si_rrr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (and:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,f,e,f")
                  (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f,e,f"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_register_op"    "=e,f,r,r")
        (and:SI (match_dup 1)
                (match_dup 2)))]
  "!TARGET_METAC_1_1 || TARGET_MTX"
  "ANDS\\t%0, %1, %2\\t%@ (*ands si xdd OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

(define_insn "*ands_<mode>_si_rri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (and:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,e,f,d")
                  (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K,K,K"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_register_op"    "=d, d,d,d,e,f,r")
        (and:SI (match_dup 1)
                (match_dup 2)))]
  ""
  "@
   ANDS\\t%0, %1, %2\\t%@ (*ands si r0I OK)
   ANDST\\t%0, %1, #HI(%c2)\\t%@ (*ands si r0J OK)
   ANDSMB\\t%0, %1, #LO(%c2)\\t%@ (*ands si r0M OK)
   ANDSMT\\t%0, %1, #HI(%c2)\\t%@ (*ands si r0N OK)
   ANDS\\t%0, %1, %2\\t\\t%@ (*ands si eeK OK)
   ANDS\\t%0, %1, %2\\t\\t%@ (*ands si ffK OK)
   ANDS\\t%0, %1, %2\\t\\t%@ (*ands si ddK OK)"
  [(set_attr "type"    "fast,fast,fast,fast,fast,fast,slow")
   (set_attr "ccstate" "set,set,set,set,set,set,set")])

;; and register ops combined with test cases - v1.1 case
(define_insn "*ands_si_rrr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (and:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, f, e, f")
                  (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_register_op"    "=e, f, r, r")
        (and:SI (match_dup 1)
                (match_dup 2)))]
  "TARGET_METAC_1_1 && !TARGET_MTX"
  "ANDS\\t%0, %1, %2\\t%@ (*ands si xdb OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")
   (set_attr "o2rhint" "op2op1")])

;; bitextract - matched during combine
(define_insn_and_split "*zeroextractsi_<mode>_compare0"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
           (zero_extract:SI (match_operand:SI 0 "metag_reg_nofloat_op" "")
                            (match_operand    1 "const_int_operand"    "")
                            (match_operand    2 "const_int_operand"    ""))
           (const_int 0)))]
  "metag_zeroextract_mask_p (operands[1], operands[2])"
  "#"
  ""
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (and:SI (match_dup 0)
                  (match_dup 3))
          (const_int 0)))]
  {
    operands[3] = GEN_INT (((1 << INTVAL (operands[1])) -1) << INTVAL (operands[2]));
  }
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

;; inclusive-or instructions

(define_insn "iordi3"
  [(set (match_operand:DI         0 "metag_register_op" "=d")
        (ior:DI (match_operand:DI 1 "metag_register_op"  "d")
                (match_operand:DI 2 "metag_register_op"  "d")))]
   "TARGET_DSP"
   "DL\\tOR\\t%0, %1, %2\\t%@ (*OR\\t%t0, %t1, %t2)"
  [(set_attr "type" "fast")])

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; iorsi3 is made up of many parts..  ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "iorsi3"
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "")
        (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                (match_operand:SI 2 "metag_regorint_op"    "")))]
  ""
  {
    if (CONST_INT_P (operands[2])
        && METAG_LETTER_FOR_CONST (operands[2]) == 0)
      {
        /* Need to use I,J cases to implement op */
        rtx temp = (reload_in_progress || reload_completed)
                    ? operands[0] : gen_reg_rtx (SImode);
        HOST_WIDE_INT value = INTVAL (operands[2]);

        emit_insn (gen_rtx_SET (VOIDmode, temp,
                                gen_rtx_IOR (SImode, operands[1], 
                                             gen_int_mode (value & 0xFFFF0000, SImode))));

        emit_insn (gen_rtx_SET (VOIDmode, operands[0],
                                gen_rtx_IOR (SImode, temp,
                                             gen_int_mode (value & 0x0000FFFF, SImode))));
        DONE;
      }
  }
)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; and these are the iorsi3 parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;

;; register | register ops
(define_insn "*ior_si_rrr"
  [(set (match_operand:SI         0 "metag_register_op"    "=e,f,r,r")
        (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,f,e,f")
                (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f,e,f")))]
  "!TARGET_METAC_1_1 || TARGET_MTX"
  "OR%?\\t%0, %1, %2\\t%@ (*ior si xdd OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; register | register ops - v1.1 case
(define_insn "*ior_si_rrr_1_1"
  [(set (match_operand:SI         0 "metag_register_op"    "=e, f, r, r")
        (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, f, e, f")
                (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE && !TARGET_MTX"
  "OR%?\\t%0, %1, %2\\t%@ (*ior si xdb OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes")
   (set_attr "predicable" "yes")
   (set_attr "o2rhint" "op2op1")])

;; register | register ops - minim case
(define_insn "*ior_si_rrr_minim"
  [(set (match_operand:SI         0 "metag_register_op"    "=e,!e, f,!f, r,!r, r,!r")
        (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, e, f, f, e, e, f, f")
                (match_operand:SI 2 "metag_reg_nofloat_op"  "e, be,f, bf,e, be,f, bf")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE && !TARGET_MTX"
  "OR%?\\t%0, %1, %2\\t%@ (*ior si xdb OK)"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")
   (set_attr "o2rhint" "op2op1")])

;; register | immediate ops
(define_insn "*ior_si_rri"
  [(set (match_operand:SI         0 "metag_register_op"    "=d, d,d,d,e,f,r")
        (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,e,f,d")
                (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K,K,K")))]
  "!TARGET_MINIM_CORE"
  "@
   OR\\t%0, %1, %2\\t%@ (*ior si r0I OK)
   ORT\\t%0, %1, #HI(%c2)\\t%@ (*ior si r0J OK)
   ORMB\\t%0, %1, #LO(%c2)\\t%@ (*ior si r0M OK)
   ORMT\\t%0, %1, #HI(%c2)\\t%@ (*ior si r0N OK)
   OR%?\\t%0, %1, %2\\t\\t%@ (*ior si eeK OK)
   OR%?\\t%0, %1, %2\\t\\t%@ (*ior si ffK OK)
   OR\\t%0, %1, %2\\t\\t%@ (*ior si rdK OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,slow")
   (set_attr "cond" "no,no,no,no,yes,yes,no")])

;; register | immediate ops
(define_insn "*ior_si_rri"
  [(set (match_operand:SI         0 "metag_register_op"    "=d, d,d,d,d,!e,!f,!r")
        (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,0, e, f, d")
                (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K, K, K, K")))]
  "TARGET_MINIM_CORE"
  "@
   OR\\t%0, %1, %2\\t%@ (*ior si r0I OK)
   ORT\\t%0, %1, #HI(%c2)\\t%@ (*ior si r0J OK)
   ORMB\\t%0, %1, #LO(%c2)\\t%@ (*ior si r0M OK)
   ORMT\\t%0, %1, #HI(%c2)\\t%@ (*ior si r0N OK)
   OR%?\\t%0, %1, %2\\t\\t%@ (*ior si d0K OK)
   OR%?\\t%0, %1, %2\\t\\t%@ (*ior si eeK OK)
   OR%?\\t%0, %1, %2\\t\\t%@ (*ior si ffK OK)
   OR\\t%0, %1, %2\\t\\t%@ (*ior si rdK OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,slow")
   (set_attr "cond" "no,no,no,no,yes,yes,yes,no")])

;; conditional version for specific cases of ior_si_rri
(define_insn "*cond_<mode>_ior_si_rri"
  [(cond_exec
     (match_operator                3 "comparison_operator"
       [(match_operand:CCALL        4 "metag_<mode>_reg"  "")
        (const_int 0)])
     (set (match_operand:SI         0 "metag_datareg_op" "=e,f")
          (ior:SI (match_operand:SI 1 "metag_datareg_op"  "e,f")
                  (match_operand:SI 2 "metag_K_operand"   "K,K"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])"
  "@
   OR%?\\t%0, %1, %2\\t\\t%@ (*ior si eeK OK)
   OR%?\\t%0, %1, %2\\t\\t%@ (*ior si ffK OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "no")])

;; ior register ops combined with test cases
(define_insn "*iors_<mode>_si_rrr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ior:SI (match_operand:SI 1 "metag_datareg_op"  "%e,f,e,f")
                  (match_operand:SI 2 "metag_datareg_op"   "e,f,e,f"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_register_op" "=e,f,r,r")
        (ior:SI (match_dup 1)
                (match_dup 2)))]
  "!TARGET_METAC_1_1 || TARGET_MTX"
  "ORS\\t%0, %1, %2\\t%@ (*iors si xdd OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

(define_insn "*iors_<mode>_si_rri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ior:SI (match_operand:SI 1 "metag_datareg_op"   "0, 0,0,0,e,f,d")
                  (match_operand:SI 2 "metag_int_operand"  "IP,J,M,N,K,K,K"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_register_op" "=d, d,d,d,e,f,r")
        (ior:SI (match_dup 1)
                (match_dup 2)))]
  ""
  "@
   ORS\\t%0, %1, %2\\t%@ (*iors si d0I OK)
   ORST\\t%0, %1, #HI(%c2)\\t%@ (*ands si d0J OK)
   ORSMB\\t%0, %1, #LO(%c2)\\t%@ (*iors si d0M OK)
   ORSMT\\t%0, %1, #HI(%c2)\\t%@ (*ands si d0N OK)
   ORS\\t%0, %1, %2\\t\\t%@ (*iors si eeK OK)
   ORS\\t%0, %1, %2\\t\\t%@ (*iors si ffK OK)
   ORS\\t%0, %1, %2\\t\\t%@ (*iors si ddK OK)"
  [(set_attr "type"    "fast,fast,fast,fast,fast,fast,slow")
   (set_attr "ccstate" "set,set,set,set,set,set,set")])

;; ior register ops combined with test cases - v1.1 case
(define_insn "*iors_<mode>_si_rrr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, f, e, f")
                  (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_register_op"    "=e, f, r, r")
        (ior:SI (match_dup 1)
                (match_dup 2)))]
  "TARGET_METAC_1_1 && !TARGET_MTX"
  "ORS\\t%0, %1, %2\\t%@ (*iors si xdb OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")
   (set_attr "o2rhint" "op2op1")])

;; scratch ior register ops setting the NOOV flags - needed to enable combines
(define_insn "*tior_<mode>_si_rr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,f,e,f")
                  (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f,e,f"))
          (const_int 0)))
   (clobber (match_scratch:SI       0                        "=e,f,r,r"))]
  "!TARGET_METAC_1_1 || TARGET_MTX"
  "ORS\\t%0, %1, %2\\t%@ (*tior si xdd OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

(define_insn "*tior_<mode>_si_ri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,e,f")
                  (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K,K"))
          (const_int 0)))
   (clobber (match_scratch:SI       0                        "=d, d,d,d,e,f"))]
  ""
  "@
   ORS\\t%0, %1, %2\\t%@ (*tior si dI OK)
   ORST\\t%0, %1, #HI(%c2)\\t%@ (*tior si dJ OK)
   ORSMB\\t%0, %1, #LO(%c2)\\t%@ (*tior si dM OK)
   ORSMT\\t%0, %1, #HI(%c2)\\t%@ (*tior si dN OK)
   ORS\\t%0, %1, %2\\t\\t%@ (*tior si eK OK)
   ORS\\t%0, %1, %2\\t\\t%@ (*tior si fK OK)"
  [(set_attr "type"    "fast,fast,fast,fast,fast,fast")
   (set_attr "ccstate" "set,set,set,set,set,set")])

;; scratch ior register ops setting the NOOV flags - v1.1 case
(define_insn "*tior_<mode>_si_rr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ior:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, f, e, f")
                  (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf"))
          (const_int 0)))
   (clobber (match_scratch:SI       0                        "=e, f, r, r"))]
  "TARGET_METAC_1_1 && !TARGET_MTX"
  "ORS\\t%0, %1, %2\\t%@ (*tior si xdb OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")
   (set_attr "o2rhint" "op2op1")])

;; exclusive-or instructions

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; and these are the xordi3 parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;

(define_insn "*xordi3_rri"
  [(set (match_operand:DI         0 "metag_register_op" "+d")
        (xor:DI (match_dup 0)
                (match_operand:DI 1 "metag_16bit_op"     "KIP")))]
   "TARGET_DSP"
   "DL\\tXOR\\t%0, %0, %1\\t%@ (*OR\\t%t0, %t0, %1)"
  [(set_attr "type" "fast")])

(define_insn "xordi3"
  [(set (match_operand:DI         0 "metag_register_op" "=d")
        (xor:DI (match_operand:DI 1 "metag_register_op"  "d")
                (match_operand:DI 2 "metag_register_op"  "d")))]
   "TARGET_DSP"
   "DL\\tXOR\\t%0, %1, %2\\t%@ (*OR\\t%t0, %t1, %t2)"
  [(set_attr "type" "fast")])

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; xorsi3 is made up of many parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
(define_expand "xorsi3"
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "")
        (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")
                (match_operand:SI 2 "metag_regorint_op"    "")))]
  ""
  {
    if (CONST_INT_P (operands[2])
        && METAG_LETTER_FOR_CONST (operands[2]) == 0)
      {
        /* Need to use I,J cases to implement op */
        rtx temp = (reload_in_progress || reload_completed)
                    ? operands[0] : gen_reg_rtx (SImode);
        HOST_WIDE_INT value = INTVAL (operands[2]);

        emit_insn (gen_rtx_SET (VOIDmode, temp,
                                gen_rtx_XOR (SImode, operands[1],
                                             gen_int_mode (value & 0xFFFF0000, SImode))));

        emit_insn (gen_rtx_SET (VOIDmode, operands[0],
                                gen_rtx_XOR (SImode, temp,
                                             gen_int_mode (value & 0x0000FFFF, SImode))));
        DONE;
      }
  }
)

;; register ^ register ops

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;
;; and these are the xorsi3 parts.. ;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;;

;; register ^ register ops
(define_insn "*xor_si_rrr"
  [(set (match_operand:SI         0 "metag_register_op"    "=e,f,r,r")
        (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,f,e,f")
                (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f,e,f")))]
  "!TARGET_METAC_1_1 || TARGET_MTX"
  "XOR%?\\t%0, %1, %2\\t%@ (*xor si xdd OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes")
   (set_attr "predicable" "yes")])

;; register ^ register ops - v1.1 case
(define_insn "*xor_si_rrr_1_1"
  [(set (match_operand:SI         0 "metag_register_op"    "=e, f, r, r")
        (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, f, e, f")
                (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE && !TARGET_MTX"
  "XOR%?\\t%0, %1, %2\\t%@ (*xor si xdb OK)"
  [(set_attr "type" "fast,fast,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes")
   (set_attr "predicable" "yes")
   (set_attr "o2rhint" "op2op1")])

;; register ^ register ops - minim case
(define_insn "*xor_si_rrr_minim"
  [(set (match_operand:SI         0 "metag_register_op"    "=e,!e, f,!f, r,!r, r,!r")
        (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, e, f, f, e, e, f, f")
                (match_operand:SI 2 "metag_reg_nofloat_op"  "e, be,f, bf,e, be,f, bf")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE && !TARGET_MTX"
  "XOR%?\\t%0, %1, %2\\t%@ (*xor si xdb OK)"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")
   (set_attr "o2rhint" "op2op1")])

;; same register ^ immediate ops
(define_insn "*xor_si_rri"
  [(set (match_operand:SI         0 "metag_register_op"    "=d, d,d,d,e,f,r")
        (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,e,f,d")
                (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K,K,K")))]
  "!TARGET_MINIM_CORE"
  "@
   XOR\\t%0, %1, %2\\t%@ (*xor si d0I OK)
   XORT\\t%0, %1, #HI(%c2)\\t%@ (*xor si d0J OK)
   XORMB\\t%0, %1, #LO(%c2)\\t%@ (*xor si d0M OK)
   XORMT\\t%0, %1, #HI(%c2)\\t%@ (*xor si d0N OK)
   XOR%?\\t%0, %1, %2\\t\\t%@ (*xor si eeK OK)
   XOR%?\\t%0, %1, %2\\t\\t%@ (*xor si ffK OK)
   XOR\\t%0, %1, %2\\t\\t%@ (*xor si rdK OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,slow")
   (set_attr "cond" "no,no,no,no,yes,yes,no")])

;; same register ^ immediate ops
(define_insn "*xor_si_rri"
  [(set (match_operand:SI         0 "metag_register_op"    "=d, d,d,d,d,e,f,r")
        (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,0,e,f,d")
                (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K,K,K,K")))]
  "TARGET_MINIM_CORE"
  "@
   XOR\\t%0, %1, %2\\t%@ (*xor si d0I OK)
   XORT\\t%0, %1, #HI(%c2)\\t%@ (*xor si d0J OK)
   XORMB\\t%0, %1, #LO(%c2)\\t%@ (*xor si d0M OK)
   XORMT\\t%0, %1, #HI(%c2)\\t%@ (*xor si d0N OK)
   XOR%?\\t%0, %1, %2\\t\\t%@ (*xor si d0K OK)
   XOR%?\\t%0, %1, %2\\t\\t%@ (*xor si eeK OK)
   XOR%?\\t%0, %1, %2\\t\\t%@ (*xor si ffK OK)
   XOR\\t%0, %1, %2\\t\\t%@ (*xor si rdK OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,fast,slow")
   (set_attr "cond" "no,no,no,no,yes,yes,yes,no")])

;; conditional version for specific cases of xor_si_rri
(define_insn "*cond_<mode>_xor_si_rri"
  [(cond_exec
     (match_operator                3 "comparison_operator"
       [(match_operand:CCALL        4 "metag_<mode>_reg"  "")
        (const_int 0)])
     (set (match_operand:SI         0 "metag_datareg_op" "=e,f")
          (xor:SI (match_operand:SI 1 "metag_datareg_op"  "e,f")
                  (match_operand:SI 2 "metag_K_operand"   "K,K"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])"
  "@
   XOR%?\\t%0, %1, %2\\t\\t%@ (*xor si eeK OK)
   XOR%?\\t%0, %1, %2\\t\\t%@ (*xor si ffK OK)"
  [(set_attr "type" "fast")
   (set_attr "cond" "no")])

;; xor register ops combined with test cases
(define_insn "*xors_<mode>_si_rrr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,f,e,f")
                  (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f,e,f"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_register_op"    "=e,f,r,r")
        (xor:SI (match_dup 1)
                (match_dup 2)))]
  "!TARGET_METAC_1_1 || TARGET_MTX"
  "XORS\\t%0, %1, %2\\t%@ (*xors si xdd OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

(define_insn "*xors_<mode>_si_rri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,e,f,d")
                  (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K,K,K"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_register_op"    "=d, d,d,d,e,f,r")
        (xor:SI (match_dup 1)
                (match_dup 2)))]
  ""
  "@
   XORS\\t%0, %1, %2\\t%@ (*xors si d0I OK)
   XORST\\t%0, %1, #HI(%c2)\\t%@ (*xors si d0J OK)
   XORSMB\\t%0, %1, #LO(%c2)\\t%@ (*xors si d0M OK)
   XORSMT\\t%0, %1, #HI(%c2)\\t%@ (*xors si d0N OK)
   XORS\\t%0, %1, %2\\t\\t%@ (*xors si eeK OK)
   XORS\\t%0, %1, %2\\t\\t%@ (*xors si ffK OK)
   XORS\\t%0, %1, %2\\t\\t%@ (*xors si rdK OK)"
  [(set_attr "type"    "fast,fast,fast,fast,fast,fast,slow")
   (set_attr "ccstate" "set,set,set,set,set,set,set")])

;; xor register ops combined with test cases - v1.1 case
(define_insn "*xors_<mode>_si_rrr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, f, e, f")
                  (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_register_op"    "=e, f, r, r")
        (xor:SI (match_dup 1)
                (match_dup 2)))]
  "TARGET_METAC_1_1 && !TARGET_MTX"
  "XORS\\t%0, %1, %2\\t%@ (*xors si xdb OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")
   (set_attr "o2rhint" "op2op1")])

;; scratch xor register ops setting the NOOV flags - needed to enable combines
(define_insn "*txor_<mode>_si_rr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,f,e,f")
                  (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f,e,f"))
          (const_int 0)))
   (clobber (match_scratch:SI       0                        "=e,f,r,r"))]
  "!TARGET_METAC_1_1 || TARGET_MTX"
  "XORS\\t%0, %1, %2\\t%@ (*txor si xdd OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")])

(define_insn "*txor_<mode>_si_ri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0, 0,0,0,e,f")
                  (match_operand:SI 2 "metag_int_operand"     "IP,J,M,N,K,K"))
          (const_int 0)))
   (clobber (match_scratch:SI       0                        "=d, d,d,d,e,f"))]
  ""
  "@
   XORS\\t%0, %1, %2\\t%@ (*txor si dI OK)
   XORST\\t%0, %1, #HI(%c2)\\t%@ (*txor si dJ OK)
   XORSMB\\t%0, %1, #LO(%c2)\\t%@ (*txor si dM OK)
   XORSMT\\t%0, %1, #HI(%c2)\\t%@ (*txor si dN OK)
   XORS\\t%0, %1, %2\\t\\t%@ (*txor si eK OK)
   XORS\\t%0, %1, %2\\t\\t%@ (*txor si fK OK)"
  [(set_attr "type"    "fast,fast,fast,fast,fast,fast")
   (set_attr "ccstate" "set,set,set,set,set,set")])

;; scratch xor register ops setting the NOOV flags - v1.1 case
(define_insn "*txor_<mode>_si_rr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (xor:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e, f, e, f")
                  (match_operand:SI 2 "metag_reg_nofloat_op"  "be,bf,be,bf"))
          (const_int 0)))
   (clobber (match_scratch:SI       0                        "=e, f, r, r"))]
  "TARGET_METAC_1_1 && !TARGET_MTX"
  "XORS\\t%0, %1, %2\\t%@ (*txor si xdd OK)"
  [(set_attr "type"    "fast,fast,slow,slow")
   (set_attr "ccstate" "set,set,set,set")
   (set_attr "o2rhint" "op2op1")])

;; rotate instructions
(define_insn "rotsi2_16"
  [(set (match_operand:SI              0 "metag_reg_nofloat_op" "=d,e,f")
        (rotate:SI   (match_operand:SI 1 "metag_reg_nofloat_op"  "0,e,f")
                     (const_int 16)))]
  ""
  "RTDW\\t%0, %1"
  [(set_attr "type" "fast,fast,fast")])

(define_insn "parallel_rotsi2_16"
  [(parallel 
     [(set (subreg:SI (match_operand:DI              0 "metag_datareg_op" "=d") 0)
           (rotate:SI (subreg:SI (match_operand:DI   1 "metag_datareg_op"  "d") 0)
                      (const_int 16)))
      (set (subreg:SI (match_dup 0) 4)
           (rotate:SI (subreg:SI (match_dup 1) 4)
                      (const_int 16)))])]
  "TARGET_DSP"
  "DL\\tRTDW\\t%0, %1"
  [(set_attr "type" "fast")])

;; arithmetic shift instructions

(define_expand "ashlsi3"
  [(set (match_operand:SI              0 "metag_register_op" "")
        (ashift:SI   (match_operand:SI 1 "metag_register_op" "")
                     (match_operand:SI 2 "metag_regorint_op" "")))]
  ""
  "")

(define_insn "*ashlsi3"
  [(set (match_operand:SI              0 "metag_register_op"    "=e,e,f,f,r,r,r,r")
        (ashift:SI   (match_operand:SI 1 "metag_reg_nofloat_op"  "e,e,f,f,e,e,f,f")
                     (match_operand:SI 2 "metag_regorint_op"     "e,L,f,L,e,L,f,L")))]
  "!TARGET_MINIM_CORE"
  "LSL%?\\t%0, %1, %2\\t%@ (*ashl si rrx OK)"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,no,no,no,no")])

(define_insn "*ashlsi3_minim"
  [(set (match_operand:SI              0 "metag_register_op"    "=e,e,!e,f,f,!f,r,r,r,r")
        (ashift:SI   (match_operand:SI 1 "metag_reg_nofloat_op"  "e,0, e,f,0, f,e,e,f,f")
                     (match_operand:SI 2 "metag_regorint_op"     "e,L, L,f,L, L,e,L,f,L")))]
  "TARGET_MINIM_CORE"
  "LSL%?\\t%0, %1, %2\\t%@ (*ashl si rrx OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,no,no,no,no")])

;; conditional version for specific cases of ashlsi3
(define_insn "*cond_<mode>_ashlsi3_rrr"
  [(cond_exec
     (match_operator                     3 "comparison_operator"
       [(match_operand:CCALL             4 "metag_<mode>_reg"  "")
        (const_int 0)])
     (set (match_operand:SI              0 "metag_datareg_op" "=e,f")
          (ashift:SI   (match_operand:SI 1 "metag_datareg_op"  "e,f")
                       (match_operand:SI 2 "metag_datareg_op"  "e,f"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])
   && metag_same_regclass_p (operands[0], operands[2])"
  "@
   LSL%?\\t%0, %1, %2\\t%@ (*ashl si eee OK)
   LSL%?\\t%0, %1, %2\\t%@ (*ashl si fff OK)"
  [(set_attr "type" "fast,fast")
   (set_attr "cond" "no")])

(define_insn "*cond_<mode>_ashlsi3_rrL"
  [(cond_exec
     (match_operator                     3 "comparison_operator"
       [(match_operand:CCALL             4 "metag_<mode>_reg"       "")
        (const_int 0)])
     (set (match_operand:SI              0 "metag_datareg_op" "=e,f")
          (ashift:SI   (match_operand:SI 1 "metag_datareg_op"  "e,f")
                       (match_operand:SI 2 "metag_L_operand"   "L,L"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])"
  "@
   LSL%?\\t%0, %1, %2\\t%@ (*ashl si eeL OK)
   LSL%?\\t%0, %1, %2\\t%@ (*ashl si ffL OK)"
  [(set_attr "type" "fast,fast")
   (set_attr "cond" "no")])

(define_expand "ashrsi3"
  [(set (match_operand:SI              0 "metag_register_op" "=e,e,f,f,r,r,r,r")
        (ashiftrt:SI (match_operand:SI 1 "metag_datareg_op"   "e,e,f,f,e,e,f,f")
                     (match_operand:SI 2 "metag_regorint_op"  "e,L,f,L,e,L,f,L")))]
  ""
  "")

(define_insn "*ashrsi3"
  [(set (match_operand:SI              0 "metag_register_op" "=e,e,f,f,r,r,r,r")
        (ashiftrt:SI (match_operand:SI 1 "metag_datareg_op"   "e,e,f,f,e,e,f,f")
                     (match_operand:SI 2 "metag_regorint_op"  "e,L,f,L,e,L,f,L")))]
  "!TARGET_MINIM_CORE"
  "ASR%?\\t%0, %1, %2\\t%@ (*ashr si rrx OK)"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,no,no,no,no")])

(define_insn "*ashrsi3_minim"
  [(set (match_operand:SI              0 "metag_register_op" "=e,e,!e,f,f,!f,r,r,r,r")
        (ashiftrt:SI (match_operand:SI 1 "metag_datareg_op"   "e,0, e,f,0, f,e,e,f,f")
                     (match_operand:SI 2 "metag_regorint_op"  "e,L, L,f,L, L,e,L,f,L")))]
  "TARGET_MINIM_CORE"
  "ASR%?\\t%0, %1, %2\\t%@ (*ashr si rrx OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,no,no,no,no")])

;; conditional version for specific cases of ashrsi3
(define_insn "*cond_<mode>_ashrsi3_rrr"
  [(cond_exec
     (match_operator                     3 "comparison_operator"
       [(match_operand:CCALL             4 "metag_<mode>_reg"  "")
        (const_int 0)])
     (set (match_operand:SI              0 "metag_datareg_op" "=e,f")
          (ashiftrt:SI (match_operand:SI 1 "metag_datareg_op"  "e,f")
                       (match_operand:SI 2 "metag_datareg_op"  "e,f"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])
   && metag_same_regclass_p (operands[0], operands[2])"
  "@
   ASR%?\\t%0, %1, %2\\t%@ (*ashr si eee OK)
   ASR%?\\t%0, %1, %2\\t%@ (*ashr si fff OK)"
  [(set_attr "type" "fast,fast")
   (set_attr "cond" "no")])

(define_insn "*cond_<mode>_ashrsi3_rrL"
  [(cond_exec
     (match_operator                     3 "comparison_operator"
       [(match_operand:CCALL             4 "metag_<mode>_reg"  "")
        (const_int 0)])
     (set (match_operand:SI              0 "metag_datareg_op" "=e,f")
          (ashiftrt:SI (match_operand:SI 1 "metag_datareg_op"  "e,f")
                       (match_operand:SI 2 "metag_L_operand"   "L,L"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])"
  "@
   ASR%?\\t%0, %1, %2\\t%@ (*ashr si eeL OK)
   ASR%?\\t%0, %1, %2\\t%@ (*ashr si ffL OK)"
  [(set_attr "type" "fast,fast")
   (set_attr "cond" "no")])

(define_insn_and_split "*ashrdi3_32"
  [(set (match_operand:DI              0 "metag_reg_nofloat_op" "=d,a")
        (ashiftrt:DI (match_operand:DI 1 "metag_datareg_op"      "d,d")
                     (const_int 32)))]
  ""
  "#"
  "SPLIT_EARLY"
  [(set (match_dup 2)
        (match_dup 4))
   (set (match_dup 3)
        (ashiftrt:SI (match_dup 4)
                     (const_int 31)))]
  {
    if (reload_completed)
      {
        operands[2] = gen_rtx_REG (SImode, REGNO (operands[0]));
        operands[3] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);

        operands[4] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
      }
    else
      {
        operands[2] = simplify_gen_subreg (SImode, operands[0], DImode, 0);
        operands[3] = simplify_gen_subreg (SImode, operands[0], DImode, UNITS_PER_WORD);

        operands[4] = simplify_gen_subreg (SImode, operands[1], DImode, UNITS_PER_WORD);
      }
  }
  [(set_attr "type" "two,slowslow")])

;; logical shift instructions

(define_expand "lshrsi3"
  [(set (match_operand:SI              0 "metag_register_op"    "=e,e,f,f,r,r,r,r")
        (lshiftrt:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "e,e,f,f,e,e,f,f")
                     (match_operand:SI 2 "metag_regorint_op"     "e,L,f,L,e,L,f,L")))]
  ""
  "")

(define_insn "*lshrsi3"
  [(set (match_operand:SI              0 "metag_register_op"    "=e,e,f,f,r,r,r,r")
        (lshiftrt:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "e,e,f,f,e,e,f,f")
                     (match_operand:SI 2 "metag_regorint_op"     "e,L,f,L,e,L,f,L")))]
  "!TARGET_MINIM_CORE"
  "LSR%?\\t%0, %1, %2\\t%@ (lshr si rrx OK)"
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,no,no,no,no")])

(define_insn "*lshrsi3_minim"
  [(set (match_operand:SI              0 "metag_register_op"    "=e,e,!e,f,f,!f,r,r,r,r")
        (lshiftrt:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "e,0, e,f,0, f,e,e,f,f")
                     (match_operand:SI 2 "metag_regorint_op"     "e,L, L,f,L, L,e,L,f,L")))]
  "TARGET_MINIM_CORE"
  "LSR%?\\t%0, %1, %2\\t%@ (lshr si rrx OK)"
  [(set_attr "type" "fast,fast,fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,yes,yes,no,no,no,no")])

;; conditional version for specific cases of lshrsi3
(define_insn "*cond_<mode>_lshrsi3_rrr"
  [(cond_exec
     (match_operator                     3 "comparison_operator"
       [(match_operand:CCALL             4 "metag_<mode>_reg"  "")
        (const_int 0)])
     (set (match_operand:SI              0 "metag_datareg_op" "=e,f")
          (lshiftrt:SI (match_operand:SI 1 "metag_datareg_op"  "e,f")
                       (match_operand:SI 2 "metag_datareg_op"  "e,f"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])
   && metag_same_regclass_p (operands[0], operands[2])"
  "@
   LSR%?\\t%0, %1, %2\\t%@ (lshr si eee OK)
   LSR%?\\t%0, %1, %2\\t%@ (lshr si fff OK)"
  [(set_attr "type" "fast,fast")
   (set_attr "cond" "no")])

(define_insn "*cond_<mode>_lshrsi3_rrL"
  [(cond_exec
     (match_operator                     3 "comparison_operator"
       [(match_operand:CCALL             4 "metag_<mode>_reg"   "")
        (const_int 0)])
     (set (match_operand:SI              0 "metag_datareg_op" "=e,f")
          (lshiftrt:SI (match_operand:SI 1 "metag_datareg_op"  "e,f")
                       (match_operand:SI 2 "metag_L_operand"   "L,L"))))]
  "reload_completed
   && metag_same_regclass_p (operands[0], operands[1])"
  "@
   LSR%?\\t%0, %1, %2\\t%@ (lshr si eeL OK)
   LSR%?\\t%0, %1, %2\\t%@ (lshr si ffL OK)"
  [(set_attr "type" "fast,fast")
   (set_attr "cond" "no")])

;; shift instructions combined with setting NOOV flags

(define_insn "*ashls_<mode>_si_rrx"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ashift:SI
            (match_operand:SI 1 "metag_reg_nofloat_op"  "e,e,f,f,e,e,f,f")
            (match_operand:SI 2 "metag_regorint_op"     "e,L,f,L,e,L,f,L"))
          (const_int 0)))
   (set (match_operand:SI     0 "metag_register_op"    "=e,e,f,f,r,r,r,r")
        (ashift:SI (match_dup 1)
                   (match_dup 2)))]
  ""
  "LSLS\\t%0, %1, %2\\t%@ (*ashls si rrx OK)"
  [(set_attr "type"    "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "ccstate" "set,set,set,set,set,set,set,set")])

(define_insn "*ashrs_<mode>_si_rrx"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ashiftrt:SI
            (match_operand:SI 1 "metag_reg_nofloat_op" "e,e,f,f,e,e,f,f")
            (match_operand:SI 2 "metag_regorint_op"    "e,L,f,L,e,L,f,L"))
         (const_int 0)))
   (set (match_operand:SI     0 "metag_register_op"   "=e,e,f,f,r,r,r,r")
        (ashiftrt:SI (match_dup 1)
                     (match_dup 2)))]
  ""
  "ASRS\\t%0, %1, %2\\t%@ (*ashrs si rrx OK)"
  [(set_attr "type"    "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "ccstate" "set,set,set,set,set,set,set,set")])

(define_insn "*lshrs_<mode>_si_rrx"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (lshiftrt:SI
            (match_operand:SI 1 "metag_reg_nofloat_op"  "e,e,f,f,e,e,f,f")
            (match_operand:SI 2 "metag_regorint_op"     "e,L,f,L,e,L,f,L"))
          (const_int 0)))
   (set (match_operand:SI     0 "metag_register_op"    "=e,e,f,f,r,r,r,r")
        (lshiftrt:SI (match_dup 1)
                     (match_dup 2)))]
  ""
  "LSRS\\t%0, %1, %2\\t%@ (lshrs si rrx OK)"
  [(set_attr "type"    "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "ccstate" "set,set,set,set,set,set,set,set")])

;; shift instructions setting NOOV flags using scratch
(define_insn "*tashls_<mode>_si_rrx"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ashift:SI
            (match_operand:SI 1 "metag_reg_nofloat_op"  "e,e,f,f,e,e,f,f")
            (match_operand:SI 2 "metag_regorint_op"     "e,L,f,L,e,L,f,L"))
          (const_int 0)))
   (clobber (match_scratch:SI 0                        "=e,e,f,f,r,r,r,r"))]
  ""
  "LSLS\\t%0, %1, %2\\t%@ (tashls si rrx OK)"
  [(set_attr "type"    "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "ccstate" "set,set,set,set,set,set,set,set")])

(define_insn "*tashrs_<mode>_si_rrx"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (ashiftrt:SI
            (match_operand:SI 1 "metag_reg_nofloat_op"  "e,e,f,f,e,e,f,f")
            (match_operand:SI 2 "metag_regorint_op"     "e,L,f,L,e,L,f,L"))
          (const_int 0)))
   (clobber (match_scratch:SI 0                        "=e,e,f,f,r,r,r,r"))]
  ""
  "ASRS\\t%0, %1, %2\\t%@ (tashrs si rrx OK)"
  [(set_attr "type"    "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "ccstate" "set,set,set,set,set,set,set,set")])

(define_insn "*tlshrs_<mode>_si_rrx"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (lshiftrt:SI
            (match_operand:SI 1 "metag_reg_nofloat_op"  "e,e,f,f,e,e,f,f")
            (match_operand:SI 2 "metag_regorint_op"     "e,L,f,L,e,L,f,L"))
          (const_int 0)))
   (clobber (match_scratch:SI 0                        "=e,e,f,f,r,r,r,r"))]
  ""
  "LSRS\\t%0, %1, %2\\t%@ (tlshrs si rrx OK)"
  [(set_attr "type"    "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "ccstate" "set,set,set,set,set,set,set,set")])

;; negate instructions

;; DImode negate
(define_expand "negdi2"
  [(parallel
     [(set (match_operand:DI         0 "metag_reg_nofloat_op" "")
           (neg:DI (match_operand:DI 1 "metag_reg_nofloat_op" "")))
      (clobber (reg:CC CC_REG))])]
  ""
  "")

(define_insn_and_split "*negdi2"
  [(set (match_operand:DI         0 "metag_reg_nofloat_op" "=d")
        (neg:DI (match_operand:DI 1 "metag_reg_nofloat_op"  "d")))
   (clobber (reg:CC CC_REG))]
  ""
  "#"
  "SPLIT_EARLY"
  [(parallel
     [(set (reg:CC_Z CC_REG)
           (compare:CC_Z (neg:SI (match_dup 5))
                         (const_int 0))) 
      (set (match_dup 3)
           (neg:SI (match_dup 5)))])
   (set (match_dup 4)
        (neg:SI (match_dup 6)))
   (set (match_dup 4)
        (if_then_else:SI (ne (reg:CC_Z CC_REG)
                             (const_int 0))
           (plus:SI (match_dup 4)
                    (const_int -1))
           (match_dup 4)))]
  {
    if (reload_completed)
      {
        operands[3] = gen_rtx_REG (SImode, REGNO (operands[0]));
        operands[4] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);

        operands[5] = gen_rtx_REG (SImode, REGNO (operands[1]));
        operands[6] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
      }
    else
      {
        operands[3] = simplify_gen_subreg (SImode, operands[0], DImode, 0);
        operands[4] = simplify_gen_subreg (SImode, operands[0], DImode, UNITS_PER_WORD);

        operands[5] = simplify_gen_subreg (SImode, operands[1], DImode, 0);
        operands[6] = simplify_gen_subreg (SImode, operands[1], DImode, UNITS_PER_WORD);
      }
  }
  [(set_attr "type"    "three")
   (set_attr "ccstate" "ccx")])

;; SImode negate
(define_expand "negsi2"
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "")
        (neg:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")))]
  ""
  "")

(define_insn "*negsi2"
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "=e,f")
        (neg:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "e,f")))]
  "!TARGET_METAC_1_1"
  "NEG\\t%0, %1\\t\\t%@ (neg si dd OK)"
  [(set_attr "type" "fast,fast")])

(define_insn "*negs_<mode>_si_rr"
  [(set (reg:<MODE> CC_REG)
      (compare:CCZNC
         (neg:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "e,f"))
         (const_int 0)))
   (set (match_operand:SI          0 "metag_reg_nofloat_op" "=e,f")
        (neg:SI (match_dup 1)))]
  "!TARGET_METAC_1_1"
  "NEGS\\t%0, %1\\t\\t%@ (negs si dd OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")])

(define_insn "*tneg_<mode>_si_rr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (neg:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "e,f"))
          (const_int 0)))
   (clobber (match_scratch:SI       0                        "=e,f"))]
  "!TARGET_METAC_1_1"
  "NEGS\\t%0, %1\\t\\t%@ (tneg si dd OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")])

;; negate instructions - v1.1 case
(define_insn "*negsi2_1_1"
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "=e, f")
        (neg:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "be,bf")))]
  "TARGET_METAC_1_1 && !TARGET_MINIM_CORE"
  "NEG\\t%0, %1\\t\\t%@ (neg si db OK)"
  [(set_attr "type" "fast,fast")
   (set_attr "o2rhint" "op1op0")])

;; negate instructions - minim case
(define_insn "*negsi2_minim"
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "=e,!e, f,!f")
        (neg:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "e, be,f, bf")))]
  "TARGET_METAC_1_1 && TARGET_MINIM_CORE"
  "NEG\\t%0, %1\\t\\t%@ (neg si db OK)"
  [(set_attr "type" "fast,fast,fast,fast")
   (set_attr "o2rhint" "op1op0")])

(define_insn "*negs_<mode>_si_rr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (neg:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "be,bf"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_reg_nofloat_op" "=e, f")
        (neg:SI (match_dup 1)))]
  "TARGET_METAC_1_1"
  "NEGS\\t%0, %1\\t\\t%@ (negs si db OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")
   (set_attr "o2rhint" "op1op0")])

(define_insn "*tneg_<mode>_si_rr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (neg:SI (match_operand:SI 1 "metag_reg_nofloat_op" "be,bf"))
          (const_int 0)))
   (clobber (match_scratch:SI       0                        "=e, f"))]
  "TARGET_METAC_1_1"
  "NEGS\\t%0, %1\\t\\t%@ (tneg si dx OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")
   (set_attr "o2rhint" "op1op0")])

;; complement instructions
(define_insn "one_cmplsi2"
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "=e,f,&e,&f")
        (not:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0,0, e, f")))]
  ""
  "@
   XOR\\t%0, %1, #-1\\t%@ (not si e0 OK)
   XOR\\t%0, %1, #-1\\t%@ (not si f0 OK)
   #
   #"
  [(set_attr "type" "fast,fast,two,two")])

;; Split the above insn if it needs more than one insn
(define_split
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "")
        (not:SI (match_operand:SI 1 "metag_reg_nofloat_op" "")))]
  "reload_completed
   && REGNO (operands[0]) != REGNO (operands[1])"
  [(set (match_dup 0)
        (const_int -1))
   (set (match_dup 0)
        (xor:SI (match_dup 0)
                (match_dup 1)))]
  "")

(define_insn "*nots_<mode>_si_rr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (not:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "0,0, e, f"))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_reg_nofloat_op" "=e,f,&e,&f")
        (not:SI (match_dup 1)))]
  ""
  "@
   XORS\\t%0, %1, #-1\\t%@ (nots si e0 OK)
   XORS\\t%0, %1, #-1\\t%@ (nots si f0 OK)
   #
   #"
  [(set_attr "type"    "fast,fast,two,two")
   (set_attr "ccstate" "set,set,fastset,fastset")])

;; Split the above insn if it needs more than one insn
(define_split
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (not:SI (match_operand:SI 1 "metag_reg_nofloat_op" ""))
          (const_int 0)))
   (set (match_operand:SI           0 "metag_reg_nofloat_op" "")
        (not:SI (match_dup 1)))]
  "reload_completed
   && REGNO (operands[0]) != REGNO (operands[1])"
  [(set (match_dup 0)
        (const_int -1))
   (parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (xor:SI (match_dup 0)
                    (match_dup 1))
            (const_int 0)))
     (set (match_dup 0)
          (xor:SI (match_dup 0)
                  (match_dup 1)))])]
  "")

(define_insn "*tnot_<mode>_si_rr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (not:SI (match_operand:SI 1 "metag_reg_nofloat_op" "0,0, e, f"))
          (const_int 0)))
   (clobber (match_scratch:SI       0                       "=e,f,&e,&f"))]
  ""
  "@
   XORS\\t%0, %1, #-1\\t%@ (tnot si e0 OK)
   XORS\\t%0, %1, #-1\\t%@ (tnot si f0 OK)
   #
   #"
  [(set_attr "type"    "fast,fast,two,two")
   (set_attr "ccstate" "set,set,fastset,fastset")])

;; Split the above insn if it needs more than one insn.
(define_split
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (not:SI (match_operand:SI 1 "metag_reg_nofloat_op" ""))
          (const_int 0)))
   (clobber (match_scratch:SI       0                        ""))]
  "reload_completed
   && REGNO (operands[0]) != REGNO (operands[1])"
  [(set (match_dup 0)
        (const_int -1))
   (parallel
    [(set (reg:<MODE> CC_REG)
          (compare:<MODE>
            (xor:SI (match_dup 0)
                    (match_dup 1))
            (const_int 0)))
     (set (match_dup 0)
          (xor:SI (match_dup 0)
                  (match_dup 1)))])]
  "")


;; Comparison operations

(define_expand "cmpsi"
  [(match_operand:SI 0 "metag_reg_nofloat_op" "")
   (match_operand:SI 1 "metag_regorint_op"    "")]
  ""
  {
    /* These are processed via the conditional branch define_expand's later */
    metag_compare_op0 = operands[0];
    metag_compare_op1 = operands[1];

    if (CONST_INT_P (operands[1]) 
        && METAG_LETTER_FOR_CONST (operands[1]) == 0)
      {
        /* Have to do register to register comparison for big constants */
        metag_compare_op1 = force_reg (SImode, operands[1]);
      }

    DONE;
  }
)

;; compare si instruction
(define_insn "*cmpsi_<mode>_rr"
  [(set (reg:<MODE> CC_REG)
        (compare:CCANY 
          (match_operand:SI 0 "metag_reg_nofloat_op" "e,f")
          (match_operand:SI 1 "metag_reg_nofloat_op" "e,f")))]
  "!TARGET_METAC_1_1"
  "@
   CMP\\t%0, %1\\t\\t%@ (*cmpsi ee OK)
   CMP\\t%0, %1\\t\\t%@ (*cmpsi ff OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")])

(define_insn "*cmpsi_<mode>_ri"
  [(set (reg:<MODE> CC_REG)
        (compare:CCANY
          (match_operand:SI 0 "metag_reg_nofloat_op" "d,d, d,d,d")
          (match_operand:SI 1 "metag_int_operand"    "K,IP,J,M,N")))]
  ""
  "@
   CMP\\t%0, %1\\t\\t%@ (*cmpsi dI OK)
   CMP\\t%0, %1\\t\\t%@ (*cmpsi dI OK)
   CMPT\\t%0, #HI(%c1)\\t\\t%@ (*cmpsi dJ OK)
   CMPMB\\t%0, #LO(%c1)\\t\\t%@ (*cmpsi dM OK)
   CMPMT\\t%0, #HI(%c1)\\t\\t%@ (*cmpsi dN OK)"
  [(set_attr "type"    "fast,fast,fast,fast,fast")
   (set_attr "ccstate" "set,set,set,set,set")])

;; compare si instruction - v1.1 case
(define_insn "*cmpsi_rr_1_1"
  [(set (reg:<MODE> CC_REG)
        (compare:CCANY
          (match_operand:SI 0 "metag_reg_nofloat_op" "e, f")
          (match_operand:SI 1 "metag_reg_nofloat_op" "be,bf")))]
  "TARGET_METAC_1_1"
  "@
   CMP\\t%0, %1\\t\\t%@ (*cmpsi eb OK)
   CMP\\t%0, %1\\t\\t%@ (*cmpsi fb OK)"
  [(set_attr "type"    "fast,fast")
   (set_attr "ccstate" "set,set")
   (set_attr "o2rhint" "op1op0")])

;; compare hi instruction for zero flag
(define_insn "*tst_zhi"
  [(set (reg:CC_Z CC_REG)
        (compare:CC_Z
          (match_operand:HI 0 "metag_datareg_op" "d")
          (const_int 0)))]
  ""
  "TST\\t%0, #0xFFFF\\t%@ (*tst zhi d OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

;; compare qi instruction for zero flag
(define_insn "*tst_zqi"
  [(set (reg:CC_Z CC_REG)
        (compare:CC_Z
          (match_operand:QI 0 "metag_datareg_op" "d")
          (const_int 0)))]
  ""
  "TST\\t%0, #255\\t%@ (*tst zqi d OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

;; Copy and compare combines the flag setup with a move
;; Note MOVS/ADDS for DU -> FX is allowed but FX -> DU is not
(define_insn "*cmpsi_movsi_<mode>_eq0"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (match_operand:SI 1 "metag_datareg_op"      "e,f,d")
          (const_int 0)))
   (set (match_operand:SI   0 "metag_reg_nofloat_op" "=e,f,da")
        (match_dup 1))]
  ""
  "ADDS\\t%0, %1, #0\\t\\t%@ (*movs si rr OK)"
  [(set_attr "type"    "fast,fast,slow")
   (set_attr "ccstate" "set")])

(define_insn "*cmpsi_movsi_cc_eq0"
  [(set (reg:CC CC_REG)
        (compare:CC
          (match_operand:SI 1 "metag_datareg_op"      "e,f,d")
          (const_int 0)))
   (set (match_operand:SI   0 "metag_reg_nofloat_op" "=e,f,da")
        (match_dup 1))]
  ""
  "SUBS\\t%0, %1, #0\\t\\t%@ (*movs si rr OK)"
  [(set_attr "type"    "fast,fast,slow")
   (set_attr "ccstate" "set")])

;; min instruction

(define_insn "sminsi3"
  [(set (match_operand:SI          0 "metag_datareg_op" "=e,f")
        (smin:SI (match_operand:SI 1 "metag_datareg_op" "%e,f")
                 (match_operand:SI 2 "metag_datareg_op"  "e,f")))
   (clobber (reg:CC CC_REG))]
  "!TARGET_METAC_0_1"
  "MIN\\t%0, %1, %2\\t%@ (*min si ddd OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "ccx")])

;; max instruction

(define_insn "smaxsi3"
  [(set (match_operand:SI          0 "metag_reg_nofloat_op" "=e,f")
        (smax:SI (match_operand:SI 1 "metag_reg_nofloat_op" "%e,f")
                 (match_operand:SI 2 "metag_reg_nofloat_op"  "e,f")))
   (clobber (reg:CC CC_REG))]
  "!TARGET_METAC_0_1"
  "MAX\\t%0, %1, %2\\t%@ (*max si ddd OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "ccx")])

;; abs instruction

(define_insn "abssi2"
  [(set (match_operand:SI         0 "metag_reg_nofloat_op" "=e,f")
        (abs:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "e,f")))
   (clobber (reg:CC CC_REG))]
  "!TARGET_METAC_0_1"
  "ABS\\t%0, %1\\t%@ (*abs si dd OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "ccx")])

;; Conditional move support
(define_expand "movsicc"
  [(set (match_operand:SI                  0 "metag_register_op"   "")
        (if_then_else:SI (match_operand    1 "comparison_operator" "")
                         (match_operand:SI 2 "metag_regorint_op"   "")
                         (match_operand:SI 3 "metag_regorint_op"   "")))]
  ""
  {
    enum rtx_code code = GET_CODE (operands[1]);
    enum machine_mode mode;
    rtx ccreg;

    if (CONST_INT_P (operands[2]) && CONST_INT_P (operands[3]))
      {
        /* Can only support -255 to 255 delta between constants */
        HOST_WIDE_INT op2_mi_op3 = INTVAL (operands[2]) - INTVAL (operands[3]);
        rtx           value      = GEN_INT (op2_mi_op3);

        if (satisfies_constraint_P (value)
            || satisfies_constraint_K (value))
          {
            rtx temp = (reload_in_progress || reload_completed)
                        ? operands[0] : gen_reg_rtx (SImode);

            emit_move_insn (temp, operands[3]);
            operands[2] = gen_rtx_PLUS (SImode, temp, value);
            operands[3] = temp;
          }
      }

    if (CONST_INT_P (operands[3]))
      {
        /* Make second source operand a register */
        operands[3] = force_reg (SImode, operands[3]);
      }

    if (CONST_INT_P (operands[2]))
      {
        /* Make first source operand a register! */
        operands[2] = force_reg (SImode, operands[2]);
      }

    /* Generate correct comparison insn */
    mode = SELECT_CC_MODE (code, metag_compare_op0, metag_compare_op1);
    ccreg = gen_rtx_REG (mode, CC_REG);
    emit_insn (gen_rtx_SET (VOIDmode, ccreg,
                            gen_rtx_COMPARE (mode, metag_compare_op0, metag_compare_op1)));

    /* Expand condition to act on result */
    operands[1] = gen_rtx_fmt_ee (code, VOIDmode, ccreg, const0_rtx);
  }
)

(define_insn "*mov_if_<mode>_rr0"
  [(set (match_operand:SI                         0 "metag_reg_nofloat_op" "=e,f,h,l,da")
        (if_then_else:SI (match_operator          1 "comparison_operator"
                          [(match_operand:CCALL   2 "metag_<mode>_reg" "")
                           (const_int 0)])
                         (match_operand:SI        3 "metag_reg_nofloat_op"  "e,f,h,l,da")
                         (match_operand:SI        4 "metag_reg_nofloat_op"  "0,0,0,0,0")))]
  ""
  "MOV%z1\\t%0, %3\\t%@ (*mov if <mode> rr0 OK)"
  [(set_attr "type"    "fast,fast,fast,fast,slow")
   (set_attr "ccstate" "xcc,xcc,xcc,xcc,xcc")])

;; Conditional add is targeted by expansion of if_then_else(const, const)
(define_insn "*add_if_<mode>_r0KP"
  [(set (match_operand:SI                           0 "metag_reg_nofloat_op" "=e,f,h,l,e,f,h,l")
        (if_then_else:SI (match_operator            1 "comparison_operator"
                          [(match_operand:CCALL     2 "metag_<mode>_reg" "")
                           (const_int 0)])
                         (plus:SI (match_operand:SI 3 "metag_regnofrm_op"     "e,f,h,l,e,f,h,l")
                                  (match_operand:SI 4 "metag_smallint_op"     "K,K,K,K,P,P,P,P"))
                         (match_operand:SI          5 "metag_reg_nofloat_op"  "0,0,0,0,0,0,0,0")))]
  ""
  "@
   ADD%z1\\t%0, %3, %4\\t\\t%@ (*add if eeK OK)
   ADD%z1\\t%0, %3, %4\\t\\t%@ (*add if ffK OK)
   ADD%z1\\t%0, %3, %4\\t\\t%@ (*add if hhK OK)
   ADD%z1\\t%0, %3, %4\\t\\t%@ (*add if llK OK)
   SUB%z1\\t%0, %3, #%n4\\t\\t%@ (*sub if eeP OK)
   SUB%z1\\t%0, %3, #%n4\\t\\t%@ (*sub if ffP OK)
   SUB%z1\\t%0, %3, #%n4\\t\\t%@ (*sub if hhP OK)
   SUB%z1\\t%0, %3, #%n4\\t\\t%@ (*sub if llP OK)"
  [(set_attr "type"    "fast,fast,fast,fast,fast,fast,fast,fast")
   (set_attr "ccstate" "xcc,xcc,xcc,xcc,xcc,xcc,xcc,xcc")])


;; zero/sign extend instructions

(define_insn_and_split "zero_extendsidi2"
  [(set (match_operand:DI     0 "metag_reg_nofloat_op" "=da")
        (zero_extend:DI
            (match_operand:SI 1 "metag_reg_nofloat_op" "da")))]
  ""
  "#"
  "SPLIT_EARLY"
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 3)
        (const_int 0))]
  {
    if (reload_completed)
      {
        operands[2] = gen_rtx_REG (SImode, REGNO (operands[0]));
        operands[3] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
      }
    else
      {
        operands[2] = simplify_gen_subreg (SImode, operands[0], DImode, 0);
        operands[3] = simplify_gen_subreg (SImode, operands[0], DImode, UNITS_PER_WORD);
      }
  }
  [(set_attr "type" "two")]
)

(define_expand "zero_extendhisi2"
  [(set (match_operand:SI     0 "metag_reg_nofloat_op" "")
        (zero_extend:SI
            (match_operand:HI 1 "metag_datareg_op"     "")))]
  ""
  {
  }
)

(define_insn_and_split "*zero_extendhisi2"
  [(set (match_operand:SI   0 "metag_datareg_op" "=d")
        (zero_extend:SI
          (match_operand:HI 1 "metag_datareg_op"  "0")))]
  ""
  "#"
  "SPLIT_EARLY"
  [(set (match_dup 0)
        (and:SI (match_dup 1)
                (match_dup 2)))]
  {
    operands[1] = gen_lowpart (SImode, operands[1]);
    operands[2] = gen_int_mode (0xFFFF, SImode);
  })

(define_expand "zero_extendqisi2"
  [(set (match_operand:SI     0 "metag_reg_nofloat_op" "")
        (zero_extend:SI
            (match_operand:QI 1 "metag_datareg_op"     "")))]
  ""
  {
  }
)

(define_insn_and_split "*zero_extendqisi2"
  [(set (match_operand:SI   0 "metag_reg_nofloat_op" "=da")
        (zero_extend:SI
          (match_operand:QI 1 "metag_datareg_op"      "d")))]
  ""
  "#"
  "SPLIT_EARLY"
  [(set (match_dup 0)
        (and:SI (match_dup 1)
                (match_dup 2)))]
  {
    operands[1] = gen_lowpart (SImode, operands[1]);
    operands[2] = gen_int_mode (0xFF, SImode);
  })

(define_insn_and_split "extendsidi2"
  [(set (match_operand:DI     0 "metag_reg_nofloat_op" "=d,d,a")
        (sign_extend:DI
            (match_operand:SI 1 "metag_datareg_op"      "f,e,d")))]
  ""
  "#"
  "SPLIT_EARLY"
  [(set (match_dup 2)
        (match_dup 1))
   (set (match_dup 3)
        (ashiftrt:SI  (match_dup 1)
                      (const_int 31)))]
  {
    if (reload_completed)
      {
        operands[2] = gen_rtx_REG (SImode, REGNO (operands[0]));
        operands[3] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
      }
    else
      {
        operands[2] = simplify_gen_subreg (SImode, operands[0], DImode, 0);
        operands[3] = simplify_gen_subreg (SImode, operands[0], DImode, UNITS_PER_WORD);
      }
  }
  [(set_attr "type" "two,slowslow,slowslow")]
)

(define_expand "extendhisi2"
  [(set (match_operand:SI     0 "metag_reg_nofloat_op" "")
        (sign_extend:SI
            (match_operand:HI 1 "metag_datareg_op"     "")))]
  ""
  {
  }
)

(define_expand "extendqisi2"
  [(set (match_operand:SI     0 "metag_reg_nofloat_op" "")
        (sign_extend:SI
            (match_operand:QI 1 "metag_datareg_op"     "")))]
  ""
  {
  }
)

;; -----------------------------------------------------------------------------
;; | Matching zero extends loads to HI post/pre_inc/dec/modify
;; -----------------------------------------------------------------------------

(define_insn "*lodz_<mode>hi_post_inc"
  [(set (match_operand:HI                     0 "metag_register_op"    "=cr")
        (zero_extend:HI 
           (mem:EXTHI 
               (post_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da")))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1++]\\t%@ (*lodz <MODE> HI post_inc OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>hi_post_dec"
  [(set (match_operand:HI                     0 "metag_register_op"    "=cr")
        (zero_extend:HI 
           (mem:EXTHI
               (post_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da")))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1--]\\t%@ (*lodz <MODE> HI post_dec OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>hi_pre_inc"
  [(set (match_operand:HI                    0 "metag_register_op"    "=cr")
        (zero_extend:HI 
           (mem:EXTHI
               (pre_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da")))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [++%1]\\t%@ (*lodz <MODE> HI pre_inc OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>hi_pre_dec"
  [(set (match_operand:HI                    0 "metag_register_op"    "=cr")
        (zero_extend:HI
           (mem:EXTHI
               (pre_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da")))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [--%1]\\t%@ (*lodz <MODE> HI pre_dec OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>hi_post_modify_disp"
  [(set (match_operand:HI                    0 "metag_register_op"    "=cr")
        (zero_extend:HI 
           (mem:EXTHI
               (post_modify:SI
                  (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                  (plus:SI (match_dup 1)
                           (match_operand:SI 2 "metag_offset6_<mode>"  "<O>"))))))]
  ""
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1+%2++]\\t%@ (*load <MODE> HI post_modify_disp OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>hi_post_modify_reg"
  [(set (match_operand:HI                    0 "metag_register_op"    "=cr,cr,cr,cr")
        (zero_extend:HI
           (mem:EXTHI
               (post_modify:SI
                  (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                  (plus:SI (match_dup 1)
                           (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l"))))))]
  ""
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1+%2++]\\t%@ (*lodz <MODE> HI post_modify_reg OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>hi_pre_modify_disp"
  [(set (match_operand:HI                    0 "metag_reg_nofloat_op" "=da")
        (zero_extend:HI
           (mem:EXTHI
               (pre_modify:SI
                  (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
                  (plus:SI (match_dup 1)
                           (match_operand:SI 2 "metag_offset6_<mode>" "<O>"))))))]
  ""
  "GET<W>\\t%0, [%1++%2]\\t%@ (*lodz <MODE> HI pre_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>hi_pre_modify_reg"
  [(set (match_operand:HI                    0 "metag_register_op"    "=cr,cr,cr,cr")
        (zero_extend:HI
           (mem:EXTHI
               (pre_modify:SI
                  (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
                  (plus:SI (match_dup 1)
                           (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l"))))))]
  ""
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1++%2]\\t%@ (*lodz <MODE> HI pre_modify_reg OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; zero_extend loads to SI from EXTSI mode using base+index
(define_insn "*lodz_<mode>si_rma"
  [(set (match_operand:SI                 0 "metag_register_op" "=cr,cr,cr,cr")
        (zero_extend:SI
           (mem:EXTSI
               (plus:SI (match_operand:SI 1 "metag_regnofrm_op"  "e, f, h, l")
                        (match_operand:SI 2 "metag_regnofrm_op"  "e, f, h, l")))))]
  ""
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1+%2]\\t%@ (*lodz <MODE> SI rma OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; zero_extend loads to SI from EXTSI mode using base+offset6
(define_insn "*lodz_<mode>si_rmi"
  [(set (match_operand:SI                 0 "metag_register_op"    "=cr")
        (zero_extend:SI 
           (mem:EXTSI
               (plus:SI (match_operand:SI 1 "metag_regnofrm_op"     "da")
                        (match_operand:SI 2 "metag_offset6_<mode>"  "<O>")))))]
  ""
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1+%2]\\t%@ (*lodz <MODE> SI rmi OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; zero_extend loads to SI from EXTSI mode using base+offset12
(define_insn "*lodz_<mode>si_rmi"
  [(set (match_operand:SI                 0 "metag_reg_nofloat_op"  "=da, da")
        (zero_extend:SI 
           (mem:EXTSI
               (plus:SI (match_operand:SI 1 "metag_reg12bit_op"      "da, Yr")
                        (match_operand:SI 2 "metag_offset12_<mode>"  "<O>,<Z>")))))]
  ""
  "GET<W>\\t%0, [%1+%2]\\t%@ (*lodz <MODE> SI rmi OK)"
  [(set_attr "type" "load")])

;; zero_extend loads to SI from EXTSI mode using base++index
(define_insn "*lodz_<mode>si_rmab"
  [(set (match_operand:SI                 3 "metag_register_op" "=cr,cr,cr,cr")
        (zero_extend:SI 
           (mem:EXTSI
               (plus:SI (match_operand:SI 1 "metag_regnofrm_op" "%0, 0, 0, 0")
                        (match_operand:SI 2 "metag_regnofrm_op"  "e, f, h, l")))))
   (set (match_operand:SI                 0 "metag_regnofrm_op" "=e, f, h, l")
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "0"
  {
    static const char fmt[] = "F\\tGET<W>\\t%3, [%1++%2]\\t%@ (*lodz <MODE> SI rmab OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[3])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; zero_extend loads to SI from EXTSI mode using base++increment
(define_insn "*lodz_<mode>si_rmib"
  [(set (match_operand:SI                 3 "metag_register_op"    "=cr")
        (zero_extend:SI
           (mem:EXTSI
               (plus:SI (match_operand:SI 1 "metag_regnofrm_op"     "0")
                        (match_operand:SI 2 "metag_offset6_<mode>"  "<O>")))))
   (set (match_operand:SI                 0 "metag_regnofrm_op"    "=da")
        (plus:SI (match_dup 1)
                 (match_dup 2)))]
  "0"
  {
    static const char fmt[] = "F\\tGET<W>\\t%3, [%1++%2]\\t%@ (*lodz <MODE> SI rmib OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[3])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------
 
;; -----------------------------------------------------------------------------
;; | Matching zero extend loads to SI post/pre_inc/dec/modify                    |
;; -----------------------------------------------------------------------------

(define_insn "*lodz_<mode>si_post_inc"
  [(set (match_operand:SI                  0 "metag_register_op"    "=cr")
        (zero_extend:SI (mem:EXTSI
            (post_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da")))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1++]\\t%@ (*lodz <MODE> SI post_inc OK)";
    
    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>si_post_dec"
  [(set (match_operand:SI                  0 "metag_register_op"    "=cr")
        (zero_extend:SI (mem:EXTSI
            (post_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da")))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1--]\\t%@ (*lodz <MODE> SI post_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>si_pre_inc"
  [(set (match_operand:SI                 0 "metag_register_op"    "=cr")
        (zero_extend:SI (mem:EXTSI
            (pre_inc:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da")))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [++%1]\\t%@ (*lodz <MODE> SI pre_inc OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>si_pre_dec"
  [(set (match_operand:SI                 0 "metag_register_op"    "=cr")
        (zero_extend:SI (mem:EXTSI
            (pre_dec:SI (match_operand:SI 1 "metag_reg_nofloat_op" "+da")))))]
  "TARGET_METAC_1_1"
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [--%1]\\t%@ (*lodz <MODE> SI pre_dec OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>si_post_modify_disp"
  [(set (match_operand:SI                 0 "metag_register_op"    "=r")
        (zero_extend:SI (mem:EXTSI
            (post_modify:SI
               (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
               (plus:SI (match_dup 1)
                        (match_operand:SI 2 "metag_offset6_<mode>" "<O>"))))))]
  ""
  "GET<W>\\t%0, [%1+%2++]\\t%@ (*lodz <MODE> SI post_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>si_post_modify_reg"
  [(set (match_operand:SI                 0 "metag_register_op"    "=cr,cr,cr,cr")
        (zero_extend:SI (mem:EXTSI
            (post_modify:SI
               (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
               (plus:SI (match_dup 1)
                        (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l"))))))]
  ""
  {
    static const char fmt[] = "F\\tGET<W>\\t%0, [%1+%2++]\\t%@ (*lodz <MODE> SI post_modify_reg OK)";

    return &fmt[METAG_FPC_REG_P (REGNO (operands[0])) ? 0 : 2];
  }
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>si_pre_modify_disp"
  [(set (match_operand:SI                 0 "metag_reg_nofloat_op" "=da")
        (zero_extend:SI (mem:EXTSI
            (pre_modify:SI
               (match_operand:SI          1 "metag_reg_nofloat_op" "+da")
               (plus:SI (match_dup 1)
                        (match_operand:SI 2 "metag_offset6_<mode>" "<O>"))))))]
  ""
  "GET<W>\\t%0, [%1++%2]\\t%@ (*load <MODE> SI pre_modify_disp OK)"
  [(set_attr "type" "load")])

(define_insn "*lodz_<mode>si_pre_modify_reg"
  [(set (match_operand:SI                 0 "metag_reg_nofloat_op" "=da,da,da,da")
        (zero_extend:SI (mem:EXTSI
            (pre_modify:SI
               (match_operand:SI          1 "metag_reg_nofloat_op" "+e, f, h, l")
               (plus:SI (match_dup 1)
                        (match_operand:SI 2 "metag_reg_nofloat_op"  "e, f, h, l"))))))]
  ""
  "GET<W>\\t%0, [%1++%2]\\t%@ (*loadz <MODE> SI pre_modify_reg OK)"
  [(set_attr "type" "load")])

;; zero_extend loads to SI from EXTSI mode - rest expanded as AND operations
(define_insn "*lodz_<mode>si"
  [(set (match_operand:SI                    0 "metag_reg_nofloat_op" "=da")
        (zero_extend:SI (match_operand:EXTSI 1 "memory_operand"        "m")))]
  ""
  "GET<W>\\t%0, %1\\t%@ (*lodz <MODE> SI rm OK)"
  [(set_attr "type" "load")])

;; -----------------------------------------------------------------------------

;; Sign extend register to SI mode register moves
(define_insn "*sign_extend_<mode>si"
  [(set (match_operand:SI                    0 "metag_reg_nofloat_op" "=d,e,f")
        (sign_extend:SI (match_operand:EXTSI 1 "metag_reg_nofloat_op"  "0,e,f")))]
  ""
  "@
   XSD<W>\\t%0, %1\\t\\t%@ (*ext <MODE> SI d0 OK)
   XSD<W>\\t%0, %1\\t\\t%@ (*ext <MODE> SI ee OK)
   XSD<W>\\t%0, %1\\t\\t%@ (*ext <MODE> SI ff OK)"
  [(set_attr "type" "fast,fast,fast")])

(define_insn "*sign_extend_hisi_<mode>"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (sign_extend:SI (match_operand:HI 1 "metag_datareg_op"  "0,e,f"))
          (const_int 0)))
   (set (match_operand:SI                   0 "metag_datareg_op" "=d,e,f")
        (sign_extend:SI (match_dup 1)))]
  ""
  "XSDSW\\t%0, %1\\t\\t%@ (*exts HI SI dd OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

(define_insn "*sign_extend_qisi_<mode>"
  [(set (reg:<MODE> CC_REG)
        (compare:CCZNC
          (sign_extend:SI (match_operand:QI 1 "metag_datareg_op"  "0,e,f"))
          (const_int 0)))
   (set (match_operand:SI                   0 "metag_datareg_op" "=d,e,f")
        (sign_extend:SI (match_dup 1)))]
  ""
  "XSDSB\\t%0, %1\\t\\t%@ (*exts QI SI dd OK)"
  [(set_attr "type"    "fast")
   (set_attr "ccstate" "set")])

;; bit field instructions


;; Low overhead loop support
;; operand 0 is the loop count pseudo register
;; operand 1 is the number of loop iterations or 0 if it is unknown
;; operand 2 is the maximum number of loop iterations or -1 if unknown
;; operand 3 is the number of levels of enclosed loops
;; operand 4 is the label to jump to at the top of the loop
(define_expand "doloop_end"
  [(use (match_operand            0 ""                  ""))
   (use (match_operand:SI         1 "const_int_operand" ""))
   (use (match_operand:SI         2 "const_int_operand" ""))
   (use (match_operand:SI         3 "const_int_operand" ""))
   (use (label_ref (match_operand 4 ""                  "")))]
  ""
  {
    enum machine_mode mode = GET_MODE (operands[0]);

    if (mode == SImode && INTVAL (operands[3]) != -1)
    {
      unsigned HOST_WIDE_INT mask           = GET_MODE_MASK (mode);
      unsigned HOST_WIDE_INT num_iterations = INTVAL (operands[1]) & mask;
      unsigned HOST_WIDE_INT max_iterations = INTVAL (operands[2]) & mask;
      unsigned HOST_WIDE_INT limit          = INTVAL (gen_int_mode (0xFFFFFFFF, SImode)) & mask;
    
      if (!(num_iterations > limit || (num_iterations == 0 && max_iterations > limit)))
      {
        emit_jump_insn (gen_br_si_txrpt (operands[4], operands[0]));
        DONE;
      }
    }

    FAIL;
  }
)

(define_insn "br_si_txrpt"
  [(set (pc)
        (if_then_else (ne (match_operand:SI     1 "metag_txrpt_op" "+Wx")
                          (const_int 0))
                      (label_ref (match_operand 0 ""               ""))
                      (pc)))
   (set (match_dup 1)
        (plus:SI (match_dup 1)
                 (const_int -1)))
   (clobber (reg:CC_NOOV CC_REG))]
  ""
  "BR\\t%c0"
  [(set_attr "type" "branch")
   (set_attr "ccstate" "ccx")])


;; conditional branch instruction generators; expand previous compare

(define_expand "b<code>"
  [(set (pc)
        (if_then_else (CCANYCOND (match_dup 1)
                                 (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  {
    if (!gen_metag_compare (<CODE>, operands, 1))
      FAIL;
  }
)

;; patterns to match conditional branch insns

(define_insn "*b<mode>"
  [(set (pc)
        (if_then_else (match_operator           1 "comparison_operator"
                       [(match_operand:CCALL    2 "metag_<mode>_reg" "")
                        (const_int 0)])
                      (label_ref (match_operand 0 ""                 ""))
                      (pc)))]
  ""
  {
    if (metag_consume_branch (insn))
      return "";

    return "B%z1\\t%c0\\t\\t\\t%@ (*b<mode> OK)";
  }
  [(set_attr "type"    "branch")
   (set_attr "ccstate" "xcc")])

(define_insn "*b<mode>_reversed"
  [(set (pc)
        (if_then_else (match_operator           1 "comparison_operator"
                       [(match_operand:CCALL    2 "metag_<mode>_reg" "")
                        (const_int 0)])
                      (pc)
                      (label_ref (match_operand 0 ""                 ""))))]
  ""
  {
    if (metag_consume_branch (insn))
      return "";

    return "B%Z1\\t%c0\\t\\t\\t%@ (*b<mode> rev OK)";
  }
  [(set_attr "type"    "branch")
   (set_attr "ccstate" "xcc")])

;; condition status evaluation
(define_expand "s<code>"
  [(set (match_operand:SI 0 "metag_reg_nofloat_op" "")
        (CCANYCOND:SI (match_dup 1)
                      (const_int 0)))]
  ""
  {
    if (!gen_metag_compare (<CODE>, operands, 1))
      FAIL;
  }
)

;; patterns to match condition status insns
(define_insn_and_split "*movsi_m<mode>"
  [(set (match_operand:SI       0 "metag_reg_nofloat_op" "=da")
        (match_operator:SI      1 "comparison_operator"
         [(match_operand:CCALL  2 "metag_<mode>_reg"      "")
          (const_int 0)]))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (const_int 0))
   (set (match_dup 0)
        (if_then_else:SI (match_op_dup 1 [(match_dup 2)
                                          (const_int 0)])
                         (plus:SI (match_dup 0)
                                  (const_int 1))
                         (match_dup 0)))]
  ""
  [(set_attr "type"    "two")
   (set_attr "ccstate" "xcc")])

(define_insn_and_split "*movsi_negm<mode>"
  [(set (match_operand:SI               0 "metag_reg_nofloat_op" "=da")
        (neg:SI (match_operator:SI      1 "comparison_operator"
                 [(match_operand:CCALL  2 "metag_<mode>_reg"      "")
                  (const_int 0)])))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (const_int 0))
   (set (match_dup 0)
        (if_then_else:SI (match_op_dup 1 [(match_dup 2)
                                          (const_int 0)])
                         (plus:SI (match_dup 0)
                                  (const_int -1))
                         (match_dup 0)))]
  ""
  [(set_attr "type"    "two")
   (set_attr "ccstate" "xcc")])

(define_insn_and_split "*movsi_notm<mode>"
  [(set (match_operand:SI               0 "metag_reg_nofloat_op" "=da")
        (not:SI (match_operator:SI      1 "comparison_operator"
                 [(match_operand:CCALL  2 "metag_<mode>_reg"      "")
                  (const_int 0)])))]
  ""
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (const_int 0))
   (set (match_dup 0)
        (if_then_else:SI (match_op_dup 1 [(match_dup 2)
                                          (const_int 0)])
                         (plus:SI (match_dup 0)
                                  (const_int -2))
                         (match_dup 0)))]
  ""
  [(set_attr "type"    "two")
   (set_attr "ccstate" "xcc")])

;; call instructions - can handle call to symbol hence special predicate
(define_expand "sibcall"
  [(parallel [(call (match_operand:QI 0 "metag_call_addr" "")
                    (match_operand    1 ""                ""))
              (unspec [(const_int 0)] UNSPEC_SIBCALL)])]
  ""
  {
    if (GET_CODE (operands[0]) != MEM)
      {
        rtx tmp = gen_rtx_REG (SImode, D0Re0_REG);

        emit_move_insn (tmp, operands[0]);
        operands[0] = tmp;
      }
  }
)

(define_expand "call"
  [(call (match_operand:QI 0 "metag_call_addr" "")
         (match_operand    1 ""                ""))]
  ""
  "")

(define_expand "call_value"
  [(set (match_operand          0 "metag_reg_nofloat_op" "")
        (call (match_operand:QI 1 "metag_call_addr"      "")
              (match_operand    2 ""                     "")))]
  ""
  "")

(define_expand "sibcall_value"
  [(parallel [(set (match_operand          0 "metag_reg_nofloat_op"  "")
                   (call (match_operand:QI 1 "metag_call_addr"       "")
                         (match_operand    2 ""                      "")))
              (unspec [(const_int 0)] UNSPEC_SIBCALL_VALUE)])]
  ""
  {
    if (GET_CODE (operands[1]) != MEM)
      {
        rtx tmp = gen_rtx_REG (SImode, D1Re0_REG);

        emit_move_insn (tmp, operands[1]);
        operands[1] = tmp;
      }
  }
)

(define_insn "*sibcall_reg"
  [(call (mem:QI (match_operand:SI 0 "metag_addrreg_op"  "a"))
         (match_operand:SI         1 "immediate_operand" ""))
   (unspec [(const_int 0)] UNSPEC_SIBCALL)]
  ""
  "MOV\\tPC, %0"
  [(set_attr "type" "unknown")])

(define_insn "*call_reg"
  [(call (mem:QI (match_operand:SI 0 "metag_reg_nofloat_op" "da"))
         (match_operand:SI         1 "immediate_operand"    ""))]
  ""
  "*
   return output_call (operands, 0);"
  [(set_attr "type" "unknown")
   (set_attr "length" "8")])

(define_insn "*sibcall_value_reg"
  [(set (match_operand                  0 "metag_reg_nofloat_op" "=da")
        (call (mem:QI (match_operand:SI 1 "metag_addrreg_op"      "a"))
              (match_operand:SI         2 "immediate_operand"     "")))
   (unspec [(const_int 0)] UNSPEC_SIBCALL_VALUE)]
  ""
  "MOV\\tPC, %1"
  [(set_attr "type" "unknown")])

(define_insn "*call_value_reg"
  [(set (match_operand                  0 "metag_reg_nofloat_op" "=da")
        (call (mem:QI (match_operand:SI 1 "metag_reg_nofloat_op"  "da"))
              (match_operand:SI         2 "immediate_operand"     "")))]
  ""
  "*
   return output_call (operands, 1);"
  [(set_attr "type" "unknown")
   (set_attr "length" "8")])

(define_insn "*sibcall_sym"
  [(call (mem:QI (match_operand:SI 0 "symbolic_operand"  ""))
         (match_operand:SI         1 "immediate_operand" ""))
   (unspec [(const_int 0)] UNSPEC_SIBCALL)]
  ""
  "*
   return output_sibcall (operands, 0);"
  [(set_attr "type" "branch")
   (set_attr "length" "8")])

(define_insn "*call_sym"
  [(call (mem:QI (match_operand:SI 0 "symbolic_operand"  ""))
         (match_operand:SI         1 "immediate_operand" ""))]
  ""
  "*
   return output_call (operands, 0);"
  [(set_attr "type" "unknown")
   (set (attr "length")
        (if_then_else
          (eq (symbol_ref "metag_tbiassert_p (operands[0])") (const_int 0))
          (const_int 8)
          (const_int 12)))])

(define_insn "*sibcall_value_sym"
  [(set (match_operand                  0 "metag_reg_nofloat_op" "=da")
        (call (mem:QI (match_operand:SI 1 "symbolic_operand"      ""))
              (match_operand:SI         2 "immediate_operand"     "")))
   (unspec [(const_int 0)] UNSPEC_SIBCALL_VALUE)]
  ""
  "*
   return output_sibcall (operands, 1);"
  [(set_attr "type" "branch")
   (set_attr "length" "8")])

(define_insn "*call_value_sym"
  [(set (match_operand                  0 "metag_reg_nofloat_op" "=da")
        (call (mem:QI (match_operand:SI 1 "symbolic_operand"      ""))
              (match_operand:SI         2 "immediate_operand"     "")))]
  ""
  "*
   return output_call (operands, 1);"
  [(set_attr "type" "unknown")
   (set (attr "length")
        (if_then_else
          (eq (symbol_ref "metag_tbiassert_p (operands[1])") (const_int 0))
          (const_int 8)
          (const_int 12)))])

;; Return instruction
(define_insn "return_internal"
  [(use (reg:SI D1RtP_REG))
   (return)]
  ""
  {
    /* META 2 and unconditional return and no return stub emitted */
    if (!metag_cond_exec_p ()
        && current_insn_predicate == NULL_RTX
        && TARGET_METAC_2_1
        && cfun->machine->cond_return_state != METAG_COND_RETURN_DONE)
      return metag_gen_cond_return_stub ();
    else if (!TARGET_METAC_2_1                  /* META 1.2 or unconditional returns */
             || (!metag_cond_exec_p ()
                 && current_insn_predicate == NULL_RTX))
      return "MOV%?\\tPC, D1RtP";
    else
      return metag_gen_cond_return_branch ("B%%?\\t$LX%d %%@\\t(* cond return stub)");
  }
  [(set_attr "type" "unknown")
   (set_attr "cond" "yes")])

(define_insn "return_internal_cond_<mode>"
  [(set (pc)
        (if_then_else (match_operator         0 "comparison_operator"
                       [(match_operand:CCANY  1 "metag_<mode>_reg" "")
                        (unspec [(const_int 0)] UNSPEC_RET_COND)])
                       (return)
                       (pc)))]
  ""
  {
    if (!TARGET_METAC_2_1)
      return "MOV%z0\\tPC, D1RtP";
    else
      return metag_gen_cond_return_branch ("B%%z0\\t$LX%d %%@\\t(* cond return stub)");
  }
  [(set_attr "type" "unknown")])

(define_insn "return_internal_cond_inverted_<mode>"
  [(set (pc)
        (if_then_else (match_operator         0 "comparison_operator"
                       [(match_operand:CCANY  1 "metag_<mode>_reg" "")
                        (unspec [(const_int 0)] UNSPEC_RET_COND_INVERTED)])
                       (pc)
                       (return)))]
  ""
  {
    if (!TARGET_METAC_2_1)
      return "MOV%Z0\\tPC, D1RtP";
    else
      return metag_gen_cond_return_branch ("B%%Z0\\t$LX%d %%@\\t(* cond return stub)");
  }
  [(set_attr "type" "unknown")
   (set_attr "cond" "yes")])

(define_insn_and_split "return"
  [(return)]
  "METAG_USE_RETURN_INSN (false)"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    metag_expand_epilogue (false);
    emit_jump_insn (gen_return_internal ());
    DONE;
  }
  [(set_attr "type" "unknown")])

(define_insn_and_split "*cond_<mode>_return"
  [(set (pc)
        (if_then_else (match_operator         0 "comparison_operator"
                       [(match_operand:CCANY  1 "metag_<mode>_reg" "")
                        (const_int 0)])
                      (return)
                      (pc)))]
  "METAG_USE_RETURN_INSN (true)"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    metag_expand_epilogue (false);
    emit_jump_insn (gen_return_internal_cond_<mode> (operands[0], operands[1]));
    DONE;
 }
  [(set_attr "type"    "unknown")
   (set_attr "ccstate" "xcc")])

(define_insn_and_split "*cond_<mode>_return_inverted"
  [(set (pc)
        (if_then_else (match_operator         0 "comparison_operator"
                       [(match_operand:CCANY  1 "metag_<mode>_reg" "")
                        (const_int 0)])
                      (pc)
                      (return)))]
  "METAG_USE_RETURN_INSN (true)"
  "#"
  "&& TRUE"
  [(const_int 0)]
  {
    metag_expand_epilogue (false);
    emit_jump_insn (gen_return_internal_cond_inverted_<mode> (operands[0], operands[1]));
    DONE;
  }
  [(set_attr "type"    "unknown")
   (set_attr "ccstate" "xcc")])

;; No-op instruction

(define_insn "nop"
  [(const_int 0)]
  ""
  "NOP\\t\\t! (*nop OK)"
  [(set_attr "type" "nop")])

(define_expand "casesi"
  [(match_operand:SI 0 "metag_reg_nofloat_op" "")  ; index to jump on
   (match_operand:SI 1 "const_int_operand"    "")  ; lower bound
   (match_operand:SI 2 "const_int_operand"    "")  ; total range
   (match_operand:SI 3 ""                     "")  ; table label
   (match_operand:SI 4 ""                     "")] ; Out of range label
  ""
  {
    rtx op5 = gen_reg_rtx (SImode);
    rtx op6 = gen_reg_rtx (SImode);

    emit_insn (gen_addsi3 (op5, operands[0], gen_int_mode (-INTVAL (operands[1]), SImode)));
    emit_insn (gen_cmpsi (op5, operands[2]));
    emit_jump_insn (gen_bgtu (operands[4]));

    /* This code is intricate...
       MiniM code can behave in three ways with respect to jump tables:
       1) Automatic analysis and branch instruction sizing (default)
       2) Forced short branch instructions
       3) Forced long branch instructions

       META code looks like MiniM short branches have been used but they are in fact
       long branches.

       The first part of the following if block deals with all META cases and all MiniM
       cases, unless long branches have been forced on.

       The second part of the if block deals with MTX 0.1 and MTX 1.2 without MiniM
       enabled (this is 'classic MiniM') and also MiniM cases where long branches have 
       been forced.
     */

    if ((TARGET_METAC_1_0 || TARGET_METAC_1_1)       /* A Meta 1.0, 1.1, 1.2 or 2.1 */
        && !TARGET_METAC_0_1                         /* Not an MTX 0.1 */
        /* MiniM code but with short or automatic branches */
        && (!TARGET_MINIM || metag_jump_table_branch != METAG_MINIM_JUMP_TABLE_BRANCH_LONG)
        && (!TARGET_MTX || TARGET_MINIM))            /* Either not an MTX 1.2
                                                        or is an MTX 1.2 with MiniM */
    {
      int offset = 4;

      /* The instruction that is 'jumped over' ADD PC, CPCx, <reg|int> is
         always long encoded (see casesi_jmp) and can't be short encoded
         so the initial jump is 8 rather than 4 in MiniM mode */

      if (TARGET_MINIM)
        offset = 8;

      /* For automatic jump table analysis use the special ashlsi insn */
      if (TARGET_MINIM && metag_jump_table_branch == METAG_MINIM_JUMP_TABLE_BRANCH_AUTO)
        emit_insn (gen_jump_table_check_ashlsi3 (op6, op5, GEN_INT (2)));
      else
        emit_move_insn (op6,
                        gen_rtx_ASHIFT (SImode, op5, GEN_INT (2)));

      emit_insn (gen_addsi3 (op6, op6, gen_int_mode (offset, SImode)));
    }
    /* An MTX 0.1 or MiniM code with long branches or an MTX 1.2 (without MiniM) */
    else if (TARGET_METAC_0_1 || TARGET_MINIM || TARGET_MTX)
    {
      emit_move_insn (op6,
                      gen_rtx_ASHIFT (SImode, op5, GEN_INT (3)));
      emit_insn (gen_addsi3 (op6, op6, GEN_INT (8)));
    }
    else
      gcc_unreachable ();

    emit_jump_insn (gen_casesi_jmp (op6, operands[3]));

    DONE;
  }
)

(define_insn "jump_table_check_ashlsi3"
  [(set (match_operand:SI                          0 "metag_register_op"    "=e,e,f,f,r,r,r,r")
        (ashift:SI   (match_operand:SI             1 "metag_reg_nofloat_op"  "e,e,f,f,e,e,f,f")
                     (unspec:SI [(match_operand:SI 2 "metag_regorint_op"     "e,L,f,L,e,L,f,L")] UNSPEC_MINIM_JUMP_TABLE)))]
  "TARGET_MINIM"
  {
    /* Detect if short branches are permitted in this function */
    /* WORK NEEDED: This only needs to take place once per function just
                    before emitting instructions */
    metag_can_use_short_branch ();

    operands[2] = GEN_INT (cfun->machine->can_use_short_branch ? 2 : 3);

    return "LSL%?\\t%0, %1, %2\\t%@ (*ashl si rrx OK)";
  }
  [(set_attr "type" "fast,fast,fast,fast,slow,slow,slow,slow")
   (set_attr "cond" "yes,yes,yes,yes,no,no,no,no")])

;; The USE in this pattern is needed to tell flow analysis that this is
;; a CASESI insn.  It has no other purpose.
(define_insn "casesi_jmp"
 [(parallel
    [(set (pc) 
          (plus:SI (pc)
                   (match_operand:SI 0 "metag_addrreg_op" "h,l")))
     (use (label_ref (match_operand  1 ""                 "")))])]
  ""
  {
    /* These instructions are guaranteed to be long encoded as there are
       no possible short encodings. However for clarity they are forced
       long */
    static const char* fmt;

    if (which_alternative == 0)
      fmt = "XL\\tADD\\tPC, CPC0, %0\\t%@ ... OK)";
    else
      fmt = "XL\\tADD\\tPC, CPC1, %0\\t%@ ... OK)";

    return &fmt[TARGET_MINIM ? 0 : 3];
  }
  [(set_attr "type"    "branch")])

;; jump instructions
(define_insn "jump"
  [(set (pc)
        (label_ref (match_operand 0 "" "")))]
  ""
  {
    if (metag_consume_branch (insn))
      return "";

    return "B%?\\t%c0\\t\\t\\t%@ (*b ... OK)";
  }
  [(set_attr "type" "branch")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_expand "indirect_jump"
  [(set (pc)
        (match_operand:SI 0 "address_operand" "p"))]
  ""
  {
    if (!REG_P (operands[0]))
      {
        /* Can only jump to register, see reg_jump below */
        rtx reg = gen_reg_rtx (SImode);

        emit_move_insn (reg, operands[0]);
        operands[0] = reg;
      }
  }
)

(define_insn "*reg_jump"
  [(set (pc)
        (match_operand:SI 0 "metag_register_op" "r"))]
  ""
  "MOV%?\\tPC, %0\\t\\t%@ (*j r OK)"
  [(set_attr "type" "branch")
   (set_attr "cond" "yes")
   (set_attr "predicable" "yes")])

(define_insn "load_pic"
  [(set (match_operand:SI             0 "register_operand" "=X")
        (unspec:SI [(match_operand:SI 1 "register_operand"  "X")] UNSPEC_PIC_BASE))]
  ""
  "ADDT\\t%0, %1, #HI(__GLOBAL_OFFSET_TABLE__)\\t\\n\\tADD\\t%0, %0, #LO(__GLOBAL_OFFSET_TABLE__ + 4)"
  [(set_attr "type"     "two")
   (set_attr "ccstate"  "ncc")
   (set_attr "rename"   "no")])

(include "vector.md")
(include "builtins.md")

(include "peephole2.md")
(include "dsppeephole2.md")

(include "peephole.md")
(include "dsppeephole.md")

(include "combines.md")
(include "fp.md")

(include "tls.md")

;; The 6bit frame elimination insns below intentionally have 12bit predicates on
;; their operands in the hope that the sum 'reduces' this value to fit a 6bit
;; value

;; stores
(define_insn_and_split "*store_<mode>_via_frame_elimination_6bit"
  [(set (mem:MODES (plus:SI (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op"  "da")
                                     (match_operand:SI 1 "metag_offset12_<mode>" "<Z>"))
                            (match_operand:SI          2 "metag_offset12_<mode>" "<Z>")))
        (match_operand:<MODE>                          3 "metag_register_op"     "r"))]
  "(reload_in_progress || reload_completed)
   && metag_offset6_<mode> (GEN_INT (INTVAL (operands[1]) + INTVAL (operands[2])), SImode)"
  "#"
  "reload_completed"
  [(set (mem:<MODE> (plus:SI (match_dup 0)
                             (match_dup 4)))
        (match_dup 3))]
  {
    operands[4] = GEN_INT (INTVAL (operands[1]) + INTVAL (operands[2]));
  }
)

(define_insn_and_split "*store_<mode>_via_frame_elimination_12bit"
  [(set (mem:MODES (plus:SI (plus:SI (match_operand:SI 0 "metag_reg12bit_op"     "Yr")
                                     (match_operand:SI 1 "metag_offset12_<mode>" "<Z>"))
                            (match_operand:SI          2 "metag_offset12_<mode>" "<Z>")))
        (match_operand:<MODE>                          3 "metag_register_op"     "r"))]
  "(reload_in_progress || reload_completed)
   && metag_offset12_<mode> (GEN_INT (INTVAL (operands[1]) + INTVAL (operands[2])), SImode)"
  "#"
  "reload_completed"
  [(set (mem:<MODE> (plus:SI (match_dup 0)
                             (match_dup 4)))
        (match_dup 3))]
  {
    operands[4] = GEN_INT (INTVAL (operands[1]) + INTVAL (operands[2]));
  }
)

;; loads

(define_insn_and_split "*load_<mode>_frame_elimination_6bit"
  [(set (match_operand:<MODE>                          0 "metag_register_op"     "=r")
        (mem:MODES (plus:SI (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"   "da")
                                     (match_operand:SI 2 "metag_offset12_<mode>"  "<Z>"))
                            (match_operand:SI          3 "metag_offset12_<mode>"  "<Z>"))))]
  "(reload_in_progress || reload_completed)
   && metag_offset6_<mode> (GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3])), SImode)"
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (mem:<MODE> (plus:SI (match_dup 1)
                             (match_dup 4))))]
  {
    operands[4] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]));
  }
)

(define_insn_and_split "*load_<mode>_frame_elimination_12bit"
  [(set (match_operand:<MODE>                          0 "metag_register_op"     "=r")
        (mem:MODES (plus:SI (plus:SI (match_operand:SI 1 "metag_reg12bit_op"      "Yr")
                                     (match_operand:SI 2 "metag_offset12_<mode>"  "<Z>"))
                            (match_operand:SI          3 "metag_offset12_<mode>"  "<Z>"))))]
  "(reload_in_progress || reload_completed)
   && metag_offset12_<mode> (GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3])), SImode)"
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (mem:<MODE> (plus:SI (match_dup 1)
                             (match_dup 4))))]
  {
    operands[4] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]));
  }
)

;; load zero extend to SI
(define_insn_and_split "*loadz_<mode>si_frame_elimination_6bit"
  [(set (match_operand:SI                                 0 "metag_register_op"    "=r")
        (zero_extend:SI
           (mem:EXTSI (plus:SI (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                                        (match_operand:SI 2 "metag_offset12_<mode>" "<Z>"))
                               (match_operand:SI          3 "metag_offset12_<mode>" "<Z>")))))]
  "(reload_in_progress || reload_completed)
   && metag_offset6_<mode> (GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3])), SImode)"
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (zero_extend:SI
           (mem:<MODE> (plus:SI (match_dup 1)
                                (match_dup 4)))))]
  {
    operands[4] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]));
  }
)

(define_insn_and_split "*loadz_<mode>si_frame_elimination_12bit"
  [(set (match_operand:SI                                 0 "metag_register_op"    "=r")
        (zero_extend:SI
           (mem:EXTSI (plus:SI (plus:SI (match_operand:SI 1 "metag_reg12bit_op"     "Yr")
                                        (match_operand:SI 2 "metag_offset12_<mode>" "<Z>"))
                               (match_operand:SI          3 "metag_offset12_<mode>" "<Z>")))))]
  "(reload_in_progress || reload_completed)
   && metag_offset12_<mode> (GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3])), SImode)"
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (zero_extend:SI
           (mem:<MODE> (plus:SI (match_dup 1)
                                (match_dup 4)))))]
  {
    operands[4] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]));
  }
)

;; load zero extend to HI
(define_insn_and_split "*loadz_<mode>hi_frame_elimination_6bit"
  [(set (match_operand:HI                                 0 "metag_register_op"    "=r")
        (zero_extend:HI
           (mem:EXTHI (plus:SI (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                                        (match_operand:SI 2 "metag_offset12_<mode>" "<Z>"))
                               (match_operand:SI          3 "metag_offset12_<mode>" "<Z>")))))]
  "(reload_in_progress || reload_completed)
   && metag_offset6_<mode> (GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3])), SImode)"
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (zero_extend:HI
           (mem:<MODE> (plus:SI (match_dup 1)
                                (match_dup 4)))))]
  {
    operands[4] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]));
  }
)

(define_insn_and_split "*loadz_<mode>hi_frame_elimination_12bit"
  [(set (match_operand:HI                                 0 "metag_register_op"    "=r")
        (zero_extend:HI
           (mem:EXTHI (plus:SI (plus:SI (match_operand:SI 1 "metag_reg12bit_op"     "Yr")
                                        (match_operand:SI 2 "metag_offset12_<mode>" "<Z>"))
                               (match_operand:SI          3 "metag_offset12_<mode>" "<Z>")))))]
  "(reload_in_progress || reload_completed)
   && metag_offset12_<mode> (GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3])), SImode)"
  "#"
  "reload_completed"
  [(set (match_dup 0)
        (zero_extend:HI
           (mem:<MODE> (plus:SI (match_dup 1)
                                (match_dup 4)))))]
  {
    operands[4] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]));
  }
)

(define_insn "*sto_<mode>_reload"
  [(set (mem:MODES (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "e,f,h,l")
                            (match_operand:SI 1 "const_int_operand"    "n,n,n,n")))
        (match_operand:<MODE>                 2 "metag_register_op"    "t,u,y,z"))]
  "!TARGET_METAC_1_1 && reload_in_progress && REGNO (operands[0]) == FRAME_POINTER_REGNUM"
  "#"
  [(set_attr "type" "fast")])

(define_insn "*sto_<mode>_1_1_reload"
  [(set (mem:MODES (plus:SI (match_operand:SI 0 "metag_reg_nofloat_op" "da")
                            (match_operand:SI 1 "const_int_operand"    "n")))
        (match_operand:<MODE>                 2 "metag_register_op"    "r"))]
  "TARGET_METAC_1_1 && reload_in_progress && REGNO (operands[0]) == FRAME_POINTER_REGNUM"
  "#"
  [(set_attr "type" "fast")])

(define_insn "*load_<mode>_reload"
  [(set (match_operand:<MODE>                 0 "metag_register_op"    "=r")
        (mem:MODES (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                            (match_operand:SI 2 "const_int_operand"     "n"))))]
  "reload_in_progress && REGNO (operands[1]) == FRAME_POINTER_REGNUM"
  "#"
  [(set_attr "type" "load")])

(define_insn "*loadz_<mode>si_reload"
  [(set (match_operand:SI                        0 "metag_register_op"    "=r")
        (zero_extend:SI 
           (mem:EXTSI (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                               (match_operand:SI 2 "const_int_operand"     "n")))))]
  "reload_in_progress && REGNO (operands[1]) == FRAME_POINTER_REGNUM"
  "#"
  [(set_attr "type" "load")])

(define_insn "*loadz_<mode>hi_reload"
  [(set (match_operand:HI                        0 "metag_register_op"    "=r")
        (zero_extend:HI 
           (mem:EXTHI (plus:SI (match_operand:SI 1 "metag_reg_nofloat_op"  "da")
                               (match_operand:SI 2 "const_int_operand"     "n")))))]
  "reload_in_progress && REGNO (operands[1]) == FRAME_POINTER_REGNUM"
  "#"
  [(set_attr "type" "load")])

(define_cond_exec
  [(match_operator    0 "comparison_operator"
   [(match_operand:CC 1 "metag_cc_reg"        "")
    (const_int 0)])]
  "!metag_cond_exec_p ()"
  "")

;; end of file
