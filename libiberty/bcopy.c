/* bcopy -- copy memory regions of arbitary length

@deftypefn Supplemental void bcopy (char *@var{in}, char *@var{out}, int @var{length})

Copies @var{length} bytes from memory region @var{in} to region
@var{out}.  The use of @code{bcopy} is deprecated in new programs.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: bcopy.c 34cc8511e100 2007/11/30 19:16:06 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include <stddef.h>

void
bcopy (const void *src, void *dest, size_t len)
{
  if (dest < src)
    {
      const char *firsts = (const char *) src;
      char *firstd = (char *) dest;
      while (len--)
	*firstd++ = *firsts++;
    }
  else
    {
      const char *lasts = (const char *)src + (len-1);
      char *lastd = (char *)dest + (len-1);
      while (len--)
        *lastd-- = *lasts--;
    }
}
