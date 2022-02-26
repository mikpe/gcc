// crti.s for ELF

//   Copyright (C) 1992, 1998, 1999 Free Software Foundation, Inc.
//   Written By David Vinayak Henkel-Wallace, June 1992
//
// This file is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
// later version.
//
// In addition to the permissions in the GNU General Public License, the
// Free Software Foundation gives you unlimited permission to link the
// compiled version of this file with other programs, and to distribute
// those programs without any restriction coming from the use of this
// file.  (The General Public License restrictions do apply in other
// respects; for example, they cover modification of the file, and
// distribution when not linked into another program.)
//
// This file is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
//    As a special exception, if you link this library with files
//    compiled with GCC to produce an executable, this does not cause
//    the resulting executable to be covered by the GNU General Public License.
//    This exception does not however invalidate any other reasons why
//    the executable file might be covered by the GNU General Public License.
//

// This file just make a stack frame for the contents of the .fini and
// .init sections.  Users may put any desired instructions in those
// sections.

	.file		"crti.asm"
        .section .ctors, "a", @progbits

        .p2align 2
__CTOR_LIST__:
        .word 0

/* DTOR list always begins with an ignored function pointer *
 * may hole 0, -1 or count of function pointers after it.   */
        .section .dtors, "a", @progbits

        .global __DTOR_LIST__
        .p2align 2
__DTOR_LIST__:
        .word 0

/*.eh_frame contains exception handler information frames from all*
 * linked C++ modules and is aligned to begin with a long zero.*
 * __EH_FRAME_BEGIN__ marks the beginning of the list          */
        .section .eh_frame, "a", @progbits

        .p2align 5
__EH_FRAME_BEGIN__:

	.text
	.global __register_frame_info_bases



        .section	".init"
	.globl	_init
	.type	_init,#function
	.p2align	2
_init:
	// These nops are here to align the end of this code with a 16 byte
	// boundary.  The linker will start inserting code into the .init
	// section at such a boundary.
	nop
	nop
	nop
	nop
	nop
	nop
        allocframe(#8)
        memw (sp) = r27

 1:
        r27.h = #HI (__CTOR_END__)
        r27.l = #LO (__CTOR_END__)

 2:
	r27 = add (r27, #-4)
/* save r1 as into callee saved r27 for next iteration*/
        r0 = memw (r27)
        { p0 = cmp.eq (r0, #0)
	  if (p0.new) jump:nt 3f }

        callr r0
	jump 2b

3:
        { r27 = memw (sp)
          deallocframe }
        jumpr lr

        .size _init, . - _init

	.section .ctors, "a", @progbits





	.section	".fini"
	.globl	_fini
	.type	_fini,#function
	.p2align	2
_fini:
	nop
	nop
	nop
	nop
	nop
	nop
	allocframe (#8)
	memw (sp) = r27

	r27.h = #HI (__DTOR_LIST__)
	r27.l = #LO (__DTOR_LIST__)

1:
/* the first entry in the destructor list   */
/* is the ignored function ptr hence skip it*/
	r27 = add (r27, #4)
	r0 = memw (r27)
	{ p0 = cmp.eq (r0, #0)
	  if (p0.new) jump:nt 2f }

	callr r0
	jump 1b

2:
	{ r27 = memw (sp)
	  deallocframe }
	jumpr lr

	.size _fini, . - _fini

