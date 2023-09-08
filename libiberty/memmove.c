/* Wrapper to implement ANSI C's memmove using BSD's bcopy. */
/* This function is in the public domain.  --Per Bothner. */

/*

@deftypefn Supplemental void* memmove (void *@var{from}, const void *@var{to}, size_t @var{count})

Copies @var{count} bytes from memory area @var{from} to memory area
@var{to}, returning a pointer to @var{to}.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: memmove.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include <ansidecl.h>
#include <stddef.h>

void bcopy (const void*, void*, size_t);

PTR
memmove (PTR s1, const PTR s2, size_t n)
{
  bcopy (s2, s1, n);
  return s1;
}
