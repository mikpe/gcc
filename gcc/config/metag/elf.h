/* Definitions of target machine for GNU compiler,
   for Meta Linux-based GNU systems.
   Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007
   Free Software Foundation, Inc.
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

#ifndef OBJECT_FORMAT_ELF
#error elf.h included before elfos.h
#endif

#undef BSS_SECTION_ASM_OP

/* Dots in labels are not allowed. */

#define NO_DOT_IN_LABEL 1

#define ASM_PN_FORMAT   "%s___%lu"

#undef  DBX_DEBUGGING_INFO

#undef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG

#undef ASM_FINAL_SPEC

#ifdef MINIM_DEFAULT
#define DEFAULT_MINIM_LINK_SPEC "%{!mno-minim:%{mmetac=2.1:--minim}} "
#else
#define DEFAULT_MINIM_LINK_SPEC
#endif

#ifdef METAG_LINK_GLOBAL
#define LINK_MACHINE_TYPE "-m elf32metag_global "
#else
#define LINK_MACHINE_TYPE "-m elf32metag "
#endif

#undef LINK_SPEC
#define LINK_SPEC                                                       \
  LINK_MACHINE_TYPE                                                     \
  "%{shared:-shared} "                                                  \
  "-init=__init -fini=__fini "                                          \
  "%{mminim:%{mmetac=2.1:--minim}%{!mmetac=2.1:%eMiniM mode is only available on a META 2.1}} "\
  DEFAULT_MINIM_LINK_SPEC                                               \
  "%{!shared: "                                                         \
    "%{!static: "                                                       \
      "%{rdynamic:-export-dynamic} "                                    \
      "%{!dynamic-linker:-dynamic-linker %(elf_dynamic_linker)}} "      \
    "%{static:-static}} "

#ifndef ASM_COMMENT_START
#define ASM_COMMENT_START       "!"
#endif

#define ASM_IDENTIFY_LANGUAGE(FILE)                                     \
  fprintf (FILE, "%s \"GCC (%s) %s\"\n", IDENT_ASM_OP,                  \
           lang_identify (), version_string)


#undef ASM_OUTPUT_CASE_LABEL

/* For PIC code we need to explicitly specify (PLT) and (GOT) relocs.  */
#define NEED_PLT_RELOC  flag_pic
#define NEED_GOT_RELOC  flag_pic

#ifndef SUBTARGET_CPP_SPEC
#define SUBTARGET_CPP_SPEC  "-D__ELF__"
#endif
