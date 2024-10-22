/* Prototypes for CDP1802 functions used in the md file and elsewhere.
   Copyright (C) 2024 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_CDP1802_PROTOS_H
#define GCC_CDP1802_PROTOS_H

extern void cdp1802_cpu_cpp_builtins (cpp_reader *);
extern bool cdp1802_direct_return_p (void);
extern bool cdp1802_epilogue_uses_p (int);
extern bool cdp1802_expand_ashlhi3 (rtx, rtx, rtx);
extern void cdp1802_expand_call (rtx, rtx, rtx);
extern void cdp1802_expand_cbranchhi4 (enum rtx_code, rtx, rtx, rtx);
extern void cdp1802_expand_epilogue (bool);
extern void cdp1802_expand_nonlocal_goto (rtx, rtx, rtx, rtx);
extern void cdp1802_expand_prologue (void);
extern void cdp1802_expand_restore_stack_nonlocal (rtx, rtx);
extern void cdp1802_expand_save_stack_nonlocal (rtx, rtx);
extern void cdp1802_function_profiler (FILE *, int);
extern int cdp1802_initial_elimination_offset (int, int);
extern void cdp1802_output_addr_vec_elt (FILE *, int);
extern void cdp1802_output_align (FILE *, int);
extern void cdp1802_output_align_with_nop (FILE *, int);
extern void cdp1802_output_reg_pop (FILE *, int);
extern void cdp1802_output_reg_push (FILE *, int);
extern void cdp1802_print_operand_address (FILE *, rtx);
extern poly_int64 cdp1802_push_rounding (poly_int64);
extern bool cdp1802_regno_ok_for_base_p (int);
extern enum reg_class cdp1802_regno_reg_class (int);
extern const char *cdp1802_template_incdec (bool, rtx);
extern const char *cdp1802_template_movdf_r_r (rtx, rtx);
extern const char *cdp1802_template_movsf_r_r (rtx, rtx);

#endif /* GCC_CDP1802_PROTOS_H */
