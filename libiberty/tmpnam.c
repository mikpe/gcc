/*

@deftypefn Supplemental char* tmpnam (char *@var{s})

This function attempts to create a name for a temporary file, which
will be a valid file name yet not exist when @code{tmpnam} checks for
it.  @var{s} must point to a buffer of at least @code{L_tmpnam} bytes,
or be @code{NULL}.  Use of this function creates a security risk, and it must
not be used in new projects.  Use @code{mkstemp} instead.

@end deftypefn

*/
#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: tmpnam.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif


#include <stdio.h>

#ifndef L_tmpnam
#define L_tmpnam 100
#endif
#ifndef P_tmpdir
#define P_tmpdir "/usr/tmp"
#endif

static char tmpnam_buffer[L_tmpnam];
static int tmpnam_counter;

extern int getpid (void);

char *
tmpnam (char *s)
{
  int pid = getpid ();

  if (s == NULL)
    s = tmpnam_buffer;

  /*  Generate the filename and make sure that there isn't one called
      it already.  */

  while (1)
    {
      FILE *f;
      sprintf (s, "%s/%s%x.%x", P_tmpdir, "t", pid, tmpnam_counter);
      f = fopen (s, "r");
      if (f == NULL)
	break;
      tmpnam_counter++;
      fclose (f);
    }

  return s;
}
