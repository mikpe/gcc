/* Provide a version vfprintf in terms of _doprnt.
   By Kaveh Ghazi  (ghazi@caip.rutgers.edu)  3/29/98
   Copyright (C) 1998 Free Software Foundation, Inc.
 */

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: vfprintf.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include "ansidecl.h"
#include <stdarg.h>
#include <stdio.h>
#undef vfprintf

int
vfprintf (FILE *stream, const char *format, va_list ap)
{
  return _doprnt (format, ap, stream);
}
