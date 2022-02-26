/* Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007
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

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively. If not, see
<http://www.gnu.org/licenses/>. */ 

/* As a special exception, if you link this library with other files,
   some of which are compiled with GCC, to produce an executable,
   this library does not by itself cause the resulting executable
   to be covered by the GNU General Public License.
   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.  */

!! libgcc1 routines for META cpu.
!! Contributed by Metagence Technologies (toolkit@metagence.com)

!! Division routines

#ifdef L_udivsi3
!!
!! 32-bit division unsigned i/p - passed unsigned 32-bit numbers
!!
        .text
        .global ___udivsi3
        .type   ___udivsi3,function
        .align  2
___udivsi3:
!!
!! Since core is signed divide case, just set control variable
!!
        MOV     D1Re0,D0Ar2             ! Au already in A1Ar1, Bu -> D1Re0
        MOV     D0Re0,#0                ! Result is 0
        MOV     D0Ar4,#0                ! Return positive result
        B       __IDMCUStart
        .size   ___udivsi3,.-___udivsi3
#endif

#ifdef L_divsi3
!!
!! 32-bit division signed i/p - passed signed 32-bit numbers
!!
        .text
        .global ___divsi3
        .type   ___divsi3,function
        .align  2
___divsi3:
!!
!! A already in D1Ar1, B already in D0Ar2 -> make B abs(B)
!!
        MOV     D1Re0,D0Ar2             ! A already in A1Ar1, B -> D1Re0
        MOV     D0Re0,#0                ! Result is 0
        XOR     D0Ar4,D1Ar1,D1Re0       ! D0Ar4 -ive if result is -ive
        ABS     D1Ar1,D1Ar1             ! abs(A) -> Au
        ABS     D1Re0,D1Re0             ! abs(B) -> Bu
        .global __IDMCUStart
        .hidden __IDMCUStart
__IDMCUStart:
        CMP     D1Ar1,D1Re0             ! Is ( Au > Bu )?
        LSR     D1Ar3,D1Ar1,#2          ! Calculate (Au & (~3)) >> 2
        CMPHI   D1Re0,D1Ar3             ! OR ( (Au & (~3)) <= (Bu << 2) )?
        LSLSHI  D1Ar3,D1Re0,#1          ! Buq = Bu << 1
        BLS     $IDMCUSetup             ! Yes: Do normal divide
!!
!! Quick divide setup can assume that CurBit only needs to start at 2
!!
$IDMCQuick:
        CMP     D1Ar1,D1Ar3             ! ( A >= Buq )?
        ADDCC   D0Re0,D0Re0,#2          ! If yes result += 2
        SUBCC   D1Ar1,D1Ar1,D1Ar3       !        and A -= Buq
        CMP     D1Ar1,D1Re0             ! ( A >= Bu )?
        ADDCC   D0Re0,D0Re0,#1          ! If yes result += 1
        SUBCC   D1Ar1,D1Ar1,D1Re0       !        and A -= Bu
        ORS     D0Ar4,D0Ar4,D0Ar4       ! Return neg result?
        NEG     D0Ar2,D0Re0             ! Calulate neg result
        MOVMI   D0Re0,D0Ar2             ! Yes: Take neg result
$IDMCRet:
        MOV     PC,D1RtP
!!
!!  Setup for general unsigned divide code
!!
!!      D0Re0 is used to form the result, already set to Zero
!!      D1Re0 is the input Bu value, this gets trashed
!!      D0Ar6 is curbit which is set to 1 at the start and shifted up
!!      D0Ar4 is negative if we should return a negative result
!!      D1Ar1 is the input Au value, eventually this holds the remainder
!!
$IDMCUSetup:
        CMP     D1Ar1,D1Re0             ! Is ( Au < Bu )?
        MOV     D0Ar6,#1                ! Set curbit to 1
        BCS     $IDMCRet                ! Yes: Return 0 remainder Au
!!
!! Calculate alignment using FFB instruction
!!
        FFB     D1Ar5,D1Ar1             ! Find first bit of Au
        ANDN    D1Ar5,D1Ar5,#31         ! Handle exceptional case.
        ORN     D1Ar5,D1Ar5,#31         ! if N bit set, set to 31
        FFB     D1Ar3,D1Re0             ! Find first bit of Bu
        ANDN    D1Ar3,D1Ar3,#31         ! Handle exceptional case.
        ORN     D1Ar3,D1Ar3,#31         ! if N bit set, set to 31
        SUBS    D1Ar3,D1Ar5,D1Ar3       ! calculate diff, ffbA - ffbB
        MOV     D0Ar2,D1Ar3             ! copy into bank 0
        LSLGT   D1Re0,D1Re0,D1Ar3       ! ( > 0) ? left shift B
        LSLGT   D0Ar6,D0Ar6,D0Ar2       ! ( > 0) ? left shift curbit
!!
!! Now we start the divide proper, logic is 
!!
!!       if ( A >= B ) add curbit to result and subtract B from A
!!       shift curbit and B down by 1 in either case
!!
$IDMCLoop:
        CMP     D1Ar1, D1Re0            ! ( A >= B )?
        ADDCC   D0Re0, D0Re0, D0Ar6     ! If yes result += curbit
        SUBCC   D1Ar1, D1Ar1, D1Re0     ! and A -= B
        LSRS    D0Ar6, D0Ar6, #1        ! Shift down curbit, is it zero?
        LSR     D1Re0, D1Re0, #1        ! Shift down B
        BNZ     $IDMCLoop               ! Was single bit in curbit lost?
        ORS     D0Ar4,D0Ar4,D0Ar4       ! Return neg result?
        NEG     D0Ar2,D0Re0             ! Calulate neg result
        MOVMI   D0Re0,D0Ar2             ! Yes: Take neg result
        MOV     PC,D1RtP
        .size   ___divsi3,.-___divsi3
#endif

!! Modulus routines

#ifdef L_umodsi3
!!
!! 32-bit modulus unsigned i/p - passed unsigned 32-bit numbers
!!
        .text
        .global ___umodsi3
        .type   ___umodsi3,function
        .align  2
___umodsi3:
#ifdef __PIC__
        SETL    [A0StP++],D0FrT,D1RtP   ! Save return address
        CALLR   D1RtP,___udivsi3@PLT
        GETL    D0FrT,D1RtP,[--A0StP]   ! Recover return address
#else
        MOV     D0FrT,DRtP              ! Save original return address
        CALLR   D1RtP,___udivsi3
        MOV     D1RtP,D0FrT             ! Recover return address
#endif
        MOV     D0Re0,D1Ar1             ! Return remainder
        MOV     PC,D1RtP
        .size   ___umodsi3,.-___umodsi3
#endif

#ifdef L_modsi3
!!
!! 32-bit modulus signed i/p - passed signed 32-bit numbers
!!
        .text
        .global ___modsi3
        .type   ___modsi3,function
        .align  2
___modsi3:
#ifdef  __PIC__
        MOV     D0.4,D1Ar1
        SETL    [A0StP++],D0.4,D1RtP    ! Save A and return address
        CALLR   D1RtP,___divsi3@PLT
        GETL    D0.4,D1RtP,[--A0StP]    ! Recover A and return address
        MOV     D1Re0,D0.4
#else
        MOV     D0FrT,D1RtP             ! Save original return address
        MOV     A0.2,D1Ar1              ! Save A in A0.2
        CALLR   D1RtP,___divsi3
        MOV     D1RtP,D0FrT             ! Recover return address
        MOV     D1Re0,A0.2              ! Recover A
#endif
        MOV     D0Re0,D1Ar1             ! Return remainder
        ORS     D1Re0,D1Re0,D1Re0       ! Was A negative?
        NEG     D1Ar1,D1Ar1             ! Negate remainder
        MOVMI   D0Re0,D1Ar1             ! Return neg remainder
        MOV     PC, D1RtP
        .size   ___modsi3,.-___modsi3
#endif

!! Floating point support routines

#ifdef  L_adddf3
!!
!! Floating point - double add
!!
        .text
        .global ___adddf3
        .type   ___adddf3,function
        .align  2
___adddf3:
        AND     D1Re0, D1Ar1, D1Ar3
        ANDT    D1Re0, D1Re0, #0x8000           ! sign1 & sign2

        LSL     D0Re0, D1Ar1, #1                ! Ignore sign
        ORS     D0Re0, D0Re0, D0Ar2             ! Zero?

        LSL     D0Re0, D1Ar3, #1                ! Ignore sign
        ORSZ    D0Re0, D0Re0, D0Ar4             ! Zero

        MOVZ    PC, D1RtP                       ! both zero return +/-Zero

        LSL     D0Re0, D1Ar1, #1                ! Ignore sign
        ORS     D0Re0, D0Re0, D0Ar2             ! Zero?

        MOVZ    D0Re0, D0Ar4
        MOVZ    D1Re0, D1Ar3
        MOVZ    PC, D1RtP                       ! Arg1 zero return Arg2

        LSL     D0Re0, D1Ar3, #1                ! Ignore sign
        ORS     D0Re0, D0Re0, D0Ar4             ! Zero?

        MOVZ    D1Re0, D1Ar1
        MOVZ    D0Re0, D0Ar2
        MOVZ    PC, D1RtP                       ! Arg2 zero return Arg1

        MOV     D1Ar5, D1Ar1
        ANDMT   D1Ar5, D1Ar5, #0x7FFF
        LSR     D1Ar5, D1Ar5, #20               ! exp1

        MOV     D0Ar6, D1Ar3
        ANDMT   D0Ar6, D0Ar6, #0x7FFF
        LSR     D0Ar6, D0Ar6, #20               ! exp2

        MOV     D0Re0, D1Ar5
        SUBS    D1Re0, D0Re0, D0Ar6             ! exp = exp1 - exp2 ---> D1Re0
        BGE     $L2

        SWAP    D0Re0, D1Ar3                    ! mant1 <-> mant2
        SWAP    D0Re0, D1Ar1
        SWAP    D0Re0, D1Ar3

        SWAP    D1Re0, D0Ar4
        SWAP    D1Re0, D0Ar2
        SWAP    D1Re0, D0Ar4

        SWAP    D0Ar6, D1Ar5                    ! exp1 <-> exp2
        NEG     D1Re0, D1Re0                    ! exp = -exp

$L2:
        CMP     D1Re0, #54
        BLE     $L3

        MOV     D1Re0, D1Ar1
        MOV     D0Re0, D0Ar2

        MOV     PC, D1RtP

$L3:
        SWAP    D1Re0, D0Re0

        ADDS    D1Ar1, D1Ar1, #0
        ANDMT   D1Ar1, D1Ar1, #0x000F
        ORT     D1Ar1, D1Ar1, #0x0010
        LSL     D1Ar1, D1Ar1, #9

        LSR     D1Re0, D0Ar2, #23
        OR      D1Ar1, D1Ar1, D1Re0
        LSL     D0Ar2, D0Ar2, #9                ! man1 <<= 9 ---> D1Ar1:D0Ar2

        BGE     $L4

        NEGS    D0Ar2, D0Ar2
        NEG     D1Ar1, D1Ar1
        SUBNZ   D1Ar1, D1Ar1, #1                ! man1 D1Ar1:D0Ar2

$L4:
        ADDS    D1Ar3, D1Ar3, #0
        ANDMT   D1Ar3, D1Ar3, #0x000F
        ORT     D1Ar3, D1Ar3, #0x0010
        LSL     D1Ar3, D1Ar3, #9

        LSR     D1Re0, D0Ar4, #23
        OR      D1Ar3, D1Ar3, D1Re0
        LSL     D0Ar4, D0Ar4, #9                ! man2 <<= 9 --->D1Ar3, D0Ar4

        BGE     $L5

        NEGS    D0Ar4, D0Ar4
        NEG     D1Ar3, D1Ar3
        SUBNZ   D1Ar3, D1Ar3, #1                ! man2 D1Ar3:D0Ar4

$L5:
        SWAP    D1Re0, D0Re0
        CMP     D1Re0, #32                      ! ought to consider 32 <= exp1 - exp2 <64
        BGE     $L6

        CMP     D1Re0, #0                       ! Zero is a special case
        BZ      $L7 

        MOV     D0Re0, D1Re0
        NEG     D0Re0, D0Re0
        ADD     D0Re0, D0Re0, #32               ! 32 + (- (exp1 - exp2))
                                                ! man2 D1Ar3:D0Ar4
        MOV     D0Ar6, D1Re0                    ! man2 >> exp1 - exp2
        LSR     D0Ar4, D0Ar4, D0Ar6
        MOV     D0Ar6, D1Ar3
        LSL     D0Ar6, D0Ar6, D0Re0
        OR      D0Ar4, D0Ar4, D0Ar6
        ASR     D1Ar3, D1Ar3, D1Re0
        B       $L7

$L6:                                            ! exp >= 32
        SUB     D1Re0, D1Re0, #32
        ASRS    D0Ar4, D1Ar3, D1Re0             ! man2 >>= exp
        MOV     D1Ar3, #-1
        ADDGE   D1Ar3, D1Ar3, #1

$L7:                                            ! man (D1Re0:D1Re0)
        ADDS    D0Re0, D0Ar2, D0Ar4             ! man = man1 + man2
        ADD     D1Re0, D1Ar1, D1Ar3
        ADDCS   D1Re0, D1Re0, #1

        MOV     D0Ar6, #0                       ! assume sign +ve

        CMP     D1Re0, #0                       ! man < 0 ?
        BGT     $L9
        BLT     $L8

        CMP     D0Re0, #0

        MOVZ    PC, D1RtP                       ! man == 0 return 0

