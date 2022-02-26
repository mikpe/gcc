;;--------------;;
;; Mode Macros  ;;
;;--------------;;

(define_mode_iterator	IVEC64 [V8QI V4HI V2SI])
(define_mode_iterator	SIVEC64 [V4HI V2SI])         ;; signed byte support lacking

(define_mode_iterator	IVEC32 [V4QI V2HI])
(define_mode_iterator	SIVEC32 [V2HI])              ;; signed byte support lacking

(define_mode_iterator	IVEC128 [V16QI V8HI V4SI])   ;; double-size for valign and such

(define_mode_attr velsize [(V8QI "b") (V4QI "b") (V4HI "h") (V2HI "h") (V2SI "w")])
;;(define_mode_attr velsize [(V8QI "b") (V4HI "h") (V2SI "w")])


;;--------------;;
;; Vector Moves ;;
;;--------------;;

;;;---------;;
;;; movv8bi ;;
;;;---------;;
;
;(define_expand "movv8bi"
;  [(set (match_operand:V8BI 0 "nonimmediate_operand" "")
;        (match_operand:V8BI 1 "general_operand" ""))]
;  ""
;  {
;    if(GET_CODE (operands[0]) != REG){
;      operands[1] = force_reg(V8BImode, operands[1]);
;    }
;  }
;)
;
;(define_insn "movv8bi_czero"
;  [(set (match_operand:V8BI 0 "gr_register_operand" "=Rp, Rg")
;        (match_operand:V8BI 1 "zero_constant"       "Iu0,Iu0"))]
;  ""
;  "@
;   %0 = cmp.gt(r0,r0)  // movv8bi_czero
;   %0 = #0             // movv8bi_czero"
;  [(set_attr "type" "A,A")]
;)
;
;(define_insn "movv8bi_cvec"
;  [(set (match_operand:V8BI 0 "gr_register_operand" "=Rg")
;        (match_operand:V8BI 1 "immediate_operand"  "Is16"))]
;  ""
;  "%0 = #%1        // movv8bi_cvec"
;  [(set_attr "type" "A")]
;)
;
;(define_insn "movv8bi_real"
;  [(set (match_operand:V8BI 0 "nonimmediate_operand" "=Rg,Rg, m,Rp,Rp,Rg")
;        (match_operand:V8BI 1 "general_operand"       "Rg, m,Rg,Rp,Rg,Rp"))]
;  ""
;  "@
;   %0 = %1
;   %0 = memb(%1)
;   memb(%0) = %1
;   %0 = or(%1,%1)
;   %0 = %1
;   %0 = %1"
;  [(set_attr "type" "A,Load,Store,S,S,S")]
;)

(define_expand "mov<mode>"
  [(set (match_operand:IVEC64 0 "nonimmediate_operand" "")
        (match_operand:IVEC64 1 "general_operand" ""))]
  ""
 {
  if (GET_CODE(operands[0]) != REG) {
    operands[1] = force_reg(<MODE>mode, operands[1]);
  } else if (CONSTANT_P(operands[1]) && !zero_constant(operands[1],<MODE>mode)) {
    operands[1] = validize_mem(force_const_mem(<MODE>mode,operands[1]));
  }
 }
)

(define_insn "mov<mode>_zero64"
  [(set (match_operand:IVEC64 0 "nonimmediate_operand" "=Rg")
        (match_operand:IVEC64 1 "zero_constant"        "i"))]
  ""
  "%P0 = xor(r1:0,r1:0) // CLEAR VECTOR"
  [(set_attr "type" "X")]
)

(define_insn "mov<mode>_real_64"
  [(set (match_operand:IVEC64 0 "nonimmediate_operand" "=Rg,Rg, m")
        (match_operand:IVEC64 1 "nonimmediate_operand"  "Rg, m,Rg"))]
  "!(memory_operand(operands[0], <MODE>mode)
     && immediate_operand(operands[1], <MODE>mode))"
  {
    switch(which_alternative){
      case 0:
        if(TARGET_V2_FEATURES){
          return "%P0 = %P1";
        }
        else {
          return "%P0 = combine(%H1,%L1) // %P0 = %P1";
        }
      case 1:
        return "%P0 = memd(%1)";
      case 2:
        return "memd(%0) = %P1";
      default:
        gcc_unreachable();
    }
  }
  [(set_attr "type" "A,Load,Store")]
)

