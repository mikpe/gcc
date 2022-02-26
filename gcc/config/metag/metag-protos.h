/* Definitions of target machine for GNU compiler.
   Imagination Technologies Meta version.
   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2010,
                 2013
   Imagination Technologies Ltd

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifdef HAVE_ATTR_metacore
extern enum attr_metacore metacore;
#endif

extern void metag_split_movsi_immediate (rtx []);
extern void metag_split_movdi (rtx []);
extern void metag_split_movdi_immediate (rtx []);
extern void metag_split_movdf (rtx []);
extern void metag_split_movdf_immediate (rtx []);
extern void metag_split_movsf (rtx []);
extern void metag_split_movsf_immediate (rtx []);
extern void metag_abort (rtx val) ATTRIBUTE_NORETURN;
extern int  metag_search_rhs (rtx);
extern bool metag_cheap_return (bool);
extern int  debug_metag_md (void);
extern unsigned int  metag_mem_base (rtx);
extern bool metag_mem_base_p (rtx, enum reg_class);
extern void metag_override_options (void);
extern bool metag_valid_machine_decl_attribute (tree, tree, tree, tree);
extern bool metag_cond_exec_p (void);
extern void metag_print_cc_if_conditional (FILE *);
extern void metag_ccexec_label (const char *);
extern bool metag_consume_branch (rtx);

extern void metag_init_expanders (void);

extern bool metag_legitimate_reg_p (rtx, bool);

extern bool metag_legitimate_regno_p (unsigned int, bool);

extern bool metag_regs_ok_for_base_offset_p (rtx, rtx, bool);
extern bool metag_reg_ok_for_index_p (rtx, bool);

extern bool metag_reg_ok_for_base_p (rtx, bool);
extern bool metag_reg_ok_for_offset_p (rtx, bool);

extern bool metag_legitimate_address_p (rtx, enum machine_mode, bool);

extern bool metag_legitimate_post_incdec_p (rtx, enum machine_mode, bool);
extern bool metag_legitimate_pre_incdec_p (rtx, enum machine_mode, bool);

extern bool metag_legitimate_off_p (rtx, rtx, enum machine_mode, bool);

extern bool metag_legitimate_twin_p (rtx, rtx, enum machine_mode, bool);

extern bool metag_return_in_memory (tree);

extern void output_fn_prologue (FILE *, int);
extern void output_fn_epilogue (FILE *, int);

extern bool output_call_addr (rtx, enum machine_mode);
extern const char * output_sibcall (rtx [], unsigned int);
extern const char * output_call (rtx [], unsigned int);
extern bool metag_slow_store (rtx, rtx);
extern rtx  metag_gen_safe_temp (enum machine_mode, rtx);
#ifdef RTX_CODE
extern enum machine_mode metag_select_cc_mode (RTX_CODE, rtx, rtx);
extern bool gen_metag_compare (RTX_CODE, rtx[], int);
#endif

extern rtx  metag_gen_load_multiple (unsigned int, unsigned int, enum machine_mode, rtx, bool, rtx, HOST_WIDE_INT *);
extern rtx  metag_gen_store_multiple (unsigned int, unsigned int, enum machine_mode, rtx, bool, rtx, HOST_WIDE_INT *);
extern bool metag_gen_movmemqi (rtx []);

extern void metag_final_prescan_insn (rtx);
extern int  metag_initial_elimination_offset (int, int);
#ifdef CUMULATIVE_ARGS
extern void metag_function_arg_advance (CUMULATIVE_ARGS *, enum machine_mode, tree, bool);
extern rtx  metag_function_arg (CUMULATIVE_ARGS *, enum machine_mode, tree, bool);
#endif

extern long metag_const_double_to_hp (rtx op, bool *inexact);

extern void metag_function_profiler (FILE *);
#ifdef RTX_CODE
extern void metag_print_operand (FILE *, rtx, RTX_CODE);
#endif
extern void metag_print_operand_address (FILE *, rtx);

extern void metag_asm_output_opcode (FILE *, const char *);

extern bool metag_frame_related_rtx (rtx);
extern bool metag_symbolic_reference_mentioned_p (rtx);
extern bool metag_legitimate_pic_address_disp_p (rtx);
extern rtx  metag_legitimize_pic_address (rtx, rtx);
extern rtx  metag_legitimize_address (rtx, rtx, enum machine_mode);
extern int  metag_letter_for_const (rtx);
extern bool metag_const_ok_for_letters_p (rtx, const char []);
extern bool metag_datareg_p (unsigned int);
extern bool metag_addrreg_p (unsigned int);
extern bool metag_fpcreg_p (unsigned int);
extern bool metag_fppreg_p (unsigned int);
extern bool metag_legitimate_modify_p (rtx, enum machine_mode, bool);

extern bool metag_same_regclass_p (rtx, rtx);

extern bool metag_regno_same_unit_p (unsigned int, unsigned int);

extern bool metag_zeroextract_mask_p (rtx, rtx);

extern rtx  metag_return_addr_rtx (int, rtx);

extern HOST_WIDE_INT metag_function_arg_boundary (enum machine_mode, tree);

extern int  metag_first_parm_offset (tree);

extern bool metag_consumer_is_cond_p (rtx, rtx);

extern bool metag_bypass_before_reload_p (rtx, rtx);

extern bool metag_hard_regno_rename_ok_p (rtx, unsigned int, unsigned int);

extern void metag_expand_prologue (void);
extern void metag_expand_epilogue (bool);

extern enum reg_class metag_regno_reg_class_minimal (unsigned int);

extern enum reg_class metag_regno_reg_class_unit (unsigned int);

extern bool metag_use_return_insn (bool);

extern bool metag_frame_pointer_required (void);

extern void metag_setup_frame_addresses (void);

extern void metag_expand_set_return_address (rtx);

extern bool metag_doloop_loop_nest_optimized(struct loop *, struct doloopnest *);

extern bool metag_doloop_check_any_nest_optimized (struct loop *, struct doloopnest *);

extern void metag_doloop_mark_nests_optimized (struct loop *, struct doloopnest **);

extern bool metag_current_function_loads_pic_register (void);

extern rtx  metag_legitimize_reload_address (rtx, enum machine_mode, int, int, int);

extern bool metag_offset6_mode (rtx, enum machine_mode);

extern bool metag_offset12_mode (rtx, enum machine_mode);

extern bool metag_regno12bit_p (unsigned int);

extern bool metag_split_early (void);

extern bool metag_split_hi_lo_sum_early (void);

extern bool metag_hard_regno_mode_ok (unsigned int, enum machine_mode);

extern void metag_override_options_per_os (void);
extern bool metag_handle_option_per_os (size_t, const char *, int);
extern bool metag_function_ok_for_sibcall_per_os (tree, tree);

/* These functions are part of a framework to allow the support of OS 
   specific builtin functions within GCC. */