$L8:
        CMP     D1Re0, #0                       ! man < 0
        BZ      $L9                             ! treat D1Re0 0 as positive 

        MOVT    D0Ar6, #0x8000                  ! sign -ve

                                                ! man D1Re0:D0Re0
        NEGS    D0Re0, D0Re0                    ! man = -man
        NEG     D1Re0, D1Re0
        SUBNZ   D1Re0, D1Re0, #1

$L9:                                            ! man +ve
        NORM    D1Ar1, D1Re0
        BZ      $L10                            ! MSword zero

        CMP     D1Ar1, #0                       ! Already normalised ?
        BZ      $L11                            ! yes, skip normlisation

        MOV     D0Ar2, D1Re0                    ! Shifting < 32 bits
        FFB     D0Ar2, D0Ar2

        LSL     D1Re0, D1Re0, D1Ar1             ! MSWord
        ADD     D0Ar2, D0Ar2, #2
        LSR     D1Ar3, D0Re0, D0Ar2
        OR      D1Re0, D1Re0, D1Ar3

        MOV     D0Ar2, D1Ar1
        LSL     D0Re0, D0Re0, D0Ar2             ! LSWord 

        SUB     D1Ar5, D1Ar5, D1Ar1             ! exp -= NORM (man)
        B       $L11

$L10:                                           ! Shifting >= 32 bits
        MOVS    D1Re0, D0Re0                    ! Shift by 32 bits.
        SUBGT   D0Re0, D0Re0, D0Re0

        LSRLT   D1Re0, D1Re0, #1                ! If the results has MSBit set the
        LSLLT   D0Re0, D0Re0, #31               ! we must only shift by 31 bits so
                                                ! we dont use NORM on a neg value.

        SUBLT   D1Ar5, D1Ar5, #31               ! adjust exponent
        SUBGE   D1Ar5, D1Ar5, #32

        NORM    D1Ar1, D1Re0


        NEG     D0Ar4, D1Ar1                    ! (32 - D1Ar1)
        ADD     D0Ar4, D0Ar4, #32

        LSR     D0Ar4, D0Re0, D0Ar4
        MOV     D0Ar2, D1Ar1
        LSL     D0Re0, D0Re0, D0Ar2
        OR      D0Re0, D0Re0, D0Ar4
        LSL     D1Re0, D1Re0, D1Ar1

        SUB     D1Ar5, D1Ar5, D1Ar1             ! exp -= NORM (man)

$L11:
        ADD     D1Ar5, D1Ar5, #(10-9)

        TST     D0Re0, #(1<<10)
        BZ      $L12

        ADDS    D0Re0, D0Re0, #1                ! man += 1
        ADDCS   D1Re0, D1Re0, #1

$L12:
        ADDS    D0Re0, D0Re0, #((1<<9)-1)
        ADDCS   D1Re0, D1Re0, #1

        TSTT    D1Re0, #0x8000                  ! rounding overflowed?
        BZ      $L14

                                                ! adjust man and exp
        LSR     D0Re0, D0Re0, #1                ! man >>= 1
        LSL     D0Ar2, D1Re0, #(32-1)
        LSR     D1Re0, D1Re0, #1
        OR      D0Re0, D0Re0, D0Ar2

        ADD     D1Ar5, D1Ar5, #1                ! exp += 1

$L14:
        CMP     D1Ar5, #0                       ! exp <= 0 ?
        SUBLE   D0Re0, D0Re0, D0Re0             ! underflow
        MOVLE   D1Re0, D1Ar3
        MOVLE   PC, D1RtP                       ! return +/-Zero

        LSR     D0Re0, D0Re0, #10
        LSL     D0Ar4, D1Re0, #(32-10)
        OR      D0Re0, D0Re0, D0Ar4
        LSR     D1Re0, D1Re0, #10               ! man >>= 10

        ANDMT   D1Re0, D1Re0, #0x000F
        LSL     D1Ar5, D1Ar5, #(52 - 32)        ! position exp
        MOV     D1Ar3, D0Ar6
        OR      D1Ar5, D1Ar5, D1Ar3             ! sign|exp
        OR      D1Re0, D1Re0, D1Ar5             ! man|exp|sign -> D1Re0, D0Re0

        MOV     PC, D1RtP
        .size   ___adddf3,.-___adddf3
#endif

#ifdef L_addsf3
!!
!! Floating point - float add
!!
        .text
        .global ___addsf3
        .type   ___addsf3,function
        .align  2
___addsf3:
        MOV     D1Re0, D0Ar2
        AND     D1Re0, D1Re0, D1Ar1
        ANDT    D1Re0, D1Re0, #0x8000           ! sign = sign1 & sign2

        LSLS    D0Re0, D1Ar1, #1                ! Ignore sign bit
        LSL     D0Re0, D0Ar2, #1
        ORSZ    D0Re0, D0Re0, D0Re0

        MOVZ    D0Re0, D1Re0                    ! sign = sign1 & sign2
        MOVZ    PC, D1RtP                       ! both zero return +/-Zero

        LSLS    D0Re0, D1Ar1, #1                ! Ignore sign bit
        MOVZ    D0Re0, D0Ar2
        MOVZ    PC, D1RtP                       ! Arg1 zero return Arg2

        LSLS    D0Re0, D0Ar2, #1                ! Ignore sign bit
        MOVZ    D0Re0, D1Ar1
        MOVZ    PC, D1RtP                       ! Arg2 zero return Arg1

        MOV     D1Ar5, D1Ar1
        ANDMT   D1Ar5, D1Ar5, #0x7FFF
        LSR     D1Ar5, D1Ar5, #23               ! exp1

        MOV     D1Ar3, D0Ar2
        ANDMT   D1Ar3, D1Ar3, #0x7FFF
        LSR     D1Ar3, D1Ar3, #23               ! exp2

        SUB     D0Re0, D1Ar5, D1Ar3             ! exp1 - exp2
        CMP     D0Re0, #25                      ! >25

        MOVGT   D0Re0, D1Ar1
        MOVGT   PC, D1RtP

        CMP     D0Re0, #-25                     ! <-25

        MOVLT   D0Re0, D0Ar2
        MOVLT   PC, D1RtP

        MOV     D0Ar6, D1Ar1
        ANDMT   D0Ar6, D0Ar6, #0x007F
        ORT     D0Ar6, D0Ar6, #0x0080           ! man1
        LSL     D0Ar6, D0Ar6, #6                ! man1 <<= 6

        MOV     D0Ar4, D0Ar2
        ANDMT   D0Ar4, D0Ar4, #0x007F
        ORT     D0Ar4, D0Ar4, #0x0080           ! man2
        LSL     D0Ar4, D0Ar4, #6                ! man2 <<= 6

        CMP     D1Ar1, #0
        BGE     $L4

        NEG     D0Ar6, D0Ar6                      ! man1 = -man1

$L4:
        CMP     D0Ar2, #0
        BGE     $L5

        NEG     D0Ar4, D0Ar4                    ! man2 = -man2

$L5:
        CMP     D0Re0, #0                       ! D0Re0 = exp1 - exp2
        BGE     $L6

        NEG     D0Re0, D0Re0
        ASR     D0Ar6, D0Ar6, D0Re0             ! man1 >> (exp2 - exp1)
        MOV     D1Ar5, D1Ar3                    ! D1Ar5 = exp2
        B       $L7

$L6:
        ASR     D0Ar4, D0Ar4, D0Re0             ! man2 >> (exp1 - exp2)

$L7:
        ADDS    D0Re0, D0Ar6, D0Ar4             ! man = man1 + man2
        MOV     D1Re0, #0                       ! --> sign
        MOVZ    PC, D1RtP

        BGT     $L8
        MOVT    D1Re0, #0x8000                  ! --->sign
        NEG     D0Re0, D0Re0

$L8:
        MOV     D0Ar4, D0Re0                    ! man
        ANDT    D0Ar4, D0Ar4, #0xE000
        CMP     D0Ar4, #0
        BNE     $L9

        LSL     D0Re0, D0Re0, #1
        SUB     D1Ar5, D1Ar5, #1
        B       $L8

$L9:
        MOV     D0Ar4, D0Re0                    ! D0Ar4 = man
        ANDT    D0Re0, D0Re0, #0x4000
        CMP     D0Re0, #0
        BZ      $L11

        LSR     D0Ar4, D0Ar4, #1
        ADD     D1Ar5, D1Ar5, #1

$L11:
        MOV     D0Ar6, D0Ar4
        AND     D0Ar6, D0Ar6, #0x40
        CMP     D0Ar6, #0
        ADDNZ   D0Ar4, D0Ar4, #1
        ADD     D0Ar4, D0Ar4, #0x1F
        MOV     D0Ar6, D0Ar4
        ANDT    D0Ar6, D0Ar6, #0x4000
        CMP     D0Ar6, #0
        BZ      $L13

        ASR     D0Ar4, D0Ar4, #1
        ADD     D1Ar5, D1Ar5, #1

$L13:
        CMP     D1Ar5, #0                       ! exp <= 0 ?
        MOVLE   D0Re0, D1Re0                    ! underflow
        MOVLE   PC, D1RtP                       ! return +/-Zero

        LSR     D0Ar4, D0Ar4, #6                ! man >>= 6
        MOV     D0Ar6, D1Ar5
        LSL     D0Ar6, D0Ar6, #23

        MOV     D0Re0, D1Re0
        OR      D0Re0, D0Re0, D0Ar6
        ANDMT   D0Ar4, D0Ar4, #0x007F
        OR      D0Re0, D0Re0, D0Ar4

        MOV     PC, D1RtP
        .size   ___addsf3,.-___addsf3
#endif

#ifdef  L_subdf3
!!
!! Floating point - double sub
!!
        .text
        .global ___subdf3
        .type   ___subdf3,function
        .align  2
___subdf3:
        MOV     D1Re0, D0Ar4
        ORS     D0Re0, D1Ar3, D1Re0
        MOVZ    D1Re0, D1Ar1
        MOVZ    D0Re0, D0Ar2
        MOVZ    PC, D1RtP
        XORT    D1Ar3, D1Ar3, #0x8000
#ifdef __PIC__
        B       ___adddf3@PLT
#else
        B       ___adddf3
#endif
        .size   ___subdf3,.-___subdf3
#endif

#ifdef  L_addsf3
!!
!! Floating point - float sub
!!
        .text
        .global ___subsf3
        .type   ___subsf3,function
        .align  2
___subsf3:
        CMP     D0Ar2, #0
        MOVZ    D0Re0, D1Ar1
        MOVZ    PC, D1RtP
        XORT    D0Ar2, D0Ar2, #0x8000
#ifdef __PIC__
        B       ___addsf3@PLT
#else
        B       ___addsf3
#endif
        .size   ___subsf3,.-___subsf3
#endif

#ifdef L_negdf2
!!
!! Floating point - double negate
!!
        .text
        .global ___negdf2
        .type   ___negdf2,function
        .align  2
___negdf2:
        MOV     D0Re0, D0Ar2
        MOV     D1Re0, D1Ar1
        XORT    D1Re0, D1Re0, #0x8000
        MOV     PC, D1RtP
        .size   ___negdf2,.-___negdf2
#endif

#ifdef L_negsf2
!!
!! Floating point - double negate
!!
        .text
        .global ___negsf2
        .type   ___negsf2,function
        .align  2
___negsf2:
        MOV     D0Re0, D1Ar1
        XORT    D0Re0, D0Re0, #0x8000
        MOV     PC, D1RtP
        .size   ___negsf2,.-___negsf2
#endif

#ifdef L_cmpdf2
!!
!! Compare two double(s)
!! return -1 if <
!!         0 if ==
!!        +1 if >
        .text
        .global ___cmpdf2
        .type   ___cmpdf2,function
        .align  2
___cmpdf2:
        LSR     A0.2, D1Ar1, #31                ! sign1
        LSR     A0.3, D1Ar3, #31                ! sign2

        LSR     D1Ar5, D1Ar1, #(52-32)          ! exp1
        AND     D1Ar5, D1Ar5, #0x07FF

        LSR     D0Ar6, D1Ar3, #(52-32)          ! exp2
        AND     D0Ar6, D0Ar6, #0x07FF

        ANDMT   D1Ar1, D1Ar1, #0x000F           ! mant1
        ANDMT   D1Ar3, D1Ar3, #0x000F           ! mant1

!!
!!  if (D_NAN_P (dp1) || D_NAN_P (dp2))
!!    return 1;

        MOV     D0Re0, #1

        CMP     D1Ar5, #0x07FF                  ! exp1 == 0x07FF
        BNE     $L1

        ORS     D1Re0, D1Ar1, D1Ar1             ! HI(mant1) == 0?
        ORSZ    D1Re0, D0Ar2, D0Ar2             ! and LO(mant1) == 0?
        MOVNZ   PC, D1RtP                       ! no, Nan, return 1

        B       $L2

$L1:

        CMP     D0Ar6, #0x07FF                  ! exp2 == 0x07FF
        BNE     $L2

        ORS     D1Re0, D1Ar3, D1Ar3             ! HI(mant2) == 0 ?
        ORSZ    D1Re0, D0Ar4, D0Ar4             ! and LO(mant2) == 0 ?
        MOVNZ   PC, D1RtP                       ! no, Nan, return 1