(define_expand "mov<mode>"
  [(set (match_operand:IVEC32 0 "nonimmediate_operand" "")
        (match_operand:IVEC32 1 "general_operand" ""))]
  ""
{
  if (GET_CODE(operands[0]) != REG) {
    operands[1] = force_reg(<MODE>mode, operands[1]);
  } else if (CONSTANT_P(operands[1]) && !zero_constant(operands[1],<MODE>mode)) {
    operands[1] = validize_mem(force_const_mem(<MODE>mode,operands[1]));
  }
}
)

(define_insn "mov<mode>_zero32"
  [(set (match_operand:IVEC32 0 "nonimmediate_operand" "=Rg")
        (match_operand:IVEC32 1 "zero_constant"        "i"))]
  ""
  "%0 = #0"
  [(set_attr "type" "A")]
)

(define_insn "mov<mode>_real_32"
  [(set (match_operand:IVEC32 0 "nonimmediate_operand" "=Rg,Rg, m")
       (match_operand:IVEC32 1 "nonimmediate_operand"  "Rg, m,Rg"))]
  "!(memory_operand(operands[0], <MODE>mode)
     && immediate_operand(operands[1], <MODE>mode))"
  "@
   %0 = %1
   %0 = memw(%1)
   memw(%0) = %1"
  [(set_attr "type" "A,Load,Store")]
)


;;(define_insn "vec_realign_load_helper_predmove"
;;  [(set (match_operand:V8BI 0 "pr_register_operand" "=Rp")
;;        (unspec:V8BI [(match_operand:SI 1 "gr_register_operand" "Rg")] UNSPEC_REALIGN_SETUP))]
;;  ""
;;  "%0 = %1"
;;  [(set_attr "type" "S")]
;;)

;;(define_expand "vec_realign_load_<mode>"
;;  [(set (match_operand:IVEC64 0 "gr_register_operand" "")
;;        (unspec:IVEC64 [(match_operand 1 "gr_register_operand" "")
;;                        (match_operand 2 "gr_register_operand" "")
;;                        (match_operand 3 "nonmemory_operand" "")] UNSPEC_REALIGN_LOAD))]
;;  ""
;;  {
;;     rtx reg = gen_reg_rtx(V8BImode);
;;     fprintf(stderr,"vec_realign_load_<mode> expander.  Args:");
;;     debug_rtx(operands[0]);
;;     debug_rtx(operands[1]);
;;     debug_rtx(operands[2]);
;;     debug_rtx(operands[3]);
;;     operands[0] = force_reg(<MODE>mode,operands[0]);
;;     operands[1] = force_reg(<MODE>mode,operands[1]);
;;     operands[2] = force_reg(<MODE>mode,operands[2]);
;;     emit_insn(gen_rtx_SET(V8BImode,reg,gen_rtx_UNSPEC(V8BImode,gen_rtvec(1,operands[3]),UNSPEC_REALIGN_SETUP)));
;;     operands[3] = reg;
;;  }
;;)

;;(define_expand "vec_realign_load_<mode>"
;;  [(set (match_operand:IVEC64 0 "gr_register_operand" "")
;;        (unspec:IVEC64 [(match_operand:IVEC64 1 "gr_register_operand" "")
;;                        (match_operand:IVEC64 2 "gr_register_operand" "")
;;                        (match_operand:QI 3 "pr_register_operand" "")] UNSPEC_REALIGN_LOAD))]
;;  ""
;;  {
;;  }
;;)

