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
#define ARG2		9
#define ARG3		10
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
	;; !(ARG0 ^ ARG1)
	glo	ARG1
	str	SP
	glo	ARG0
	xor
	plo	ARG0
	ghi	ARG1
	str	SP
	ghi	ARG0
	xor
	str	SP
	glo	ARG0
	or
	;; D is 0 if equal, non-zero otherwise
	lsz		; long skip if EQ
	ldi	1
	;; D is 0 if equal, 1 otherwise
	xri	1
	;; D is 1 if equal, 0 otherwise
	shr
	;; carry is 1 if equal, 0 otherwise
	RET
ENDF	__cmphi_eq
#endif

/* Return with carry set if ARG0 != ARG1, clear otherwise.  */
#ifdef L__cmphi_ne
BEGINF	__cmphi_ne
	;; !(ARG0 ^ ARG1)
	glo	ARG1
	str	SP
	glo	ARG0
	xor
	plo	ARG0
	ghi	ARG1
	str	SP
	ghi	ARG0
	xor
	str	SP
	glo	ARG0
	or
	;; D is 0 if equal, non-zero otherwise
	lsz		; long skip if EQ
	ldi	1
	;; D is 0 if equal, 1 otherwise
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

/* Return with carry set if ARG0 >u ARG1, clear otherwise.  */
#ifdef L__cmphi_gtu
BEGINF	__cmphi_gtu
	;; compute ARG1 - ARG0
	glo	ARG0
	str	SP
	glo	ARG1
	sm
	ghi	ARG0
	str	SP
	ghi	ARG1
	smb
	;; if >u then borrow, i.e. carry clear
	;; negate the carry
	shlc
	xri	1
	shr
	;; if >u then carry set
	RET
ENDF	__cmphi_gtu
#endif

/* Return with carry set if ARG0 <=u ARG1, clear otherwise.  */
#ifdef L__cmphi_leu
BEGINF	__cmphi_leu
	;; compute ARG1 - ARG0
	glo	ARG0
	str	SP
	glo	ARG1
	sm
	ghi	ARG0
	str	SP
	ghi	ARG1
	smb
	;; if <=u then no borrow, i.e. carry set
	RET
ENDF	__cmphi_leu
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

1:	;; ARG1 &= 7
	glo	ARG1
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

/* Return <ARG0,ARG1> >>u ARG2.  */
#ifdef L__lshrsi3
BEGINF	__lshrsi3
	;; if (ARG2 & 16) <ARG0,ARG1> >>=u 16
	glo	ARG2
	ani	16
	lbz	1f

	;; ARG1 = ARG0
	glo	ARG0
	plo	ARG1
	ghi	ARG0
	phi	ARG1

	;; clear ARG0
	ldi	0
	plo	ARG0
	phi	ARG0

1:	;; if (ARG2 & 8) <ARG0,ARG1> >>=u 8
	glo	ARG2
	ani	8
	lbz	2f

	ghi	ARG1
	plo	ARG1
	glo	ARG0
	phi	ARG1
	ghi	ARG0
	plo	ARG0

	;; clear ARG0 high byte
	ldi	0
	phi	ARG0

2:	;; ARG2 &= 7
	glo	ARG2
	ani	7
	plo	ARG2
	lbr	4f

3:	;; <ARG0,ARG1> >>=u 1
	ghi	ARG0
	shr
	phi	ARG0
	glo	ARG0
	shrc
	plo	ARG0
	ghi	ARG1
	shrc
	phi	ARG1
	glo	ARG1
	shrc
	plo	ARG1

	;; --ARG2
	dec	ARG2
	glo	ARG2

4:	;; if (ARG2 != 0) goto 3
	lbnz	3b

	;; done, result in <ARG0,ARG1>
	RET
ENDF	__lshrsi3
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

1:	;; ARG1 &= 7
	glo	ARG1
	ani	7
	plo	ARG1
	lbr	3f

2:	;; ARG0 >>=s 1
	ghi	ARG0
	shl		; preload carry with the sign of ARG0
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

/* Return <ARG0,ARG1> >>s ARG2.  */
#ifdef L__ashrsi3
BEGINF	__ashrsi3
	;; if (ARG2 & 16) <ARG0,ARG1> >>=s 16
	glo	ARG2
	ani	16
	lbz	1f

	;; ARG1 = ARG0
	glo	ARG0
	plo	ARG1
	ghi	ARG0
	phi	ARG1

	;; fill ARG0 with its sign bit
	shl		; sign bit to carry
	ldi	0xff
	adci	0	; 0x00 if negative, 0xff if non-negative
	xri	0xff	; 0xff if negative, 0x00 if non-negative
	phi	ARG0
	plo	ARG0