$L2:
!!
!!  if (D_INF_P (dp1) && D_INF_P (dp2))
!!    return sign2 - sign1;
!!

        SUB     D0Re0, A0.3, A0.2

        MOV     D1Re0, D0Ar6
        AND     D1Re0, D1Re0, D1Ar5
        CMP     D1Re0, #0x7FF                   ! (exp1 & exp2) == 0x7FF

        CMPEQ   D1Ar1,D1Ar3
        CMPEQ   D0Ar2,D0Ar4                     ! mant1 == mant2
        CMPEQ   D1Ar1, #0
        CMPEQ   D0Ar2, #0                       ! == 0
        MOVEQ   PC, D1RtP

!!  if (D_INF_P (dp1))
!!    return sign1 ? -1 : 1;

        MOV     D0Re0, #0                       ! result
        MOV     D1Re0, A0.2
        TST     D1Re0, #1                       ! sign1 ?
        SUBNZ   D0Re0, D0Re0, #1                ! -1
        ADDZ    D0Re0, D0Re0, #1                ! +1

        CMP     D1Ar5, #0x7FF                   ! exp1 == 0x7FF
        MOVEQ   D1Re0, D0Ar2
        ORSEQ   D1Re0, D1Re0, D1Ar1             ! mant1 == 0
        MOVEQ   PC, D1RtP

!!
!!  if (D_INF_P (dp2))
!!    return sign2 ? 1 : -1;

        MOV     D0Re0, #0                       ! result
        MOV     D1Re0, A0.3
        TST     D1Re0, #1                       ! sign2 ?
        ADDNZ   D0Re0, D0Re0, #1                ! +1
        SUBZ    D0Re0, D0Re0, #1                ! -1

        CMP     D0Ar6, #0x7FF                   ! exp2 == 0x7FF
        MOVEQ   D1Re0, D0Ar4
        ORSEQ   D1Re0, D1Re0, D1Ar3             ! mant2 == 0
        MOVEQ   PC, D1RtP

!!
!!  if (D_ZERO_P (dp1) && D_ZERO_P (dp2))
!!    return 0;

        MOV     D0Re0, #0
        MOV     D1Re0, D0Ar6
        ORS     D1Re0, D1Re0, D1Ar5             ! exp1 | exp1
        ORSZ    D1Re0, D1Ar1, D1Ar3             ! mant1 | mant2
        ORSZ    D1Re0, D0Ar2, D0Ar4
        MOVZ    PC, D1RtP                       ! both zero return 0
!!
!!  if (D_ZERO_P (dp1))
!!    return sign2 ? 1 : -1;
!!   

                                                ! result (D0Re0) already 0
        MOV     D1Re0, A0.3
        TST     D1Re0, #1                       ! sign2?
        ADDNZ   D0Re0, D0Re0, #1                ! +1
        SUBZ    D0Re0, D0Re0, #1                ! -1

        ORS     D1Re0, D1Ar5, D1Ar5             ! exp1 == 0
        ORSZ    D1Re0, D1Ar1, D1Ar1             ! and mant1 == 0
        ORSZ    D1Re0, D0Ar2, D0Ar2
        MOVZ    PC, D1RtP

!!  if (D_ZERO_P (dp2))
!!    return sign1 ? -1 : 1;

        MOV     D0Re0, #0                       ! result
        MOV     D1Re0, A0.2
        TST     D1Re0, #1                       ! sign1?
        SUBNZ   D0Re0, D0Re0, #1                ! -1
        ADDZ    D0Re0, D0Re0, #1                ! +1

        ORS     D1Re0, D0Ar6, D0Ar6             ! exp2 == 0
        ORSZ    D1Re0, D1Ar3, D1Ar3             ! and mant2 == 0
        ORSZ    D1Re0, D0Ar4, D0Ar4
        MOVZ    PC, D1RtP

!!  /* Normalize the numbers. */
!!  D_NORMALIZE (dp1, exp1, mant1);
!!  D_NORMALIZE (dp2, exp2, mant2);

!! now both are "normal".

        MOV     D0Re0, #0                       ! result
        MOV     D1Re0,A0.2
        TST     D1Re0, #1                       ! sign1
        SUBNZ   D0Re0, D0Re0, #1                ! -1
        ADDZ    D0Re0, D0Re0, #1                ! +1

        SUB     D1Re0, A0.2, A0.3
        CMP     D1Re0, #0                       ! sign1 != sign2
        MOVNE   PC, D1RtP                       ! yes, return sign1 ? -1 : +1

        MOV     D1Re0, D0Ar6
        CMP     D1Ar5, D1Re0                    ! exp1 > exp2
        MOVGT   PC, D1RtP                       ! yes, return sign1 ? -1 : +1

        NEG     D0Re0, D0Re0                    ! result -= result
        MOVLT   PC, D1RtP                       ! exp < exp2 return -(sign1 ? -1 : +1)

        CMP     D1Ar1, D1Ar3                    ! HI(mant1) < HI(mant2)
        CMPEQ   D0Ar2, D0Ar4                    ! LO(mant1) < LO(mant2)
        MOVLO   PC, D1RtP                       ! yes, return -(sign1 ? -1 : +1)

        NEG     D0Re0, D0Re0                    ! result = -result
        MOVHI   PC, D1RtP                       ! > return sign1 ? -1 : +1

        MOV     D0Re0, #0
        MOV     PC, D1RtP
        .size   ___cmpdf2,.-___cmpdf2
#endif

#ifdef L_cmpdf2_nan
!!
!! Filters for Nan arguments before calling ___cmpdf2
!!
!! If either Arg1 or Arg2 is Nan then return Arg3 (D1Ar5)
!! otherwise tail calls  ___cmpdf2
!!
        .text
        .global ___cmpdf2_nan
        .type   ___cmpdf2_nan,function
        .align  2
___cmpdf2_nan:
        LSR     D1Re0, D1Ar1, #(52-32)          ! arg1 NAN ?
        AND     D1Re0, D1Re0, #0x07FF
        CMP     D1Re0, #0x07FF                  ! exp all 1s
        BNE     $L10

        MOV     D0Ar6, D1Ar1
        LSL     D0Ar6, D0Ar6, #(64-52)          ! mantisa non-zero?
        ORS     D0Ar6, D0Ar6, D0Ar2
        MOVNZ   D0Re0, D1Ar5
        MOVNZ   PC, D1RtP                       ! return (D1Ar3)

$L10:
        LSR     D1Re0, D1Ar3, #(52-32)          ! arg2 NAN ?
        AND     D1Re0, D1Re0, #0x07FF
        CMP     D1Re0, #0x07FF                  ! exp all 1s
        BNE     $L11

        MOV     D0Ar6, D1Ar3
        LSL     D0Ar6, D0Ar6, #(64-52)          ! mantisa non-zero?
        ORS     D0Ar6, D0Ar6, D0Ar4
        MOVNZ   D0Re0, D1Ar5
        MOVNZ   PC, D1RtP                       ! return (D1Ar3)

$L11:
#ifdef __PIC__
        B       ___cmpdf2@PLT
#else
        B       ___cmpdf2
#endif
        .size   ___cmpdf2_nan,.-___cmpdf2_nan
#endif

#ifdef L_cmpsf2
!!
!! Compare two float(s)
!! return -1 if <
!!         0 if ==
!!        +1 if >
!!
        .text
        .global ___cmpsf2
        .type   ___cmpsf2,function
        .align  2
___cmpsf2:
        LSR     D1Ar3, D1Ar1, #31               ! sign1
        LSR     D1Ar5, D0Ar2, #31               ! sign2

        LSR     D0Ar4, D1Ar1, #23               ! exp1
        AND     D0Ar4, D0Ar4, #0x00FF

        LSR     D0Ar6, D0Ar2, #23               ! exp2
        AND     D0Ar6, D0Ar6, #0x00FF

        ANDMT   D1Ar1, D1Ar1, #0x007F           ! mant1
        ANDMT   D0Ar2, D0Ar2, #0x007F           ! mant2

!!  if (F_NAN_P (fp1) || F_NAN_P (fp2))
!!    return 1;

        MOV     D0Re0, #1

        CMP     D0Ar4, #0xFF                    ! exp1 == 0xFF?
        BNE     $L1

        CMP     D1Ar1, #0                       ! and mant1 == 0?
        MOVNZ   PC, D1RtP                       ! No, Nan, return 1

        B       $L2

$L1:
        CMP     D0Ar6, #0xFF                    ! exp2 == 0xFF?
        BNE     $L2

        CMP     D0Ar2, #0                       ! mant2 == 0?
        MOVNZ   PC, D1RtP                       ! No, Nan, return 1

$L2:

!!  if (F_INF_P (fp1) && F_INF_P (fp2))
!!    return sign2 - sign1;

        SUB     D0Re0, D1Ar5, D1Ar3             ! sign2 - sign1

        AND     D1Re0, D0Ar4, D0Ar6
        CMP     D1Re0, #0xFF                    ! (exp1 & exp2) == 0xFF
        MOV     D1Re0, D0Ar2
        CMPEQ   D1Ar1, D1Re0                    ! mant1 == mant2
        CMPEQ   D1Ar1, #0                       ! == 0
        MOVEQ   PC, D1RtP

!!  if (F_INF_P (fp1))
!!    return sign1 ? -1 : 1;

        MOV     D0Re0, #0                       ! result
        TST     D1Ar3, #1                       ! sign1 ?
        SUBNZ   D0Re0, D0Re0, #1                ! -1
        ADDZ    D0Re0, D0Re0, #1                ! +1

        CMP     D0Ar4, #0x00FF                  ! exp1 == 0xFF
        CMPEQ   D1Ar1, #0                       ! mant1 == 0
        MOVEQ   PC, D1RtP

!!  if (F_INF_P (fp2))
!!    return sign2 ? 1 : -1;

        MOV     D0Re0, #0                       ! result
        TST     D1Ar5, #1                       ! sign2 ?
        ADDNZ   D0Re0, D0Re0, #1                ! +1
        SUBZ    D0Re0, D0Re0, #1                ! -1

        CMP     D0Ar6, #0x00FF                  ! exp2 == 0xFF
        CMPEQ   D0Ar2, #0                       ! mant2 == 0
        MOVEQ   PC, D1RtP

!!  if (F_ZERO_P (fp1) && F_ZERO_P (fp2))
!!    return 0;

        MOV     D0Re0, #0
        ORS     D1Re0, D0Ar4, D0Ar6             ! exp1 | exp2 
        MOV     D1Re0, D0Ar2
        ORSZ    D1Re0, D1Ar1, D1Re0             ! mant1 | mant1
        MOVZ    PC, D1RtP                       ! both zero return 0

!!  if (F_ZERO_P (fp1))
!!    return sign2 ? 1 : -1;

                                                ! result (D0Re0) already 0
        TST     D1Ar5, #1                       ! sign2 ?
        ADDNZ   D0Re0, D0Re0, #1                ! +1
        SUBZ    D0Re0, D0Re0, #1                ! -1

        ORS     D1Re0, D0Ar4, D0Ar4             ! exp1 == 0
        ORSZ    D1Re0, D1Ar1, D1Ar1             ! and mant1 == 0?
        MOVZ    PC, D1RtP

!!  if (F_ZERO_P (fp2))
!!    return sign1 ? -1 : 1;
  
        MOV     D0Re0, #0                       ! result
        TST     D1Ar3, #1                       ! sign1 ?
        SUBNZ   D0Re0, D0Re0, #1                ! -1
        ADDZ    D0Re0, D0Re0, #1                ! +1

        ORS     D1Re0, D0Ar6, D0Ar2             ! exp2 and mant2 == 0?
        MOVZ    PC, D1RtP

!!  /* Normalize the numbers. */
!!  F_NORMALIZE (fp1, exp1, mant1);
!!  F_NORMALIZE (fp2, exp2, mant2);

!! now both are "normal".

        MOV     D0Re0, #0                       ! result
        TST     D1Ar3, #1                       ! sign1 ?
        SUBNZ   D0Re0, D0Re0, #1                ! -1
        ADDZ    D0Re0, D0Re0, #1                ! +1

        CMP     D1Ar3, D1Ar5                    ! sign1 != sign2
        MOVNE   PC, D1RtP                       ! yes, return sign1 ? -1 : +1

        CMP     D0Ar4, D0Ar6                    ! exp1 > exp2
        MOVGT   PC, D1RtP                       ! yes, return sign1 ? -1 : +1

        NEG     D0Re0, D0Re0                    ! result -= result
        MOVLT   PC, D1RtP                       ! exp < exp2 return -(sign1 ? -1 : +1)

        MOV     D1Re0, D0Ar2
        CMP     D1Ar1, D1Re0                    ! mant1 < mant2
        MOVLT   PC, D1RtP                       ! yes, return -(sign1 ? -1 : +1)

        NEG     D0Re0, D0Re0                    ! result = -result
        MOVGT   PC, D1RtP                       ! > return sign1 ? -1 : +1

        MOV     D0Re0, #0
        MOV     PC, D1RtP
        .size   ___cmpsf2,.-___cmpsf2
#endif

#ifdef L_cmpsf2_nan
!!
!! Filters for Nan arguments before calling ___cmpsf2
!!
!! If either Arg1 or Arg2 is Nan then return Arg3 (D1Ar3)
!! otherwise tail calls  ___cmpsf2
!!
        .text
        .global ___cmpsf2_nan
        .type   ___cmpsf2_nan,function
        .align  2
___cmpsf2_nan:
        LSR     D1Re0, D1Ar1, #23               ! arg1 NAN ?
        AND     D1Re0, D1Re0, #0x00FF
        CMP     D1Re0, #0x00FF                  ! exp all 1s
        BNE     $L10

        MOV     D1Re0, D1Ar1
        LSLS    D1Re0, D1Re0, #(32-23)          ! mantisa non-zero?
        MOVNZ   D0Re0, D1Ar3
        MOVNZ   PC, D1RtP                       ! return (D1Ar3)

