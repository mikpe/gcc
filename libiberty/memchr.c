/*

@deftypefn Supplemental void* memchr (const void *@var{s}, int @var{c}, size_t @var{n})

This function searches memory starting at @code{*@var{s}} for the
character @var{c}.  The search only ends with the first occurrence of
@var{c}, or after @var{length} characters; in particular, a null
character does not terminate the search.  If the character @var{c} is
found within @var{length} characters of @code{*@var{s}}, a pointer
to the character is returned.  If @var{c} is not found, then @code{NULL} is
returned.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: memchr.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include <ansidecl.h>
#include <stddef.h>

PTR
memchr (register const PTR src_void, int c, size_t length)
{
  const unsigned char *src = (const unsigned char *)src_void;
  
  while (length-- > 0)
  {
    if (*src == c)
     return (PTR)src;
    src++;
  }
  return NULL;
}
