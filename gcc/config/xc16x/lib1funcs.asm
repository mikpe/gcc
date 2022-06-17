;; GCC Library functions for Infineon xc16x
;; Copyright (C)  2006 Free Software Foundation, Inc.

;; Contributed by Shrirang Khisti on behalf of KPIT Cummins Infosystems Ltd.

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; GCC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING.  If not, write to
;; the Free Software Foundation, 51 Franklin Street, Fifth Floor,
;; Boston, MA 02110-1301, USA.

;Function For unsigned division in SI mode (32 bits)

.section .text
.align 2
.global ___udivsi3
___udivsi3:

;Temporary 
	mov [-r0],r2
	prior r2,r11
	jmpr cc_NZ,.l1
	mov MDL,r9
	divu r10
	mov r2,MDH
	mov r9,MDL
	mov MDH,r2
	mov MDL,r8
	divlu r10  
	mov r8,MDL
	mov r4,r8
	mov r5,r9
	mov r2,[r0+]

	#ifdef __xc16xL__   
	   rets
	#else 
	   ret
	#endif   
.l1:
	mov [-r0],r9
	mov [-r0],r8
	mov [-r0],r10
	mov [-r0],r11
	cmp r2,#0
	jmpr cc_NZ ,.l2
	mov r8,r9
	mov r9,#0x0
	jmpr cc_UC,.l3
.l2:
	shl r11,r2
	neg r2
	shr r10,r2
	or r11,r10
	mov r10,r9
	shr r9,r2
	shr r8,r2
	neg r2
	shl r10,r2
	or r8,r10
.l3:
	mov MDH,r9
	mov MDL,r8
	divlu r11
	mov r8,MDL
	mov r11,[r0+]
	mov r10,[r0+]
	mulu r8,r11
	mov r9,MDL
	mulu r8,r10
	add r9,MDH
	mov r11,MDL
	jmpr cc_NC,.l4
	add r0,#0x4
	jmpr cc_UC,.l5
.l4:
	sub r11,[r0+]
	subc r9,[r0+]
	jmpr cc_ULE,.l6
.l5: 
	sub r8,#0x1
.l6: 
	mov r9,#0x0
        mov r5,r9
        mov r4,r8
        mov r2,[r0+]

	#ifdef __xc16xL__   
	   rets
	#else 
	   ret
	#endif   
     
;Function For signed division in SI mode (32 bits)

.align 2
.global ___divsi3

___divsi3:
	mov [-r0],r1
	mov r1,r9
	jmpr cc_NN,.dl1
	neg r9
	neg r8
	subc r9,#0x0
.dl1:
	jnb r11.15,.dl2
	cpl r1
	neg r11
	neg r10
	subc r11,#0x0
.dl2: 
	#ifdef __xc16xL__
	calls #SEG:___udivsi3,#SOF:___udivsi3
	#else
;calla cc_UC,___umodsi3
	calla cc_UC,___udivsi3
	#endif
	jnb r1.15,.dl3
	neg r5
	neg r4
	subc r5,#0x0
.dl3:
	mov r1,[r0+]

	#ifdef __xc16xL__   
	   rets
	#else 
	   ret
	#endif   
    
 ;Function For calculating signed mod in SI mode (32 bits)   

.align 2
.global ___modsi3
___modsi3:
	mov [-r0],r1
	mov r1,r9
	jmpr cc_NN,.ml1
	neg r9
	neg r8
	subc r9,#0x0
.ml1:
	jnb r11.15,.ml2
	neg r11
	neg r10
	subc r11,#0x0
.ml2:
	#ifdef __xc16xL__
	calls #SEG:___umodsi3,#SOF:___umodsi3
	#else
	calla cc_UC,___umodsi3
	#endif
	jnb r1.15,.ml3
	neg r5
	neg r4
	subc r5,#0x0
.ml3: 
	mov r1,[r0+]
	#ifdef __xc16xL__   
	   rets
	#else 
	   ret
	#endif   

