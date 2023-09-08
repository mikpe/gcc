/* Portable version of bzero for systems without it.
   This function is in the public domain.  */

/*

@deftypefn Supplemental void bzero (char *@var{mem}, int @var{count})

Zeros @var{count} bytes starting at @var{mem}.  Use of this function
is deprecated in favor of @code{memset}.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: bzero.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include <stddef.h>

extern void *memset(void *, int, size_t);

void
bzero (void *to, size_t count)
{
  memset (to, 0, count);
}