$L10:
        LSR     D1Re0, D0Ar2, #23               ! arg2 NAN ?
        AND     D1Re0, D1Re0, #0x00FF
        CMP     D1Re0, #0x00FF                  ! exp all 1s
        BNE     $L11

        MOV     D1Re0, D0Ar2
        LSLS    D1Re0, D1Re0, #(32-23)          ! mantisa non-zero?
        MOVNZ   D0Re0, D1Ar3
        MOVNZ   PC, D1RtP                       ! return (D1Ar3)

$L11:
#ifdef __PIC__
        B       ___cmpsf2@PLT
#else
        B       ___cmpsf2
#endif
        .size   ___cmpsf2_nan,.-___cmpsf2_nan
#endif

#ifdef L_eqdf2
!!
!! Floating point - double comparison routines ==,!=,<,<==,>,>=
!! Wrapper entry points for common cmpdf2 routine, these entry points
!! set up correct return value if either is Nan.
!!
        .text
        .global ___eqdf2
        .type   ___eqdf2,function
        .global ___nedf2
        .type   ___nedf2,function
        .global ___ltdf2
        .type   ___ltdf2,function
        .global ___ledf2
        .type   ___ledf2,function
        .align  2
___eqdf2:
___nedf2:
___ltdf2:
___ledf2:

!! If either is NAN return +1

        MOV     D1Ar5, #1
#ifdef __PIC__
        B       ___cmpdf2_nan@PLT
#else
        B       ___cmpdf2_nan
#endif
        .size   ___eqdf2,.-___eqdf2
        .size   ___nedf2,.-___nedf2
        .size   ___ltdf2,.-___ltdf2
        .size   ___ledf2,.-___ledf2

        .global ___gedf2
        .type   ___gedf2,function
        .global ___gtdf2
        .type   ___gtdf2,function
        .align  2
___gedf2:
___gtdf2:

!! If either is NAN return -1

        MOV     D1Ar5, #-1
#ifdef __PIC__
        B       ___cmpdf2_nan@PLT
#else
        B       ___cmpdf2_nan
#endif
        .size   ___gedf2,.-___gedf2
        .size   ___gtdf2,.-___gtdf2
#endif

#ifdef L_eqsf2
!!
!! Floating point - float comparison routines ==,!=,<,<==,>,>=
!! Wrapper entry points for common cmpsf2 routine, these entry points
!! set up correct return value if either is Nan.
!!
        .text
        .global ___eqsf2
        .type   ___eqsf2,function
        .global ___nesf2
        .type   ___nesf2,function
        .global ___ltsf2
        .type   ___ltsf2,function
        .global ___lesf2
        .type   ___lesf2,function
        .align  2
___eqsf2:
___nesf2:
___ltsf2:
___lesf2:

!! If either is NAN return +1

        MOV     D1Ar3, #1
#ifdef __PIC__
        B       ___cmpsf2_nan@PLT
#else
        B       ___cmpsf2_nan
#endif
        .size   ___eqsf2,.-___eqsf2
        .size   ___nesf2,.-___nesf2
        .size   ___ltsf2,.-___ltsf2
        .size   ___lesf2,.-___lesf2

        .global ___gesf2
        .type   ___gesf2,function
        .global ___gtsf2
        .type   ___gtsf2,function
        .align  2
___gesf2:
___gtsf2:

!! If either is NAN return -1

        MOV     D1Ar3, #-1
#ifdef __PIC__
        B       ___cmpsf2_nan@PLT
#else
        B       ___cmpsf2_nan
#endif
        .size   ___gesf2,.-___gesf2
        .size   ___gtsf2,.-___gtsf2
#endif

!! Division routines.

#ifdef L_divdf3
!!
!! Function   : double divdf3 (double a1, double a2)
!!
!! Args       : a1 - double precision floating point number (D1Ar1:D0Ar2) 
!!            : a2 - double precision floating point number (D1Ar3:D0Ar4)
!!
!! Description: Returns a1 divided by a2 (D1Re0:D0Re0)
!!
!! Returns    : (+/-)0.0 / (+/-)0.0 = QNaN
!!            : (+/-)Inf / (+/-)Inf = NaN
!!            :      n   / (+/-)Inf = 0.0
!!            :      n   / (+/-)0.0 = (+/-)Inf
!!            :      a1  /      a2  = n.
!!
!! Notes      : QNaN = 0xFFF80000:00000000 (Quiet NaN)
!!            : SNan = 0xFFF7FFFF:FFFFFFFF (Signaling Nan)
!!            : +Inf = 0x7FF00000:00000000
!!            : -Inf = 0xFFF00000:00000000
!!            : +0   = 0x00000000:00000000
!!            : -0   = 0x80000000:00000000
!!
        .text
        .global ___divdf3
        .type   ___divdf3,function
        .align  2
___divdf3:
        XOR     D1Re0, D1Ar1, D1Ar3                     ! sign1 ^ sign2
        ANDT    D1Re0, D1Re0, #0x8000                   ! extract sign result

        ! Test if a1 == 0.0
        LSL     D0Re0, D1Ar1, #1                        ! Ignore sign bit
        ORS     D0Re0, D0Re0, D0Ar2                     ! a1 == 0 ?
        BZ      $A1IsZero

        ! Test if a2 == 0.0
        LSL     D0Re0, D1Ar3, #1                        ! Ignore sign bit
        ORS     D0Re0, D0Re0, D0Ar4                     ! a2 == 0 ?
        BNZ     $DoDivision

        ! a1 != 0.0 and a2 == 0.0, so return +/-Inf
        ORT     D1Re0, D1Re0, #0x7FF0                   ! Set exponent for (+/-)Inf
        MOV     PC, D1RtP

$A1IsZero:
        LSL     D0Re0, D1Ar3, #1                        ! Ignore sign bit
        ORS     D0Re0, D0Re0, D0Ar4                     ! a2 == 0 ?
        MOV     D0Re0, #0

        ! a1 == 0.0 and a2 != 0.0, so return +/-Zero
        SUBNZ   D0Re0, D0Re0, D0Re0
        MOVNZ   PC, D1RtP                               ! (zero return already set up)

        ! a1 == 0.0 and a2 == 0.0, so return QNaN.
        MOVT    D1Re0, #0xFFF8                          ! maybe sign depends on sign1 ^ sign2
        MOV     PC, D1RtP

$DoDivision:
        MOV     A0.3, D1Re0                             ! sign -> A0.3

        MOV     D1Ar5, D1Ar1
        ANDMT   D1Ar5, D1Ar5, #0x7FFF
        LSR     D1Ar5, D1Ar5, #20                       ! exp1

        MOV     D1Re0, D1Ar3
        ANDMT   D1Re0, D1Re0, #0x7FFF
        LSR     D1Re0, D1Re0, #20                       ! exp2

        SUB     D1Ar5, D1Ar5, D1Re0                     ! exp = exp1 - exp2 + 1022
        ADD     D1Ar5, D1Ar5, #1022                     ! exp ---> D1Ar5
        MOV     A0.2, D1Ar5                             ! exp ---> A0.2

        ANDMT   D1Ar1, D1Ar1, #0x000F
        ORT     D1Ar1, D1Ar1, #0x0010                   ! man1 --->D1Ar1, D0Ar2

        ANDMT   D1Ar3, D1Ar3, #0x000F
        ORT     D1Ar3, D1Ar3, #0x0010                   ! man2 --->D1Ar3, D0Ar4

        CMP     D1Ar1, D1Ar3                            ! man1 < man2
        BGT     $L2
        BLT     $L3

        CMP     D0Ar2, D0Ar4
        BGE     $L2

$L3:
        LSL     D1Ar1, D1Ar1, #1                        ! man1 <<= 1
        LSR     D1Ar5, D0Ar2, #(32-1)
        OR      D1Ar1, D1Ar1, D1Ar5
        LSL     D0Ar2, D0Ar2, #1

        SUB     A0.2, A0.2, #1                          ! exp -= 1

$L2:
        MOV     D0Re0, #0                               ! prepare the result in D1Re0, D0Re0
                                                        ! mask =  1<<53   split into 21 + 31
        MOVT    D0Ar6, #(1<<(21-16))                    ! 1<< 21
        MOV     D1Ar5, #2

$L5: 
        ! while (mask)
        CMP     D0Ar6, #0
        BZ      $L4

        CMP     D1Ar1, D1Ar3                            ! man1 >= man2
        BLT     $L5_1
        BGT     $L5_2

        CMP     D0Ar2, D0Ar4
        BLT     $L5_1

$L5_2:
        OR      D0Re0, D0Re0, D0Ar6                     ! result |= mask

        SUB     D1Ar1, D1Ar1, D1Ar3                     ! man1 - man2
        SUBS    D0Ar2, D0Ar2, D0Ar4
        SUBCS   D1Ar1, D1Ar1, #1

$L5_1:
        LSL     D1Ar1, D1Ar1, #1                        ! man1 <<= 1
        TSTT    D0Ar2, #0x8000
        ORNZ    D1Ar1, D1Ar1, #1                        ! MSBit -> LSBit
        LSL     D0Ar2, D0Ar2, #1

        LSR     D0Ar6, D0Ar6, #1                        ! mask >>= 1
        B       $L5

$L4:
        SUBS    D1Ar5, D1Ar5, #1
        BZ      $L6

        MOV     D1Re0, D0Re0
        MOV     D0Re0, #0
        MOVT    D0Ar6, #(1<<(31-16))                    ! 1 << 31
        B       $L5

$L6:
        ADDS    D0Re0, D0Re0, #1                        ! result += 1
        ADDCS   D1Re0, D1Re0, #1

        ADD     D1Ar3, A0.2, #1                         ! exp += 1

        LSR     D0Re0, D0Re0, #1                        ! result <<= 1
        LSL     D0Ar2, D1Re0, #(32-1)
        OR      D0Re0, D0Re0, D0Ar2
        LSR     D1Re0, D1Re0, #1

        MOV     D1Ar1, A0.3
        ANDMT   D1Re0, D1Re0, #0x000F                   ! discard hidden bit
        OR      D1Re0, D1Re0, D1Ar1                     ! combine sign

        MOV     D1Ar5, #0
        MAX     D1Ar3, D1Ar3, D1Ar5                     ! exp < 0 ?
        SUBLT   D0Re0, D0Re0, D0Re0                     ! < 0 return +/-Zero
        MOVLT   D1Re0, D1Ar1
        MOVLT   PC, D1RtP

        SUBEQ   D0Re0, D0Re0, D0Re0                     ! Don(t) currently handle
        MOVEQ   D1Re0, D1Ar1                            ! denormals, so return
        MOVEQ   PC, D1RtP                               ! +/-Zero

        MOV     D1Ar5, #0x7FF
        MIN     D1Ar3, D1Ar3, D1Ar5                     ! exp >= 0x7FF
        SUBGE   D0Re0, D0Re0, D0Re0                     ! >= return +/- Inf
        MOVGE   D1Re0, D1Ar1

        LSL     D1Ar3, D1Ar3, #20                       ! postion exp
        OR      D1Re0, D1Re0, D1Ar3                     ! combine exp

        MOV     PC, D1RtP
        .size   ___divdf3,.-___divdf3
#endif

#ifdef L_divsf3
!!
!! Function   : float divsf3 (float a1, float a2)
!!
!! Args       : a1 - single precision floating point number (D1Ar1) 
!!            : a2 - single precision floating point number (D0Ar2)
!!
!! Description: Returns a1 divided by a2 (D0Re0)
!!
!! Returns    : (+/-)0.0 / (+/-)0.0 = QNaN
!!            : (+/-)Inf / (+/-)Inf = NaN
!!            :      n   / (+/-)Inf = 0.0
!!            :      n   / (+/-)0.0 = (+/-)Inf
!!            :      a1  /      a2  = n.
!!
!! Notes      : QNaN = 0xFFC00000 (Quiet NaN)
!!            : SNan = 0xFFBFFFFF (Signaling Nan)
!!            : +Inf = 0x7F800000
!!            : -Inf = 0xFF800000
!!            : +0   = 0x00000000
!!            : -0   = 0x80000000
!!
        .text
        .global ___divsf3
        .type   ___divsf3,function
        .align  2
___divsf3:
        MOV     D0Re0, D1Ar1
        XOR     D0Re0, D0Re0, D0Ar2             ! sign1 ^ sign2
        ANDT    D0Re0, D0Re0, #0x8000           ! extract sign

        ! Test if a1 == 0.0
        LSLS    D1Re0, D1Ar1, #1                ! Ignore sign bit
        BZ      $L1

        ! Test if a2 == 0.0
        LSLS    D0Ar4, D0Ar2, #1                ! Ignore sign bit
        BNZ     $L2

        ! a1 != 0.0 and a2 == 0.0 so return +/-Inf
        ORT     D0Re0, D0Re0, #0x7F80
        MOV     PC, D1RtP

$L1:
        ! 0 / X return QNan or +/-Zero
        LSLS    D0Ar4, D0Ar2, #1                ! Ignore sign bit (D0Ar4)

        ! a1 == 0.0 and a2 != 0.0, so return +/-Zero
        MOVNZ   PC, D1RtP                       ! (zero return already set up)

        ! a1 == 0.0 and a2 == 0.0, so return QNAN
        MOVT    D0Re0, #0xFFC0                  ! maybe sign depends on sign1 ^ sign2
        MOV     PC, D1RtP