1:
	;; if (ARG2 & 8) <ARG0,ARG1> >>=s 8
	glo	ARG2
	ani	8
	lbz	2f

	ghi	ARG1
	plo	ARG1
	glo	ARG0
	phi	ARG1
	ghi	ARG0
	plo	ARG0

	;; fill ARG0 high byte with its sign bit
	shl		; sign bit to carry
	ldi	0xff
	adci	0	; 0x00 if negative, 0xff if non-negative
	xri	0xff	; 0xff if negative, 0x00 if non-negative
	phi	ARG0

2:	;; ARG2 &= 7
	glo	ARG2
	ani	7
	plo	ARG2
	lbr	4f

3:	;; <ARG0,ARG1> >>=s 1
	ghi	ARG0
	shl		; preload carry with the sign of ARG0
	ghi	ARG0
	shrc
	phi	ARG0
	glo	ARG0
	shrc
	plo	ARG0
	ghi	ARG1
	shrc
	phi	ARG1
	glo	ARG1
	shrc
	plo	ARG1

	;; --ARG2
	dec	ARG2
	glo	ARG2

4:	;; if (ARG2 != 0) goto 3
	lbnz	3b

	;; done, result in <ARG0,ARG1>
	RET
ENDF	__ashrsi3
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

1:	;; ARG1 &= 7
	glo	ARG1
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

/* Return <ARG0,ARG1> << ARG2.  */
#ifdef L__ashlsi3
BEGINF	__ashlsi3
	;; if (ARG2 & 16) <ARG0,ARG1> <<= 16
	glo	ARG2
	ani	16
	lbz	1f

	;; ARG0 = ARG1
	glo	ARG1
	plo	ARG0
	ghi	ARG1
	phi	ARG0

	;; clear ARG1
	ldi	0
	plo	ARG1
	phi	ARG1

1:
	;; if (ARG2 & 8) <ARG0,ARG1> <<= 8
	glo	ARG2
	ani	8
	lbz	2f

	glo	ARG0
	phi	ARG0
	ghi	ARG1
	plo	ARG0
	glo	ARG1
	phi	ARG1

	;; clear ARG1 low byte
	ldi	0
	plo	ARG1

2:	;; ARG2 &= 7
	glo	ARG2
	ani	7
	plo	ARG2
	lbr	4f

3:	;; <ARG0,ARG1> <<= 1
	glo	ARG1
	shl
	plo	ARG1
	ghi	ARG1
	shlc
	phi	ARG1
	glo	ARG0
	shlc
	plo	ARG0
	ghi	ARG0
	shlc
	phi	ARG0

	;; --ARG2
	dec	ARG2
	glo	ARG2

4:	;; if (ARG2 != 0) goto 3
	lbnz	3b

	;; done, result in <ARG0,ARG1>
	RET
ENDF	__ashlsi3
#endif

/* Signed comparisons.  There are implemented using a trick from
   "Hacker's Delight" section 2-12, which allows a signed comparison
   to be implemented by its unsigned version by inverting the signs
   of the inputs first.  */

/* Return with carry set if ARG0 >=s ARG1, clear otherwise.  */
#ifdef L__cmphi_ge
BEGINF	__cmphi_ge
	;; invert sign bit on the inputs
	ghi	ARG0
	xri	0x80
	phi	ARG0
	ghi	ARG1
	xri	0x80
	phi	ARG1
	;; defer to the unsigned operation
	lbr	__cmphi_geu
ENDF	__cmphi_ge
#endif

/* Return with carry set if ARG0 >s ARG1, clear otherwise.  */
#ifdef L__cmphi_gt
BEGINF	__cmphi_gt
	;; invert sign bit on the inputs
	ghi	ARG0
	xri	0x80
	phi	ARG0
	ghi	ARG1
	xri	0x80
	phi	ARG1
	;; defer to the unsigned operation
	lbr	__cmphi_gtu
ENDF	__cmphi_gt
#endif

/* Return with carry set if ARG0 <=s ARG1, clear otherwise.  */
#ifdef L__cmphi_le
BEGINF	__cmphi_le
	;; invert sign bit on the inputs
	ghi	ARG0
	xri	0x80
	phi	ARG0
	ghi	ARG1
	xri	0x80
	phi	ARG1
	;; defer to the unsigned operation
	lbr	__cmphi_leu
ENDF	__cmphi_le
#endif

