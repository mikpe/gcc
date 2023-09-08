/* calloc -- allocate memory which has been initialized to zero.
   This function is in the public domain. */

/*

@deftypefn Supplemental void* calloc (size_t @var{nelem}, size_t @var{elsize})

Uses @code{malloc} to allocate storage for @var{nelem} objects of
@var{elsize} bytes each, then zeros the memory.

@end deftypefn

*/
 
#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: calloc.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#include "ansidecl.h"
#include <stddef.h>

/* For systems with larger pointers than ints, this must be declared.  */
PTR malloc (size_t);
void bzero (PTR, size_t);

PTR
calloc (size_t nelem, size_t elsize)
{
  register PTR ptr;  

  if (nelem == 0 || elsize == 0)
    nelem = elsize = 1;
  
  ptr = malloc (nelem * elsize);
  if (ptr) bzero (ptr, nelem * elsize);
  
  return ptr;
}