$L2:
        MOV     D0Ar4, D0Re0                    ! sign bit

        MOV     D1Ar5, D1Ar1
        ANDMT   D1Ar5, D1Ar5, #0x7FFF
        LSR     D1Ar5, D1Ar5, #23               ! exp1

        MOV     D1Re0, D0Ar2
        ANDMT   D1Re0, D1Re0, #0x7FFF
        LSR     D1Re0, D1Re0, #23               ! exp2

        SUB     D1Ar5, D1Ar5, D1Re0
        ADD     D1Ar5, D1Ar5, #126              ! exp = exp1 - exp2 + 126

        MOV     D0Ar6, D1Ar1
        ANDMT   D0Ar6, D0Ar6, #0x007F
        ORT     D0Ar6, D0Ar6, #0x0080           ! man1 -->D0Ar6

        MOV     D0Re0, D0Ar2
        ANDMT   D0Re0, D0Re0, #0x007F
        ORT     D0Re0, D0Re0, #0x0080           ! man2 --> D0Re0

        CMP     D0Ar6, D0Re0                    ! man1 < man2
        BGE     $L3

        LSL     D0Ar6, D0Ar6, #1                ! man1 <<= 1
        SUB     D1Ar5, D1Ar5, #1                ! exp -= 1

$L3:
        !mask = 0x01000000
        MOVT    D1Ar1, #0x0100
        MOV     D1Re0, #0                       ! result = 0

$L4:
        CMP     D0Ar6, D0Re0
        ORGE    D1Re0, D1Re0, D1Ar1             ! result |= mask
        SUBGE   D0Ar6, D0Ar6, D0Re0

        LSL     D0Ar6, D0Ar6, #1                ! man1 <<= 1
        LSRS    D1Ar1, D1Ar1, #1                ! mask >>= 1
        BNZ     $L4

        ADD     D1Ar1, D1Re0, #1                ! result += 1

        ADD     D1Ar5, D1Ar5, #1                ! exp += 1
        LSR     D1Ar1, D1Ar1, #1                ! result >>= 1

        ANDMT   D1Ar1, D1Ar1, #0x007F           ! remove hidden bit

        MOV     D1Ar3, #0
        MAX     D1Ar5, D1Ar5, D1Ar3             ! exp < 0 ?
        MOVLT   D0Re0, D0Ar4                    ! < 0 return +/-Zero
        MOVLT   PC, D1RtP

        MOVEQ   D0Re0, D0Ar4                    ! Don(t) currently handle
        MOVEQ   PC, D1RtP                       ! denormals, so return +/-Zero

        MOV     D1Ar3, #0xFF
        MIN     D1Ar5, D1Ar5, D1Ar3             ! exp >= 0xFF
        SUBGE   D1Ar1, D1Ar1, D1Ar1             ! >= return +/-Inf

        LSL     D1Ar5, D1Ar5, #23
        OR      D0Re0, D1Ar1, D1Ar5             ! man|exp
        OR      D0Re0, D0Re0, D0Ar4             ! |sign ->D0Re0

        MOV     PC, D1RtP
        .size   ___divsf3,.-___divsf3
#endif

!! Floating point multiplication

#ifdef L_muldf3
!!
!! Floating point - double multiplicatiion
!!
        .text
        .global ___muldf3
        .type   ___muldf3,function
        .align  2