/* Return with carry set if ARG0 <s ARG1, clear otherwise.  */
#ifdef L__cmphi_lt
BEGINF	__cmphi_lt
	;; invert sign bit on the inputs
	ghi	ARG0
	xri	0x80
	phi	ARG0
	ghi	ARG1
	xri	0x80
	phi	ARG1
	;; defer to the unsigned operation
	lbr	__cmphi_ltu
ENDF	__cmphi_lt
#endif

/* HI-mode divide and modulo, manually translated from C.  */

#ifdef L__udivmodhi4
#define NUM ARG0
#define DEN ARG1
#define MODWANTED ARG2
#define BIT ARG3
#define RES STATIC_CHAIN
BEGINF	__udivmodhi4
	;; BIT = 1, RES = 0
	ldi	1
	plo	BIT
	ldi	0
	phi	BIT
	plo	RES
	phi	RES
	lbr	1f

	;; first loop

0:	;; DEN <<= 1
	glo	DEN
	shl
	plo	DEN
	ghi	DEN
	shlc
	phi	DEN
	;; BIT <<= 1
	glo	BIT
	shl
	plo	BIT
	ghi	BIT
	shlc
	lbdf	4f	; if BIT became zero then carry: skip both loops
	phi	BIT

1:	;; if (DEN >= NUM) break
	glo	NUM	; compute DEN - NUM
	str	SP
	glo	DEN
	sm
	ghi	NUM
	str	SP
	ghi	DEN
	smb
	lbdf	2f	; if >=u then no borrow, i.e. carry set

	;; if (!(DEN & (1U<<15))) continue
	ghi	DEN
	shl
	lbnf	0b	; if high bit not set then no carry

	;; second loop, INV: BIT != 0

2:	;; compute NUM - DEN
	glo	DEN
	str	SP
	glo	NUM
	sm
	phi	MODWANTED	; use MODWANTED high as a temp
	ghi	DEN
	str	SP
	ghi	NUM
	smb
	lbnf	3f	; if <u then borrow, i.e. carry clear

	;; NUM -= DEN (reusing the compare above)
	phi	NUM
	ghi	MODWANTED
	plo	NUM
	;; RES |= BIT
	glo	BIT
	str	SP
	glo	RES
	or
	plo	RES
	ghi	BIT
	str	SP
	ghi	RES
	or
	phi	RES

3:	;; DEN >>= 1
	ghi	DEN
	shr
	phi	DEN
	glo	DEN
	shrc
	plo	DEN
	;; BIT >>= 1
	ghi	BIT
	shr
	phi	BIT
	glo	BIT
	shrc
	plo	BIT
	lbnf	2b	; if BIT went from 1 to 0 then carry

4:	;; if (!MODWANTED) NUM = RES
	glo	MODWANTED
	lbnz	5f

	;; NUM = RES
	glo	RES
	plo	NUM
	ghi	RES
	phi	NUM

5:	;; return NUM
	RET
ENDF	__udivmodhi4
#undef RES
#undef BIT
#undef MODWANTED
#undef DEN
#undef NUM
#endif

#ifdef L__udivhi3
BEGINF	__udivhi3
	;; return __udivmodhi4(ARG0, ARG1, 0)
	ldi	0
	plo	ARG2
	lbr	__udivmodhi4
ENDF	__udivhi3
#endif

#ifdef L__umodhi3
BEGINF	__umodhi3
	;; return __udivmodhi4(ARG0, ARG1, 1)
	ldi	1
	plo	ARG2
	lbr	__udivmodhi4
ENDF	__umodhi3
#endif

#ifdef L__divhi3
BEGINF	__divhi3
	;; init NEG = 0
	ldi	0
	plo	ARG2

	;; check if A < 0
	ghi	ARG0
	shl		; copy sign to carry
	lbnf	0f

	;; A = -A, NEG = !NEG
	glo	ARG0
	sdi	0
	plo	ARG0
	ghi	ARG0
	sdbi	0
	phi	ARG0
	inc	ARG2	; flip low bit

0:	;; check if B < 0
	ghi	ARG1
	shl		; copy sign to carry
	lbnf	1f

	;; B = -B, NEG = !NEG
	glo	ARG1
	sdi	0
	plo	ARG1
	ghi	ARG1
	sdbi	0
	phi	ARG1
	inc	ARG2	; flip low bit

