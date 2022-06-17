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

#ifdef __MAXQ20__
	.text

;struct IntType
;{
;       int low;
;       int high;
;};
;
;union LongType
;{
;       long long val;
;       struct IntType Int;
;};

;Algorithm used for implementing __mulsi
;long long __mulsi3(long long  op1, long long  op2)
;{
;       int i;
;       long long result=0;
;       for(i=0; i< sizeof(long long)*8;i++)
;       {
;               if(op1&1)
;                       result+=op2;
;               op1>>=1;
;               op2<<=1;
;       }
;       return result;
;}
;Similar logic applies for MAXQ10 also

#ifdef L_muldi3
	.align	1
	.global	___mulsi3
___mulsi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#8
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[12]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[13]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[14]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, LC[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, LC[1]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[14], #0
	move	LC[0], A[14]
	move	GR, A[14]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[14]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[6], A[0]
	move	A[7], A[1]
	move	A[4], A[2]
	move	A[5], A[3]
	move	LC[1], #32
L2:
	move	ACC, A[6]
	and	#1
	move	A[11], #0
	sub	A[11]
	jump	z, L3
	move	ACC, A[14]
	add	A[4]
	move	A[14], ACC
	move	ACC, LC[0]
	addc	A[5]
	move	LC[0], ACC
L3:
	move	A[2], #1
	move	A[0], A[6]
	move	A[1], A[7]
	call	___ashrsi3
	move	A[6], A[0]
	move	A[7], A[1]
	move	ACC, A[4]
	add	A[4]
	move	A[12], ACC
	move	ACC, A[5]
	addc	A[5]
	move	A[4], A[12]
	move	A[5], ACC
	move	ACC, LC[1]
	add	#-1
	move	LC[1], ACC
	sub	#0
	jump	nz, L2
	move	GR, A[14]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, LC[0]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[0], A[14]
	move	A[1], LC[0]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	LC[1], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	LC[0], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[14], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[13], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[12], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#8
	move	DP[1], ACC
	ret	
#endif

;Algorithm used for implementing __mulhi3
;int __mulhi3(int op1, int op2)
;{
;        int i;
;        int result=0;
;        for(i=0; i< sizeof(int)*8;i++)
;        {
;                if(op1&1)
;                        result+=op2;
;                op1>>=1;
;                op2<<=1;
;        }
;        return result;
;}
;Similar logic applies for MAXQ10 also

#ifdef L_mulhi3

	.align	1
	.global	___mulhi3
___mulhi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[4], #0
	move	A[3], #16
L10:
	move	ACC, A[0]
	and	#1
	sub	#0
	jump	z, L11
	move	ACC, A[4]
	add	A[1]
	move	A[4], ACC
L11:
	move	ACC, A[0]
	sra	
	move	A[0], ACC
	move	ACC, A[1]
	sla	
	move	A[1], ACC
	move	ACC, A[3]
	add	#-1
	move	A[3], ACC
	sub	#0
	jump	nz, L10
	move	A[0], A[4]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;Algorithm used for implementing __negsi2
;long long __negsi2(long long op)
;{
;   return ~op + 1;
;}
;Similar logic applies for MAXQ10 also

#ifdef L_negdi2

	.align	1
	.global	___negsi2
___negsi2:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	ACC, A[0]
	cpl	
	move	A[4], ACC
	move	ACC, A[1]
	cpl	
	move	A[5], ACC
	move	ACC, A[4]
	add	#1
	move	A[6], ACC
	move	ACC, A[5]
	addc	#0
	move	A[0], A[6]
	move	A[1], ACC
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;Algorithm for implementing __ashrsi3
;long long __ashrsi3(long long op, unsigned int n)
;{
;       int mask;
;       int hival;
;       union LongType x;
;       const int  size = sizeof(int) * 8;
;       if(!n)
;               return op;
;       x.val = op;
;
;        if(n>= size-1 )
;        {
;                x.Int.low = x.Int.high >> (n-size);
;                x.Int.high>>=size-1;
;                return x.val;
;        }
;
;       mask = (1 <<n) - 1;
;       hival = x.Int.high & mask;
;       hival <<= (size- n);
;       mask <<= (size - n);
;       hival = x.Int.high & mask;
;       hival <<= (size- n);
;       mask <<= (size - n);
;
;       x.Int.low >>= n;
;       x.Int.high >>= n;
;       x.Int.low &= (~mask);
;       x.Int.low |= hival;
;
;       return x.val;
;}
;Similar logic applies for MAXQ10 also

#ifdef L_ashrdi3

	.align	1
	.global	___ashrsi3
___ashrsi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[12]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[13]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[6], #0
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[10], A[2]
	move	ACC, A[2]
	sub	A[6]
	jump	z, L20
	move	ACC, #14
	sub	A[2]
	jump	nc, L22
	jump	z, L22
	move	ACC, A[2]
	add	#-16
	move	A[1], ACC
	move	A[0], A[5]
	call	___ashrhi3
	move	ACC, A[5]
	sra4	
	sra4	
	sra4	
	sra2	
	sra	
	move	A[4], A[0]
	move	A[5], ACC
	jump	L20
L22:
	move	A[1], A[2]
	move	A[0], #1
	call	___ashlhi3
	move	ACC, A[0]
	add	#-1
	move	A[12], ACC
	move	ACC, #16
	sub	A[10]
	move	A[11], ACC
	move	ACC, A[12]
	and	A[5]
	move	A[1], A[11]
	move	A[0], ACC
	call	___ashlhi3
	move	A[13], A[0]
	move	A[1], A[10]
	move	A[0], A[4]
	call	___ashrhi3
	move	A[6], A[0]
	move	A[1], A[10]
	move	A[0], A[5]
	call	___ashrhi3
	move	A[7], A[0]
	move	A[1], A[11]
	move	A[0], A[12]
	call	___ashlhi3
	move	ACC, A[0]
	cpl	
	and	A[6]
	move	A[10], ACC
	move	ACC, A[13]
	or	A[10]
	move	A[4], ACC
	move	A[5], A[7]
L20:
	move	A[0], A[4]
	move	A[1], A[5]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[13], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[12], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;Algorithm for implementing __lshrsi3
;long long __lshrsi3(long long op, unsigned int n)
;{
;        int mask;
;        unsigned int hival;
;        union LongType x;
;        const int  size = sizeof(int) * 8;
;        if(!n)
;                return op;
;       x.val = op;
;        if(n>= size-1 )
;        {
;                hival = x.Int.high;
;                x.Int.low = hival >> (n-size);
;                x.Int.high=0;
;                return x.val;
;        }
;
;        mask = (1 <<n) - 1;
;        hival = x.Int.high & mask;
;        hival <<= (size - n);
;        mask <<= (size - n);
;
;       x.Int.low >>= n;
;       x.Int.high >>= n;
;        x.Int.low &= (~mask);
;       x.Int.high &= (~mask);
;        x.Int.low |= hival;
;
;        return x.val;
;}
;Similar logic applies for MAXQ10 also

#ifdef L_lshrdi3

	.align	1
	.global	___lshrsi3
___lshrsi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[12]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[13]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[4], #0
	move	A[8], A[4]
	move	A[6], A[0]
	move	A[7], A[1]
	move	A[10], A[2]
	move	ACC, A[2]
	sub	A[4]
	jump	z, L26
	move	ACC, #14
	sub	A[2]
	jump	nc, L28
	jump	z, L28
	move	ACC, A[2]
	add	#-16
	move	A[1], ACC
	move	A[0], A[7]
	call	___lshrhi3
	move	A[6], A[0]
	move	A[7], A[8]
	jump	L26
L28:
	move	A[1], A[2]
	move	A[0], #1
	call	___ashlhi3
	move	ACC, A[0]
	add	#-1
	move	A[12], ACC
	move	ACC, #16
	sub	A[10]
	move	A[11], ACC
	move	ACC, A[12]
	and	A[7]
	move	A[1], A[11]
	move	A[0], ACC
	call	___ashlhi3
	move	A[13], A[0]
	move	A[1], A[10]
	move	A[0], A[6]
	call	___ashrhi3
	move	A[4], A[0]
	move	A[1], A[10]
	move	A[0], A[7]
	call	___ashrhi3
	move	A[5], A[0]
	move	A[1], A[11]
	move	A[0], A[12]
	call	___ashlhi3
	move	ACC, A[0]
	cpl	
	move	A[10], ACC
	and	A[4]
	move	A[4], ACC
	move	ACC, A[10]
	and	A[5]
	move	A[5], ACC
	move	ACC, A[13]
	or	A[4]
	move	A[6], ACC
	move	A[7], A[5]
L26:
	move	A[0], A[6]
	move	A[1], A[7]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[13], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[12], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;Algorithm for implementing __ashlsi3
;long long __ashlsi3(long long op, int n)
;{
;        int mask;
;        unsigned int lowval;
;        union LongType x;
;        const int  size = sizeof(int) * 8;
;       x.val = op;
;        if(!n)
;                return op;
;        if(n>= size-1 )
;        {
;                x.Int.high = x.Int.low << (n-size);
;                x.Int.low=0;
;                return x.val;
;        }
;
;        mask = (1 <<n) - 1;
;       mask <<= (size - n);
;        lowval = x.Int.low & mask;
;        lowval >>= (size -n);
;
;       x.Int.low <<= n;
;       x.Int.high <<= n;
;        x.Int.high |= lowval;
;
;        return x.val;
;}
;Similar logic applies for MAXQ10 also

#ifdef L_ashldi3

	.align	1
	.global	___ashlsi3
___ashlsi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[12]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[6], #0
	move	A[8], A[6]
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[10], A[2]
	move	ACC, A[2]
	sub	A[8]
	jump	z, L32
	move	ACC, A[2]
	add	#-32768
	move	A[0], ACC
	move	ACC, #-32754
	sub	A[0]
	jump	nc, L34
	jump	z, L34
	move	ACC, A[2]
	add	#-16
	move	A[1], ACC
	move	A[0], A[4]
	call	___ashlhi3
	move	A[4], A[8]
	move	A[5], A[0]
	jump	L32
L34:
	move	ACC, #16
	sub	A[2]
	move	A[12], ACC
	move	A[1], A[2]
	move	A[0], A[4]
	call	___ashlhi3
	move	A[6], A[0]
	move	A[1], A[10]
	move	A[0], #1
	call	___ashlhi3
	move	ACC, A[0]
	add	#-1
	move	A[1], A[12]
	move	A[0], ACC
	call	___ashlhi3
	move	ACC, A[0]
	and	A[4]
	move	A[1], A[12]
	move	A[0], ACC
	call	___lshrhi3
	move	A[11], A[0]
	move	A[1], A[10]
	move	A[0], A[5]
	call	___ashlhi3
	move	ACC, A[11]
	or	A[0]
	move	A[4], A[6]
	move	A[5], ACC
L32:
	move	A[0], A[4]
	move	A[1], A[5]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[12], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;Algorithm for implementing __lshrhi3
;int __lshrhi3(int op,  int n)
;{
;        int i,m;
;       int mask;
;        const int  size = sizeof(int) * 8;
;        if(n>=32)
;                return 0;
;
;        mask = (1 <<n) - 1;
;        mask <<= (size - n);
;
;       m=n>>2;
;        for(i=0;i<m;i++)
;                op>>=4;
;        if(n&2)
;                op>>=2;
;        if(n&1)
;                op>>=1;
;
;        return (op & ~mask);
;}
;Similar logic applies for MAXQ10 also

#ifdef L_lshrhi3

	.align	1
	.global	___lshrhi3
___lshrhi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[4], A[0]
	move	A[5], A[1]
	move	ACC, A[1]
	add	#-32768
	move	A[0], ACC
	move	ACC, #-32737
	sub	A[0]
	jump	nc, L38
	jump	z, L38
	move	A[0], #0
	jump	L40
L38:
	move	A[0], #1
	call	___ashlhi3
	move	ACC, A[0]
	add	#-1
	move	A[0], ACC
	move	ACC, #16
	sub	A[5]
	move	A[1], ACC
	call	___ashlhi3
	move	ACC, A[5]
	sra2	
	move	A[3], ACC
	add	#-32768
	move	A[1], ACC
	move	ACC, #-32768
	sub	A[1]
	jump	nc, L41
	jump	z, L41
	move	A[2], #0
L43:
	move	ACC, A[4]
	sra4	
	move	A[4], ACC
	move	ACC, A[2]
	add	#1
	move	A[2], ACC
	move	ACC, A[3]
	add	#-32768
	move	A[1], ACC
	move	ACC, A[2]
	add	#-32768
	sub	A[1]
	jump	c, L43
L41:
	move	ACC, A[5]
	and	#2
	sub	#0
	jump	z, L44
	move	ACC, A[4]
	sra2	
	move	A[4], ACC
L44:
	move	ACC, A[5]
	and	#1
	sub	#0
	jump	z, L46
	move	ACC, A[4]
	sra	
	move	A[4], ACC
L46:
	move	ACC, A[0]
	cpl	
	move	A[0], ACC
	move	ACC, A[4]
	and	A[0]
	move	A[0], ACC
L40:
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;Algorithm for implementing __ashrhi3
;int __ashrhi3(int op, int n)
;{
;       int i,m;
;       if(n>32)
;               n=32;
;       m = n>>2;
;       for(i=0;i<m;i++)
;               op >>= 4;
;       if( n&2 )
;               op >>= 2;
;       if( n&1 )
;               op >>= 1;
;       return op;
;}
;Similar logic applies for MAXQ10 also

#ifdef L_ashrhi3

	.align	1
	.global	___ashrhi3
___ashrhi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	ACC, A[1]
	add	#-32768
	move	A[2], ACC
	move	ACC, #-32736
	sub	A[2]
	jump	nc, L51
	jump	z, L51
	move	A[1], #32
L51:
	move	ACC, A[1]
	sra2	
	move	A[4], ACC
	add	#-32768
	move	A[2], ACC
	move	ACC, #-32768
	sub	A[2]
	jump	nc, L53
	jump	z, L53
	move	A[3], #0
L55:
	move	ACC, A[0]
	sra4	
	move	A[0], ACC
	move	ACC, A[3]
	add	#1
	move	A[3], ACC
	move	ACC, A[4]
	add	#-32768
	move	A[2], ACC
	move	ACC, A[3]
	add	#-32768
	sub	A[2]
	jump	c, L55
L53:
	move	ACC, A[1]
	and	#2
	sub	#0
	jump	z, L56
	move	ACC, A[0]
	sra2	
	move	A[0], ACC
L56:
	move	ACC, A[1]
	and	#1
	sub	#0
	jump	z, L58
	move	ACC, A[0]
	sra	
	move	A[0], ACC
L58:
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;Algorithm for implementing __ashlhi3
;int __ashlhi3(int op, int n)
;{
;        int i,m;
;        if(n>=32)
;                return 0;
;        m=n>>2;
;        for(i=0;i<m;i++)
;                op <<= 4;
;        if(n&2)
;                op <<= 2;
;        if(n&1)
;                op <<= 1;
;        return op;
;}
;Similar logic applies for MAXQ10 also

#ifdef L_ashlhi3

	.align	1
	.global	___ashlhi3
___ashlhi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	ACC, A[1]
	add	#-32768
	move	A[2], ACC
	move	ACC, #-32737
	sub	A[2]
	jump	nc, L63
	jump	z, L63
	move	A[0], #0
	jump	L65
L63:
	move	ACC, A[1]
	sra2	
	move	A[4], ACC
	add	#-32768
	move	A[2], ACC
	move	ACC, #-32768
	sub	A[2]
	jump	nc, L66
	jump	z, L66
	move	A[3], #0
L68:
	move	ACC, A[0]
	sla4	 
	move	A[0], ACC
	move	ACC, A[3]
	add	#1
	move	A[3], ACC
	move	ACC, A[4]
	add	#-32768
	move	A[2], ACC
	move	ACC, A[3]
	add	#-32768
	sub	A[2]
	jump	c, L68
L66:
	move	ACC, A[1]
	and	#2
	sub	#0
	jump	z, L69
	move	ACC, A[0]
	sla2	 
	move	A[0], ACC
L69:
	move	ACC, A[1]
	and	#1
	sub	#0
	jump	z, L65
	move	ACC, A[0]
	sla	
	move	A[0], ACC
L65:
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;Algorithm for implementing __divsi3, __modsi3, __udivsi3, __umodsi3
;static unsigned long long  unsigned_long_divide(unsigned long long  dividend,
;                        unsigned long long divisor,
;                       int qr)
;{
;        unsigned long long t, d;
;        unsigned long long quotient;
;        unsigned long long remainder, bit, q;
;
;        unsigned int num_bits;
;        int i;
;        const int size = sizeof(long long) * 8 - 1;
;
;
;        remainder = 0;
;        quotient = 0;
;
;        if (divisor == 0)
;                return 0;
;        if (divisor > dividend)
;        {
;               remainder = dividend;
;               if(qr)
;                       return quotient;
;               else
;                       return remainder;
;        }
;        if (divisor == dividend)
;       {
;               quotient = 1;
;                if(qr)
;                        return quotient;
;                else
;                        return remainder;
;
;        }
;
;        num_bits = size+1;
;        while (remainder < divisor)
;        {
;                bit = (dividend & (1LL<<size) )  >> size;
;                remainder = (remainder << 1) | bit;
;                d = dividend;
;               dividend = dividend << 1;
;               num_bits--;
;        }
;
;        dividend = d;
;        remainder = remainder >> 1;
;       num_bits++;
;
;        for (i = 0; i < num_bits; i++)
;        {
;                bit = (dividend & (1LL<<size) )  >> size;
;                remainder = (remainder << 1) | bit;
;                t = remainder - divisor;
;                q = !((t & (1LL<<size) )  >> size);
;               dividend = dividend << 1;
;               quotient = (quotient << 1) | q;
;               if (q)
;                        remainder = t;
;        }
;       if(qr)
;               return quotient;
;        else
;                return remainder;
;}
;Similar logic applies for MAXQ10 also

#if defined(L_divdi3) || defined(L_moddi3) || defined(L_udivdi3) || defined(L_umoddi3)

	.align	1
_unsigned_long_divide:
	move	AP, #15
	move	ACC, DP[1]
	sub	#80
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[12]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[13]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[14]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, LC[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, LC[1]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[12], #0
	move	GR, A[12]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #8
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #10
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #12
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #14
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #16
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #18
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #20
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #22
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #24
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #26
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #28
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #30
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #32
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #34
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #36
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #38
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #40
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #42
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #44
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #46
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #48
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #50
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #52
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #54
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #56
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #58
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #60
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #62
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #64
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #66
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #68
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #70
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #72
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #74
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #76
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[12]
	move	OFFS, #78
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[10], A[2]
	move	A[11], A[3]
	move	ACC, A[2]
	or	A[3]
	sub	A[12]
	jump	z, L75
	move	ACC, A[1]
	sub	A[3]
	jump	c, L79
	move	ACC, A[3]
	sub	A[1]
	jump	nz, L77
	move	ACC, A[0]
	sub	A[2]
	jump	nc, L77
	jump	z, L77
L79:
	move	OFFS, #80
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[1],GR
	move	ACC, A[1]
	sub	#0
	jump	nz, L75
	move	A[6], A[4]
	move	A[7], A[5]
	jump	L81
L77:
	move	ACC, A[10]
	sub	A[4]
	jump	nz, L84
	move	ACC, A[11]
	sub	A[5]
	jump	z, L82
L84:
	move	A[8], #0
	move	A[9], A[8]
	move	LC[1], #32
	jump	L85
L82:
	move	OFFS, #80
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[2],GR
	move	ACC, A[2]
	sub	#0
	jump	z, L75
	move	A[6], #1
	move	A[7], #0
	jump	L81
L87:
	move	LC[1], A[0]
L85:
	move	A[1], #0
	move	GR, A[1]
	move	OFFS, #32
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[5]
	and	#-32768
	move	GR, ACC
	move	OFFS, #34
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[2], #31
	move	A[0], A[1]
	move	A[1], ACC
	call	___lshrsi3
	move	GR, A[0]
	move	OFFS, #36
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[1]
	move	OFFS, #38
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[0]
	move	OFFS, #20
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	OFFS, #38
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[2],GR
	move	GR, A[2]
	move	OFFS, #22
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[8]
	add	A[8]
	move	GR, ACC
	move	OFFS, #40
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[9]
	addc	A[9]
	move	GR, ACC
	move	OFFS, #42
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[0]
	move	OFFS, #40
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[2],GR
	or	A[2]
	move	A[8], ACC
	move	OFFS, #38
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[1],GR
	move	ACC, A[1]
	move	OFFS, #42
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[2],GR
	or	A[2]
	move	A[9], ACC
	move	ACC, A[4]
	add	A[4]
	move	A[1], ACC
	move	GR, ACC
	move	OFFS, #44
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[5]
	addc	A[5]
	move	GR, ACC
	move	OFFS, #46
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[4], A[1]
	move	A[5], ACC
	move	ACC, LC[1]
	add	#-1
	move	A[0], ACC
	move	ACC, A[9]
	sub	A[11]
	jump	c, L87
	move	ACC, A[11]
	sub	A[9]
	jump	nz, L89
	move	ACC, A[8]
	sub	A[10]
	jump	c, L87
L89:
	move	A[2], #1
	move	A[0], A[8]
	move	A[1], A[9]
	call	___lshrsi3
	move	GR, A[0]
	move	OFFS, #48
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[1]
	move	OFFS, #50
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[12], A[0]
	move	A[13], A[1]
	move	ACC, LC[1]
	sub	#0
	jump	z, L90
	move	OFFS, #20
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[8],GR
	move	GR, A[8]
	move	OFFS, #12
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	OFFS, #38
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[1],GR
	move	GR, A[1]
	move	OFFS, #14
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[4]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[5]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[5], #0
	move	A[6], A[5]
	move	A[7], A[5]
L92:
	move	ACC, A[12]
	add	A[12]
	move	A[4], ACC
	move	GR, ACC
	move	OFFS, #52
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[13]
	addc	A[13]
	move	A[8], ACC
	move	GR, ACC
	move	OFFS, #54
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	OFFS, #12
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[1],GR
	move	ACC, A[1]
	or	A[4]
	move	A[12], ACC
	move	OFFS, #14
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[4],GR
	move	ACC, A[4]
	or	A[8]
	move	A[13], ACC
	move	ACC, A[12]
	sub	A[10]
	move	GR, ACC
	move	OFFS, #24
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[13]
	subb	A[11]
	move	GR, ACC
	move	OFFS, #26
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[14], #0
	move	LC[0], A[14]
	move	GR, A[14]
	move	OFFS, #56
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	and	#-32768
	move	GR, ACC
	move	OFFS, #58
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[2], #31
	move	A[0], A[14]
	move	A[1], ACC
	call	___lshrsi3
	move	GR, A[0]
	move	OFFS, #60
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[1]
	move	OFFS, #62
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[0]
	move	OFFS, #62
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[2],GR
	or	A[2]
	sub	A[14]
	jump	nz, L93
	move	A[14], #1
L93:
	move	ACC, A[6]
	add	A[6]
	move	A[4], ACC
	move	GR, ACC
	move	OFFS, #64
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[7]
	addc	A[7]
	move	A[8], ACC
	move	GR, ACC
	move	OFFS, #66
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[14]
	or	A[4]
	move	A[6], ACC
	move	ACC, LC[0]
	or	A[8]
	move	A[7], ACC
	move	ACC, A[14]
	or	LC[0]
	sub	#0
	jump	z, L94
	move	OFFS, #24
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[2],GR
	move	A[12], A[2]
	move	OFFS, #26
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[4],GR
	move	A[13], A[4]
L94:
	move	ACC, A[5]
	add	#1
	move	A[5], ACC
	sub	LC[1]
	jump	z, L96
	move	A[8], #0
	move	GR, A[8]
	move	OFFS, #68
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	OFFS, #6
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[1],GR
	move	ACC, A[1]
	and	#-32768
	move	GR, ACC
	move	OFFS, #70
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[2], #31
	move	A[0], A[8]
	move	A[1], ACC
	call	___lshrsi3
	move	GR, A[0]
	move	OFFS, #72
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[1]
	move	OFFS, #74
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[0]
	move	OFFS, #16
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[1]
	move	OFFS, #18
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	OFFS, #4
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[1],GR
	move	ACC, A[1]
	add	A[1]
	move	A[2], ACC
	move	GR, ACC
	move	OFFS, #76
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	OFFS, #6
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[4],GR
	move	ACC, A[4]
	addc	A[4]
	move	GR, ACC
	move	OFFS, #78
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[1], A[2]
	move	GR, A[2]
	move	OFFS, #8
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, ACC
	move	OFFS, #10
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[0]
	move	OFFS, #12
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	OFFS, #74
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[8],GR
	move	GR, A[8]
	move	OFFS, #14
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[1]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, ACC
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	jump	L92
L90:
	move	A[6], #0
	move	A[7], A[6]
L96:
	move	OFFS, #80
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[4],GR
	move	ACC, A[4]
	sub	#0
	jump	nz, L81
	move	A[6], A[12]
	move	A[7], A[13]
	jump	L81
L75:
	move	A[6], #0
	move	A[7], A[6]
L81:
	move	GR, A[6]
	move	OFFS, #28
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[7]
	move	OFFS, #30
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[0], A[6]
	move	A[1], A[7]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	LC[1], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	LC[0], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[14], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[13], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[12], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#80
	move	DP[1], ACC
	ret	
#endif

;Algorithm for implementing __divhi3, __modhi3, __udivhi3, __umodhi3
;static unsigned int unsigned_int_divide(unsigned int dividend,
;                       unsigned int divisor,
;                       int qr )
;{
;       unsigned int t, d;
;        unsigned int quotient;
;        unsigned int remainder;
;       unsigned int bit, num_bits,q;
;       int i;
;       const int size = sizeof(int)*8 -1;
;
;       remainder = 0;
;       quotient = 0;
;
;       if (divisor == 0)
;               return 0;
;       if (divisor > dividend)
;       {
;               remainder = dividend;
;                if(qr)
;                        return quotient;
;                else
;                        return remainder;
;       }
;
;       if (divisor == dividend)
;       {
;               quotient = 1;
;                if(qr)
;                        return quotient;
;                else
;                        return remainder;
;       }
;
;       num_bits = size + 1;
;       while (remainder < divisor)
;       {
;               bit = (dividend & ( 1<< size )) >> size;
;               remainder = (remainder << 1) | bit;
;               d = dividend;
;               dividend = dividend << 1;
;               num_bits--;
;       }
;
;       dividend = d;
;       remainder = remainder >> 1;
;       num_bits++;
;       for (i = 0; i < num_bits; i++)
;       {
;               bit = (dividend & ( 1<< size )) >> size;
;               remainder = (remainder << 1) | bit;
;               t = remainder - divisor;
;               q = !((t & ( 1<< size )) >> size);
;               dividend = dividend << 1;
;               quotient = (quotient << 1) | q;
;               if (q)
;                       remainder = t;
;       }
;        if(qr)
;              return quotient;
;        else
;               return remainder;
;
;}
;Similar logic applies for MAXQ10 also

#if defined(L_divhi3) || defined(L_udivhi3) || defined(L_modhi3) || defined(L_umodhi3)

	.align	1
_unsigned_int_divide:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[12]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[12], A[2]
	move	ACC, A[1]
	sub	#0
	jump	z, L101
	move	ACC, A[0]
	sub	A[1]
	jump	nc, L103
	jump	z, L103
	move	ACC, A[2]
	sub	#0
	jump	nz, L101
	jump	L106
L103:
	move	ACC, A[1]
	sub	A[0]
	jump	z, L107
	move	A[6], #0
	move	A[10], #16
	jump	L109
L107:
	move	ACC, A[2]
	sub	#0
	jump	z, L101
	move	A[4], #1
	jump	L106
L111:
	move	A[10], A[0]
L109:
	move	ACC, A[4]
	and	#-32768
	move	A[1], #15
	move	A[0], ACC
	call	___lshrhi3
	move	A[8], A[0]
	move	ACC, A[6]
	sla	
	move	A[0], ACC
	move	ACC, A[8]
	or	A[0]
	move	A[6], ACC
	move	ACC, A[4]
	sla	
	move	A[7], ACC
	move	A[4], ACC
	move	ACC, A[10]
	add	#-1
	move	A[0], ACC
	move	ACC, A[6]
	sub	A[5]
	jump	c, L111
	move	A[1], #1
	move	A[0], A[6]
	call	___lshrhi3
	move	A[9], A[0]
	move	ACC, A[10]
	sub	#0
	jump	z, L113
	move	A[0], A[8]
	move	A[4], #0
	move	A[11], A[4]
L115:
	move	ACC, A[9]
	sla	
	move	A[1], ACC
	move	ACC, A[0]
	or	A[1]
	move	A[9], ACC
	sub	A[5]
	move	A[8], ACC
	move	A[6], #0
	and	#-32768
	move	A[1], #15
	move	A[0], ACC
	call	___lshrhi3
	move	ACC, A[0]
	sub	A[6]
	jump	nz, L116
	move	A[6], #1
L116:
	move	ACC, A[4]
	sla	
	move	A[0], ACC
	move	ACC, A[6]
	or	A[0]
	move	A[4], ACC
	move	ACC, A[6]
	sub	#0
	jump	z, L117
	move	A[9], A[8]
L117:
	move	ACC, A[11]
	add	#1
	move	A[11], ACC
	sub	A[10]
	jump	z, L119
	move	ACC, A[7]
	and	#-32768
	move	A[1], #15
	move	A[0], ACC
	call	___lshrhi3
	move	ACC, A[7]
	sla	
	move	A[7], ACC
	jump	L115
L113:
	move	A[4], #0
L119:
	move	ACC, A[12]
	sub	#0
	jump	nz, L106
	move	A[4], A[9]
	jump	L106
L101:
	move	A[4], #0
L106:
	move	A[0], A[4]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[12], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif


;#define ABS(x)  ((x) < 0 ? -(x) : (x))
;
;Algorithm for implementing __divhi3, __modhi3
;static int signed_int_divide(int dividend, int divisor, int qr)
;{
;       int quotient;
;        int remainder;
;       int rst;
;       unsigned int dend, dor;
;       int r = 0;
;
;       dend = ABS(dividend);
;       dor  = ABS(divisor);
;       rst = unsigned_int_divide( dend, dor, qr );
;
;       if (dividend < 0)
;               r ^= 1;
;
;        if (!qr && r)
;                rst = -rst;
;
;       if (divisor < 0)
;               r ^= 1;
;        if (qr && r)
;               rst = -rst;
;
;       return rst;
;}
;Similar logic applies for MAXQ10 also

#if defined(L_divhi3) ||  defined(L_modhi3)

	.align	1
_signed_int_divide:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[4], A[0]
	move	A[6], A[1]
	move	A[7], A[2]
	move	A[5], #-32768
	move	ACC, A[6]
	add	#-32768
	sub	A[5]
	jump	nc, L124
	jump	z, L124
	move	ACC, A[6]
	neg	
	move	A[1], ACC
L124:
	move	A[0], A[4]
	move	ACC, A[4]
	add	#-32768
	sub	A[5]
	jump	nc, L125
	jump	z, L125
	move	ACC, A[4]
	neg	
	move	A[0], ACC
L125:
	call	_unsigned_int_divide
	move	A[1], #0
	move	ACC, A[4]
	add	#-32768
	sub	A[5]
	jump	nc, L126
	jump	z, L126
	move	A[1], #1
L126:
	move	ACC, A[1]
	and	#00ffh	
	move	A[4], ACC
	move	ACC, A[7]
	sub	#0
	jump	nz, L127
	move	ACC, A[4]
	sub	#0
	jump	z, L127
	move	ACC, A[0]
	neg	
	move	A[0], ACC
L127:
	move	A[1], #-32768
	move	ACC, A[6]
	add	#-32768
	sub	A[1]
	jump	nc, L130
	jump	z, L130
	move	ACC, A[4]
	xor	#1
	move	A[4], ACC
L130:
	move	ACC, A[7]
	sub	#0
	jump	z, L132
	move	ACC, A[4]
	sub	#0
	jump	z, L132
	move	ACC, A[0]
	neg	
	move	A[0], ACC
L132:
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;Algorithm for implementing __divsi3, __modsi3
;static long long signed_long_divide(long long dividend,
;                       long long divisor,
;                       int qr )
;{
;        long long quotient;
;        long long remainder;
;        unsigned long long  dend, dor;
;       long long rst;
;        int r = 0;
;
;        dend = ABS(dividend);
;        dor  = ABS(divisor);
;        rst = unsigned_long_divide( dend, dor, qr );
;
;        if (dividend < 0)
;                r ^= 1;
;
;        if (!qr && r)
;                rst = -rst;
;
;        if (divisor < 0)
;                r ^= 1;
;        if (qr && r)
;                rst = -rst;
;
;        return rst;
;}
;Similar logic applies for MAXQ10 also

#if defined(L_divdi3) || defined(L_moddi3)

	.align	1
_signed_long_divide:
	move	AP, #15
	move	ACC, DP[1]
	sub	#24
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[12]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[13]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[14]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, LC[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, LC[1]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[8], #0
	move	GR, A[8]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[8]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[8]
	move	OFFS, #8
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[8]
	move	OFFS, #10
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[8]
	move	OFFS, #12
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[8]
	move	OFFS, #14
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[8]
	move	OFFS, #16
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[8]
	move	OFFS, #18
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[8]
	move	OFFS, #20
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[8]
	move	OFFS, #22
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[10], A[0]
	move	A[11], A[1]
	move	GR, A[2]
	move	OFFS, #8
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[3]
	move	OFFS, #10
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	OFFS, #24
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	LC[1],GR
	move	A[4], A[2]
	move	A[5], A[3]
	move	ACC, A[3]
	add	#-32768
	move	A[0], ACC
	move	ACC, #-32768
	sub	A[0]
	jump	c, L137
	move	ACC, A[3]
	sub	A[8]
	jump	nz, L138
	move	ACC, A[8]
	sub	A[2]
	jump	c, L137
L138:
	move	A[12], #0
	move	ACC, A[12]
	sub	A[4]
	move	A[4], ACC
	move	ACC, A[12]
	subb	A[5]
	move	A[5], ACC
L137:
	move	A[6], A[10]
	move	A[7], A[11]
	move	ACC, A[11]
	add	#-32768
	move	A[0], ACC
	move	ACC, #-32768
	sub	A[0]
	jump	c, L139
	move	ACC, A[11]
	sub	#0
	jump	nz, L140
	move	ACC, #0
	sub	A[10]
	jump	c, L139
L140:
	move	A[1], #0
	move	GR, A[1]
	move	OFFS, #12
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[1]
	move	OFFS, #14
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[1]
	sub	A[6]
	move	A[6], ACC
	move	ACC, A[1]
	subb	A[7]
	move	A[7], ACC
L139:
	move	GR, LC[1]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	A[2], A[4]
	move	A[3], A[5]
	move	A[0], A[6]
	move	A[1], A[7]
	call	_unsigned_long_divide
	move	A[8], A[0]
	move	A[9], A[1]
	move	A[5], #0
	move	ACC, DP[1]
	add	#2
	move	DP[1], ACC
	move	A[4], #-32768
	move	ACC, A[11]
	add	#-32768
	sub	A[4]
	jump	nc, L141
	jump	z, L141
	move	A[5], #1
L141:
	move	ACC, A[5]
	and	#00ffh	
	move	A[5], ACC
	move	ACC, LC[1]
	sub	#0
	jump	nz, L143
	move	ACC, A[5]
	sub	#0
	jump	z, L143
	move	A[4], #0
	move	GR, A[4]
	move	OFFS, #16
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[4]
	move	OFFS, #18
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[4]
	sub	A[8]
	move	A[8], ACC
	move	ACC, A[4]
	subb	A[9]
	move	A[9], ACC
L143:
	move	A[4], #-32768
	move	OFFS, #10
	move	DPC, #2
	move	GRL,@BP[OFFS++]
	move	GRH,@BP[OFFS]
	move	A[6],GR
	move	ACC, A[6]
	add	#-32768
	sub	A[4]
	jump	nc, L146
	jump	z, L146
	move	ACC, A[5]
	xor	#1
	move	A[5], ACC
L146:
	move	ACC, LC[1]
	sub	#0
	jump	z, L149
	move	ACC, A[5]
	sub	#0
	jump	z, L149
	move	LC[1], #0
	move	GR, LC[1]
	move	OFFS, #20
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, LC[1]
	move	OFFS, #22
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, LC[1]
	sub	A[8]
	move	A[8], ACC
	move	ACC, LC[1]
	subb	A[9]
	move	A[9], ACC
L149:
	move	GR, A[8]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, A[9]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[0], A[8]
	move	A[1], A[9]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	LC[1], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	LC[0], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[14], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[13], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[12], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#24
	move	DP[1], ACC
	ret	
#endif

;This function calls the actual implementation for __udvisi3
;unsigned long long __udivsi3(unsigned long long op1, unsigned long long op2)
;{
;       return unsigned_long_divide (op1, op2,1);
;}
;Similar thing exists for MAXQ10 also

#ifdef L_udivdi3

	.align	1
	.global	___udivsi3
___udivsi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	GR, #1
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	call	_unsigned_long_divide
	move	ACC, DP[1]
	add	#2
	move	DP[1], ACC
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;This function calls the actual implementation of __divsi3
;long long __divsi3(long long op1, long long op2)
;{
;        return signed_long_divide (op1, op2,1);
;}
;Similar thing exists for MAXQ10 also

#ifdef L_divdi3

	.align	1
	.global	___divsi3
___divsi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	GR, #1
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	call	_signed_long_divide
	move	ACC, DP[1]
	add	#2
	move	DP[1], ACC
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;This function calls the actual implementation of __udivhi3
;unsigned int  __udivhi3(unsigned int op1, unsigned int op2)
;{
;        return unsigned_int_divide (op1, op2,1);
;}
;Similar thing exist for MAXQ10 also

#ifdef L_udivhi3

	.align	1
	.global	___udivhi3
___udivhi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[2], #1
	call	_unsigned_int_divide
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;This function calls the actual implementation of __divhi3
;int  __divhi3(int op1, int op2)
;{
;        return signed_int_divide (op1, op2,1);
;}
;Similar thing exists for MAXQ10 also

#ifdef L_divhi3

	.align	1
	.global	___divhi3
___divhi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[2], #1
	call	_signed_int_divide
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;This function calls the actual implementation of __umodsi3
;unsigned long long __umodsi3(unsigned long long op1, unsigned long long op2)
;{
;        return unsigned_long_divide (op1, op2,0);
;}
;Similar thing exists for MAXQ10 also

#ifdef L_umoddi3

	.align	1
	.global	___umodsi3
___umodsi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[8], #0
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	call	_unsigned_long_divide
	move	ACC, DP[1]
	add	#2
	move	DP[1], ACC
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;This function calls the actual implementation of __modsi3
;long long __modsi3(long long op1, long long op2)
;{
;        return signed_long_divide (op1, op2,0);
;}
;Similar thing exists for MAXQ10 also

#ifdef L_moddi3

	.align	1
	.global	___modsi3
___modsi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, A[4]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[5]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[6]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[7]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[9]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, A[11]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[8], #0
	move	GR, A[8]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	call	_signed_long_divide
	move	ACC, DP[1]
	add	#2
	move	DP[1], ACC
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[11], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[10], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[9], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[8], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[7], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[6], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[5], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	A[4], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;This function calls the actual implementation of __umodhi3
;unsigned int  __umodhi3(unsigned int op1, unsigned int op2)
;{
;        return unsigned_int_divide (op1, op2,0);
;}
;Similar thing exists for MAXQ10 also

#ifdef L_umodhi3

	.align	1
	.global	___umodhi3
___umodhi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[2], #0
	call	_unsigned_int_divide
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

;This function calls the actual implementation of __modhi3
;int  __modhi3(int op1, int op2)
;{
;        return signed_int_divide (op1, op2,0);
;}
;Similar thing exists for MAXQ10 also

#ifdef L_modhi3

	.align	1
	.global	___modhi3
___modhi3:
	move	AP, #15
	move	ACC, DP[1]
	sub	#4
	move	DP[1], ACC
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	ACC, DP[1]
	add	#2
	move	BP, ACC
	move	GR, @SP--
	move	@--DP[1], GRL
	move	@--DP[1], GRH
	move	@++SP, GR
	move	A[2], #0
	call	_signed_int_divide
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	ACC, DP[1]
	add	#4
	move	DP[1], ACC
	ret	
#endif

#endif


;Library implementation for MAXQ10 starts here

#ifdef __MAXQ10__

	.file	"lib1funcs.c"
	.version	0
	.text
#ifdef L_muldi3
	.align	1
	.global	___mulsi3
___mulsi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#16
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	DPC,#1
	move	@--DP[1],A[12]
	move	DPC,#1
	move	@--DP[1],A[13]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	A[4], #0
	move	OFFS,#8
	move	@BP[OFFS], A[4]
	move	OFFS,#9
	move	@BP[OFFS], A[4]
	move	OFFS,#10
	move	@BP[OFFS], A[4]
	move	OFFS,#11
	move	@BP[OFFS], A[4]
	move	OFFS,#12
	move	@BP[OFFS], A[4]
	move	OFFS,#13
	move	@BP[OFFS], A[4]
	move	OFFS,#14
	move	@BP[OFFS], A[4]
	move	OFFS,#15
	move	@BP[OFFS], A[4]
	move	A[8], A[0]
	move	A[9], A[1]
	move	A[10], A[2]
	move	A[11], A[3]
	move	GR,#16
	move	A[1],GRL
	move	A[2],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[1]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[2]
	move	GRH, ACC
	move	A[1], GRL
	move	A[2], GRH
	move	GRL, A[1]
	move	GRH, A[2]
	move	DP[0], GR
	move	DPC, #0
	move	A[4],@DP[0]++
	move	GRL, A[1]
	move	GRH, A[2]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	GRL, A[1]
	move	GRH, A[2]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[6],@DP[0]++
	move	GRL, A[1]
	move	GRH, A[2]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[7],@DP[0]++
	move	A[2], #0
	move	OFFS,#4
	move	@BP[OFFS], A[2]
	move	OFFS,#5
	move	@BP[OFFS], A[2]
	move	OFFS,#6
	move	@BP[OFFS], A[2]
	move	OFFS,#7
	move	@BP[OFFS], A[2]
	move	GR,#32
	move	A[12],GRL
	move	A[13],GRH
L2:
	move	ACC, A[8]
	and	#1
	move	OFFS,#8
	move	@BP[OFFS], ACC
	move	A[2], #0
	move	OFFS,#9
	move	@BP[OFFS], A[2]
	move	OFFS,#10
	move	@BP[OFFS], A[2]
	move	OFFS,#11
	move	@BP[OFFS], A[2]
	or	A[2]
	sub	A[2]
	jump	z, L3
	move	DPC, #2
	move	OFFS, #4
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	add	A[4]
	move	OFFS,#4
	move	@BP[OFFS], ACC
	move	DPC, #2
	move	OFFS, #5
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	addc	A[5]
	move	OFFS,#5
	move	@BP[OFFS], ACC
	move	DPC, #2
	move	OFFS, #6
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	addc	A[6]
	move	OFFS,#6
	move	@BP[OFFS], ACC
	move	DPC, #2
	move	OFFS, #7
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	addc	A[7]
	move	OFFS,#7
	move	@BP[OFFS], ACC
L3:
	move	GR, DP[1]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	DP[1], GR
	move	DPC,#1
	move	@--DP[1],#1
	move	A[0], A[8]
	move	A[1], A[9]
	move	A[2], A[10]
	move	A[3], A[11]
	call	___ashrsi3
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move	A[8], A[0]
	move	A[9], A[1]
	move	A[10], A[2]
	move	A[11], A[3]
	move	ACC, A[4]
	add	A[4]
	move	OFFS,#12
	move	@BP[OFFS], ACC
	move	ACC, A[5]
	addc	A[5]
	move	OFFS,#13
	move	@BP[OFFS], ACC
	move	ACC, A[6]
	addc	A[6]
	move	OFFS,#14
	move	@BP[OFFS], ACC
	move	ACC, A[7]
	addc	A[7]
	move	OFFS,#15
	move	@BP[OFFS], ACC
	move	DPC, #2
	move	OFFS, #12
	move	A[1], @BP[OFFS++]
	move	A[4], A[1]
	move	DPC, #2
	move	OFFS, #13
	move	A[2], @BP[OFFS++]
	move	A[5], A[2]
	move	DPC, #2
	move	OFFS, #14
	move	A[1], @BP[OFFS++]
	move	A[6], A[1]
	move	A[7], ACC
	move	GRL, A[12]
	move	GRH, A[13]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[12], GRL
	move	A[13], GRH
	move	ACC, GR
	or	A[13]
	sub	#0
	jump	nz, L2
	move	DPC, #2
	move	OFFS, #4
	move	A[0], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #5
	move	A[1], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #6
	move	A[2], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #7
	move	A[3], @BP[OFFS++]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[13],@DP[1]++
	move	DPC,#1h
	move	A[12],@DP[1]++
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#16
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_mulhi3
	.align	1
	.global	___mulhi3
___mulhi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GR,#0
	move	A[8],GRL
	move	A[9],GRH
	move	GR,#16
	move	A[6],GRL
	move	A[7],GRH
L10:
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	and	#1
	move	GRL, ACC
	move	ACC,GRH
	and	#0
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	ACC, GR
	or	A[5]
	sub	#0
	jump	z, L11
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	A[2]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[3]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
L11:
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[2]
	move	GRH, A[3]
	move	ACC,GRL
	add	A[2]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[3]
	move	GRH, ACC
	move	GR, GR
	move	A[2], GRL
	move	A[3], GRH
	move	GRL, A[6]
	move	GRH, A[7]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[6], GRL
	move	A[7], GRH
	move	ACC, GR
	or	A[7]
	sub	#0
	jump	nz, L10
	move	GRL, A[8]
	move	GRH, A[9]
	move	A[0],GRL
	move	A[1], GRH
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_negdi2

	.align	1
	.global	___negsi2
___negsi2:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#8
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	OFFS,#4
	move	@BP[OFFS], A[0]
	move	OFFS,#5
	move	@BP[OFFS], A[1]
	move	OFFS,#6
	move	@BP[OFFS], A[2]
	move	OFFS,#7
	move	@BP[OFFS], A[3]
	move	ACC, A[0]
	cpl	
	move	A[4], ACC
	move	DPC, #2
	move	OFFS, #5
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	cpl	
	move	A[5], ACC
	move	ACC, A[2]
	cpl	
	move	A[6], ACC
	move	ACC, A[3]
	cpl	
	move	A[7], ACC
	move	ACC, A[4]
	add	#1
	move	A[8], ACC
	move	ACC, A[5]
	addc	#0
	move	A[9], ACC
	move	ACC, A[6]
	addc	#0
	move	A[10], ACC
	move	ACC, A[7]
	addc	#0
	move	A[0], A[8]
	move	A[1], A[9]
	move	A[2], A[10]
	move	A[3], ACC
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#8
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_ashrdi3

	.align	1
	.global	___ashrsi3
___ashrsi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#12
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	DPC,#1
	move	@--DP[1],A[12]
	move	DPC,#1
	move	@--DP[1],A[13]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[6], A[2]
	move	A[7], A[3]
	move	GR,#12
	move	A[1],GRL
	move	A[2],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[1]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[2]
	move	GRH, ACC
	move	A[1], GRL
	move	A[2], GRH
	move	GRL, A[1]
	move	GRH, A[2]
	move	DP[0], GR
	move	DPC, #0
	move	A[8],@DP[0]++
	move	A[9],@DP[0]
	move	ACC, A[8]
	or	A[9]
	sub	#0
	jump	z, L20
	move	GR, BP
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[0]
	move	GRH, A[1]
	move	DP[0], GR
	move	@DP[0], A[4]
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	@DP[0], A[5]
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	@DP[0], A[6]
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	@DP[0], A[3]
	move	ACC, #0
	sub	A[9]
	jump	c, L24
	move	ACC, A[9]
	sub	#0
	jump	nz, L22
	move	ACC, #14
	sub	A[8]
	jump	nc, L22
	jump	z, L22
L24:
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#-16
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[2], GRL
	move	A[3], GRH
	move	OFFS, #6
	move	DPC, #2
	move	A[8],@BP[OFFS++]
	move	A[9],@BP[OFFS]
	move	GRL, A[8]
	move	GRH, A[9]
	move	A[0],GRL
	move	A[1], GRH
	call	___ashrhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	OFFS, #6
	jump	L26
L22:
	move	A[2], A[8]
	move	GR,#1
	move	A[0],GRL
	move	A[1],GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[10], GRL
	move	A[11], GRH
	move	GRL, A[10]
	move	GRH, A[11]
	move	OFFS, #8
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, #16
	move	ACC,GRL
	sub	A[8]
	move	GRL, ACC
	move	ACC,GRH
	subb	A[9]
	move	GRH, ACC
	move	DP[0], GR
	move	OFFS, #6
	move	DPC, #2
	move	A[10],@BP[OFFS++]
	move	A[11],@BP[OFFS]
	move	OFFS, #8
	move	DPC, #2
	move	A[1], @BP[OFFS++]
	move	A[2], @BP[OFFS++]
	move	GRL, A[1]
	move	GRH, A[2]
	move	ACC,GRL
	and	A[10]
	move	GRL, ACC
	move	ACC,GRH
	and	A[11]
	move	GRH, ACC
	move	A[2], DP[0]
	move	GR, GR
	move	A[0], GRL
	move	A[1], GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	OFFS, #10
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[2], A[8]
	move	OFFS, #4
	move	DPC, #2
	move	A[0], @BP[OFFS++]
	move	A[1], @BP[OFFS++]
	call	___ashrhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	A[12],GRL
	move	A[13], GRH
	move	A[2], A[8]
	move	GRL, A[10]
	move	GRH, A[11]
	move	A[0],GRL
	move	A[1], GRH
	call	___ashrhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[2], DP[0]
	move	OFFS, #8
	move	DPC, #2
	move	A[8], @BP[OFFS++]
	move	A[9], @BP[OFFS++]
	move	GRL, A[8]
	move	GRH, A[9]
	move	A[0],GRL
	move	A[1], GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	cpl	
	move	GRL, ACC
	move	ACC,GRH
	cpl	
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[12]
	move	GRH, A[13]
	move	ACC,GRL
	and	A[0]
	move	GRL, ACC
	move	ACC,GRH
	and	A[1]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	OFFS, #10
	move	DPC, #2
	move	A[10], @BP[OFFS++]
	move	A[11], @BP[OFFS++]
	move	GRL, A[10]
	move	GRH, A[11]
	move	ACC,GRL
	or	A[8]
	move	GRL, ACC
	move	ACC,GRH
	or	A[9]
	move	GRH, ACC
	move	OFFS, #4
L26:
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, BP
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	GRL, A[8]
	move	GRH, A[9]
	move	DP[0], GR
	move	DPC, #0
	move	A[4],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[6],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[7],@DP[0]++
L20:
	move	A[0], A[4]
	move	A[1], A[5]
	move	A[2], A[6]
	move	A[3], A[7]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[13],@DP[1]++
	move	DPC,#1h
	move	A[12],@DP[1]++
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#12
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_lshrdi3

	.align	1
	.global	___lshrsi3
___lshrsi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#12
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	DPC,#1
	move	@--DP[1],A[12]
	move	DPC,#1
	move	@--DP[1],A[13]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[6], A[2]
	move	A[7], A[3]
	move	GR,#12
	move	A[1],GRL
	move	A[2],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[1]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[2]
	move	GRH, ACC
	move	A[1], GRL
	move	A[2], GRH
	move	GRL, A[1]
	move	GRH, A[2]
	move	DP[0], GR
	move	DPC, #0
	move	A[8],@DP[0]++
	move	A[9],@DP[0]
	move	ACC, A[8]
	or	A[9]
	sub	#0
	jump	z, L28
	move	GR, BP
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[0]
	move	GRH, A[1]
	move	DP[0], GR
	move	@DP[0], A[4]
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	@DP[0], A[5]
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	@DP[0], A[6]
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	@DP[0], A[3]
	move	ACC, #0
	sub	A[9]
	jump	c, L32
	move	ACC, A[9]
	sub	#0
	jump	nz, L30
	move	ACC, #14
	sub	A[8]
	jump	nc, L30
	jump	z, L30
L32:
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#-16
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[2], GRL
	move	A[3], GRH
	move	OFFS, #6
	move	DPC, #2
	move	A[0], @BP[OFFS++]
	move	A[1], @BP[OFFS++]
	call	___lshrhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, #0
	move	OFFS, #6
	jump	L34
L30:
	move	A[2], A[8]
	move	GR,#1
	move	A[0],GRL
	move	A[1],GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[10], GRL
	move	A[11], GRH
	move	GRL, A[10]
	move	GRH, A[11]
	move	OFFS, #8
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, #16
	move	ACC,GRL
	sub	A[8]
	move	GRL, ACC
	move	ACC,GRH
	subb	A[9]
	move	GRH, ACC
	move	DP[0], GR
	move	OFFS, #6
	move	DPC, #2
	move	A[10],@BP[OFFS++]
	move	A[11],@BP[OFFS]
	move	OFFS, #8
	move	DPC, #2
	move	A[1], @BP[OFFS++]
	move	A[2], @BP[OFFS++]
	move	GRL, A[1]
	move	GRH, A[2]
	move	ACC,GRL
	and	A[10]
	move	GRL, ACC
	move	ACC,GRH
	and	A[11]
	move	GRH, ACC
	move	A[2], DP[0]
	move	GR, GR
	move	A[0], GRL
	move	A[1], GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	OFFS, #10
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[2], A[8]
	move	OFFS, #4
	move	DPC, #2
	move	A[0], @BP[OFFS++]
	move	A[1], @BP[OFFS++]
	call	___ashrhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	A[12],GRL
	move	A[13], GRH
	move	A[2], A[8]
	move	GRL, A[10]
	move	GRH, A[11]
	move	A[0],GRL
	move	A[1], GRH
	call	___ashrhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	A[8],GRL
	move	A[9], GRH
	move	A[2], DP[0]
	move	OFFS, #8
	move	DPC, #2
	move	A[10], @BP[OFFS++]
	move	A[11], @BP[OFFS++]
	move	GRL, A[10]
	move	GRH, A[11]
	move	A[0],GRL
	move	A[1], GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	cpl	
	move	GRL, ACC
	move	ACC,GRH
	cpl	
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[12]
	move	GRH, A[13]
	move	ACC,GRL
	and	A[0]
	move	GRL, ACC
	move	ACC,GRH
	and	A[1]
	move	GRH, ACC
	move	A[10], GRL
	move	A[11], GRH
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	and	A[8]
	move	GRL, ACC
	move	ACC,GRH
	and	A[9]
	move	GRH, ACC
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	OFFS, #10
	move	DPC, #2
	move	A[1], @BP[OFFS++]
	move	A[2], @BP[OFFS++]
	move	GRL, A[1]
	move	GRH, A[2]
	move	ACC,GRL
	or	A[10]
	move	GRL, ACC
	move	ACC,GRH
	or	A[11]
	move	GRH, ACC
	move	OFFS, #4
L34:
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, BP
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	GRL, A[8]
	move	GRH, A[9]
	move	DP[0], GR
	move	DPC, #0
	move	A[4],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[6],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[7],@DP[0]++
L28:
	move	A[0], A[4]
	move	A[1], A[5]
	move	A[2], A[6]
	move	A[3], A[7]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[13],@DP[1]++
	move	DPC,#1h
	move	A[12],@DP[1]++
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#12
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_ashldi3

	.align	1
	.global	___ashlsi3
___ashlsi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#8
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	DPC,#1
	move	@--DP[1],A[12]
	move	DPC,#1
	move	@--DP[1],A[13]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[6], A[2]
	move	A[7], A[3]
	move	GR,#8
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	GRL, A[8]
	move	GRH, A[9]
	move	DP[0], GR
	move	DPC, #0
	move	A[8],@DP[0]++
	move	A[9],@DP[0]
	move	GR, BP
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[0]
	move	GRH, A[1]
	move	DP[0], GR
	move	@DP[0], A[4]
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	@DP[0], A[5]
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	@DP[0], A[2]
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	@DP[0], A[3]
	move	ACC, A[8]
	or	A[9]
	sub	#0
	jump	z, L36
	move	ACC, A[9]
	xor	#-128
	move	A[0], ACC
	move	ACC, #-128
	sub	A[0]
	jump	c, L40
	move	ACC, A[9]
	sub	#0
	jump	nz, L38
	move	ACC, #14
	sub	A[8]
	jump	nc, L38
	jump	z, L38
L40:
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#-16
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[2], GRL
	move	A[3], GRH
	move	OFFS, #4
	move	DPC, #2
	move	A[0], @BP[OFFS++]
	move	A[1], @BP[OFFS++]
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, #0
	move	OFFS, #4
	jump	L42
L38:
	move	GR, #16
	move	ACC,GRL
	sub	A[8]
	move	GRL, ACC
	move	ACC,GRH
	subb	A[9]
	move	GRH, ACC
	move	A[12], GRL
	move	A[13], GRH
	move	OFFS, #4
	move	DPC, #2
	move	A[10],@BP[OFFS++]
	move	A[11],@BP[OFFS]
	move	A[2], A[8]
	move	GRL, A[10]
	move	GRH, A[11]
	move	A[0],GRL
	move	A[1], GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	A[2], A[8]
	move	GR,#1
	move	A[0],GRL
	move	A[1],GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[2], A[12]
	move	GR, GR
	move	A[0], GRL
	move	A[1], GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	and	A[10]
	move	GRL, ACC
	move	ACC,GRH
	and	A[11]
	move	GRH, ACC
	move	A[2], A[12]
	move	GR, GR
	move	A[0], GRL
	move	A[1], GRH
	call	___lshrhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	A[10],GRL
	move	A[11], GRH
	move	A[2], A[8]
	move	OFFS, #6
	move	DPC, #2
	move	A[0], @BP[OFFS++]
	move	A[1], @BP[OFFS++]
	call	___ashlhi3
	move	GRL, A[10]
	move	GRH, A[11]
	move	ACC,GRL
	or	A[0]
	move	GRL, ACC
	move	ACC,GRH
	or	A[1]
	move	GRH, ACC
	move	OFFS, #6
L42:
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR, BP
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	GRL, A[8]
	move	GRH, A[9]
	move	DP[0], GR
	move	DPC, #0
	move	A[4],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[6],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[7],@DP[0]++
L36:
	move	A[0], A[4]
	move	A[1], A[5]
	move	A[2], A[6]
	move	A[3], A[7]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[13],@DP[1]++
	move	DPC,#1h
	move	A[12],@DP[1]++
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#8
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_lshrhi3

	.align	1
	.global	___lshrhi3
___lshrhi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GRL, A[0]
	move	GRH, A[1]
	move	A[8],GRL
	move	A[9], GRH
	move	GRL, A[2]
	move	GRH, A[3]
	move	A[4],GRL
	move	A[5], GRH
	move	ACC, A[5]
	xor	#-128
	move	A[0], ACC
	move	ACC, #-128
	sub	A[0]
	jump	c, L46
	move	ACC, A[5]
	sub	#0
	jump	nz, L44
	move	ACC, #31
	sub	A[2]
	jump	nc, L44
	jump	z, L44
L46:
	move	GR,#0
	move	A[0],GRL
	move	A[1],GRH
	jump	L47
L44:
	move	A[2], A[4]
	move	GR,#1
	move	A[0],GRL
	move	A[1],GRH
	call	___ashlhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GR, #16
	move	ACC,GRL
	sub	A[4]
	move	GRL, ACC
	move	ACC,GRH
	subb	A[5]
	move	GRH, ACC
	move	A[2], GR
	call	___ashlhi3
	move	GRL, A[4]
	move	GRH, A[5]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[6], GRL
	move	A[7], GRH
	move	ACC, A[7]
	xor	#-128
	move	A[2], ACC
	move	ACC, #-128
	sub	A[2]
	jump	c, L50
	move	ACC, A[7]
	sub	#0
	jump	nz, L48
	move	ACC, #0
	sub	A[6]
	jump	nc, L48
	jump	z, L48
L50:
	move	GR,#0
	move	A[10],GRL
	move	A[11],GRH
L63:
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	GRL, A[10]
	move	GRH, A[11]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[10], GRL
	move	A[11], GRH
	move	ACC, A[7]
	xor	#-128
	move	A[2], ACC
	move	ACC, A[11]
	xor	#-128
	sub	A[2]
	jump	c, L63
	move	ACC, A[7]
	sub	A[11]
	jump	nz, L48
	move	ACC, GR
	sub	A[6]
	jump	c, L63
L48:
	move	GRL, A[4]
	move	GRH, A[5]
	move	ACC,GRL
	and	#2
	move	GRL, ACC
	move	ACC,GRH
	and	#0
	move	GRH, ACC
	move	A[2], GRL
	move	A[3], GRH
	move	ACC, GR
	or	A[3]
	sub	#0
	jump	z, L53
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[8], GRL
	move	A[9], GRH
L53:
	move	GRL, A[4]
	move	GRH, A[5]
	move	ACC,GRL
	and	#1
	move	GRL, ACC
	move	ACC,GRH
	and	#0
	move	GRH, ACC
	move	A[2], GRL
	move	A[3], GRH
	move	ACC, GR
	or	A[3]
	sub	#0
	jump	z, L55
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[8], GRL
	move	A[9], GRH
L55:
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	cpl	
	move	GRL, ACC
	move	ACC,GRH
	cpl	
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	and	A[0]
	move	GRL, ACC
	move	ACC,GRH
	and	A[1]
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
L47:
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_ashrhi3

	.align	1
	.global	___ashrhi3
___ashrhi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	ACC, A[3]
	xor	#-128
	move	A[4], ACC
	move	ACC, #-128
	sub	A[4]
	jump	c, L67
	move	ACC, A[3]
	sub	#0
	jump	nz, L65
	move	ACC, #32
	sub	A[2]
	jump	nc, L65
	jump	z, L65
L67:
	move	GR,#32
	move	A[2],GRL
	move	A[3],GRH
L65:
	move	GRL, A[2]
	move	GRH, A[3]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[5], GRL
	move	A[6], GRH
	move	ACC, A[6]
	xor	#-128
	move	A[4], ACC
	move	ACC, #-128
	sub	A[4]
	jump	c, L70
	move	ACC, A[6]
	sub	#0
	jump	nz, L68
	move	ACC, #0
	sub	A[5]
	jump	nc, L68
	jump	z, L68
L70:
	move	GR,#0
	move	A[7],GRL
	move	A[8],GRH
L83:
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[7]
	move	GRH, A[8]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[7], GRL
	move	A[8], GRH
	move	ACC, A[6]
	xor	#-128
	move	A[4], ACC
	move	ACC, A[8]
	xor	#-128
	sub	A[4]
	jump	c, L83
	move	ACC, A[6]
	sub	A[8]
	jump	nz, L68
	move	ACC, GR
	sub	A[5]
	jump	c, L83
L68:
	move	GRL, A[2]
	move	GRH, A[3]
	move	ACC,GRL
	and	#2
	move	GRL, ACC
	move	ACC,GRH
	and	#0
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	ACC, GR
	or	A[5]
	sub	#0
	jump	z, L73
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[0], GRL
	move	A[1], GRH
L73:
	move	GRL, A[2]
	move	GRH, A[3]
	move	ACC,GRL
	and	#1
	move	GRL, ACC
	move	ACC,GRH
	and	#0
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	ACC, GR
	or	A[5]
	sub	#0
	jump	z, L75
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[0], GRL
	move	A[1], GRH
L75:
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_ashlhi3

	.align	1
	.global	___ashlhi3
___ashlhi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	ACC, A[3]
	xor	#-128
	move	A[4], ACC
	move	ACC, #-128
	sub	A[4]
	jump	c, L87
	move	ACC, A[3]
	sub	#0
	jump	nz, L85
	move	ACC, #31
	sub	A[2]
	jump	nc, L85
	jump	z, L85
L87:
	move	GR,#0
	move	A[0],GRL
	move	A[1],GRH
	jump	L88
L85:
	move	GRL, A[2]
	move	GRH, A[3]
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	ACC,GRH
	sra	
	move	GRH, ACC
	move	ACC,GRL
	rrc	
	move	GRL, ACC
	move	A[5], GRL
	move	A[6], GRH
	move	ACC, A[6]
	xor	#-128
	move	A[4], ACC
	move	ACC, #-128
	sub	A[4]
	jump	c, L91
	move	ACC, A[6]
	sub	#0
	jump	nz, L89
	move	ACC, #0
	sub	A[5]
	jump	nc, L89
	jump	z, L89
L91:
	move	GR,#0
	move	A[7],GRL
	move	A[8],GRH
L103:
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	sla	
	move	GRL, ACC
	move	ACC,GRH
	rlc	 
	move	GRH, ACC
	move	ACC,GRL
	sla	
	move	GRL, ACC
	move	ACC,GRH
	rlc	 
	move	GRH, ACC
	move	ACC,GRL
	sla	
	move	GRL, ACC
	move	ACC,GRH
	rlc	 
	move	GRH, ACC
	move	ACC,GRL
	sla	
	move	GRL, ACC
	move	ACC,GRH
	rlc	 
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[7]
	move	GRH, A[8]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[7], GRL
	move	A[8], GRH
	move	ACC, A[6]
	xor	#-128
	move	A[4], ACC
	move	ACC, A[8]
	xor	#-128
	sub	A[4]
	jump	c, L103
	move	ACC, A[6]
	sub	A[8]
	jump	nz, L89
	move	ACC, GR
	sub	A[5]
	jump	c, L103
L89:
	move	GRL, A[2]
	move	GRH, A[3]
	move	ACC,GRL
	and	#2
	move	GRL, ACC
	move	ACC,GRH
	and	#0
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	ACC, GR
	or	A[5]
	sub	#0
	jump	z, L94
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	sla	
	move	GRL, ACC
	move	ACC,GRH
	rlc	 
	move	GRH, ACC
	move	ACC,GRL
	sla	
	move	GRL, ACC
	move	ACC,GRH
	rlc	 
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
L94:
	move	GRL, A[2]
	move	GRH, A[3]
	move	ACC,GRL
	and	#1
	move	GRL, ACC
	move	ACC,GRH
	and	#0
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	ACC, GR
	or	A[5]
	sub	#0
	jump	z, L88
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	add	A[0]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[1]
	move	GRH, ACC
	move	GR, GR
	move	A[0], GRL
	move	A[1], GRH
L88:
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#if defined(L_divdi3) || defined(L_moddi3) || defined(L_udivdi3) || defined(L_umoddi3)
 
	.align	1
_unsigned_long_divide:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#66
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	DPC,#1
	move	@--DP[1],A[12]
	move	DPC,#1
	move	@--DP[1],A[13]
	move	DPC,#1
	move	@--DP[1],A[14]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	A[4], #0
	move	OFFS,#16
	move	@BP[OFFS], A[4]
	move	OFFS,#17
	move	@BP[OFFS], A[4]
	move	OFFS,#18
	move	@BP[OFFS], A[4]
	move	OFFS,#19
	move	@BP[OFFS], A[4]
	move	OFFS,#26
	move	@BP[OFFS], A[4]
	move	OFFS,#27
	move	@BP[OFFS], A[4]
	move	OFFS,#28
	move	@BP[OFFS], A[4]
	move	OFFS,#29
	move	@BP[OFFS], A[4]
	move	OFFS,#34
	move	@BP[OFFS], A[4]
	move	OFFS,#35
	move	@BP[OFFS], A[4]
	move	OFFS,#36
	move	@BP[OFFS], A[4]
	move	OFFS,#37
	move	@BP[OFFS], A[4]
	move	OFFS,#38
	move	@BP[OFFS], A[4]
	move	OFFS,#39
	move	@BP[OFFS], A[4]
	move	OFFS,#40
	move	@BP[OFFS], A[4]
	move	OFFS,#41
	move	@BP[OFFS], A[4]
	move	OFFS,#42
	move	@BP[OFFS], A[4]
	move	OFFS,#43
	move	@BP[OFFS], A[4]
	move	OFFS,#44
	move	@BP[OFFS], A[4]
	move	OFFS,#45
	move	@BP[OFFS], A[4]
	move	OFFS,#46
	move	@BP[OFFS], A[4]
	move	OFFS,#47
	move	@BP[OFFS], A[4]
	move	OFFS,#48
	move	@BP[OFFS], A[4]
	move	OFFS,#49
	move	@BP[OFFS], A[4]
	move	OFFS,#54
	move	@BP[OFFS], A[4]
	move	OFFS,#55
	move	@BP[OFFS], A[4]
	move	OFFS,#56
	move	@BP[OFFS], A[4]
	move	OFFS,#57
	move	@BP[OFFS], A[4]
	move	OFFS,#58
	move	@BP[OFFS], A[4]
	move	OFFS,#59
	move	@BP[OFFS], A[4]
	move	OFFS,#60
	move	@BP[OFFS], A[4]
	move	OFFS,#61
	move	@BP[OFFS], A[4]
	move	OFFS,#62
	move	@BP[OFFS], A[4]
	move	OFFS,#63
	move	@BP[OFFS], A[4]
	move	OFFS,#64
	move	@BP[OFFS], A[4]
	move	OFFS,#65
	move	@BP[OFFS], A[4]
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[6], A[2]
	move	A[7], A[3]
	move	GR,#66
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	GRL, A[8]
	move	GRH, A[9]
	move	DP[0], GR
	move	DPC, #0
	move	A[9],@DP[0]++
	move	OFFS,#20
	move	@BP[OFFS], A[9]
	move	GR,#66
	move	A[1],GRL
	move	A[2],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[1]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[2]
	move	GRH, ACC
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[2],@DP[0]++
	move	OFFS,#21
	move	@BP[OFFS], A[2]
	move	GR,#66
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[9],@DP[0]++
	move	OFFS,#22
	move	@BP[OFFS], A[9]
	move	GR,#66
	move	A[1],GRL
	move	A[2],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[1]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[2]
	move	GRH, ACC
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[2],@DP[0]++
	move	OFFS,#23
	move	@BP[OFFS], A[2]
	move	GR,#66
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[1],@DP[0]++
	move	A[2],@DP[0]
	move	GRL, A[1]
	move	GRH, A[2]
	move	OFFS, #24
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	DPC, #2
	move	OFFS, #20
	move	A[2], @BP[OFFS++]
	move	ACC, A[2]
	move	DPC, #2
	move	OFFS, #21
	move	A[3], @BP[OFFS++]
	or	A[3]
	move	DPC, #2
	move	OFFS, #22
	move	A[8], @BP[OFFS++]
	or	A[8]
	move	DPC, #2
	move	OFFS, #23
	move	A[9], @BP[OFFS++]
	or	A[9]
	sub	#0
	jump	z, L105
	move	ACC, A[7]
	sub	A[9]
	jump	c, L109
	move	ACC, A[9]
	sub	A[7]
	jump	nz, L107
	move	ACC, A[6]
	move	DPC, #2
	move	OFFS, #22
	move	A[12], @BP[OFFS++]
	sub	A[12]
	jump	c, L109
	move	ACC, A[12]
	sub	A[6]
	jump	nz, L107
	move	ACC, A[5]
	sub	A[3]
	jump	c, L109
	move	ACC, A[3]
	sub	A[5]
	jump	nz, L107
	move	ACC, A[0]
	sub	A[2]
	jump	nc, L107
	jump	z, L107
L109:
	move	DPC, #2
	move	OFFS, #24
	move	A[14], @BP[OFFS++]
	move	ACC, A[14]
	move	DPC, #2
	move	OFFS, #25
	move	A[1], @BP[OFFS++]
	or	A[1]
	sub	#0
	jump	nz, L105
	jump	L130
L107:
	move	DPC, #2
	move	OFFS, #20
	move	A[2], @BP[OFFS++]
	move	ACC, A[2]
	sub	A[4]
	jump	nz, L112
	move	DPC, #2
	move	OFFS, #21
	move	A[3], @BP[OFFS++]
	move	ACC, A[3]
	sub	A[5]
	jump	nz, L112
	move	DPC, #2
	move	OFFS, #22
	move	A[8], @BP[OFFS++]
	move	ACC, A[8]
	sub	A[6]
	jump	nz, L112
	move	DPC, #2
	move	OFFS, #23
	move	A[9], @BP[OFFS++]
	move	ACC, A[9]
	sub	A[7]
	jump	nz, L112
	move	DPC, #2
	move	OFFS, #24
	move	A[12], @BP[OFFS++]
	move	ACC, A[12]
	move	DPC, #2
	move	OFFS, #25
	move	A[14], @BP[OFFS++]
	or	A[14]
	sub	#0
	jump	z, L105
	move	A[8], #1
	move	A[9], #0
	move	A[10], A[9]
	move	A[11], A[9]
	jump	L111
L112:
	move	A[8], #0
	move	A[9], A[8]
	move	A[10], A[8]
	move	A[11], A[8]
	move	DP[0], #32
L115:
	move	A[1], #0
	move	OFFS,#26
	move	@BP[OFFS], A[1]
	move	OFFS,#27
	move	@BP[OFFS], A[1]
	move	OFFS,#28
	move	@BP[OFFS], A[1]
	move	ACC, A[7]
	and	#-128
	move	OFFS,#29
	move	@BP[OFFS], ACC
	move	GR, DP[1]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	DP[1], GR
	move	DPC,#1
	move	@--DP[1],#31
	move	DPC, #2
	move	OFFS, #26
	move	A[0], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #27
	move	A[1], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #28
	move	A[2], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #29
	move	A[3], @BP[OFFS++]
	call	___lshrsi3
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move	OFFS,#30
	move	@BP[OFFS], A[0]
	move	OFFS,#31
	move	@BP[OFFS], A[1]
	move	OFFS,#32
	move	@BP[OFFS], A[2]
	move	OFFS,#33
	move	@BP[OFFS], A[3]
	move	ACC, A[8]
	add	A[8]
	move	A[12], ACC
	move	OFFS,#34
	move	@BP[OFFS], ACC
	move	ACC, A[9]
	addc	A[9]
	move	OFFS,#35
	move	@BP[OFFS], ACC
	move	ACC, A[10]
	addc	A[10]
	move	OFFS,#36
	move	@BP[OFFS], ACC
	move	ACC, A[11]
	addc	A[11]
	move	OFFS,#37
	move	@BP[OFFS], ACC
	move	ACC, A[0]
	or	A[12]
	move	A[8], ACC
	move	DPC, #2
	move	OFFS, #31
	move	A[14], @BP[OFFS++]
	move	ACC, A[14]
	move	DPC, #2
	move	OFFS, #35
	move	A[1], @BP[OFFS++]
	or	A[1]
	move	A[9], ACC
	move	DPC, #2
	move	OFFS, #32
	move	A[2], @BP[OFFS++]
	move	ACC, A[2]
	move	DPC, #2
	move	OFFS, #36
	move	A[3], @BP[OFFS++]
	or	A[3]
	move	A[10], ACC
	move	DPC, #2
	move	OFFS, #33
	move	A[12], @BP[OFFS++]
	move	ACC, A[12]
	move	DPC, #2
	move	OFFS, #37
	move	A[14], @BP[OFFS++]
	or	A[14]
	move	A[11], ACC
	move	ACC, A[4]
	add	A[4]
	move	A[1], ACC
	move	OFFS,#38
	move	@BP[OFFS], ACC
	move	ACC, A[5]
	addc	A[5]
	move	A[2], ACC
	move	OFFS,#39
	move	@BP[OFFS], ACC
	move	ACC, A[6]
	addc	A[6]
	move	A[3], ACC
	move	OFFS,#40
	move	@BP[OFFS], ACC
	move	ACC, A[7]
	addc	A[7]
	move	OFFS,#41
	move	@BP[OFFS], ACC
	move	A[4], A[1]
	move	A[5], A[2]
	move	A[6], A[3]
	move	A[7], ACC
	move	ACC, A[11]
	move	DPC, #2
	move	OFFS, #23
	move	A[3], @BP[OFFS++]
	sub	A[3]
	jump	c, L118
	move	ACC, A[3]
	sub	A[11]
	jump	nz, L116
	move	ACC, A[10]
	move	DPC, #2
	move	OFFS, #22
	move	A[12], @BP[OFFS++]
	sub	A[12]
	jump	c, L118
	move	ACC, A[12]
	sub	A[10]
	jump	nz, L116
	move	ACC, A[9]
	move	DPC, #2
	move	OFFS, #21
	move	A[14], @BP[OFFS++]
	sub	A[14]
	jump	c, L118
	move	ACC, A[14]
	sub	A[9]
	jump	nz, L116
	move	ACC, A[8]
	move	DPC, #2
	move	OFFS, #20
	move	A[1], @BP[OFFS++]
	sub	A[1]
	jump	nc, L116
	jump	z, L116
L118:
	move	GR, DP[0]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	DP[0], GR
	jump	L115
L116:
	move	GR, DP[1]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	DP[1], GR
	move	DPC,#1
	move	@--DP[1],#1
	move	A[0], A[8]
	move	A[1], A[9]
	move	A[2], A[10]
	move	A[3], A[11]
	call	___lshrsi3
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[6], A[2]
	move	A[7], A[3]
	move	ACC, DP[0]
	move	A[2], DP[0]
	or	A[2]
	sub	#0
	jump	z, L119
	move	DPC, #2
	move	OFFS, #30
	move	A[3], @BP[OFFS++]
	move	OFFS,#8
	move	@BP[OFFS], A[3]
	move	DPC, #2
	move	OFFS, #31
	move	A[8], @BP[OFFS++]
	move	OFFS,#9
	move	@BP[OFFS], A[8]
	move	DPC, #2
	move	OFFS, #32
	move	A[9], @BP[OFFS++]
	move	OFFS,#10
	move	@BP[OFFS], A[9]
	move	DPC, #2
	move	OFFS, #33
	move	A[12], @BP[OFFS++]
	move	OFFS,#11
	move	@BP[OFFS], A[12]
	move	DPC, #2
	move	OFFS, #38
	move	A[14], @BP[OFFS++]
	move	OFFS,#4
	move	@BP[OFFS], A[14]
	move	DPC, #2
	move	OFFS, #39
	move	A[1], @BP[OFFS++]
	move	OFFS,#5
	move	@BP[OFFS], A[1]
	move	DPC, #2
	move	OFFS, #40
	move	A[2], @BP[OFFS++]
	move	OFFS,#6
	move	@BP[OFFS], A[2]
	move	DPC, #2
	move	OFFS, #41
	move	A[3], @BP[OFFS++]
	move	OFFS,#7
	move	@BP[OFFS], A[3]
	move	GR,#0
	move	A[12],GRL
	move	A[13],GRH
	move	A[8], #0
	move	A[9], A[8]
	move	A[10], A[8]
	move	A[11], A[8]
L121:
	move	ACC, A[4]
	add	A[4]
	move	OFFS,#42
	move	@BP[OFFS], ACC
	move	ACC, A[5]
	addc	A[5]
	move	OFFS,#43
	move	@BP[OFFS], ACC
	move	ACC, A[6]
	addc	A[6]
	move	OFFS,#44
	move	@BP[OFFS], ACC
	move	ACC, A[7]
	addc	A[7]
	move	OFFS,#45
	move	@BP[OFFS], ACC
	move	DPC, #2
	move	OFFS, #8
	move	A[14], @BP[OFFS++]
	move	ACC, A[14]
	move	DPC, #2
	move	OFFS, #42
	move	A[1], @BP[OFFS++]
	or	A[1]
	move	A[4], ACC
	move	DPC, #2
	move	OFFS, #9
	move	A[2], @BP[OFFS++]
	move	ACC, A[2]
	move	DPC, #2
	move	OFFS, #43
	move	A[3], @BP[OFFS++]
	or	A[3]
	move	A[5], ACC
	move	DPC, #2
	move	OFFS, #10
	move	A[14], @BP[OFFS++]
	move	ACC, A[14]
	move	DPC, #2
	move	OFFS, #44
	move	A[1], @BP[OFFS++]
	or	A[1]
	move	A[6], ACC
	move	DPC, #2
	move	OFFS, #11
	move	A[2], @BP[OFFS++]
	move	ACC, A[2]
	move	DPC, #2
	move	OFFS, #45
	move	A[3], @BP[OFFS++]
	or	A[3]
	move	A[7], ACC
	move	ACC, A[4]
	move	DPC, #2
	move	OFFS, #20
	move	A[14], @BP[OFFS++]
	sub	A[14]
	move	OFFS,#16
	move	@BP[OFFS], ACC
	move	ACC, A[5]
	move	DPC, #2
	move	OFFS, #21
	move	A[2], @BP[OFFS++]
	subb	A[2]
	move	OFFS,#17
	move	@BP[OFFS], ACC
	move	ACC, A[6]
	move	DPC, #2
	move	OFFS, #22
	move	A[14], @BP[OFFS++]
	subb	A[14]
	move	OFFS,#18
	move	@BP[OFFS], ACC
	move	ACC, A[7]
	move	DPC, #2
	move	OFFS, #23
	move	A[2], @BP[OFFS++]
	subb	A[2]
	move	OFFS,#19
	move	@BP[OFFS], ACC
	move	A[14], #0
	move	OFFS,#12
	move	@BP[OFFS], A[14]
	move	OFFS,#13
	move	@BP[OFFS], A[14]
	move	OFFS,#14
	move	@BP[OFFS], A[14]
	move	OFFS,#15
	move	@BP[OFFS], A[14]
	move	OFFS,#46
	move	@BP[OFFS], A[14]
	move	OFFS,#47
	move	@BP[OFFS], A[14]
	move	OFFS,#48
	move	@BP[OFFS], A[14]
	and	#-128
	move	OFFS,#49
	move	@BP[OFFS], ACC
	move	GR, DP[1]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	DP[1], GR
	move	DPC,#1
	move	@--DP[1],#31
	move	DPC, #2
	move	OFFS, #46
	move	A[0], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #47
	move	A[1], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #48
	move	A[2], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #49
	move	A[3], @BP[OFFS++]
	call	___lshrsi3
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move	OFFS,#50
	move	@BP[OFFS], A[0]
	move	OFFS,#51
	move	@BP[OFFS], A[1]
	move	OFFS,#52
	move	@BP[OFFS], A[2]
	move	OFFS,#53
	move	@BP[OFFS], A[3]
	move	ACC, A[0]
	or	A[1]
	or	A[2]
	or	A[3]
	sub	#0
	jump	nz, L122
	move	A[3], #1
	move	OFFS,#12
	move	@BP[OFFS], A[3]
	move	A[14], #0
	move	OFFS,#13
	move	@BP[OFFS], A[14]
	move	OFFS,#14
	move	@BP[OFFS], A[14]
	move	OFFS,#15
	move	@BP[OFFS], A[14]
L122:
	move	ACC, A[8]
	add	A[8]
	move	OFFS,#54
	move	@BP[OFFS], ACC
	move	ACC, A[9]
	addc	A[9]
	move	OFFS,#55
	move	@BP[OFFS], ACC
	move	ACC, A[10]
	addc	A[10]
	move	OFFS,#56
	move	@BP[OFFS], ACC
	move	ACC, A[11]
	addc	A[11]
	move	OFFS,#57
	move	@BP[OFFS], ACC
	move	DPC, #2
	move	OFFS, #12
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	move	DPC, #2
	move	OFFS, #54
	move	A[2], @BP[OFFS++]
	or	A[2]
	move	A[8], ACC
	move	DPC, #2
	move	OFFS, #13
	move	A[3], @BP[OFFS++]
	move	ACC, A[3]
	move	DPC, #2
	move	OFFS, #55
	move	A[14], @BP[OFFS++]
	or	A[14]
	move	A[9], ACC
	move	DPC, #2
	move	OFFS, #14
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	move	DPC, #2
	move	OFFS, #56
	move	A[2], @BP[OFFS++]
	or	A[2]
	move	A[10], ACC
	move	DPC, #2
	move	OFFS, #15
	move	A[3], @BP[OFFS++]
	move	ACC, A[3]
	move	DPC, #2
	move	OFFS, #57
	move	A[14], @BP[OFFS++]
	or	A[14]
	move	A[11], ACC
	move	DPC, #2
	move	OFFS, #12
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	or	A[3]
	sub	#0
	jump	z, L123
	move	DPC, #2
	move	OFFS, #16
	move	A[2], @BP[OFFS++]
	move	A[4], A[2]
	move	DPC, #2
	move	OFFS, #17
	move	A[3], @BP[OFFS++]
	move	A[5], A[3]
	move	DPC, #2
	move	OFFS, #18
	move	A[14], @BP[OFFS++]
	move	A[6], A[14]
	move	DPC, #2
	move	OFFS, #19
	move	A[1], @BP[OFFS++]
	move	A[7], A[1]
L123:
	move	GRL, A[12]
	move	GRH, A[13]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[12], GRL
	move	A[13], GRH
	move	ACC, DP[0]
	sub	A[12]
	jump	nz, L127
	move	ACC, DP[0]
	sub	A[13]
	jump	z, L125
L127:
	move	A[3], #0
	move	OFFS,#58
	move	@BP[OFFS], A[3]
	move	OFFS,#59
	move	@BP[OFFS], A[3]
	move	OFFS,#60
	move	@BP[OFFS], A[3]
	move	DPC, #2
	move	OFFS, #7
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	and	#-128
	move	OFFS,#61
	move	@BP[OFFS], ACC
	move	GR, DP[1]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	DP[1], GR
	move	DPC,#1
	move	@--DP[1],#31
	move	DPC, #2
	move	OFFS, #58
	move	A[0], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #59
	move	A[1], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #60
	move	A[2], @BP[OFFS++]
	move	DPC, #2
	move	OFFS, #61
	move	A[3], @BP[OFFS++]
	call	___lshrsi3
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move	DPC, #2
	move	OFFS, #4
	move	A[14], @BP[OFFS++]
	move	ACC, A[14]
	add	A[14]
	move	OFFS,#62
	move	@BP[OFFS], ACC
	move	DPC, #2
	move	OFFS, #5
	move	A[14], @BP[OFFS++]
	move	ACC, A[14]
	addc	A[14]
	move	OFFS,#63
	move	@BP[OFFS], ACC
	move	DPC, #2
	move	OFFS, #6
	move	A[14], @BP[OFFS++]
	move	ACC, A[14]
	addc	A[14]
	move	OFFS,#64
	move	@BP[OFFS], ACC
	move	DPC, #2
	move	OFFS, #7
	move	A[14], @BP[OFFS++]
	move	ACC, A[14]
	addc	A[14]
	move	OFFS,#65
	move	@BP[OFFS], ACC
	move	OFFS,#8
	move	@BP[OFFS], A[0]
	move	OFFS,#9
	move	@BP[OFFS], A[1]
	move	OFFS,#10
	move	@BP[OFFS], A[2]
	move	OFFS,#11
	move	@BP[OFFS], A[3]
	move	DPC, #2
	move	OFFS, #62
	move	A[1], @BP[OFFS++]
	move	OFFS,#4
	move	@BP[OFFS], A[1]
	move	DPC, #2
	move	OFFS, #63
	move	A[2], @BP[OFFS++]
	move	OFFS,#5
	move	@BP[OFFS], A[2]
	move	DPC, #2
	move	OFFS, #64
	move	A[3], @BP[OFFS++]
	move	OFFS,#6
	move	@BP[OFFS], A[3]
	move	OFFS,#7
	move	@BP[OFFS], ACC
	jump	L121
L119:
	move	A[8], #0
	move	A[9], A[8]
	move	A[10], A[8]
	move	A[11], A[8]
L125:
	move	DPC, #2
	move	OFFS, #24
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	move	DPC, #2
	move	OFFS, #25
	move	A[2], @BP[OFFS++]
	or	A[2]
	sub	#0
	jump	nz, L111
L130:
	move	A[8], A[4]
	move	A[9], A[5]
	move	A[10], A[6]
	move	A[11], A[7]
	jump	L111
L105:
	move	A[8], #0
	move	A[9], A[8]
	move	A[10], A[8]
	move	A[11], A[8]
L111:
	move	A[0], A[8]
	move	A[1], A[9]
	move	A[2], A[10]
	move	A[3], A[11]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[14],@DP[1]++
	move	DPC,#1h
	move	A[13],@DP[1]++
	move	DPC,#1h
	move	A[12],@DP[1]++
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#66
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#if defined(L_divhi3) || defined(L_udivhi3) || defined(L_modhi3) || defined(L_umodhi3)
 
	.align	1
_unsigned_int_divide:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#12
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	DPC,#1
	move	@--DP[1],A[12]
	move	DPC,#1
	move	@--DP[1],A[13]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GRL, A[0]
	move	GRH, A[1]
	move	A[4],GRL
	move	A[5], GRH
	move	GRL, A[2]
	move	GRH, A[3]
	move	A[6],GRL
	move	A[7], GRH
	move	GR,#12
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	GRL, A[8]
	move	GRH, A[9]
	move	DP[0], GR
	move	DPC, #0
	move	A[9],@DP[0]++
	move	A[10],@DP[0]
	move	GRL, A[9]
	move	GRH, A[10]
	move	OFFS, #10
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	ACC, A[2]
	or	A[7]
	sub	#0
	jump	z, L132
	move	ACC, A[5]
	sub	A[7]
	jump	c, L136
	move	ACC, A[7]
	sub	A[5]
	jump	nz, L134
	move	ACC, A[0]
	sub	A[2]
	jump	nc, L134
	jump	z, L134
L136:
	move	DPC, #2
	move	OFFS, #10
	move	A[10], @BP[OFFS++]
	move	ACC, A[10]
	move	DPC, #2
	move	OFFS, #11
	move	A[11], @BP[OFFS++]
	or	A[11]
	sub	#0
	jump	nz, L132
	move	GRL, A[4]
	move	GRH, A[5]
	move	A[8],GRL
	move	A[9], GRH
	jump	L138
L134:
	move	ACC, A[6]
	sub	A[4]
	jump	nz, L139
	move	ACC, A[7]
	sub	A[5]
	jump	nz, L139
	move	DPC, #2
	move	OFFS, #10
	move	A[4], @BP[OFFS++]
	move	ACC, A[4]
	move	DPC, #2
	move	OFFS, #11
	move	A[5], @BP[OFFS++]
	or	A[5]
	sub	#0
	jump	z, L132
	move	GR,#1
	move	A[8],GRL
	move	A[9],GRH
	jump	L138
L139:
	move	GR,#0
	move	A[8],GRL
	move	A[9],GRH
	move	GR,#16
	move	A[10],GRL
	move	A[11],GRH
	move	GRL, A[10]
	move	GRH, A[11]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
L142:
	move	GRL, A[4]
	move	GRH, A[5]
	move	ACC, #0
	move	GRL, ACC
	move	ACC,GRH
	and	#-128
	move	GRH, ACC
	move	A[2], #15
	move	GR, GR
	move	A[0], GRL
	move	A[1], GRH
	call	___lshrhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	DP[0], GR
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
	move	GR, DP[0]
	move	ACC,GRL
	or	A[0]
	move	GRL, ACC
	move	ACC,GRH
	or	A[1]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	ACC,GRL
	add	A[4]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[5]
	move	GRH, ACC
	move	A[12], GRL
	move	A[13], GRH
	move	GRL, A[12]
	move	GRH, A[13]
	move	A[4],GRL
	move	A[5], GRH
	move	ACC, A[9]
	sub	A[7]
	jump	c, L145
	move	ACC, A[7]
	sub	A[9]
	jump	nz, L143
	move	ACC, A[8]
	sub	A[6]
	jump	nc, L143
	jump	z, L143
L145:
	move	OFFS, #4
	move	DPC, #2
	move	A[10], @BP[OFFS++]
	move	A[11], @BP[OFFS++]
	move	GRL, A[10]
	move	GRH, A[11]
	move	ACC,GRL
	add	#-1
	move	GRL, ACC
	move	ACC,GRH
	addc	#-1
	move	GRH, ACC
	move	A[10], GRL
	move	A[11], GRH
	move	GRL, A[10]
	move	GRH, A[11]
	move	OFFS, #4
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	jump	L142
L143:
	move	A[2], #1
	move	GRL, A[8]
	move	GRH, A[9]
	move	A[0],GRL
	move	A[1], GRH
	call	___lshrhi3
	move	GRL, A[0]
	move	GRH, A[1]
	move	A[10],GRL
	move	A[11], GRH
	move	DPC, #2
	move	OFFS, #4
	move	A[4], @BP[OFFS++]
	move	ACC, A[4]
	move	DPC, #2
	move	OFFS, #5
	move	A[5], @BP[OFFS++]
	or	A[5]
	sub	#0
	jump	z, L146
	move	GR, DP[0]
	move	A[0], GRL
	move	A[1], GRH
	move	GRL, A[12]
	move	GRH, A[13]
	move	DP[0], GR
	move	GR,#0
	move	A[8],GRL
	move	A[9],GRH
	move	GRL, A[8]
	move	GRH, A[9]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
L148:
	move	GRL, A[10]
	move	GRH, A[11]
	move	ACC,GRL
	add	A[10]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[11]
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	or	A[4]
	move	GRL, ACC
	move	ACC,GRH
	or	A[5]
	move	GRH, ACC
	move	A[10], GRL
	move	A[11], GRH
	move	ACC,GRL
	sub	A[6]
	move	GRL, ACC
	move	ACC,GRH
	subb	A[7]
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	OFFS, #8
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	GR,#0
	move	A[12],GRL
	move	A[13],GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	ACC, #0
	move	GRL, ACC
	move	ACC,GRH
	and	#-128
	move	GRH, ACC
	move	A[2], #15
	move	GR, GR
	move	A[0], GRL
	move	A[1], GRH
	call	___lshrhi3
	move	ACC, A[0]
	or	A[1]
	sub	#0
	jump	nz, L149
	move	GR,#1
	move	A[12],GRL
	move	A[13],GRH
L149:
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[12]
	move	GRH, A[13]
	move	ACC,GRL
	or	A[4]
	move	GRL, ACC
	move	ACC,GRH
	or	A[5]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	ACC, A[12]
	or	A[13]
	sub	#0
	jump	z, L150
	move	OFFS, #8
	move	DPC, #2
	move	A[4], @BP[OFFS++]
	move	A[5], @BP[OFFS++]
	move	GRL, A[4]
	move	GRH, A[5]
	move	A[10],GRL
	move	A[11], GRH
L150:
	move	OFFS, #6
	move	DPC, #2
	move	A[4], @BP[OFFS++]
	move	A[5], @BP[OFFS++]
	move	GRL, A[4]
	move	GRH, A[5]
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	OFFS, #6
	move	@BP[OFFS],GRL
	move	@BP[++OFFS],GRH
	move	DPC, #2
	move	OFFS, #4
	move	A[5], @BP[OFFS++]
	move	ACC, A[5]
	sub	A[4]
	jump	nz, L154
	move	DPC, #2
	move	OFFS, #5
	move	A[4], @BP[OFFS++]
	move	ACC, A[4]
	move	DPC, #2
	move	OFFS, #7
	move	A[5], @BP[OFFS++]
	sub	A[5]
	jump	z, L152
L154:
	move	GR, DP[0]
	move	ACC, #0
	move	GRL, ACC
	move	ACC,GRH
	and	#-128
	move	GRH, ACC
	move	A[2], #15
	move	GR, GR
	move	A[0], GRL
	move	A[1], GRH
	call	___lshrhi3
	move	GR, DP[0]
	move	ACC,GRL
	move	GR, DP[0]
	move	A[4], GRL
	move	A[5], GRH
	add	A[4]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[4]
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	DP[0], GR
	jump	L148
L146:
	move	GR,#0
	move	A[8],GRL
	move	A[9],GRH
L152:
	move	DPC, #2
	move	OFFS, #10
	move	A[5], @BP[OFFS++]
	move	ACC, A[5]
	move	DPC, #2
	move	OFFS, #11
	move	A[6], @BP[OFFS++]
	or	A[6]
	sub	#0
	jump	nz, L138
	move	GRL, A[10]
	move	GRH, A[11]
	move	A[8],GRL
	move	A[9], GRH
	jump	L138
L132:
	move	GR,#0
	move	A[8],GRL
	move	A[9],GRH
L138:
	move	GRL, A[8]
	move	GRH, A[9]
	move	A[0],GRL
	move	A[1], GRH
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[13],@DP[1]++
	move	DPC,#1h
	move	A[12],@DP[1]++
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#12
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#if defined(L_divhi3) ||  defined(L_modhi3)
	.align	1
_signed_int_divide:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GRL, A[0]
	move	GRH, A[1]
	move	A[5],GRL
	move	A[6], GRH
	move	GRL, A[2]
	move	GRH, A[3]
	move	A[7],GRL
	move	A[8], GRH
	move	GR,#4
	move	A[9],GRL
	move	A[10],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[9]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[10]
	move	GRH, ACC
	move	A[9], GRL
	move	A[10], GRH
	move	GRL, A[9]
	move	GRH, A[10]
	move	DP[0], GR
	move	DPC, #0
	move	A[9],@DP[0]++
	move	A[10],@DP[0]
	move	ACC, A[8]
	xor	#-128
	move	A[0], ACC
	move	ACC, #-128
	sub	A[0]
	jump	c, L158
	move	ACC, A[8]
	sub	#0
	jump	nz, L159
	move	ACC, #0
	sub	A[7]
	jump	c, L158
L159:
	move	ACC, A[7]
	cpl	
	add	#1
	move	DPC,#1
	move	@--DP[1], PSF
	move	A[2], ACC
	move	ACC, A[3]
	cpl	
	move	DPC,#1h
	move	PSF,@DP[1]++
	addc	#0
	move	A[3], ACC
L158:
	move	GRL, A[5]
	move	GRH, A[6]
	move	A[0],GRL
	move	A[1], GRH
	move	ACC, A[6]
	xor	#-128
	move	A[4], ACC
	move	ACC, #-128
	sub	A[4]
	jump	c, L160
	move	ACC, A[6]
	sub	#0
	jump	nz, L161
	move	ACC, #0
	sub	A[5]
	jump	c, L160
L161:
	move	ACC, A[5]
	cpl	
	add	#1
	move	DPC,#1
	move	@--DP[1], PSF
	move	A[0], ACC
	move	ACC, A[1]
	cpl	
	move	DPC,#1h
	move	PSF,@DP[1]++
	addc	#0
	move	A[1], ACC
L160:
	move	GRL, A[9]
	move	GRH, A[10]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	call	_unsigned_int_divide
	move	GRL, A[0]
	move	GRH, A[1]
	move	A[2],GRL
	move	A[3], GRH
	move	A[11], #0
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move	A[4], #-128
	move	ACC, A[6]
	xor	#-128
	sub	A[4]
	jump	c, L163
	move	ACC, A[6]
	sub	A[11]
	jump	nz, L162
	move	ACC, A[5]
	sub	A[11]
	jump	L162
L163:
	move	A[11], #1
L162:
	move	GRL, A[11]
	move	GRH, #0
	move	A[5], GRL
	move	A[6], GRH
	move	ACC, A[9]
	or	A[10]
	sub	#0
	jump	nz, L164
	move	ACC, A[11]
	sub	#0
	jump	z, L164
	move	ACC, A[2]
	cpl	
	add	#1
	move	DPC,#1
	move	@--DP[1], PSF
	move	A[0], ACC
	move	ACC, A[1]
	cpl	
	move	DPC,#1h
	move	PSF,@DP[1]++
	addc	#0
	move	A[1], ACC
L164:
	move	A[4], #-128
	move	ACC, A[8]
	xor	#-128
	sub	A[4]
	jump	nc, L167
	jump	z, L167
	move	GRL, A[5]
	move	GRH, A[6]
	move	ACC,GRL
	xor	#1
	move	GRL, ACC
	move	ACC,GRH
	move	GRH, ACC
	move	A[5], GRL
	move	A[6], GRH
L167:
	move	ACC, A[9]
	or	A[10]
	sub	#0
	jump	z, L170
	move	ACC, A[5]
	or	A[6]
	sub	#0
	jump	z, L170
	move	ACC, A[0]
	cpl	
	add	#1
	move	DPC,#1
	move	@--DP[1], PSF
	move	A[0], ACC
	move	ACC, A[1]
	cpl	
	move	DPC,#1h
	move	PSF,@DP[1]++
	addc	#0
	move	A[1], ACC
L170:
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#if defined(L_divdi3) || defined(L_moddi3)

	.align	1
_signed_long_divide:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#16
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	DPC,#1
	move	@--DP[1],A[10]
	move	DPC,#1
	move	@--DP[1],A[11]
	move	DPC,#1
	move	@--DP[1],A[12]
	move	DPC,#1
	move	@--DP[1],A[13]
	move	DPC,#1
	move	@--DP[1],A[14]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	OFFS,#4
	move	@BP[OFFS], A[0]
	move	OFFS,#5
	move	@BP[OFFS], A[1]
	move	OFFS,#6
	move	@BP[OFFS], A[2]
	move	OFFS,#7
	move	@BP[OFFS], A[3]
	move	GR,#16
	move	A[4],GRL
	move	A[5],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[4]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[5]
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	OFFS,#8
	move	@BP[OFFS], A[5]
	move	GR,#16
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[9],@DP[0]++
	move	OFFS,#9
	move	@BP[OFFS], A[9]
	move	GR,#16
	move	A[4],GRL
	move	A[5],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[4]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[5]
	move	GRH, ACC
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	OFFS,#10
	move	@BP[OFFS], A[5]
	move	GR,#16
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[9],@DP[0]++
	move	OFFS,#11
	move	@BP[OFFS], A[9]
	move	GR,#16
	move	A[4],GRL
	move	A[5],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[4]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[5]
	move	GRH, ACC
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[13],@DP[0]++
	move	A[14],@DP[0]
	move	A[8], A[0]
	move	A[12], A[1]
	move	A[9], A[1]
	move	A[10], A[2]
	move	A[11], A[3]
	move	ACC, A[3]
	xor	#-128
	move	A[4], ACC
	move	ACC, #-128
	sub	A[4]
	jump	c, L175
	move	ACC, A[3]
	sub	#0
	jump	nz, L176
	move	ACC, #0
	sub	A[2]
	jump	c, L175
	move	ACC, A[2]
	sub	#0
	jump	nz, L176
	move	ACC, #0
	sub	A[12]
	jump	c, L175
	move	ACC, A[12]
	sub	#0
	jump	nz, L176
	move	ACC, #0
	sub	A[0]
	jump	c, L175
L176:
	move	A[4], #0
	move	ACC, A[4]
	move	DPC, #2
	move	OFFS, #4
	move	A[1], @BP[OFFS++]
	sub	A[1]
	move	A[8], ACC
	move	ACC, A[4]
	subb	A[9]
	move	A[9], ACC
	move	ACC, A[4]
	subb	A[10]
	move	A[10], ACC
	move	ACC, A[4]
	subb	A[11]
	move	A[11], ACC
L175:
	move	GRL, A[13]
	move	GRH, A[14]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	DPC, #2
	move	OFFS, #8
	move	A[12], @BP[OFFS++]
	move	A[4], A[12]
	move	DPC, #2
	move	OFFS, #9
	move	A[1], @BP[OFFS++]
	move	A[5], A[1]
	move	DPC, #2
	move	OFFS, #10
	move	A[12], @BP[OFFS++]
	move	A[6], A[12]
	move	DPC, #2
	move	OFFS, #11
	move	A[1], @BP[OFFS++]
	move	A[7], A[1]
	move	ACC, A[1]
	xor	#-128
	move	A[0], ACC
	move	ACC, #-128
	sub	A[0]
	jump	c, L177
	move	ACC, A[1]
	sub	#0
	jump	nz, L178
	move	ACC, #0
	sub	A[12]
	jump	c, L177
	move	ACC, A[12]
	sub	#0
	jump	nz, L178
	move	ACC, #0
	sub	A[5]
	jump	c, L177
	move	ACC, A[5]
	sub	#0
	jump	nz, L178
	move	ACC, #0
	sub	A[4]
	jump	c, L177
L178:
	move	A[0], #0
	move	ACC, A[0]
	move	DPC, #2
	move	OFFS, #8
	move	A[12], @BP[OFFS++]
	sub	A[12]
	move	A[4], ACC
	move	ACC, A[0]
	subb	A[5]
	move	A[5], ACC
	move	ACC, A[0]
	subb	A[6]
	move	A[6], ACC
	move	ACC, A[0]
	subb	A[7]
	move	A[7], ACC
L177:
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[4]
	move	A[0], A[8]
	move	A[1], A[9]
	move	A[2], A[10]
	move	A[3], A[11]
	call	_unsigned_long_divide
	move	A[9], A[0]
	move	A[4], A[0]
	move	A[5], A[1]
	move	A[6], A[2]
	move	A[7], A[3]
	move	A[2], #0
	move	GR, DP[1]
	move	ACC,GRL
	add	#6
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move	A[8], #-128
	move	DPC, #2
	move	OFFS, #7
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	xor	#-128
	sub	A[8]
	jump	c, L180
	move	ACC, A[1]
	sub	A[2]
	jump	nz, L179
	move	DPC, #2
	move	OFFS, #6
	move	A[8], @BP[OFFS++]
	move	ACC, A[8]
	sub	A[2]
	jump	nz, L179
	move	DPC, #2
	move	OFFS, #5
	move	A[1], @BP[OFFS++]
	move	ACC, A[1]
	sub	A[2]
	jump	nz, L179
	move	DPC, #2
	move	OFFS, #4
	move	A[8], @BP[OFFS++]
	move	ACC, A[8]
	sub	A[2]
	jump	L179
L180:
	move	A[2], #1
L179:
	move	GRL, A[2]
	move	GRH, #0
	move	A[0], GRL
	move	A[1], GRH
	move	ACC, A[13]
	or	A[14]
	sub	#0
	jump	nz, L181
	move	ACC, A[2]
	sub	#0
	jump	z, L181
	move	A[8], #0
	move	OFFS,#12
	move	@BP[OFFS], A[8]
	move	OFFS,#13
	move	@BP[OFFS], A[8]
	move	OFFS,#14
	move	@BP[OFFS], A[8]
	move	OFFS,#15
	move	@BP[OFFS], A[8]
	move	ACC, A[8]
	sub	A[9]
	move	A[4], ACC
	move	ACC, A[8]
	subb	A[5]
	move	A[5], ACC
	move	ACC, A[8]
	subb	A[6]
	move	A[6], ACC
	move	ACC, A[8]
	subb	A[7]
	move	A[7], ACC
L181:
	move	A[8], #-128
	move	DPC, #2
	move	OFFS, #11
	move	A[9], @BP[OFFS++]
	move	ACC, A[9]
	xor	#-128
	sub	A[8]
	jump	nc, L184
	jump	z, L184
	move	GRL, A[0]
	move	GRH, A[1]
	move	ACC,GRL
	xor	#1
	move	GRL, ACC
	move	ACC,GRH
	move	GRH, ACC
	move	A[0], GRL
	move	A[1], GRH
L184:
	move	ACC, A[13]
	or	A[14]
	sub	#0
	jump	z, L187
	move	ACC, A[0]
	or	A[1]
	sub	#0
	jump	z, L187
	move	A[8], #0
	move	ACC, A[8]
	sub	A[4]
	move	A[4], ACC
	move	ACC, A[8]
	subb	A[5]
	move	A[5], ACC
	move	ACC, A[8]
	subb	A[6]
	move	A[6], ACC
	move	ACC, A[8]
	subb	A[7]
	move	A[7], ACC
L187:
	move	A[0], A[4]
	move	A[1], A[5]
	move	A[2], A[6]
	move	A[3], A[7]
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[14],@DP[1]++
	move	DPC,#1h
	move	A[13],@DP[1]++
	move	DPC,#1h
	move	A[12],@DP[1]++
	move	DPC,#1h
	move	A[11],@DP[1]++
	move	DPC,#1h
	move	A[10],@DP[1]++
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#16
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_udivdi3

	.align	1
	.global	___udivsi3
___udivsi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GR,#4
	move	A[4],GRL
	move	A[5],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[4]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[5]
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	DP[0], GR
	move	DPC, #0
	move	A[4],@DP[0]++
	move	GR,#4
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[6],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[7],@DP[0]++
	move	GR, #1
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[4]
	call	_unsigned_long_divide
	move	GR, DP[1]
	move	ACC,GRL
	add	#6
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_divdi3

	.align	1
	.global	___divsi3
___divsi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GR,#4
	move	A[4],GRL
	move	A[5],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[4]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[5]
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	DP[0], GR
	move	DPC, #0
	move	A[4],@DP[0]++
	move	GR,#4
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[6],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[7],@DP[0]++
	move	GR, #1
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[4]
	call	_signed_long_divide
	move	GR, DP[1]
	move	ACC,GRL
	add	#6
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_udivhi3

	.align	1
	.global	___udivhi3
___udivhi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GR, #1
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	call	_unsigned_int_divide
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_divhi3

	.align	1
	.global	___divhi3
___divhi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GR, #1
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	call	_signed_int_divide
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_umoddi3

	.align	1
	.global	___umodsi3
___umodsi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GR,#4
	move	A[4],GRL
	move	A[5],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[4]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[5]
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	DP[0], GR
	move	DPC, #0
	move	A[4],@DP[0]++
	move	GR,#4
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[6],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[7],@DP[0]++
	move	GR, #0
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[4]
	call	_unsigned_long_divide
	move	GR, DP[1]
	move	ACC,GRL
	add	#6
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_moddi3

	.align	1
	.global	___modsi3
___modsi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	DPC,#1
	move	@--DP[1],A[4]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[8]
	move	DPC,#1
	move	@--DP[1],A[9]
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GR,#4
	move	A[4],GRL
	move	A[5],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[4]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[5]
	move	GRH, ACC
	move	A[4], GRL
	move	A[5], GRH
	move	GRL, A[4]
	move	GRH, A[5]
	move	DP[0], GR
	move	DPC, #0
	move	A[4],@DP[0]++
	move	GR,#4
	move	A[8],GRL
	move	A[9],GRH
	move	GR, BP
	move	ACC,GRL
	add	A[8]
	move	GRL, ACC
	move	ACC,GRH
	addc	A[9]
	move	GRH, ACC
	move	A[8], GRL
	move	A[9], GRH
	move	ACC,GRL
	add	#1
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[5],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[6],@DP[0]++
	move	GRL, A[8]
	move	GRH, A[9]
	move	ACC,GRL
	add	#3
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[0], GR
	move	DPC, #0
	move	A[7],@DP[0]++
	move	GR, #0
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	DPC,#1
	move	@--DP[1],A[7]
	move	DPC,#1
	move	@--DP[1],A[6]
	move	DPC,#1
	move	@--DP[1],A[5]
	move	DPC,#1
	move	@--DP[1],A[4]
	call	_signed_long_divide
	move	GR, DP[1]
	move	ACC,GRL
	add	#6
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	A[9],@DP[1]++
	move	DPC,#1h
	move	A[8],@DP[1]++
	move	DPC,#1h
	move	A[7],@DP[1]++
	move	DPC,#1h
	move	A[6],@DP[1]++
	move	DPC,#1h
	move	A[5],@DP[1]++
	move	DPC,#1h
	move	A[4],@DP[1]++
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_umodhi3

	.align	1
	.global	___umodhi3
___umodhi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GR, #0
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	call	_unsigned_int_divide
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#ifdef L_modhi3

	.align	1
	.global	___modhi3
___modhi3:
	move	AP, #15
	move	GR, DP[1]
	move	ACC,GRL
	sub	#4
	move	GRL, ACC
	move	ACC,GRH
	subb	#0
	move	GRH, ACC
	move	DP[1], GR
	move	GR, BP
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	BP, GR
	move	GR, DP[0]
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	move	GR, @SP--
	move	@--DP[1], GRH
	move	@--DP[1], GRL
	move	@++SP, GR
	move	GR, #0
	move	@--DP[1],GRH
	move	@--DP[1],GRL 
	call	_signed_int_divide
	move	GR, DP[1]
	move	ACC,GRL
	add	#2
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	move 	DPC, #1 
	move 	NUL, @DP[1]++ 
	move 	NUL, @DP[1]++ 
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	DP[0], GR
	move	DPC,#1h
	move	GRL,@DP[1]++
	move	GRH,@DP[1]++
	move	BP, GR
	move	GR, DP[1]
	move	ACC,GRL
	add	#4
	move	GRL, ACC
	move	ACC,GRH
	addc	#0
	move	GRH, ACC
	move	DP[1], GR
	ret	
#endif

#endif

