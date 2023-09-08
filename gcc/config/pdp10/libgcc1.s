/* This file is filtered through the C preprocessor and is
   assembled separately for each function within it. */

	.DIRECTIVE      KL10

.IFN FL,SYMBOL,<FL=:0>		; Scratch, USED by compiler
.IFN T1,SYMBOL,<T1=:1>		; Argument register 1
.IFN T2,SYMBOL,<T2=:2>		; Argument register 2
.IFN T3,SYMBOL,<T3=:3>		; Argument register 3
.IFN T4,SYMBOL,<T4=:4>		; Argument register 4
.IFN Q1,SYMBOL,<Q1=:5>		; Function scratch register 1
.IFN Q2,SYMBOL,<Q2=:6>		; Function scratch register 2
.IFN Q3,SYMBOL,<Q3=:7>		; Function scratch register 3
.IFN P1,SYMBOL,<P1=:10>	; Preserved register 1
.IFN P2,SYMBOL,<P2=:11>	; Preserved register 2
.IFN P3,SYMBOL,<P3=:12>	; Preserved register 3
.IFN P4,SYMBOL,<P4=:13>	; Preserved register 4
.IFN SC,SYMBOL,<SC=:14>	; Static Chain Pointer
.IFN FP,SYMBOL,<FP=:15>	; Frame Pointer
.IFN CX,SYMBOL,<CX=:16>	; RESERVED for PIC code
.IFN SP,SYMBOL,<SP=:17>	; Stack Pointer

.IFN LAW,MACRO,		<DEFINE LAW(Y)		<Y>>				; Local Address Word
.IFN LIW,MACRO,		<DEFINE LIW(I,X,Y)	<2B1!<I>B13!<X>B17!LAW(Y)>>	; Local Indirect Word
.IFN GAW,MACRO,		<DEFINE GAW(Y)		<<Y>*ONE>>			; Global Address Word
.IFN GIW,MACRO,		<DEFINE GIW(I,X,Y)	<<I>B1!<X>B5!GAW(Y)>>		; Global Indirect Word
.IFN OWGBP,MACRO,	<DEFINE OWGBP(PS,Y)	<<PS>B5!GAW(Y)>>		; One Word Global Byte Pointer
.IFN TWGBP,MACRO,	<DEFINE TWGBP(P,S,Y)	<EXP <P>B5!<S>B11!1B12,<Y>>>	; Two Word Global Byte Pointer
.IFN ALIGN,MACRO,	<DEFINE ALIGN(N)	<RELOC <.+<1_N-1>>&^-<1_N-1>>>	; Align PC to 2**N word boundary
.IFN BSS,MACRO,		<DEFINE BSS(A)		<				; Zero initialized memory
		SALL
		REPEAT A,<EXP 0>
		XALL>>
.IFN FRM.PC,SYMBOL,<FRM.PC=:	^D-10>	; procedure return address
.IFN FRM.ET1,SYMBOL,<FRM.ET1=:	^D-9>	; __FILE__
.IFN FRM.ET2,SYMBOL,<FRM.ET2=:	^D-8>	; __LINE__
.IFN FRM.EP1,SYMBOL,<FRM.EP1=:	^D-7>	; exception handler's preserved register set
.IFN FRM.EP2,SYMBOL,<FRM.EP2=:	^D-6>	; .
.IFN FRM.EP3,SYMBOL,<FRM.EP3=:	^D-5>	; .
.IFN FRM.EP4,SYMBOL,<FRM.EP4=:	^D-4>	; .
.IFN FRM.ESC,SYMBOL,<FRM.ESC=:	^D-3>	; .
.IFN FRM.EFP,SYMBOL,<FRM.EFP=:	^D-2>	; .
.IFN FRM.ESP,SYMBOL,<FRM.ESP=:	^D-1>	; exception handler's SP
.IFN FRM.EPC,SYMBOL,<FRM.EPC=:	^D-0>	; address of exception handler
.IFN FRM.SZ,SYMBOL,<FRM.SZ=:	FRM.EPC - FRM.PC + 1>