1:	;; push NEG & 1
	glo	ARG2
	ani	1
	stxd

	;; RES = __udivmodhi4 (A, B, 0)
	ldi	0
	plo	ARG2
	sep	4
	.hword	__udivmodhi4

	;; if (NEG) RES = -RES
	inc	SP
	ldn	SP
	lbz	2f

	;; RES = -RES
	glo	ARG0
	sdi	0
	plo	ARG0
	ghi	ARG0
	sdbi	0
	phi	ARG0

2:	;; return RES
	RET
ENDF	__divhi3
#endif

#ifdef L__modhi3
BEGINF	__modhi3
	;; check if A < 0
	ghi	ARG0
	shl		; copy sign to carry
	ldi	0	; NEG = 0
	lbnf	0f

	;; A = -A, NEG = 1
	glo	ARG0
	sdi	0
	plo	ARG0
	ghi	ARG0
	sdbi	0
	phi	ARG0
	ldi	1	; NEG = 1

0:	;; push NEG
	stxd

	;; check if B < 0
	ghi	ARG1
	shl		; copy sign to carry
	lbnf	1f

	;; B = -B
	glo	ARG1
	sdi	0
	plo	ARG1
	ghi	ARG1
	sdbi	0
	phi	ARG1

1:	;; RES = __udivmodhi4 (A, B, 1)
	ldi	1
	plo	ARG2
	sep	4
	.hword	__udivmodhi4

	;; if (NEG) RES = -RES
	inc	SP
	ldn	SP
	lbz	2f

	;; RES = -RES
	glo	ARG0
	sdi	0
	plo	ARG0
	ghi	ARG0
	sdbi	0
	phi	ARG0

2:	;; return RES
	RET
ENDF	__modhi3
#endif

#ifdef L__mulhi3
#define A ARG0
#define B ARG1
#define RESULT ARG2
BEGINF	__mulhi3
	;; if (A == 0) return 0
	glo	A
	str	SP
	ghi	A
	or
	lbz	3f

	;; RESULT = 0
	ldi	0
	plo	RESULT
	phi	RESULT

	lbr	2f

0:	;; if (B & 1) ...
	glo	B
	shr		; (B & 1) to carry
	lbnf	1f

	;; RESULT += A
	glo	A
	str	SP
	glo	RESULT
	add
	plo	RESULT
	ghi	A
	str	SP
	ghi	RESULT
	adc
	phi	RESULT

1:	;; A <<= 1
	glo	A
	shl
	plo	A
	ghi	A
	shlc
	phi	A

	;; B >>= 1
	ghi	B
	shr
	phi	B
	glo	B
	shrc
	plo	B

2:	;; if (B != 0) goto 0
	glo	B
	str	SP
	ghi	B
	or
	lbnz	0b

	;; return RESULT
	glo	RESULT
	plo	ARG0
	ghi	RESULT
	phi	ARG0

3:	RET
ENDF	__mulhi3
#undef RESULT
#undef B
#undef A
#endif

#ifdef L__ucmpsi2
#define A_HIGH ARG0
#define A_LOW  ARG1
#define B_HIGH ARG2
#define B_LOW  ARG3
#define TMP    STATIC_CHAIN
BEGINF	__ucmpsi2
	;; if (A_HIGH == B_HIGH) ...
	glo	A_HIGH
	str	SP
	glo	B_HIGH
	xor
	plo	TMP
	ghi	A_HIGH
	str	SP
	ghi	B_HIGH
	xor
	str	SP
	glo	TMP
	or		; zero if equal
	lbnz	2f	; jump to 2f if not equal

	;; if (A_LOW == B_LOW) ...
	glo	A_LOW
	str	SP
	glo	B_LOW
	xor
	plo	TMP
	ghi	A_LOW
	str	SP
	ghi	B_LOW
	xor
	str	SP
	glo	TMP
	or		; zero if equal
	lbnz	1f	; jump to 1f if not equal
	ldi	1
	lbr	4f	; return 1

1:	;; return (A_LOW < B_LOW) ? 0 : 2
	glo	B_LOW
	str	SP
	glo	A_LOW
	sm
	ghi	B_LOW
	str	SP
	ghi	A_LOW
	lbr	3f

2:	;; return (A_HIGH < B_HIGH) ? 0 : 2
	glo	B_HIGH
	str	SP
	glo	A_HIGH
	sm
	ghi	B_HIGH
	str	SP
	ghi	A_HIGH
3:	smb
	;; if <u then borrow, i.e. carry clear
	ldi	0
	lsnf		; skip next ldi if <u
	ldi	2

4:	;; set ARG0 = <0,D> then return
	plo	ARG0
	ldi	0
	phi	ARG0
	RET
ENDF	__ucmpsi2
#undef TMP
#undef B_LOW
#undef B_HIGH
#undef A_LOW
#undef A_HIGH
#endif

