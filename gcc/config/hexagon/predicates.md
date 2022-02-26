;;-----------;;
;; Registers ;;
;;-----------;;

;; general purpose register
(define_predicate "gr_register_operand"
  (match_operand 0 "register_operand")
  {
    unsigned int regno;
    if(GET_CODE (op) == SUBREG){
      op = SUBREG_REG (op);
      /* Paradoxical SUBREGs where the smaller mode is BI are causing problems.
         Try disallowing them here. */
      if(GET_MODE (op) == BImode){
        return false;
      }
    }
    /* ??? Allow (subreg (mem))? */
    if(!REG_P (op)){
      return false;
    }
    regno = REGNO (op);
    return G_REGNO_P (regno)
           || regno == FRAME_POINTER_REGNUM
           || regno == ARG_POINTER_REGNUM
           || regno >= FIRST_PSEUDO_REGISTER;
  }
)

;; predicate register
(define_predicate "pr_register_operand"
  (match_operand 0 "register_operand")
  {
    unsigned int regno;
    if(GET_CODE (op) == SUBREG){
      op = SUBREG_REG (op);
    }
    /* ??? Allow (subreg (mem))? */
    if(!REG_P (op)){
      return false;
    }
    regno = REGNO (op);
    return P_REGNO_P (regno) || regno >= FIRST_PSEUDO_REGISTER;
  }
)

;; non-general purpose register
(define_predicate "nongr_register_operand"
  (match_operand 0 "register_operand")
  {
    if(GET_CODE (op) == SUBREG){
      op = SUBREG_REG (op);
    }
    /* ??? Allow (subreg (mem))? */
    return REG_P (op) && !G_REG_P (op);
  }
)

;; vector suitable for splatting
(define_predicate "splattable_vector"
  (match_operand 0 "gr_register_operand")
  {
    int i;
    for(i = 1; i < GET_MODE_NUNITS (mode); i++){
      if(!rtx_equal_p(XVECEXP (op, 0, i), XVECEXP (op, 0, 0))){
        return false;
      }
    }
    return true;
  }
)

(define_predicate "gr_reg_or_scratch_operand"
  (ior (match_operand 0 "gr_register_operand")
       (match_operand 0 "scratch_operand")))

;;-------------------;;
;; Memory/References ;;
;;-------------------;;

;; memory with address in register
(define_predicate "indirect_memory_operand"
  (and (match_operand 0 "memory_operand")
       (match_test "REG_P (XEXP (op, 0))"))
)

;; absolute address
(define_predicate "absolute_address_operand"
  (match_code "symbol_ref,const,const_int")
)

;; call target
(define_predicate "call_target_operand"
  (ior (match_code "symbol_ref")
       (match_operand 0 "register_operand"))
)

;; small data reference
(define_predicate "sdata_symbolic_operand"
  (match_code "symbol_ref,const")
  {
    HOST_WIDE_INT offset = 0, size = 0;
    switch(GET_CODE (op)){
      case CONST:
        op = XEXP (op, 0);
        if(!(GET_CODE (op) == PLUS
             && GET_CODE (XEXP (op, 0)) == SYMBOL_REF
             && GET_CODE (XEXP (op, 1)) == CONST_INT)){
          return false;
        }
        offset = INTVAL (XEXP (op, 1));
        op = XEXP (op, 0);
        /* FALL THROUGH */
      case SYMBOL_REF:
        if(CONSTANT_POOL_ADDRESS_P (op)){
          size = GET_MODE_SIZE (get_pool_mode(op));
          if (size > g_switch_value){
            return false;
          }
        }
        else {
          tree t;
          if(!SYMBOL_REF_SMALL_P (op)){
            return false;
          }
          t = SYMBOL_REF_DECL (op);
          if(DECL_P (t)){
            t = DECL_SIZE_UNIT (t);
          }
          else {
            t = TYPE_SIZE_UNIT (TREE_TYPE (t));
          }
          if(t && host_integerp(t, 0)){
            size = tree_low_cst(t, 0);
            if(size < 0){
              size = 0;
            }
          }
        }
        return offset >= 0 && offset <= size;
      default:
        gcc_unreachable();
    }
  }
)

