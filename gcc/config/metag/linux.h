/* Definitions of target machine for GNU compiler,
   for Meta Linux-based GNU systems.
   Copyright (C) 2005, 2007 Free Software Foundation, Inc.
   Contributed by Imagination Technologies Ltd (toolkit@metagence.com)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#undef CPP_SUBTARGET_SPEC
#define CPP_SUBTARGET_SPEC                              \
  "%{posix:-D_POSIX_SOURCE} "                           \
  "%{pthread:-D_REENTRANT -D_PTHREADS} "

#define NO_IMPLICIT_EXTERN_C

#define CPLUSPLUS_CPP_SPEC "-D_GNU_SOURCE %(cpp)"

#undef TARGET_C99_FUNCTIONS
#define TARGET_C99_FUNCTIONS    1
