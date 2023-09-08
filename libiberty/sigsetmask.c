/* Version of sigsetmask.c
   Written by Steve Chamberlain (sac@cygnus.com).
   Contributed by Cygnus Support.
   This file is in the public doamin. */

/*

@deftypefn Supplemental int sigsetmask (int @var{set})

Sets the signal mask to the one provided in @var{set} and returns
the old mask (which, for libiberty's implementation, will always
be the value @code{1}).

@end deftypefn

*/

#define _POSIX_SOURCE
#include <ansidecl.h>
/* Including <sys/types.h> seems to be needed by ISC. */
#include <sys/types.h>
#include <signal.h>

extern void abort (void) ATTRIBUTE_NORETURN;

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: sigsetmask.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

#ifdef SIG_SETMASK
int
sigsetmask (int set)
{
    sigset_t new_sig;
    sigset_t old_sig;
    
    sigemptyset (&new_sig);
    if (set != 0) {
      abort();	/* FIXME, we don't know how to translate old mask to new */
    }
    sigprocmask(SIG_SETMASK, &new_sig, &old_sig);
    return 1;	/* FIXME, we always return 1 as old value.  */
}
#endif
