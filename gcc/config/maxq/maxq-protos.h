/*  Copyright (C) 2004, 2005 Free Software Foundation, Inc.
    Contributed by:
            Prabu Pariasamy (prabup@noida.hcltech.com)
            Konark Goel (konarkg@noida.hcltech.com)
            C Jaiprakash (cjaiprakash@noida.hcltech.com).

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef GCC_MAXQ_PROTOS_H
#define GCC_MAXQ_PROTOS_H

extern void maxq_expand_prologue (void);
extern void maxq_expand_epilogue (void);
extern int maxq_U_constraint (rtx);
extern void maxq_expand_builtin_va_start (tree, rtx);
extern void maxq_print_operand (FILE *, rtx, int);
extern int reg_or_imm_operand (rtx, enum machine_mode);
extern int call_address_operand (rtx, enum machine_mode);

#ifdef RTX_CODE

extern void maxq_gen_compare_reg (enum rtx_code, rtx, rtx);

#endif

extern rtx maxq_compare_op0;	/* Operand 0 for comparisons.  */
extern rtx maxq_compare_op1;	/* Operand 1 for comparisons.  */
extern int maxq_signed_cmp;
extern int maxq_equal;
#endif
