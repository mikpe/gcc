#include "version.h"

#ifdef ENABLE_SVNID_TAG
# ifdef __GNUC__
#  define _unused_ __attribute__((unused))
# else
#  define _unused_  /* define for other platforms here */
# endif
  static char const *SVNID _unused_ = "$Id$";
# undef ENABLE_SVNID_TAG
#endif

#include "versionid.h"
#ifndef VERSUFFIX
#define VERSUFFIX ".99999 (XKL 4.3.0 experimental)"
#endif

/* This is the location of the online document giving instructions for
   reporting bugs.  If you distribute a modified version of GCC,
   please configure with --with-bugurl pointing to a document giving
   instructions for reporting bugs to you, not us.  (You are of course
   welcome to forward us bugs reported to you, if you determine that
   they are not bugs in your modifications.)  */

#ifndef BUGURL
#define BUGURL "<URL:http://source1.xkl.com/bugzilla>"
#endif
const char bug_report_url[] = BUGURL;

/* The complete version string, assembled from several pieces.
   BASEVER, DATESTAMP, DEVPHASE, and REVISION are defined by the
   Makefile.  */

const char version_string[] = BASEVER VERSUFFIX REVISION DATESTAMP DEVPHASE;
const char pkgversion_string[] = PKGVERSION;