#ifdef __DUALSTACK__
.IFN CALL,MACRO,<DEFINE CALL(A)		<jrst	[push	SP,T1			; Save register
						xmovei	T1,A			; get the extended call address
						setzm	FRM.SZ(CX)		; PROBE control stack
						.IFN .call,SYMBOL,<EXTERN .call>; declare extern if needed
						pushj	CX,@[GAW(.call)]	; call the procedure
						jrst	.+1			; return inline
						]>>
.IFN CALLRET,MACRO,<DEFINE CALLRET(A)		<jrst	[exch	T1,(SP)		; Save register
						xmovei	T1,A			; get the extended call address
								.IFN .cret,SYMBOL,<EXTERN .cret>; declare extern if needed
						jrst	@[GAW(.cret)]		; recycle slot and call the procedure
						]>>
.IFN RET,MACRO,<DEFINE RET		<.IFN .ret,SYMBOL,<EXTERN .ret>		; declare extern if needed
						jrst	@[GAW(.ret)]		; remove slot and return
						>>
#else
.IFN CALL,MACRO,	<DEFINE CALL(A)         <pushj SP,A>>           ; Call a procedure
.IFN CALLRET,MACRO,	<DEFINE CALLRET(A)      <jrst A>>               ; Call and Return from a procedure
.IFN RET,MACRO,		<DEFINE RET             <popj SP,>>             ; Return from procedure
#endif

.IFN EPILOG,MACRO,      <DEFINE EPILOG          <>>			; end of procedure hook

	
/* The sequences for udivsi3 and umodsi3 were taken from KCC and are
   derived from one suggested by Peter Samson at Systems Concepts.  */

	.PSECT text/ronly/concatenate

#ifdef L_udivsi3
	TITLE udivsi3
	ENTRY %..udivsi3
%..udivsi3:

#if __REGPARM__ == 0
	MOVE T1,-1(SP)
#endif

#if __REGPARM__ >= 2
	SKIPGE T3,T2
#else
	SKIPGE T3,__REGPARM__-2(SP)
#endif
	 JRST .L1
	JUMPGE T1,.L3
	CAIG T3,1
	 JRST .L2
	MOVE T2,T1
	MOVEI T1,1
	DIV T1,T3
	RET
.L1:	MOVE T2,T1
	MOVEI T1,0
	JUMPGE T2,.L4
	CAMGE T2,T3
	 RET
	SUB T2,T3
	AOJA T1,.L4
.L2:	TDZA T2,T2
.L3:	IDIV T1,T3
.L4:	RET

#endif


#ifdef L_umodsi3
	TITLE umodsi3
	ENTRY %..umodsi3
%..umodsi3:

#if __REGPARM__ == 0
	MOVE T1,-1(SP)
#endif

#if __REGPARM__ >= 2
	SKIPGE T3,T2
#else
	SKIPGE T3,__REGPARM__-2(SP)
#endif
	 JRST .L1
	JUMPGE T1,.L3
	CAIG T3,1
	 JRST .L2
	MOVE T2,T1
	MOVEI T1,1
	DIV T1,T3
	JRST .L4
.L1:	MOVE T2,T1
	MOVEI T1,0
	JUMPGE T2,.L4
	CAMGE T2,T3
	 JRST .L4
	SUB T2,T3
	AOJA T1,.L4
.L2:	TDZA T2,T2
.L3:	IDIV T1,T3
.L4:	MOVE T1,T2
	RET
#endif


/* This code is based loosely on the udivsi3 code above     */
/* Divide the unsigned long long value in T1,T2 by the       */
/* unsigned long long value in T3,T4 and return the result */
/* in T1,T2									      */

#ifdef L_udivdi3
	TITLE udivdi3
	ENTRY %..udivdi3
%..udivdi3:

	/* Shuffle the parameters so dividend is in T1,T2,T3,T4 */
	/* and divisor is in Q1,Q2                                                 */
	/* Note that normally __REGPARM__ >= 7                     */
#if __REGPARM__ <= 1
	DMOVE Q1, -4(SP)
	DMOVE T3, -2(SP)
	SETZB T1,T2
#else
#if __REGPARM__ <= 3
	DMOVE Q1, -2(SP)
	DMOVE T3,T1
	SETZB T1,T2
#else
	DMOVE Q1,T3
	DMOVE T3,T1
	SETZB T1,T2
