/* memcpy (the standard C function)
   This function is in the public domain.  */

/*

@deftypefn Supplemental void* memcpy (void *@var{out}, const void *@var{in}, size_t @var{length})

Copies @var{length} bytes from memory region @var{in} to region
@var{out}.  Returns a pointer to @var{out}.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: memcpy.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include <ansidecl.h>
#include <stddef.h>

void bcopy (const void*, void*, size_t);

PTR
memcpy (PTR out, const PTR in, size_t length)
{
    bcopy(in, out, length);
    return out;
}