#ifdef L__popcounthi2
BEGINF	__popcounthi2
	;; init sum in ARG1 low
	ldi	0
	plo	ARG1
	lbr	1f

0:	;; [D = ARG0 >> 8] ARG0 >>= 8
	plo	ARG0
	ldi	0
	phi	ARG0

1:	;; compute popcount for ARG0 low into D
	;; this clobbers ARG0 low, D, DF, and MEM[SP]

	;; x = x - ((x >> 1) & 0x55)
	glo	ARG0
	str	SP
	shr
	ani	0x55
	sd
	plo	ARG0

	;; x = (x & 0x33) + ((x >> 2) & 0x33)
	ani	0x33
	str	SP
	glo	ARG0
	shr
	shr
	ani	0x33
	add

	;; x = (x + (x >> 4)) & 0x0F
	str	SP
	shr
	shr
	shr
	shr
	add
	ani	0x0F

	;; add to sum
	str	SP
	glo	ARG1
	add
	plo	ARG1

	;; if ((ARG0 >> 8) != 0) goto 0b
	ghi	ARG0
	lbnz	0b

	;; return sum
	glo	ARG1
	plo	ARG0
	RET
ENDF	__popcounthi2
#endif

/* Our __clzhi2(0) returns 16.  */
#ifdef L__clzhi2
#define N	ARG1
BEGINF	__clzhi2
	;; n = 16
	ldi	16
	plo	N

	;; if ((y = x >> 8) != 0) { x = y; n -= 8; }
	ghi	ARG0
	lbz	1f
	plo	ARG0
	ldi	0
	phi	ARG0	; ensure high byte of ret val is zero
	ldi	8
	plo	N

1:	;; if ((y = x >> 4) != 0) { x = y; n -= 4; }
	glo	ARG0
	shr
	shr
	shr
	shr
	lbz	2f
	plo	ARG0
	glo	N
	smi	4
	plo	N

2:	;; if ((y = x >> 2) != 0) { x = y; n -= 2; }
	glo	ARG0
	shr
	shr
	lbz	3f
	plo	ARG0
	dec	N
	dec	N

3:	;; if ((x >> 1) != 0) { x = 2; }
	glo	ARG0
	shr
	lbz	4f
	ldi	2
	plo	ARG0

4:	;; return n - x
	glo	N
	str	SP
	glo	ARG0
	sd
	plo	ARG0
	RET
ENDF	__clzhi2
#undef N
#undef X
#endif

/* Stubs for NYI functions load a cookie into D and then execute
   a special simulator instruction to output a NYI error with the
   PC and that cookie, and then halt.  Libc uses cookies 1 up, so
   we use 255 down.  */
.macro	NYI	nr
	ldi	\nr
	.byte	0x68, 0x1f
.endm

#ifdef L__udivsi3
BEGINF	__udivsi3
	NYI	0xff
ENDF	__udivsi3
#endif

#ifdef L__umodsi3
BEGINF	__umodsi3
	NYI	0xfe
ENDF	__umodsi3
#endif

#ifdef L__addvhi3
BEGINF	__addvhi3
	NYI	0xfd
ENDF	__addvhi3
#endif

#ifdef L__clrsbhi2
BEGINF	__clrsbhi2
	NYI	0xfc
ENDF	__clrsbhi2
#endif

#ifdef L__ctzhi2
BEGINF	__ctzhi2
	NYI	0xfa
ENDF	__ctzhi2
#endif

#ifdef L__divsi3
BEGINF	__divsi3
	NYI	0xf8
ENDF	__divsi3
#endif

#ifdef L__ffshi2
BEGINF	__ffshi2
	NYI	0xf7
ENDF	__ffshi2
#endif

#ifdef L__modsi3
BEGINF	__modsi3
	NYI	0xf6
ENDF	__modsi3
#endif

#ifdef L__mulsi3
BEGINF	__mulsi3
	NYI	0xf5
ENDF	__mulsi3
#endif

#ifdef L__mulvhi3
BEGINF	__mulvhi3
	NYI	0xf4
ENDF	__mulvhi3
#endif

#ifdef L__negvhi2
BEGINF	__negvhi2
	NYI	0xf3
ENDF	__negvhi2
#endif

#ifdef L__parityhi2
BEGINF	__parityhi2
	NYI	0xf2
ENDF	__parityhi2
#endif

#ifdef L__subvhi3
BEGINF	__subvhi3
	NYI	0xf0
ENDF	__subvhi3
#endif
