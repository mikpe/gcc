/* Emulate vfork using just plain fork, for systems without a real vfork.
   This function is in the public domain. */

/*

@deftypefn Supplemental int vfork (void)

Emulates @code{vfork} by calling @code{fork} and returning its value.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: vfork.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include "ansidecl.h"

extern int fork (void);

int
vfork (void)
{
  return (fork ());
}
