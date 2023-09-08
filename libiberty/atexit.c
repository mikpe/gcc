/* Wrapper to implement ANSI C's atexit using SunOS's on_exit. */
/* This function is in the public domain.  --Mike Stump. */

/*

@deftypefn Supplemental int atexit (void (*@var{f})())

Causes function @var{f} to be called at exit.  Returns 0.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: atexit.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include "config.h"

#ifdef HAVE_ON_EXIT

int
atexit(void (*f)(void))
{
  /* If the system doesn't provide a definition for atexit, use on_exit
     if the system provides that.  */
  on_exit (f, 0);
  return 0;
}

#endif
