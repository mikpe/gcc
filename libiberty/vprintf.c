/*

@deftypefn Supplemental int vprintf (const char *@var{format}, va_list @var{ap})
@deftypefnx Supplemental int vfprintf (FILE *@var{stream}, const char *@var{format}, va_list @var{ap})
@deftypefnx Supplemental int vsprintf (char *@var{str}, const char *@var{format}, va_list @var{ap})

These functions are the same as @code{printf}, @code{fprintf}, and
@code{sprintf}, respectively, except that they are called with a
@code{va_list} instead of a variable number of arguments.  Note that
they do not call @code{va_end}; this is the application's
responsibility.  In @libib{} they are implemented in terms of the
nonstandard but common function @code{_doprnt}.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: vprintf.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include <ansidecl.h>
#include <stdarg.h>
#include <stdio.h>
#undef vprintf
int
vprintf (const char *format, va_list ap)
{
  return vfprintf (stdout, format, ap);
}