;Function For calculating unsigned mod in SI mode (32 bits)   

.align 2
.global ___umodsi3

___umodsi3:
	mov [-r0],r2
	prior r2,r11
	jmpr cc_NZ,.uml1
	mov MDL,r9
	divu r10
	mov MDL,r8
	divlu r10
	mov r8,MDH
	mov r9,MDL
	mov r9,#0x0
	mov r5,r9
	mov r4,r8
	mov r2,[r0+]

	#ifdef __xc16xL__   
	   rets
	#else 
	   ret
	#endif   

.uml1:
	mov [-r0],r9
	mov [-r0],r8
	mov [-r0],r10
    	mov [-r0],r11
    	cmp r2,#0x0
    	jmpr cc_NZ,.uml2
    	mov r8,r9
    	mov r9,#0x0
    	jmpr cc_UC,.uml3

.uml2:
	shl r11,r2
    	neg r2
	shr r10,r2
    	or r11,r10
    	mov r10,r9
    	shr r9,r2
    	shr r8,r2
    	neg r2
    	shl r10,r2
    	or r8,r10
    
.uml3:
	mov MDH,r9
    	mov MDL,r8
    	divlu r11
    	mov r8,MDL
    	mov r11,[r0+]
    	mov r10,[r0+]
    	mulu r8,r11
    	mov r9,MDL
    	mulu r8,r10
    	add r9,MDH
    	mov r8,MDL
    	jmpr cc_NC,.uml4
    	sub r8,r10
    	subc r9,r11

.uml4:
	sub r8,[r0+]
    	subc r9,[r0+]
    	jmpr cc_ULE,.uml5
    	sub r8,r10
    	subc r9,r11
    	
.uml5:
	neg r9
    	neg r8
    	subc r9,#0x0
    	mov r5,r9
    	mov r4,r8
    	mov r2,[r0+]
	
	#ifdef __xc16xL__   
	   rets
	#else 
	   ret
	#endif   

;Function For Logical Shift right  in SI mode (32 bits)   

.global ___lshrsi3
___lshrsi3:
	JMPr	cc_UC,.l0003
.l0002:
	SHR	R8,#1
	BMOV	R8.15,R9.0
	SHR	R9,#1
.l0003:
	CMPD1	R10,#0
	JMPr	cc_NE,.l0002
	mov r4,r8
	mov r5,r9

	#ifdef __xc16xL__   
	   rets
	#else 
	   ret
	#endif   

;Function For Arithmatic Shift left  in SI mode (32 bits)   

.global ___ashlsi3
___ashlsi3:

	JMPr	cc_UC,.l0013
.l0012:
	add r8,r8
	addc r9,r9
.l0013:
	CMPD1	R10,#0
	JMPr	cc_NE,.l0012
	mov r4,r8
	mov r5,r9

	#ifdef __xc16xL__   
	   rets
	#else 
	   ret
	#endif   

;Function For Arithmatic Shift right  in SI mode (32 bits)   
#if 0
.global ___ashrsi3
___ashrsi3:
	cmp r10,#0
	jmpr cc_Z,.rt1
	jbc r10.4,.rt2
	mov r11,r9
	shr r8,r10
	ashr r9,r10
	neg r10
	shl r11,r10
	or r8,r11
	jmpr cc_UC,.rt1
.rt2: 
	mov r8,r9
	ashr r9,#0x0f
	ashr r8,r10
.rt1:
	mov r4,r8
	mov r5,r9

	#ifdef __xc16xL__
	   rets
	#else
	   ret
	#endif	
	#endif

.global ___ashrsi3
___ashrsi3:
	JMPr	cc_UC,.l1003
.l1002:
	SHR	R8,#1
	BMOV	R8.15,R9.0
	ASHR	R9,#1
.l1003:
	CMPD1	R10,#0
	JMPr	cc_NE,.l1002
	mov r4,r8
	mov r5,r9

	#ifdef __xc16xL__   
	   rets
	#else 
	   ret
	#endif   