(define_predicate "rmw_memory_operand"
  (match_code "mem")
  {
    rtx plus;
    int scaled_size = GET_MODE_SIZE (mode) * 64;

    if (!TARGET_MEMOPS)
      return false;

    if (REG_P (XEXP (op, 0)))
      return true;

    if (GET_CODE (XEXP (op, 0)) == PLUS)
      {
	plus = XEXP (op, 0);
	if (REG_P (XEXP (plus, 0))
	    && GET_CODE (XEXP (plus, 1)) == CONST_INT
	    && INTVAL (XEXP (plus, 1)) >= 0
	    && INTVAL (XEXP (plus, 1)) < scaled_size)
	  return true;
      }

    return false;
  }
)

(define_predicate "gr_reg_or_rmw_mem_operand"
  (match_code "reg,subreg,mem")
  {
    if (gr_register_operand (op, mode))
      return true;
    
    if (rmw_memory_operand (op, mode))
      return true;
    
    return false;
  }
)

(define_predicate "gr_reg_rmw_or_imm_operand"
  (ior (match_operand 0 "gr_reg_or_rmw_mem_operand")
       (match_operand 0 "immediate_operand")))

;;-----------;;
;; Constants ;;
;;-----------;;

;; s16 constant
(define_predicate "s16_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -32768, 32767)"))
)

;; s12 constant
(define_predicate "s12_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -2048, 2047)"))
)

;; s10 constant
(define_predicate "s10_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -512, 511)"))
)

;; s8 constant
(define_predicate "s8_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -128, 127)"))
)

;; s7 constant
(define_predicate "s7_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -64, 63)"))
)


;; s6 constant
(define_predicate "s6_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -32, 31)"))
)


;; s5 constant
(define_predicate "s5_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -16, 15)"))
)


;; s4 constant
(define_predicate "s4_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -8, 7)"))
)

;; u9 constant
(define_predicate "u9_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), 0, 511)"))
)

;; u8 constant
(define_predicate "u8_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), 0, 255)"))
)

;; u7 constant
(define_predicate "u7_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), 0, 127)"))
)

;; u6 constant
(define_predicate "u6_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), 0, 63)"))
)

;; u5 constant
(define_predicate "u5_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), 0, 31)"))
)

;; u3 constant
(define_predicate "u3_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), 0, 7)"))
)

;; u2 constant
(define_predicate "u2_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), 0, 3)"))
)

;; u1 constant
(define_predicate "u1_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), 0, 1)"))
)

;; u0 constant
(define_predicate "zero_constant"
  (and (match_code "const_int,const_double,const_vector")
       (match_test "op == CONST0_RTX (mode)"))
)

;; m9 signed magnitude constant
(define_predicate "m9_const_int_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -255, 255)"))
)

;; constant == 2^N where N is an integer
(define_predicate "power_of_two_operand"
  (and (match_code "const_int")
       (match_test "exact_log2(INTVAL (op)) != -1"))
)

;; constant with all bits set except one.
(define_predicate "inv_power_of_two_operand"
  (and (match_code "const_int")
       (match_test "exact_log2 (~INTVAL (op)) != -1"))
)

