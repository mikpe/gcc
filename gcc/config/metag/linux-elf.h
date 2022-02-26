/* Definitions of target machine for GNU compiler
   for Meta Linux-based GNU systems using ELF.
   Copyright (C) 2004, 2007 Free Software Foundation, Inc.
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

#undef TARGET_VERSION
#define TARGET_VERSION fputs (" (Metag Linux GNU)", stderr)

#undef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS                           \
  { "elf_dynamic_linker", ELF_DYNAMIC_LINKER },

#define ELF_DYNAMIC_LINKER      "/lib/ld-uClibc.so.0"

#define TARGET_OS_CPP_BUILTINS()                        \
  do                                                    \
  {                                                     \
    LINUX_TARGET_OS_CPP_BUILTINS ();                    \
  }                                                     \
while (0)

#undef LIB_SPEC
#define LIB_SPEC                                        \
  "%{pthread:-lpthread} "                               \
  "%{shared:-lc} "                                      \
  "%{!shared: %{profile:-lc_p} %{!profile:-lc}} "

#undef TYPE_OPERAND_FMT
#define TYPE_OPERAND_FMT        "@%s"

#undef GLOBAL_ASM_OP
#define GLOBAL_ASM_OP           "\t.global\t"


/* Provide a STARTFILE_SPEC appropriate for ELF.  Here we add the
   (even more) magical crtbegin.o file which provides part of the
   support for getting C++ file-scope static object constructed before
   entering `main'. 

   Don't bother seeing crtstuff.c -- there is absolutely no hope of
   getting that file to understand multiple GPs.  GNU Libc provides a
   hand-coded version that is used on Linux; it could be copied here
   if there is ever a need. */
   
#undef  STARTFILE_SPEC
#define STARTFILE_SPEC                                          \
  "%{!shared: "                                                 \
      "%{pg:crt1.o%s} %{!pg:%{p:crt1.o%s} "                     \
                        "%{!p:%{profile:crt1.o%s} "             \
                          "%{!profile:crt1.o%s}}}} "            \
  "crti.o%s %{!shared:crtbegin.o%s} %{shared:crtbeginS.o%s} "

/* Provide a ENDFILE_SPEC appropriate for ELF.  Here we tack on the
   magical crtend.o file which provides part of the support for
   getting C++ file-scope static object constructed before entering
   `main', followed by a normal ELF "finalizer" file, `crtn.o'.  */

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC                                            \
  "%{!shared:crtend.o%s} %{shared:crtendS.o%s} crtn.o%s"