extern void metag_init_builtins_per_os (void);
extern rtx  metag_expand_builtin_per_os (tree, rtx);
extern void metag_pad_function_call (rtx);
extern bool metag_tbiassert_p (rtx);

extern void metag_internal_label (FILE *, const char *, unsigned long);
extern void metag_function_prologue (FILE *, HOST_WIDE_INT);
extern void metag_function_end_prologue (FILE *);
extern void metag_function_begin_epilogue (FILE *);
extern void metag_function_epilogue (FILE *, HOST_WIDE_INT);
extern void metag_output_mi_thunk (FILE *, tree, HOST_WIDE_INT, HOST_WIDE_INT,
                                   tree);
extern bool metag_can_output_mi_thunk (tree, HOST_WIDE_INT, HOST_WIDE_INT,
                                       tree);
extern int metag_sched_adjust_cost (rtx, rtx, rtx, int);
extern bool metag_handle_option (size_t, const char *, int);
extern tree metag_merge_decl_attributes (tree, tree);
extern tree metag_merge_type_attributes (tree, tree);

#ifdef GCC_TREE_H
extern const struct attribute_spec metag_attribute_table[];
#endif

extern int metag_comp_type_attributes (tree, tree);
extern void metag_init_builtins (void);
extern rtx metag_expand_builtin (tree, rtx, rtx, enum machine_mode, int);
extern bool metag_function_ok_for_sibcall (tree, tree);
extern void metag_encode_section_info (tree, rtx, int);
extern bool metag_scalar_mode_supported_p (enum machine_mode);
extern bool metag_rtx_costs (rtx, int, int, int *);
extern int metag_address_cost (rtx);
extern void metag_machine_dependent_reorg (void);
extern tree metag_gimplify_va_arg_expr (tree, tree, tree *, tree *);
extern const char * metag_invalid_within_doloop (rtx);
extern bool metag_pass_by_reference (CUMULATIVE_ARGS *, enum machine_mode, tree,
                                     bool);
