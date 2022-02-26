/* DWARF2 EH unwinding support for Hexagon.
   Copyright (C) 2004, 2005, 2009, 2011 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#ifndef inhibit_libc

#include <signal.h>
#ifdef __UCLIBC__
# include <asm/ucontext.h>
#else
# include <sys/ucontext.h>
#endif

/* Do code reading to identify a signal frame, and set the frame
   state data appropriately.  See unwind-dw2.c for the structs.  */

#define MD_FALLBACK_FRAME_STATE_FOR hexagon_fallback_frame_state

static _Unwind_Reason_Code
hexagon_fallback_frame_state (struct _Unwind_Context *context,
                          _Unwind_FrameState *fs)
{
	long new_cfa;
	struct sigcontext *sc;

	const unsigned int *pc = context->ra;

	/* As of kernel 2.6.31, the signal trampoline is:
	   <__rt_sigtramp_template>:
           0:   7800d166     r6 = #139  __NR_rt_sigreturn
           4:   5400c004     trap0 (#1)
           This trampoline is located on a VDSO page, and not on the stack.
           (i.e. the pc should point at where the VDSO page was mapped.)
 	 */
	if ((pc[0] != 0x7800d166) || (pc[1] != 0x5400c004))
		return _URC_END_OF_STACK;

        /* The cfa is, of course, the "current frame address" */
	if (context->cfa == 0)
		return _URC_END_OF_STACK;

	/* From the kernel ... */
	struct rt_sigframe {
     unsigned long tramp[2];
     struct siginfo info;
     struct ucontext uc;
   } *rts = context->cfa;
	sc = &rts->uc.uc_mcontext;

	fs->retaddr_column = DWARF_ALT_FRAME_RETURN_COLUMN;

	fs->regs.cfa_how = CFA_REG_OFFSET;
	new_cfa = sc->sc_regs.r30;
	fs->regs.cfa_reg = 30;

	/* CFA for hexagon is the previous frame's stack pointer,
	 * i.e. its 8 bytes above the current frame pointer.
	 * See dwarf2out_def_cfa in hexagon.c for details. */
	fs->regs.cfa_offset = 8;
	new_cfa += 8;

	fs->regs.reg[0].how = REG_SAVED_OFFSET;
       fs->regs.reg[0].loc.offset = (long) &sc->sc_regs.r0 - new_cfa;
	fs->regs.reg[1].how = REG_SAVED_OFFSET;
       fs->regs.reg[1].loc.offset = (long) &sc->sc_regs.r1 - new_cfa;
	fs->regs.reg[2].how = REG_SAVED_OFFSET;
       fs->regs.reg[2].loc.offset = (long) &sc->sc_regs.r2 - new_cfa;
	fs->regs.reg[3].how = REG_SAVED_OFFSET;
       fs->regs.reg[3].loc.offset = (long) &sc->sc_regs.r3 - new_cfa;
	fs->regs.reg[4].how = REG_SAVED_OFFSET;
       fs->regs.reg[4].loc.offset = (long) &sc->sc_regs.r4 - new_cfa;
	fs->regs.reg[5].how = REG_SAVED_OFFSET;
       fs->regs.reg[5].loc.offset = (long) &sc->sc_regs.r5 - new_cfa;
	fs->regs.reg[6].how = REG_SAVED_OFFSET;
       fs->regs.reg[6].loc.offset = (long) &sc->sc_regs.r6 - new_cfa;
	fs->regs.reg[7].how = REG_SAVED_OFFSET;
       fs->regs.reg[7].loc.offset = (long) &sc->sc_regs.r7 - new_cfa;
	fs->regs.reg[8].how = REG_SAVED_OFFSET;
       fs->regs.reg[8].loc.offset = (long) &sc->sc_regs.r8 - new_cfa;
	fs->regs.reg[9].how = REG_SAVED_OFFSET;
       fs->regs.reg[9].loc.offset = (long) &sc->sc_regs.r9 - new_cfa;

	fs->regs.reg[10].how = REG_SAVED_OFFSET;
	fs->regs.reg[10].loc.offset = (long) &sc->sc_regs.r10 - new_cfa;
	fs->regs.reg[11].how = REG_SAVED_OFFSET;
	fs->regs.reg[11].loc.offset = (long) &sc->sc_regs.r11 - new_cfa;
	fs->regs.reg[12].how = REG_SAVED_OFFSET;
	fs->regs.reg[12].loc.offset = (long) &sc->sc_regs.r12 - new_cfa;
	fs->regs.reg[13].how = REG_SAVED_OFFSET;
	fs->regs.reg[13].loc.offset = (long) &sc->sc_regs.r13 - new_cfa;
	fs->regs.reg[14].how = REG_SAVED_OFFSET;
	fs->regs.reg[14].loc.offset = (long) &sc->sc_regs.r14 - new_cfa;
	fs->regs.reg[15].how = REG_SAVED_OFFSET;
	fs->regs.reg[15].loc.offset = (long) &sc->sc_regs.r15 - new_cfa;
	fs->regs.reg[16].how = REG_SAVED_OFFSET;
	fs->regs.reg[16].loc.offset = (long) &sc->sc_regs.r16 - new_cfa;
	fs->regs.reg[17].how = REG_SAVED_OFFSET;
	fs->regs.reg[17].loc.offset = (long) &sc->sc_regs.r17 - new_cfa;
	fs->regs.reg[18].how = REG_SAVED_OFFSET;
	fs->regs.reg[18].loc.offset = (long) &sc->sc_regs.r18 - new_cfa;
	fs->regs.reg[19].how = REG_SAVED_OFFSET;
	fs->regs.reg[19].loc.offset = (long) &sc->sc_regs.r19 - new_cfa;

	fs->regs.reg[20].how = REG_SAVED_OFFSET;
	fs->regs.reg[20].loc.offset = (long) &sc->sc_regs.r20 - new_cfa;
	fs->regs.reg[21].how = REG_SAVED_OFFSET;
	fs->regs.reg[21].loc.offset = (long) &sc->sc_regs.r21 - new_cfa;
	fs->regs.reg[22].how = REG_SAVED_OFFSET;
	fs->regs.reg[22].loc.offset = (long) &sc->sc_regs.r22 - new_cfa;
	fs->regs.reg[23].how = REG_SAVED_OFFSET;
	fs->regs.reg[23].loc.offset = (long) &sc->sc_regs.r23 - new_cfa;
	fs->regs.reg[24].how = REG_SAVED_OFFSET;
	fs->regs.reg[24].loc.offset = (long) &sc->sc_regs.r24 - new_cfa;
	fs->regs.reg[25].how = REG_SAVED_OFFSET;
	fs->regs.reg[25].loc.offset = (long) &sc->sc_regs.r25 - new_cfa;
	fs->regs.reg[26].how = REG_SAVED_OFFSET;
	fs->regs.reg[26].loc.offset = (long) &sc->sc_regs.r26 - new_cfa;
	fs->regs.reg[27].how = REG_SAVED_OFFSET;
	fs->regs.reg[27].loc.offset = (long) &sc->sc_regs.r27 - new_cfa;
	fs->regs.reg[28].how = REG_SAVED_OFFSET;
	fs->regs.reg[28].loc.offset = (long) &sc->sc_regs.r28 - new_cfa;
	fs->regs.reg[29].how = REG_SAVED_OFFSET;
	fs->regs.reg[29].loc.offset = (long) &sc->sc_regs.r29 - new_cfa;
#if 0
	fs->regs.reg[30].how = REG_SAVED_OFFSET;
	fs->regs.reg[30].loc.offset = (long) &sc->sc_regs.r30 - new_cfa;
#endif
	fs->regs.reg[31].how = REG_SAVED_OFFSET;
	fs->regs.reg[31].loc.offset = (long) &sc->sc_regs.r31 - new_cfa;

	fs->regs.reg[32].how = REG_SAVED_OFFSET;
	fs->regs.reg[32].loc.offset = (long) &sc->sc_regs.sa0 - new_cfa;
	fs->regs.reg[33].how = REG_SAVED_OFFSET;
	fs->regs.reg[33].loc.offset = (long) &sc->sc_regs.lc0 - new_cfa;
	fs->regs.reg[34].how = REG_SAVED_OFFSET;
	fs->regs.reg[34].loc.offset = (long) &sc->sc_regs.sa1 - new_cfa;
	fs->regs.reg[35].how = REG_SAVED_OFFSET;
	fs->regs.reg[35].loc.offset = (long) &sc->sc_regs.lc1 - new_cfa;
	fs->regs.reg[36].how = REG_SAVED_OFFSET;
	fs->regs.reg[36].loc.offset = (long) &sc->sc_regs.m0 - new_cfa;
	fs->regs.reg[37].how = REG_SAVED_OFFSET;
	fs->regs.reg[37].loc.offset = (long) &sc->sc_regs.m1 - new_cfa;
	fs->regs.reg[38].how = REG_SAVED_OFFSET;
	fs->regs.reg[38].loc.offset = (long) &sc->sc_regs.usr - new_cfa;
	fs->regs.reg[39].how = REG_SAVED_OFFSET;
       fs->regs.reg[39].loc.offset = (long) &sc->sc_regs.p3_0 - new_cfa;
       fs->regs.reg[40].how = REG_SAVED_OFFSET;
       fs->regs.reg[40].loc.offset = (long) &sc->sc_regs.gp - new_cfa;
       fs->regs.reg[41].how = REG_SAVED_OFFSET;
       fs->regs.reg[41].loc.offset = (long) &sc->sc_regs.ugp - new_cfa;

       fs->regs.reg[DWARF_ALT_FRAME_RETURN_COLUMN].how = REG_SAVED_OFFSET;
       fs->regs.reg[DWARF_ALT_FRAME_RETURN_COLUMN].loc.offset
	 = (long) &sc->sc_regs.pc - new_cfa;

	/* Program counter is at insn after */
	fs->signal_frame = 1;

	return _URC_NO_REASON;
}

#endif
