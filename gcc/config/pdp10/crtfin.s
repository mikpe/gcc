        TITLE   CRTFIN
	SEARCH	MONSYM
	
	;; 
	;; C runtime initialization
	;;
	;; This is a VERY rough cut, just a starting point for testing
	;; 
	;; This module MUST be loaded LAST so it can establish variables
	;; referencing the actual sizes of various .PSECTs
	;;
	;; 
	;; TODO:	FIX %abort, %exit for compilations with mregparm=0
	;; TODO:	Deal with nonextended programs too
	;; TODO:	Setup stack dynamically
	;; 

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
		
	;;
	;; text .PSECT
	;; 
	
	.PSECT	text/ronly/concatenate/paligned

	;;
	;; Tops20 entry vector
	;;
	
crtev:	jrst	.crt0		; start address
	jrst	%abort		; restart address (abort immediately if restarted)
	BYTE	(3)0,(9)0,(6)0,(18)0 ; who did it#, version #, patch #, edit #

	;;
	;; initialize stack and call main()
	;;
	
.crt0:	RESET%			; initialize process state
#ifdef __DUALSTACK__
	move	CX,[GAW(ctrl.b)]; setup the control stack
#endif
	move	SP,[GAW(stak.b)]; setup the application stack
	move	T1,0		; argc, zero for now
	move	T2,0		; **argv, NULL for now
	CALL	%main		; CALL the "C" main program
	jrst	%exit		; force %exit upon return

	ENTRY	%abort		; abort() is a user symbol
%abort:	movei	P1,0		; set 0 as abort status
	move	P2,[OWGBP(61,ab.msg)]; get abort message 
	jrst	both		; join common code

	ENTRY	%exit		; exit() is a user symbol
%exit:	skipn	P1,T1		; save exit status, check if 0
	 jrst	done		; 0, just halt program silently
	move	P2,[OWGBP(61,ex.msg)]; get exit message

both:	move	T1,[OWGBP(61,crlf)] ; print cr/lf
	PSOUT%			;
	move	T1,P2		; print exit/abort message
	PSOUT%			; print message
	movei	T1,.PRIOU	; set primary output descriptor
	skipn	T2,P1		; supress numeric printout if status is 0
	 jrst	wcrlf		; 0, just write final cr/lf
	movei	T3,^D10		; decimal radix, no flags
	NOUT%			; print status number
	 ERJMP	.+1		; ignore any error
wcrlf:	move	T1,[OWGBP(61,crlf)]	; print cr/lf
	PSOUT%			;

done:	HALTF%			; program halt
	jrst	%abort		; abort if continued from program halt

	ENTRY	%..main		; Null out collect2's constructor entry point for now
%..main:RET			; It's not being generated properly

	EPILOG


#ifdef __DUALSTACK__	
	;;
	;; Compiler support routines
	;;
	;; call/ret support, invoked by CALL, CALLRET and RET macros
	;;

	;;
	;; .call:  allocate a control slot and call a procedure
	;;         invoked by CALL macro
	;;
	;; called with:  (SP) saved T1
	;;                T1  address to call
	;;               (CX) return address (first word of NEW control slot)
	;;
	;;
	;; .cret:  modify a control slot and call a procedure
	;;         invoked by CALLRET macro
	;;
	;; called with:  (SP) saved T1
	;;                T1  address to call
	;;               (CX) current control slot
	;;
	;; NOTE WELL:  CX is explicitly probed BEFORE we start building a new frame
	;;             We will NOT get here if there is no room for a new frame
	;;             Instead, we will have faulted with CX pointing to the current frame
	;;             This behavior is essential to ensure that the fault handler can
	;;             always issue a _throw() with a known good frame pointer, even
	;;             if the cause of the fault is stack overflow
	;; 
	ENTRY	.call
	ENTRY	.calltext
	ENTRY	.cret
	ENTRY	.crettext
