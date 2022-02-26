(define_constants [
  (UNSPEC_HEXAGON_SI_to_SXTHI_asrh 900)
  (UNSPEC_HEXAGON_val_for_valignb 901)
  (UNSPEC_HEXAGON_circ_ldd   902)
  (UNSPEC_HEXAGON_circ_ldw   903)
  (UNSPEC_HEXAGON_circ_ldh   904)
  (UNSPEC_HEXAGON_circ_lduh  905)
  (UNSPEC_HEXAGON_circ_ldb   906)
  (UNSPEC_HEXAGON_circ_ldub  907)
  (UNSPEC_HEXAGON_circ_std   908)
  (UNSPEC_HEXAGON_circ_stw   909)
  (UNSPEC_HEXAGON_circ_sth   910)
  (UNSPEC_HEXAGON_circ_sthhi 911)
  (UNSPEC_HEXAGON_circ_stb   912)
  (UNSPEC_HEXAGON_brev_ldd   913)
  (UNSPEC_HEXAGON_brev_ldw   914)
  (UNSPEC_HEXAGON_brev_ldh   915)
  (UNSPEC_HEXAGON_brev_lduh  916)
  (UNSPEC_HEXAGON_brev_ldb   917)
  (UNSPEC_HEXAGON_brev_ldub  918)
  (UNSPEC_HEXAGON_brev_std   919)
  (UNSPEC_HEXAGON_brev_stw   920)
  (UNSPEC_HEXAGON_brev_sth   921)
  (UNSPEC_HEXAGON_brev_sthhi 922)
  (UNSPEC_HEXAGON_brev_stb   923)
])


(define_expand "hexagon_builtin_SI_to_SXTHI_asrh"
  [(set (match_operand:SI 0 "gr_register_operand" "=Rg")
        (sign_extend:SI (unspec:HI [
           (match_operand:SI 1 "gr_register_operand" "Rg")
           ] UNSPEC_HEXAGON_SI_to_SXTHI_asrh))
  )]
  ""
  {
    if(!TARGET_LITERAL_INTRINSICS){
      emit_insn(gen_ashrsi3(operands[0], operands[1], gen_int_mode(16, SImode)));
      DONE;
    }
  }
)

(define_insn "hexagon_unspec_SI_to_SXTHI_asrh"
  [(set (match_operand:SI 0 "gr_register_operand" "=Rg")
        (sign_extend:SI (unspec:HI [
           (match_operand:SI 1 "gr_register_operand" "Rg")
           ] UNSPEC_HEXAGON_SI_to_SXTHI_asrh))
  )]
  ""
  "%0 =asrh(%1)"
  [(set_attr "type" "A")]
)

(define_insn "hexagon_builtin_val_for_valignb"
  [(set (match_operand:QI 0 "pr_register_operand" "=Rp")
        (unspec:QI [ (mem (match_operand:SI 1 "gr_register_operand" "Rg")) ] UNSPEC_HEXAGON_val_for_valignb))]
  ""
  "%0 = %1 // val for valignb"
  [(set_attr "type" "S")]
)



