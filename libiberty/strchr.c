/* Portable version of strchr()
   This function is in the public domain.  */

/*

@deftypefn Supplemental char* strchr (const char *@var{s}, int @var{c})

Returns a pointer to the first occurrence of the character @var{c} in
the string @var{s}, or @code{NULL} if not found.  If @var{c} is itself the
null character, the results are undefined.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: strchr.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include <ansidecl.h>

char *
strchr (register const char *s, int c)
{
  do {
    if (*s == c)
      {
	return (char*)s;
      }
  } while (*s++);
  return (0);
}
