/* MIPS support code for fibers and multithreading.
   Copyright (C) 2019-2025 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#include "../common/threadasm.S"

#if _MIPS_SIM == _ABIO32

/**
 * Performs a context switch.
 *
 * $a0 - void** - ptr to old stack pointer
 * $a1 - void*  - new stack pointer
 *
 */
    .text
    .globl fiber_switchContext
    .align 2
    .ent fiber_switchContext,0
fiber_switchContext:
    .cfi_startproc
    addiu $sp, $sp, -(10 * 4)

    // fp regs and return address are stored below the stack
    // because we don't want the GC to scan them.

#ifdef __mips_hard_float
#define ALIGN8(val) (val + (-val & 7))
#define BELOW (ALIGN8(6 * 8 + 4))
    s.d $f20, (0 * 8 - BELOW)($sp)
    s.d $f22, (1 * 8 - BELOW)($sp)
    s.d $f24, (2 * 8 - BELOW)($sp)
    s.d $f26, (3 * 8 - BELOW)($sp)
    s.d $f28, (4 * 8 - BELOW)($sp)
    s.d $f30, (5 * 8 - BELOW)($sp)
#endif
    sw $ra, -4($sp)

    sw $s0, (0 * 4)($sp)
    sw $s1, (1 * 4)($sp)
    sw $s2, (2 * 4)($sp)
    sw $s3, (3 * 4)($sp)
    sw $s4, (4 * 4)($sp)
    sw $s5, (5 * 4)($sp)
    sw $s6, (6 * 4)($sp)
    sw $s7, (7 * 4)($sp)
    sw $s8, (8 * 4)($sp)
    sw $gp, (9 * 4)($sp)

    // swap stack pointer
    sw $sp, 0($a0)
    move $sp, $a1

#ifdef __mips_hard_float
    l.d $f20, (0 * 8 - BELOW)($sp)
    l.d $f22, (1 * 8 - BELOW)($sp)
    l.d $f24, (2 * 8 - BELOW)($sp)
    l.d $f26, (3 * 8 - BELOW)($sp)
    l.d $f28, (4 * 8 - BELOW)($sp)
    l.d $f30, (5 * 8 - BELOW)($sp)
#endif
    lw $ra, -4($sp)

    lw $s0, (0 * 4)($sp)
    lw $s1, (1 * 4)($sp)
    lw $s2, (2 * 4)($sp)
    lw $s3, (3 * 4)($sp)
    lw $s4, (4 * 4)($sp)
    lw $s5, (5 * 4)($sp)
    lw $s6, (6 * 4)($sp)
    lw $s7, (7 * 4)($sp)
    lw $s8, (8 * 4)($sp)
    lw $gp, (9 * 4)($sp)

    addiu $sp, $sp, (10 * 4)

    jr $ra // return
    .cfi_endproc
    .end fiber_switchContext
    .size fiber_switchContext,.-fiber_switchContext

#endif /* _MIPS_SIM == _ABIO32 */

#if defined(__mips64) && _MIPS_SIM == _ABI64
/************************************************************************************
 * MIPS 64 ASM BITS
 * $a0 - void** - ptr to old stack pointer
 * $a1 - void*  - new stack pointer
 *
 */
    .text
    .globl fiber_switchContext
    .align 2
    .ent fiber_switchContext,0
fiber_switchContext:
    .cfi_startproc
    daddiu $sp, $sp, -(10 * 8)

    // fp regs and return address are stored below the stack
    // because we don't want the GC to scan them.

#ifdef __mips_hard_float
#define BELOW (8 * 8 + 8)
    s.d  $f24, (0 * 8 - BELOW)($sp)
    s.d  $f25, (1 * 8 - BELOW)($sp)
    s.d  $f26, (2 * 8 - BELOW)($sp)
    s.d  $f27, (3 * 8 - BELOW)($sp)
    s.d  $f28, (4 * 8 - BELOW)($sp)
    s.d  $f29, (5 * 8 - BELOW)($sp)
    s.d  $f30, (6 * 8 - BELOW)($sp)
    s.d  $f31, (7 * 8 - BELOW)($sp)
#endif
    sd $ra, -8($sp)

    sd  $s0, (0 * 8)($sp)
    sd  $s1, (1 * 8)($sp)
    sd  $s2, (2 * 8)($sp)
    sd  $s3, (3 * 8)($sp)
    sd  $s4, (4 * 8)($sp)
    sd  $s5, (5 * 8)($sp)
    sd  $s6, (6 * 8)($sp)
    sd  $s7, (7 * 8)($sp)
    sd  $gp, (8 * 8)($sp)
    sd  $fp, (9 * 8)($sp)

    // swap stack pointer
    sd   $sp, 0($a0)
    move $sp, $a1

#ifdef __mips_hard_float
    l.d  $f24, (0 * 8 - BELOW)($sp)
    l.d  $f25, (1 * 8 - BELOW)($sp)
    l.d  $f26, (2 * 8 - BELOW)($sp)
    l.d  $f27, (3 * 8 - BELOW)($sp)
    l.d  $f28, (4 * 8 - BELOW)($sp)
    l.d  $f29, (5 * 8 - BELOW)($sp)
    l.d  $f30, (6 * 8 - BELOW)($sp)
    l.d  $f31, (7 * 8 - BELOW)($sp)
#endif
    ld $ra, -8($sp)

    ld $s0, (0 * 8)($sp)
    ld $s1, (1 * 8)($sp)
    ld $s2, (2 * 8)($sp)
    ld $s3, (3 * 8)($sp)
    ld $s4, (4 * 8)($sp)
    ld $s5, (5 * 8)($sp)
    ld $s6, (6 * 8)($sp)
    ld $s7, (7 * 8)($sp)
    ld $gp, (8 * 8)($sp)
    ld $fp, (9 * 8)($sp)

    daddiu $sp, $sp, (10 * 8)

    jr $ra // return
    .cfi_endproc
    .end fiber_switchContext
    .size fiber_switchContext,.-fiber_switchContext

#endif /* defined(__mips64) && _MIPS_SIM == _ABI64 */
