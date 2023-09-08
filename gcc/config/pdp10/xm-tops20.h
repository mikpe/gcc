/* Configuration for GCC for TOPS-20 hosts.
   Copyright (C) 2001 Free Software Foundation, Inc.
   Contributed by Lars Brinkhoff (lars@nocrew.org), funded by XKL, LLC.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef __XM_TOPS20_H__
#define __XM_TOPS20_H__

/* We support both "/" and ">" since everybody tests both but we
   default to "/".  This is important because if GCC produces TOPS-20
   paths containing angle brackets, make and configure may get
   confused. */
#define DIR_SEPARATOR '/'
#define DIR_SEPARATOR_2 '>'

/* A C string representing the suffix for object files in TOPS-20.  */
#define HOST_OBJECT_SUFFIX ".REL"

/* A C string representing the suffix for executable files in TOPS-20.  */
#define HOST_EXECUTABLE_SUFFIX ".EXE"

/* A C string controlling C preprocessor defines */
/* TODO: should probably set -DTOPS_20 and variations here */

#if 0
#undef CPP_SPEC
#define CPP_SPEC       "%{posix:-D_POSIX_SOURCE} %{bsd:-D_BSD_SOURCE}"
#endif

/* C strings controlling the linker */

/* any extra stuff embedded in the link command */
#undef	LINK_SPEC
#define LINK_SPEC      ""

/* standard C libraries - we do not have any of these yet - NULL out for now */
#undef LIB_SPEC
#if 0
#define LIB_SPEC       "%{!shared:%{g*:-lg} %{!p:%{!pg:-lc}}%{p:-lc_p}%{pg:-lc_p}}"
#else
#define LIB_SPEC       ""
#endif

/* an object file guaranteed to be loaded BEFORE any others */
#undef	STARTFILE_SPEC
#define STARTFILE_SPEC "%{!mdual-stack:crtini%O%s}%{mdual-stack:crtini-dual-stack%O%s}"

/* an object file guaranteed to be loaded AFTER all others */
#undef	ENDFILE_SPEC
#define ENDFILE_SPEC   "%{!mdual-stack:crtfin%O%s}%{mdual-stack:crtfin-dual-stack%O%s}"

#endif /* EOF xm-tops20.h */