;;
;; A pictorial illustration of Circular Load/Store intrinsics
;; (use load word, i.e. memw as an example)
;;
;; =========================================================
;;  addr = hexagon_builtin_circ_ldw(addr, &val, LK, #inc);
;; =========================================================
;;
;; 0. Operands and Result
;;    Operands:
;;      addr: The datum (4 bytes) at this location will be loaded into the CPU;
;;      &val: The address that specifies where the loaded datum will be stored
;;            into;
;;      LK:   The value that will be assgiend to the M register. It contains
;;            two values. The L value specifies the length of the circular buffer;
;;            The K value specifies the nearest power-of-2 size that is greater
;;            the length field: L < 2**(K+2)
;;      #inc: the the post-incremental value;
;;    Result:
;;      addr: the result of circular updated address. See below for details
;;
;; 1. Cicular Buffer
;;    A chunk of consecutive memory address space. Its size ranges from 3 bytes
;;    to (128k-1) bytes. Its alignment must be power-of-2, and must be bigger
;;    than its size. For a 600-byte circular buffer, it looks like this:
;;
;;    +------------------------------------------+
;;    |                                          |
;;    |                                          |
;;    +------------------------------------------+
;;    |<---------------- 1024 bytes ------------>|
;;    |<------ 600 bytes ------>|
;;  start                      end
;;   = 0x#####000
;;     0x#####400
;;     0x#####800
;;     0x#####C00
;;
;; 2. Circular Addressing
;;    After the datum at location "addr" is loaded, it will be updated in a
;;    circular way.
;;
;;    addr = addr + #inc;
;;    if (addr >= (start+length))
;;        addr = addr - length;
;;    else if (addr < start)
;;        addr = addr + length;
;;    return addr;
;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_ldd(addr, &val, LK, #inc);
;; Rdd=memd(Rx++#s4:3:circ(Mu));
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define_expand "hexagon_builtin_circ_ldd"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is7")
        ] UNSPEC_HEXAGON_circ_ldd))]
  ""
  {
    rtx val;
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_ldd intrinsic can not be NULL");
    }
    if (GET_CODE (operands[4]) != CONST_INT) {
      error("non-constant integer passed to argument 4 of HEXAGON_circ_ldd intrinsic");
    }
    if (!s7_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_ldd intrinsic cannot be encoded as s7 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(DImode);

    ptr = gen_rtx_MEM(DImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_ldd(val,
                                         ptr,
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    emit_move_insn(gen_rtx_MEM(DImode, operands[2]), val);

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_ldd"
  [(parallel [ (set (match_operand:DI 0 "gr_register_operand" "=Rg")
                    (match_operand:DI 1 "memory_operand" "m"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is7")
                    ] UNSPEC_HEXAGON_circ_ldd))])]
  ""
  "%P0=memd(%3++#%4:circ(m0))"
  [(set_attr "type" "Load")]
)



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_ldw(addr, &val, LK, #inc);
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define_expand "hexagon_builtin_circ_ldw"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is6")
        ] UNSPEC_HEXAGON_circ_ldw))]
  ""
  {
    rtx val;
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_ldw intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_ldw intrinsic");
    }
    if (!s6_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_ldw intrinsic cannot be encoded as s6 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(SImode);
    ptr = gen_rtx_MEM(SImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_ldw(val,
                                         ptr,
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    emit_move_insn(gen_rtx_MEM(SImode, operands[2]), val);

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_ldw"
  [(parallel [ (set (match_operand:SI 0 "gr_register_operand" "=Rg")
                    (match_operand:SI 1 "memory_operand" "m"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is6")
                    ] UNSPEC_HEXAGON_circ_ldw))])]
  ""
  "%0=memw(%3++#%4:circ(m0))"
  [(set_attr "type" "Load")]
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_ldh(addr, &val, LK, #inc);
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define_expand "hexagon_builtin_circ_ldh"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is5")
        ] UNSPEC_HEXAGON_circ_ldh))]
  ""
  {
    rtx val;
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_ldh intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_ldh intrinsic");
    }
    if (!s5_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_ldh intrinsic cannot be encoded as s5 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(HImode);
    ptr = gen_rtx_MEM(HImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_ldh(val,
                                         ptr,
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    emit_move_insn(gen_rtx_MEM(HImode, operands[2]), val);

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_ldh"
  [(parallel [ (set (match_operand:HI 0 "gr_register_operand" "=Rg")
                    (match_operand:HI 1 "memory_operand" "m"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is5")
                    ] UNSPEC_HEXAGON_circ_ldh))])]
  ""
  "%0=memh(%3++#%4:circ(m0))"
  [(set_attr "type" "Load")]
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_lduh(addr, &val, LK, #inc);
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define_expand "hexagon_builtin_circ_lduh"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is5")
        ] UNSPEC_HEXAGON_circ_lduh))]
  ""
  {
    rtx val;
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_lduh intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_lduh intrinsic");
    }
    if (!s5_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_lduh intrinsic cannot be encoded as s5 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(HImode);
    ptr = gen_rtx_MEM(HImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_lduh(val,
                                         ptr,
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    emit_move_insn(gen_rtx_MEM(HImode, operands[2]), val);

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_lduh"
  [(parallel [ (set (match_operand:HI 0 "gr_register_operand" "=Rg")
                    (match_operand:HI 1 "memory_operand" "m"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is5")
                    ] UNSPEC_HEXAGON_circ_lduh))])]
  ""
  "%0=memuh(%3++#%4:circ(m0))"
  [(set_attr "type" "Load")]
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_ldb(addr, &val, LK, #inc);
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define_expand "hexagon_builtin_circ_ldb"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is4")
        ] UNSPEC_HEXAGON_circ_ldb))]
  ""
  {
    rtx val;
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_ldb intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_ldb intrinsic");
    }
    if (!s4_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_ldb intrinsic cannot be encoded as s4 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(QImode);
    ptr = gen_rtx_MEM(QImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_ldb(val,
                                         ptr,
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    emit_move_insn(gen_rtx_MEM(QImode, operands[2]), val);

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_ldb"
  [(parallel [ (set (match_operand:QI 0 "gr_register_operand" "=Rg")
                    (match_operand:QI 1 "memory_operand" "m"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is4")
                    ] UNSPEC_HEXAGON_circ_ldb))])]
  ""
  "%0=memb(%3++#%4:circ(m0))"
  [(set_attr "type" "Load")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_ldub(addr, &val, LK, #inc);
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define_expand "hexagon_builtin_circ_ldub"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is4")
        ] UNSPEC_HEXAGON_circ_ldub))]
  ""
  {
    rtx val;
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_ldub intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_ldub intrinsic");
    }
    if (!s4_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_ldub intrinsic cannot be encoded as s4 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(QImode);
    ptr = gen_rtx_MEM(QImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_ldub(val,
                                         ptr,
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    emit_move_insn(gen_rtx_MEM(QImode, operands[2]), val);

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_ldub"
  [(parallel [ (set (match_operand:QI 0 "gr_register_operand" "=Rg")
                    (match_operand:QI 1 "memory_operand" "m"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is4")
                    ] UNSPEC_HEXAGON_circ_ldub))])]
  ""
  "%0=memub(%3++#%4:circ(m0))"
  [(set_attr "type" "Load")]
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_std(addr, val, LK, #inc);
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "hexagon_builtin_circ_std"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:DI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is7")
        ] UNSPEC_HEXAGON_circ_std))]
  ""
  {
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_std intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_std intrinsic");
    }
    if (!s7_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_std intrinsic cannot be encoded as s7 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    ptr = gen_rtx_MEM(DImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_std(ptr,
                                         operands[2],
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_std"
  [(parallel [ (set (match_operand:DI 0 "memory_operand" "=m")
                    (match_operand:DI 1 "gr_register_operand" "Rg"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is7")
                    ] UNSPEC_HEXAGON_circ_std))])]
  ""
  "memd(%3++#%4:circ(m0))=%P1"
  [(set_attr "type" "Store")]
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_stw(addr, val, LK, #inc);
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "hexagon_builtin_circ_stw"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is6")
        ] UNSPEC_HEXAGON_circ_stw))]
  ""
  {
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_stw intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_stw intrinsic");
    }
    if (!s6_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_stw intrinsic cannot be encoded as s6 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    ptr = gen_rtx_MEM(SImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_stw(ptr,
                                         operands[2],
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_stw"
  [(parallel [ (set (match_operand:SI 0 "memory_operand" "=m")
                    (match_operand:SI 1 "gr_register_operand" "Rg"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is6")
                    ] UNSPEC_HEXAGON_circ_stw))])]
  ""
  "memw(%3++#%4:circ(m0))=%1"
  [(set_attr "type" "Store")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_sth(addr, val, LK, #inc);
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "hexagon_builtin_circ_sth"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:HI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is5")
        ] UNSPEC_HEXAGON_circ_sth))]
  ""
  {
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_sth intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_sth intrinsic");
    }
    if (!s5_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_sth intrinsic cannot be encoded as s5 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    ptr = gen_rtx_MEM(HImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_sth(ptr,
                                         operands[2],
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_sth"
  [(parallel [ (set (match_operand:HI 0 "memory_operand" "=m")
                    (match_operand:HI 1 "gr_register_operand" "Rg"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is5")
                    ] UNSPEC_HEXAGON_circ_sth))])]
  ""
  "memh(%3++#%4:circ(m0))=%1"
  [(set_attr "type" "Store")]
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_sthhi(addr, val, LK, #inc);
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "hexagon_builtin_circ_sthhi"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is5")
        ] UNSPEC_HEXAGON_circ_sthhi))]
  ""
  {
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_sthhi intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_sthhi intrinsic");
    }
    if (!s5_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_sthhi intrinsic cannot be encoded as s5 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    ptr = gen_rtx_MEM(HImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_sthhi(ptr,
                                         operands[2],
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_sthhi"
  [(parallel [ (set (match_operand:HI 0 "memory_operand" "=m")
                    (subreg:HI (lshiftrt:SI (match_operand:SI 1 "gr_register_operand" "Rg")
                                            (const_int 16)) 0))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is5")
                    ] UNSPEC_HEXAGON_circ_sthhi))])]
  ""
  "memh(%3++#%4:circ(m0))=%1.h"
  [(set_attr "type" "Store")]
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_circ_stb(addr, val, LK, #inc);
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "hexagon_builtin_circ_stb"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:QI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
          (match_operand:SI 4 "immediate_operand" "Is4")
        ] UNSPEC_HEXAGON_circ_stb))]
  ""
  {
    rtx m0;
    rtx ptr;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL ||
        operands[4] == NULL) {
      error("argument passed to HEXAGON_circ_stb intrinsic can not be NULL");
    }
    if(GET_CODE (operands[4]) != CONST_INT){
      error("non-constant integer passed to argument 4 of HEXAGON_circ_stb intrinsic");
    }
    if (!s4_const_int_operand(operands[4], SImode)) {
      error("argument 4 of HEXAGON_circ_stb intrinsic cannot be encoded as s4 immediate");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    ptr = gen_rtx_MEM(QImode, operands[1]);

    emit_insn (gen_hexagon_unspec_circ_stb(ptr,
                                         operands[2],
                                         operands[0],
                                         operands[1],
                                         operands[4]));

    DONE;
  }
)