___muldf3:
        XOR     D1Re0, D1Ar1, D1Ar3             ! sign1 ^ sign2
        ANDT    D1Re0, D1Re0, #0x8000           ! extract sign bit
        LSL     D0Re0, D1Ar1, #1                ! Ignore sign
        ORS     D0Re0, D0Re0, D0Ar2             ! zero * ...
        MOVZ    PC, D1RtP                       ! return 0

        LSL     D0Re0, D1Ar3, #1                ! Ignore sign
        ORS     D0Re0, D0Re0, D0Ar4             ! ... * zero
        MOVZ    PC, D1RtP                       ! return 0

        MSETL   [A0StP], D0.4, D0.5, D0.6, D0.7

        MOV     D1Ar5, D1Ar1
        ANDMT   D1Ar5, D1Ar5, #0x7FFF
        LSR     D1Ar5, D1Ar5, #20               ! exp1

        MOV     D1.5, D1Ar3
        ANDMT   D1.5, D1.5, #0x7FFF
        LSR     D1.5, D1.5, #20                 ! exp2

        SUB     D1Ar5, D1Ar5, #1022
        ADD     D1Ar5, D1Ar5, D1.5              ! exp = exp1 + exp2 - 1022

        MOV     D1.5, D1Ar1
        ANDMT   D1.5, D1.5, #0x000F
        ORT     D1.5, D1.5, #0x0010             ! man1 --->D1.5, D0.5
        MOV     D0.5, D0Ar2

        MOV     D1.6, D1Ar3
        ANDMT   D1.6, D1.6, #0x000F
        ORT     D1.6, D1.6, #0x0010             ! man2 --->D1.6, D0.6
        MOV     D0.6, D0Ar4

        XOR     D1Re0, D1Ar1, D1Ar3             ! store the sign bit
        MOV     D0Re0, D1Ar5

        SETL    [A0StP+#8++], D0Re0,D1Re0       ! XXX

        LSR     D0Ar6, D0.5, #21                ! D1Ar1:D0Ar2 = man1 >> 21
        LSL     D0Ar2, D1.5, #(32-21)
        MOV     D1Ar1, #0                       ! top 32 bits are zero
        OR      D0Ar2, D0Ar2, D0Ar6

        LSR     D0Ar6, D0.6, #21                ! D1Ar3:D0Ar4 = man2 >> 21
        LSL     D0Ar4, D1.6, #(32-21)
        MOV     D1Ar3, #0                       ! top 32 bits are zero
        OR      D0Ar4, D0Ar4, D0Ar6

        CALLR   D1RtP, ___muldi3_

        MOV     D1.7, D1Re0
        MOV     D0.7, D0Re0

        LSR     D0Ar6, D0.5, #21                ! man1 >> 21
        LSL     D0Ar2, D1.5, #(32-21)
        MOV     D1Ar1, #0
        OR      D0Ar2, D0Ar2, D0Ar6

        MOV     D0Ar4, D0.6
        ANDMT   D0Ar4, D0Ar4, #0x001F
        MOV     D1Ar3, #0

        CALLR   D1RtP, ___muldi3_

        LSR     D0Re0, D0Re0, #21               ! >> 21
        LSL     D0Ar6, D1Re0, #(32-21)
        LSR     D1Re0, D1Re0, #21
        OR      D0Re0, D0Re0, D0Ar6

        ADDS    D0.7, D0.7, D0Re0               ! D1.7:D0.7 + D1Re0:D0Re0
        ADD     D1.7, D1.7, D1Re0
        ADDCS   D1.7, D1.7, #1

        MOV     D1Ar1, #0
        LSL     D0Ar2, D1.6, #(32-21)           ! man2 >> 21
        LSR     D0Ar6, D0.6, #21
        OR      D0Ar2, D0Ar2, D0Ar6

        MOV     D0Ar4, D0.5
        ANDMT   D0Ar4, D0Ar4, #0x001F
        MOV     D1Ar3, #0

        CALLR   D1RtP, ___muldi3_

        LSR     D0Re0, D0Re0, #21               ! >> 21
        LSL     D0Ar6, D1Re0, #(32-21)
        LSR     D1Re0, D1Re0, #21
        OR      D0Re0, D0Re0, D0Ar6

        ADDS    D0.7, D0.7, D0Re0               ! D1.7:D0.7 + D1Re0:D0Re0
        ADD     D1.7, D1.7, D1Re0
        ADDCS   D1.7, D1.7, #1

        GETL D0Re0,D1Re0, [A0StP++#-8]          !  prepare to recover D1Re0, D1Ar5

        MOV     D1.6, D1.7
        MOV     D0.6, D0.7

        LSR     D0.6, D0.6, #2                  ! man >> 2 (d1.6, d0.6)
        LSL     D0.5, D1.6, #(32-2)
        LSR     D1.6, D1.6, #2
        OR      D0.6, D0.6, D0.5

        MOV     D1Ar5, D0Re0

        MOV     D0Re0, D1.6
        ANDT    D0Re0, D0Re0, #0x2000           ! 1 << 61
        CMP     D0Re0, #0                       ! round ...
        BZ      $L11

        ADDS    D0.6, D0.6, #0x100              ! D1.6:D0.6 + 0x100
        ADDCS   D1.6, D1.6, #1

        LSL     D0.5, D1.6, #(32-9)             ! >> 9
        LSR     D0.6, D0.6, #9
        LSR     D1.6, D1.6, #9
        OR      D0.6, D0.6, D0.5
        B       $L13

$L11:
        ADDS    D0.6, D0.6, #0x80               ! +128
        ADDCS   D1.6, D1.6, #1

        LSR     D0.6, D0.6, #8                  ! >> 8
        LSL     D0.5, D1.6, #(32-8)
        LSR     D1.6, D1.6, #8
        OR      D0.6, D0.6, D0.5

        SUB     D1Ar5, D1Ar5, #1                ! exp -= 1

$L13:
        MOV     D0Re0, D1.6
        ANDT    D0Re0, D0Re0, #0x0020
        CMP     D0Re0, #0
        BZ      $L14

        LSR     D0.6, D0.6, #1                  ! man >> 1
        LSL     D0.5, D1.6, #(32-1)
        LSR     D1.6, D1.6, #1
        OR      D0.6, D0.6, D0.5

        ADD     D1Ar5, D1Ar5, #1                ! exp += 1

$L14:
        ANDT    D1Re0, D1Re0, #0x8000           ! get the sign bit
        MOV     D1Ar1, D1Re0                    ! remember sign
        ANDMT   D1.6, D1.6, #0x000F             ! remove hidden bit
        OR      D1Re0, D1Re0, D1.6              ! sign | HI(mantisa)
        MOV     D0Re0, D0.6                     ! LO(mantisa)

        SUB     A0.3, A0StP, #32
        MGETL   D0.4, D0.5, D0.6, D0.7, [A0.3]
        SUB     A0StP, A0StP, #32

        MOV     D1Ar3, #0
        MAX     D1Ar5, D1Ar5, D1Ar3             ! exp < 0 ?
        SUBLT   D0Re0, D0Re0, D0Re0             ! < 0 return +/-Zero
        MOVLT   D1Re0, D1Ar1
        MOVLT   PC, D1RtP

        SUBEQ   D0Re0, D0Re0, D0Re0             ! Don(t) currently handle
        MOVEQ   D1Re0, D1Ar1                    ! denormals, so return
        MOVEQ   PC, D1RtP                       ! +/-Zero

        MOV     D1Ar3, #0x7FF
        MIN     D1Ar5, D1Ar5, D1Ar3             ! exp >= 0x7FF ?
        SUBGE   D0Re0, D0Re0, D0Re0             ! >= return +/-Inf
        MOVGE   D1Re0, D1Ar1
        
        LSL     D1Ar5, D1Ar5, #20               ! position exp
        OR      D1Re0, D1Re0, D1Ar5             ! add exp

        MOV     PC, D1RtP

!!
!! 32-bit x 32-bit -> 64-bit
!!
        .align  2
___muldi3_:
        MOV     A0.2, D0Ar6
        MOV     A0.3, D1Ar5

        LSR     D1Ar1, D0Ar2, #16               ! h1
        LSR     D1Ar3, D0Ar4, #16               ! high 2
        MULW    D1Re0, D1Ar1, D1Ar3             ! h1 * h2

        MOV     D0Ar6, #0xFFFF
        AND     D1Ar5, D0Ar2, D0Ar6             ! l1
        MULW    D0Re0, D1Ar5, D1Ar3             ! l1 * h2

        AND     D0Ar2, D0Ar4, D0Ar6             ! l2
        MOV     D0Ar4, D1Ar1                    ! h1
        MULW    D1Ar1, D0Ar4, D0Ar2             ! h1 * l2

        MOV     D0Ar4, D1Ar5                    ! l1
        LSR     D1Ar5, D0Re0, #16
        LSL     D0Re0, D0Re0, #16
        ADD     D1Re0, D1Re0, D1Ar5
        LSR     D1Ar5, D1Ar1, #16
        ADD     D1Re0, D1Re0, D1Ar5
        LSL     D0Ar6, D1Ar1, #16

        ADDS    D0Re0, D0Re0, D0Ar6
        ADDCS   D1Re0, D1Re0, #1

        MULW    D0Ar6, D0Ar2, D0Ar4

        ADDS    D0Re0, D0Re0, D0Ar6
        ADDCS   D1Re0, D1Re0, #1

        MOV     D0Ar6, A0.2
        MOV     D1Ar5, A0.3

        MOV     PC, D1RtP
        .size   ___muldf3,.-___muldf3
#endif

#ifdef L_mulsf3
!!
!! Floating point - float multiplication
!!
        .text
        .global ___mulsf3
        .type   ___mulsf3,function
        .align  2
___mulsf3:
        MOV     D0Re0, D1Ar1
        XOR     D0Re0, D0Re0, D0Ar2             ! sign1 ^ sign2
        ANDT    D0Re0, D0Re0, #0x8000           ! extract sign bit
        LSLS    D1Re0, D1Ar1, #1                ! Ignore sign
        MOVZ    PC, D1RtP                       ! +/-Zero * ...  return Zero

        LSLS    D1Re0, D0Ar2, D0Ar2             ! Ignore sign
        MOVZ    PC, D1RtP                       ! ... * +/-Zero return Zero 

        MOV     D1Ar5, D1Ar1
        ANDMT   D1Ar5, D1Ar5, #0x7fff
        LSR     D1Ar5, D1Ar5, #23               ! exp1

        MOV     D1Re0, D0Ar2
        ANDMT   D1Re0, D1Re0, #0x7fff
        LSR     D1Re0, D1Re0, #23               ! exp2

        SUB     D1Ar5, D1Ar5, #126              ! exp = exp1 - 126
        ADD     D1Ar5, D1Ar5, D1Re0             ! exp += exp2

        MOV     D0Ar6, D1Ar1
        ANDMT   D0Ar6, D0Ar6, #0x7f
        ORT     D0Ar6, D0Ar6, #0x80             ! man1

        MOV     D0Re0, D0Ar2
        ANDMT   D0Re0, D0Re0, #0x7f
        ORT     D0Re0, D0Re0, #0x80             ! man2

        MOV     D1Re0, D0Ar2
        XOR     D1Re0, D1Ar1, D1Re0             ! store the sign bit in D1Re0

        LSR     D0Ar2, D0Ar6, #8
        LSR     D0Ar4, D0Re0, #8
        MULW    D1Ar1, D0Ar2, D0Ar4             ! one 16X16

        MOV     D0Ar2, D0Ar6
        AND     D0Ar2, D0Ar2, #0xff
        MULW    D1Ar3, D0Ar2, D0Ar4             ! one 8X16

        LSR     D0Ar2, D0Ar6, #8
        MOV     D0Ar4, D0Re0
        AND     D0Ar4, D0Ar4, #0xff
        MULW    D0Re0, D0Ar2, D0Ar4             ! another 16X8

        SWAP    D1Re0, D0Re0
        ADD     D1Ar3, D1Ar3, D1Re0             ! add 16x8(s) partial results
        LSR     D1Ar3, D1Ar3, #8
        ADD     D1Ar1, D1Ar1, D1Ar3             ! accumulate partial result

        LSR     D1Ar1, D1Ar1, #2                ! man >> 2
        TSTT    D1Ar1, #0x2000                  ! round ... 

        ADDNZ   D1Ar1, D1Ar1, #0x20
        LSRNZ   D1Ar1, D1Ar1, #6

        ADDZ    D1Ar1, D1Ar1, #0x10
        LSRZ    D1Ar1, D1Ar1, #5
        SUBZ    D1Ar5, D1Ar5, #1

        TSTT    D1Ar1, #0x100

        LSRNZ   D1Ar1, D1Ar1, #1
        ADDNZ   D1Ar5, D1Ar5, #1

        ANDT    D0Re0, D0Re0, #0x8000           ! get the sign bit
        ANDMT   D1Ar1, D1Ar1, #0x7f             ! remove hidden hit

        MOV     D1Ar3, #0
        MAX     D1Ar5, D1Ar5, D1Ar3             ! exp < 0 ?
        MOVLT   PC, D1RtP                       ! < 0 return +/-Zero
                                                ! Don(t) currently handle
        MOVEQ   PC, D1RtP                       ! denormals, so return +/-Zero

        MOV     D1Ar3, #0xFF
        MIN     D1Ar5, D1Ar5, D1Ar3             ! exp >= 0xFF ?
        SUBGE   D1Ar1, D1Ar1, D1Ar1             ! >= return +/-Inf

        MOV     D1Re0, D0Re0
        LSL     D1Ar5, D1Ar5, #23
        OR      D1Re0, D1Re0, D1Ar5             ! sign|exp
        OR      D0Re0, D1Re0, D1Ar1             ! |man ->D0Re0

        MOV     PC, D1RtP
        .size   ___mulsf3,.-___mulsf3
#endif

!! Floating point conversion routines

#ifdef L_extendsfdf2
!!
!! float -> double conversion
!!
        .text
        .global ___extendsfdf2
        .type   ___extendsfdf2,function
        .align  2
___extendsfdf2:
        LSLS    D0Re0, D1Ar1, #1                ! Ignore sign bit
        MOVZ    D1Re0, D1Ar1                    ! +/- Zero
        MOVZ    PC, D1RtP                       ! return +/- Zero

        MOV     D1Ar3, D1Ar1
        ANDT    D1Ar3, D1Ar3, #0x8000           ! extract sign (D1Ar3)

        MOV     D0Ar2, D1Ar1                    ! extract mant (D0Ar2)
        ANDMT   D0Ar2, D0Ar2, #0x007F

        LSR     D1Re0, D1Ar1, #23               ! extract exp (D1Re0)
        AND     D1Re0, D1Re0, #0x00FF

        ADD     D1Re0, D1Re0, #(1023-127)       ! exp += ...

        LSL     D1Re0, D1Re0, #(52-32)          ! position exp MSWord 

        OR      D1Re0, D1Re0, D1Ar3             ! combine with sign | exp

        MOV     D1Ar3, D0Ar2                    ! extract mant MSWord
        LSR     D1Ar3, D1Ar3, #(23-(52-32))

        OR      D1Re0, D1Re0, D1Ar3             ! combine sign | exp | mant
        LSL     D0Re0, D0Ar2, #(32 - 3)         ! 3 = (23-(52-32))

        MOV     PC, D1RtP
        .size   ___extendsfdf2,.-___extendsfdf2
#endif

#ifdef L_truncdfsf2
!!
!! double -> float conversion
!!
        .text
        .global ___truncdfsf2
        .type   ___truncdfsf2,function
        .align  2
___truncdfsf2:                          ! has round solution
        LSL     D0Re0, D1Ar1, #1        ! Ignore sign bit
        ORS     D0Re0, D0Re0, D0Ar2     ! zero?
        MOVZ    D0Re0, D1Ar1
        MOVZ    PC, D1RtP               ! return +/-Zero

!! extract sign
        MOV     D0Ar6, D1Ar1
        ANDT    D0Ar6, D0Ar6, #0x8000   ! save sign

!! extract exponent
        MOV     D1Re0, D1Ar1
        ANDMT   D1Re0, D1Re0, #0x7FFF   ! remove the sign bit
        LSR     D1Re0, D1Re0, #20
        SUB     D1Re0, D1Re0, #(1023-127)

!! extract mantisa
        ANDMT   D1Ar1, D1Ar1, #0x000F

!! add hidden bit
        ORT     D1Ar1, D1Ar1, #0x0010

!! position significand for rounding
        LSL     D0Re0, D1Ar1, #4        ! (24 - (52 - 32))
        LSR     D0Ar2, D0Ar2, #(32 - 4)
        OR      D0Re0, D0Re0, D0Ar2

        ADD     D0Re0, D0Re0, #1        ! round + 1
        TSTT    D0Re0, #0xFE00          ! test round overflowed?
        ADDNZ   D1Re0, D1Re0, #1
        LSR     D0Re0, D0Re0, #1

!! check biased exponent within range 0 .. 254
        CMP     D1Re0, #0
        MOVLT   D0Re0, D0Ar6            ! return +/- Zero
        MOVLT   PC, D1RtP               ! return 0

        MOVT    D0Ar4, #0x7F80          ! Inf
        OR      D0Ar4, D0Ar4, D0Ar6     ! +/-Inf

        CMP     D1Re0, #254
        MOVGT   D0Re0, D0Ar4
        MOVGT   PC, D1RtP               ! return +/Inf

!! pack sign , exp, mantisa
        ANDMT   D0Re0, D0Re0, #0x007F   ! remove hidden bit
        LSL     D0Ar2, D1Re0, #23       ! align exp
        OR      D0Re0, D0Re0, D0Ar2     ! exp | mantisa
        OR      D0Re0, D0Re0, D0Ar6     ! sign | exp | mantisa
        MOV     PC, D1RtP               ! done
        .size   ___truncdfsf2,.-___truncdfsf2
#endif

!! Floating point  -> integer conversion.

#ifdef L_fixdfdi
!!
!! Floating point - double -> signed long long
!!
        .text
        .global ___fixdfdi
        .type   ___fixdfdi,function
        .align  2
___fixdfdi:
        MOV     D0Re0, D1Ar1
        ORS     D0Re0, D0Re0, D0Ar2
        MOVZ    D1Re0, D0Re0
        MOVZ    PC, D1RtP

        MOV     D1Re0, D1Ar1
        ANDMT   D1Re0, D1Re0, #0x7FFF   ! discard sign bit

        LSR     D1Re0, D1Re0, #20
        SUBS    D1Re0, D1Re0, #1085     ! exp
        BLE     $L3

!! exp > 0 not representable (overflow)

        TSTT    D1Ar1, #0x8000          ! signed?
        MOVT    D1Re0, #0x8000          ! yes, result
        MOV     D0Re0, #0               ! MIN_INT
        SUBZ    D1Re0, D1Re0, #1        ! else result
        SUBZ    D0Re0, D0Re0, #1        ! MAX_INT
        MOV     PC, D1RtP               ! return

$L3:
        CMP     D1Re0, #-62             ! -(BITS_PER_DI - 2)
        SUBLT   D0Re0, D0Re0, D0Re0
        MOVLT   D1Re0, D0Re0
        MOVLT   PC, D1RtP               ! underflow

        MOV     D0Ar6, D1Re0            ! exp

        MOV     D0Re0, D0Ar2
        MOV     D1Re0, D1Ar1            ! man -> D1Re0, D0Re0

        ANDMT   D1Re0, D1Re0, #0x000F
        ORT     D1Re0, D1Re0, #0x0010

        LSL     D1Re0, D1Re0, #10       ! man <<=10

        LSR     D1Ar5, D0Re0, #22
        OR      D1Re0, D1Re0, D1Ar5     ! mantissa

        LSL     D0Re0, D0Re0, #10

        CMP     D0Ar6, #0
        BZ      $L5

        CMP     D0Ar6, #-32
        BGT     $L5_1
        BLT     $L5_2

!! >> 32 bits.
        MOV     D0Re0, D1Re0
        MOV     D1Re0, #0
        B       $L5

$L5_2:
!! >> more than 32 bits

        ADD     D0Ar6, D0Ar6, #32
        CMP     D0Ar6, #-32

!! >> more than 64 bits, return 0
        ADDLE   D0Re0, D0Re0, D0Re0
        MOVLE   PC, D1RtP

        MOV     D0Re0, D1Re0
        MOV     D1Re0, #0
        NEG     D0Ar6, D0Ar6
        LSR     D0Re0, D0Re0, D0Ar6
        B       $L5

$L5_1:
!! >> less than 32 bits
        CMP     D0Ar6, #0               ! shift >>0 ?
        BEQ     $L5

        ADD     D1Ar5, D0Ar6, #32       ! (32 + (-exp))
        LSL     D0Ar2, D1Re0, D1Ar5

        NEG     D0Ar6, D0Ar6
        LSR     D0Re0, D0Re0, D0Ar6

        MOV     D1Ar5, D0Ar6
        LSR     D1Re0, D1Re0, D1Ar5

        OR      D0Re0, D0Re0, D0Ar2

$L5:
        TSTT    D1Ar1, #0x8000          ! test sign
        MOVZ    PC, D1RtP

        NEGS    D0Re0, D0Re0            ! change sign
        NEG     D1Re0, D1Re0
        SUBNZ   D1Re0, D1Re0, #1
        MOV     PC, D1RtP
        .size   ___fixdfdi,.-___fixdfdi
#endif

#ifdef L_fixdfsi
!!
!! Floating point - double -> signed long long
!!
        .text
        .global ___fixdfsi
        .type   ___fixdfsi,function
        .align  2
___fixdfsi:
        MOV     D0Re0, D1Ar1
        ORS     D0Re0, D0Re0, D0Ar2     ! zero?
        MOVZ    PC, D1RtP               ! return 0

        MOV     D1Re0, D1Ar1            ! keep sign bit

        LSR     D0Ar6, D1Ar1, #20       ! extact exponent (D0Ar6)
        AND     D0Ar6, D0Ar6, #0x07FF
        SUBS    D0Ar6, D0Ar6, #1053     ! exp -= (1023 + 30)
        BLE     $L1

!! exp > 0 not representable (overflow)

        TSTT    D1Re0, #0x8000          ! signed ?
        MOVT    D0Re0, #0x8000          ! yes, result MIN INT
        SUBZ    D0Re0, D0Re0, #1        ! no, result MAX INT
        MOV     PC, D1RtP               ! return 

$L1:
        ANDMT   D1Ar1, D1Ar1, #0x000F   ! extract mantisa
        ORT     D1Ar1, D1Ar1, #0x0010   ! add hidden bit
        LSR     D1Ar3, D0Ar2, #22       ! (32 - 10)
        LSL     D0Ar2, D0Ar2, #10
        LSL     D1Ar1, D1Ar1, #10
        OR      D1Ar1, D1Ar1, D1Ar3

        CMP     D0Ar6, #-30              ! -(BITS_PER_SI - 2)

        SUBLT   D0Re0, D0Re0, D0Re0     ! < -30 underflow
        MOVLT   PC, D1RtP               ! return 0

        MOV     D0Re0, D1Ar1
        NEG     D0Ar6, D0Ar6
        LSR     D0Re0, D0Re0, D0Ar6     ! mant >>= exp
        TSTT    D1Re0, #0x8000          ! signed?

        MOVZ    PC, D1RtP               ! return mant
        NEG     D0Re0, D0Re0
        MOV     PC, D1RtP               ! return -mant
        .size   ___fixdfsi,.-___fixdfsi
#endif

#ifdef L_fixsfdi
!!
!! Floating point - float -> signed long long
!!
        .text
        .global ___fixsfdi
        .type   ___fixsfdi,function
        .align  2
___fixsfdi:
        SETL    [A0StP+#8++], D0.4, D1RtP
#ifdef __PIC__
        CALLR   D1RtP, ___extendsfdf2@PLT
#else
        CALLR   D1RtP, ___extendsfdf2
#endif
        MOV     D1Ar1, D1Re0
        MOV     D0Ar2, D0Re0

        GETL    D0.4, D1RtP, [A0StP++#-8]

#ifdef __PIC__
        B       ___fixdfdi@PLT
#else
        B       ___fixdfdi
#endif
        .size   ___fixsfdi,.-___fixsfdi
#endif

#ifdef L_fixsfsi
!!
!! Floating point - float -> signed int/long
!!
        .text
        .global ___fixsfsi
        .type   ___fixsfsi,function
        .align  2
___fixsfsi:
        LSLS    D0Re0, D1Ar1, #1        ! Ignore sign bit
        MOVZ    PC, D1RtP               ! Zero? return 0

        MOV     D1Re0, D1Ar1
        ANDMT   D1Re0, D1Re0, #0x7FFF   ! remove sign bit
        LSR     D1Re0, D1Re0, #23
        SUBS    D1Re0, D1Re0, #(127 + 30)

        BLE     $L3

!! exp > 0 (overflow) return MIN_INT or MAX_INT

        MOVT    D0Re0, #0x8000          ! MIN_INT (0x80000000)
        TSTT    D1Ar1, #0x8000
        SUBZ    D0Re0, D0Re0, #1        ! MAX_INT (0x7FFFFFFF)
        MOV     PC, D1RtP

$L3:
        CMP     D1Re0, #-30             ! -(BITS_PER_SI - 2)

        SUBLT   D0Re0, D0Re0, D0Re0     ! underflow?
        MOVLT   PC, D1RtP               ! return 0

        MOV     D0Re0, D1Ar1
        ANDMT   D1Ar1, D1Ar1, #0x007F   ! extract mantisa
        ORT     D1Ar1, D1Ar1, #0x0080   ! add hidden bit
        LSL     D1Ar1, D1Ar1, #7

        CMP     D1Re0, #0
        BZ      $L5

        NEG     D1Re0, D1Re0
        LSR     D1Ar1, D1Ar1, D1Re0
$L5:

        TSTT    D0Re0, #0x8000          ! signed?
        MOV     D0Re0, D1Ar1
        MOVZ    PC, D1RtP
        NEG     D0Re0, D0Re0
        MOV     PC, D1RtP
        .size   ___fixsfsi,.-___fixsfsi
#endif

#ifdef L_fixunsdfdi
!!
!!  Floating point - double -> unsigned long long
!!
        .text
        .global ___fixunsdfdi
        .type   ___fixunsdfdi,function
        .align  2
___fixunsdfdi:
        LSL     D0Re0, D1Ar1, #1        ! Ignore sign bit
        ORS     D0Re0, D0Re0, D0Ar2     ! zero?
        MOVZ    D1Re0, D0Re0
        MOVZ    PC, D1RtP               ! return 0

        TSTT    D1Ar1, #0x8000          ! Negative?
        SUBNZ   D0Re0, D0Re0, D0Re0     ! < 0
        MOVNZ   D1Re0, D0Re0
        MOVNZ   PC, D1RtP               ! return 0

        LSR     D1Ar3, D1Ar1, #20       ! extract exponent (D1Ar3)
        SUBS    D1Ar3, D1Ar3, #1086     ! exp -= (1023 + 32 + 31)

        SUBGT   D0Re0, D0Re0, D0Re0     ! exp > 0
        SUBGT   D0Re0, D0Re0, #1
        MOVGT   D1Re0, D0Re0
        MOVGT   PC, D1RtP               ! return MAX_UDI

        ANDMT   D1Ar1, D1Ar1, #0x000F   ! extract mantisa
        LSR     D1Ar5, D0Ar2, #21       ! (32 - 11)
        LSL     D0Ar2, D0Ar2, #11
        LSL     D1Ar1, D1Ar1, #11
        OR      D1Ar1, D1Ar1, D1Ar5
        ORT     D1Ar1, D1Ar1, #0x8000   ! add hidden bit

        CMP     D1Ar3, #-32             ! -exp >= 32
        BLE     $L9                     ! branch 

        CMP     D1Ar3, #0               ! exp == 0?
        MOVEQ   D1Re0, D1Ar1
        MOVEQ   D0Re0, D0Ar2
        MOVEQ   PC, D1RtP

!! Shift < 32 bits

        ADD     D1Re0, D1Ar3, #32       ! (32 + (-exp))
        LSL     D0Re0, D1Ar1, D1Re0     ! H << (exp - 32)
        NEG     D1Ar3, D1Ar3            ! exp = -exp
        MOV     D1Ar5, D0Ar2
        LSR     D1Re0, D1Ar1, D1Ar3     ! H >> exp
        LSR     D0Ar6, D1Ar5, D1Ar3     ! L >> exp
        OR      D0Re0, D0Re0, D0Ar6
        MOV     PC, D1RtP

$L9:

!! Shift >= 32 bits

        MOV     D1Re0, #0               ! shifting >= 32 (MSWord result 0)
        ADD     D1Ar3, D1Ar3, #32
        CMP     D1Ar3, #-31             ! -((BITS_PER_DI - 32) - 1)

        MOVLT   D0Re0, D1Re0            ! underflow?
        MOVLT   PC, D1RtP               ! return 0

        NEG     D1Ar3, D1Ar3            ! exp = -exp
        LSR     D1Ar1, D1Ar1, D1Ar3     ! >>(exp - 32)
        MOV     D0Re0, D1Ar1
        MOV     PC, D1RtP
        .size   ___fixunsdfdi,.-___fixunsdfdi
#endif

#ifdef L_fixunsdfsi
!!
!! Floating point - double -> unsigned int/long
!!
        .text
        .global ___fixunsdfsi
        .type   ___fixunsdfsi,function
___fixunsdfsi:
        LSL     D0Re0, D1Ar1, #1                ! Ignore sign bit
        ORS     D0Re0, D0Re0, D0Ar2             ! zero?
        MOVZ    PC, D1RtP                       ! return 0

        TSTT    D1Ar1, #0x8000                  ! Negative?
        SUBNZ   D0Re0, D0Re0, D0Re0             ! < 0
        MOVNZ   PC, D1RtP                       ! return 0

        LSR     D0Ar6, D1Ar1, #20               ! extract exponent (D0Ar6)
        SUBS    D0Ar6, D0Ar6, #(1023+32+31)     ! exp -= (1023 + 32 + 31)

        SUBGT   D0Re0, D0Re0, D0Re0             ! exp > 0 (overflow)
        SUBGT   D0Re0, D0Re0, #1
        MOVGT   PC, D1RtP                       ! return MAX_USI

        ANDMT   D1Ar1, D1Ar1, #0x000F           ! extract mantisa
        LSR     D1Ar3, D0Ar2, #21               ! (32 - 11)
        LSL     D0Ar2, D0Ar2, #11
        LSL     D1Ar1, D1Ar1, #11
        OR      D1Ar1, D1Ar1, D1Ar3
        ORT     D1Ar1, D1Ar1, #0x8000           ! add hidden bit

        NEG     D0Ar6, D0Ar6                    ! exp = -exp
        CMP     D0Ar6, #64                      ! exp >= BITS_PER_DI ?

        SUBGE   D0Re0, D0Re0, D0Re0             ! >= underflow
        MOVGE   PC, D1RtP                       ! return 0

        CMP     D0Ar6, #32                      ! exp > (BITS_PER_DI - BITS_PER_SI)
        SUBLT   D0Re0, D0Re0, D0Re0             ! < overflow
        SUBLT   D0Re0, D0Re0, #1
        MOVLT   PC, D1RtP                       ! return MAX_USI_INT

        SUB     D0Ar6, D0Ar6, #32               ! exp -= (BITS_PER_DI - BITS_PER_SI)

        MOV     D0Re0, D1Ar1
        LSR     D0Re0, D0Re0, D0Ar6             ! return mant >> exp
        MOV     PC, D1RtP
        .size   ___fixunsdfsi,.-___fixunsdfsi
#endif

#ifdef L_fixunssfdi
!!
!! Floating point - float -> unsigned long long
!!
        .text
        .global ___fixunssfdi
        .type   ___fixunssfdi,function
        .align  2
___fixunssfdi:
        LSLS    D0Re0, D1Ar1, #1                ! Ignore sign bit
        MOVZ    PC, D1RtP                       ! Zero? return 0

        TSTT    D1Ar1, #0x8000                  ! Negative?
        SUBNZ   D0Re0, D0Re0, D0Re0             ! < 0
        MOVNZ   D1Re0, D0Re0
        MOVNZ   PC, D1RtP                       ! return 0

        LSR     D0Ar6, D1Ar1, #23               ! extract exponent (D0Ar6)
        SUBS    D0Ar6, D0Ar6, #(127+63)

        SUBGT   D0Re0, D0Re0, D0Re0             ! exp > 0 (overflow)
        SUBGT   D0Re0, D0Re0, #1
        MOVGT   D1Re0, D0Re0
        MOVGT   PC, D1RtP                       ! return MAX_UDI

        CMP     D0Ar6, #-(64-1)                 ! -exp > (BITS_PER_DI - 1)

        SUBLT   D0Re0, D0Re0, D0Re0             ! underflow
        MOVLT   D1Re0, D0Re0
        MOVLT   PC, D1RtP                       ! return 0

        LSL     D1Re0, D1Ar1, #(63-32-23)       ! extract mantisa MSWord
        ORT     D1Re0, D1Re0, #0x8000           ! add hidden bit

        CMP     D0Ar6, #-32
        NEG     D0Ar4, D0Ar6
        MOV     D1Ar3, D0Ar4
        BLE     $L1

!! Shift < 32 bits

        ORS     D0Re0, D1Ar3, D1Ar3             ! shifting 0 ?
        MOVZ    PC, D1RtP                       !

        ADD     D1Ar5, D0Ar6, #32               ! (-exp) + 32

        LSL     D0Re0, D1Re0, D1Ar5             ! mant >> exp
        LSR     D1Re0, D1Re0, D1Ar3

        MOV     PC, D1RtP
$L1:
!! Shift > 32 bits
        SUB     D1Ar3, D1Ar3, #32               ! exp -= 32

        LSR     D0Re0, D1Re0, D1Ar3
        MOV     D1Re0, #0

        MOV     PC, D1RtP
        .size   ___fixunssfdi,.-___fixunssfdi
#endif

#ifdef L_fixunssfsi
!!
!! Floating point - float -> unsigned int/long
!!
        .text
        .global ___fixunssfsi
        .type   ___fixunssfsi,function
        .align  2
___fixunssfsi:
        LSLS    D0Re0, D1Ar1, #1                ! Ignore sign bit
        MOVZ    PC, D1RtP                       ! Zero? return 0

        TSTT    D1Ar1, #0x8000                  ! Negative?
        SUBNZ   D0Re0, D0Re0, D0Re0             ! < 0
        MOVNZ   PC, D1RtP                       ! return 0

        LSR     D0Ar6, D1Ar1, #23               ! extract exponent (D0Ar6)
        SUBS    D0Ar6, D0Ar6, #(127+31)

        SUBGT   D0Re0, D0Re0, D0Re0             ! exp > 0 (overflow)
        SUBGT   D0Re0, D0Re0, #1
        MOVGT   PC, D1RtP                       ! return MAX_USI

        CMP     D0Ar6, #-(32-1)                 ! -exp > (BITS_PER_SI - 1)

        SUBLT   D0Re0, D0Re0, D0Re0             ! underflow
        MOVLT   PC, D1RtP                       ! return 0

        LSL     D0Re0, D1Ar1, #(31-23)          ! extract mantisa
        ORT     D0Re0, D0Re0, #0x8000           ! add hidden bit

        NEG     D0Ar6, D0Ar6

        LSR     D0Re0, D0Re0, D0Ar6             ! mant >> exp

        MOV     PC, D1RtP
        .size   ___fixunssfsi,.-___fixunssfsi
#endif

!! Integer -> Floating point conversion

#ifdef L_floatdidf
!!
!! signed long long -> double conversion
!!
        .text
        .global ___floatdidf
        .type   ___floatdidf,function
        .align  2
___floatdidf:

        MOV     D1Ar3, #(1023+32+30)    ! exp
        MOV     D0Ar6, #0               ! sign, assume +ve

        ORS     D1Re0, D1Ar1, D1Ar1
        BLT     $L1
        BGT     $L2

        ORS     D0Re0, D0Ar2, D0Ar2
        BNE     $L2

        MOV     PC, D1RtP

$L1:
        !       <0
        MOVT    D0Ar6, #0x8000          ! sign, -ve
        
        NEGS    D0Ar2, D0Ar2            ! negate
        NEG     D1Re0, D1Ar1
        SUBNZ   D1Re0, D1Re0, #1

        CMP     D1Re0, #0               ! negative?
        BGT     $L2                     ! branch, not MIN DI

! Handle most negative value

        LSR     D0Ar2, D0Ar2, #1        ! significand >>= 1
        LSL     D0Ar4, D1Re0, #(32-1)
        OR      D0Ar2, D0Ar2, D0Ar4
        LSR     D1Re0, D1Re0, #1
        
        ADD     D1Ar3, D1Ar3, #1        ! exp += 1

$L2:
        MOV     D0Re0, D0Ar2            ! mantisa
        NORM    D1Ar1, D1Re0            ! normalize up
        BZ      $L4                     ! MSWord zero

        CMP     D1Ar1, #0               ! Already normalised ?
        BZ      $L5                     ! yes, skip normalisation

        MOV     D0Ar4, D1Re0
        FFB     D0Ar4, D0Ar4

        LSL     D1Re0, D1Re0, D1Ar1
        ADD     D0Ar4, D0Ar4, #2
        LSR     D1Ar5, D0Re0, D0Ar4
        OR      D1Re0, D1Re0, D1Ar5
        MOV     D0Ar4, D1Ar1
        LSL     D0Re0, D0Re0, D0Ar4

        SUB     D1Ar3, D1Ar3, D1Ar1
        B       $L5

$L4:
        SWAP    D1Re0, D0Re0            ! D1Re0 <- D0Re0, D0Re0 <- #0
        SUB     D1Ar3, D1Ar3, #32

        NORM    D1Ar1, D1Re0
        LSRLT   D1Re0, D1Re0, #1        ! Watch out for low 32-bits with MSbit set
        ADDLT   D1Ar3, D1Ar3, #1        ! adjust exp and mantissa

        LSL     D1Re0, D1Re0, D1Ar1

        SUB     D1Ar3, D1Ar3, D1Ar1

$L5:
        TST     D0Re0, #(1<<9)          ! Test MSbit discared
        LSR     D0Re0, D0Re0, #10       ! Discard excess bits not representable
        LSL     D0Ar4, D1Re0, #(32 - 10)
        LSR     D1Re0, D1Re0, #10
        OR      D0Re0, D0Re0, D0Ar4
        BZ      $L7

        ADDS    D0Re0, D0Re0, #1        ! Round up if MSbit discarded non-zero
        ADDCS   D1Re0, D1Re0, #1

$L6:
        TSTT    D1Re0, #0xFFE0          ! Round up overflowed?
        BZ      $L7

        ADD     D1Ar3, D1Ar3, #1        ! yes, adjust exp +=1
        LSR     D0Re0, D0Re0, #1        ! and adjust significand shifting >>1
        LSL     D0Ar4, D1Re0, #(32 - 1)
        LSR     D1Re0, D1Re0, #1
        OR      D0Re0, D0Re0, D0Ar4
        B       $L6

$L7:
        ANDMT   D1Re0, D1Re0, #0x000F   ! remove hidden bit <- mantisa MSword

        LSL     D1Ar3, D1Ar3, #(52-32)  ! position exp in result
        MOV     D1Ar1, D0Ar6            ! sign
        OR      D1Re0, D1Re0, D1Ar1     ! combine with sign -> sign | mant
        OR      D1Re0, D1Re0, D1Ar3     ! combine with exp -> sign | exp | mant

        MOV     PC, D1RtP
        .size   ___floatdidf,.-___floatdidf
#endif

#ifdef L_floatdisf
!!
!!  signed signed long long -> float conversion
!!
        .text
        .global ___floatdisf
        .type   ___floatdisf,function
___floatdisf:

        MOV     D0Ar6, #(127+32+30)     ! exp
        MOV     D0Re0, #0               ! sign

        CMP     D1Ar1, #0
        BLT     $L1
        BGT     $L2

        CMP     D0Ar2, #0
        BNE     $L2

        MOV     PC, D1RtP
$L1:
        !       <0
        MOVT    D0Re0, #0x8000

        NEGS    D0Ar2, D0Ar2            ! negate
        NEG     D1Ar1, D1Ar1
        SUBNZ   D1Ar1, D1Ar1, #1

        CMP     D1Ar1, #0               ! deal min DI
        BGT     $L2                     ! branch not min DI

        LSR     D0Ar2, D0Ar2, #1        ! significand >> 1
        MOV     D0Ar4, D1Ar1
        LSL     D0Ar4, D0Ar4, #(32-1)
        OR      D0Ar2, D0Ar2, D0Ar4
        LSR     D1Ar1, D1Ar1, #1

        ADD     D0Ar6, D0Ar6, #1        ! exp += 1

$L2:                                    ! normalize up
        NORM    D1Re0, D1Ar1
        BZ      $L4                     ! MSWord zero

        CMP     D1Re0, #0               ! Already normalised ?
        BZ      $L5                     ! yes, skip normalisation

        MOV     D0Ar4, D1Ar1
        FFB     D0Ar4, D0Ar4

        LSL     D1Ar1, D1Ar1, D1Re0
        ADD     D0Ar4, D0Ar4, #2
        LSR     D1Ar5, D0Ar2, D0Ar4
        OR      D1Ar1, D1Ar1, D1Ar5
        MOV     D0Ar4, D1Re0
        LSL     D0Ar2, D0Ar2, D0Ar4

        MOV     D0Ar4, D1Re0
        SUB     D0Ar6, D0Ar6, D0Ar4
        B       $L5

$L4:
        SWAP    D1Ar1, D0Ar2            ! D1Ar1 <- D0Ar2, D0Ar2 <- #0
        SUB     D0Ar6, D0Ar6, #32

        NORM    D1Re0, D1Ar1
        LSL     D1Ar1, D1Ar1, D1Re0

        MOV     D0Ar4, D1Re0
        SUB     D0Ar6, D0Ar6, D0Ar4

$L5:
        MOV     D0Ar2, D1Ar1
        TST     D0Ar2, #(1<<6)          ! Round before discarding
        LSR     D0Ar2, D0Ar2, #7        ! Discard excess bits not representable
        ADDNZ   D0Ar2, D0Ar2, #1

$L6:
        TSTT    D0Ar2, #0xFF00          ! Round up overflowed?
        BZ      $L7

        ADD     D0Ar6, D0Ar6, #1        ! yes, adjust exp += 1
        LSR     D0Ar2, D0Ar2, #1        ! adn adjust significand shifting >>1
        B       $L6

$L7:
        ANDMT   D0Ar2, D0Ar2, #0x007F   ! remove hidden bit <- mantisa

        LSL     D0Ar6, D0Ar6, #23       ! position exp in result
        OR      D0Re0, D0Re0, D0Ar6     ! combine sign with exp -> sign|exp
        OR      D0Re0, D0Re0, D0Ar2     ! combine sign|exp with mant -> sign|exp|mant

        MOV     PC, D1RtP
        .size   ___floatdisf,.-___floatdisf
#endif

#ifdef L_floatsidf
!!
!! signed int/long -> double conversion
!!
        .text
        .global ___floatsidf
        .type   ___floatsidf,function
        .align  2
___floatsidf:

        MOV     D1Ar5, #(1023+30)       ! exp
        MOV     D1Re0, #0               ! assume result +ve
        CMP     D1Ar1, #0               ! num < 0
        BGT     $L2

        ! <= 0
        MOVEQ   D0Re0, D1Re0
        MOVEQ   PC, D1RtP               ! num == 0 return 0

        ! < 0
        MOVT    D1Re0, #0x8000          ! result will be -ve
        NEG     D1Ar1, D1Ar1            ! num -= num
        CMP     D1Ar1, #0               ! num < 0
        BGT     $L2

        ! < 0 (num == MIN_INT)
        LSR     D1Ar1, D1Ar1, #1        ! num >>= 1
        ADD     D1Ar5, D1Ar5, #1        ! exp += 1

$L2:
        NORM    D1Ar3, D1Ar1            ! Make MSBit non-zero
        LSL     D1Ar1, D1Ar1, D1Ar3     ! num <<= N
        SUB     D1Ar5, D1Ar5, D1Ar3     ! exp -= N

        MOV     D0Re0, D1Ar1            ! position mantisa LSWord
        LSL     D0Re0, D0Re0, #(32-10)

        LSL     D1Ar5, D1Ar5, #(52-32)  ! position exponent
        OR      D1Re0, D1Re0, D1Ar5     ! combine sign | exp

        LSR     D1Ar1, D1Ar1, #10       ! position significand MSWord
        ANDMT   D1Ar1, D1Ar1, #0x000F   ! discard hidden bit
        OR      D1Re0, D1Re0, D1Ar1     ! combine sign | exp | mantisa

        MOV     PC, D1RtP
        .size   ___floatsidf,.-___floatsidf
#endif

#ifdef L_floatunsidf
!!
!! unsigned int/long -> double conversion
!!
        .text
        .global ___floatunsidf
        .type   ___floatunsidf,function
        .align  2
___floatunsidf:

        MOV     D1Ar5, #(1023+30)       ! exp
        MOV     D1Re0, #0               ! assume result +ve
        CMP     D1Ar1, #0               ! num == 0

        ! == 0
        MOVEQ   D0Re0, D1Re0
        MOVEQ   PC, D1RtP               ! num == 0 return 0

        NORM    D1Ar3, D1Ar1            ! Make MSBit non-zero
        LSL     D1Ar1, D1Ar1, D1Ar3     ! num <<= N
        SUB     D1Ar5, D1Ar5, D1Ar3     ! exp -= N

        MOV     D0Re0, D1Ar1            ! position mantisa LSWord
        LSL     D0Re0, D0Re0, #(32-10)

        LSL     D1Ar5, D1Ar5, #(52-32)  ! position exponent
        OR      D1Re0, D1Re0, D1Ar5     ! combine sign | exp

        LSR     D1Ar1, D1Ar1, #10       ! position significand MSWord
        ANDMT   D1Ar1, D1Ar1, #0x000F   ! discard hidden bit
        OR      D1Re0, D1Re0, D1Ar1     ! combine sign | exp | mantisa

        MOV     PC, D1RtP
        .size   ___floatunsidf,.-___floatunsidf
#endif

#ifdef L_floatsisf
!!
!! signed int/long -> float conversion
!!
        .text
        .global ___floatsisf
        .type   ___floatsisf,function
        .align  2
___floatsisf:
        MOV     D0Ar6, #(127+32-2)      ! exp
        MOV     D1Re0, #0               ! sign
        ORS     D0Re0, D1Ar1, D1Ar1     ! num ? -ve, zero, +ve
        BGT     $L2                     ! branch, +ve
        ! <=
        MOVZ   PC, D1RtP                ! zero, return 0

        ! < 0
        MOVT    D1Re0, #0x8000          ! sign
        NEGS    D1Ar1, D1Ar1            ! Make >= 0
        BGT     $L2                     ! branch, +ve

        ! Handle MIN INT
        LSR     D1Ar1, D1Ar1, #1        ! num >>= 1
        ADD     D0Ar6, D0Ar6, #1        ! exp += 1

$L2:                                    ! num > 0
        NORM    D1Ar3, D1Ar1            ! Shift up N bits
        LSL     D1Ar1, D1Ar1, D1Ar3     ! num <<= N
        MOV     D0Ar4, D1Ar3
        SUB     D0Ar6, D0Ar6, D0Ar4     ! exp -= N

        LSR     D1Ar1, D1Ar1, #(8-2)
        ADD     D1Ar1, D1Ar1, #1        ! round
        MOV     D0Re0, D1Ar1            ! mantisa --> D0Re0

        TSTT    D1Ar1, #0xFE00          ! rounding overflowed ?
        ADDNZ   D0Ar6, D0Ar6, #1        ! yes, adjust exp++
        LSR     D0Re0, D0Re0, #1        ! position mantisa

        LSL     D0Ar6, D0Ar6, #23       ! position exponent
        ANDMT   D0Re0, D0Re0, #0x007F   ! remove hidden bit
        OR      D0Re0, D0Re0, D0Ar6     ! man | exp

        MOV     D0Ar6, D1Re0
        OR      D0Re0, D0Re0, D0Ar6     ! |sign

        MOV     PC, D1RtP
        .size   ___floatsisf,.-___floatsisf
#endif

#ifdef L_floatunsisf
!!
!! signed int/long -> float conversion
!!
        .text
        .global ___floatunsisf
        .type   ___floatunsisf,function
        .align  2
___floatunsisf:
        MOV     D0Ar6, #(127+32-2)      ! exp
        ORS     D0Re0, D1Ar1, D1Ar1     ! num ? -ve, zero, +ve
        ! <=
        MOVZ   PC, D1RtP                ! zero, return 0

        NORM    D1Ar3, D1Ar1            ! Shift up N bits
        LSL     D1Ar1, D1Ar1, D1Ar3     ! num <<= N
        MOV     D0Ar4, D1Ar3
        SUB     D0Ar6, D0Ar6, D0Ar4     ! exp -= N

        LSR     D1Ar1, D1Ar1, #(8-2)
        ADD     D1Ar1, D1Ar1, #1        ! round
        MOV     D0Re0, D1Ar1            ! mantisa --> D0Re0

        TSTT    D1Ar1, #0xFE00          ! rounding overflowed ?
        ADDNZ   D0Ar6, D0Ar6, #1        ! yes, adjust exp++
        LSR     D0Re0, D0Re0, #1        ! position mantisa

        LSL     D0Ar6, D0Ar6, #23       ! position exponent
        ANDMT   D0Re0, D0Re0, #0x007F   ! remove hidden bit
        OR      D0Re0, D0Re0, D0Ar6     ! man | exp

        MOV     PC, D1RtP
        .size   ___floatunsisf,.-___floatunsisf
#endif

#ifdef L_unordsf2
        .text
        .global ___unordsf2
        .type   ___unordsf2,function
        .align  2
___unordsf2:
        MOV     PC, D1RtP
#endif

#ifdef L_unorddf2
        .text
        .global ___unorddf2
        .type   ___unorddf2,function
        .align  2
___unorddf2:
        MOV     PC, D1RtP
        .size   ___unorddf2,.-___unorddf2
#endif

