/* kOpenSolaris-based GNU systems support needed only by D front-end.
   Copyright (C) 2017-2025 Free Software Foundation, Inc.

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tm_d.h"
#include "d/d-target.h"
#include "d/d-target-def.h"

/* Implement TARGET_D_OS_VERSIONS for kOpenSolaris targets.  */

static void
kopensolaris_d_os_builtins (void)
{
  d_add_builtin_version ("Posix");
  d_add_builtin_version ("Solaris");
  d_add_builtin_version ("CRuntime_Glibc");
}

/* Handle a call to `__traits(getTargetInfo, "objectFormat")'.  */

static tree
kopensolaris_d_handle_target_object_format (void)
{
  const char *objfmt = "elf";

  return build_string_literal (strlen (objfmt) + 1, objfmt);
}

/* Implement TARGET_D_REGISTER_OS_TARGET_INFO for kOpenSolaris targets.  */

static void
kopensolaris_d_register_target_info (void)
{
  const struct d_target_info_spec handlers[] = {
    { "objectFormat", kopensolaris_d_handle_target_object_format },
    { NULL, NULL },
  };

  d_add_target_info_handlers (handlers);
}

#undef TARGET_D_OS_VERSIONS
#define TARGET_D_OS_VERSIONS kopensolaris_d_os_builtins

#undef TARGET_D_REGISTER_OS_TARGET_INFO
#define TARGET_D_REGISTER_OS_TARGET_INFO kopensolaris_d_register_target_info

struct gcc_targetdm targetdm = TARGETDM_INITIALIZER;