;;(define_insn "vec_realign_load_<mode>_real"
;;  [(set (match_operand:IVEC64 0 "register_operand" "=Rg,Rg")
;;        (unspec:IVEC64 [(match_operand:IVEC64 1 "gr_register_operand" "Rg,Rg")
;;                        (match_operand:IVEC64 2 "gr_register_operand" "Rg,Rg")
;;                        (match_operand:V8BI 3 "pr_register_operand" "Rp,Iu3")] UNSPEC_REALIGN_LOAD))]
;;  ""
;;  "@
;;   %P0 = valignb(%P2,%P1,%3) // %0 %1 %2 %3 Realign <mode>
;;   %P0 = valignb(%P2,%P1,#%3) // %0 %1 %2 %3 Realign <mode>"
;;   [(set_attr "type" "S")]
;;)

(define_insn "vec_realign_load_<mode>"
  [(set (match_operand:IVEC64 0 "register_operand" "=Rg,Rg")
        (unspec:IVEC64 [(match_operand:IVEC64 1 "gr_register_operand" "Rg,Rg")
                        (match_operand:IVEC64 2 "gr_register_operand" "Rg,Rg")
                        (match_operand:QI 3 "pr_register_operand" "Rp,Iu3")] UNSPEC_REALIGN_LOAD))]
  ""
  "@
   %P0 = valignb(%P2,%P1,%3) // %0 %1 %2 %3 Realign <mode>
   %P0 = valignb(%P2,%P1,#%3) // %0 %1 %2 %3 Realign <mode>"
   [(set_attr "type" "S")]
)


(define_insn "splatv8qi"
  [(set (match_operand:V8QI 0 "gr_register_operand" "=Rg")
        (vec_duplicate:V8QI (match_operand:QI 1 "gr_register_operand" "Rg")))]
  ""
  "%P0 = vsplatb(%1)"
  [(set_attr "type" "S")]
)

(define_insn "splatv4hi"
  [(set (match_operand:V4HI 0 "gr_register_operand" "=Rg")
        (vec_duplicate:V4HI (match_operand:HI 1 "gr_register_operand" "Rg")))]
  ""
  "%P0 = vsplath(%1)"
  [(set_attr "type" "S")]
)

(define_insn "splatv2si"
  [(set (match_operand:V2SI 0 "gr_register_operand" "=Rg")
        (vec_duplicate:V2SI (match_operand:SI 1 "gr_register_operand" "Rg")))]
  ""
  "%P0 = combine(%1,%1) // splatv2si "
  [(set_attr "type" "A")]
)

(define_expand "vcond<mode>"
  [(set (match_operand:IVEC64 0 "gr_register_operand" "=Rg")
        (if_then_else:IVEC64 (match_operator 3 ""
                               [(match_operand:IVEC64 4 "gr_register_operand" "Rg")
                                (match_operand:IVEC64 5 "gr_register_operand" "Rg")])
        (match_operand:IVEC64 1 "gr_register_operand" "Rg")
        (match_operand:IVEC64 2 "gr_register_operand" "Rg")))]
   ""
   {
      fprintf(stderr,"vcond<mode> attempt! FIX THIS PATTERN\n");
      FAIL;
   })


(define_expand "vcondu<mode>"
  [(set (match_operand:IVEC64 0 "gr_register_operand" "=Rg")
        (if_then_else:IVEC64 (match_operator 3 ""
                               [(match_operand:IVEC64 4 "gr_register_operand" "Rg")
                                (match_operand:IVEC64 5 "gr_register_operand" "Rg")])
        (match_operand:IVEC64 1 "gr_register_operand" "Rg")
        (match_operand:IVEC64 2 "gr_register_operand" "Rg")))]
   ""
   {
      fprintf(stderr,"vcondu<mode> attempt! FIX THIS PATTERN\n");
      FAIL;
   })


;;----------------;;
;; movmisalign{m} ;;
;;----------------;;

;;(define_expand "movmisalign<mode>"
;;  [(set (match_operand:IVEC64 0 "nonimmediate_operand" "")
;;        (match_operand:IVEC64 1 "nonimmediate_operand" ""))]
;;  ""
;;  if (<MODE>mode == VQ8Imode)


;;---------;;
;; add{m}3 ;;
;;---------;;

(define_insn "add<mode>3"
  [(set (match_operand:IVEC64 0 "gr_register_operand"             "=Rg")
        (plus:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")
                     (match_operand:IVEC64 2 "gr_register_operand" "Rg")))]
  ""
  "%P0 = vadd<velsize>(%P1,%P2)"
  [(set_attr "type" "X")]
)

(define_insn "add<mode>3"
  [(set (match_operand:IVEC32 0 "gr_register_operand"             "=Rg")
        (plus:IVEC32 (match_operand:IVEC32 1 "gr_register_operand" "Rg")
                     (match_operand:IVEC32 2 "gr_register_operand" "Rg")))]
  ""
  "%0 = vadd<velsize>(%1,%2)"
  [(set_attr "type" "A")]
)

(define_insn "ssadd<mode>3"
  [(set (match_operand:SIVEC64 0 "gr_register_operand"                 "=Rg")
        (ss_plus:SIVEC64 (match_operand:SIVEC64 1 "gr_register_operand" "Rg")
                     (match_operand:SIVEC64 2 "gr_register_operand"     "Rg")))]
  ""
  "%P0 = vadd<velsize>(%P1,%P2):sat"
  [(set_attr "type" "X")]
)

(define_insn "usadd<mode>3"
  [(set (match_operand:IVEC64 0 "gr_register_operand"                "=Rg")
        (us_plus:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")
                     (match_operand:IVEC64 2 "gr_register_operand"    "Rg")))]
  "(<MODE>mode != SImode)"
  "%P0 = vaddu<velsize>(%P1,%P2):sat"
  [(set_attr "type" "X")]
)


;;---------;;
;; sub{m}3 ;;
;;---------;;

(define_insn "sub<mode>3"
  [(set (match_operand:IVEC64 0 "gr_register_operand"              "=Rg")
        (minus:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")
                      (match_operand:IVEC64 2 "gr_register_operand" "Rg")))]
  ""
  "%P0 = vsub<velsize>(%P1,%P2)"
  [(set_attr "type" "X")]
)

(define_insn "sub<mode>3"
  [(set (match_operand:IVEC32 0 "gr_register_operand"              "=Rg")
        (minus:IVEC32 (match_operand:IVEC32 1 "gr_register_operand" "Rg")
                      (match_operand:IVEC32 2 "gr_register_operand" "Rg")))]
  ""
  "%0 = vsub<velsize>(%1,%2)"
  [(set_attr "type" "A")]
)

(define_insn "sssub<mode>3"
  [(set (match_operand:SIVEC64 0 "gr_register_operand"                  "=Rg")
        (ss_minus:SIVEC64 (match_operand:SIVEC64 1 "gr_register_operand" "Rg")
                     (match_operand:SIVEC64 2 "gr_register_operand"      "Rg")))]
  ""
  "%P0 = vsub<velsize>(%P1,%P2):sat"
  [(set_attr "type" "X")]
)

(define_insn "ussub<mode>3"
  [(set (match_operand:IVEC64 0 "gr_register_operand"                 "=Rg")
        (us_minus:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")
                     (match_operand:IVEC64 2 "gr_register_operand"     "Rg")))]
  "(<MODE>mode != SImode)"
  "%P0 = vsubu<velsize>(%P1,%P2):sat"
  [(set_attr "type" "X")]
)


;;----------;;
;; umin{m}3 ;;
;;----------;;

(define_insn "umin<mode>3"
  [(set (match_operand:IVEC64 0 "gr_register_operand"             "=Rg")
        (umin:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")
                     (match_operand:IVEC64 2 "gr_register_operand" "Rg")))]
  ""
  "%P0 = vminu<velsize>(%P1,%P2)"
  [(set_attr "type" "X")]
)

(define_insn "umin<mode>3"
  [(set (match_operand:IVEC32 0 "gr_register_operand"             "=Rg")
        (umin:IVEC32 (match_operand:IVEC32 1 "gr_register_operand" "Rg")
                     (match_operand:IVEC32 2 "gr_register_operand" "Rg")))]
  ""
  "%0 = vminu<velsize>(%1,%2)"
  [(set_attr "type" "X")]
)


;;----------;;
;; umax{m}3 ;;
;;----------;;

(define_insn "umax<mode>3"
  [(set (match_operand:IVEC64 0 "gr_register_operand"             "=Rg")
        (umax:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")
                     (match_operand:IVEC64 2 "gr_register_operand" "Rg")))]
  ""
  "%P0 = vmaxu<velsize>(%P1,%P2)"
  [(set_attr "type" "X")]
)

(define_insn "umax<mode>3"
  [(set (match_operand:IVEC32 0 "gr_register_operand"             "=Rg")
        (umax:IVEC32 (match_operand:IVEC32 1 "gr_register_operand" "Rg")
                     (match_operand:IVEC32 2 "gr_register_operand" "Rg")))]
  ""
  "%0 = vmaxu<velsize>(%1,%2)"
  [(set_attr "type" "X")]
)


;;---------;;
;; and{m}3 ;;
;;---------;;

(define_insn "and<mode>3"
  [(set (match_operand:IVEC64 0 "gr_register_operand"            "=Rg")
        (and:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")
                    (match_operand:IVEC64 2 "gr_register_operand" "Rg")))]
  ""
  "%P0 = and(%P1,%P2) // <MODE>"
  [(set_attr "type" "X")]
)

(define_insn "and<mode>3"
  [(set (match_operand:IVEC32 0 "gr_register_operand"            "=Rg")
        (and:IVEC32 (match_operand:IVEC32 1 "gr_register_operand" "Rg")
                    (match_operand:IVEC32 2 "gr_register_operand" "Rg")))]
  ""
  "%0 = and(%1,%2) // <MODE>"
  [(set_attr "type" "X")]
)


;;---------;;
;; ior{m}3 ;;
;;---------;;

(define_insn "ior<mode>3"
  [(set (match_operand:IVEC64 0 "gr_register_operand"            "=Rg")
        (ior:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")
                    (match_operand:IVEC64 2 "gr_register_operand" "Rg")))]
  ""
  "%P0 = or(%P1,%P2) // <MODE>"
  [(set_attr "type" "X")]
)

(define_insn "ior<mode>3"
  [(set (match_operand:IVEC32 0 "gr_register_operand"            "=Rg")
        (ior:IVEC32 (match_operand:IVEC32 1 "gr_register_operand" "Rg")
                    (match_operand:IVEC32 2 "gr_register_operand" "Rg")))]
  ""
  "%0 = or(%1,%2) // <MODE>"
  [(set_attr "type" "X")]
)


;;---------;;
;; xor{m}3 ;;
;;---------;;

(define_insn "xor<mode>3"
  [(set (match_operand:IVEC64 0 "gr_register_operand"            "=Rg")
        (xor:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")
                    (match_operand:IVEC64 2 "gr_register_operand" "Rg")))]
  ""
  "%P0 = or(%P1,%P2) // <MODE>"
  [(set_attr "type" "X")]
)

(define_insn "xor<mode>3"
  [(set (match_operand:IVEC32 0 "gr_register_operand"            "=Rg")
        (xor:IVEC32 (match_operand:IVEC32 1 "gr_register_operand" "Rg")
                    (match_operand:IVEC32 2 "gr_register_operand" "Rg")))]
  ""
  "%0 = xor(%1,%2) // <MODE>"
  [(set_attr "type" "X")]
)


;;----------;;
;; smin{m}3 ;;
;;----------;;

(define_insn "smin<mode>3"
  [(set (match_operand:SIVEC64 0 "gr_register_operand"              "=Rg")
        (smin:SIVEC64 (match_operand:SIVEC64 1 "gr_register_operand" "Rg")
                      (match_operand:SIVEC64 2 "gr_register_operand" "Rg")))]
  ""
  "%P0 = vmin<velsize>(%P1,%P2)"
  [(set_attr "type" "X")]
)

(define_insn "smin<mode>3"
  [(set (match_operand:SIVEC32 0 "gr_register_operand"              "=Rg")
        (smin:SIVEC32 (match_operand:SIVEC32 1 "gr_register_operand" "Rg")
                      (match_operand:SIVEC32 2 "gr_register_operand" "Rg")))]
  ""
  "%0 = vmin<velsize>(%1,%2)"
  [(set_attr "type" "X")]
)


;;----------;;
;; smax{m}3 ;;
;;----------;;

(define_insn "smax<mode>3"
  [(set (match_operand:SIVEC64 0 "gr_register_operand"              "=Rg")
        (smax:SIVEC64 (match_operand:SIVEC64 1 "gr_register_operand" "Rg")
                      (match_operand:SIVEC64 2 "gr_register_operand" "Rg")))]
  ""
  "%P0 = vmax<velsize>(%P1,%P2)"
  [(set_attr "type" "X")]
)

(define_insn "smax<mode>3"
  [(set (match_operand:SIVEC32 0 "gr_register_operand"              "=Rg")
        (smax:SIVEC32 (match_operand:SIVEC32 1 "gr_register_operand" "Rg")
                      (match_operand:SIVEC32 2 "gr_register_operand" "Rg")))]
  ""
  "%0 = vmax<velsize>(%1,%2)"
  [(set_attr "type" "X")]
)


;;----------------;;
;; reduc_smin_{m} ;;
;;----------------;;

(define_insn "reduc_smin_v2si"
  [(set (match_operand:V2SI 0 "gr_register_operand"             "=Rg")
        (unspec:V2SI [(match_operand:V2SI 1 "gr_register_operand" "0")]
                     UNSPEC_REDUC_SMIN))]
  ""
  "%L0 = min(%L0,%H0)"
  [(set_attr "type" "X")]
)


;;----------------;;
;; reduc_smax_{m} ;;
;;----------------;;

(define_insn "reduc_smax_v2si"
  [(set (match_operand:V2SI 0 "gr_register_operand"             "=Rg")
        (unspec:V2SI [(match_operand:V2SI 1 "gr_register_operand" "0")]
                     UNSPEC_REDUC_SMAX))]
  ""
  "%L0 = max(%L0,%H0)"
  [(set_attr "type" "X")]
)


;;----------------;;
;; reduc_umin_{m} ;;
;;----------------;;

(define_insn "reduc_umin_v2si"
  [(set (match_operand:V2SI 0 "gr_register_operand"              "=Rg")
        (unspec:V2SI [(match_operand:V2SI 1  "gr_register_operand" "0")]
                     UNSPEC_REDUC_UMIN))]
  ""
  "%L0 = minu(%L0,%H0)"
  [(set_attr "type" "X")]
)


;;----------------;;
;; reduc_umax_{m} ;;
;;----------------;;

(define_insn "reduc_umax_v2si"
  [(set (match_operand:V2SI 0 "gr_register_operand"             "=Rg")
        (unspec:V2SI [(match_operand:V2SI 1 "gr_register_operand" "0")]
                     UNSPEC_REDUC_UMAX))]
  ""
  "%L0 = maxu(%L0,%H0)"
  [(set_attr "type" "X")]
)


;;-----------------;;
;; reduc_splus_{m} ;;
;;-----------------;;

(define_insn "reduc_splus_v2si"
  [(set (match_operand:V2SI 0 "gr_register_operand"             "=Rg")
        (unspec:V2SI [(match_operand:V2SI 1 "gr_register_operand" "0")]
                     UNSPEC_REDUC_SPLUS))]
  ""
  "%L0 = add(%L0,%H0)"
  [(set_attr "type" "X")]
)

(define_insn "reduc_splus_real"
  [(set (match_operand:V4HI 0 "gr_register_operand"              "=Rg")
        (unspec:V4HI [(match_operand:V4HI 1 "gr_register_operand" "Rg")]
                     UNSPEC_REDUC_SPLUS))
   (use (match_operand:V4HI 2 "gr_register_operand"               "Rg"))]
  ""
  "%P0 = vrmpyh(%P1,%P2)      //reduc_splus_real"
  [(set_attr "type" "M")]
)

(define_expand "reduc_splus_v4hi"
  [(set (match_operand:V4HI 0 "gr_register_operand" "")
        (unspec:V4HI [(match_operand:V4HI 1 "gr_register_operand" "" )]
                     UNSPEC_REDUC_SPLUS))]
  ""
  {
    rtx one = gen_reg_rtx(HImode);
    rtx vone = gen_reg_rtx(V4HImode);
    emit_insn(gen_movhi(one,GEN_INT(1)));
    emit_insn(gen_splatv4hi(vone,one));
    emit_insn(gen_reduc_splus_real(operands[0],operands[1],vone));
    DONE;
  }
)


;;-----------------;;
;; reduc_uplus_{m} ;;
;;-----------------;;

(define_expand "reduc_uplus_v2si"
  [(set (match_operand:V2SI 0 "gr_register_operand"             "=Rg")
        (unspec:V2SI [(match_operand:V2SI 1 "gr_register_operand" "0")]
                     UNSPEC_REDUC_SPLUS))]
  ""
  {
        /* Uplus is the same as Splus unless we are expanding size,
           which we are not */
  }
)

(define_expand "reduc_uplus_v4hi"
  [(set (match_operand:V4HI 0 "gr_register_operand" "")
        (unspec:V4HI [(match_operand:V4HI 1  "gr_register_operand" "")]
                     UNSPEC_REDUC_SPLUS))]
  ""
  {
    rtx one = gen_reg_rtx(HImode);
    rtx vone = gen_reg_rtx(V4HImode);
    emit_insn(gen_movhi(one,GEN_INT(1)));
    emit_insn(gen_splatv4hi(vone,one));
    emit_insn(gen_reduc_splus_real(operands[0],operands[1],vone));
    DONE;
  }
)


;;-------------;;
;; vec_shl_{m} ;;
;;-------------;;

(define_expand "vec_shl_<mode>"
  [(set (match_operand:IVEC64 0 "gr_register_operand" "")
        (unspec:IVEC64 [(match_operand:IVEC64 1 "gr_register_operand" "")
                     (match_operand:SI 2 "nonmemory_operand" "")] UNSPEC_VEC_SHL))]
  ""
  {
  }
)

(define_expand "vec_shl_<mode>"
  [(set (match_operand:IVEC32 0 "gr_register_operand" "")
        (unspec:IVEC32 [(match_operand:IVEC32 1 "gr_register_operand" "")
                     (match_operand:SI 2 "nonmemory_operand" "")] UNSPEC_VEC_SHL))]
  ""
  {
  }
)

(define_insn "real_vec_shl_<mode>"
  [(set (match_operand:IVEC64 0 "gr_register_operand"                "=Rg, Rg")
        (unspec:IVEC64 [(match_operand:IVEC64 1 "gr_register_operand" "Rg, Rg")
                     (match_operand:SI 2 "nonmemory_operand"          "Rg,Iu6")] UNSPEC_VEC_SHL))]
  ""
  "@
   %P0 = asl(%P1,%2) // whole vector shift
   %P0 = asl(%P1,#%2) // whole vector shift"
  [(set_attr "type" "S")]
)

(define_insn "real_vec_shl_<mode>"
  [(set (match_operand:IVEC32 0 "gr_register_operand"                "=Rg, Rg")
        (unspec:IVEC32 [(match_operand:IVEC32 1 "gr_register_operand" "Rg, Rg")
                     (match_operand:SI 2 "nonmemory_operand"          "Rg,Iu6")] UNSPEC_VEC_SHL))]
  ""
  "@
   %0 = asl(%1,%2) // whole vector shift
   %0 = asl(%1,#%2) // whole vector shift"
  [(set_attr "type" "S")]
)


;;-------------;;
;; vec_shr_{m} ;;
;;-------------;;

(define_expand "vec_shr_<mode>"
  [(set (match_operand:IVEC64 0 "gr_register_operand" "")
        (unspec:IVEC64 [(match_operand:IVEC64 1 "gr_register_operand" "")
                     (match_operand:SI 2 "nonmemory_operand" "")] UNSPEC_VEC_SHR))]
  ""
  {
  }
)

(define_expand "vec_shr_<mode>"
  [(set (match_operand:IVEC32 0 "gr_register_operand" "")
        (unspec:IVEC32 [(match_operand:IVEC32 1 "gr_register_operand" "")
                     (match_operand:SI 2 "nonmemory_operand" "")] UNSPEC_VEC_SHR))]
  ""
  {
  }
)

(define_insn "real_vec_shr_<mode>"
  [(set (match_operand:IVEC64 0 "gr_register_operand"                "=Rg, Rg")
        (unspec:IVEC64 [(match_operand:IVEC64 1 "gr_register_operand" "Rg, Rg")
                     (match_operand:SI 2 "nonmemory_operand"          "Rg,Iu6")] UNSPEC_VEC_SHR))]
  ""
  "@
   %P0 = lsr(%P1,%2) // whole vector shift
   %P0 = lsr(%P1,#%2) // whole vector shift"
  [(set_attr "type" "S")]
)

(define_insn "real_vec_shr_<mode>"
  [(set (match_operand:IVEC32 0 "gr_register_operand"                "=Rg, Rg")
        (unspec:IVEC32 [(match_operand:IVEC32 1 "gr_register_operand" "Rg, Rg")
                     (match_operand:SI 2 "nonmemory_operand"          "Rg,Iu6")] UNSPEC_VEC_SHR))]
  ""
  "@
   %0 = lsr(%1,%2) // whole vector shift
   %0 = lsr(%1,#%2) // whole vector shift"
  [(set_attr "type" "S")]
)


;;----------;;
;; ashl{m}3 ;;
;;----------;;

(define_insn "ashl<mode>3"
  [(set (match_operand:SIVEC64 0 "gr_register_operand"                "=Rg, Rg")
        (ashift:SIVEC64 (match_operand:SIVEC64 1 "gr_register_operand" "Rg, Rg")
                       (match_operand:SI 2 "nonmemory_operand"         "Rg,Iu6")))]
  ""
  "@
   %P0 = vasl<velsize>(%P1,%2)
   %P0 = vasl<velsize>(%P1,#%2)"
  [(set_attr "type" "S,S")]
)


;;----------;;
;; ashr{m}3 ;;
;;----------;;

(define_insn "ashr<mode>3"
  [(set (match_operand:SIVEC64 0 "gr_register_operand"                  "=Rg, Rg")
        (ashiftrt:SIVEC64 (match_operand:SIVEC64 1 "gr_register_operand" "Rg, Rg")
                         (match_operand:SI 2 "nonmemory_operand"         "Rg,Iu6")))]
  ""
  "@
   %P0 = vasr<velsize>(%P1,%2)
   %P0 = vasr<velsize>(%P1,#%2)"
  [(set_attr "type" "S,S")]
)


;;----------;;
;; lshr{m}3 ;;
;;----------;;

(define_insn "lshr<mode>3"
  [(set (match_operand:SIVEC64 0 "gr_register_operand"                  "=Rg, Rg")
        (lshiftrt:SIVEC64 (match_operand:SIVEC64 1 "gr_register_operand" "Rg, Rg")
                         (match_operand:SI 2 "nonmemory_operand"         "Rg,Iu6")))]
  ""
  "@
   %P0 = vlsr<velsize>(%P1,%2)
   %P0 = vlsr<velsize>(%P1,#%2)"
  [(set_attr "type" "S,S")]
)


;;---------;;
;; neg{m}2 ;;
;;---------;;
(define_expand "neg<mode>2"
  [(set (match_operand:IVEC64 0 "gr_register_operand"            "=Rg")
        (neg:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")))]
  ""
  {
  rtx vzero = gen_reg_rtx(<MODE>mode);
  emit_insn(gen_mov<mode>_zero64(vzero,const0_rtx));
  emit_insn(gen_sub<mode>3(operands[0],vzero,operands[1]));
  DONE;
  }
)

;;(define_insn "neg<mode>2"
;;  [(set (match_operand:IVEC32 0 "gr_register_operand"            "=Rg")
;;        (neg:IVEC32 (match_operand:IVEC32 1 "gr_register_operand" "Rg")))]
;;  ""
;;  "%0 = vneg<velsize>(%1)"
;;  [(set_attr "type" "X")]
;;)


;;--------------;;
;; one_cmpl{m}2 ;;
;;--------------;;

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:IVEC64 0 "gr_register_operand"            "=Rg")
        (not:IVEC64 (match_operand:IVEC64 1 "gr_register_operand" "Rg")))]
  ""
  "%P0 = not(%P1) // <MODE>"
  [(set_attr "type" "X")]
)

(define_insn "one_cmpl<mode>2"
  [(set (match_operand:IVEC32 0 "gr_register_operand"            "=Rg")
        (not:IVEC32 (match_operand:IVEC32 1 "gr_register_operand" "Rg")))]
  ""
  "%0 = not(%1) // <MODE>"
  [(set_attr "type" "X")]
)