.call:
.calltext:
	xmovei	CX,FRM.SZ-1(CX)		; reserve remainder of control slot
					; TODO: is it safe to assume FRM.EPC(CX) is still zeroed from probe?
					; it *should* be; interrupts use another stack, and
					; there should be no process traps between the probe and here...
					; If so, we can avoid the following setzm
.cret:
.crettext:
	setzm	FRM.ET1(CX)		; clear traceback data
	setzm	FRM.EPC(CX)		; clear exception PC
	popj		SP,					; jump to the called routine

	;;
	;; .ret:  deallocate a control slot and return from procedure
	;;        invoked by RET macro
	;;
	;; called with: (CX) current control slot
	;;              (SP) address of procedure we just returned from
	;;
	ENTRY	.ret
	ENTRY	.rettext
.ret:
.rettext:
	xmovei	CX,-<FRM.SZ-1>(CX)	; remove control slot, except for return PC
	popj	CX,			; return to caller

	EPILOG

#endif

	;;
	
	INTERN text.e
text.e:				; End of all program text
	.ENDPS	

	;;
	;; rdat .PSECT
	;;

	.PSECT	rdat/ronly/concatenate/paligned

#ifdef __DUALSTACK__
	INTERN	ctrl.p
ctrl.p:	EXP	<ctrl.e-ctrl.b>_-^D9	; current control stack size, in pages
#endif

	INTERN	stak.p
stak.p:	EXP	<stak.e-stak.b>_-^D9	; current application stack size, in pages

crlf:	ASCIZ	/
/

ex.msg:	ASCIZ	/?CRT - program exit, status is /
ab.msg:	ASCIZ	/?CRT - program abort/

	INTERN	rdat.e
rdat.e:				; End of all initialized readonly static data
	.ENDPS
	
	;;
	;; dat .PSECT
	;;

	.PSECT	dat/rwrite/concatenate/paligned
	
	INTERN	dat.e
dat.e:				; End of all initialized read/write static data
	.ENDPS
	
	;;
	;; bss .PSECT
	;;
	;; NOTE:	TOPS20 does neither creates nor maps this section unless 
	;; 		the data it contains is initialized.  Since TDboot
	;;		is most likely equally stupid, bss data is initialized
	;;		at compile time using the BSS macro, which forces zeros.
	;;
	;;		There are likely better ways to allocate zero pages.
	;;		The virtue of this scheme is that it will always work.
	;;

	.PSECT	bss/rwrite/concatenate/paligned
	
	INTERN	bss.e
bss.e:				; End of all unitialized read/write static data
	.ENDPS
	
	;;
	;; symbol table
	;; 
	
	.PSECT	symb/ronly/concatenate/paligned
	
	INTERN	symb.e
symb.e:				; End of the symbol table
	.ENDPS

	;; 
	;; stack.
	;; 
	;; NOTE:	This should really be done dynamically.
	;;		For the moment, though, it is also initialized to
	;;		ensure creation.
	;;

	.PSECT	stak/overlaid/rwrite/paligned

	SALL			; supress endless output
stak.g:	REPEAT	01000,<EXP -1>	; guard page, should unmap this page permanently
stak.b:	REPEAT	016000,<EXP 0>	; beginning of stack, 14 (decimal) pages total
stak.e:	REPEAT	01000,<EXP -1>	; end page, should unmap this page permanently
	XALL			; resume normal listing

	.ENDPS

#ifdef __DUALSTACK__
	.PSECT	ctrl/overlaid/rwrite/paligned

	SALL			; supress endless output
ctrl.g:	REPEAT	01000,<EXP -1>	; guard page, should unmap this page permanently
ctrl.b:	REPEAT	016000,<EXP 0>	; beginning of stack, 14 (decimal) pages total
ctrl.e:	REPEAT	01000,<EXP -1>	; end page, should unmap this page permanently
	XALL			; resume normal listing
#endif

	EXTERN	one,%main
	END	crtev