extern void metag_setup_incoming_varargs (CUMULATIVE_ARGS *, enum machine_mode,
                                          tree, int *, int);
extern bool metag_must_pass_in_stack (enum machine_mode, tree);
extern int metag_arg_partial_bytes (CUMULATIVE_ARGS *, enum machine_mode, tree,
                                    bool);
#ifdef GCC_TARGET_H
extern enum reg_class metag_secondary_reload (bool, rtx, enum reg_class,
                                              enum machine_mode,
                                              secondary_reload_info *);
#endif
extern bool metag_vector_mode_supported_p (enum machine_mode);
extern enum reg_class metag_secondary_reload_class (enum reg_class,
                                                    enum machine_mode,
                                                    rtx, bool);

extern bool metag_output_addr_const_extra (FILE *, rtx);

extern bool metag_dsp_ri16_operands (rtx[]);
extern bool metag_dsp_ri5_operands (rtx[]);
extern bool metag_dsp_rrr_operands (rtx[], bool);
extern bool metag_dsp_cmp_rrr_operands (rtx[], bool);
extern bool metag_dsp_cmp_ri16_operands (rtx[]);
extern bool metag_dsp_rrr_mov_operands (rtx[], bool);
extern bool metag_dsp_rri5_operands (rtx[]);
extern bool metag_dsp_rr_operands (rtx[]);
extern bool metag_dsp_cmp_rri5_operands (rtx[]);
extern bool metag_dsp_rr_rr_mov_operands (rtx[]);

extern void metag_dsp_peephole2_rr_convert (rtx[]);
extern void metag_dsp_peephole2_rr_mov_convert (rtx[]);
extern void metag_dsp_peephole2_rrr_convert (rtx[]);
extern void metag_dsp_peephole2_rrr_mov_convert (rtx[]);
extern void metag_dsp_peephole2_ri16_convert (rtx[]);
extern void metag_dsp_peephole2_rri5_convert (rtx[]);

extern bool metag_move_valid_p (rtx, rtx);

extern void metag_expand_didf2 (rtx, rtx);

#ifdef LIBCPP_CPPLIB_H
extern void metag_cpu_cpp_builtins (cpp_reader *);
/* Handle the jump_table_branch pragma */
extern void metag_pragma_jump_table_branch (struct cpp_reader *);
/* Handle the hwtrace_function pragma */
extern void metag_pragma_hwtrace_function (struct cpp_reader *);
#endif

extern bool metag_meta2_bex_enabled;
extern void metag_can_use_short_branch (void);
extern void metag_emit_move_sequence (rtx[], enum machine_mode);

extern rtx metag_libcall_value (enum machine_mode);
extern rtx metag_function_value (tree, tree, bool);

extern bool tls_symbolic_operand_p (rtx);
extern bool metag_bfd_tls_referenced_p (rtx);
extern rtx  metag_bfd_legitimize_tls_address (rtx);

extern bool metag_have_tls (void);

extern char * metag_gen_cond_return_branch (const char *);
extern char * metag_gen_cond_return_stub (void);
extern void   metag_emit_cond_return_stub_if_reqd (void);