#endif
#endif

	/* Begin by testing for a big divisor */
	JUMPL Q1, .L1

	/* We have a reasonable size divisor */
	JUMPGE T3,.L3

	/* We have a big dividend, but a reasonable size divisor */
	CAIG Q2,1
	JUMPE Q1, .L2

	/* We have a big dividend, but a reasonable size divisor known to be at least 2 */
	MOVEI T2,1
	JRST .L3

	/* We have a big divisor */	
.L1:	JUMPGE T3,.L4

	/* We have a big dividend and a big divisor, the result is either zero or one */	
	CAMGE T3,Q1
	RET
	CAME T3,Q1
	AOJA T2,.L4
	CAMGE T4,Q2
	RET
	AOJA T2,.L4

	/* divisor is zero or one, so the result is the dividend */
.L2:	DMOVE T1,T3
	RET

	/* we have reasonable size dividend and divisor */
.L3:	DDIV T1,Q1

	/* if we've deduced the result and placed it in T1,T2 we can jump here */
.L4:	RET

#endif


/* This code is based loosely on the udivsi3 code above            */
/* Divide the unsigned long long value in T1,T2 by the              */
/* unsigned long long value in T3,T4 and return the remainder */
/* in T1,T2									             */

#ifdef L_umoddi3
	TITLE umoddi3
	ENTRY %..umoddi3
%..umoddi3:

	/* Shuffle the parameters so dividend is in T1,T2,T3,T4 */
	/* and divisor is in Q1,Q2                                                 */
	/* Note that normally __REGPARM__ >= 7                     */
#if __REGPARM__ <= 1
	DMOVE Q1, -4(SP)
	DMOVE T3, -2(SP)
	SETZB T1,T2
#else
#if __REGPARM__ <= 3
	DMOVE Q1, -2(SP)
	DMOVE T3,T1
	SETZB T1,T2
#else
	DMOVE Q1,T3
	DMOVE T3,T1
	SETZB T1,T2
#endif
#endif

	/* Begin by testing for a big divisor */
	JUMPL Q1, .L1

	/* We have a reasonable size divisor */
	JUMPGE T3,.L3

	/* We have a big dividend, but a reasonable size divisor */
	CAIG Q2,1
	JUMPE Q1, .L5

	/* We have a big dividend, but a reasonable size divisor known to be at least 2 */
	MOVEI T2,1
	JRST .L3

	/* We have a big divisor */	
.L1:	JUMPGE T3,.L4

	/* We have a big dividend and a big divisor, the quotient is either zero or one */
	/* and the remainder is either the dividend or the dividend minus the divisor    */
	CAMGE T3,Q1
	JRST .L4
	CAME T3,Q1
	JRST .L2
	CAMGE T4,Q2
	JRST .L4

	/* We have a big dividend and a big divisor and the dividend is bigger than the divisor */
	/* so the remainder is the dividend minus the divisor */
.L2:	DSUB T3,Q1
	JRST .L4

	/* we have reasonable size dividend and divisor */
.L3:	DDIV T1,Q1

	/* if we've deduced the result and the remainder is T3,T4 we can jump here */
.L4:	DMOVE T1,T3

	/* if we've deduced the result and the remainder is in T1,T2 we can jump here */
.L5:	RET

#endif




#ifdef L_umulsi3
	TITLE umulsi3
	ENTRY %..umulsi3
%..umulsi3:

#if __REGPARM__ == 0
	MOVE T1,-1(SP)
	MOVE T2,-2(SP)
#endif

#if __REGPARM__ == 1
	MOVE T2,-1(SP)
#endif

/*	We don't care about overflow and want the result to be the
	low order 36 bits of the result regardless, so just do the multiplcation
	and squeeze out the redundant sign bit
*/
	MUL T1,T2
	LSH T2,1
	LSHC T1,-1
	EXCH T1,T2
	RET
#endif

/* TODO: many unimplemented functions...
   These are not presently even assembled in order to ensure
   that link failures will result if they are referenced */

#ifdef L_adddf3
	TITLE adddf3
	ENTRY %..adddf3
%..adddf3:

	HALT
#endif

#ifdef L_subdf3
	TITLE subdf3
	ENTRY %..subdf3
%..subdf3:

	HALT
#endif

#ifdef L_negdf3
	TITLE negdf3
	ENTRY %..negdf3
%..negdf3:

	HALT
