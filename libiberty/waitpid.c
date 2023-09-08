/*

@deftypefn Supplemental int waitpid (int @var{pid}, int *@var{status}, int)

This is a wrapper around the @code{wait} function.  Any ``special''
values of @var{pid} depend on your implementation of @code{wait}, as
does the return value.  The third argument is unused in @libib{}.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: waitpid.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "ansidecl.h"

/* On some systems (such as WindISS), you must include <sys/types.h>
   to get the definition of "pid_t" before you include <sys/wait.h>.  */
#include <sys/types.h>

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

pid_t
waitpid (pid_t pid, int *stat_loc, int options ATTRIBUTE_UNUSED)
{
  for (;;)
    {
      int wpid = wait(stat_loc);
      if (wpid == pid || wpid == -1)
	return wpid;
    }
}