;; constant == 2^N where N is an element of {0, 1, 2, 3, 4, 5, 6, 7}
(define_predicate "addasl_const_int_operand"
  (and (match_code "const_int")
       (match_test "INTVAL (op) == (1 << 0)
                 || INTVAL (op) == (1 << 1)
                 || INTVAL (op) == (1 << 2)
                 || INTVAL (op) == (1 << 3)
                 || INTVAL (op) == (1 << 4)
                 || INTVAL (op) == (1 << 5)
                 || INTVAL (op) == (1 << 6)
                 || INTVAL (op) == (1 << 7)"))
)

;; constant that might not fit in s16
(define_predicate "immediate_not_s16_operand"
  (and (match_code "const_int,const_double,const_vector,const,label_ref,symbol_ref")
       (and (match_operand 0 "immediate_operand")
            (not (match_operand 0 "s16_const_int_operand"))))
)

(define_predicate "minus_31_to_31_operand"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), -31, 31)")))

(define_predicate "no_int_operand"
  (and (match_code "const_int")
       (match_test "false")))

;;---------------------;;
;; Memory or Registers ;;
;;---------------------;;

;; general purpose register or memory
(define_predicate "gr_reg_or_memory_operand"
  (ior (match_operand 0 "gr_register_operand")
       (match_operand 0 "memory_operand"))
)

;; non-general purpose register or memory
(define_predicate "nongr_reg_or_memory_operand"
  (ior (match_operand 0 "nongr_register_operand")
       (match_operand 0 "memory_operand"))
)

;; conditional move destination
(define_predicate "conditional_dest_operand"
  (if_then_else (match_test "!reload_completed")
                (match_operand 0 "nonimmediate_operand")
                (ior (and (match_operand 0 "memory_operand")
                          (match_test "hexagon_cond_mem_operand (GET_MODE (op),
					 op, TARGET_V4_FEATURES)"))
                     (match_operand 0 "gr_register_operand")))
)


;;------------------------;;
;; Registers or Constants ;;
;;------------------------;;

;; general purpose register or s16 constant
(define_predicate "gr_or_s16_operand"
  (ior (match_operand 0 "gr_register_operand")
       (match_operand 0 "s16_const_int_operand"))
)

;; general purpose register or u6 constant
(define_predicate "gr_or_u6_operand"
  (ior (match_operand 0 "gr_register_operand")
       (match_operand 0 "u6_const_int_operand"))
)

;; general purpose register or m9 constant
(define_predicate "gr_or_m9_operand"
  (ior (match_operand 0 "gr_register_operand")
       (match_operand 0 "m9_const_int_operand"))
)

;; conditional add operand
(define_predicate "conditional_add_operand"
  (if_then_else (match_test "!reload_completed")
                (match_operand 0 "nonmemory_operand")
                (ior (match_operand 0 "gr_register_operand")
                     (ior (and (match_test "TARGET_V4_FEATURES")
                               (match_operand 0 "immediate_operand"))
                          (match_operand 0 "s8_const_int_operand"))))
)


;;---------;;
;; General ;;
;;---------;;

(define_predicate "combine_imm_operand"
  (match_code "const,const_int,symbol_ref,label_ref"))

;; conditional move source
(define_predicate "conditional_src_operand"
  (if_then_else (match_test "!reload_completed")
                (match_operand 0 "general_operand")
                (ior (and (match_operand 0 "memory_operand")
                          (match_test "hexagon_cond_mem_operand (GET_MODE (op),
					 op, TARGET_V4_FEATURES)"))
                     (ior (match_operand 0 "gr_register_operand")
                          (and (match_test "GET_MODE (op) != DImode")
                               (ior (and (match_test "TARGET_V4_FEATURES")
                                         (match_operand 0 "combine_imm_operand"))
                                    (match_operand 0 "s12_const_int_operand"))))))
)

(define_predicate "sym_or_lab_operand"
  (match_code "symbol_ref,label_ref"))

;;thread-local symbols
(define_predicate "tls_symbolic_operand"
  (and (match_code "symbol_ref")
       (and (match_test "SYMBOL_REF_TLS_MODEL (op) != TLS_MODEL_NONE")
            (match_test "SYMBOL_REF_TLS_MODEL (op) != TLS_MODEL_EMULATED"))))


;;-----------;;
;; Operators ;;
;;-----------;;

;; operator used for predication
(define_predicate "predicate_operator"
  (match_code "ne,eq")
)

(define_predicate "gpr_cond_jump_operator"
  (match_code "ne,eq,ge,le")
)

(define_predicate "subword_reg_compare_operator"
  (match_code "eq,gt,gtu,lt,ltu")
)

(define_predicate "binary_operator"
  (match_test "BINARY_P (op)")
)


;;---------------;;
;; SDATA sorting ;;
;;---------------;;

(define_predicate "GP_or_reg_operand"
  (match_operand 0 "general_operand")
  {
    enum rtx_code code = GET_CODE (op);

    if (mode == VOIDmode)	mode = GET_MODE (op);

    /* Don't accept CONST_INT or anything similar
       if the caller wants something floating.  */
    if (GET_MODE (op) == VOIDmode && mode != VOIDmode
        && GET_MODE_CLASS (mode) != MODE_INT
        && GET_MODE_CLASS (mode) != MODE_PARTIAL_INT)
      return 0;

    if (GET_CODE (op) == CONST_INT
        && mode != VOIDmode
        && trunc_int_for_mode (INTVAL (op), mode) != INTVAL (op))
      return 0;

    if (CONSTANT_P (op)){
      return ((GET_MODE (op) == VOIDmode || GET_MODE (op) == mode
               || mode == VOIDmode)
              && (! flag_pic || LEGITIMATE_PIC_OPERAND_P (op))
              && LEGITIMATE_CONSTANT_P (op));
     }

    /* Except for certain constants with VOIDmode, already checked for,
       OP's mode must match MODE if MODE specifies a mode.  */
    if (GET_MODE (op) != mode)	return 0;

    if (code == SUBREG){
        rtx sub = SUBREG_REG (op);

#ifdef INSN_SCHEDULING
        /* On machines that have insn scheduling, we want all memory
           reference to be explicit, so outlaw paradoxical SUBREGs.
           However, we must allow them after reload so that they can
           get cleaned up by cleanup_subreg_operands.  */
        if (!reload_completed && MEM_P (sub)
            && GET_MODE_SIZE (mode) > GET_MODE_SIZE (GET_MODE (sub)))
          return 0;
#endif
        /* Avoid memories with nonzero SUBREG_BYTE, as offsetting the memory
           may result in incorrect reference.  We should simplify all valid
           subregs of MEM anyway.  But allow this after reload because we
           might be called from cleanup_subreg_operands.
           ??? This is a kludge.  */
        if (!reload_completed && SUBREG_BYTE (op) != 0 && MEM_P (sub))
          return 0;

        /* FLOAT_MODE subregs can't be paradoxical.  Combine will occasionally
           create such rtl, and we must reject it.  */
        if (SCALAR_FLOAT_MODE_P (GET_MODE (op))
            && GET_MODE_SIZE (GET_MODE (op)) > GET_MODE_SIZE (GET_MODE (sub)))
          return 0;

        op = sub;
        code = GET_CODE (op);
      }
    if (code == REG)
      /* A register whose class is NO_REGS is not a general operand.  */
      return (REGNO (op) >= FIRST_PSEUDO_REGISTER
              || REGNO_REG_CLASS (REGNO (op)) != NO_REGS);

    if (code == MEM){
        rtx y = XEXP (op, 0);

        if (! volatile_ok && MEM_VOLATILE_P (op))	return 0;

        /* Use the mem's mode, since it will be reloaded thus.  */
        mode = GET_MODE (op);

        /* Catch GP relative MEM accesses */
        if(sdata_symbolic_operand(y, Pmode)){
          unsigned HOST_WIDE_INT mem_size = 0;

          if((GET_CODE (op) == MEM) && MEM_SIZE (op))
             mem_size = INTVAL (MEM_SIZE (op));

          /* This makes this predicate to reject GP relative
             addressing if access unit is smaller than declaration unit*/
          if(TARGET_SECTION_SORTING_CODE_SUPPORT
          && TARGET_SECTION_SORTING
          &&  mem_size < sdata_symbolic_operand_size(y)){
             if (!reload_completed)
               return 0;
          }
        }
        if (memory_address_p (mode, y))	return 1;
     }
    return 0;
  }
)

(define_predicate "nonimmediate_operand_with_GP"
  (match_operand 0 "nonimmediate_operand")
  {
    return (! CONSTANT_P (op) && GP_or_reg_operand (op, mode));
  }
)