(define_insn "hexagon_unspec_circ_stb"
  [(parallel [ (set (match_operand:QI 0 "memory_operand" "=m")
                    (match_operand:QI 1 "gr_register_operand" "Rg"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (unspec:SI [
                      (match_operand:SI 3 "pmode_register_operand" "2")
                      (reg:SI M0_REGNUM)
                      (match_operand:SI 4 "immediate_operand" "Is4")
                    ] UNSPEC_HEXAGON_circ_stb))])]
  ""
  "memb(%3++#%4:circ(m0))=%1"
  [(set_attr "type" "Store")]
)





;;
;; A pictorial illustration of bit-reverse Load/Store intrinsics
;; (use load word, i.e. memw as an example)
;;
;; =========================================================
;;  addr = hexagon_builtin_brev_ldw(addr, &val, M);
;; =========================================================
;;
;; 0. Operands and Result
;;    Operands:
;;      addr: A 32-bit value. Its higher 16 bits are kept untouched.
;;            Its lower 16 bits are bit-reversed. The result will be used
;;            as the address to load a datum from.
;;      &val: The loaded datum will be saved to here.
;;      M:    the post-incremental value that stored in the M register;
;;    Result:
;;      addr: the post-incremented address: addr = addr + M;
;;
;; 1. bit-reversed buffer
;;    The length of the bit-reversal buffer must be an aligned power-of-2.
;;    It must not exceed 64k bytes.
;;    The alignment of the buffer must be a power-of-2 value and should be
;;    greater than or equal to the length. Alignment of 64KB is recommended
;;    for simplicity
;;
;; 2. Bit-reverse Addressing
;;    Given an address "addr", its lower 16 bits are reversed,
;;    i.e. bit 0 is exchanged with bit 15, bit 1 is exchanged with bit 14,
;;    and so on. The result is called "effective address", which is used
;;    to load from memory.
;;    To implement reversals of fewer than 16 bits, the address-modifier
;;    register must be set to the value of:
;;     1<<(16-log2(size in bytes of bit-reverse buffer))
;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_ldd(addr, &val, M);
;; m0 = LK;
;; addr = addr.h[1] | brev(addr.h[0])
;; *val = memw[addr];
;; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_ldd"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_ldd))]
  ""
  {
    rtx val;
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_ldd intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(DImode);

    emit_insn (gen_hexagon_unspec_brev_ldd(val,
                                         operands[1],
                                         operands[0],
                                         operands[1]));

    emit_move_insn(gen_rtx_MEM(DImode, operands[2]), val);

    DONE;
  }
)
(define_insn "hexagon_unspec_brev_ldd"
  [(parallel [ (set (match_operand:DI 0 "gr_register_operand" "=Rg")
                    (mem:DI (unspec:SI [
                              (match_operand:SI 1 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_ldd)))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "%P0=memd(%3++m0:brev)"
  [(set_attr "type" "Load")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_ldw(addr, &val, M);
;; m0 = LK; addr = addr.h[1] | brev(addr.h[0])
;; *val = memw[addr]; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_ldw"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_ldw))]
  ""
  {
    rtx val;
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_ldw intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(SImode);

    emit_insn (gen_hexagon_unspec_brev_ldw(val,
                                         operands[1],
                                         operands[0],
                                         operands[1]));

    emit_move_insn(gen_rtx_MEM(SImode, operands[2]), val);

    DONE;
  }
)
(define_insn "hexagon_unspec_brev_ldw"
  [(parallel [ (set (match_operand:SI 0 "gr_register_operand" "=Rg")
                    (mem:SI (unspec:SI [
                              (match_operand:SI 1 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_ldw)))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "%0=memw(%3++m0:brev)"
  [(set_attr "type" "Load")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_ldh(addr, &val, M);
;; m0 = LK; addr = addr.h[1] | brev(addr.h[0])
;; *val = memw[addr]; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_ldh"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_ldh))]
  ""
  {
    rtx val;
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_ldh intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(HImode);

    emit_insn (gen_hexagon_unspec_brev_ldh(val,
                                         operands[1],
                                         operands[0],
                                         operands[1]));

    emit_move_insn(gen_rtx_MEM(HImode, operands[2]), val);

    DONE;
  }
)
(define_insn "hexagon_unspec_brev_ldh"
  [(parallel [ (set (match_operand:HI 0 "gr_register_operand" "=Rg")
                    (mem:HI (unspec:SI [
                              (match_operand:SI 1 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_ldh)))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "%0=memh(%3++m0:brev)"
  [(set_attr "type" "Load")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_lduh(addr, &val, M);
;; m0 = LK; addr = addr.h[1] | brev(addr.h[0])
;; *val = memw[addr]; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_lduh"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_lduh))]
  ""
  {
    rtx val;
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_lduh intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(HImode);

    emit_insn (gen_hexagon_unspec_brev_lduh(val,
                                         operands[1],
                                         operands[0],
                                         operands[1]));

    emit_move_insn(gen_rtx_MEM(HImode, operands[2]), val);

    DONE;
  }
)
(define_insn "hexagon_unspec_brev_lduh"
  [(parallel [ (set (match_operand:HI 0 "gr_register_operand" "=Rg")
                    (mem:HI (unspec:SI [
                              (match_operand:SI 1 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_lduh)))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "%0=memuh(%3++m0:brev)"
  [(set_attr "type" "Load")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_ldb(addr, &val, M);
;; m0 = LK; addr' = addr.h[1] | brev(addr.h[0])
;; *val = memw[addr']; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_ldb"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_ldb))]
  ""
  {
    rtx val;
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_ldb intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(QImode);

    emit_insn (gen_hexagon_unspec_brev_ldb(val,
                                         operands[1],
                                         operands[0],
                                         operands[1]));

    emit_move_insn(gen_rtx_MEM(QImode, operands[2]), val);

    DONE;
  }
)
(define_insn "hexagon_unspec_brev_ldb"
  [(parallel [ (set (match_operand:QI 0 "gr_register_operand" "=Rg")
                    (mem:QI (unspec:SI [
                              (match_operand:SI 1 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_ldb)))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "%0=memb(%3++m0:brev)"
  [(set_attr "type" "Load")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_ldub(addr, &val, M);
;; m0 = LK; addr = addr.h[1] | brev(addr.h[0])
;; *val = memw[addr]; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_ldub"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "pmode_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_ldub))]
  ""
  {
    rtx val;
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_ldub intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    val = gen_reg_rtx(QImode);

    emit_insn (gen_hexagon_unspec_brev_ldub(val,
                                         operands[1],
                                         operands[0],
                                         operands[1]));

    emit_move_insn(gen_rtx_MEM(QImode, operands[2]), val);

    DONE;
  }
)
(define_insn "hexagon_unspec_brev_ldub"
  [(parallel [ (set (match_operand:QI 0 "gr_register_operand" "=Rg")
                    (mem:QI (unspec:SI [
                              (match_operand:SI 1 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_ldub)))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "%0=memub(%3++m0:brev)"
  [(set_attr "type" "Load")]
)



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_std(addr, val, M);
;; m0 = LK;
;; addr = addr.h[1] | brev(addr.h[0])
;; memw[addr] = val;
;; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_std"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:DI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_std))]
  ""
  {
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_std intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    emit_insn (gen_hexagon_unspec_brev_std(operands[1],
                                         operands[2],
                                         operands[0],
                                         operands[1]));
    DONE;
  }
)
(define_insn "hexagon_unspec_brev_std"
  [(parallel [ (set (mem:DI (unspec:SI [
                              (match_operand:SI 0 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_std))
                    (match_operand:DI 1 "gr_register_operand" "Rg"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "memd(%3++m0:brev)=%P1"
  [(set_attr "type" "Store")]
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_stw(addr, val, M);
;; m0 = LK;
;; addr = addr.h[1] | brev(addr.h[0])
;; memw[addr] = val;
;; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_stw"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_stw))]
  ""
  {
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_stw intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    emit_insn (gen_hexagon_unspec_brev_stw(operands[1],
                                         operands[2],
                                         operands[0],
                                         operands[1]));
    DONE;
  }
)
(define_insn "hexagon_unspec_brev_stw"
  [(parallel [ (set (mem:SI (unspec:SI [
                              (match_operand:SI 0 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_stw))
                    (match_operand:SI 1 "gr_register_operand" "Rg"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "memw(%3++m0:brev)=%1"
  [(set_attr "type" "Store")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_sth(addr, val, M);
;; m0 = LK; addr = addr.h[1] | brev(addr.h[0]); memw[addr] = val; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_sth"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:HI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_sth))]
  ""
  {
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_sth intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    emit_insn (gen_hexagon_unspec_brev_sth(operands[1],
                                         operands[2],
                                         operands[0],
                                         operands[1]));
    DONE;
  }
)
(define_insn "hexagon_unspec_brev_sth"
  [(parallel [ (set (mem:HI (unspec:SI [
                              (match_operand:SI 0 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_sth))
                    (match_operand:HI 1 "gr_register_operand" "Rg"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "memh(%3++m0:brev)=%1"
  [(set_attr "type" "Store")]
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_sthhi(addr, val, M);
;; m0 = LK; addr = addr.h[1] | brev(addr.h[0])
;; memw[addr] = val; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_sthhi"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:SI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_sthhi))]
  ""
  {
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_sthhi intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    emit_insn (gen_hexagon_unspec_brev_sthhi(operands[1],
                                           operands[2],
                                           operands[0],
                                           operands[1]));
    DONE;
  }
)
(define_insn "hexagon_unspec_brev_sthhi"
  [(parallel [ (set (mem:HI (unspec:SI [
                              (match_operand:SI 0 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_sthhi))
                    (subreg:HI (lshiftrt:SI (match_operand:SI 1 "gr_register_operand" "Rg")
                                            (const_int 16)) 0))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "memh(%3++m0:brev)=%1.h"
  [(set_attr "type" "Store")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; addr = hexagon_builtin_brev_stb(addr, val, M);
;; m0 = LK; ;; addr = addr.h[1] | brev(addr.h[0])
;; memw[addr] = val; ;; addr = addr + M;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "hexagon_builtin_brev_stb"
  [(set (match_operand:SI 0 "pmode_register_operand" "=Rg")
        (unspec:SI [
          (match_operand:SI 1 "pmode_register_operand" "0")
          (match_operand:QI 2 "gr_register_operand" "Rg")
          (match_operand:SI 3 "gr_register_operand" "Rg")
        ] UNSPEC_HEXAGON_brev_stb))]
  ""
  {
    rtx m0;

    if (operands[0] == NULL ||
        operands[1] == NULL ||
        operands[2] == NULL ||
        operands[3] == NULL) {
      error("argument passed to HEXAGON_brev_stb intrinsic can not be NULL");
    }

    m0 = gen_rtx_REG(SImode, M0_REGNUM);
    emit_move_insn(m0, operands[3]);

    emit_insn (gen_hexagon_unspec_brev_stb(operands[1],
                                         operands[2],
                                         operands[0],
                                         operands[1]));
    DONE;
  }
)
(define_insn "hexagon_unspec_brev_stb"
  [(parallel [ (set (mem:QI (unspec:SI [
                              (match_operand:SI 0 "pmode_register_operand" "Rg")
                            ] UNSPEC_HEXAGON_brev_stb))
                    (match_operand:QI 1 "gr_register_operand" "Rg"))
               (set (match_operand:SI 2 "pmode_register_operand" "=Rg")
                    (plus:SI (match_operand:SI 3 "pmode_register_operand" "2")
                             (reg:SI M0_REGNUM)))])]
  ""
  "memb(%3++m0:brev)=%1"
  [(set_attr "type" "Store")]
)



