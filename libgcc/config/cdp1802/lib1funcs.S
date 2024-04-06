/* libgcc routines for the RCA CDP1802 processor.
   Copyright (C) 2024 Free Software Foundation, Inc.
   Contributed by Mikael Pettersson <mikpelinux@gmail.com>.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.

   This file is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

.macro	BEGINF	name
	.text
	.global	\name
	.type	\name, @function
\name:
.endm

.macro	ENDF	name
	.size	\name, .-\name
.endm

.macro	RET
	sep	5
.endm

#define SP		2
#define LINK		6
#define ARG0		7
#define ARG1		8
#define STATIC_CHAIN	11

/* Call via a function pointer.
   The call is made to __call_reg with the function pointer in the
   static chain register.  We push the link register and move the
   function pointer to it, then issue an SCRT return.  */
#ifdef L__call_reg
BEGINF	__call_reg
	;; push the link register
	ghi	LINK
	stxd
	glo	LINK
	stxd
	;; move callee to the link register
	ghi	STATIC_CHAIN
	phi	LINK
	glo	STATIC_CHAIN
	plo	LINK
	;; the SCRT return sequence does the rest
	RET
ENDF	__call_reg
#endif

/* Return with carry set if ARG0 == ARG1, clear otherwise.  */
#ifdef L__cmphi_eq
BEGINF	__cmphi_eq
	;; compute ARG0 - ARG1
	glo	ARG1
	str	SP
	glo	ARG0
	sm
	ghi	ARG1
	str	SP
	ghi	ARG0
	smb
	;; D is 0 if equal, non-zero otherwise
	lsz		; long skip if EQ
	ldi	0xff
	;; D is 0x00 if equal, 0xff otherwise
	xri	0xff
	;; D is 0xff if equal, 0x00 otherwise
	shr
	;; carry is 1 if equal, 0 otherwise
	RET
ENDF	__cmphi_eq
#endif

/* Return with carry set if ARG0 != ARG1, clear otherwise.  */
#ifdef L__cmphi_ne
BEGINF	__cmphi_ne
	;; compute ARG0 - ARG1
	glo	ARG1
	str	SP
	glo	ARG0
	sm
	ghi	ARG1
	str	SP
	ghi	ARG0
	smb
	;; D is 0 if equal, non-zero otherwise
	lsz		; long skip if EQ
	ldi	0xff
	;; D is 0x00 if equal, 0xff otherwise
	shr
	;; carry is 1 if non-equal, 0 otherwise
	RET
ENDF	__cmphi_ne
#endif

/* Return with carry set if ARG0 >=u ARG1, clear otherwise.  */
#ifdef L__cmphi_geu
BEGINF	__cmphi_geu
	;; compute ARG0 - ARG1
	glo	ARG1
	str	SP
	glo	ARG0
	sm
	ghi	ARG1
	str	SP
	ghi	ARG0
	smb
	;; if >=u then no borrow, i.e. carry set
	RET
ENDF	__cmphi_geu
#endif

/* Return with carry set if ARG0 <u ARG1, clear otherwise.  */
#ifdef L__cmphi_ltu
BEGINF	__cmphi_ltu
	;; compute ARG0 - ARG1
	glo	ARG1
	str	SP
	glo	ARG0
	sm
	ghi	ARG1
	str	SP
	ghi	ARG0
	smb
	;; if <u then borrow, i.e. carry clear
	;; negate the carry
	shlc
	xri	1
	shr
	;; if <u then carry set
	RET
ENDF	__cmphi_ltu
#endif

/* Return ARG0 >>u ARG1.  */
#ifdef L__lshrhi3
BEGINF	__lshrhi3
	;; if (ARG1 & 8) ARG0 >>=u 8
	glo	ARG1
	ani	8
	lbz	1f
	ghi	ARG0
	plo	ARG0

	;; clear ARG0 high byte
	ldi	0
	phi	ARG0

	glo	ARG1
1:	;; ARG1 &= 7
	ani	7
	plo	ARG1
	lbr	3f

2:	;; ARG0 >>=u 1
	ghi	ARG0
	shr
	phi	ARG0
	glo	ARG0
	shrc
	plo	ARG0

	;; --ARG1
	dec	ARG1
	glo	ARG1

3:	;; if (ARG1 != 0) goto 2
	lbnz	2b

	;; done, result in ARG0
	RET
ENDF	__lshrhi3
#endif

/* Return ARG0 >>s ARG1.  */
#ifdef L__ashrhi3
BEGINF	__ashrhi3
	;; if (ARG1 & 8) ARG0 >>=s 8
	glo	ARG1
	ani	8
	lbz	1f
	ghi	ARG0
	plo	ARG0

	;; fill ARG0 high byte with its sign bit
	shl		; sign bit to carry
	ldi	0xff
	adci	0	; 0x00 if negative, 0xff if non-negative
	xri	0xff	; 0xff if negative, 0x00 if non-negative
	phi	ARG0

	glo	ARG1
1:	;; ARG1 &= 7
	ani	7
	plo	ARG1
	lbr	3f

2:	;; ARG0 >>=s 1
	ghi	ARG0
	shr		; preload carry with the sign of ARG0
	ghi	ARG0
	shrc
	phi	ARG0
	glo	ARG0
	shrc
	plo	ARG0

	;; --ARG1
	dec	ARG1
	glo	ARG1

3:	;; if (ARG1 != 0) goto 2
	lbnz	2b

	;; done, result in ARG0
	RET
ENDF	__ashrhi3
#endif

/* Return ARG0 << ARG1.  */
#ifdef L__ashlhi3
BEGINF	__ashlhi3
	;; if (ARG1 & 8) ARG0 <<= 8
	glo	ARG1
	ani	8
	lbz	1f
	glo	ARG0
	phi	ARG0

	;; clear ARG0 low byte
	ldi	0
	plo	ARG0

	glo	ARG1
1:	;; ARG1 &= 7
	ani	7
	plo	ARG1
	lbr	3f

2:	;; ARG0 <<= 1
	glo	ARG0
	shl
	plo	ARG0
	ghi	ARG0
	shlc
	phi	ARG0

	;; --ARG1
	dec	ARG1
	glo	ARG1

3:	;; if (ARG1 != 0) goto 2
	lbnz	2b

	;; done, result in ARG0
	RET
ENDF	__ashlhi3
#endif