#endif

#ifdef L_muldf3
	TITLE muldf3
	ENTRY %..muldf3
%..muldf3:

	HALT
#endif

#ifdef L_divdf3
	TITLE divdf3
	ENTRY %..divdf3
%..divdf3:

	HALT
#endif

#ifdef L_eqdf2
	TITLE eqdf2
	ENTRY %..eqdf2
%..eqdf2:

	HALT
#endif

#ifdef L_nedf2
	TITLE nedf2
	ENTRY %..nedf2
%..nedf2:

	HALT
#endif

#ifdef L_gtdf2
	TITLE gtdf2
	ENTRY %..gtdf2
%..gtdf2:

	HALT
#endif

#ifdef L_gedf2
	TITLE gedf2
	ENTRY %..gedf2
%..gedf2:

	HALT
#endif

#ifdef L_ltdf2
	TITLE ltdf2
	ENTRY %..ltdf2
%..ltdf2:

	HALT
#endif

#ifdef L_ledf2
	TITLE ledf2
	ENTRY %..ledf2
%..ledf2:

	HALT
#endif

#ifdef L_fixsfsi
	TITLE fixsfsi
	ENTRY %..fixsfsi
%..fixsfsi:
#if __REGPARM__ == 0
	MOVE T1,-1(SP)
#endif
	SETZ	T4,			;T4<-sign, T1<-abs(T1) 
	JUMPGE	T1, .+3
	MOVNS	T1
	AOS		T4
	MULI	T1, 400		;separate exponent from fraction
	MOVE	T3, T2
	SETZ	T2,			;fraction is now in T2,T3, sign bit of T3 is clear
	ASHC	T2, -243(T1)	;shift per exponent with adjustment to put binary point at right of T3
	LSH		T3, 1		;squeeze the T3 sign bit out
	LSHC	T2, -1		;T3 is now result without sign adjustment
	MOVE	T1, T3
	SKIPE	T4
	MOVN	T1, T1
	RET
#endif

#ifdef L_fixdfsi
	TITLE fixdfsi
	ENTRY %..fixdfsi
%..fixdfsi:
#if __REGPARM__ <= 1
	MOVE T1,-1(SP)		;parameters should never be split between registers and stack
	MOVE T2,-2(SP)
#endif
	SETZ	T4,			;T4<-sign, T1<-abs(T1) 
	JUMPGE	T1, .+3
	DMOVN	T1, T1
	AOS		T4
	MOVE	T3, T2
	MULI	T1, 400		;separate exponent from fraction
	ASH		T2, -10		;shift high order fraction back, so fraction is now in T2,T3, sign bits clear
	ASHC	T2, -276(T1)	;shift per exponent with adjustment to put binary point at right of T3
	LSH		T3, 1		;squeeze the T3 sign bit out
	LSHC	T2, -1		;T3 is now result without sign adjustment
	MOVE	T1, T3
	SKIPE	T4
	MOVN	T1, T1
	RET
#endif

#ifdef L_fixsfdi
	TITLE fixsfdi
	ENTRY %..fixsfdi
%..fixsfdi:
#if __REGPARM__ == 0
	MOVE T1,-1(SP)
#endif
	SETZ	T4,			;T4<-sign, T1<-abs(T1) 
	JUMPGE	T1, .+3
	MOVNS	T1
	AOS		T4
	MULI	T1, 400		;separate exponent from fraction
	MOVE	T3, T2
	SETZ	T2,			;fraction is now in T2,T3, sign bit of T3 is clear
	CAIG	T1, 306		;biggest exponent where a simple shift can't overflow
	JUMPA	.+6
	ASHC	T2, -244(T1)	;shift by one less than ultimate shift amount, this is correct only when total shift is positive
	LSH		T2, 1		;copy bit we're about to lose into the sign/high magnitude bit
	LSH		T2, -1
	ASHC	T2, 1		;shift the last bit
	SKIPA
	ASHC	T2, -243(T1)	;shift per exponent with adjustment to put binary point at right of T3
	MOVE	T1, T2
	MOVE	T2, T3		;T1,T2 is now result without sign adjustment
	SKIPE	T4
	DMOVN	T1, T1
	RET
#endif

#ifdef L_fixdfdi
	TITLE fixdfdi
	ENTRY %..fixdfdi
