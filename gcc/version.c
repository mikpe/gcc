#include "version.h"
#include "../ccs_version.h"

/* This is the trailing component of the string reported as the
   version number by all components of the compiler.  For an official
   FSF release, it is empty.  If you distribute a modified version of
   GCC, please change this string to indicate that.  The suggested
   format is a leading space, followed by your organization's name
   in parentheses.  You may also wish to include a number indicating
   the revision of your modified compiler.  */

#ifndef CCS_BRANCH0_VN
#define CCS_BRANCH0_VN 0
#endif

#ifndef CCS_STEP0_VN
#define CCS_STEP0_VN   0
#endif

#define LIT(S)	#S
#define STR(N)	LIT(N)

#define VERSUFFIX " (IMG-" STR (CCS_MAJOR_VN)   "." \
                           STR (CCS_MINOR_VN)   "." \
                           STR (CCS_RELEASE_VN) "." \
                           STR (CCS_BUILD_VN)       \
                           STR (CCS_BRANCH0_VN)     \
                           STR (CCS_STEP0_VN)   ")"

/* This is the location of the online document giving instructions for
   reporting bugs.  If you distribute a modified version of GCC,
   please change this to refer to a document giving instructions for
   reporting bugs to you, not us.  (You are of course welcome to
   forward us bugs reported to you, if you determine that they are
   not bugs in your modifications.)  */

const char bug_report_url[] = "toolkit@metagence.com";

/* The complete version string, assembled from several pieces.
   BASEVER, DATESTAMP, and DEVPHASE are defined by the Makefile.  */

const char version_string[] = BASEVER DATESTAMP DEVPHASE VERSUFFIX;
