# crti.S for the RCA CDP1802 processor.
# Copyright (C) 2024 Free Software Foundation, Inc.
# Contributed by Mikael Pettersson <mikpelinux@gmail.com>.
#
# This file is part of GCC.
#
# GCC is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GCC is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# Under Section 7 of GPL version 3, you are granted additional
# permissions described in the GCC Runtime Library Exception, version
# 3.1, as published by the Free Software Foundation.
#
# You should have received a copy of the GNU General Public License and
# a copy of the GCC Runtime Library Exception along with this program;
# see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
# <http://www.gnu.org/licenses/>.

# This file creates a stack frame for the contents of the .fini and
# .init sections.  Users may put any desired instructions in those
# sections.

	.file	"crti.S"
	.ident	"GNU C crti.o"

.macro	PUSH	regno
	glo	\regno
	stxd
	ghi	\regno
	stxd
.endm

	.section .init
	.globl	_init
	.type	_init,@function
_init:
	# push the callee-save registers on the stack
	sex	2
	PUSH	15
	PUSH	14
	PUSH	13
	PUSH	12
	PUSH	11

	.section .fini
	.globl	_fini
	.type	_fini,@function
_fini:
	# push the callee-save registers on the stack
	sex	2
	PUSH	15
	PUSH	14
	PUSH	13
	PUSH	12
	PUSH	11
