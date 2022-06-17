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

; The MAXQ processor automatically starts execution at address 8000h in the utility ROM
; From the utility ROM, the control should be transferred to program memory at 0000h.

; On MAXQ program memory and data memory are separate from one another.
; Since program memory is read only and normally implemented using nonvolatile memory,
; all static data should be present in the program memory and it should be copied to the actual
; data memory before executing the program.

; Compiler and binutils view as flat memory model in which code section is placed after the data
; section. We write the start up function in the data section so that it will be placed at 0000h.
; In the start up function we copy the static data to the
; actual data memory at the same location.

; Only when the program control is in utility ROM we can read the program memory as data.
; Since static data is present in the program memory, we need to place the following instructions
; in the utility ROM at 8002h.

; at 80002h:
;           move GRL, @DP[0]
;           ret


.data
start:
; size of this function is 3ch bytes
; 803ah points to 003ch in the program memory while executing from the utility ROM.
        move DP[0], #803ch
        move DP[1], #3ch

; result of 'end_data-60' will be in terms of words.
        move LC[0], end_data-60
        
loop_top:
        move DPC, #0h

; call to the utility ROM.
        lcall #8002h
        move @DP[1], GRL 
        move DPC, #0h
        move NUL, @DP[0]++
        move DPC, #1h
        move NUL, @DP[1]++
        move DPC, #0h

; call to the utility ROM.
        lcall #8002h
        move @DP[1], GRL
        move DPC, #0h
        move NUL, @DP[0]++
        move DPC, #1h
        move NUL, @DP[1]++

        djnz LC[0], loop_top 
        move DPC, #0h
        move DP[1], #8000h
        lcall _main
        lcall _exit

.text
end_data:
	NOP

.global _exit
_exit:
        NOP
        ret

.global _abort
_abort:
       NOP
       ret
