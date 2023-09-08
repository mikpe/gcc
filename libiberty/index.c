/* Stub implementation of (obsolete) index(). */

/*

@deftypefn Supplemental char* index (char *@var{s}, int @var{c})

Returns a pointer to the first occurrence of the character @var{c} in
the string @var{s}, or @code{NULL} if not found.  The use of @code{index} is
deprecated in new programs in favor of @code{strchr}.

@end deftypefn

*/

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id: index.c f3ded7179433 2007/04/26 21:47:09 Martin Chaney <chaney@xkl.com> $";
# undef ENABLE_SVNID_TAG
#endif

extern char * strchr(const char *, int);

char *
index (const char *s, int c)
{
  return strchr (s, c);
}