%..fixdfdi:
#if __REGPARM__ <= 1
	MOVE T1,-1(SP)		;parameters should never be split between registers and stack
	MOVE T2,-2(SP)
#endif
	SETZ	T4,			;T4<-sign, T1<-abs(T1) 
	JUMPGE	T1, .+3
	DMOVN	T1, T1
	AOS		T4
	MOVE	T3, T2
	MULI	T1, 400		;separate exponent from fraction
	ASH		T2, -10		;shift high order fraction back, so fraction is now in T2,T3, sign bits clear
	CAIG	T1, 306		;biggest exponent where a simple shift can't overflow
	JUMPA	.+6
	ASHC	T2, -277(T1)	;shift by one less than ultimate shift amount, this is correct only when total shift is positive
	LSH		T2, 1		;copy bit we're about to lose into the sign/high magnitude bit
	LSH		T2, -1
	ASHC	T2, 1		;shift the last bit
	SKIPA
	ASHC	T2, -276(T1)	;shift per exponent with adjustment to put binary point at right of T3
	MOVE	T1, T2
	MOVE	T2, T3		;T1,T2 is now result without sign adjustment
	SKIPE	T4
	DMOVN	T1, T1
	RET
#endif

#ifdef L_floatsisf
	TITLE floatsisf
	ENTRY %..floatsisf
%..floatsisf:

	HALT
#endif

#ifdef L_floatsidf
	TITLE floatsidf
	ENTRY %..floatsidf
%..floatsidf:

	HALT
#endif

	.ENDPS

/* The following tables are used to calculate byte pointer differences.
   They are taken, with small changes, from KCC.  */

/* these symbols are declared as ENTRY rather than INTERN to make
   them searchable in libgcc.a 
   INTERN symbols are not indexed for search */
	
	.PSECT rdat/ronly/concatenate

#ifdef LBADL6
	TITLE BADL6
	ENTRY BADL6
BADL6:	/* TODO.  */
#endif

	#ifdef LBADL7
	TITLE BADL7
	ENTRY BADL7
BADL7:	/* TODO.  */
#endif

	#ifdef LBADL8
	TITLE BADL8
	ENTRY BADL8
BADL8:	/* TODO.  */
#endif

	#ifdef LBADL9
	TITLE BADL9
	ENTRY BADL9
BADL9:	/* TODO.  */
#endif

#ifdef LBADX6
	TITLE BADX6
	ENTRY BADX6

	777777777773
	017777777773
	0
	777777777774
	017777777774
	0
	777777777775
	017777777775
	0
	777777777776
	017777777776
	0
	777777777777
	017777777777
	0
	0
BADX6:
	000000000000
	0
	760000000001
	000000000001
	0
	760000000002
	000000000002
	0
	760000000003
	000000000003
	0
	760000000004
	000000000004
	0
	760000000005
	000000000005
#endif

#ifdef LBADX7
	TITLE BADX7
	ENTRY BADX7

	777777777774
	007777777774
	0
	0
	0
	777777777775
	007777777775
	0
	0
	0
	777777777776
	007777777776
	0
	0
	0
	777777777777
	007777777777
	0
	0
	0
	0
BADX7:
	000000000000
	0
	0
	0
	770000000001
	000000000001
	0
	0
	0
	770000000002
	000000000002
	0
	0
	0
	770000000003
	000000000003
	0
	0
	0
	770000000004
	000000000004
#endif

#if defined (LBADX8) || defined (LBADX9)
#if defined (LBADX8)
	TITLE BADX8
	ENTRY BADX8
#endif
#if defined (LBADX9)
	TITLE BADX9
	ENTRY BADX9
#endif
	777777777775
	017777777775
	777777777776
	017777777776
	777777777777
	017777777777
	0

#if defined (LBADX8)
BADX8:
#endif
#if defined (LBADX9)
BADX9:
#endif
	000000000000
	760000000001
	000000000001
	760000000002
	000000000002
	760000000003
	000000000003
#endif

#ifdef LBADXH
	TITLE BADXH
	ENTRY BADXH

	777777777777
	007777777777
	0
BADXH:	0
	770000000001
	000000000001
#endif

	.ENDPS

	EXTERN	ONE
	
	END
