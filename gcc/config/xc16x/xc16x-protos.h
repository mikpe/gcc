/*Prototypes for xc16x.c functions used in the md file & elsewhere.
   Copyright (C) 1 2006
   Free Software Foundation, Inc.

   Contributed by Shrirang Khisti On behalf of KPIT Cummins Infosystems ltd.

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
 the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1307, USA. */

extern void print_operand (FILE * file, rtx x, int code);
extern void print_operand_address (FILE * file, rtx x);
const char *output_call_insn (rtx operands[3]);
const char *output_call_insn1 (rtx operands[3]);
const char *op_or_insn (rtx * operands);
const char *op_and_insn (rtx * operands);
void xc16x_disable_intr (struct cpp_reader *);
struct cpp_reader;
extern void xc16x_interrupt_handler (struct cpp_reader *);
extern void xc16x_interrupt_noreturn (struct cpp_reader *);
const char *output_movpsi_insn (rtx *);
const char *output_addpsi_insn (rtx, rtx *, int flag);
const char *output_cmppsi_insn (rtx *);
const char *output_movhi_insn (rtx *);
unsigned int compute_mov_length (rtx * operands);
unsigned int compute_add_length (rtx * operands);
unsigned int compute_call_length (rtx * operands, int number);
const char *output_cmpsi_insn (rtx *);
extern struct rtx_def *function_arg (int, enum machine_mode, tree, int);
