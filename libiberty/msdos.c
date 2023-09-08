#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: msdos.c 701a9ad92cef 2006/03/01 22:15:58 Mike Brady <mbrady@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

char msg[] = "No vfork available - aborting\n";
vfork()
{
  write(1, msg, sizeof(msg));
}

sigsetmask()
{
  /* no signals support in go32 (yet) */
}

waitpid()
{
  return -1;
}
