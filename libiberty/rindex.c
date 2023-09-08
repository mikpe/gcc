/* Stub implementation of (obsolete) rindex(). */

/*

@deftypefn Supplemental char* rindex (const char *@var{s}, int @var{c})

Returns a pointer to the last occurrence of the character @var{c} in
the string @var{s}, or @code{NULL} if not found.  The use of @code{rindex} is
deprecated in new programs in favor of @code{strrchr}.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: rindex.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

extern char *strrchr (const char *, int);

char *
rindex (const char *s, int c)
{
  return strrchr (s, c);
}
