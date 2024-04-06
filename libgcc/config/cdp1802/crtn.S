# crtn.S for the RCA CDP1802 processor.
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

# This file just makes sure that the .fini and .init sections do in
# fact return.  Users may put any desired instructions in those sections.
# This file is the last thing linked into any executable.

	.file	"crtn.S"
	.ident	"GNU C crtn.o"

.macro	POP	regno
	lda	2
	phi	\regno
	lda	2
	plo	\regno
.endm

.macro	HALFPOP	regno
	lda	2
	phi	\regno
	ldn	2
	plo	\regno
.endm

	.section .init
	# pop the callee-save registers off the stack
	inc	2
	POP	11
	POP	12
	POP	13
	POP	14
	HALFPOP	15
	# return
	sep	5

	.section .fini
	# pop the callee-save registers off the stack
	inc	2
	POP	11
	POP	12
	POP	13
	POP	14
	HALFPOP	15
	# return
	sep	5
